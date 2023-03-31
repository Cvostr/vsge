cmake_minimum_required (VERSION 3.8)
project ("Engine")

include_directories("../vendor/headers/" REQUIRED)
include_directories("../vendor/headers/bullet" REQUIRED)
include_directories("./" REQUIRED)
include_directories("../vendor/headers/mpi" REQUIRED)

set(CMAKE_CXX_STANDARD 17)

set(SRC_FILES 
				Audio/AudioBuffer.cpp
				Audio/AudioLayer.cpp
				Core/ByteSerialize.cpp
				Core/ByteSolver.cpp
				Core/FileLoader.cpp
				Core/Logger.cpp
				Core/Time.cpp
				Core/Threading/Thread.cpp
				Core/VarTypes/String.cpp
				Core/VarTypes/Variant.cpp
				Core/VarTypes/StringHash.cpp
				Engine/Application.cpp
				Engine/Window.cpp
				Graphics/Animation.cpp
				Graphics/AnimationChannel.cpp
				Graphics/Bone.cpp
				Graphics/Fonts.cpp
				Graphics/GraphicsCreation.cpp
				Graphics/Material.cpp
				Graphics/MaterialTemplate.cpp
				Graphics/MaterialTemplateCache.cpp
				Graphics/Mesh.cpp
				Graphics/Renderer.cpp
				Graphics/Sampler.cpp
				Graphics/Shader.cpp
				Graphics/Texture.cpp
				Graphics/LightsBuffer.cpp
				Graphics/Vulkan/VulkanBuffer.cpp
				Graphics/Vulkan/VulkanCommandBuffer.cpp
				Graphics/Vulkan/VulkanDescriptors.cpp
				Graphics/Vulkan/VulkanQueue.cpp
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
				Graphics/Vulkan/VulkanPresenter.cpp
				Graphics/Vulkan/Rendering/VulkanMaterials.cpp
				Graphics/Vulkan/Rendering/VulkanRenderer.cpp
				Graphics/Vulkan/Rendering/VulkanRenderTarget.cpp
				Graphics/Vulkan/Rendering/VulkanDeferredLight.cpp
				Graphics/Vulkan/Rendering/VulkanGBufferRenderer.cpp
				Graphics/Vulkan/Rendering/VulkanShadowmapping.cpp
				Graphics/Vulkan/Rendering/VulkanEnvMap.cpp
				Graphics/Vulkan/Rendering/VulkanTerrainRenderer.cpp
				Graphics/Vulkan/Rendering/VulkanCamerasBuffer.cpp
				Graphics/Vulkan/Rendering/VulkanUiRenderer.cpp
				Graphics/Vulkan/Rendering/VulkanFinalPass.cpp
				Graphics/Vulkan/Rendering/PBR/Vulkan_BRDF_LUT.cpp
				Graphics/Vulkan/Rendering/PBR/VulkanIrradianceMap.cpp
				Graphics/Vulkan/Rendering/PBR/VulkanSpecularMap.cpp
				Graphics/Vulkan/Rendering/PBR/VulkanIBL.cpp
				Graphics/Vulkan/Rendering/Postprocess/VulkanGammaCorrection.cpp
				Graphics/Vulkan/Rendering/Postprocess/VulkanBloom.cpp
				Graphics/Vulkan/Rendering/Postprocess/VulkanSSAO.cpp
				Input/Input.cpp
				Input/InputLayer.cpp
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
				Math/Rect.cpp
				Physics/PhysicsLayer.cpp
				Physics/PhysicsWorld.cpp
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
				Resources/ResourceTypes/SceneResource.cpp
				Scene/Camera.cpp
				Scene/Entity.cpp
				Scene/IEntityComponent.cpp
				Scene/Scene.cpp
				Scene/SceneEnvironment.cpp
				Scene/SceneLayer.cpp
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
				Scene/EntityComponents/CharacterControllerComponent.cpp
				MonoScripting/MonoScriptingLayer.cpp
				MonoScripting/MonoScript.cpp
				MonoScripting/MonoScriptBlob.cpp
				MonoScripting/MonoClassDesc.cpp
				MonoScripting/MonoClassFieldDesc.cpp
				MonoScripting/MonoEventDesc.cpp
				MonoScripting/MonoMethodDesc.cpp
				MonoScripting/Api/ApiLogger.cpp
				MonoScripting/Api/ApiApplication.cpp
				MonoScripting/Api/ApiMaterial.cpp
				MonoScripting/Api/ApiEntity.cpp
				MonoScripting/Api/ApiScene.cpp
				MonoScripting/Api/ApiScenes.cpp
				MonoScripting/Api/ApiResource.cpp
				MonoScripting/Api/ApiInput.cpp
				MonoScripting/Api/ApiEntityComponentsApi.cpp
				MonoScripting/Api/EntityComponents/LightsourceComponent.cpp
				MonoScripting/Api/EntityComponents/AudioSourceComponent.cpp
				MonoScripting/Api/EntityComponents/AnimatorComponent.cpp
				MonoScripting/Api/EntityComponents/RigidBodyComponent.cpp
				MonoScripting/Api/EntityComponents/ColliderComponent.cpp
				MonoScripting/Api/EntityComponents/CameraComponent.cpp
				MonoScripting/Api/EntityComponents/MaterialComponent.cpp
				MonoScripting/Api/EntityComponents/MeshComponent.cpp
				MonoScripting/Api/EntityComponents/CharacterControllerComponent.cpp
				MonoScripting/Api/EntityComponents/ParticleEmitterComponent.cpp
				MonoScripting/Api/UI/UiRenderList.cpp
				UI/UiLayer.cpp
				UI/View.cpp
				UI/UiRenderList.cpp
				UI/Widgets/Button.cpp
				UI/Widgets/ImageView.cpp
				UI/Widgets/TextView.cpp
				System/PlatformSpecific.cpp
				)

