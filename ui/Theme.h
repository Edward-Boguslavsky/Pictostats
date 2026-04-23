#ifndef PICTOSTATS_THEME_H
#define PICTOSTATS_THEME_H

#endif //PICTOSTATS_THEME_H

#pragma once
#include "imgui.h"

namespace Theme {
    inline ImVec4 Vec4FromRGB(int r, int g, int b, int a = 255) { return ImVec4(r/255.f, g/255.f, b/255.f, a/255.f); }
    inline ImU32 ToU32(const ImVec4& col) { return ImGui::ColorConvertFloat4ToU32(col); }

    // --- COLORS ---
    inline ImVec4 AppBackground   = Vec4FromRGB(12, 12, 12);
    inline ImVec4 PanelBackground = Vec4FromRGB(20, 20, 20);
    inline ImVec4 PanelBorder     = Vec4FromRGB(35, 35, 35);

    inline ImVec4 TextLight       = Vec4FromRGB(220, 220, 220);
    inline ImVec4 TextMedium      = Vec4FromRGB(140, 140, 140);
    inline ImVec4 TextDark        = Vec4FromRGB(90, 90, 90);

    inline ImVec4 ArcTrackColor   = Vec4FromRGB(35, 35, 35);
    inline ImVec4 AccentRed       = Vec4FromRGB(255, 0, 51);
    inline ImVec4 AccentBlue      = Vec4FromRGB(43, 112, 255);
    inline ImVec4 AccentPurple    = Vec4FromRGB(163, 113, 247);
    inline ImVec4 AccentYellow    = Vec4FromRGB(255, 193, 7);

    // --- DIMENSIONS ---
    inline float GlobalScale      = 1.1f;
    inline float BaseUnit         = 55.0f;
    inline float OverlayPadding   = 40.0f;
    inline float DeleteBtnSize    = 32.0f;
    inline float DeleteBtnPadding = 16.0f;

    // --- FONTS ---
    inline ImFont* fontTitleWide    = nullptr;
    inline ImFont* fontSubtitleWide = nullptr;
    inline ImFont* fontTextWide     = nullptr;

    inline ImFont* fontTitleNum     = nullptr;
    inline ImFont* fontSubtitleNum  = nullptr;
    inline ImFont* fontTextNum      = nullptr;

    inline ImFont* fontTitleDot     = nullptr;
    inline ImFont* fontSubtitleDot  = nullptr;
    inline ImFont* fontTextDot      = nullptr;

    inline void SetupModernTheme() {
        ImGuiStyle& style = ImGui::GetStyle();

        // Window & Spacing
        style.WindowRounding   = 24.0f;
        style.WindowPadding    = ImVec2(32.0f, 32.0f);
        style.ItemSpacing      = ImVec2(24.0f, 24.0f);
        style.WindowBorderSize = 2.0f;

        // Inputs, Dropdowns, Popups
        style.FramePadding     = ImVec2(16.0f * GlobalScale, 14.0f * GlobalScale);
        style.FrameRounding    = 8.0f * GlobalScale;
        style.PopupRounding    = 12.0f * GlobalScale;

        // Color Mapping
        style.Colors[ImGuiCol_WindowBg]       = PanelBackground;
        style.Colors[ImGuiCol_Border]         = PanelBorder;
        style.Colors[ImGuiCol_Text]           = TextLight;
        style.Colors[ImGuiCol_PopupBg]        = Vec4FromRGB(30, 30, 30);
        style.Colors[ImGuiCol_FrameBg]        = Vec4FromRGB(30, 30, 30);
        style.Colors[ImGuiCol_FrameBgHovered] = Vec4FromRGB(45, 45, 45);
        style.Colors[ImGuiCol_FrameBgActive]  = Vec4FromRGB(55, 55, 55);
    }

    // --- UNIVERSAL WIDGET HELPERS ---

    inline bool ModernCombo(const char* label, const char* preview_value) {
        bool open = ImGui::BeginCombo(label, preview_value, ImGuiComboFlags_NoArrowButton);
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        float chevronHalfWidth = 8.0f * GlobalScale;
        float chevronHeight    = 4.0f * GlobalScale;
        float chevronThickness = 2.0f * GlobalScale;
        ImVec2 center(max.x - 28.0f * GlobalScale, (min.y + max.y) * 0.5f);

        float dir = open ? -1.0f : 1.0f;
        dl->AddLine(ImVec2(center.x - chevronHalfWidth, center.y - chevronHeight * dir), ImVec2(center.x, center.y + chevronHeight * dir), ToU32(TextMedium), chevronThickness);
        dl->AddLine(ImVec2(center.x, center.y + chevronHeight * dir), ImVec2(center.x + chevronHalfWidth, center.y - chevronHeight * dir), ToU32(TextMedium), chevronThickness);

        return open;
    }

    inline bool ButtonPrimary(const char* label, const ImVec2& size, ImVec4 accentColor) {
        ImGui::PushStyleColor(ImGuiCol_Button, accentColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.85f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.70f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0)); // Fix vertical text centering

        bool pressed = ImGui::Button(label, size);

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(4);
        return pressed;
    }

    inline bool ButtonSecondary(const char* label, const ImVec2& size, ImVec4 accentColor) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.15f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.30f));
        ImGui::PushStyleColor(ImGuiCol_Text, accentColor);
        ImGui::PushStyleColor(ImGuiCol_Border, accentColor);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0)); // Fix vertical text centering

        bool pressed = ImGui::Button(label, size);

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(5);
        return pressed;
    }

    inline bool ButtonTertiary(const char* label, const ImVec2& size) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Vec4FromRGB(128, 128, 128, 40));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Vec4FromRGB(128, 128, 128, 80));
        ImGui::PushStyleColor(ImGuiCol_Text, TextMedium);
        ImGui::PushStyleColor(ImGuiCol_Border, TextMedium);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0)); // Fix vertical text centering

        bool pressed = ImGui::Button(label, size);

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(5);
        return pressed;
    }
}