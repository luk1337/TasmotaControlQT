cmake_minimum_required(VERSION 3.20)
project(TasmotaControlQT)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Network Svg Widgets REQUIRED)

add_executable(TasmotaControlQT MACOSX_BUNDLE src/main.cpp res/resources.qrc)
target_link_libraries(TasmotaControlQT Qt6::Network Qt6::Svg Qt6::Widgets)

if (APPLE)
    set_target_properties(TasmotaControlQT PROPERTIES
            BUNDLE True
            MACOSX_BUNDLE_BUNDLE_NAME "TasmotaControlQT"
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/MacOSXBundleInfo.plist.in")
endif ()
