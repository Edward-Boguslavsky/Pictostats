#ifndef PICTOSTATS_HARDWAREBACKEND_H
#define PICTOSTATS_HARDWAREBACKEND_H

#endif //PICTOSTATS_HARDWAREBACKEND_H

#pragma once
#include <string>
#include <vector>

class HardwareBackend {
public:
    static void Init();
    static void Shutdown();

    static void RegisterSensor(const std::string& path);
    static float GetSensorValue(const std::string& path);

    static float GetTotalSystemPowerW();
    static float GetUsedRamGB();
    static float GetTotalRamGB();

    // Retrieves dynamic hardware strings parsed from the host PC
    static const std::vector<std::string>& GetDetectedHardwareNames();

private:
    static int s_RamSticks;
    static int s_StorageDrives;
    static std::vector<std::string> s_HardwareNames;
};