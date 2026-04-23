#include "AddModal.h"
#include "Theme.h"
#include "imgui_internal.h"
#include <string>
#include <algorithm>
#include <cctype>

struct SimpleSensorDef {
    const char* DisplayName;
    const char* Path;
    float MinValue;
    float MaxValue;
};

static const std::vector<SimpleSensorDef> s_SimpleSensors = {
    { "CPU Usage",       "/amdcpu/0/load/0",            0.0f, 100.0f },
    { "CPU Temperature", "/amdcpu/0/temperature/2",     20.0f, 95.0f },
    { "GPU Usage",       "/gpu-nvidia/0/load/0",        0.0f, 100.0f },
    { "GPU Temperature", "/gpu-nvidia/0/temperature/0", 20.0f, 90.0f },
    { "RAM Usage",       "/virtual/ram/used",           0.0f, 64.0f },
    { "NVME 1 Usage",    "/virtual/nvme/1/used",        0.0f, 2000.0f },
    { "NVME 2 Usage",    "/virtual/nvme/0/used",        0.0f, 500.0f },
    { "PSU Draw",        "/virtual/sys/power",          0.0f, 1000.0f }
};

// Helper macro to cleanly draw a small description above an input/combo field
#define DRAW_LABEL(text) \
    ImGui::PushFont(Theme::fontTextNum); \
    ImGui::TextColored(Theme::TextMedium, text); \
    ImGui::PopFont();

