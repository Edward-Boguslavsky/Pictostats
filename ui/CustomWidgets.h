#ifndef PICTOSTATS_CUSTOMWIDGETS_H
#define PICTOSTATS_CUSTOMWIDGETS_H

#endif //PICTOSTATS_CUSTOMWIDGETS_H

#pragma once
#include "imgui.h"

namespace CustomWidgets {
    void DrawRoundedArcProgressBar(const char* label, float progress, float radius, float thickness,
                                   ImVec4 bgColor, ImVec4 fgColor, ImFont* valFont, ImFont* labelFont,
                                   ImVec4 labelColor, const char* valueText);

    void DrawRoundedLinearProgressBar(const char* label, float progress, float width, float height,
                                      ImVec4 bgColor, ImVec4 fgColor, ImFont* valFont, ImFont* labelFont,
                                      ImVec4 labelColor, const char* valueText);
}