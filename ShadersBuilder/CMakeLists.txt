cmake_minimum_required (VERSION 3.14)

project ("ShadersBuilder")

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES main.cpp
                            ShaderStages.cpp
							compilers/glsl_compiler.cpp
							io/FileLoader.cpp
							io/ShaderBundleWriter.cpp)

file(GLOB_RECURSE HEADER_FILES 
							compilers/glsl_compiler.hpp
							io/FileLoader.hpp
							io/ShaderBundleWriter.hpp)

include_directories("../vendor/headers/" REQUIRED)
include_directories(../Engine REQUIRED)

add_executable (compiler ${SRC_FILES} ${HEADER_FILES})
IF (WIN32)
#Static Libs linkage
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/win)

if (CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/dbg/glslangd.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/dbg/SPIRVd.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/dbg/SPVRemapperd.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/dbg/MachineIndependentd.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/dbg/OGLCompilerd.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/dbg/OSDependentd.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/dbg/GenericCodeGend.lib)
ENDIF()
if (CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/glslang.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/SPIRV.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/SPVRemapper.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/MachineIndependent.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/OGLCompiler.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/OSDependent.lib)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/GenericCodeGen.lib)
ENDIF()

ENDIF()

IF(UNIX)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/linux)

target_link_libraries(compiler ${CMAKE_DL_LIBS})
target_link_libraries(compiler pthread)

target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/libglslang.a)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/libSPIRV.a)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/libSPVRemapper.a)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/libMachineIndependent.a)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/libOGLCompiler.a)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/libOSDependent.a)
target_link_libraries(compiler ${STATIC_LIBS_DIR}/glslang/libGenericCodeGen.a)


ENDIF()