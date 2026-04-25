#include "AddModal.h"
#include "HardwareBackend.h"
#include "Theme.h"
#include "imgui_internal.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <cstring>

struct SimpleSensorDef {
    const char* Category;
    const char* DisplayName;
    const char* FullName;
    const char* Path;
    float MinValue;
    float MaxValue;
    const char* DefaultLabel;
    const char* DefaultUnit;
};

static const std::vector<SimpleSensorDef> s_SimpleSensors = {
    { "CPU",     "Usage",       "CPU Usage",       "/amdcpu/0/load/0",            0.0f, 100.0f,  "USAGE", "%" },
    { "CPU",     "Temperature", "CPU Temperature", "/amdcpu/0/temperature/2",     20.0f, 95.0f,  "TEMP",  "\xC2\xB0" },
    { "GPU",     "Usage",       "GPU Usage",       "/gpu-nvidia/0/load/0",        0.0f, 100.0f,  "USAGE", "%" },
    { "GPU",     "Temperature", "GPU Temperature", "/gpu-nvidia/0/temperature/0", 20.0f, 90.0f,  "TEMP",  "\xC2\xB0" },
    { "RAM",     "Usage",       "RAM Usage",       "/virtual/ram/used",           0.0f, 64.0f,   "USAGE", " GB" },
    { "NVME 1",  "Usage",       "NVME 1 Usage",    "/virtual/nvme/1/used",        0.0f, 2000.0f, "USED",  " GB" },
    { "NVME 2",  "Usage",       "NVME 2 Usage",    "/virtual/nvme/0/used",        0.0f, 500.0f,  "USED",  " GB" },
    { "PSU",     "Draw",        "PSU Draw",        "/virtual/sys/power",          0.0f, 1000.0f, "DRAW",  " W" }
};

#define DRAW_LABEL(text) \
    ImGui::PushFont(Theme::fontTextNum); \
    ImGui::TextColored(Theme::TextMedium, text); \
    ImGui::PopFont();

