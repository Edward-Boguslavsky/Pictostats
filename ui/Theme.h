#ifndef PICTOSTATS_THEME_H
#define PICTOSTATS_THEME_H

#endif //PICTOSTATS_THEME_H

#pragma once
#include "imgui.h"

namespace Theme {
#define IM_COL_FROM_RGB(r, g, b) IM_COL32(r, g, b, 255)
    inline ImVec4 Vec4FromRGB(int r, int g, int b) { return ImVec4(r/255.f, g/255.f, b/255.f, 1.0f); }

    inline ImVec4 AppBackground   = Vec4FromRGB(12, 12, 12);
    inline ImVec4 PanelBackground = Vec4FromRGB(20, 20, 20);
    inline ImVec4 PanelBorder     = Vec4FromRGB(35, 35, 35);
    inline ImVec4 TextPrimary     = Vec4FromRGB(220, 220, 220);
    inline ImVec4 TextDark        = Vec4FromRGB(90, 90, 90);

    inline ImU32 ArcTrackColor    = IM_COL_FROM_RGB(35, 35, 35);
    inline ImU32 AccentRed        = IM_COL_FROM_RGB(255, 0, 51);
    inline ImU32 AccentBlue       = IM_COL_FROM_RGB(43, 112, 255);
    inline ImU32 AccentPurple     = IM_COL_FROM_RGB(163, 113, 247);
    inline ImU32 AccentYellow     = IM_COL_FROM_RGB(255, 193, 7);
    inline ImU32 TextMuted        = IM_COL_FROM_RGB(140, 140, 140);

    inline float GlobalScale = 1.1f;
    inline float BaseUnit    = 55.0f;

    inline ImFont* fontHugeTitle = nullptr;
    inline ImFont* fontTitle = nullptr;
    inline ImFont* fontValue = nullptr;
    inline ImFont* fontLabel = nullptr;

    inline void SetupModernTheme() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 24.0f;
        style.WindowPadding  = ImVec2(32.0f, 32.0f);
        style.ItemSpacing    = ImVec2(24.0f, 24.0f);
        style.WindowBorderSize = 2.0f;
        style.Colors[ImGuiCol_WindowBg] = PanelBackground;
        style.Colors[ImGuiCol_Border]   = PanelBorder;
        style.Colors[ImGuiCol_Text]     = TextPrimary;
    }
}