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
		Camera(const f32 aspectRatio_, const f32 initialZ_);

		void Reset(const f32 initialZ_);
		void UpdateProjection(const usize vpWidth_, const usize vpHeight_);

		/// Applies a camera update based on user input
		void UpdateFree(const ImGuiIO& io_, const Eigen::Vector2f& vpStart_, const usize vpWidth_, const usize vpHeight_);

		/// Applies a camera update based on a fixed position XY or XYZ position
		void UpdateFixed(const ImGuiIO& io_, const Eigen::Vector2f& posXY_, const Eigen::Vector2f& vpStart_, const usize vpWidth_, const usize vpHeight_);
		void UpdateFixed(const Eigen::Vector3f& pos_, const Eigen::Vector2f& vpStart_, const usize vpWidth_, const usize vpHeight_);

		/// Returns the mouse position in world coordinates
		Eigen::Vector3f CalculateMousePosition(const Eigen::Vector2f& mousePos_, const usize vpWidth_, const usize vpHeight_);

		Eigen::Matrix4f GetWorldToView() const;
		Eigen::Matrix4f GetViewToProj()  const;

	private:
		struct Consts
		{
			static constexpr f32 nearPlane =  0.001f;
			static constexpr f32 farPlane  =  10.0f;
			static constexpr f32 camXMin   = -2.5f;
			static constexpr f32 camYMin   = -2.5f;
			static constexpr f32 camZMin   = -5.0f;
			static constexpr f32 camXMax   =  2.5f;
			static constexpr f32 camYMax   =  2.5f;
			static constexpr f32 camZMax   = -0.01f;
			static constexpr f32 camZScF   =  0.1f;
			static constexpr f32 camXYScF  =  0.001f;
		};

		Eigen::Matrix4f worldToView;
		Eigen::Matrix4f viewToProj;

		Eigen::Matrix4f CalculateProjection(const f32 aspectRatio_) const;

		/// Returns ray as <origin, dir, t>, with ||dir||_2 = 1
		struct Ray
		{
			Eigen::Vector3f origin;
			Eigen::Vector3f dir;
			f32			    t;
		};
		Ray GenerateRayToMousePosition(const Eigen::Vector2f& mousePos_, const usize vpWidth_, const usize vpHeight_) const;
	};
}