void AddModal::Render(bool& isOpen, int appWidth, int appHeight, std::vector<std::unique_ptr<Panel>>& panels) {
    if (!isOpen) return;

    if (!ImGui::IsPopupOpen("##AddOverlayPopup")) {
        ImGui::OpenPopup("##AddOverlayPopup");
        m_StyleIdx = 0; m_NumSensors = 1; m_SelectedColorIdx = 0;
        m_TitleBuf[0] = '\0'; m_SubtitleBuf[0] = '\0';

        for(int i = 0; i < 3; i++) {
            m_PresetIdx[i] = 0;
            strncpy_s(m_LabelBuf[i], sizeof(m_LabelBuf[i]), s_SimpleSensors[0].DefaultLabel, _TRUNCATE);
            strncpy_s(m_UnitBuf[i], sizeof(m_UnitBuf[i]), s_SimpleSensors[0].DefaultUnit, _TRUNCATE);
        }
    }

    float pad = Theme::OverlayPadding * Theme::GlobalScale;
    ImGui::SetNextWindowPos(ImVec2(pad, pad), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(appWidth - pad * 2.0f, appHeight - pad * 2.0f), ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_Border, Theme::TextMedium);
    ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));

    if (ImGui::BeginPopupModal("##AddOverlayPopup", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {

        // --- TOP FIXED HEADER ROW ---
        ImGui::PushFont(Theme::fontTitleDot);
        ImGui::TextColored(Theme::TextMedium, "ADD A PANEL");
        ImGui::PopFont();

        float btnW = Theme::ModalButtonWidth * Theme::GlobalScale;
        float btnH = Theme::ModalButtonHeight * Theme::GlobalScale;
        float spacing = Theme::ModalButtonSpacing * Theme::GlobalScale;
        bool doAdd = false;

        ImGui::SameLine(ImGui::GetWindowWidth() - (btnW * 2) - spacing - ImGui::GetStyle().WindowPadding.x);

        ImGui::PushFont(Theme::fontSubtitleNum);
        if (Theme::ButtonTertiary("CANCEL", ImVec2(btnW, btnH))) {
            isOpen = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine(0, spacing);
        if (Theme::ButtonPrimary("ADD", ImVec2(btnW, btnH), Theme::AccentRed)) {
            doAdd = true;
        }
        ImGui::PopFont();

        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 4.0f * Theme::GlobalScale));

        // --- SCROLLABLE CONTENT REGION ---
        ImGui::BeginChild("##ScrollRegion", ImVec2(0, 0), false);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.22f, 0.22f, 0.22f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, Theme::Vec4FromRGB(80, 80, 80));
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Theme::ModalPadding * Theme::GlobalScale, Theme::ModalPadding * Theme::GlobalScale));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Theme::ModalRounding * Theme::GlobalScale);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, Theme::TightItemSpacing * Theme::GlobalScale));

        ImGui::PushFont(Theme::fontTextWide);

        ImGui::PushFont(Theme::fontSubtitleDot);
        ImGui::TextColored(Theme::AccentRed, "PANEL SETTINGS");
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(0, 4.0f * Theme::GlobalScale));

        DRAW_LABEL(" STYLE");
        ImGui::SetNextItemWidth(Theme::ModalInputWidth * Theme::GlobalScale);
        const char* styles[] = { "Linear", "Arc" };
        if (Theme::ModernCombo("##StyleCombo", styles[m_StyleIdx])) {
            for (int i = 0; i < 2; i++) if (ImGui::Selectable(styles[i], m_StyleIdx == i)) m_StyleIdx = i;
            ImGui::EndCombo();
        }
        ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

        DRAW_LABEL(" LAYOUT");
        ImGui::SetNextItemWidth(Theme::ModalInputWidth * Theme::GlobalScale);
        const char* counts[] = { "1 Widget", "2 Widgets", "3 Widgets" };
        if (Theme::ModernCombo("##LayoutCombo", counts[m_NumSensors - 1])) {
            for (int i = 0; i < 3; i++) if (ImGui::Selectable(counts[i], m_NumSensors == (i + 1))) m_NumSensors = i + 1;
            ImGui::EndCombo();
        }
        ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

        DRAW_LABEL(" TITLE (OPTIONAL)");
        ImGui::SetNextItemWidth(Theme::ModalInputWidth * Theme::GlobalScale);
        ImGui::InputText("##HugeTitle", m_TitleBuf, sizeof(m_TitleBuf));
        ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

        DRAW_LABEL(" SUBTITLE (OPTIONAL)");

        // Slightly larger button for comfortable internal padding
        float autoBtnW = 120.0f * Theme::GlobalScale;
        float subInputW = (Theme::ModalInputWidth * Theme::GlobalScale) - autoBtnW - (Theme::TightItemSpacing * Theme::GlobalScale);

        ImGui::SetNextItemWidth(subInputW);
        ImGui::InputText("##Subtitle", m_SubtitleBuf, sizeof(m_SubtitleBuf));

        // Capture height BEFORE pushing the new font to prevent vertical layout stretching!
        float textBoxHeight = ImGui::GetFrameHeight();
        ImGui::SameLine();

        ImGui::PushFont(Theme::fontSubtitleNum);

        // Use TextLight so it inherits the crisp white border of a true secondary button
        if (Theme::ButtonSecondary("AUTO", ImVec2(autoBtnW, textBoxHeight), Theme::AccentRed)) {
            ImGui::OpenPopup("AutoSubtitlePopup");
        }
        ImGui::PopFont();

        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, Theme::TextMedium);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 8.0f * Theme::GlobalScale);
        if (ImGui::BeginPopup("AutoSubtitlePopup")) {
            ImGui::PushFont(Theme::fontTextWide);
            for (const auto& name : HardwareBackend::GetDetectedHardwareNames()) {
                if (ImGui::Selectable(name.c_str())) {
                    strncpy_s(m_SubtitleBuf, sizeof(m_SubtitleBuf), name.c_str(), _TRUNCATE);
                }
            }
            ImGui::PopFont();
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        DRAW_LABEL(" ACCENT COLOR");
        ImVec4 colors[6] = { Theme::AccentRed, Theme::AccentYellow, Theme::AccentGreen, Theme::AccentTeal, Theme::AccentBlue, Theme::AccentPurple };
        float pickerSize = ImGui::GetFrameHeight();
        for (int i = 0; i < 6; i++) {
            if (i > 0) ImGui::SameLine(0, 16.0f * Theme::GlobalScale);
            ImGui::PushID(i);
            if (Theme::ColorPickerButton("##color", ImVec2(pickerSize, pickerSize), colors[i], m_SelectedColorIdx == i)) {
                m_SelectedColorIdx = i;
            }
            ImGui::PopID();
        }

        ImGui::Dummy(ImVec2(0, 24.0f * Theme::GlobalScale));

        // --- STEP 2: WIDGET TAB SYSTEM ---
        ImGui::PushFont(Theme::fontSubtitleDot);
        ImGui::TextColored(Theme::AccentRed, "WIDGET SETTINGS");
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(0, 4.0f * Theme::GlobalScale));

        ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, Theme::ModalRounding * Theme::GlobalScale);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(32.0f * Theme::GlobalScale, 24.0f * Theme::GlobalScale));

        ImGui::PushStyleColor(ImGuiCol_Tab, Theme::TextDark);
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextLight);
        ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(Theme::AccentRed.x, Theme::AccentRed.y, Theme::AccentRed.z, 0.70f));
        ImGui::PushStyleColor(ImGuiCol_TabActive, Theme::AccentRed);

        ImGui::PushFont(Theme::fontTextNum);

        if (ImGui::BeginTabBar("##WidgetTabs")) {
            for (int i = 0; i < m_NumSensors; ++i) {
                std::string tabName = "WIDGET " + std::to_string(i + 1);

                if (ImGui::BeginTabItem(tabName.c_str())) {

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Theme::ModalPadding * Theme::GlobalScale, Theme::ModalPadding * Theme::GlobalScale));
                    ImGui::PushFont(Theme::fontTextWide);

                    ImGui::PushID(i);
                    ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

                    DRAW_LABEL(" HARDWARE SENSOR");
                    ImGui::SetNextItemWidth(Theme::ModalInputWidth * Theme::GlobalScale);

                    if (Theme::ModernCombo("##HardwareSensor", s_SimpleSensors[m_PresetIdx[i]].FullName)) {
                        const char* lastCategory = nullptr;

                        for (int j = 0; j < s_SimpleSensors.size(); j++) {
                            if (lastCategory == nullptr || strcmp(lastCategory, s_SimpleSensors[j].Category) != 0) {
                                if (j != 0) ImGui::Dummy(ImVec2(0, 8.0f * Theme::GlobalScale));

                                ImGui::PushFont(Theme::fontSubtitleNum);
                                ImGui::TextColored(Theme::TextDark, " %s", s_SimpleSensors[j].Category);
                                ImGui::PopFont();

                                lastCategory = s_SimpleSensors[j].Category;
                            }

                            ImGui::PushID(j);
                            ImGui::Indent(16.0f * Theme::GlobalScale);

                            if (ImGui::Selectable(s_SimpleSensors[j].DisplayName, m_PresetIdx[i] == j)) {
                                m_PresetIdx[i] = j;
                                strncpy_s(m_LabelBuf[i], sizeof(m_LabelBuf[i]), s_SimpleSensors[j].DefaultLabel, _TRUNCATE);
                                strncpy_s(m_UnitBuf[i], sizeof(m_UnitBuf[i]), s_SimpleSensors[j].DefaultUnit, _TRUNCATE);
                            }

                            ImGui::Unindent(16.0f * Theme::GlobalScale);
                            ImGui::PopID();
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

                    DRAW_LABEL(" LABEL");
                    ImGui::SetNextItemWidth(Theme::ModalInputWidth * Theme::GlobalScale);
                    ImGui::InputText("##Label", m_LabelBuf[i], sizeof(m_LabelBuf[i]));
                    ImGui::Dummy(ImVec2(0, 12.0f * Theme::GlobalScale));

                    DRAW_LABEL(" UNIT");
                    ImGui::SetNextItemWidth(Theme::ModalInputWidth * Theme::GlobalScale);
                    ImGui::InputText("##Unit", m_UnitBuf[i], sizeof(m_UnitBuf[i]));

                    ImGui::PopID();
                    ImGui::PopFont();
                    ImGui::PopStyleVar();

                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }

        ImGui::PopFont();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);

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
                std::string unitStr = m_UnitBuf[i];
                std::string formatStr = "%.0f";

                if (!unitStr.empty()) {
                    std::string escaped;
                    for (char c : unitStr) { if (c == '%') escaped += "%%"; else escaped += c; }
                    formatStr += escaped;
                }

                config.push_back({ sensor.Path, finalLabel, formatStr, sensor.MinValue, sensor.MaxValue });
            }

            ImVec4 finalColor = Theme::AccentRed;
            if (m_SelectedColorIdx == 1) finalColor = Theme::AccentBlue;
            else if (m_SelectedColorIdx == 2) finalColor = Theme::AccentPurple;
            else if (m_SelectedColorIdx == 3) finalColor = Theme::AccentYellow;

            panels.push_back(std::make_unique<SensorPanel>(uniqueID, m_TitleBuf, m_SubtitleBuf, finalColor, parsedStyle, 8, config));
            isOpen = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleColor(2);
}