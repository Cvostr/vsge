cmake_minimum_required (VERSION 3.14)

project ("EngineApplication")


set(CMAKE_CXX_STANDARD 17)

include_directories(../Engine REQUIRED)
include_directories("../vendor/headers/bullet" REQUIRED)
include_directories("../vendor/headers/" REQUIRED)

set(SRC_FILES 
                main.cpp)

set(HEADER_FILES 
              )

add_executable (EngineApplication ${SRC_FILES} ${HEADER_FILES})

target_link_libraries(EngineApplication Engine)	


IF (WIN32)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/win)
#Static Libs linkage
#copy assimp shared lib
file(COPY ${STATIC_LIBS_DIR}/assimp/assimp-vc142-mt.dll DESTINATION ${CMAKE_BINARY_DIR}/EngineApplication)
file(COPY ${STATIC_LIBS_DIR}/openal/OpenAL32.dll DESTINATION ${CMAKE_BINARY_DIR}/EngineApplication)

target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/SDL2/SDL2.lib)
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/SDL2/SDL2main.lib)
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/vulkan/vulkan-1.lib)
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/freetype/freetype.lib)
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/assimp/assimp.lib)
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/openal/OpenAL32.lib)

if (CMAKE_BUILD_TYPE MATCHES Release)
#bullet
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/bullet/BulletCollision.lib)
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/bullet/BulletDynamics.lib)
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/bullet/BulletSoftBody.lib)
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/bullet/LinearMath.lib)
#yaml
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/yaml/yaml-cpp-rel.lib)
ENDIF()

if (CMAKE_BUILD_TYPE MATCHES Debug)
#bullet
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/bullet/BulletCollision_Debug.lib)
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/bullet/BulletDynamics_Debug.lib)
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/bullet/BulletSoftBody_Debug.lib)
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/bullet/LinearMath_Debug.lib)
#yaml
target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/yaml/yaml-cpp-dbg.lib)
ENDIF()

#Win32 libs
target_link_libraries(EngineApplication Winmm.lib)
target_link_libraries(EngineApplication Setupapi.lib)
target_link_libraries(EngineApplication Version.lib)
target_link_libraries(EngineApplication Imm32.lib)
target_link_libraries(EngineApplication Bcrypt.lib)
target_link_libraries(EngineApplication ws2_32.lib)

target_link_libraries(EngineApplication ${STATIC_LIBS_DIR}/mono/libmono-static-sgen.lib)

ENDIF()

target_link_libraries(EngineApplication Engine)