CMAKE_MINIMUM_REQUIRED(VERSION 3.18.1)
project("W1reless Driver")

IF(WIN32)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY C:/Program\ Files/Steam/steamapps/common/SteamVR/drivers/W1reless/bin/win64)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY C:/Program\ Files/Steam/steamapps/common/SteamVR/drivers/W1reless/bin/win64)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY C:/Program\ Files/Steam/steamapps/common/SteamVR/drivers/W1reless/bin/win64)

	include_directories(D:/Vulkan/Include D:/Vulkan/Include/vulkan D:/openvr/headers)

	link_directories(D:/Vulkan/Lib D:/openvr/lib/win64 D:/openvr/bin/win64)

	find_package(glfw3 REQUIRED)
ELSE()
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux64)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux64)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux64)

	find_package(Vulkan REQUIRED)
	find_package(glfw3 REQUIRED)
ENDIF()

file(GLOB SOURCES
  ${CMAKE_SOURCE_DIR}/linux64/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(_W1reless SHARED ${SOURCES})

set_target_properties(_W1reless PROPERTIES PREFIX "")
set_target_properties(_W1reless PROPERTIES OUTPUT_NAME "driver_W1reless")

IF(WIN32)
	target_link_libraries(_W1reless vulkan-1 openvr_api glfw)

  #set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY C:/Program\ Files/Steam/steamapps/common/SteamVR/drivers/W1reless/bin/win32)
  #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY C:/Program\ Files/Steam/steamapps/common/SteamVR/drivers/W1reless/bin/win32)
  #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY C:/Program\ Files/Steam/steamapps/common/SteamVR/drivers/W1reless/bin/win32)

  #add_library(_W1reless32 SHARED ${SOURCES})

  #set_target_properties(_W1reless32 PROPERTIES PREFIX "")
  #set_target_properties(_W1reless32 PROPERTIES OUTPUT_NAME "driver_W1reless")
  #set_target_properties(_W1reless32 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

  #target_link_libraries(_W1reless32 vulkan-1 openvr_api glfw)
ELSE()
	target_link_libraries(_W1reless vulkan openvr_api)

  #set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux32)
  #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux32)
  #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux32)

  #add_library(_W1reless32 SHARED ${SOURCES})

  #set_target_properties(_W1reless32 PROPERTIES PREFIX "")
  #set_target_properties(_W1reless32 PROPERTIES OUTPUT_NAME "driver_W1reless")
  #set_target_properties(_W1reless32 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

  #target_link_libraries(_W1reless32 vulkan openvr_api)
ENDIF()
