cmake_minimum_required(VERSION 3.20)
project(PyFi LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-format-style=file")

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


add_library(PyFi STATIC
        src/Bond.cpp
        src/library.cpp
)
target_include_directories(PyFi PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>)
set_target_properties(PyFi PROPERTIES POSITION_INDEPENDENT_CODE ON)

#pybind11_add_module(pyfi MODULE
#        src/bindings.cpp
#)
#target_link_libraries(pyfi PRIVATE PyFi)

add_subdirectory(test)