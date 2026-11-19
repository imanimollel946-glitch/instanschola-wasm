#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include <string>

enum class Screen { Login, JoinSchool, CreateProfile, Dashboard };

int main() {
    InitWindow(1280, 720, "InstanSchola");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    Screen screen = Screen::Login;
    char user[64]{}, pass[64]{};
    std::string err;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground({ 28, 32, 42, 255 });
        rlImGuiBegin();

        if (screen == Screen::Login) {
            ImVec2 size(360, 230);
            ImGui::SetNextWindowPos({ (GetScreenWidth() - size.x) * 0.5f, (GetScreenHeight() - size.y) * 0.5f });
            ImGui::SetNextWindowSize(size);

            if (ImGui::Begin("Login", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
                ImGui::InputText("User", user, sizeof(user));
                ImGui::InputText("Pass", pass, sizeof(pass), ImGuiInputTextFlags_Password);

                if (ImGui::Button("Log In", { -1, 32 })) {
                    if (!user[0] || !pass[0]) err = "Enter user and password.";
                    else { err.clear(); screen = Screen::Dashboard; }
                }
                if (!err.empty()) ImGui::TextColored({ 1, 0.3f, 0.3f, 1 }, "%s", err.c_str());

                ImGui::Separator();
                if (ImGui::SmallButton("Join School")) screen = Screen::JoinSchool;
                ImGui::SameLine();
                if (ImGui::SmallButton("Create Profile")) screen = Screen::CreateProfile;
            }
            ImGui::End();
        }
        else {
            ImGui::Begin("Screen");
            ImGui::Text("Screen switched: %d", static_cast<int>(screen));
            if (ImGui::Button("Back")) screen = Screen::Login;
            ImGui::End();
        }

        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}