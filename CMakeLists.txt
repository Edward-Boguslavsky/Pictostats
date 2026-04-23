cmake_minimum_required(VERSION 4.0)
project(Pictostats)

set(CMAKE_CXX_STANDARD 20)

include_directories(libs/imgui)
include_directories(libs/lhwm)
include_directories(core)
include_directories(ui)
include_directories(panels)

add_executable(Pictostats
        main.cpp
        core/Application.cpp
        core/HardwareBackend.cpp
        ui/CustomWidgets.cpp
        ui/AddModal.cpp
        panels/Panels.cpp
        libs/imgui/imgui.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/imgui_impl_win32.cpp
        libs/imgui/imgui_impl_dx11.cpp
)

target_link_libraries(Pictostats
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/lhwm/lhwm-cpp-wrapper.lib
        mscoree.lib
        d3d11.lib
)

add_custom_command(TARGET Pictostats POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libs/lhwm/lhwm-wrapper.dll"
        $<TARGET_FILE_DIR:Pictostats>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libs/lhwm/LibreHardwareMonitorLib.dll"
        $<TARGET_FILE_DIR:Pictostats>

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/fonts"
        "$<TARGET_FILE_DIR:Pictostats>/fonts"
)