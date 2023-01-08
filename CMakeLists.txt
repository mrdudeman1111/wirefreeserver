cmake_minimum_required(VERSION 3.18.1)
project("CustomDriver")

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/driver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

find_package(PkgConfig REQUIRED)
pkg_search_module(OVR REQUIRED openvr)

add_library(W1relessDriver SHARED ${SOURCES})

target_include_directories(W1relessDriver PUBLIC ${OVR_INCLUDE_DIRS})

target_link_libraries(W1relessDriver PUBLIC ${OVR_LIBRARIES})
