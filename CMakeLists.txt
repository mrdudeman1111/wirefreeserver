cmake_minimum_required(VERSION 3.18.1)
project("CustomDriver")

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(OVR REQUIRED openvr)

include_directories(/usr/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(W1relessDriver SHARED ${SOURCES})

target_include_directories(W1relessDriver PUBLIC ${OVR_INCLUDE_DIRS})

target_link_libraries(W1relessDriver PUBLIC ${OVR_LIBRARIES} amdenc64 vulkan)
