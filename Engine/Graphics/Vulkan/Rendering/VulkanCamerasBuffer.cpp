#include "VulkanCamerasBuffer.hpp"
#include <Math/MatrixCamera.hpp>

using namespace VSGE;

VulkanCamerasBuffer::VulkanCamerasBuffer() {
	Create();
}
VulkanCamerasBuffer::~VulkanCamerasBuffer() {
	Destroy();
}

void VulkanCamerasBuffer::Create() {
	_cameras_buffer = new VulkanBuffer(GPU_BUFFER_TYPE_UNIFORM);
	_cameras_buffer->Create((MAX_CAMERAS + 6) * CAMERA_ELEM_SIZE);

	_cameras = new Camera* [MAX_CAMERAS + 6];

	//Fill last 6 cameras with empty cameras
	for (uint32 i = ENVMAP_CAMS_POS; i < ENVMAP_CAMS_POS + 6; i++) {
		_cameras[i] = new Camera;
		_cameras[i]->SetFOV(90);
		_cameras[i]->SetAspectRatio(1.f);
		_cameras[i]->SetNearPlane(0.1f);
	}
}
void VulkanCamerasBuffer::Destroy() {
	SAFE_RELEASE(_cameras_buffer)
	SAFE_RELEASE_ARR(_cameras)
}

void VulkanCamerasBuffer::SetEnvmapCameras(const Vec3& position, float far_plane) {
	Mat4 projection = GetPerspectiveRH_ZeroOne(90.f, 1, 0.01f, far_plane);

	Vec3 fronts[6] = { Vec3(1, 0, 0), Vec3(-1, 0, 0), Vec3(0, 1, 0), Vec3(0, -1, 0), Vec3(0, 0, 1), Vec3(0, 0, -1) };
	Vec3 ups[6] = { Vec3(0, -1, 0), Vec3(0, -1, 0), Vec3(0, 0, 1), Vec3(0, 0, -1), Vec3(0, -1, 0), Vec3(0, -1, 0) };

	for (uint32 i = ENVMAP_CAMS_POS; i < ENVMAP_CAMS_POS + 6; i++) {
		_cameras[i]->SetPosition(position);
		_cameras[i]->SetFarPlane(far_plane);

		_cameras[i]->SetFront(fronts[i - ENVMAP_CAMS_POS]);
		_cameras[i]->SetUp(ups[i - ENVMAP_CAMS_POS]);
		_cameras[i]->UpdateMatrices();

		Mat4 projection = _cameras[i]->GetProjectionMatrix();
		projection[1][1] *= -1;

		WriteCameraToBuffer(i, projection, _cameras[i]->GetViewMatrix(), position);
	}
}

void VulkanCamerasBuffer::WriteCameraToBuffer(
	uint32 index, 
	const Mat4& projection,
	const Mat4& view,
	const Vec3& pos
) {
	Mat4 projection_view = view * projection;
	Mat4 skybox_viewproj = RemoveTranslationFromViewMat(view) * projection;

	uint32 offset = index * CAMERA_ELEM_SIZE;
	_cameras_buffer->WriteData(offset, sizeof(Mat4), &projection_view);
	_cameras_buffer->WriteData(offset + sizeof(Mat4), sizeof(Mat4), &skybox_viewproj);
	_cameras_buffer->WriteData(offset + sizeof(Mat4) * 2, sizeof(Vec3), (void*)&pos);
}

void VulkanCamerasBuffer::SetCamera(uint32 camera_index, Camera* camera) {
	if (camera_index >= MAX_CAMERAS)
		return;

	_cameras[camera_index] = camera;
	camera->UpdateMatrices();

	uint32 offset = camera_index * CAMERA_ELEM_SIZE;

	WriteCameraToBuffer(camera_index, 
		camera->GetProjectionMatrix(),
		camera->GetViewMatrix(), 
		camera->GetPosition());
	}
uint32 VulkanCamerasBuffer::GetCameraId(Camera* camera) {
	for (uint32 i = 0; i < MAX_CAMERAS; i++) {
		if (_cameras[i] == camera)
			return i;
	}
	return 0xFFFFFFFF;
}
Camera* VulkanCamerasBuffer::GetCameraByIndex(uint32 index) {
	return _cameras[index];
}
VulkanBuffer* VulkanCamerasBuffer::GetCamerasBuffer() {
	return _cameras_buffer;
}