void AddModal::Render(bool& isOpen, int appWidth, int appHeight, std::vector<std::unique_ptr<Panel>>& panels) {
    if (!isOpen) return;

    if (!ImGui::IsPopupOpen("##AddOverlayPopup")) {
        ImGui::OpenPopup("##AddOverlayPopup");
        m_StyleIdx = 0; m_NumSensors = 1; m_TitleBuf[0] = '\0'; m_SubtitleBuf[0] = '\0';
        for(int i = 0; i < 3; i++) {
            m_PresetIdx[i] = 0;
            strncpy_s(m_LabelBuf[i], sizeof(m_LabelBuf[i]), "Usage", _TRUNCATE);
            strncpy_s(m_UnitBuf[i], sizeof(m_UnitBuf[i]), "%", _TRUNCATE);
        }
    }

    float pad = Theme::OverlayPadding * Theme::GlobalScale;
    ImGui::SetNextWindowPos(ImVec2(pad, pad), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(appWidth - pad * 2.0f, appHeight - pad * 2.0f), ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_Border, Theme::TextMedium);
    ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));

    if (ImGui::BeginPopupModal("##AddOverlayPopup", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {

        // --- TOP FIXED HEADER ROW ---
        ImGui::PushFont(Theme::fontTitleWide);
        ImGui::TextColored(Theme::TextDark, "ADD A PANEL");
        ImGui::PopFont();

        float btnW = 140.0f * Theme::GlobalScale;
        float btnH = 48.0f * Theme::GlobalScale;
        float spacing = 12.0f * Theme::GlobalScale;
        bool doAdd = false;

        ImGui::SameLine(ImGui::GetWindowWidth() - (btnW * 2) - spacing - ImGui::GetStyle().WindowPadding.x);

        ImGui::PushFont(Theme::fontTextWide);
        if (Theme::ButtonTertiary("CANCEL", ImVec2(btnW, btnH))) {
            isOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine(0, spacing);
        if (Theme::ButtonPrimary("ADD", ImVec2(btnW, btnH), Theme::AccentRed)) {
            doAdd = true;
        }
        ImGui::PopFont();

        ImGui::Dummy(ImVec2(0, 8.0f * Theme::GlobalScale));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 8.0f * Theme::GlobalScale));

        // --- SCROLLABLE CONTENT REGION ---
        ImGui::BeginChild("##ScrollRegion", ImVec2(0, 0), false);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.22f, 0.22f, 0.22f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, Theme::Vec4FromRGB(80, 80, 80)); // Faint borders for all fields
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16.0f * Theme::GlobalScale, 14.0f * Theme::GlobalScale));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f * Theme::GlobalScale);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); // Enable the faint borders

        // Tighten the Y spacing so the label text sits closer to the input box beneath it
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 6.0f * Theme::GlobalScale));

        ImGui::PushFont(Theme::fontTextWide); // Standard font for the text typed into the inputs

        ImGui::PushFont(Theme::fontSubtitleWide);
        ImGui::TextColored(Theme::TextMedium, "STEP 1: PANEL SETTINGS");
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(0, 4.0f * Theme::GlobalScale));

        DRAW_LABEL("STYLE");
        ImGui::SetNextItemWidth(400.0f * Theme::GlobalScale);
        const char* styles[] = { "Linear", "Arc" };
        if (Theme::ModernCombo("##StyleCombo", styles[m_StyleIdx])) {
            for (int i = 0; i < 2; i++) if (ImGui::Selectable(styles[i], m_StyleIdx == i)) m_StyleIdx = i;
            ImGui::EndCombo();
        }
        ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

        DRAW_LABEL("LAYOUT");
        ImGui::SetNextItemWidth(400.0f * Theme::GlobalScale);
        const char* counts[] = { "1 Widget", "2 Widgets", "3 Widgets" };
        if (Theme::ModernCombo("##LayoutCombo", counts[m_NumSensors - 1])) {
            for (int i = 0; i < 3; i++) if (ImGui::Selectable(counts[i], m_NumSensors == (i + 1))) m_NumSensors = i + 1;
            ImGui::EndCombo();
        }
        ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

        DRAW_LABEL("HUGE TITLE (OPTIONAL)");
        ImGui::SetNextItemWidth(400.0f * Theme::GlobalScale);
        ImGui::InputText("##HugeTitle", m_TitleBuf, sizeof(m_TitleBuf));
        ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

        DRAW_LABEL("SUBTITLE (OPTIONAL)");
        ImGui::SetNextItemWidth(400.0f * Theme::GlobalScale);
        ImGui::InputText("##Subtitle", m_SubtitleBuf, sizeof(m_SubtitleBuf));
        ImGui::Dummy(ImVec2(0, 24.0f * Theme::GlobalScale));

        // --- STEP 2: WIDGET TAB SYSTEM ---
        ImGui::PushFont(Theme::fontSubtitleWide);
        ImGui::TextColored(Theme::TextMedium, "STEP 2: WIDGET DATA");
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(0, 4.0f * Theme::GlobalScale));

        if (ImGui::BeginTabBar("##WidgetTabs")) {
            for (int i = 0; i < m_NumSensors; ++i) {
                std::string tabName = "Sensor " + std::to_string(i + 1);
                if (ImGui::BeginTabItem(tabName.c_str())) {

                    ImGui::PushID(i);
                    ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

                    DRAW_LABEL("HARDWARE SENSOR");
                    ImGui::SetNextItemWidth(400.0f * Theme::GlobalScale);
                    if (Theme::ModernCombo("##HardwareSensor", s_SimpleSensors[m_PresetIdx[i]].DisplayName)) {
                        for (int j = 0; j < s_SimpleSensors.size(); j++) {
                            if (ImGui::Selectable(s_SimpleSensors[j].DisplayName, m_PresetIdx[i] == j)) m_PresetIdx[i] = j;
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

                    DRAW_LABEL("LABEL");
                    ImGui::SetNextItemWidth(400.0f * Theme::GlobalScale);
                    ImGui::InputText("##Label", m_LabelBuf[i], sizeof(m_LabelBuf[i]));
                    ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

                    DRAW_LABEL("UNIT");
                    ImGui::SetNextItemWidth(400.0f * Theme::GlobalScale);
                    ImGui::InputText("##Unit", m_UnitBuf[i], sizeof(m_UnitBuf[i]));

                    ImGui::PopID();
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }

        ImGui::PopFont();
        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor(5);
        ImGui::EndChild();

        // --- EXECUTE ADD LOGIC ---
        if (doAdd) {
            PanelStyle parsedStyle = (m_StyleIdx == 0) ? STYLE_BAR : STYLE_ARC;
            std::string uniqueID = "Panel_" + std::to_string(m_IdCounter++);
            std::vector<SensorConfig> config;

            for (int i = 0; i < m_NumSensors; i++) {
                const auto& sensor = s_SimpleSensors[m_PresetIdx[i]];
                std::string finalLabel = m_LabelBuf[i];
                std::transform(finalLabel.begin(), finalLabel.end(), finalLabel.begin(), ::toupper);

                std::string unitStr = m_UnitBuf[i];
                std::string formatStr = "%.0f";
                if (!unitStr.empty()) {
                    if (unitStr == "%") formatStr += "%%";
                    else {
                        std::string escaped;
                        for (char c : unitStr) { if (c == '%') escaped += "%%"; else escaped += c; }
                        formatStr += " " + escaped;
                    }
                }
                config.push_back({ sensor.Path, finalLabel, formatStr, sensor.MinValue, sensor.MaxValue });
            }

            panels.push_back(std::make_unique<SensorPanel>(uniqueID, m_TitleBuf, m_SubtitleBuf, Theme::AccentRed, parsedStyle, 8, config));
            isOpen = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleColor(2);
}