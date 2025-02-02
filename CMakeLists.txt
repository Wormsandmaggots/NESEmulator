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
CPMAddPackage("gh:google/googletest@1.15.2")

set(SDL2_PATH "thirdparty/SDL2-2.30.8")

file(GLOB_RECURSE SOURCE_FILES
        code/src/*.c
        code/src/*.cpp)


file(GLOB_RECURSE HEADER_FILES
        code/inc/*.h
        code/inc/*.hpp)

file(GLOB_RECURSE TEST_FILES
        tests/*.cpp)

add_executable(NESEmulator
        code/main.cpp
        ${HEADER_FILES}
        ${SOURCE_FILES}
)

find_package(SDL2 REQUIRED)
target_include_directories(NESEmulator PRIVATE ${SDL2_INCLUDE_DIR}
thirdparty)

target_include_directories(NESEmulator PRIVATE
    code/inc)

target_link_libraries(NESEmulator
        ${SDL2_LIBRARY}
        spdlog
)

#add_executable(MyTests ${TEST_FILES} ${HEADER_FILES}
#        ${SOURCE_FILES})
#
#target_include_directories(MyTests PRIVATE
#        code/inc)
#
#target_link_libraries(MyTests gtest gtest_main ${SDL2_LIBRARY}
#        spdlog)