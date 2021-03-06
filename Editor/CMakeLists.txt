cmake_minimum_required (VERSION 3.14)

project ("Editor")

set(CMAKE_CXX_STANDARD 17)

file(COPY "res" DESTINATION ${CMAKE_BINARY_DIR}/Editor)

file(GLOB_RECURSE HEADER_FILES "*.hpp")

set(SRC_FILES 
                Base/ImguiHelper.cpp
                Base/ImGuiWindow.cpp
                Base/Project.cpp
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
                InspectorInterfaces/EntityComponents/Terrain.cpp
                InspectorInterfaces/EntityComponents/CharacterController.cpp
                InspectorInterfaces/EntityComponents/EntityScript.cpp
                InspectorInterfaces/ResourceTypes/AudioResource.cpp
                InspectorInterfaces/ResourceTypes/TextureResource.cpp
                Menus/File_Menu.cpp
                Menus/Edit_Menu.cpp
                Menus/Windows_Menu.cpp
                Misc/AssimpMeshLoader.cpp
                Misc/ResourceImporter.cpp
                Misc/SceneExt.cpp
                Misc/Thumbnails.cpp
                Misc/TerrainThreadedEditor.cpp
                Misc/VkMaterialsThumbnails.cpp
                Misc/VkGizmos.cpp
                Misc/EditorIcons.cpp
                Misc/DialogWindows.cpp
                ProjectBuild/ResourcePacker.cpp
                Windows/BrowserWindow.cpp
                Windows/ConsoleWindow.cpp
                Windows/EditorSettingsWindow.cpp
                Windows/InspectorWindow.cpp
                Windows/ProjectSettingsWindow.cpp
                Windows/ProjectBuildingWindow.cpp
                Windows/CreateProjectWindow.cpp
                Windows/EnvironmentSettingsWindow.cpp
                Windows/ResourcePickWindow.cpp
                Windows/SceneViewWindow.cpp
                Windows/PlayerViewWindow.cpp
                Windows/SceneWindow.cpp
                Windows/StartWindow.cpp
                Windows/ToolbarWindow.cpp
                Windows/HoldOnWindow.cpp
                Windows/TextEditing/TextEditWindows.cpp
                Windows/TextEditing/TextEditWindow.cpp
                MonoScripting/MonoScriptStorage.cpp
                MonoScripting/MonoScriptCompile.cpp
                MonoScripting/MonoApiDescriptions.cpp
                main.cpp)



include_directories("./" REQUIRED)
include_directories(../Engine REQUIRED)
include_directories(../ImGui/src REQUIRED)
include_directories("../vendor/headers/bullet" REQUIRED)
include_directories("../vendor/headers/" REQUIRED)

IF(UNIX)
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK REQUIRED gtk+-3.0)
find_package(GTK REQUIRED)
include_directories(${GTK_INCLUDE_DIRS} REQUIRED)
link_directories(${GTK_LIBRARY_DIRS})
ADD_DEFINITIONS(${GTK_CFLAGS_OTHER})
ENDIF()


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

if (CMAKE_BUILD_TYPE MATCHES Release)
#bullet
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletCollision.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletDynamics.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletSoftBody.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/LinearMath.lib)
#yaml
target_link_libraries(Editor ${STATIC_LIBS_DIR}/yaml/yaml-cpp-rel.lib)
ENDIF()

if (CMAKE_BUILD_TYPE MATCHES Debug)
#bullet
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletCollision_Debug.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletDynamics_Debug.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/BulletSoftBody_Debug.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/LinearMath_Debug.lib)
#yaml
target_link_libraries(Editor ${STATIC_LIBS_DIR}/yaml/yaml-cpp-dbg.lib)
ENDIF()

#Win32 libs
target_link_libraries(Editor Winmm.lib)
target_link_libraries(Editor Setupapi.lib)
target_link_libraries(Editor Version.lib)
target_link_libraries(Editor Imm32.lib)
target_link_libraries(Editor Bcrypt.lib)
target_link_libraries(Editor ws2_32.lib)

target_link_libraries(Editor ${STATIC_LIBS_DIR}/mono/libmono-static-sgen.lib)

ENDIF()

target_link_libraries(Editor Engine)
target_link_libraries(Editor ImGui)


IF(UNIX)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/linux)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/openal/libopenal.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/SDL2/libSDL2.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/SDL2/libSDL2main.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/vulkan/vulkan-1.lib)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/freetype/libfreetype.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/assimp/libassimp.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/yaml/libyaml-cpp.a)

#bullet3 libs should be in this correct order
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/libBulletSoftBody.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/libBulletDynamics.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/libBulletCollision.a)
target_link_libraries(Editor ${STATIC_LIBS_DIR}/bullet/libLinearMath.a)



target_link_libraries(Editor sndio)
target_link_libraries(Editor ${CMAKE_DL_LIBS})
target_link_libraries(Editor pthread)
target_link_libraries(Editor ${GTK_LIBRARIES})


target_link_libraries(Editor ${STATIC_LIBS_DIR}/mono/libmonosgen-2.0.a)
target_link_libraries(Editor z)

ENDIF()