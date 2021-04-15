cmake_minimum_required (VERSION 3.8)
project ("Engine")

set(HEADER_DIR "")
set(SRC_DIR "")

add_compile_definitions(GLEW_STATIC)

include_directories("../vendor/headers/" REQUIRED)
include_directories("./" REQUIRED)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.hpp")

add_library (Engine ${SRC_FILES} ${HEADER_FILES})
