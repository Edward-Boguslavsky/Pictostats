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

    // NEW: Exposes all hardware paths for the future "Advanced Mode" UI
    static const std::vector<std::string>& GetAvailableSensors();

private:
    static int s_RamSticks;
    static int s_StorageDrives;
    static std::vector<std::string> s_AllSensors;
};