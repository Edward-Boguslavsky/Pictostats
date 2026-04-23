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
#include "AddModal.h"

bool editMode    = true;
bool showAddModal = false;

int main()
{
    HardwareBackend::Init();

    Application app("Pictostats");
    if (!app.Init()) return 1;

    Theme::SetupModernTheme();
    ImGuiIO& io = ImGui::GetIO();

    // WIDE (Titles & Headers)
    Theme::fontTitleWide    = io.Fonts->AddFontFromFileTTF("fonts/Blinker-ExtraBold.ttf", 48.0f * Theme::GlobalScale);
    Theme::fontSubtitleWide = io.Fonts->AddFontFromFileTTF("fonts/Blinker-SemiBold.ttf", 40.0f * Theme::GlobalScale);
    Theme::fontTextWide     = io.Fonts->AddFontFromFileTTF("fonts/Blinker-Light.ttf", 32.0f * Theme::GlobalScale);

    // NUM (Graphs & Data)
    Theme::fontTitleNum     = io.Fonts->AddFontFromFileTTF("fonts/PPFraktionSans-Bold.otf", 44.0f * Theme::GlobalScale);
    Theme::fontSubtitleNum  = io.Fonts->AddFontFromFileTTF("fonts/PPFraktionSans-Bold.otf", 36.0f * Theme::GlobalScale);
    Theme::fontTextNum      = io.Fonts->AddFontFromFileTTF("fonts/PPFraktionSans-Bold.otf", 28.0f * Theme::GlobalScale);

    // DOT (Stylized variants for future use)
    Theme::fontTitleDot     = io.Fonts->AddFontFromFileTTF("fonts/Doto_Rounded-Bold.ttf", 48.0f * Theme::GlobalScale);
    Theme::fontSubtitleDot  = io.Fonts->AddFontFromFileTTF("fonts/Doto_Rounded-Black.ttf", 42.0f * Theme::GlobalScale);
    Theme::fontTextDot      = io.Fonts->AddFontFromFileTTF("fonts/Doto_Rounded-Black.ttf", 36.0f * Theme::GlobalScale);

    ImGui::GetStyle().ScaleAllSizes(Theme::GlobalScale);

    std::vector<std::unique_ptr<Panel>> myPanels;

    myPanels.push_back(std::make_unique<SensorPanel>("Panel_CPU", "CPU", "AMD Ryzen 9 9900X3D", Theme::AccentRed, STYLE_ARC, 8, std::vector<SensorConfig>{ {"/amdcpu/0/load/0", "USAGE", "%.0f%%", 0.0f, 100.0f}, {"/amdcpu/0/temperature/2", "TEMP", "%.0f\xC2\xB0", 20.0f, 95.0f} }));
    myPanels.push_back(std::make_unique<SensorPanel>("Panel_GPU", "", "NVIDIA GeForce RTX 5080", Theme::AccentBlue, STYLE_ARC, 8, std::vector<SensorConfig>{ {"/gpu-nvidia/0/load/0", "USAGE", "%.0f%%", 0.0f, 100.0f}, {"/gpu-nvidia/0/temperature/0", "TEMP", "%.0f\xC2\xB0", 20.0f, 90.0f} }));
    myPanels.push_back(std::make_unique<SensorPanel>("Panel_RAM", "RAM", "", Theme::AccentPurple, STYLE_BAR, 8, std::vector<SensorConfig>{ {"/virtual/ram/used", "USAGE", "%.0f GB", 0.0f, 64.0f} }));
    myPanels.push_back(std::make_unique<SensorPanel>("Panel_PWR", "", "", Theme::AccentYellow, STYLE_BAR, 8, std::vector<SensorConfig>{ {"/virtual/sys/power", "DRAW", "%.0f W", 0.0f, 1000.0f} }));

    AddModal addModal;

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
            ImU32 gridColor = Theme::ToU32(Theme::TextDark);
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

        if (ImGui::BeginPopupContextWindow("ContextMenu", ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem("Add Panel")) showAddModal = true;
            ImGui::Separator();
            if (ImGui::MenuItem(editMode ? "Lock Layout" : "Edit Layout")) editMode = !editMode;
            ImGui::Separator();
            if (ImGui::MenuItem("Exit Dashboard")) app.Close();
            ImGui::EndPopup();
        }

        ImGui::End();
        ImGui::PopStyleColor(2);

        ImGuiWindowFlags panelFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        if (!editMode) panelFlags |= ImGuiWindowFlags_NoMove;

        for (const auto& panel : myPanels) {
            if (editMode) {
                ImGui::PushStyleColor(ImGuiCol_Border, Theme::TextMedium);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0,0,0,0));
            }

            ImGui::Begin(panel->GetWindowID().c_str(), nullptr, panelFlags);

            if (editMode && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                ImVec2 currentPos = ImGui::GetWindowPos();
                float snappedX = centerX + (std::round((currentPos.x - centerX) / snapGrid) * snapGrid);
                float snappedY = centerY + (std::round((currentPos.y - centerY) / snapGrid) * snapGrid);
                if (currentPos.x != snappedX || currentPos.y != snappedY)
                    ImGui::SetWindowPos(ImVec2(snappedX, snappedY));
            }

            panel->Draw();

            if (editMode) panel->DrawDeleteButton();

            ImGui::End();
            ImGui::PopStyleColor();
        }

        myPanels.erase(std::remove_if(myPanels.begin(), myPanels.end(),[](const std::unique_ptr<Panel>& p) { return p->IsPendingDelete(); }), myPanels.end());

        if (showAddModal) {
            addModal.Render(showAddModal, app.GetWidth(), app.GetHeight(), myPanels);
        }
    });

    HardwareBackend::Shutdown();
    return 0;
}