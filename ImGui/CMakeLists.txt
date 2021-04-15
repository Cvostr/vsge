cmake_minimum_required (VERSION 3.8)
project ("ImGui")

set(SRC_DIR "src")

include_directories("../vendor/headers/" REQUIRED)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.h")

add_library (ImGui ${SRC_FILES} ${HEADER_FILES})
