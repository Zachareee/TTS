cmake_minimum_required(VERSION 4.1)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Speech)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_NAME main)
add_executable(${TARGET_NAME})

find_package(cppwinrt CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE Microsoft::CppWinRT)
target_link_libraries(${TARGET_NAME} PRIVATE WindowsApp.lib)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE spdlog::spdlog_header_only)

target_precompile_headers(${TARGET_NAME}
  PRIVATE
    src/pch.h
)

target_sources(${TARGET_NAME}
  PRIVATE
    src/main.cpp
)
