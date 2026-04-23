#ifndef PICTOSTATS_PANELS_H
#define PICTOSTATS_PANELS_H

#endif //PICTOSTATS_PANELS_H

#pragma once
#include <string>
#include <vector>
#include "imgui.h"

enum PanelStyle { STYLE_ARC, STYLE_BAR };

struct SensorData {
    float Progress;
    std::string Text;
    std::string Label;
};

struct SensorConfig {
    std::string Path;
    std::string Label;
    std::string FormatStr;
    float MinValue = 0.0f;
    float MaxValue = 100.0f;
};

class Panel {
public:
    Panel(std::string windowID, std::string shortName, std::string title, ImVec4 themeColor, PanelStyle style, int columnSpan);
    virtual ~Panel() = default;

    virtual void Draw() = 0;

    const std::string& GetWindowID() const { return m_WindowID; }
    bool IsPendingDelete() const { return m_PendingDelete; }

    void SetStyle(PanelStyle style) { m_Style = style; }
    void DrawDeleteButton();

protected:
    std::string m_WindowID;
    std::string m_ShortName;
    std::string m_Title;
    ImVec4 m_ThemeColor;
    PanelStyle m_Style;
    int m_ColumnSpan;
    bool m_PendingDelete = false;

    void DrawHeader();
    void RenderVisuals(const std::vector<SensorData>& sensors);
};

class SensorPanel : public Panel {
public:
    SensorPanel(std::string windowID, std::string shortName, std::string title, ImVec4 themeColor, PanelStyle style, int columnSpan, std::vector<SensorConfig> sensors);
    void Draw() override;
private:
    std::vector<SensorConfig> m_Sensors;
};