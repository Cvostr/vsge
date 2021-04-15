#pragma once

#include <Math/Vec3.hpp>
#include <Math/Mat4.hpp>

namespace VSGE {
	class Camera {
	private:
		float mFarPlane;
		float mNearPlane;
		float mFOV;
		float mAspectRatio;

		Vec3 mPosition;
		Vec3 mFront;
		Vec3 mUp;

		Mat4 ProjectionMatrix;
		Mat4 ViewMatrix;

	public:

		Camera() : mAspectRatio(1280.f / 720.f),
			mNearPlane(1.f),
			mFarPlane(1000.f),
			mFov(45)
		{

		}

		void SetNearPlane(float NearPlane) { mNearPlane = NearPlane; }
		void SetFarPlane(float FarPlane) { mFarPlane = FarPlane; }
		void SetFOV(float FOV) { mFOV = FOV; }
		void SetAspectRatio(float AspectRatio) { mAspectRatio = AspectRatio; }

		float GetNearPlane() { return mNearPlane; }
		float GetFarPlane() { return mFarPlane; }
		float GetFOV() { return mFOV; }
		float GetAspectRatio() { return mAspectRatio; }

		void SetPosition(const Vec3& Position) { mPosition = Position; }
		void SetFront(const Vec3& Front) { mFront = Front; }
		void SetUp(const Vec3& Up) { mUp = Up; }

		const Vec3& GetPosition() { return mPosition; }
		const Vec3& GetFront() { return mFront; }
		const Vec3& GetUp() { return mUp; }
	};
}