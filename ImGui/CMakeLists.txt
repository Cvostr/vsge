cmake_minimum_required (VERSION 3.8)
project ("ImGui")

set(SRC_DIR "src")

include_directories("../vendor/headers/" REQUIRED)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.h")

add_library (ImGui ${SRC_FILES} ${HEADER_FILES})

IF(UNIX)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/linux)
target_link_libraries(ImGui ${STATIC_LIBS_DIR}/glew/libGLEW.a)

target_link_libraries(ImGui GL)
target_link_libraries(ImGui ${GLEW_LIBRARIES})

ENDIF()
