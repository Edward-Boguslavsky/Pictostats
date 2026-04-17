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

Panel::Panel(std::string windowID, std::string shortName, std::string title, ImU32 themeColor, PanelStyle style, int columnSpan)
    : m_WindowID(std::move(windowID)), m_ShortName(std::move(shortName)), m_Title(std::move(title)), m_ThemeColor(themeColor), m_Style(style), m_ColumnSpan(columnSpan) {}

void Panel::DrawHeader() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0.0f));
    ImGui::PushFont(Theme::fontHugeTitle);
    ImGui::TextColored(Theme::TextDark, "%s", m_ShortName.c_str());
    ImGui::PopFont();
    ImGui::PushFont(Theme::fontTitle);
    ImGui::TextColored(Theme::TextPrimary, "%s", m_Title.c_str());
    ImGui::PopFont();
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

        float availableWidthForArcs = contentWidth - arcSpacing;
        float singleArcWidth = availableWidthForArcs / 2.0f;
        float radius = (singleArcWidth - thickness) / 2.0f;
        radius = std::max(10.0f, radius);

        for (size_t i = 0; i < sensors.size(); ++i) {
            CustomWidgets::DrawRoundedArcProgressBar(
                sensors[i].Label.c_str(), sensors[i].Progress,
                radius, thickness,
                Theme::ArcTrackColor, m_ThemeColor, Theme::fontValue, Theme::fontLabel, Theme::TextMuted, sensors[i].Text.c_str()
            );
            if (i < sensors.size() - 1) ImGui::SameLine(0, arcSpacing);
        }
        ImGui::EndGroup();
    }
    else if (m_Style == STYLE_BAR) {
        for (size_t i = 0; i < sensors.size(); ++i) {
            CustomWidgets::DrawRoundedLinearProgressBar(
                sensors[i].Label.c_str(),
                sensors[i].Progress,
                contentWidth,
                20.0f * Theme::GlobalScale,
                Theme::ArcTrackColor, m_ThemeColor, Theme::fontValue, Theme::fontLabel, Theme::TextMuted, sensors[i].Text.c_str()
            );
            if (i < sensors.size() - 1) ImGui::Dummy(ImVec2(0, 16.0f * Theme::GlobalScale));
        }
    }
}

CpuGpuPanel::CpuGpuPanel(std::string windowID, std::string shortName, std::string title, ImU32 themeColor, PanelStyle style, int columnSpan, std::string usagePath, std::string tempPath)
    : Panel(std::move(windowID), std::move(shortName), std::move(title), themeColor, style, columnSpan), m_UsagePath(std::move(usagePath)), m_TempPath(std::move(tempPath)) {
    HardwareBackend::RegisterSensor(m_UsagePath);
    HardwareBackend::RegisterSensor(m_TempPath);
}

void CpuGpuPanel::Draw() {
    DrawHeader();
    float usage = HardwareBackend::GetSensorValue(m_UsagePath);
    float temp  = HardwareBackend::GetSensorValue(m_TempPath);
    std::vector<SensorData> data = {
        { usage / 100.0f, FormatStr("%.0f%%", usage), "USAGE" },
        { temp / 100.0f, FormatStr("%.0f\xC2\xB0", temp), "TEMP" }
    };
    RenderVisuals(data);
}

void RamPanel::Draw() {
    DrawHeader();
    float usedRAM = HardwareBackend::GetUsedRamGB();
    float totalRAM = HardwareBackend::GetTotalRamGB();
    float progress = totalRAM > 0.0f ? (usedRAM / totalRAM) : 0.0f;
    std::vector<SensorData> data = {
        { progress, FormatStr("%.0f GB", usedRAM), "USAGE" }
    };
    RenderVisuals(data);
}

void PowerPanel::Draw() {
    DrawHeader();
    float totalPower = HardwareBackend::GetTotalSystemPowerW();
    std::vector<SensorData> data = {
        { totalPower / 1000.0f, FormatStr("%.0f W", totalPower), "DRAW" }
    };
    RenderVisuals(data);
}