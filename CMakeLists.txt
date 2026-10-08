cmake_minimum_required(VERSION 3.28) 

project(6502-C++20 VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBRARY_NAME Core)

option(ENABLE_TESTING "Enable a Unit Test Build" ON)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

option(USE_CPM "Use CPM?" ON)
if(USE_CPM)
    message(STATUS "Activate CPM")
    include(CPM)
     if(ENABLE_TESTING)
        cpmaddpackage("gh:catchorg/Catch2#v3.11.0")
        list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
    endif()
endif()

add_subdirectory(src)
add_subdirectory(app)

if(ENABLE_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
