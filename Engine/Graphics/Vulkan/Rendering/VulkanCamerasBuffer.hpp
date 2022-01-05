#pragma once

#include "../VulkanBuffer.hpp"
#include <Scene/Camera.hpp>

#define MAX_CAMERAS 100
#define CAMERA_ELEM_SIZE 256
#define ENVMAP_CAMS_POS MAX_CAMERAS

namespace VSGE {
	class VulkanCamerasBuffer {
	private:
		
		byte* _cameras_cpu_buffer;
		VulkanBuffer* _cameras_buffer;
		Camera** _cameras;

		uint32 GetNeededBufferSize();
		void WriteCameraToBuffer(uint32 index, const Mat4& projection, const Mat4& view, const Vec3& pos);
	public:
		VulkanCamerasBuffer();
		~VulkanCamerasBuffer();

		void Create();
		void Destroy();
		void SetEnvmapCameras(const Vec3& position, float far_plane);
		void SetCamera(uint32 camera_index, Camera* camera);
		void SetAllCamerasNull();
		uint32 GetCameraId(Camera* camera);
		uint32 GetCamerasCount();
		Camera* GetCameraByIndex(uint32 index);
		VulkanBuffer* GetCamerasBuffer();
		void UpdateGpuBuffer();
	};
}