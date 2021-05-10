#pragma once

#include <Math/Vec3.hpp>
#include <Math/Mat4.hpp>

namespace VSGE {

	typedef unsigned long long ViewMask;

	class Camera {
	private:
		float mFarPlane;
		float mNearPlane;
		float mFov;
		float mAspectRatio;
		ViewMask mViewMask;

		Vec3 mPosition;
		Vec3 mFront;
		Vec3 mUp;

		Mat4 ProjectionMatrix;
		Mat4 ViewMatrix;
		Mat4 ProjectionViewMatrix;

	public:

		Camera() : mAspectRatio(1280.f / 720.f),
			mNearPlane(1.f),
			mFarPlane(1000.f),
			mFov(45),
			mViewMask(MAX_INT64)
		{

		}

		void SetNearPlane(float nearPlane) { mNearPlane = nearPlane; }
		void SetFarPlane(float farPlane) { mFarPlane = farPlane; }
		void SetFOV(float fov) { mFov = fov; }
		void SetAspectRatio(float aspectRatio) { mAspectRatio = aspectRatio; }

		float GetNearPlane() { return mNearPlane; }
		float GetFarPlane() { return mFarPlane; }
		float GetFOV() { return mFov; }
		float GetAspectRatio() { return mAspectRatio; }

		void SetPosition(const Vec3& position) { mPosition = position; }
		void SetFront(const Vec3& front) { mFront = front; }
		void SetUp(const Vec3& up) { mUp = up; }

		const Vec3& GetPosition() { return mPosition; }
		const Vec3& GetFront() { return mFront; }
		const Vec3& GetUp() { return mUp; }
	};
}