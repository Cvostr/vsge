#pragma once

#include "../VulkanBuffer.hpp"
#include <Scene/Camera.hpp>

#define MAX_CAMERAS 100
#define CAMERA_ELEM_SIZE 256
#define ENVMAP_CAMS_POS MAX_CAMERAS

namespace VSGE {
	class VulkanCamerasBuffer {
	private:
		
		VulkanBuffer* _cameras_buffer;
		Camera** _cameras;

		void WriteCameraToBuffer(uint32 index, const Mat4& projection, const Mat4& view, const Vec3& pos);
	public:
		VulkanCamerasBuffer();
		~VulkanCamerasBuffer();

		void Create();
		void Destroy();
		void SetEnvmapCameras(const Vec3& position, float far_plane);
		void SetCamera(uint32 camera_index, Camera* camera);
		uint32 GetCameraId(Camera* camera);
		Camera* GetCameraByIndex(uint32 index);
		VulkanBuffer* GetCamerasBuffer();
	};
}