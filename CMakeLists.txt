CMAKE_MINIMUM_REQUIRED(VERSION 3.18.1)
project("W1reless Driver")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux64)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux64)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux64)

file(GLOB SOURCES
  ${CMAKE_SOURCE_DIR}/linux64/vrDriver.cpp
  ${CMAKE_SOURCE_DIR}/linux64/Encoder.cpp
  ${CMAKE_SOURCE_DIR}/linux64/VulkanBackend.cpp)

find_package(Vulkan REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(_W1reless SHARED ${SOURCES})

set_target_properties(_W1reless PROPERTIES PREFIX "")
set_target_properties(_W1reless PROPERTIES OUTPUT_NAME "driver_W1reless")

target_link_libraries(_W1reless vulkan)

