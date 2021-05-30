#include "Camera.hpp"
#include "Engine/Application.hpp"
#include <Math/MatrixCamera.hpp>
#include <Math/MathBase.hpp>

using namespace VSGE;

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
		_projectionMatrix.Values[1][1] *= -1;
		_vewMatrix = GetViewRH(_position, _position + _front, _up);
		_projectionViewMatrix = _vewMatrix * _projectionMatrix;
	}
	//update frustum
	_frustum.Update(_projectionViewMatrix);
}

bool Camera::IsVisibleInFrustum(const AABB& aabb) {
	return _frustum.CheckAABB(aabb) == FRUSTUM_INSIDE || _frustum.CheckAABB(aabb) == FRUSTUM_INTERSECTS;
}