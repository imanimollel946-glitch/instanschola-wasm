#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <algorithm>

// Create a clean 2-letter alias so we don't collide with Raylib's Text() or Image()
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

// Helper: Calculate NECTA-style Grade & Points from Average
static const char* CalculateNectaGrade(float avg, int& outPoints) {
    if (avg >= 75.0f) { outPoints = 1; return "A"; }
    if (avg >= 65.0f) { outPoints = 2; return "B"; }
    if (avg >= 45.0f) { outPoints = 3; return "C"; }
    if (avg >= 30.0f) { outPoints = 4; return "D"; }
    outPoints = 5;
    return "F";
}

int main() {
    // 1. Initialize Raylib 6.0 Window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(1440, 860, "InstanSchola ERP - Dear ImGui v1.92 Showcase");
    SetTargetFPS(60);

    // 2. Initialize rlImGui (true = Dark Theme)
    rlImGuiSetup(true);

    // Enable Keyboard Navigation & Docking (if supported by your ImGui config)
    ImGuiIO& io = ig::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // 3. Seed Sample Student Data
    std::vector<StudentRecord> students = {
        { 1, "S.0101/0001", "Amina Juma Hassan",     4, 84.0f, 78.5f, 88.0f, true  },
        { 2, "S.0101/0002", "Baraka Joseph Mwamba",  4, 68.0f, 71.0f, 64.5f, true  },
        { 3, "S.0101/0003", "Neema Godfrey Kimaro",  4, 91.5f, 89.0f, 85.0f, true  },
        { 4, "S.0101/0004", "Tariq Salim Mushi",     4, 49.0f, 44.0f, 55.0f, false },
        { 5, "S.0101/0005", "Grace Emmanuel Nyerere",4, 34.0f, 29.5f, 41.0f, true  },
        { 6, "S.0101/0006", "Kelvin Peter Massawe",  4, 76.0f, 81.0f, 72.0f, true  },
    };

    // UI State Variables
    bool showOfficialDemo = true;   // Toggles ImGui's full built-in widget encyclopaedia
    bool showMetricsWindow = false; // Toggles FPS & draw-call profiler
    char searchFilter[64] = "";
    int selectedStudentIdx = 0;

    // New Student Modal Form State
    char newReg[32] = "S.0101/0007";
    char newName[64] = "";
    int newForm = 4;
    float newMath = 70.0f, newPhys = 70.0f, newEng = 70.0f;

    // 4. Main Application Loop
    while (!WindowShouldClose()) {
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
                if (ig::MenuItem("Exit", "Alt+F4")) { break; }
                ig::EndMenu();
            }
            if (ig::BeginMenu("Developer Tools")) {
                ig::MenuItem("Show Official ImGui Feature Demo", nullptr, &showOfficialDemo);
                ig::MenuItem("Show ImGui Metrics / FPS Profiler", nullptr, &showMetricsWindow);
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

            // --- KPI SUMMARY BAR (Columns & Progress Bars) ---
            int presentCount = 0;
            float totalClassAvg = 0.0f;
            for (const auto& s : students) {
                if (s.presentToday) presentCount++;
                totalClassAvg += (s.mathScore + s.physicsScore + s.englishScore) / 3.0f;
            }
            float attendanceRatio = students.empty() ? 0.0f : (float)presentCount / students.size();
            float classMean = students.empty() ? 0.0f : totalClassAvg / students.size();

            ig::TextColored(ImVec4(0.4f, 0.85f, 1.0f, 1.0f), "TERM II ACADEMIC OVERVIEW (FORM 4 SCIENCE)");
            ig::ProgressBar(attendanceRatio, ImVec2(280, 0), TextFormat("Attendance: %d / %d Present", presentCount, (int)students.size()));
            ig::SameLine();
            ig::Text("  |  Class Mean Score: %.1f%%", classMean);
            ig::Separator();
            ig::Spacing();

            // --- TAB BAR (Organizing Modules) ---
            if (ig::BeginTabBar("MainModulesTabBar")) {

                // ------------------------------------------------------------
                // TAB 1: INTERACTIVE NECTA GRADING & ATTENDANCE TABLE
                // ------------------------------------------------------------
                if (ig::BeginTabItem("Student Roster & Grading Table")) {

                    // Search Bar + Add Student Button
                    ig::SetNextItemWidth(260);
                    ig::InputTextWithHint("##Search", "Filter by name or Reg No...", searchFilter, IM_ARRAYSIZE(searchFilter));
                    ig::SameLine();
                    if (ig::Button("+ Enroll Student")) {
                        ig::OpenPopup("EnrollStudentModal");
                    }

                    // --- MODAL POPUP FOR ADDING A STUDENT ---
                    if (ig::BeginPopupModal("EnrollStudentModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ig::Text("Enter New Student Details:");
                        ig::Separator();
                        ig::InputText("NECTA Reg No", newReg, IM_ARRAYSIZE(newReg));
                        ig::InputText("Full Name", newName, IM_ARRAYSIZE(newName));
                        ig::SliderInt("Form Level", &newForm, 1, 6);
                        ig::DragFloat("Math Score", &newMath, 0.5f, 0.0f, 100.0f, "%.1f");
                        ig::DragFloat("Physics Score", &newPhys, 0.5f, 0.0f, 100.0f, "%.1f");
                        ig::DragFloat("English Score", &newEng, 0.5f, 0.0f, 100.0f, "%.1f");

                        ig::Spacing();
                        if (ig::Button("Save Record", ImVec2(120, 0))) {
                            if (strlen(newName) > 0) {
                                StudentRecord rec{};
                                rec.id = (int)students.size() + 1;
                                strncpy_s(rec.regNo, newReg, sizeof(rec.regNo) - 1);
                                strncpy_s(rec.fullName, newName, sizeof(rec.fullName) - 1);
                                rec.formLevel = newForm;
                                rec.mathScore = newMath;
                                rec.physicsScore = newPhys;
                                rec.englishScore = newEng;
                                rec.presentToday = true;
                                students.push_back(rec);
                                newName[0] = '\0';
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
                        // Setup Headers (Freeze top header row when scrolling)
                        ig::TableSetupScrollFreeze(0, 1);
                        ig::TableSetupColumn("Reg No",     ImGuiTableColumnFlags_WidthFixed, 105.0f);
                        ig::TableSetupColumn("Full Name",  ImGuiTableColumnFlags_WidthStretch);
                        ig::TableSetupColumn("Present",    ImGuiTableColumnFlags_WidthFixed, 60.0f);
                        ig::TableSetupColumn("Math",       ImGuiTableColumnFlags_WidthFixed, 65.0f);
                        ig::TableSetupColumn("Physics",    ImGuiTableColumnFlags_WidthFixed, 65.0f);
                        ig::TableSetupColumn("English",    ImGuiTableColumnFlags_WidthFixed, 65.0f);
                        ig::TableSetupColumn("Average",    ImGuiTableColumnFlags_WidthFixed, 65.0f);
                        ig::TableSetupColumn("Grade",      ImGuiTableColumnFlags_WidthFixed, 55.0f);
                        ig::TableHeadersRow();

                        for (int i = 0; i < (int)students.size(); i++) {
                            StudentRecord& s = students[i];

                            // Apply live search filter
                            if (searchFilter[0] != '\0') {
                                if (strstr(s.fullName, searchFilter) == nullptr &&
                                    strstr(s.regNo, searchFilter) == nullptr) {
                                    continue;
                                }
                            }

                            ig::PushID(s.id); // Crucial in ImGui loops so widgets have unique IDs!
                            ig::TableNextRow();

                            // Col 0: Selectable Row
                            ig::TableSetColumnIndex(0);
                            bool isSelected = (selectedStudentIdx == i);
                            if (ig::Selectable(s.regNo, isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap)) {
                                selectedStudentIdx = i;
                            }

                            // Col 1: Name
                            ig::TableSetColumnIndex(1);
                            ig::TextUnformatted(s.fullName);

                            // Col 2: Attendance Checkbox
                            ig::TableSetColumnIndex(2);
                            ig::Checkbox("##att", &s.presentToday);

                            // Col 3-5: Inline Editable Marks (DragFloat acts like spreadsheet cells!)
                            ig::TableSetColumnIndex(3);
                            ig::SetNextItemWidth(-FLT_MIN);
                            ig::DragFloat("##math", &s.mathScore, 0.5f, 0.0f, 100.0f, "%.1f");

                            ig::TableSetColumnIndex(4);
                            ig::SetNextItemWidth(-FLT_MIN);
                            ig::DragFloat("##phys", &s.physicsScore, 0.5f, 0.0f, 100.0f, "%.1f");

                            ig::TableSetColumnIndex(5);
                            ig::SetNextItemWidth(-FLT_MIN);
                            ig::DragFloat("##eng", &s.englishScore, 0.5f, 0.0f, 100.0f, "%.1f");

                            // Col 6: Live Calculated Average
                            float avg = (s.mathScore + s.physicsScore + s.englishScore) / 3.0f;
                            ig::TableSetColumnIndex(6);
                            ig::Text("%.1f%%", avg);

                            // Col 7: Color-Coded NECTA Grade
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
                    if (selectedStudentIdx >= 0 && selectedStudentIdx < (int)students.size()) {
                        StudentRecord& sel = students[selectedStudentIdx];
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
                // TAB 2: EXTRA WIDGETS PLAYGROUND (Combos, Trees, Plots, Colors)
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
        // C. OFFICIAL DEAR IMGUI BUILT-IN DEMO WINDOW (EVERY WIDGET IN IMGUI)
        // ====================================================================
        if (showOfficialDemo) {
            ig::ShowDemoWindow(&showOfficialDemo);
        }

        if (showMetricsWindow) {
            ig::ShowMetricsWindow(&showMetricsWindow);
        }

        rlImGuiEnd();
        EndDrawing();
    }

    // 5. Clean Shutdown
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}