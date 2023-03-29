cmake_minimum_required (VERSION 3.8)
project ("ImGui")

set(SRC_DIR "src")

include_directories("../vendor/headers/" REQUIRED)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES "*.cpp")

set(SRC_FILES 
	src/ImageBtnText.cpp
	src/imgui.cpp
	src/imgui_demo.cpp
	src/imgui_draw.cpp
	src/imgui_impl_sdl.cpp
	src/imgui_impl_vulkan.cpp
	src/imgui_stdlib.cpp
	src/imgui_tables.cpp
	src/imgui_widgets.cpp
	src/ImGuizmo.cpp
)

set(HEADER_FILES 
	src/ImageBtnText.h
	src/imconfig.h
	src/imgui.h
	src/imgui_impl_vulkan.h
	src/imgui_internal.h
	src/imgui_stdlib.h
	src/ImGuizmo.h
	src/imstb_rectpack.h
	src/imstb_textedit.h
	src/imstb_truetype.h
)

add_library (ImGui ${SRC_FILES} ${HEADER_FILES})
