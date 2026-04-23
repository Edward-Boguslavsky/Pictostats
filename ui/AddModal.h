#pragma once
#include <vector>
#include <memory>
#include "Panels.h"

class AddModal {
public:
    void Render(bool& isOpen, int appWidth, int appHeight, std::vector<std::unique_ptr<Panel>>& panels);

private:
    int m_IdCounter = 100; // Used to generate unique window IDs like "Panel_101"

    // Step 1: Panel State
    int m_StyleIdx = 0;     // 0 = Linear Bar, 1 = Radial Arc
    int m_NumSensors = 1;   // How many widgets (1, 2, or 3)
    char m_TitleBuf[64] = "";
    char m_SubtitleBuf[64] = "";

    // Step 2: Per-Widget State (Arrays to hold up to 3 sensors)
    int m_PresetIdx[3] = {0, 0, 0};
    char m_LabelBuf[3][32];
    char m_UnitBuf[3][16];
};