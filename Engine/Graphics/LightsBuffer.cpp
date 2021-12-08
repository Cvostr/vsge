#include "LightsBuffer.hpp"
#include <Scene/Entity.hpp>
#include <Core/Memory.hpp>

using namespace VSGE;

LightsBuffer::LightsBuffer() {
	_max_lights_count = 200;
}
LightsBuffer::~LightsBuffer() {
	Destroy();
}

uint32 LightsBuffer::GetNeededBufferSize() {
	return 16 + _max_lights_count * 64;
}

void LightsBuffer::Create() {
	_cpu_buffer = new byte[GetNeededBufferSize()];
	_lights_buffer = CreateGpuBuffer(GPU_BUFFER_TYPE_UNIFORM);
	_lights_buffer->Create(GetNeededBufferSize());
}
void LightsBuffer::Destroy() {
	SAFE_RELEASE_ARR(_cpu_buffer)
	SAFE_RELEASE(_lights_buffer)
}

void LightsBuffer::SetMaxLightsCount(uint32 lights_count) {
	_max_lights_count = lights_count;
}

void LightsBuffer::UpdateGpuBuffer() {
	_lights_buffer->WriteData(0, GetNeededBufferSize(), _cpu_buffer);
}

void LightsBuffer::SetLightsCount(uint32 lights_count) {
	Memcpy(_cpu_buffer + 12, &lights_count, sizeof(uint32));
}

void LightsBuffer::SetAmbientColor(const Color& color) {
	Memcpy(_cpu_buffer, &color.r, 12);
}

void LightsBuffer::SetLight(uint32 light_index, LightsourceComponent* light) {
	SetLight(light_index,
		light->GetLightType(),
		light->GetIntensity(),
		light->GetRange(),
		light->GetSpotAngle(),
		light->GetEntity()->GetAbsolutePosition(),
		light->GetDirection(),
		light->GetColor());
}

void LightsBuffer::SetLight(uint32 light_index, int type, float intensity, float range, float spot_angle, const Vec3& pos, const Vec3& dir, const Color& color) {
	Memcpy(_cpu_buffer + 16 + light_index * 64, &type, sizeof(int));
	Memcpy(_cpu_buffer + 16 + light_index * 64 + 4, &intensity, sizeof(float));
	Memcpy(_cpu_buffer + 16 + light_index * 64 + 8, &range, sizeof(float));
	Memcpy(_cpu_buffer + 16 + light_index * 64 + 12, &spot_angle, sizeof(float));
	Memcpy(_cpu_buffer + 16 + light_index * 64 + 16, &pos, sizeof(Vec3));
	Memcpy(_cpu_buffer + 16 + light_index * 64 + 32, &dir, sizeof(Vec3));
	Memcpy(_cpu_buffer + 16 + light_index * 64 + 48, &color, sizeof(Color));
}

GpuBuffer* LightsBuffer::GetLightsGpuBuffer() {
	return _lights_buffer;
}