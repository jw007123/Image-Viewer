#pragma once

#include <malloc.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"

#include "GUI/SizeConsts.h"
#include "GUI/Camera.h"

#include "ImageProcessing/Image.h"

#include "Rendering/OpenGLFramebuffer.h"
#include "Rendering/OpenGLRenderer.h"

#include "imgui.h"

namespace GUI
{
	class ZoomViewport
	{
	public:
		struct Status
		{
			// Nothing here yet
		};

		ZoomViewport(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, Rendering::OpenGLRenderer& glRenderer_);

		Status Draw(const Eigen::Vector3f& centrePos_);

	private:
		struct Consts
		{
			static constexpr f32 zoomValue = -0.01f;
		};

		Utility::HeapAllocator&  heapAllocator;
		Utility::StackAllocator& stackAllocator;

		GUI::Camera					 camera;
		Rendering::OpenGLFramebuffer glFramebuffer;
		Rendering::OpenGLRenderer&   glRenderer;

		usize width;
		usize height;

		void StartFrame(const Eigen::Vector3f& centrePos_);
		void EndFrame();
	};
}
