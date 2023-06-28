CMAKE_MINIMUM_REQUIRED(VERSION 3.18.1)
project("W1reless Driver")

IF(WIN32)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY C:/Program\ Files/Steam/steamapps/common/SteamVR/drivers/W1reless/bin/win64)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY C:/Program\ Files/Steam/steamapps/common/SteamVR/drivers/W1reless/bin/win64)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY C:/Program\ Files/Steam/steamapps/common/SteamVR/drivers/W1reless/bin/win64)

	include_directories(D:/Vulkan/Include D:/openvr/headers)

	link_directories(D:/Vulkan/Lib D:/openvr/lib/win64 D:/openvr/bin/win64)
ELSE()
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux64)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux64)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ~/.steam/steam/steamapps/common/SteamVR/drivers/W1reless/bin/linux64)

	find_package(Vulkan REQUIRED)
ENDIF()

#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_CXX_COMPILER g++)
#set(CMAKE_C_FLAGS -m32)
#set(CMAKE_CXX_FLAGS -m32)

file(GLOB SOURCES
  ${CMAKE_SOURCE_DIR}/linux64/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

#add_library(_W1reless32 SHARED ${SOURCES})
#set_target_properties(_W1reless32 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

#set_target_properties(_W1reless32 PROPERTIES PREFIX "")
#set_target_properties(_W1reless32 PROPERTIES OUTPUT_NAME "driver_W1reless")

add_library(_W1reless64 SHARED ${SOURCES})

set_target_properties(_W1reless64 PROPERTIES PREFIX "driver_")
set_target_properties(_W1reless64 PROPERTIES OUTPUT_NAME "W1reless")

IF(WIN32)
	target_link_libraries(_W1reless64 vulkan-1 openvr_api)
ELSE()
	target_link_libraries(_W1reless64 vulkan openvr_api)
  #target_link_libraries(_W1reless32 vulkan openvr_api)
ENDIF()
