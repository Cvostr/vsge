cmake_minimum_required (VERSION 3.14)

project ("EngineTest")

file(COPY "res" DESTINATION ${CMAKE_BINARY_DIR}/EngineTest)
file(COPY "shaders" DESTINATION ${CMAKE_BINARY_DIR}/EngineTest)

set(SRC_DIR "")
set(HEADER_DIR "")

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(GLEW_STATIC)

file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.hpp")

include_directories(../Engine REQUIRED)
include_directories("../vendor/headers/" REQUIRED)

add_executable (EngineTest ${SRC_FILES} ${HEADER_FILES})
IF (WIN32)
#Static Libs linkage
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/win)

target_link_libraries(EngineTest ${STATIC_LIBS_DIR}/SDL2/SDL2.lib)
target_link_libraries(EngineTest ${STATIC_LIBS_DIR}/SDL2/SDL2main.lib)
target_link_libraries(EngineTest ${STATIC_LIBS_DIR}/vulkan/vulkan-1.lib)
target_link_libraries(EngineTest ${STATIC_LIBS_DIR}/freetype/freetype.lib)


#Win32 libs
target_link_libraries(EngineTest winmm.lib)
target_link_libraries(EngineTest Setupapi.lib)
target_link_libraries(EngineTest Version.lib)
target_link_libraries(EngineTest Imm32.lib)

target_link_libraries(EngineTest d3d12.lib)
target_link_libraries(EngineTest dxgi.lib)
#target_link_libraries(EngineTest d3dcompiler.lib)

ENDIF()

IF(UNIX)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/linux)
target_link_libraries(EngineTest ${STATIC_LIBS_DIR}/SDL2/libSDL2.a)
target_link_libraries(EngineTest ${STATIC_LIBS_DIR}/vulkan/vulkan-1.lib)

target_link_libraries(EngineTest ${CMAKE_DL_LIBS})
target_link_libraries(EngineTest pthread)


ENDIF()

target_link_libraries(EngineTest Engine)	
