#pragma once

#include "Eigen/Core"
#include "Eigen/Geometry"

#include "imgui.h"
#include "imgui_internal.h"

#include "Utility/Literals.h"

namespace GUI
{
	class Camera
	{
	public:
		Camera(const f32 aspectRatio_);

		/// Resets the camera to {0, 0, 5} and looking in -Z
		void Reset();

		/// Applies a camera update based on user input
		void Update(const ImGuiIO& io_, const Eigen::Vector2f& vpStart_, const usize vpWidth_, const usize vpHeight_);

		Eigen::Matrix4f GetWorldToView() const;
		Eigen::Matrix4f GetViewToProj() const;

	private:
		static constexpr f32 nearPlane = 0.01f;
		static constexpr f32 farPlane  = 500.0f;

		Eigen::Matrix4f worldToView;
		Eigen::Matrix4f viewToProj;

		/// Calculates a projection matrix
		Eigen::Matrix4f CalculateOrthographicProj(const f32 aspectRatio_) const;
	};
}
