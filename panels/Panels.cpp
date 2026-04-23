#include "Panels.h"
#include "HardwareBackend.h"
#include "CustomWidgets.h"
#include "Theme.h"
#include <stdarg.h>

inline std::string FormatStr(const char* format, ...) {
    char buf[64]; va_list args;
    va_start(args, format); vsnprintf(buf, sizeof(buf), format, args); va_end(args);
    return std::string(buf);
}

Panel::Panel(std::string windowID, std::string shortName, std::string title, ImVec4 themeColor, PanelStyle style, int columnSpan)
    : m_WindowID(std::move(windowID)), m_ShortName(std::move(shortName)), m_Title(std::move(title)), m_ThemeColor(themeColor), m_Style(style), m_ColumnSpan(columnSpan) {}

void Panel::DrawHeader() {
    if (m_ShortName.empty() && m_Title.empty()) return;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0.0f));

    if (!m_ShortName.empty()) {
        ImGui::PushFont(Theme::fontTitleWide);
        ImGui::TextColored(Theme::TextDark, "%s", m_ShortName.c_str());
        ImGui::PopFont();
    }
    if (!m_Title.empty()) {
        ImGui::PushFont(Theme::fontTextNum);
        ImGui::TextColored(Theme::TextLight, "%s", m_Title.c_str());
        ImGui::PopFont();
    }

    ImGui::PopStyleVar();
    ImGui::Dummy(ImVec2(0.0f, 0.0f));
}

void Panel::RenderVisuals(const std::vector<SensorData>& sensors) {
    float totalPanelWidth = m_ColumnSpan * Theme::BaseUnit * Theme::GlobalScale;
    float paddingX = ImGui::GetStyle().WindowPadding.x * 2.0f;
    float contentWidth = totalPanelWidth - paddingX;

    if (m_Style == STYLE_ARC) {
        ImGui::BeginGroup();
        float thickness = 20.0f * Theme::GlobalScale;
        float arcSpacing = 40.0f * Theme::GlobalScale;

        size_t count = sensors.size() == 0 ? 1 : sensors.size();
        float availableWidthForArcs = contentWidth - (arcSpacing * (count - 1));
        float singleArcWidth = availableWidthForArcs / count;
        float radius = (singleArcWidth - thickness) / 2.0f;
        radius = std::max(10.0f, radius);

        for (size_t i = 0; i < sensors.size(); ++i) {
            CustomWidgets::DrawRoundedArcProgressBar(
                sensors[i].Label.c_str(), sensors[i].Progress, radius, thickness,
                Theme::ArcTrackColor, m_ThemeColor, Theme::fontTitleNum, Theme::fontTextNum, Theme::TextMedium, sensors[i].Text.c_str()
            );
            if (i < sensors.size() - 1) ImGui::SameLine(0, arcSpacing);
        }
        ImGui::EndGroup();
    }
    else if (m_Style == STYLE_BAR) {
        for (size_t i = 0; i < sensors.size(); ++i) {
            CustomWidgets::DrawRoundedLinearProgressBar(
                sensors[i].Label.c_str(), sensors[i].Progress, contentWidth, 20.0f * Theme::GlobalScale,
                Theme::ArcTrackColor, m_ThemeColor, Theme::fontTitleNum, Theme::fontTextNum, Theme::TextMedium, sensors[i].Text.c_str()
            );
            if (i < sensors.size() - 1) ImGui::Dummy(ImVec2(0, 16.0f * Theme::GlobalScale));
        }
    }
}

void Panel::DrawDeleteButton() {
    float btnSize = Theme::DeleteBtnSize * Theme::GlobalScale;
    float padding = Theme::DeleteBtnPadding * Theme::GlobalScale;

    ImVec2 windowPos  = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 btnMin(windowPos.x + windowSize.x - btnSize - padding, windowPos.y + padding);
    ImVec2 btnMax(btnMin.x + btnSize, btnMin.y + btnSize);

    bool windowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_None);
    bool hovered = windowHovered && ImGui::IsMouseHoveringRect(btnMin, btnMax);
    bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    ImDrawList* dl = ImGui::GetWindowDrawList();

    if (hovered) {
        ImU32 bgColor = clicked ? Theme::ToU32(ImVec4(1.0f, 0.2f, 0.2f, 0.8f)) : Theme::ToU32(ImVec4(1.0f, 0.2f, 0.2f, 0.4f));
        dl->AddRectFilled(btnMin, btnMax, bgColor, 4.0f * Theme::GlobalScale);
    }

    ImGui::PushFont(Theme::fontSubtitleWide);
    ImVec2 textSize = ImGui::CalcTextSize("×");
    ImU32 textColor = hovered ? IM_COL32(255, 100, 100, 255) : Theme::ToU32(Theme::TextMedium);
    dl->AddText(ImVec2(btnMin.x + (btnSize - textSize.x) * 0.5f, btnMin.y + (btnSize - textSize.y) * 0.5f), textColor, "×");
    ImGui::PopFont();

    if (clicked) m_PendingDelete = true;
}

SensorPanel::SensorPanel(std::string windowID, std::string shortName, std::string title, ImVec4 themeColor, PanelStyle style, int columnSpan, std::vector<SensorConfig> sensors)
    : Panel(std::move(windowID), std::move(shortName), std::move(title), themeColor, style, columnSpan), m_Sensors(std::move(sensors)) {
    for (const auto& sensor : m_Sensors) HardwareBackend::RegisterSensor(sensor.Path);
}

void SensorPanel::Draw() {
    DrawHeader();
    std::vector<SensorData> data;
    for (const auto& sensor : m_Sensors) {
        float val = HardwareBackend::GetSensorValue(sensor.Path);
        float progress = 0.0f;
        if (sensor.MaxValue > sensor.MinValue) {
            progress = (val - sensor.MinValue) / (sensor.MaxValue - sensor.MinValue);
        }
        data.push_back({ progress, FormatStr(sensor.FormatStr.c_str(), val), sensor.Label });
    }
    RenderVisuals(data);
}