
cmake_minimum_required(VERSION 2.8.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED on)

project(DayOSDrivers)

set(CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/CMake
  ${CMAKE_MODULE_PATH}
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/image/system)
message("Driver system output to " ${EXECUTABLE_OUTPUT_PATH})
set(RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

set(ASM_EXTENSION *.S)
set(CMAKE_C_FLAGS "-Wall -g") # -pedantic-errors")
set(CMAKE_CXX_FLAGS "-g -fno-use-cxa-atexit -Wall -Wextra -pedantic-errors")

add_subdirectory(hid) 
add_subdirectory(character)
add_subdirectory(block)
add_subdirectory(pci)
add_subdirectory(video)
add_subdirectory(filesystem)
