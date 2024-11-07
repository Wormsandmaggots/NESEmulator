# CMakeList.txt : CMake project for SDL_cmake_template, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.7.2)

project(NESEmulator)
set(CMAKE_CXX_STANDARD 26)

# TODO: Add tests and install targets if needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(CPM)

CPMAddPackage("gh:gabime/spdlog@1.10.0")

set(SDL2_PATH "thirdparty/SDL2-2.30.8")

file(GLOB_RECURSE SOURCE_FILES
        code/src/*.c
        code/src/*.cpp)


file(GLOB_RECURSE HEADER_FILES
        code/inc/*.h
        code/inc/*.hpp)

add_executable(NESEmulator
        code/main.cpp
        ${HEADER_FILES}
        ${SOURCE_FILES}
        code/inc/Settings.h
        code/src/Cartridge.cpp
        code/inc/Cartridge.h
        code/src/CPU.cpp
        code/inc/CPU.h
        code/src/Memory.cpp
        code/inc/Memory.h
        code/src/Opcodes.cpp
        code/inc/Opcodes.h
        code/inc/Utils.h
        code/inc/LogMessages.h
        code/inc/NESHelpers.h
        code/src/NESHelpers.cpp
)

find_package(SDL2 REQUIRED)
target_include_directories(NESEmulator PRIVATE ${SDL2_INCLUDE_DIR})

target_include_directories(NESEmulator PRIVATE
    code/inc)

target_link_libraries(NESEmulator
        ${SDL2_LIBRARY}
        spdlog
)