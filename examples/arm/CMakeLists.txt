cmake_minimum_required(VERSION 3.10.2)
set(PROJECT_NAME arm)
project(${PROJECT_NAME})

if(NOT MSVC)
    add_definitions("-Wall -Wextra")
else()
    add_definitions("-WX -W2")
endif()

find_package(MAVSDK REQUIRED)
add_executable(${PROJECT_NAME}
${PROJECT_NAME}.cpp
)

target_link_libraries(${PROJECT_NAME}
MAVSDK::mavsdk_telemetry
MAVSDK::mavsdk_action
MAVSDK::mavsdk
MAVSDK::mavsdk_offboard
)