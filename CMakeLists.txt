cmake_minimum_required(VERSION 3.20)
project(PyFi LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
include(CTest)

FetchContent_Declare(
        pybind11
        URL https://github.com/pybind/pybind11/archive/refs/tags/v2.13.5.zip)
FetchContent_MakeAvailable(pybind11)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.6.0
)
FetchContent_MakeAvailable(Catch2)

find_package(Boost 1.74 REQUIRED)

add_library(
        PyFi STATIC
        src/bond.cpp
        src/option.cpp
)
target_include_directories(
        PyFi PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(PyFi PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(PyFi PRIVATE Boost::boost)

#pybind11_add_module(pyfi MODULE
#        src/bindings.cpp
#)
#target_link_libraries(pyfi PRIVATE PyFi)

add_subdirectory(test)