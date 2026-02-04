cmake_minimum_required(VERSION 4.1)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Speech)

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(main)
# set_property(TARGET main PROPERTY CXX_STANDARD 20)
# set_property(TARGET main PROPERTY CXX_STANDARD_REQUIRED ON)

find_package(cppwinrt CONFIG REQUIRED)
target_link_libraries(main PRIVATE Microsoft::CppWinRT)
target_link_libraries(main PRIVATE WindowsApp.lib)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(main PRIVATE spdlog::spdlog_header_only)

target_precompile_headers(main
  PRIVATE
    src/pch.h
)

target_sources(main
  PRIVATE
    src/main.cpp
)

target_compile_features(main PRIVATE cxx_std_20)

# Source - https://stackoverflow.com/a/50882216
# Posted by mrts, modified by community. See post 'Timeline' for change history
# Retrieved 2026-02-04, License - CC BY-SA 4.0
if(MSVC)
  target_compile_options(main PRIVATE /W4 /WX)
else()
  target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
