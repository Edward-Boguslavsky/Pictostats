#ifndef PICTOSTATS_THEME_H
#define PICTOSTATS_THEME_H

#endif //PICTOSTATS_THEME_H

#pragma once
#include "imgui.h"
#include <algorithm> // Required for std::max in color picker

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
    inline ImVec4 AccentYellow    = Vec4FromRGB(246, 166, 0);
    inline ImVec4 AccentGreen     = Vec4FromRGB(0, 170, 35);
    inline ImVec4 AccentTeal     = Vec4FromRGB(0, 177, 153);
    inline ImVec4 AccentBlue      = Vec4FromRGB(0, 131, 255);
    inline ImVec4 AccentPurple    = Vec4FromRGB(166, 80, 255);

    // --- DIMENSIONS ---
    inline float GlobalScale      = 1.1f;
    inline float BaseUnit         = 55.0f;
    inline float OverlayPadding   = 40.0f;
    inline float DeleteBtnSize    = 32.0f;
    inline float DeleteBtnPadding = 16.0f;

    // --- MODAL DIMENSIONS ---
    inline float ModalButtonWidth   = 160.0f;
    inline float ModalButtonHeight  = 64.0f;
    inline float ModalButtonSpacing = 18.0f;
    inline float ModalInputWidth    = 400.0f;
    inline float ModalPadding       = 24.0f;
    inline float ModalRounding      = 16.0f;
    inline float TightItemSpacing   = 6.0f;

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

        style.WindowRounding   = 24.0f;
        style.WindowPadding    = ImVec2(32.0f, 32.0f);
        style.ItemSpacing      = ImVec2(24.0f, 24.0f);
        style.WindowBorderSize = 2.0f;

        style.FramePadding     = ImVec2(16.0f * GlobalScale, 14.0f * GlobalScale);
        style.FrameRounding    = 8.0f * GlobalScale;
        style.PopupRounding    = 12.0f * GlobalScale;

        style.Colors[ImGuiCol_WindowBg]       = PanelBackground;
        style.Colors[ImGuiCol_Border]         = PanelBorder;
        style.Colors[ImGuiCol_Text]           = TextLight;
        style.Colors[ImGuiCol_PopupBg]        = Vec4FromRGB(30, 30, 30);
        style.Colors[ImGuiCol_FrameBg]        = Vec4FromRGB(30, 30, 30);
        style.Colors[ImGuiCol_FrameBgHovered] = Vec4FromRGB(45, 45, 45);
        style.Colors[ImGuiCol_FrameBgActive]  = Vec4FromRGB(55, 55, 55);

        style.Colors[ImGuiCol_Header]         = ImVec4(AccentRed.x, AccentRed.y, AccentRed.z, 0.70f);
        style.Colors[ImGuiCol_HeaderHovered]  = ImVec4(AccentRed.x, AccentRed.y, AccentRed.z, 0.85f);
        style.Colors[ImGuiCol_HeaderActive]   = ImVec4(AccentRed.x, AccentRed.y, AccentRed.z, 1.00f);
    }

    // --- UNIVERSAL WIDGET HELPERS ---

    inline bool ModernCombo(const char* label, const char* preview_value) {
        ImDrawList* dl = ImGui::GetWindowDrawList();

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = ImGui::CalcItemWidth();
        float height = ImGui::GetFrameHeight();
        ImVec2 max(pos.x + width, pos.y + height);

        bool open = ImGui::BeginCombo(label, preview_value, ImGuiComboFlags_NoArrowButton);

        float chevronHalfWidth = 8.0f * GlobalScale;
        float chevronHeight    = 4.0f * GlobalScale;
        float chevronThickness = 2.0f * GlobalScale;
        ImVec2 center(max.x - 28.0f * GlobalScale, pos.y + height * 0.5f);

        float dir = open ? -1.0f : 1.0f;

        dl->AddLine(ImVec2(center.x - chevronHalfWidth, center.y - chevronHeight * dir), ImVec2(center.x, center.y + chevronHeight * dir), ToU32(TextMedium), chevronThickness);
        dl->AddLine(ImVec2(center.x, center.y + chevronHeight * dir), ImVec2(center.x + chevronHalfWidth, center.y - chevronHeight * dir), ToU32(TextMedium), chevronThickness);

        return open;
    }

    inline bool ButtonPrimary(const char* label, const ImVec2& size, ImVec4 accentColor) {
        ImGui::PushStyleColor(ImGuiCol_Button, accentColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.85f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.70f));
        ImGui::PushStyleColor(ImGuiCol_Text, TextLight);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

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
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

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
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        bool pressed = ImGui::Button(label, size);

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(5);
        return pressed;
    }

    // Creates a square button highly rounded to look like a circle with dynamic border states
    inline bool ColorPickerButton(const char* str_id, const ImVec2& size, ImVec4 accentColor, bool selected) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        bool clicked = ImGui::InvisibleButton(str_id, size);
        bool hovered = ImGui::IsItemHovered();

        float rounding = 24.0f * GlobalScale;
        float borderThickness = 6.0f * GlobalScale;

        ImU32 borderColor;
        if (selected) {
            borderColor = ToU32(TextLight);
        } else if (hovered) {
            borderColor = ToU32(ImVec4(TextLight.x, TextLight.y, TextLight.z, 0.85f));
        } else {
            borderColor = ToU32(ImVec4(TextLight.x, TextLight.y, TextLight.z, 0.70f));
        }

        dl->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), borderColor, rounding);
        dl->AddRectFilled(
            ImVec2(p.x + borderThickness, p.y + borderThickness),
            ImVec2(p.x + size.x - borderThickness, p.y + size.y - borderThickness),
            ToU32(accentColor),
            std::max(0.0f, rounding - borderThickness)
        );

        return clicked;
    }
}