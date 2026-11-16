#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <algorithm>

namespace ig = ImGui;

// --- SAMPLE DATA MODEL FOR INSTANSCHOLA ---
struct StudentRecord {
    int id;
    char regNo[32];
    char fullName[64];
    int formLevel;      // 1 to 6
    float mathScore;
    float physicsScore;
    float englishScore;
    bool presentToday;
};

// Application state shared across frames (required for browser main loop)
struct AppState {
    std::vector<StudentRecord> students;
    bool showOfficialDemo = true;
    bool showMetricsWindow = false;
    bool shouldExit = false;
    char searchFilter[64] = "";
    int selectedStudentIdx = 0;

    // New Student Modal Form State
    char newReg[32] = "S.0101/0007";
    char newName[64] = "";
    int newForm = 4;
    float newMath = 70.0f, newPhys = 70.0f, newEng = 70.0f;
};

// Helper: Calculate NECTA-style Grade & Points from Average
static const char* CalculateNectaGrade(float avg, int& outPoints) {
    if (avg >= 75.0f) { outPoints = 1; return "A"; }
    if (avg >= 65.0f) { outPoints = 2; return "B"; }
    if (avg >= 45.0f) { outPoints = 3; return "C"; }
    if (avg >= 30.0f) { outPoints = 4; return "D"; }
    outPoints = 5;
    return "F";
}

