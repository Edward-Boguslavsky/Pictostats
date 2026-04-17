#ifndef PICTOSTATS_APPLICATION_H
#define PICTOSTATS_APPLICATION_H

#endif //PICTOSTATS_APPLICATION_H

#pragma once
#include <functional>

class Application {
public:
    Application(const char* title);
    ~Application();

    bool Init();

    void Run(std::function<void()> uiCallback);

    void Close();

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    const char* m_Title;
    int m_Width;
    int m_Height;
    bool m_Running;
};