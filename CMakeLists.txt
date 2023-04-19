cmake_minimum_required(VERSION 3.16)

project(dohna)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



project(dohna LANGUAGES CXX)


add_subdirectory(tools)
add_subdirectory(net)
add_subdirectory(core)

add_executable(dohna main.cpp)
add_executable(test test.cpp)

target_link_libraries(dohna PUBLIC tools)
target_link_libraries(dohna PUBLIC core)
target_link_libraries(dohna PUBLIC net)
