cmake_minimum_required(VERSION 3.20)
project(TasmotaControlQT)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Network Svg Widgets REQUIRED)

add_executable(TasmotaControlQT src/main.cpp res/resources.qrc)
target_link_libraries(TasmotaControlQT Qt6::Network Qt6::Svg Qt6::Widgets)
