cmake_minimum_required (VERSION 3.14)

project ("Editor")

set(SRC_DIR "")
set(HEADER_DIR "")

set(CMAKE_CXX_STANDARD 17)

file(COPY "res" DESTINATION ${CMAKE_BINARY_DIR}/Editor)

file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.hpp")

include_directories(../Engine REQUIRED)
include_directories(../ImGui/src REQUIRED)
include_directories("../vendor/headers/bullet" REQUIRED)
include_directories("../vendor/headers/" REQUIRED)

add_executable (Editor ${SRC_FILES} ${HEADER_FILES})
IF (WIN32)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/win)
#Static Libs linkage
#copy assimp shared lib
file(COPY ${STATIC_LIBS_DIR}/assimp/assimp-vc142-mt.dll DESTINATION ${CMAKE_BINARY_DIR}/Editor)
file(COPY ${STATIC_LIBS_DIR}/openal/OpenAL32.dll DESTINATION ${CMAKE_BINARY_DIR}/Editor)

target_link_libraries(Editor ${STATIC_LIBS_DIR}/SDL2/SDL2.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/SDL2/SDL2main.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/vulkan/vulkan-1.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/freetype/freetype.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/assimp/assimp.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/openal/OpenAL32.lib)

target_link_libraries(Editor ${STATIC_LIBS_DIR}/yaml/yaml-cpp.lib)

#bullet
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletCollision_Debug.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletDynamics_Debug.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletSoftBody_Debug.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/LinearMath_Debug.lib)

#Win32 libs
target_link_libraries(Editor Winmm.lib)
target_link_libraries(Editor Setupapi.lib)
target_link_libraries(Editor Version.lib)
target_link_libraries(Editor Imm32.lib)
target_link_libraries(Editor Bcrypt.lib)
target_link_libraries(Editor ws2_32.lib)

target_link_libraries(Editor ${STATIC_LIBS_DIR}/mono/libmono-static-sgen.lib)

ENDIF()

IF(UNIX)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/linux)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/glew/libGLEW.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/vulkan/vulkan-1.lib)

target_link_libraries(Editor ${CMAKE_DL_LIBS})
target_link_libraries(Editor pthread)


ENDIF()

target_link_libraries(Editor Engine)	
target_link_libraries(Editor ImGui)