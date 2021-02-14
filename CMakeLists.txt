cmake_minimum_required (VERSION 2.8)

project(libWebmPlayer C CXX)

set(CMAKE_DEBUG_POSTFIX  "_d")

set(VPX_VERSION_MAJOR 0)
set(VPX_VERSION_MINOR 0)
set(VPX_VERSION_PATCH 1)
set(PROJECT_VERSION ${VPX_VERSION_MAJOR}.${VPX_VERSION_MINOR}.${VPX_VERSION_PATCH})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(VPX_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(VPX_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

option(VPX_STATIC_LIBRARY "Static build" ON)
option(VPX_INSTALL_DOCS "Install documentation" OFF)

if(VPX_STATIC_LIBRARY)
	add_definitions(-DVPX_STATIC_LIBRARY)
else()
	add_definitions(-DUVPX_EXPORTS)
endif()

message(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

if(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64" OR
   ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set(VPX_ARCHITECTURE "x64" CACHE STRING "")
else()
    set(VPX_ARCHITECTURE "x86" CACHE STRING "")
endif()

message(STATUS "Architecture: ${VPX_ARCHITECTURE}")

# Print informational messages
message(STATUS "Source path: " ${PROJECT_SOURCE_DIR})
message(STATUS "Binary path: " ${PROJECT_BINARY_DIR})
message(STATUS "CXX flags: " ${CMAKE_CXX_FLAGS})

message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
message(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})

message(STATUS "SOURCE_DIR: ${VPX_SOURCE_DIR}")

set(VPX_BUILD_NUMBER 0)
if(EXISTS "${PROJECT_SOURCE_DIR}/build.txt")
file (STRINGS "${PROJECT_SOURCE_DIR}/build.txt" VPX_BUILD_NUMBER)
MATH(EXPR VPX_BUILD_NUMBER "${VPX_BUILD_NUMBER}+1")
file (WRITE "${PROJECT_SOURCE_DIR}/build.txt" ${VPX_BUILD_NUMBER})
endif()

# Proceed to include source files and build target
set(MAIN_SOURCES
${VPX_SOURCE_DIR}/audio_decoder.cpp
${VPX_SOURCE_DIR}/frame.cpp
${VPX_SOURCE_DIR}/frame_buffer.cpp
${VPX_SOURCE_DIR}/packet.cpp
${VPX_SOURCE_DIR}/timer.cpp
${VPX_SOURCE_DIR}/utils.cpp
${VPX_SOURCE_DIR}/video_player.cpp
${VPX_SOURCE_DIR}/file_reader.cpp
${VPX_SOURCE_DIR}/player.cpp
)
set(MAIN_HEADERS
${VPX_INCLUDE_DIR}/dll_defines.hpp
${VPX_INCLUDE_DIR}/audio_decoder.hpp
${VPX_INCLUDE_DIR}/buffer.hpp
${VPX_INCLUDE_DIR}/frame.hpp
${VPX_INCLUDE_DIR}/frame_buffer.hpp
${VPX_INCLUDE_DIR}/packet.hpp
${VPX_INCLUDE_DIR}/packet_queue.hpp
${VPX_INCLUDE_DIR}/object_pool.hpp
${VPX_INCLUDE_DIR}/timer.hpp
${VPX_INCLUDE_DIR}/utils.hpp
${VPX_INCLUDE_DIR}/video_player.hpp
${VPX_INCLUDE_DIR}/thread_safe_queue.hpp
${VPX_INCLUDE_DIR}/file_reader.hpp
${VPX_INCLUDE_DIR}/player.hpp
${VPX_INCLUDE_DIR}/error_codes.hpp
)

set(SOURCES ${MAIN_SOURCES}
)

set(HEADERS ${MAIN_HEADERS}
)

# Create named folders for the sources within the .vcproj
# Empty name lists them directly under the .vcproj
source_group("" FILES ${MAIN_SOURCES} ${MAIN_HEADERS})

set(VPX_INSTALL_DIR_NAME "libWebmPlayer")

if(UNIX AND NOT MACOSX) # i.e GNU/Linux

    # On linux, cmake adds lib* prefix automatically
    set(VPX_LIBRARY_NAME "WebmPlayer")

    set(CMAKE_VERBOSE_MAKEFILE ON)

    if(${VPX_ARCHITECTURE} STREQUAL "x64")
        set(VPX_RUNTIME_PATH ${PROJECT_SOURCE_DIR}/runtime/linux64 CACHE PATH "")
    else()
        set(VPX_RUNTIME_PATH ${PROJECT_SOURCE_DIR}/runtime/linux32 CACHE PATH "")
    endif()

    # Compile with a compiler that supports c++11
    set(CMAKE_CXX_COMPILER "clang++")
    set(CMAKE_CC_COMPILER "clang")
    set(_CMAKE_TOOLCHAIN_PREFIX "llvm-")

    # Setup flags (c++11 and 32bit binaries)
    # -gdwarf-3 is to use dwarf-3 debugging info format (newer are not readable by QtCreator yet)
    # -pendantic is to report warnings as errors, we don't want any warnings here (serious stuff)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -gdwarf-3 -pedantic -Wall -fPIC -fms-extensions")
    set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS} -Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -O2 -g")

	if(VPX_STATIC_LIBRARY)
		add_custom_command(TARGET INSTALL POST_BUILD
		COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<$<CONFIG:debug>:${PROJECT_BINARY_DIR}/Debug/${VPX_LIBRARY_NAME}${CMAKE_DEBUG_POSTFIX}.a>$<$<CONFIG:release>:${PROJECT_BINARY_DIR}/Release/${VPX_LIBRARY_NAME}.a>" "${VPX_RUNTIME_PATH}/lib")
	else()
		add_custom_command(TARGET INSTALL POST_BUILD
		COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<$<CONFIG:debug>:${PROJECT_BINARY_DIR}/Debug/${VPX_LIBRARY_NAME}${CMAKE_DEBUG_POSTFIX}.a>$<$<CONFIG:release>:${PROJECT_BINARY_DIR}/Release/${VPX_LIBRARY_NAME}.a>" "${VPX_RUNTIME_PATH}/bin")
		
		add_custom_command(TARGET INSTALL POST_BUILD
		COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<$<CONFIG:debug>:${PROJECT_BINARY_DIR}/Debug/${VPX_LIBRARY_NAME}${CMAKE_DEBUG_POSTFIX}.so>$<$<CONFIG:release>:${PROJECT_BINARY_DIR}/Release/${VPX_LIBRARY_NAME}.so>" "${VPX_RUNTIME_PATH}/bin")
	endif()
	
elseif(WIN32)
    set(VPX_LIBRARY_NAME "libWebmPlayer")

    if(${VPX_ARCHITECTURE} STREQUAL "x64")
        set(VPX_RUNTIME_PATH ${PROJECT_SOURCE_DIR}/runtime/win64 CACHE PATH "")
    else()
        set(VPX_RUNTIME_PATH ${PROJECT_SOURCE_DIR}/runtime/win32 CACHE PATH "")
    endif()

    # Turn on the ability to create folders to organize projects (.vcproj)
    # It creates "CMakePredefinedTargets" folder by default and adds CMake
    # defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    add_definitions("/D_CRT_SECURE_NO_WARNINGS /wd4996 /wd4800 /nologo")

    # Set compiler flags and options.
    # Here it is setting the Visual Studio warning level
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc /MP /MT")
    set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS} /Od /Ob0 /Zi /MTd")
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS} /O1 /Ob2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS} /Ox /Ob2 /Ot /DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} /Od /Ob0 /Zi")

	add_custom_target(INSTALL_AFTER_ALL ALL COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_install.cmake)
	add_dependencies(INSTALL_AFTER_ALL ${VPX_LIBRARY_NAME})
	
	if(VPX_STATIC_LIBRARY)
		add_custom_command(TARGET INSTALL_AFTER_ALL POST_BUILD
		COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<$<CONFIG:debug>:${PROJECT_BINARY_DIR}/Debug/${VPX_LIBRARY_NAME}${CMAKE_DEBUG_POSTFIX}.lib>$<$<CONFIG:release>:${PROJECT_BINARY_DIR}/Release/${VPX_LIBRARY_NAME}.lib>" "${VPX_RUNTIME_PATH}/lib")
	else()
		add_custom_command(TARGET INSTALL_AFTER_ALL POST_BUILD
		COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<$<CONFIG:debug>:${PROJECT_BINARY_DIR}/Debug/${VPX_LIBRARY_NAME}${CMAKE_DEBUG_POSTFIX}.lib>$<$<CONFIG:release>:${PROJECT_BINARY_DIR}/Release/${VPX_LIBRARY_NAME}.lib>" "${VPX_RUNTIME_PATH}/bin")
		
		add_custom_command(TARGET INSTALL_AFTER_ALL POST_BUILD
		COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<$<CONFIG:debug>:${PROJECT_BINARY_DIR}/Debug/${VPX_LIBRARY_NAME}${CMAKE_DEBUG_POSTFIX}.dll>$<$<CONFIG:release>:${PROJECT_BINARY_DIR}/Release/${VPX_LIBRARY_NAME}.dll>" "${VPX_RUNTIME_PATH}/bin")
	endif()
endif()

message(STATUS "RUNTIME_DIR: ${VPX_RUNTIME_PATH}")

# Environment
include(TestForANSIForScope)
include(TestForANSIStreamHeaders)
include(TestForSTDNamespace)

add_definitions(
-DVPX_BUILD_NUMBER=${VPX_BUILD_NUMBER}
)

# Include headers
include_directories(BEFORE SYSTEM ${PROJECT_SOURCE_DIR}/include)
include_directories(BEFORE SYSTEM ${VPX_RUNTIME_PATH}/include)

if(VPX_STATIC_LIBRARY)
add_library(${VPX_LIBRARY_NAME} STATIC ${SOURCES} ${HEADERS})
else()
add_library(${VPX_LIBRARY_NAME} SHARED ${SOURCES} ${HEADERS})
endif()

set_target_properties(${VPX_LIBRARY_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(${VPX_LIBRARY_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set_target_properties(${VPX_LIBRARY_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set_target_properties(${VPX_LIBRARY_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(${VPX_LIBRARY_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set_target_properties(${VPX_LIBRARY_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set_target_properties(${VPX_LIBRARY_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(${VPX_LIBRARY_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set_target_properties(${VPX_LIBRARY_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)

if(NOT VPX_STATIC_LIBRARY)
	if(UNIX AND NOT APPLE)
		set(LIBRARIES "-lX11 -lXt -lXaw -lXrandr -ldl -lpthread") #X11
		
		target_link_libraries(${VPX_LIBRARY_NAME}
			optimized ${VPX_RUNTIME_PATH}/lib/libWebmPlayer.a
			debug ${VPX_RUNTIME_PATH}/lib/libWebmPlayer_d.a
			${VPX_RUNTIME_PATH}/lib/libvorbis.a
			${VPX_RUNTIME_PATH}/lib/libvorbisfile.a
			${VPX_RUNTIME_PATH}/lib/libogg.a
			optimized ${VPX_RUNTIME_PATH}/lib/libwebm.a
			debug ${VPX_RUNTIME_PATH}/lib/libwebmd.a
			${VPX_RUNTIME_PATH}/lib/libvpx.a)

		# Installation
		install(TARGETS ${VPX_LIBRARY_NAME} LIBRARY DESTINATION "${PROJECT_SOURCE_DIR}/../../VpxExampleUnityProject/Assets/Plugins/${UVPX_UNITY_PLUGIN_DIR}")
	elseif(WIN32)
		target_link_libraries(${VPX_LIBRARY_NAME}
			opengl32.lib
			optimized ${VPX_RUNTIME_PATH}/lib/libWebmPlayer.lib
			debug ${VPX_RUNTIME_PATH}/lib/libWebmPlayer_d.lib
			${VPX_RUNTIME_PATH}/lib/libogg.lib
			${VPX_RUNTIME_PATH}/lib/libvorbis.lib
			${VPX_RUNTIME_PATH}/lib/libvorbisfile.lib
			optimized ${VPX_RUNTIME_PATH}/lib/libwebm.lib
			debug ${VPX_RUNTIME_PATH}/lib/libwebmd.lib
			${VPX_RUNTIME_PATH}/lib/vpxmt.lib)
	endif()
endif()

# Set properties
set_target_properties(${VPX_LIBRARY_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${VPX_VERSION_MAJOR})

# Installation
install(TARGETS ${VPX_LIBRARY_NAME}
ARCHIVE DESTINATION ${VPX_RUNTIME_PATH}/lib)

install(DIRECTORY ${VPX_INCLUDE_DIR}/ DESTINATION ${VPX_RUNTIME_PATH}/include/${VPX_INSTALL_DIR_NAME} FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.inl")

if(VPX_INSTALL_DOCS)
    install(DIRECTORY ${VPX_DOC_DIR} DESTINATION ${VPX_RUNTIME_PATH} PATTERN "Doxyfile" EXCLUDE)
endif()
