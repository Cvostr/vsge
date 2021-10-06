#pragma once

#include <Core/VarTypes/Base.hpp>
#include <Graphics/GpuBuffer.hpp>
#include <Scene/EntityComponents/LightComponent.hpp>
#include <Math/Color.hpp>

namespace VSGE {
	class LightsBuffer {
	private:
		byte* _cpu_buffer;
		GpuBuffer* _lights_buffer;
		uint32 _max_lights_count;

		uint32 GetNeededBufferSize();
	public:

		LightsBuffer();
		~LightsBuffer();

		void Create();
		void Destroy();

		void UpdateGpuBuffer();
		void SetLight(uint32 light_index, LightsourceComponent* light);
		void SetLight(uint32 light_index, int type, float intensity, float range, float spot_angle, const Vec3& pos, const Vec3& dir, const Color& color);
		void SetLightsCount(uint32 lights_count);
		void SetAmbientColor(const Color& color);

		void SetMaxLightsCount(uint32 lights_count);

		GpuBuffer* GetLightsGpuBuffer();
	};
}