cmake_minimum_required(VERSION 3.10)
project(OSCVRChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(oscpack)

add_executable(OSCVRChat Main.cpp)

target_link_libraries(OSCVRChat PRIVATE oscpack ws2_32 winmm)

add_custom_target(run
    COMMAND OSCVRChat
    DEPENDS OSCVRChat
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Debug
)
