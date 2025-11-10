cmake_minimum_required(VERSION 3.20)
project(PyFi LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
include(CTest)

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

find_package(Boost 1.74 REQUIRED)

add_library(PyFi STATIC
        src/bond.cpp
        src/option.cpp
)
target_include_directories(PyFi PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(PyFi PRIVATE Boost::boost)

if (COMMAND pybind11_add_module)
    pybind11_add_module(pyfi
            pybind/bond_bind.cpp
    )
    target_link_libraries(pyfi PRIVATE PyFi)
else ()
    add_library(pyfi MODULE pybind/bond_bind.cpp)
    if (TARGET pybind11::module)
        target_link_libraries(pyfi PRIVATE PyFi pybind11::module)
    elseif (TARGET pybind11::pybind11)
        target_link_libraries(pyfi PRIVATE PyFi pybind11::pybind11)
    else ()
        target_include_directories(pyfi PRIVATE ${pybind11_SOURCE_DIR}/include)
        target_link_libraries(pyfi PRIVATE PyFi)
    endif ()
    set_target_properties(pyfi PROPERTIES PREFIX "")
endif ()

if (DEFINED pybind11_SOURCE_DIR)
    message(STATUS "pybind11_SOURCE_DIR=${pybind11_SOURCE_DIR}")
    target_include_directories(pyfi PRIVATE ${pybind11_SOURCE_DIR}/include)
endif ()

if (TARGET pybind11::module)
    target_link_libraries(pyfi PRIVATE pybind11::module)
elseif (TARGET pybind11::pybind11)
    target_link_libraries(pyfi PRIVATE pybind11::pybind11)
endif ()

set_target_properties(pyfi PROPERTIES
        C_VISIBILITY_PRESET "default"
        CXX_VISIBILITY_PRESET "default"
        VISIBILITY_INLINES_HIDDEN OFF
)

# Ensure the compiler emits default visibility as a fallback
target_compile_options(pyfi PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-fvisibility=default>
)

set_target_properties(pyfi PROPERTIES PREFIX "")

add_custom_command(TARGET pyfi
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/python
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:pyfi>
        ${CMAKE_SOURCE_DIR}/python/$<TARGET_FILE_NAME:pyfi>
        COMMENT "Copy pyfi module to ${CMAKE_SOURCE_DIR}/python for python import"
)

install(TARGETS pyfi LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/python)

add_subdirectory(test)