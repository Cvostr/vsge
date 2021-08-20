#pragma once

#include <Math/Vec3.hpp>
#include <Math/Mat4.hpp>
#include <Math/Frustum.hpp>
#include "IEntityComponent.hpp"

namespace VSGE {

	typedef unsigned long long ViewMask;

	enum DefaultOrientation {
		CAM_ORIENTATION_Z_UP,
		CAM_ORIENTATION_Y_UP
	};

	class Camera : public IEntityComponent {
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

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_CAMERA, "Camera")

		Camera() ;
		/// <summary>
		/// Recalculate projection, view matrices and update frustum
		/// </summary>
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
		/// <summary>
		/// Get near culling plane value
		/// </summary>
		/// <returns></returns>
		float GetNearPlane();
		/// <summary>
		/// Get far culling plane value
		/// </summary>
		/// <returns></returns>
		float GetFarPlane();
		/// <summary>
		/// Get Field of View value
		/// </summary>
		/// <returns></returns>
		float GetFOV();
		/// <summary>
		/// Get Aspect ratio value
		/// </summary>
		/// <returns></returns>
		float GetAspectRatio();

		ViewMask GetViewMask();

		void SetViewMask(ViewMask mask);

		/// <summary>
		/// Move camera to new position
		/// </summary>
		/// <param name="position">- new position for camera</param>
		void SetPosition(const Vec3& position);
		/// <summary>
		/// Set new front vector to camera
		/// </summary>
		/// <param name="front"></param>
		void SetFront(const Vec3& front);
		/// <summary>
		/// Set new UP vector to camera
		/// </summary>
		/// <param name="up"></param>
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
		/// <summary>
		/// Get calculated projection matrix
		/// </summary>
		/// <returns></returns>
		const Mat4& GetProjectionMatrix();
		/// <summary>
		/// Get calculated view matrix
		/// </summary>
		/// <returns></returns>
		const Mat4& GetViewMatrix();
		/// <summary>
		/// Get calculated multiplication of projection and view matrices
		/// </summary>
		/// <returns></returns>
		const Mat4& GetProjectionViewMatrix();
		/// <summary>
		/// Get frustum, attached to this camera
		/// </summary>
		/// <returns></returns>
		const Frustum& GetFrustum() {
			return _frustum;
		}
		/// <summary>
		/// Is bounding box visible in camera's frustum
		/// </summary>
		/// <param name="aabb">- bounding box to check</param>
		/// <returns></returns>
		bool IsVisibleInFrustum(const AABB& aabb);
		/// <summary>
		/// Unproject screen point to world point on near plane
		/// </summary>
		/// <param name="screen_point">- point on camera surface</param>
		/// <returns>Position on world</returns>
		Vec3 ScreenPointToWorldPoint(const Vec2& screen_point);
		/// <summary>
		/// Project world point to screen point
		/// </summary>
		/// <param name="world_point">- point in world</param>
		/// <returns>Position on screen</returns>
		Vec2 WorldPointToScreenpoint(const Vec3& world_point);

		Mat4 ComputeOrthoProjectionMatrix(float near, float far);

		void OnPreRender();

		void Serialize(YAML::Emitter& e);
		void Deserialize(YAML::Node& entity);

		void Serialize(ByteSerialize& serializer);
		void Deserialize(ByteSolver& solver);
	};
}