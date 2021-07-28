cmake_minimum_required (VERSION 3.14)

project ("Shaders")

set(SHADERS_FILES "")
set(COUTPUT "")
file(GLOB_RECURSE SHADERS_FILES "*.vert" "*.frag")

add_custom_target(
    shaders
    DEPENDS ${SHADERS_FILES})

IF(UNIX)
set(COMPILER, "compiler")
endif()

if(WIN32)
set(COMPILER, "compiler.exe")
endif()

add_custom_command(OUTPUT ${COUTPUT}
  COMMAND ${COMPILER}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Shaders
  DEPENDS ${SHADERS_FILES}
  TARGET shaders
  VERBATIM
)
