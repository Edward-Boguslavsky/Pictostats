#pragma once
#include <vector>
#include <memory>
#include "Panels.h"

class AddModal {
public:
    void Render(bool& isOpen, int appWidth, int appHeight, std::vector<std::unique_ptr<Panel>>& panels);

private:
    int m_IdCounter = 100;

    // Step 1: Panel State
    int m_StyleIdx = 0;
    int m_NumSensors = 1;
    int m_SelectedColorIdx = 0; // 0=Red, 1=Blue, 2=Purple, 3=Yellow
    char m_TitleBuf[64] = "";
    char m_SubtitleBuf[64] = "";

    // Step 2: Per-Widget State
    int m_PresetIdx[3] = {0, 0, 0};
    char m_LabelBuf[3][32];
    char m_UnitBuf[3][16];
};