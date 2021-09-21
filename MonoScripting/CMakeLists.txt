cmake_minimum_required (VERSION 3.14)

project ("MonoScripting")

set(API_SRC_FILES 
    Api/Vec2.cs
    Api/Vec3.cs
    Api/Vec4.cs
    Api/Quat.cs
    Api/AABB.cs
    Api/Color.cs)

set(COMPILER_CMD "A")

add_custom_target(
    vsge_api
    DEPENDS ${API_SRC_FILES})

add_custom_command(
  COMMAND "${COMPILER_CMD} ${API_SRC_FILES}"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/MonoScripting
  DEPENDS ${API_SRC_FILES}
  TARGET vsge_api
  VERBATIM
)