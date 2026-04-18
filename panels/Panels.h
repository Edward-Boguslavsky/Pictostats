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

class Panel {
public:
    Panel(std::string windowID, std::string shortName, std::string title, ImU32 themeColor, PanelStyle style, int columnSpan);
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
    ImU32 m_ThemeColor;
    PanelStyle m_Style;
    int m_ColumnSpan;
    bool m_PendingDelete = false;

    void DrawHeader();
    void RenderVisuals(const std::vector<SensorData>& sensors);
};

class CpuGpuPanel : public Panel {
public:
    CpuGpuPanel(std::string windowID, std::string shortName, std::string title, ImU32 themeColor, PanelStyle style, int columnSpan, std::string usagePath, std::string tempPath);
    void Draw() override;
private:
    std::string m_UsagePath, m_TempPath;
};

class RamPanel : public Panel {
public:
    using Panel::Panel;
    void Draw() override;
};

class StoragePanel : public Panel {
public:
    StoragePanel(std::string windowID, std::string shortName, std::string title, ImU32 themeColor, PanelStyle style, int columnSpan, std::string drivePath);
    void Draw() override;
private:
    std::string m_DrivePath;
};

class PowerPanel : public Panel {
public:
    using Panel::Panel;
    void Draw() override;
};