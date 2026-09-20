cmake_minimum_required (VERSION 3.11)
project ("bc7enc")

set(CMAKE_CXX_STANDARD 17)

set(SRC_FILES 
	bc7enc.cpp
	rgbcx.cpp
)

set(HEADER_FILES 
	bc7enc.h
	rgbcx.h
)

add_library (bc7enc ${SRC_FILES} ${HEADER_FILES})
