#include "HardwareBackend.h"
#include "lhwm-cpp-wrapper.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <windows.h> // For CoInitializeEx

int HardwareBackend::s_RamSticks = 0;
int HardwareBackend::s_StorageDrives = 0;

static std::unordered_map<std::string, float> s_SensorCache;
static std::mutex s_CacheMutex;
static std::atomic<bool> s_Running{false};
static std::thread s_WorkerThread;

void HardwareBackend::Init() {
    std::cout << "Starting Hardware Monitor Engine...\n";
    auto hwMap = LHWM::GetHardwareSensorMap();

    for (const auto& kv : hwMap) {
        if (kv.first.find("/memory/dimm") != std::string::npos) s_RamSticks++;
        else if (kv.first.find("/nvme/") != std::string::npos || kv.first.find("SSD") != std::string::npos || kv.first.find("HDD") != std::string::npos) s_StorageDrives++;
    }

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
    std::lock_guard<std::mutex> lock(s_CacheMutex);
    if(s_SensorCache.find(path) == s_SensorCache.end()) s_SensorCache[path] = 0.0f;
}

float HardwareBackend::GetSensorValue(const std::string& path) {
    if (path.empty()) return 0.0f;
    std::lock_guard<std::mutex> lock(s_CacheMutex);
    return s_SensorCache[path];
}

float HardwareBackend::GetUsedRamGB() { return GetSensorValue("/ram/data/0"); }
float HardwareBackend::GetTotalRamGB() { return GetSensorValue("/ram/data/0") + GetSensorValue("/ram/data/1"); }

float HardwareBackend::GetTotalSystemPowerW() {
    float cpuPower = GetSensorValue("/amdcpu/0/power/0");
    float gpuPower = GetSensorValue("/gpu-nvidia/0/power/0");
    float gpuLoad  = GetSensorValue("/gpu-nvidia/0/load/0");
    if (gpuPower <= 0.1f && gpuLoad > 0.0f) gpuPower = (gpuLoad / 100.0f) * 320.0f;
    return cpuPower + gpuPower + (s_RamSticks * 4.0f) + (s_StorageDrives * 5.0f) + 25.0f;
}