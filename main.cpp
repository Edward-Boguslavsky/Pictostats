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

bool editMode    = true;
bool showAddPanel = false;

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
    myPanels.push_back(std::make_unique<StoragePanel>("Panel_NVMe1", "SSD", "Samsung 970 EVO Plus 2TB", Theme::AccentBlue, STYLE_BAR, 8, "/nvme/1"));
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
            if (ImGui::MenuItem("Add Panel")) showAddPanel = true;
            ImGui::Separator();
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
                if (currentPos.x != snappedX || currentPos.y != snappedY)
                    ImGui::SetWindowPos(ImVec2(snappedX, snappedY));
            }

            panel->Draw();

            if (editMode)
                panel->DrawDeleteButton();

            ImGui::End();
            ImGui::PopStyleColor();
        }

        myPanels.erase(
            std::remove_if(myPanels.begin(), myPanels.end(),
                [](const std::unique_ptr<Panel>& p) { return p->IsPendingDelete(); }),
            myPanels.end()
        );

        if (showAddPanel) {
            // Dim + input blocker — submitted first so it renders UNDER the overlay
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2((float)app.GetWidth(), (float)app.GetHeight()));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_Border,   ImVec4(0, 0, 0, 0));
            ImGui::Begin("##DimOverlay", nullptr,
                ImGuiWindowFlags_NoDecoration    |
                ImGuiWindowFlags_NoMove          |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoBringToFrontOnFocus);
            ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
            ImGui::End();
            ImGui::PopStyleColor(2);

            // Add Panel overlay — submitted second so it renders ON TOP
            float pad = Theme::OverlayPadding * Theme::GlobalScale;
            ImGui::SetNextWindowPos(ImVec2(pad, pad));
            ImGui::SetNextWindowSize(ImVec2(app.GetWidth() - pad * 2.0f, app.GetHeight() - pad * 2.0f));

            ImGui::PushStyleColor(ImGuiCol_WindowBg, Theme::PanelBackground);
            ImGui::PushStyleColor(ImGuiCol_Border,   ImGui::ColorConvertU32ToFloat4(Theme::TextMuted));
            ImGui::Begin("##AddPanelOverlay", nullptr,
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoMove       |
                ImGuiWindowFlags_NoSavedSettings);
            ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

            float btnSize = Theme::DeleteBtnSize    * Theme::GlobalScale;
            float btnPad  = Theme::DeleteBtnPadding * Theme::GlobalScale;
            ImVec2 wPos   = ImGui::GetWindowPos();
            ImVec2 wSize  = ImGui::GetWindowSize();
            ImVec2 btnMin(wPos.x + wSize.x - btnSize - btnPad, wPos.y + btnPad);
            ImVec2 btnMax(btnMin.x + btnSize, btnMin.y + btnSize);

            bool winHov = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
            bool btnHov = winHov && ImGui::IsMouseHoveringRect(btnMin, btnMax);
            bool btnClk = btnHov && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

            ImDrawList* dl = ImGui::GetWindowDrawList();
            if (btnHov) {
                dl->AddRectFilled(btnMin, btnMax,
                    btnClk ? IM_COL32(255, 50, 50, 128) : IM_COL32(255, 50, 50, 64),
                    4.0f * Theme::GlobalScale);
            }
            ImGui::PushFont(Theme::fontLabel);
            ImVec2 xSize = ImGui::CalcTextSize("X");
            dl->AddText(
                ImVec2(btnMin.x + (btnSize - xSize.x) * 0.5f, btnMin.y + (btnSize - xSize.y) * 0.5f),
                btnHov ? IM_COL32(255, 100, 100, 255) : Theme::TextMuted, "X"
            );
            ImGui::PopFont();

            if (btnClk) showAddPanel = false;

            // Content goes here

            ImGui::End();
            ImGui::PopStyleColor(2);
        }
    });

    HardwareBackend::Shutdown();
    return 0;
}