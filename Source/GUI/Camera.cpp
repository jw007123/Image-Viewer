#include "GUI/Camera.h"

namespace GUI
{
	Camera::Camera(const f32 aspectRatio_)
	{
		Reset();

		viewToProj = CalculateOrthographicProj(aspectRatio_);
	}


	void Camera::Update(const ImGuiIO& io_, const Eigen::Vector2f& vpStart_, const usize vpWidth_, const usize vpHeight_)
	{
		viewToProj = CalculateOrthographicProj((f32)vpWidth_ / vpHeight_);

		// Determine translation w/ right mouse button engaged and scrollwheel
		Eigen::Vector3f camTranslation = Eigen::Vector3f::Zero();
		{
			const f32 scale = 0.01f;
			if (io_.MouseDown[1])
			{
				// Obtain translation in XY
				camTranslation.x() = io_.MouseDelta.x * scale;
				camTranslation.y() = io_.MouseDelta.y * scale * -1.0f;
			}

			// Obtain translation in Z
			camTranslation.z() = io_.MouseWheel * -1.0f;
		}

		// Apply update to camera
		worldToView.block<3, 1>(0, 3) += camTranslation;
	}


	void Camera::Reset()
	{
		worldToView.setIdentity();
		worldToView.block<3, 1>(0, 3) = Eigen::Vector3f(0.0f, 0.0f, -5.0f);
	}


	Eigen::Matrix4f Camera::GetWorldToView() const
	{
		return worldToView;
	}


	Eigen::Matrix4f Camera::GetViewToProj() const
	{
		return viewToProj;
	}


	Eigen::Matrix4f Camera::CalculateOrthographicProj(const f32 aspectRatio_) const
	{
		const f32 xScale		  = 1.0f / (5.0f * aspectRatio_);
		const f32 yScale		  = 1.0f / 5.0f;
		const f32 zScale		  = 2.0f / (nearPlane - farPlane);
		const f32 orthoCorrection = (farPlane + nearPlane) / (nearPlane - farPlane);

		Eigen::Matrix4f projMat;
		projMat <<
			xScale, 0.0f,   0.0f,   0.0f,
			0.0f,   yScale, 0.0f,   0.0f,
			0.0f,   0.0f,   zScale, orthoCorrection,
			0.0f,   0.0f,   0.0f,   1.0f;

		return projMat;
	}
}
