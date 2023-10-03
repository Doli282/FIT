cmake_minimum_required(VERSION 3.0.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
message(${CMAKE_MODULE_PATH})

project(KernelMonitor)

option(DUMMY_MODULE_EXEC "Option for ide to recognize custom target for kernel module." OFF)

add_subdirectory(monlib)
add_subdirectory(monloop)

enable_testing()
add_subdirectory(tests)
