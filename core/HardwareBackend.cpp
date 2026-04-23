#include "HardwareBackend.h"
#include "lhwm-cpp-wrapper.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <algorithm> // For std::sort
#include <windows.h> // For CoInitializeEx

int HardwareBackend::s_RamSticks = 0;
int HardwareBackend::s_StorageDrives = 0;
std::vector<std::string> HardwareBackend::s_AllSensors;

static std::unordered_map<std::string, float> s_SensorCache;
static std::mutex s_CacheMutex;
static std::atomic<bool> s_Running{false};
static std::thread s_WorkerThread;

void HardwareBackend::Init() {
    std::cout << "Starting Hardware Monitor Engine...\n";
    auto hwMap = LHWM::GetHardwareSensorMap();

    for (const auto& kv : hwMap) {
        // Cache every single sensor path for the Advanced Mode UI
        s_AllSensors.push_back(kv.first);

        if (kv.first.find("/memory/dimm") != std::string::npos) s_RamSticks++;
        else if (kv.first.find("/nvme/") != std::string::npos || kv.first.find("SSD") != std::string::npos || kv.first.find("HDD") != std::string::npos) s_StorageDrives++;
    }

    // Sort alphabetically so the Advanced Mode dropdown is organized
    std::sort(s_AllSensors.begin(), s_AllSensors.end());

    s_Running = true;
    s_WorkerThread = std::thread([]() {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        while(s_Running) {
            std::vector<std::string> pathsToPoll;
            {
                std::lock_guard<std::mutex> lock(s_CacheMutex);
                for(auto& kv : s_SensorCache) pathsToPoll.push_back(kv.first);
            }

            std::unordered_map<std::string, float> newValues;
            for(const auto& path : pathsToPoll) {
                newValues[path] = LHWM::GetSensorValue(path);
            }

            {
                std::lock_guard<std::mutex> lock(s_CacheMutex);
                for(const auto& kv : newValues) s_SensorCache[kv.first] = kv.second;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        CoUninitialize();
    });
}

void HardwareBackend::Shutdown() {
    s_Running = false;
    if (s_WorkerThread.joinable()) s_WorkerThread.join();
}

void HardwareBackend::RegisterSensor(const std::string& path) {
    if(path.empty()) return;

    if (path == "/virtual/ram/used") {
        RegisterSensor("/ram/data/0"); RegisterSensor("/ram/data/1"); return;
    }
    if (path == "/virtual/sys/power") {
        RegisterSensor("/amdcpu/0/power/0"); RegisterSensor("/gpu-nvidia/0/power/0"); RegisterSensor("/gpu-nvidia/0/load/0"); return;
    }
    if (path == "/virtual/nvme/0/used") {
        RegisterSensor("/nvme/0/data/31"); RegisterSensor("/nvme/0/data/32"); return;
    }
    if (path == "/virtual/nvme/1/used") {
        RegisterSensor("/nvme/1/data/31"); RegisterSensor("/nvme/1/data/32"); return;
    }

    std::lock_guard<std::mutex> lock(s_CacheMutex);
    if(s_SensorCache.find(path) == s_SensorCache.end()) s_SensorCache[path] = 0.0f;
}

float HardwareBackend::GetSensorValue(const std::string& path) {
    if (path.empty()) return 0.0f;

    if (path == "/virtual/ram/used")     return GetUsedRamGB();
    if (path == "/virtual/sys/power")    return GetTotalSystemPowerW();
    if (path == "/virtual/nvme/0/used")  return GetSensorValue("/nvme/0/data/32") - GetSensorValue("/nvme/0/data/31");
    if (path == "/virtual/nvme/1/used")  return GetSensorValue("/nvme/1/data/32") - GetSensorValue("/nvme/1/data/31");

    std::lock_guard<std::mutex> lock(s_CacheMutex);
    return s_SensorCache[path];
}

const std::vector<std::string>& HardwareBackend::GetAvailableSensors() {
    return s_AllSensors;
}

float HardwareBackend::GetUsedRamGB() {
    return GetSensorValue("/ram/data/0");
}

float HardwareBackend::GetTotalRamGB() {
    return GetSensorValue("/ram/data/0") + GetSensorValue("/ram/data/1");
}

float HardwareBackend::GetTotalSystemPowerW() {
    float cpuPower = GetSensorValue("/amdcpu/0/power/0");
    float gpuPower = GetSensorValue("/gpu-nvidia/0/power/0");
    float gpuLoad  = GetSensorValue("/gpu-nvidia/0/load/0");
    // Fallback if GPU power isn't reading correctly but load is present
    if (gpuPower <= 0.1f && gpuLoad > 0.0f) gpuPower = (gpuLoad / 100.0f) * 320.0f;
    return cpuPower + gpuPower + (s_RamSticks * 4.0f) + (s_StorageDrives * 5.0f) + 25.0f;
}