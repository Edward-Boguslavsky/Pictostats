#define NOMINMAX
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <windows.h>
#include "imgui.h"
#include "imgui_internal.h"

#include "Application.h"
#include "HardwareBackend.h"
#include "Theme.h"
#include "Panels.h"

bool editMode = true;

int main()
{
    HardwareBackend::Init();

    Application app("Pictostats");
    if (!app.Init()) return 1;

    Theme::SetupModernTheme();
    ImGuiIO& io = ImGui::GetIO();

    Theme::fontHugeTitle = io.Fonts->AddFontFromFileTTF("fonts/Blinker-ExtraBold.ttf", 48.0f * Theme::GlobalScale);
    Theme::fontTitle     = io.Fonts->AddFontFromFileTTF("fonts/PPFraktionSans-Bold.otf", 28.0f * Theme::GlobalScale);
    Theme::fontValue     = io.Fonts->AddFontFromFileTTF("fonts/PPFraktionSans-Light.otf", 48.0f * Theme::GlobalScale);
    Theme::fontLabel     = io.Fonts->AddFontFromFileTTF("fonts/PPFraktionSans-Bold.otf", 28.0f * Theme::GlobalScale);
    ImGui::GetStyle().ScaleAllSizes(Theme::GlobalScale);

    std::vector<std::unique_ptr<Panel>> myPanels;

    myPanels.push_back(std::make_unique<CpuGpuPanel>("Panel_CPU", "CPU", "AMD Ryzen 9 9900X3D", Theme::AccentRed, STYLE_ARC, 8, "/amdcpu/0/load/0", "/amdcpu/0/temperature/2"));
    myPanels.push_back(std::make_unique<CpuGpuPanel>("Panel_GPU", "GPU", "NVIDIA GeForce RTX 5080", Theme::AccentBlue, STYLE_ARC, 8, "/gpu-nvidia/0/load/0", "/gpu-nvidia/0/temperature/0"));
    myPanels.push_back(std::make_unique<RamPanel>("Panel_RAM", "RAM", "Team Group Inc - UD5-6400", Theme::AccentPurple, STYLE_BAR, 8));
    myPanels.push_back(std::make_unique<PowerPanel>("Panel_PWR", "PSU", "Corsair SF1000", Theme::AccentYellow, STYLE_BAR, 8));

    app.Run([&]() {

        if (GetForegroundWindow() != FindWindowW(L"PictostatsClass", nullptr)) {
            MsgWaitForMultipleObjects(0, NULL, FALSE, 250, QS_ALLINPUT);
        }

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)app.GetWidth(), (float)app.GetHeight()));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0,0,0,0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0,0,0,0));
        ImGui::Begin("Background Canvas", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);

        float snapGrid = Theme::BaseUnit * Theme::GlobalScale;
        float centerX = app.GetWidth() / 2.0f;
        float centerY = app.GetHeight() / 2.0f;

        if (editMode) {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImU32 gridColor = ImGui::ColorConvertFloat4ToU32(Theme::TextDark);

            float crossSize = 10.0f * Theme::GlobalScale;

            int xCount = std::ceil(centerX / snapGrid);
            int yCount = std::ceil(centerY / snapGrid);

            for (int i = -xCount; i <= xCount; ++i) {
                for (int j = -yCount; j <= yCount; ++j) {
                    float cx = centerX + (i * snapGrid);
                    float cy = centerY + (j * snapGrid);
                    draw_list->AddLine(ImVec2(cx - crossSize, cy), ImVec2(cx + crossSize, cy), gridColor, 1.0f);
                    draw_list->AddLine(ImVec2(cx, cy - crossSize), ImVec2(cx, cy + crossSize), gridColor, 1.0f);
                }
            }
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 16.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 12.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 12.0f);

        if (ImGui::BeginPopupContextWindow("ContextMenu", ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem(editMode ? "Lock Layout" : "Edit Layout")) editMode = !editMode;
            ImGui::Separator();
            if (ImGui::MenuItem("Exit Dashboard")) app.Close();
            ImGui::EndPopup();
        }

        ImGui::PopStyleVar(3);
        ImGui::End();
        ImGui::PopStyleColor(2);

        ImGuiWindowFlags panelFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        if (!editMode) panelFlags |= ImGuiWindowFlags_NoMove;

        for (const auto& panel : myPanels) {

            if (editMode) {
                ImGui::PushStyleColor(ImGuiCol_Border, ImGui::ColorConvertU32ToFloat4(Theme::TextMuted));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0,0,0,0));
            }

            ImGui::Begin(panel->GetWindowID().c_str(), nullptr, panelFlags);

            if (editMode && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                ImVec2 currentPos = ImGui::GetWindowPos();

                float relX = currentPos.x - centerX;
                float relY = currentPos.y - centerY;

                float snappedX = centerX + (std::round(relX / snapGrid) * snapGrid);
                float snappedY = centerY + (std::round(relY / snapGrid) * snapGrid);

                if (currentPos.x != snappedX || currentPos.y != snappedY) {
                    ImGui::SetWindowPos(ImVec2(snappedX, snappedY));
                }
            }

            panel->Draw();
            ImGui::End();

            ImGui::PopStyleColor();
        }
    });

    HardwareBackend::Shutdown();
    return 0;
}