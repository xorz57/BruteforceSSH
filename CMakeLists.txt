cmake_minimum_required(VERSION 3.24)

cmake_policy(SET CMP0091 NEW)

project(BruteforceSSH)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (VCPKG_TARGET_TRIPLET MATCHES "-static$")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

find_package(cxxopts REQUIRED)
find_package(libssh REQUIRED)

add_executable(BruteforceSSH src/Main.cpp)
target_link_libraries(BruteforceSSH PRIVATE cxxopts::cxxopts)
target_link_libraries(BruteforceSSH PRIVATE ssh)

add_custom_target(BruteforceSSHAssets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(BruteforceSSH BruteforceSSHAssets)
