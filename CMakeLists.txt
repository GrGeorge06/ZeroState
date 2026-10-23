cmake_minimum_required(VERSION 3.16)
project(ZeroState LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebEngineWidgets WebEngineCore Network WebChannel Positioning)

set(APP_SOURCES
    src/main.cpp
)

# On Windows, attach Windows Resource file for .exe icon
if(WIN32)
    set(APP_RESOURCES windows_icon.rc)
endif()

# WIN32 parameter suppresses the black CMD / console pop-up window
add_executable(ZeroState WIN32
    ${APP_SOURCES}
    ${APP_RESOURCES}
)

target_link_libraries(ZeroState PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::WebEngineCore
    Qt6::Network
    Qt6::WebChannel
    Qt6::Positioning
)
