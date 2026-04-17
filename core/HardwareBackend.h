#ifndef PICTOSTATS_HARDWAREBACKEND_H
#define PICTOSTATS_HARDWAREBACKEND_H

#endif //PICTOSTATS_HARDWAREBACKEND_H

#pragma once
#include <string>

class HardwareBackend {
public:
    static void Init();
    static void Shutdown(); // Safely closes the background thread

    // Panels must register their paths so the background thread knows what to poll!
    static void RegisterSensor(const std::string& path);
    static float GetSensorValue(const std::string& path);

    static float GetTotalSystemPowerW();
    static float GetUsedRamGB();
    static float GetTotalRamGB();

private:
    static int s_RamSticks;
    static int s_StorageDrives;
};