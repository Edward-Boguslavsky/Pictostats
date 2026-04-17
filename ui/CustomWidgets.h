#ifndef PICTOSTATS_CUSTOMWIDGETS_H
#define PICTOSTATS_CUSTOMWIDGETS_H

#endif //PICTOSTATS_CUSTOMWIDGETS_H

#pragma once
#include "imgui.h"

namespace CustomWidgets {
    void DrawRoundedArcProgressBar(const char* label, float progress, float radius, float thickness,
                                   ImU32 bgColor, ImU32 fgColor, ImFont* valFont, ImFont* labelFont,
                                   ImU32 labelColor, const char* valueText);

    void DrawRoundedLinearProgressBar(const char* label, float progress, float width, float height,
                                      ImU32 bgColor, ImU32 fgColor, ImFont* valFont, ImFont* labelFont,
                                      ImU32 labelColor, const char* valueText);
}