// Single Frame Render & Update Callback
static void UpdateDrawFrame(void* arg) {
    AppState& app = *static_cast<AppState*>(arg);

    BeginDrawing();
    ClearBackground(Color{ 24, 26, 32, 255 });

    rlImGuiBegin();

    // ====================================================================
    // A. TOP MAIN MENU BAR
    // ====================================================================
    if (ig::BeginMainMenuBar()) {
        if (ig::BeginMenu("InstanSchola")) {
            if (ig::MenuItem("Export NECTA CSV", "Ctrl+E")) { /* Hook export here */ }
            ig::Separator();
            if (ig::MenuItem("Exit", "Alt+F4")) { app.shouldExit = true; }
            ig::EndMenu();
        }
        if (ig::BeginMenu("Developer Tools")) {
            ig::MenuItem("Show Official ImGui Feature Demo", nullptr, &app.showOfficialDemo);
            ig::MenuItem("Show ImGui Metrics / FPS Profiler", nullptr, &app.showMetricsWindow);
            ig::EndMenu();
        }
        ig::EndMainMenuBar();
    }

    // ====================================================================
    // B. INSTANSCHOLA ACADEMIC ERP SANDBOX WINDOW
    // ====================================================================
    ig::SetNextWindowPos(ImVec2(20, 40), ImGuiCond_FirstUseEver);
    ig::SetNextWindowSize(ImVec2(880, 780), ImGuiCond_FirstUseEver);

    if (ig::Begin("InstanSchola - NECTA Academic & Attendance Module")) {

        // --- KPI SUMMARY BAR ---
        int presentCount = 0;
        float totalClassAvg = 0.0f;
        for (const auto& s : app.students) {
            if (s.presentToday) presentCount++;
            totalClassAvg += (s.mathScore + s.physicsScore + s.englishScore) / 3.0f;
        }
        float attendanceRatio = app.students.empty() ? 0.0f : (float)presentCount / app.students.size();
        float classMean = app.students.empty() ? 0.0f : totalClassAvg / app.students.size();

        ig::TextColored(ImVec4(0.4f, 0.85f, 1.0f, 1.0f), "TERM II ACADEMIC OVERVIEW (FORM 4 SCIENCE)");
        ig::ProgressBar(attendanceRatio, ImVec2(280, 0), TextFormat("Attendance: %d / %d Present", presentCount, (int)app.students.size()));
        ig::SameLine();
        ig::Text("  |  Class Mean Score: %.1f%%", classMean);
        ig::Separator();
        ig::Spacing();

        // --- TAB BAR ---
        if (ig::BeginTabBar("MainModulesTabBar")) {

            // ------------------------------------------------------------
            // TAB 1: INTERACTIVE NECTA GRADING & ATTENDANCE TABLE
            // ------------------------------------------------------------
            if (ig::BeginTabItem("Student Roster & Grading Table")) {

                ig::SetNextItemWidth(260);
                ig::InputTextWithHint("##Search", "Filter by name or Reg No...", app.searchFilter, IM_ARRAYSIZE(app.searchFilter));
                ig::SameLine();
                if (ig::Button("+ Enroll Student")) {
                    ig::OpenPopup("EnrollStudentModal");
                }

                // --- MODAL POPUP FOR ADDING A STUDENT ---
                if (ig::BeginPopupModal("EnrollStudentModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                    ig::Text("Enter New Student Details:");
                    ig::Separator();
                    ig::InputText("NECTA Reg No", app.newReg, IM_ARRAYSIZE(app.newReg));
                    ig::InputText("Full Name", app.newName, IM_ARRAYSIZE(app.newName));
                    ig::SliderInt("Form Level", &app.newForm, 1, 6);
                    ig::DragFloat("Math Score", &app.newMath, 0.5f, 0.0f, 100.0f, "%.1f");
                    ig::DragFloat("Physics Score", &app.newPhys, 0.5f, 0.0f, 100.0f, "%.1f");
                    ig::DragFloat("English Score", &app.newEng, 0.5f, 0.0f, 100.0f, "%.1f");

                    ig::Spacing();
                    if (ig::Button("Save Record", ImVec2(120, 0))) {
                        if (strlen(app.newName) > 0) {
                            StudentRecord rec{};
                            rec.id = (int)app.students.size() + 1;
                            snprintf(rec.regNo, sizeof(rec.regNo), "%s", app.newReg);
                            snprintf(rec.fullName, sizeof(rec.fullName), "%s", app.newName);
                            rec.formLevel = app.newForm;
                            rec.mathScore = app.newMath;
                            rec.physicsScore = app.newPhys;
                            rec.englishScore = app.newEng;
                            rec.presentToday = true;
                            app.students.push_back(rec);
                            app.newName[0] = '\0';
                            ig::CloseCurrentPopup();
                        }
                    }
                    ig::SameLine();
                    if (ig::Button("Cancel", ImVec2(120, 0))) {
                        ig::CloseCurrentPopup();
                    }
                    ig::EndPopup();
                }

                ig::Spacing();

                // --- FULL-FEATURED DEAR IMGUI TABLE ---
                ImGuiTableFlags tableFlags =
                    ImGuiTableFlags_Borders |
                    ImGuiTableFlags_RowBg |
                    ImGuiTableFlags_Resizable |
                    ImGuiTableFlags_Reorderable |
                    ImGuiTableFlags_Hideable |
                    ImGuiTableFlags_ScrollY;

                if (ig::BeginTable("NectaRosterTable", 8, tableFlags, ImVec2(0.0f, 340.0f))) {
                    ig::TableSetupScrollFreeze(0, 1);
                    ig::TableSetupColumn("Reg No", ImGuiTableColumnFlags_WidthFixed, 105.0f);
                    ig::TableSetupColumn("Full Name", ImGuiTableColumnFlags_WidthStretch);
                    ig::TableSetupColumn("Present", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                    ig::TableSetupColumn("Math", ImGuiTableColumnFlags_WidthFixed, 65.0f);
                    ig::TableSetupColumn("Physics", ImGuiTableColumnFlags_WidthFixed, 65.0f);
                    ig::TableSetupColumn("English", ImGuiTableColumnFlags_WidthFixed, 65.0f);
                    ig::TableSetupColumn("Average", ImGuiTableColumnFlags_WidthFixed, 65.0f);
                    ig::TableSetupColumn("Grade", ImGuiTableColumnFlags_WidthFixed, 55.0f);
                    ig::TableHeadersRow();

                    for (int i = 0; i < (int)app.students.size(); i++) {
                        StudentRecord& s = app.students[i];

                        if (app.searchFilter[0] != '\0') {
                            if (strstr(s.fullName, app.searchFilter) == nullptr &&
                                strstr(s.regNo, app.searchFilter) == nullptr) {
                                continue;
                            }
                        }

                        ig::PushID(s.id);
                        ig::TableNextRow();

                        ig::TableSetColumnIndex(0);
                        bool isSelected = (app.selectedStudentIdx == i);
                        if (ig::Selectable(s.regNo, isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap)) {
                            app.selectedStudentIdx = i;
                        }

                        ig::TableSetColumnIndex(1);
                        ig::TextUnformatted(s.fullName);

                        ig::TableSetColumnIndex(2);
                        ig::Checkbox("##att", &s.presentToday);

                        ig::TableSetColumnIndex(3);
                        ig::SetNextItemWidth(-FLT_MIN);
                        ig::DragFloat("##math", &s.mathScore, 0.5f, 0.0f, 100.0f, "%.1f");

                        ig::TableSetColumnIndex(4);
                        ig::SetNextItemWidth(-FLT_MIN);
                        ig::DragFloat("##phys", &s.physicsScore, 0.5f, 0.0f, 100.0f, "%.1f");

                        ig::TableSetColumnIndex(5);
                        ig::SetNextItemWidth(-FLT_MIN);
                        ig::DragFloat("##eng", &s.englishScore, 0.5f, 0.0f, 100.0f, "%.1f");

                        float avg = (s.mathScore + s.physicsScore + s.englishScore) / 3.0f;
                        ig::TableSetColumnIndex(6);
                        ig::Text("%.1f%%", avg);

                        int pts = 0;
                        const char* grade = CalculateNectaGrade(avg, pts);
                        ig::TableSetColumnIndex(7);
                        ImVec4 gradeColor = (pts <= 2) ? ImVec4(0.2f, 0.9f, 0.3f, 1.0f) :
                            (pts == 3) ? ImVec4(1.0f, 0.8f, 0.2f, 1.0f) :
                            ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
                        ig::TextColored(gradeColor, "%s (%d)", grade, pts);

                        ig::PopID();
                    }
                    ig::EndTable();
                }

                // --- INSPECTOR CARD FOR SELECTED STUDENT ---
                if (app.selectedStudentIdx >= 0 && app.selectedStudentIdx < (int)app.students.size()) {
                    StudentRecord& sel = app.students[app.selectedStudentIdx];
                    ig::Spacing();
                    ig::BeginChild("StudentInspector", ImVec2(0, 150), true);
                    ig::TextColored(ImVec4(0.4f, 0.85f, 1.0f, 1.0f), "SELECTED STUDENT INSPECTOR: %s (%s)", sel.fullName, sel.regNo);
                    ig::Separator();
                    ig::InputText("Edit Name", sel.fullName, IM_ARRAYSIZE(sel.fullName));
                    ig::SliderFloat("Adjust Math", &sel.mathScore, 0.0f, 100.0f);
                    ig::Checkbox("Marked Present in Today's Session", &sel.presentToday);
                    ig::EndChild();
                }

                ig::EndTabItem();
            }

            // ------------------------------------------------------------
            // TAB 2: EXTRA WIDGETS PLAYGROUND
            // ------------------------------------------------------------
            if (ig::BeginTabItem("Widgets & Analytics Playground")) {
                static int selectedTerm = 1;
                const char* terms[] = { "Term I (Jan - May)", "Term II (Jul - Nov)", "Mock National Exam" };
                ig::Combo("Academic Term", &selectedTerm, terms, IM_ARRAYSIZE(terms));

                static float scoreHistory[] = { 62.0f, 65.5f, 68.0f, 71.2f, 74.8f, 79.1f };
                ig::PlotLines("School Mean Trend", scoreHistory, IM_ARRAYSIZE(scoreHistory), 0, "2021 - 2026", 40.0f, 100.0f, ImVec2(0, 80));

                if (ig::TreeNode("NECTA Grading Scale Reference")) {
                    ig::BulletText("A : 75 - 100 (1 Point - Distinction)");
                    ig::BulletText("B : 65 - 74  (2 Points - Merit)");
                    ig::BulletText("C : 45 - 64  (3 Points - Credit)");
                    ig::BulletText("D : 30 - 44  (4 Points - Pass)");
                    ig::BulletText("F : 0  - 29  (5 Points - Fail)");
                    ig::TreePop();
                }

                static float accentColor[3] = { 0.20f, 0.55f, 0.85f };
                if (ig::ColorEdit3("School Theme Accent", accentColor)) {
                    ig::GetStyle().Colors[ImGuiCol_HeaderActive] = ImVec4(accentColor[0], accentColor[1], accentColor[2], 1.0f);
                }

                ig::EndTabItem();
            }

            ig::EndTabBar();
        }
    }
    ig::End();

    // ====================================================================
    // C. OFFICIAL DEAR IMGUI BUILT-IN DEMO WINDOW
    // ====================================================================
    if (app.showOfficialDemo) {
        ig::ShowDemoWindow(&app.showOfficialDemo);
    }

    if (app.showMetricsWindow) {
        ig::ShowMetricsWindow(&app.showMetricsWindow);
    }

    rlImGuiEnd();
    EndDrawing();
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(1440, 860, "InstanSchola ERP - Dear ImGui v1.92 Showcase");

    rlImGuiSetup(true);

    ImGuiIO& io = ig::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    AppState app;
    app.students = {
        { 1, "S.0101/0001", "Amina Juma Hassan",      4, 84.0f, 78.5f, 88.0f, true },
        { 2, "S.0101/0002", "Baraka Joseph Mwamba",   4, 68.0f, 71.0f, 64.5f, true },
        { 3, "S.0101/0003", "Neema Godfrey Kimaro",   4, 91.5f, 89.0f, 85.0f, true },
        { 4, "S.0101/0004", "Tariq Salim Mushi",      4, 49.0f, 44.0f, 55.0f, false },
        { 5, "S.0101/0005", "Grace Emmanuel Nyerere", 4, 34.0f, 29.5f, 41.0f, true },
        { 6, "S.0101/0006", "Kelvin Peter Massawe",   4, 76.0f, 81.0f, 72.0f, true },
    };

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg(UpdateDrawFrame, &app, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose() && !app.shouldExit) {
        UpdateDrawFrame(&app);
    }
#endif

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}