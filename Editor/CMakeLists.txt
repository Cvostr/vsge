cmake_minimum_required (VERSION 3.14)

project ("Editor")

set(CMAKE_CXX_STANDARD 17)

file(COPY "res" DESTINATION ${CMAKE_BINARY_DIR}/Editor)

#file(GLOB_RECURSE SRC_FILES "*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.hpp")

set(SRC_FILES 
                Base/ImguiHelper.cpp
                Base/ImGuiWindow.cpp
                Base/PlatformSpecific.cpp
                EditorLayers/EditorLayer.cpp
                EditorLayers/EditorSettingsLayer.cpp
                EditorLayers/ImGuiLayer.cpp
                InspectorInterfaces/MaterialEditor.cpp
                InspectorInterfaces/ResourcePicker.cpp
                InspectorInterfaces/ScriptPicker.cpp
                InspectorInterfaces/VariantInput.cpp
                InspectorInterfaces/ViewMaskEdit.cpp
                InspectorInterfaces/EntityComponents/AnimatorComponent.cpp
                InspectorInterfaces/EntityComponents/AudioSource.cpp
                InspectorInterfaces/EntityComponents/Camera.cpp
                InspectorInterfaces/EntityComponents/Collider.cpp
                InspectorInterfaces/EntityComponents/Lightsource.cpp
                InspectorInterfaces/EntityComponents/MaterialComponent.cpp
                InspectorInterfaces/EntityComponents/MeshComponent.cpp
                InspectorInterfaces/EntityComponents/ParticleEmitter.cpp
                InspectorInterfaces/EntityComponents/RigidBody.cpp
                InspectorInterfaces/EntityComponents/ScriptComponent.cpp
                Menus/File_Menu.cpp
                Menus/Edit_Menu.cpp
                Menus/Windows_Menu.cpp
                Misc/AssimpMeshLoader.cpp
                Misc/ResourceImporter.cpp
                Misc/SceneExt.cpp
                ProjectBuild/ResourcePacker.cpp
                Windows/BrowserWindow.cpp
                Windows/ConsoleWindow.cpp
                Windows/EditorSettingsWindow.cpp
                Windows/InspectorWindow.cpp
                Windows/ProjectSettingsWindow.cpp
                Windows/EnvironmentSettingsWindow.cpp
                Windows/ResourcePickWindow.cpp
                Windows/SceneViewWindow.cpp
                Windows/SceneWindow.cpp
                Windows/StartWindow.cpp
                Windows/ToolbarWindow.cpp
                main.cpp)

include_directories(../Engine REQUIRED)
include_directories(../ImGui/src REQUIRED)
include_directories("../vendor/headers/bullet" REQUIRED)
include_directories("../vendor/headers/" REQUIRED)

add_executable (Editor ${SRC_FILES} ${HEADER_FILES})
IF (WIN32)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/win)
#Static Libs linkage
#copy assimp shared lib
file(COPY ${STATIC_LIBS_DIR}/assimp/assimp-vc142-mt.dll DESTINATION ${CMAKE_BINARY_DIR}/Editor)
file(COPY ${STATIC_LIBS_DIR}/openal/OpenAL32.dll DESTINATION ${CMAKE_BINARY_DIR}/Editor)

target_link_libraries(Editor ${STATIC_LIBS_DIR}/SDL2/SDL2.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/SDL2/SDL2main.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/vulkan/vulkan-1.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/freetype/freetype.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/assimp/assimp.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/openal/OpenAL32.lib)

target_link_libraries(Editor ${STATIC_LIBS_DIR}/yaml/yaml-cpp.lib)

#bullet
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletCollision_Debug.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletDynamics_Debug.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletSoftBody_Debug.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/LinearMath_Debug.lib)

#Win32 libs
target_link_libraries(Editor Winmm.lib)
target_link_libraries(Editor Setupapi.lib)
target_link_libraries(Editor Version.lib)
target_link_libraries(Editor Imm32.lib)

target_link_libraries(Editor ${STATIC_LIBS_DIR}/angelscript/angelscript64d.lib)

ENDIF()

target_link_libraries(Editor Engine)
target_link_libraries(Editor ImGui)


IF(UNIX)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/linux)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/angelscript/libangelscript.a)
target_link_libraries(Engine ${STATIC_LIBS_DIR}/openal/libopenal.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/SDL2/libSDL2.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/SDL2/libSDL2main.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/vulkan/vulkan-1.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/freetype/libfreetype.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/assimp/libassimp.a)
target_link_libraries(Engine ${STATIC_LIBS_DIR}/yaml/libyaml-cpp.a)

#bullet3 libs should be in this correct order
target_link_libraries(Engine ${STATIC_LIBS_DIR}/bullet/libBulletSoftBody.a)
target_link_libraries(Engine ${STATIC_LIBS_DIR}/bullet/libBulletDynamics.a)
target_link_libraries(Engine ${STATIC_LIBS_DIR}/bullet/libBulletCollision.a)
target_link_libraries(Engine ${STATIC_LIBS_DIR}/bullet/libLinearMath.a)

target_link_libraries(Engine sndio)
target_link_libraries(Editor ${CMAKE_DL_LIBS})
target_link_libraries(Editor pthread)


ENDIF()



