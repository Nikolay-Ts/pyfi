cmake_minimum_required(VERSION 3.20)
project(PyFi LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
include(CTest)

# ================================
# Fetch pybind11 + Catch2
# ================================
FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.13.5
)
FetchContent_MakeAvailable(pybind11)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.6.0
)
FetchContent_MakeAvailable(Catch2)

find_package(Boost REQUIRED)

# ================================
# Core static library
# ================================
add_library(PyFi STATIC
        src/bond.cpp
        src/option.cpp
        src/option_greeks.cpp
)

target_include_directories(PyFi PUBLIC include)
target_link_libraries(PyFi PRIVATE Boost::boost)

# ================================
# Build ONE Python extension module, with submodules
# ================================
pybind11_add_module(_pyfi
        pybind/pyfi_bind.cpp
)

target_link_libraries(_pyfi PRIVATE PyFi)
set_target_properties(_pyfi PROPERTIES PREFIX "")

# Copy built module into python package
add_custom_command(
        TARGET _pyfi POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/pyfi
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:_pyfi>
        ${CMAKE_SOURCE_DIR}/pyfi/$<TARGET_FILE_NAME:_pyfi>
        COMMENT "Copying _pyfi module to pyfi/ directory"
)

# ================================
# Tests
# ================================
add_subdirectory(test)