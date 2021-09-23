cmake_minimum_required (VERSION 3.8)
project ("Engine")

include_directories("../vendor/headers/" REQUIRED)
include_directories("../vendor/headers/bullet" REQUIRED)
include_directories("./" REQUIRED)

set(CMAKE_CXX_STANDARD 17)

set(SRC_FILES 
				Audio/AudioBuffer.cpp
				Audio/AudioLayer.cpp
				Core/ByteSerialize.cpp
				Core/ByteSolver.cpp
				Core/FileLoader.cpp
				Core/Logger.cpp
				Core/Random.cpp
				Core/Time.cpp
				Core/YamlHelper.cpp
				Core/Threading/Mutex.cpp
				Core/Threading/Thread.cpp
				Core/VarTypes/Guid.cpp
				Core/VarTypes/Variant.cpp
				Core/VarTypes/StringHash.cpp
				Engine/Application.cpp
				Engine/Window.cpp
				Graphics/Animation.cpp
				Graphics/Bone.cpp
				Graphics/Fonts.cpp
				Graphics/GraphicsCreation.cpp
				Graphics/Material.cpp
				Graphics/Mesh.cpp
				Graphics/Renderer.cpp
				Graphics/Sampler.cpp
				Graphics/Shader.cpp
				Graphics/Texture.cpp
				Graphics/LightsBuffer.cpp
				Graphics/D3D12/D3D12Buffer.cpp
				Graphics/D3D12/D3D12Device.cpp
				Graphics/D3D12/D3D12Buffer.cpp
				Graphics/D3D12/D3D12Pipeline.cpp
				Graphics/D3D12/D3D12RAPI.cpp
				Graphics/D3D12/D3D12Sampler.cpp
				Graphics/D3D12/D3D12SwapChain.cpp
				Graphics/D3D12/D3D12Texture.cpp
				Graphics/Vulkan/VulkanBuffer.cpp
				Graphics/Vulkan/VulkanCommandBuffer.cpp
				Graphics/Vulkan/VulkanDescriptors.cpp
				Graphics/Vulkan/VulkanDevice.cpp
				Graphics/Vulkan/VulkanFramebuffer.cpp
				Graphics/Vulkan/VulkanInstance.cpp
				Graphics/Vulkan/VulkanMA.cpp
				Graphics/Vulkan/VulkanMesh.cpp
				Graphics/Vulkan/VulkanPipeline.cpp
				Graphics/Vulkan/VulkanPipelineLayout.cpp
				Graphics/Vulkan/VulkanRAPI.cpp
				Graphics/Vulkan/VulkanRenderpass.cpp
				Graphics/Vulkan/VulkanSampler.cpp
				Graphics/Vulkan/VulkanShader.cpp
				Graphics/Vulkan/VulkanSwapchain.cpp
				Graphics/Vulkan/VulkanSynchronization.cpp
				Graphics/Vulkan/VulkanTexture.cpp
				Graphics/Vulkan/VulkanComputePipeline.cpp
				Graphics/Vulkan/Rendering/VulkanMaterials.cpp
				Graphics/Vulkan/Rendering/VulkanRenderer.cpp
				Graphics/Vulkan/Rendering/VulkanDeferredLight.cpp
				Graphics/Vulkan/Rendering/VulkanGBufferRenderer.cpp
				Graphics/Vulkan/Rendering/VulkanShadowmapping.cpp
				Graphics/Vulkan/Rendering/VulkanEnvMap.cpp
				Graphics/Vulkan/Rendering/VulkanTerrainRenderer.cpp
				Graphics/Vulkan/Rendering/VulkanCamerasBuffer.cpp
				Graphics/Vulkan/Rendering/PBR/Vulkan_BRDF_LUT.cpp
				Graphics/Vulkan/Rendering/PBR/VulkanIrradianceMap.cpp
				Graphics/Vulkan/Rendering/PBR/VulkanSpecularMap.cpp
				Graphics/Vulkan/Rendering/PBR/VulkanIBL.cpp
				Math/AABB.cpp
				Math/Frustum.cpp
				Math/Mat4.cpp
				Math/MathBase.cpp
				Math/MatrixCamera.cpp
				Math/MatrixTransform.cpp
				Math/Plane.cpp
				Math/Quaternion.cpp
				Math/Ray.cpp
				Math/Vertex.cpp
				Physics/PhysicsLayer.cpp
				Resources/AnimationParser.cpp
				Resources/TextureProcessor.cpp
				Resources/AsyncLoader.cpp
				Resources/ResourcesWatchdog.cpp
				Resources/DefaultResources.cpp
				Resources/ModelParser.cpp
				Resources/Resource.cpp
				Resources/ResourceReference.cpp
				Resources/ResourceCache.cpp
				Resources/ResourceLayer.cpp
				Resources/ResourceTypes/AudioClipResource.cpp
				Resources/ResourceTypes/MaterialResource.cpp
				Resources/ResourceTypes/MeshResource.cpp
				Resources/ResourceTypes/TextureResource.cpp
				Resources/ResourceTypes/AnimationResource.cpp
				Scene/Camera.cpp
				Scene/Entity.cpp
				Scene/IEntityComponent.cpp
				Scene/Scene.cpp
				Scene/SceneEnvironment.cpp
				Scene/SceneLayer.cpp
				Scene/SceneSerialization.cpp
				Scene/SceneSerializationBinary.cpp
				Scene/EntityComponents/AnimatorComponent.cpp
				Scene/EntityComponents/AudioListenerComponent.cpp
				Scene/EntityComponents/AudioSourceComponent.cpp
				Scene/EntityComponents/ColliderComponent.cpp
				Scene/EntityComponents/EntityScriptComponent.cpp
				Scene/EntityComponents/LightComponent.cpp
				Scene/EntityComponents/MaterialComponent.cpp
				Scene/EntityComponents/MeshComponent.cpp
				Scene/EntityComponents/ParticleEmitterComponent.cpp
				Scene/EntityComponents/RigidBodyComponent.cpp
				Scene/EntityComponents/TerrainComponent.cpp
				MonoScripting/MonoScriptingLayer.cpp
				MonoScripting/MonoScript.cpp
				MonoScripting/MonoScriptBlob.cpp
				MonoScripting/Api/ApiLogger.cpp
				MonoScripting/Api/ApiEntity.cpp
				MonoScripting/Api/ApiScene.cpp
				MonoScripting/Api/ApiResource.cpp
				System/FileWatcher.cpp
				System/PlatformSpecific.cpp
				System/Shell.cpp
				)

file(GLOB_RECURSE HEADER_FILES "*.hpp")

add_library (Engine ${SRC_FILES} ${HEADER_FILES})

add_dependencies(Engine shaders)
#add_dependencies(Engine vsge_api)

IF(UNIX)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/linux)


ENDIF()
