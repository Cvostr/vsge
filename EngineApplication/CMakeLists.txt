cmake_minimum_required (VERSION 3.14)

project ("EngineApplication")


set(CMAKE_CXX_STANDARD 17)

include_directories(../Engine REQUIRED)
include_directories("../vendor/headers/bullet" REQUIRED)
include_directories("../vendor/headers/" REQUIRED)

#add_executable (EngineApplication ${SRC_FILES} ${HEADER_FILES})

#target_link_libraries(EngineApplication Engine)	
