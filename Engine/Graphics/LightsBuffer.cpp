#include "LightsBuffer.hpp"
#include <Scene/Entity.hpp>
#include <Core/Memory.hpp>

using namespace VSGE;

#define LIGHTSOURCE_RECORD_SIZE 64

LightsBuffer::LightsBuffer() :
	_lights_buffer(nullptr),
	_cpu_buffer(nullptr)
{
	_max_lights_count = 200;
}
LightsBuffer::~LightsBuffer() {
	Destroy();
}

uint32 LightsBuffer::GetNeededBufferSize() {
	return 16 + _max_lights_count * LIGHTSOURCE_RECORD_SIZE;
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

void LightsBuffer::SetMaxLightsCount(uint32 lightsCount)
{
	_max_lights_count = lightsCount;
}

uint32 LightsBuffer::GetMaxLightsCount() const
{
	return _max_lights_count;
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

void LightsBuffer::SetLight(uint32 lightIndex, LightsourceComponent* light, int casterIndex) 
{
	SetLight(lightIndex,
		light->GetLightType(),
		light->GetIntensity(),
		light->GetRange(),
		light->GetSpotAngle(),
		light->GetEntity()->GetAbsolutePosition(),
		light->GetDirection(),
		light->GetColor(), 
		casterIndex);
}

void LightsBuffer::SetLight(
	uint32 light_index,
	int type,
	float intensity,
	float range,
	float spot_angle,
	const Vec3& pos,
	const Vec3& dir,
	const Color& color,
	int casterIndex)
{
	// Protect from lights buffer overflow
	if (casterIndex > _max_lights_count)
		return;

	// Calculate base address offset of light
	byte* lightAddress = _cpu_buffer + 16 + light_index * LIGHTSOURCE_RECORD_SIZE;

	// Write params
	Memcpy(lightAddress, &type, sizeof(int));
	Memcpy(lightAddress + 4, &intensity, sizeof(float));
	Memcpy(lightAddress + 8, &range, sizeof(float));
	Memcpy(lightAddress + 12, &spot_angle, sizeof(float));
	Memcpy(lightAddress + 16, &pos, sizeof(Vec3));
	Memcpy(lightAddress + 32, &dir, sizeof(Vec3));
	Memcpy(lightAddress + 48, &color, sizeof(Color));
	Memcpy(lightAddress + 60, &casterIndex, sizeof(int));
}

GpuBuffer* LightsBuffer::GetLightsGpuBuffer() {
	return _lights_buffer;
}