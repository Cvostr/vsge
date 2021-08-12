#include "Camera.hpp"
#include "Entity.hpp"
#include "Engine/Application.hpp"
#include <Math/MatrixCamera.hpp>
#include <Math/MathBase.hpp>

using namespace VSGE;
using namespace YAML;

Camera::Camera() : 
			_aspectRatio(1280.f / 720.f),
			_nearPlane(1.f),
			_farPlane(1000.f),
			_fov(45.f),
			_viewMask(MAX_INT64),
			_defaultOrientation(CAM_ORIENTATION_Z_UP),
			_front(1.f, 0.f, 0.f),
			_up(0.f, 1.f, 0.f)
		{

		}
void Camera::SetNearPlane(float nearPlane) { 
	_nearPlane = nearPlane; 
}
void Camera::SetFarPlane(float farPlane) { 
	_farPlane = farPlane; 
}
void Camera::SetFOV(float fov) { 
	_fov = fov; 
}
void Camera::SetAspectRatio(float aspectRatio) { 
	_aspectRatio = aspectRatio; 
}

void Camera::SetViewMask(ViewMask mask) {
	_viewMask = mask;
}

float Camera::GetNearPlane() { 
	return _nearPlane; 
}
float Camera::GetFarPlane() { 
	return _farPlane; 
}
float Camera::GetFOV() { 
	return _fov; 
}
float Camera::GetAspectRatio() { 
	return _aspectRatio; 
}

ViewMask Camera::GetViewMask() {
	return _viewMask;
}

void Camera::SetPosition(const Vec3& position) { 
	_position = position; 
}
void Camera::SetFront(const Vec3& front) { 
	_front = front; 
}
void Camera::SetUp(const Vec3& up) { 
	_up = up; 
}

const Vec3& Camera::GetPosition() { 
	return _position; 
}
const Vec3& Camera::GetFront() { 
	return _front; 
}
const Vec3& Camera::GetUp() { 
	return _up; 
}

Vec3 Camera::GetRight() {
	return _front.Cross(_up).GetNormalized();
}

const Mat4& Camera::GetProjectionMatrix() {
	return _projectionMatrix;
}
const Mat4& Camera::GetViewMatrix() {
	return _vewMatrix;
}
const Mat4& Camera::GetProjectionViewMatrix() {
	return _projectionViewMatrix;
}

void Camera::UpdateMatrices() {
	GraphicsApi api = Application::Get()->GetGraphicsApi();

	if (api == GRAPHICS_API_VULKAN) {
		_projectionMatrix = GetPerspectiveRH_ZeroOne(_fov, _aspectRatio, _nearPlane, _farPlane);
		_vewMatrix = GetViewRH(_position, _position + _front, _up);
		_projectionViewMatrix = _vewMatrix * _projectionMatrix;
	}
	//update frustum
	_frustum.Update(_projectionViewMatrix);
}

bool Camera::IsVisibleInFrustum(const AABB& aabb) {
	FrustumIntersection intersection = _frustum.CheckAABB(aabb);
	return intersection == FRUSTUM_INSIDE || intersection == FRUSTUM_INTERSECTS;
}

Vec3 Camera::ScreenPointToWorldPoint(const Vec2& screen_point) {
	Vec3 position_clip;
	position_clip.x = screen_point.x * 2.0f - 1.0f;
	position_clip.y = screen_point.y * -2.0f + 1.0f;
	position_clip.z = _nearPlane;

	UpdateMatrices();

	// Compute world space position
	Mat4 view_projection_inverted = _projectionViewMatrix.invert();
	Vec3 position_world = view_projection_inverted * position_clip;

	return position_world;
}
Vec2 Camera::WorldPointToScreenpoint(const Vec3& world_point) {
	Vec3 position_z = _projectionViewMatrix * world_point;
	Vec2 position;
	position.x = (position_z.x / position_z.z) * 0.5f + 0.5f;
	position.y = (position_z.y / position_z.z) * -0.5f + 0.5f;

	return position;
}

void Camera::OnPreRender() {
	Vec3 pos = _entity->GetAbsolutePosition();
	SetPosition(pos);
	Quat rot = _entity->GetAbsoluteRotation();
	Vec3 rot_euler = rot.GetEulerAngles();

	float yaw = rot_euler.y;
	float pitch = rot_euler.z;

	Vec3 front;
	front.x = cosf(to_radians(yaw)) * cosf(to_radians(pitch));
	front.y = -sinf(to_radians(pitch));
	front.z = sinf(to_radians(yaw)) * cosf(to_radians(pitch));

	SetFront(front.GetNormalized());
}

void Camera::Serialize(YAML::Emitter& e) {
	e << Key << "fov" << Value << _fov;
	e << Key << "znear" << Value << _nearPlane;
	e << Key << "zfar" << Value << _farPlane;
	e << Key << "view_mask" << Value << _viewMask;
}

void Camera::Deserialize(YAML::Node& entity) {
	_fov = entity["fov"].as<float>();
	_nearPlane = entity["znear"].as<float>();
	_farPlane = entity["zfar"].as<float>();
	_viewMask = entity["view_mask"].as<ViewMask>();
}

void Camera::Serialize(ByteSerialize& serializer) {
	serializer.Serialize(_fov);
	serializer.Serialize(_nearPlane);
	serializer.Serialize(_farPlane);
	serializer.Serialize(_viewMask);
}

void Camera::Deserialize(ByteSolver& solver) {
	_fov = solver.GetValue<float>();
	_nearPlane = solver.GetValue<float>();
	_farPlane = solver.GetValue<float>();
	_viewMask = solver.GetValue<ViewMask>();
}