cmake_minimum_required(VERSION 3.22)
project(opencrossing LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(opencrossing SHARED
    native_bridge.cpp
    game_runtime.cpp
)

target_compile_options(opencrossing PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_compile_definitions(opencrossing PRIVATE
    OPEN_CROSSING_ANDROID=1
    OPEN_CROSSING_ARM64=1
)

target_link_libraries(opencrossing
    android
    log
    GLESv3
)