set(HEADER1_FILES	Audio/AudioBuffer.hpp
					Audio/AudioLayer.hpp
					Engine/Application.hpp
					Engine/ApplicationLayer.hpp
					Engine/Event.hpp
					Engine/Window.hpp
					Engine/WindowEvent.hpp

					Graphics/Animation.hpp
					Graphics/AnimationChannel.hpp
					Graphics/Bone.hpp
					Graphics/Fonts.hpp
					Graphics/Framebuffer.hpp
					Graphics/GpuBuffer.hpp
					Graphics/GpuObject.hpp
					Graphics/GraphicsPipeline.hpp
					Graphics/LightsBuffer.hpp
					Graphics/Material.hpp
					Graphics/MaterialTemplate.hpp
					Graphics/MaterialTemplateCache.hpp
					Graphics/MaterialDescriptor.hpp
					Graphics/Mesh.hpp
					Graphics/PostprocessEffect.hpp
					Graphics/Renderer.hpp
					Graphics/RenderingAPI.hpp
					Graphics/RenderTarget.hpp
					Graphics/Sampler.hpp
					Graphics/Shader.hpp
					Graphics/Texture.hpp
					Graphics/TextureFormat.hpp
					Graphics/VertexLayout.hpp

					Graphics/Vulkan/VulkanQueue.hpp
					Graphics/Vulkan/VulkanBuffer.hpp
					Graphics/Vulkan/VulkanCommandBuffer.hpp
					Graphics/Vulkan/VulkanComputePipeline.hpp
					Graphics/Vulkan/VulkanDescriptors.hpp
					Graphics/Vulkan/VulkanDevice.hpp
					Graphics/Vulkan/VulkanFramebuffer.hpp
					Graphics/Vulkan/VulkanInstance.hpp
					Graphics/Vulkan/VulkanMA.hpp
					Graphics/Vulkan/VulkanMesh.hpp
					Graphics/Vulkan/VulkanPipeline.hpp
					Graphics/Vulkan/VulkanPipelineLayout.hpp
					Graphics/Vulkan/VulkanPresenter.hpp
					Graphics/Vulkan/VulkanRAPI.hpp
					Graphics/Vulkan/VulkanRenderpass.hpp
					Graphics/Vulkan/VulkanSampler.hpp
					Graphics/Vulkan/VulkanShader.hpp
					Graphics/Vulkan/VulkanSwapchain.hpp
					Graphics/Vulkan/VulkanTexture.hpp
					Graphics/Vulkan/VulkanSynchronization.hpp
	
					Core/VarTypes/Base.hpp
					Core/VarTypes/String.hpp
					Core/VarTypes/StringHash.hpp
					Core/VarTypes/Variant.hpp

					Resources/Resource.hpp
					Resources/ResourceCache.hpp
					Resources/ResourceLayer.hpp
					Resources/AsyncLoader.hpp

					Scene/Scene.hpp
					Scene/Entity.hpp
					Scene/IEntityComponent.hpp
					Scene/Camera.hpp
					
					)

add_library (Engine ${SRC_FILES} ${HEADER1_FILES})

add_dependencies(Engine shaders)
add_dependencies(Engine vsge_api)

IF(UNIX)
set(STATIC_LIBS_DIR  ${CMAKE_SOURCE_DIR}/vendor/static-x64/linux)


ENDIF()