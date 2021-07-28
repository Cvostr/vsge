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
target_link_libraries(Engine ${STATIC_LIBS_DIR}/assimp/libassimp.a)
target_link_libraries(Engine ${STATIC_LIBS_DIR}/angelscript/libangelscript.a)
target_link_libraries(Engine ${STATIC_LIBS_DIR}/openal/libcommon.a)
target_link_libraries(Engine ${STATIC_LIBS_DIR}/yaml/libyaml-cpp.a)
#bullet3 libs should be in this correct order
target_link_libraries(Engine ${STATIC_LIBS_DIR}/bullet/libBulletSoftBody.a)
target_link_libraries(Engine ${STATIC_LIBS_DIR}/bullet/libBulletDynamics.a)
target_link_libraries(Engine ${STATIC_LIBS_DIR}/bullet/libBulletCollision.a)
target_link_libraries(Engine ${STATIC_LIBS_DIR}/bullet/libLinearMath.a)



target_link_libraries(Engine ${CMAKE_DL_LIBS})
target_link_libraries(Engine pthread)


ENDIF()
