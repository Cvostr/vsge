#pragma once

#include <Math/Vec3.hpp>
#include <Math/Mat4.hpp>
#include <Math/Frustum.hpp>

namespace VSGE {

	typedef unsigned long long ViewMask;

	enum DefaultOrientation {
		CAM_ORIENTATION_Z_UP,
		CAM_ORIENTATION_Y_UP
	};

	class Camera {
	private:
		DefaultOrientation _defaultOrientation;
		float _farPlane;
		float _nearPlane;
		float _fov;
		float _aspectRatio;
		ViewMask _viewMask;

		Vec3 _position;
		Vec3 _front;
		Vec3 _up;

		Mat4 _projectionMatrix;
		Mat4 _vewMatrix;
		Mat4 _projectionViewMatrix;
		
		Frustum _frustum;
	public:

		Camera() : _aspectRatio(1280.f / 720.f),
			_nearPlane(1.f),
			_farPlane(1000.f),
			_fov(45.f),
			_viewMask(MAX_INT64),
			_defaultOrientation(CAM_ORIENTATION_Z_UP),
			_front(1.f, 0.f, 0.f),
			_up(0.f, 1.f, 0.f)
		{

		}

		void UpdateMatrices();
		/// <summary>
		/// Set near occlusion plane
		/// </summary>
		/// <param name="nearPlane"></param>
		void SetNearPlane(float nearPlane);
		/// <summary>
		/// Set far occlusion plane
		/// </summary>
		/// <param name="farPlane"></param>
		void SetFarPlane(float farPlane);
		/// <summary>
		/// Set Field of view value in degrees
		/// </summary>
		/// <param name="fov"></param>
		void SetFOV(float fov);
		/// <summary>
		/// Set 
		/// </summary>
		/// <param name="aspectRatio"></param>
		void SetAspectRatio(float aspectRatio);

		float GetNearPlane();
		float GetFarPlane();
		float GetFOV();
		float GetAspectRatio();

		void SetPosition(const Vec3& position);
		void SetFront(const Vec3& front);
		void SetUp(const Vec3& up);
		/// <summary>
		/// Get world position of camera
		/// </summary>
		/// <returns></returns>
		const Vec3& GetPosition();
		/// <summary>
		/// Get front vector of camera
		/// </summary>
		/// <returns></returns>
		const Vec3& GetFront();
		/// <summary>
		/// Get up vector of camera
		/// </summary>
		/// <returns></returns>
		const Vec3& GetUp();
		/// <summary>
		/// Get right vector of camera
		/// </summary>
		/// <returns></returns>
		Vec3 GetRight();

		const Mat4& GetProjectionMatrix();
		const Mat4& GetViewMatrix();
		const Mat4& GetProjectionViewMatrix();

		const Frustum& GetFrustum() {
			return _frustum;
		}

		bool IsVisibleInFrustum(const AABB& aabb);
	};
}