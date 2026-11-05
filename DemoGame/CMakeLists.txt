cmake_minimum_required (VERSION 3.14)
project ("DemoGame")
set(CMAKE_CXX_STANDARD 17)
set(SRC_FILES main.cpp Layers/DemoLayer.cpp Layers/Graphics/VulkanLayer.cpp)
set(HEADER_FILES Layers/DemoLayer.hpp)
include_directories("./" REQUIRED)
include_directories(../Engine REQUIRED)
include_directories("../vendor/headers/bullet" REQUIRED)
include_directories("../vendor/headers/" REQUIRED)
include_directories("../vendor/headers/mpi" REQUIRED)
add_executable (DemoGame ${SRC_FILES} ${HEADER_FILES})
target_link_libraries(DemoGame Engine)
IF (WIN32)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/win)
file(COPY ${STATIC_LIBS_DIR}/openal/OpenAL32.dll DESTINATION ${CMAKE_BINARY_DIR}/DemoGame)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/SDL2/SDL2.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/SDL2/SDL2main.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/vulkan/vulkan-1.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/freetype/freetype.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/openal/OpenAL32.lib)
if (CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/bullet/BulletCollision.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/bullet/BulletDynamics.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/bullet/BulletSoftBody.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/bullet/LinearMath.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/mpi/mpi.lib)
endif()
if (CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/bullet/BulletCollision_Debug.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/bullet/BulletDynamics_Debug.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/bullet/BulletSoftBody_Debug.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/bullet/LinearMath_Debug.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/mpi/mpid.lib)
endif()
target_link_libraries(DemoGame Winmm.lib)
target_link_libraries(DemoGame Setupapi.lib)
target_link_libraries(DemoGame Version.lib)
target_link_libraries(DemoGame Imm32.lib)
target_link_libraries(DemoGame Bcrypt.lib)
target_link_libraries(DemoGame ws2_32.lib)
target_link_libraries(DemoGame ${STATIC_LIBS_DIR}/mono/libmono-static-sgen.lib)
ENDIF()
IF (UNIX)
set(STATIC_LIBS_DIR ${CMAKE_SOURCE_DIR}/vendor/static-x64/linux)
find_library(LINUX_SDL2_LIBRARY NAMES libSDL2-2.0.so.0 SDL2-2.0 PATHS /usr/lib/x86_64-linux-gnu /lib/x86_64-linux-gnu REQUIRED)
find_library(LINUX_VULKAN_LIBRARY NAMES libvulkan.so.1 vulkan PATHS /usr/lib/x86_64-linux-gnu /lib/x86_64-linux-gnu REQUIRED)
find_library(LINUX_OPENAL_LIBRARY NAMES libopenal.so.1 openal PATHS /usr/lib/x86_64-linux-gnu /lib/x86_64-linux-gnu REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0 gthread-2.0)
target_link_libraries(DemoGame -Wl,--start-group
	${STATIC_LIBS_DIR}/bullet/libBulletSoftBody.a
	${STATIC_LIBS_DIR}/bullet/libBulletDynamics.a
	${STATIC_LIBS_DIR}/bullet/libBulletCollision.a
	${STATIC_LIBS_DIR}/bullet/libLinearMath.a
	${STATIC_LIBS_DIR}/mpi/libmpi.a
	${STATIC_LIBS_DIR}/mono/libmonosgen-2.0.a
-Wl,--end-group)
target_link_libraries(DemoGame ${GLIB_LIBRARIES})
target_link_libraries(DemoGame freetype ${LINUX_SDL2_LIBRARY} ${LINUX_VULKAN_LIBRARY} ${LINUX_OPENAL_LIBRARY})
target_link_libraries(DemoGame dl pthread m z)
ENDIF()

add_custom_command(TARGET DemoGame POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_BINARY_DIR}/Shaders"
		"$<TARGET_FILE_DIR:DemoGame>/Shaders")

set(HOUSE_OF_SHADOWS_MONO "${CMAKE_SOURCE_DIR}/MonoScripting/MonoBinaries")
foreach(MONO_RUNTIME_DIR lib etc)
	if(EXISTS "${HOUSE_OF_SHADOWS_MONO}/${MONO_RUNTIME_DIR}")
		add_custom_command(TARGET DemoGame POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				"${HOUSE_OF_SHADOWS_MONO}/${MONO_RUNTIME_DIR}"
				"$<TARGET_FILE_DIR:DemoGame>/MonoBinaries/${MONO_RUNTIME_DIR}")
	else()
		message(WARNING "House of Shadows Mono runtime directory not found: ${HOUSE_OF_SHADOWS_MONO}/${MONO_RUNTIME_DIR}")
	endif()
endforeach()

set(HOUSE_OF_SHADOWS_SOUNDS "${CMAKE_CURRENT_SOURCE_DIR}/sounds")
if(EXISTS "${HOUSE_OF_SHADOWS_SOUNDS}")
	add_custom_command(TARGET DemoGame POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${HOUSE_OF_SHADOWS_SOUNDS}"
			"$<TARGET_FILE_DIR:DemoGame>/sounds")
else()
	message(STATUS "House of Shadows sounds folder not found; built-in audio fallbacks will be used")
endif()
