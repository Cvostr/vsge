cmake_minimum_required (VERSION 3.14)

project ("MonoScripting")

set(API_SRC_FILES 
    Api/Vec2.cs
    Api/Vec3.cs
    Api/Vec4.cs
    Api/Quat.cs
    Api/AABB.cs
    Api/Rect.cs
    Api/Color.cs
    Api/InternalObject.cs
    Api/EntityScript.cs
    Api/Input.cs
    Api/Logger.cs
    Api/Application.cs
    Api/Screen.cs
    Api/Scenes.cs
    Api/EngineClass/Entity.cs
    Api/EngineClass/Scene.cs
    Api/EngineClass/Resource.cs
    Api/EngineClass/Material.cs
    Api/EngineClass/IEntityComponent.cs
    Api/EngineClass/EntityComponents/MaterialComponent.cs
    Api/EngineClass/EntityComponents/MeshComponent.cs
    Api/EngineClass/EntityComponents/LightComponent.cs
    Api/EngineClass/EntityComponents/AudioSourceComponent.cs
    Api/EngineClass/EntityComponents/RigidBodyComponent.cs
    Api/EngineClass/EntityComponents/ColliderComponent.cs
    Api/EngineClass/EntityComponents/CameraComponent.cs
    Api/EngineClass/EntityComponents/AnimatorComponent.cs
    Api/EngineClass/EntityComponents/CharacterControllerComponent.cs
    Api/EngineClass/EntityComponents/ParticleEmitterComponent.cs
    Api/EngineClass/EntityComponents/TerrainComponent.cs
    Api/EngineClass/UI/UiRenderList.cs
    Api/EngineClass/Networking/GameServer.cs
    Api/EngineClass/Networking/GameClient.cs
    Api/EngineClass/Networking/GameNetworking.cs)

set(COMPILER_ARGS -target:library -nologo -out:${CMAKE_CURRENT_BINARY_DIR}/api.dll)

IF(WINDOWS)
set(COMPILER_CMD "MonoBinaries/bin/csc.bat")
ENDIF()

IF(UNIX)
set(COMPILER_CMD "csc")
ENDIF()


add_custom_target(
    vsge_api
    DEPENDS ${API_SRC_FILES})

add_custom_command(
  COMMAND ${COMPILER_CMD} ${COMPILER_ARGS} ${API_SRC_FILES}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/MonoScripting
  DEPENDS ${API_SRC_FILES}
  TARGET vsge_api
  VERBATIM
)