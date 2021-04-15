cmake_minimum_required (VERSION 3.8)

project ("Editor")

#file(COPY "res" DESTINATION ${CMAKE_BINARY_DIR}/CommonEditor)

set(SRC_DIR "")
set(HEADER_DIR "")

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(GLEW_STATIC)

file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.hpp")

include_directories(../Engine REQUIRED)
include_directories(../ImGui/src REQUIRED)
include_directories("../vendor/headers/" REQUIRED)

add_executable (Editor ${SRC_FILES} ${HEADER_FILES})

#Static Libs linkage
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/win)

target_link_libraries(Editor ${STATIC_LIBS_DIR}/SDL2/SDL2.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/SDL2/SDL2main.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/vulkan/vulkan-1.lib)

target_link_libraries(Editor ${STATIC_LIBS_DIR}/glew/glew32s.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/glew/glew32.lib)

target_link_libraries(Editor OpenGL32.lib)

#target_link_libraries(Editor ${STATIC_LIBS_DIR}/dx11/d3d11.lib)
#target_link_libraries(Editor ${STATIC_LIBS_DIR}/dx11/d3dx11.lib)

#Win32 libs
target_link_libraries(Editor winmm.lib)
target_link_libraries(Editor Setupapi.lib)
target_link_libraries(Editor Version.lib)
#target_link_libraries(Editor Imm32.lib)
target_link_libraries(Editor Engine)	
target_link_libraries(Editor ImGui)	