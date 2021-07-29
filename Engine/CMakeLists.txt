cmake_minimum_required (VERSION 3.8)
project ("Engine")

set(HEADER_DIR "")
set(SRC_DIR "")

include_directories("../vendor/headers/" REQUIRED)
include_directories("../vendor/headers/bullet" REQUIRED)

include_directories("./" REQUIRED)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.hpp")

add_library (Engine ${SRC_FILES} ${HEADER_FILES})

add_dependencies(Engine shaders)

IF(UNIX)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/linux)


ENDIF()
