#include "CustomWidgets.h"
#include "imgui_internal.h"
#include "Theme.h"
#include <cmath>
#include <algorithm>

namespace CustomWidgets {

void DrawRoundedArcProgressBar(const char* label, float progress, float radius, float thickness, ImVec4 bgColor, ImVec4 fgColor, ImFont* valFont, ImFont* labelFont, ImVec4 labelColor, const char* valueText) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return;
    ImVec2 pos = window->DC.CursorPos; ImVec2 size(radius * 2.0f + thickness, radius * 2.0f + thickness);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y)); ImGui::ItemSize(bb); if (!ImGui::ItemAdd(bb, 0)) return;
    ImDrawList* draw_list = ImGui::GetWindowDrawList(); ImVec2 center(pos.x + radius + thickness * 0.5f, pos.y + radius + thickness * 0.5f);
    float a_min = 3.0f * IM_PI / 4.0f; float a_max = 9.0f * IM_PI / 4.0f;

    ImU32 bgU32 = Theme::ToU32(bgColor);
    ImU32 fgU32 = Theme::ToU32(fgColor);
    ImU32 lblU32 = Theme::ToU32(labelColor);

    draw_list->PathArcTo(center, radius, a_min, a_max, 40); draw_list->PathStroke(bgU32, 0, thickness);
    draw_list->AddCircleFilled(ImVec2(center.x + cosf(a_min) * radius, center.y + sinf(a_min) * radius), thickness / 2.0f, bgU32);
    draw_list->AddCircleFilled(ImVec2(center.x + cosf(a_max) * radius, center.y + sinf(a_max) * radius), thickness / 2.0f, bgU32);

    progress = std::max(0.0f, std::min(1.0f, progress));
    if (progress > 0.0f) {
        float a_val = a_min + (a_max - a_min) * progress; draw_list->PathArcTo(center, radius, a_min, a_val, 40); draw_list->PathStroke(fgU32, 0, thickness);
        draw_list->AddCircleFilled(ImVec2(center.x + cosf(a_min) * radius, center.y + sinf(a_min) * radius), thickness / 2.0f, fgU32);
        draw_list->AddCircleFilled(ImVec2(center.x + cosf(a_val) * radius, center.y + sinf(a_val) * radius), thickness / 2.0f, fgU32);
    }

    ImGui::PushFont(valFont); ImVec2 textSize = ImGui::CalcTextSize(valueText);
    draw_list->AddText(ImVec2(center.x - textSize.x / 2.0f, center.y - textSize.y / 2.0f), fgU32, valueText); ImGui::PopFont();

    ImGui::PushFont(labelFont); ImVec2 labelSize = ImGui::CalcTextSize(label);
    draw_list->AddText(ImVec2(center.x - labelSize.x / 2.0f, center.y + radius - labelSize.y + (10.0f * Theme::GlobalScale)), lblU32, label); ImGui::PopFont();
}

void DrawRoundedLinearProgressBar(const char* label, float progress, float width, float height, ImVec4 bgColor, ImVec4 fgColor, ImFont* valFont, ImFont* labelFont, ImVec4 labelColor, const char* valueText) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return;

    ImGui::PushFont(valFont); ImVec2 valSize = ImGui::CalcTextSize(valueText); ImGui::PopFont();
    ImGui::PushFont(labelFont); ImVec2 labelSize = ImGui::CalcTextSize(label); ImGui::PopFont();

    float spacing = 8.0f * Theme::GlobalScale;
    float totalHeight = std::max(valSize.y, labelSize.y) + spacing + height;

    ImVec2 pos = window->DC.CursorPos;
    ImRect bb(pos, ImVec2(pos.x + width, pos.y + totalHeight));
    ImGui::ItemSize(bb); if (!ImGui::ItemAdd(bb, 0)) return;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    float textBaseY = pos.y + std::max(valSize.y, labelSize.y);

    ImU32 bgU32 = Theme::ToU32(bgColor);
    ImU32 fgU32 = Theme::ToU32(fgColor);
    ImU32 lblU32 = Theme::ToU32(labelColor);

    ImGui::PushFont(labelFont);
    draw_list->AddText(ImVec2(pos.x, textBaseY - labelSize.y), lblU32, label);
    ImGui::PopFont();

    ImGui::PushFont(valFont);
    draw_list->AddText(ImVec2(pos.x + width - valSize.x, textBaseY - valSize.y), fgU32, valueText);
    ImGui::PopFont();

    ImVec2 barMin(pos.x, textBaseY + spacing);
    ImVec2 barMax(pos.x + width, barMin.y + height);
    float radius = height * 0.5f;

    draw_list->AddRectFilled(barMin, barMax, bgU32, radius);
    progress = std::max(0.0f, std::min(1.0f, progress));
    if (progress > 0.0f) {
        float fg_width = std::max(radius * 2.0f, width * progress);
        draw_list->AddRectFilled(barMin, ImVec2(barMin.x + fg_width, barMax.y), fgU32, radius);
    }
}

}