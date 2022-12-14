#include "GUI/Camera.h"

namespace GUI
{
	Camera::Camera(const f32 aspectRatio_, const f32 initialZ_)
	{
		Reset(initialZ_);

		viewToProj = CalculateProjection(aspectRatio_);
	}


	void Camera::UpdateProjection(const usize vpWidth_, const usize vpHeight_)
	{
		viewToProj = CalculateProjection((f32)vpWidth_ / vpHeight_);
	}


	void Camera::Reset(const f32 initialZ_)
	{
		worldToView.setIdentity();
		worldToView.block<3, 1>(0, 3) = Eigen::Vector3f(0.0f, 0.0f, initialZ_);
	}


	void Camera::UpdateFree(const ImGuiIO& io_, const Eigen::Vector2f& vpStart_, const usize vpWidth_, const usize vpHeight_)
	{
		viewToProj = CalculateProjection((f32)vpWidth_ / vpHeight_);

		// Determine translation w/ right mouse button engaged and scrollwheel
		{
			const f32 scale = Consts::camXYScF * worldToView(2, 3) * - 1.0f;
			if (io_.MouseDown[1])
			{
				// Obtain translation in XY
				worldToView(0, 3) += io_.MouseDelta.x * scale;
				worldToView(1, 3) += io_.MouseDelta.y * scale * -1.0f;
			}

			// Obtain translation in Z
			const Eigen::Vector2f mousePos = Eigen::Vector2f(io_.MousePos.x, io_.MousePos.y);
			const Eigen::Vector3f rayDir   = GenerateRayToMousePosition(mousePos, vpWidth_, vpHeight_).dir;
			if (io_.MouseWheel > 0 && worldToView(2, 3) != Consts::camZMax)
			{
				worldToView.block<3, 1>(0, 3) -= (Consts::camZScF * (worldToView(2, 3))) * rayDir;
			}
			else if (io_.MouseWheel < 0 && worldToView(2, 3) != Consts::camZMin)
			{
				worldToView.block<3, 1>(0, 3) += (Consts::camZScF * (worldToView(2, 3))) * rayDir;
			}
		}

		// Clamp to avoid straying too far
		worldToView(0, 3) = std::clamp<f32>(worldToView(0, 3), Consts::camXMin, Consts::camXMax);
		worldToView(1, 3) = std::clamp<f32>(worldToView(1, 3), Consts::camYMin, Consts::camYMax);
		worldToView(2, 3) = std::clamp<f32>(worldToView(2, 3), Consts::camZMin, Consts::camZMax);
	}


	void Camera::UpdateFixed(const ImGuiIO& io_, const Eigen::Vector2f& posXY_, const Eigen::Vector2f& vpStart_, const usize vpWidth_, const usize vpHeight_)
	{
		viewToProj = CalculateProjection((f32)vpWidth_ / vpHeight_);

		// XY is fixed
		worldToView(0, 3) = posXY_.x();
		worldToView(1, 3) = posXY_.y();

		// Z can still move
		if (io_.MouseWheel > 0 && worldToView(2, 3) != Consts::camZMax)
		{
			worldToView(2, 3) -= (Consts::camZScF * (worldToView(2, 3)));
		}
		else if (io_.MouseWheel < 0 && worldToView(2, 3) != Consts::camZMin)
		{
			worldToView(2, 3) += (Consts::camZScF * (worldToView(2, 3)));
		}
	}


	void Camera::UpdateFixed(const Eigen::Vector3f& pos_, const Eigen::Vector2f& vpStart_, const usize vpWidth_, const usize vpHeight_)
	{
		viewToProj = CalculateProjection((f32)vpWidth_ / vpHeight_);

		// XY is fixed
		worldToView(0, 3) = pos_.x();
		worldToView(1, 3) = pos_.y();
		worldToView(2, 3) = pos_.z();
	}


	Eigen::Vector3f Camera::CalculateMousePosition(const Eigen::Vector2f& mousePos_, const usize vpWidth_, const usize vpHeight_)
	{
		const Ray ray = GenerateRayToMousePosition(mousePos_, vpWidth_, vpHeight_);

		// Offset by origin
		return ray.dir * ray.t - ray.origin;
	}


	Eigen::Matrix4f Camera::GetWorldToView() const
	{
		return worldToView;
	}


	Eigen::Matrix4f Camera::GetViewToProj() const
	{
		return viewToProj;
	}


	Eigen::Matrix4f Camera::CalculateProjection(const f32 aspectRatio_) const
	{
		const f32 range			 = tanf(PI / 4.0f) * Consts::nearPlane;
		const f32 xScale		 = (2.0f * Consts::nearPlane) / (2.0f * range * aspectRatio_);
		const f32 yScale		 = Consts::nearPlane / range;
		const f32 zScale		 = (Consts::farPlane + Consts::nearPlane) / (Consts::nearPlane - Consts::farPlane);
		const f32 perpCorrection = (2.0f * Consts::farPlane * Consts::nearPlane) / (Consts::nearPlane - Consts::farPlane);

		Eigen::Matrix4f projMat;
		projMat <<
			xScale, 0.0f,	0.0f,	0.0f,
			0.0f,	yScale, 0.0f,	0.0f,
			0.0f,	0.0f,	zScale, perpCorrection,
			0.0f,	0.0f,	-1.0f,	0.0f;

		return projMat;
	}


	Camera::Ray Camera::GenerateRayToMousePosition(const Eigen::Vector2f& mousePos_, const usize vpWidth_, const usize vpHeight_) const
	{
		// (0, 0) is top-left corner by convention
		const f32 aspectRatio = (f32)vpWidth_ / vpHeight_;
		const f32 fov		  = PI * 0.5f;

		Eigen::Vector2f p = Eigen::Vector2f::Zero();
		p.x()			  = (2.0f * ((mousePos_.x() + 0.5f) / vpWidth_) - 1.0f)  * tanf(fov / 2.0f) * aspectRatio;
		p.y()			  = (1.0f - 2.0f * ((mousePos_.y() + 0.5f) / vpHeight_)) * tanf(fov / 2.0f);

		Ray cameraRay;

		cameraRay.origin		  = (worldToView.inverse() * Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f)).topLeftCorner<3, 1>();
		const Eigen::Vector3f end = (worldToView.inverse() * Eigen::Vector4f(p.x(), p.y(), -1.0f, 1.0f)).topLeftCorner<3, 1>();
		cameraRay.dir			  = (cameraRay.origin - end).normalized();
		cameraRay.t				  =  cameraRay.origin.z() / cameraRay.dir.z();

		return cameraRay;
	}
}
