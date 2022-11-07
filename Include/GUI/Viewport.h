#pragma once

#include <malloc.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"

#include "GUI/Camera.h"

#include "ImageProcessing/Image.h"

#include "Rendering/OpenGLFramebuffer.h"
#include "Rendering/OpenGLRenderer.h"

#include "imgui.h"

namespace GUI
{
	class Viewport
	{
	public:
		struct Status
		{
			// Nothing here yet
		};

		Viewport(Utility::HeapAllocator* heapAllocator_, Utility::StackAllocator* stackAllocator_, ImageProcessing::Image* image_);

		Status Draw();

	private:
		struct Consts
		{
			static constexpr usize startWidth  = 1280;
			static constexpr usize startHeight = 720;
		};

		ImageProcessing::Image*		 image;
		GUI::Camera					 camera;
		Rendering::OpenGLFramebuffer glFramebuffer;
		Rendering::OpenGLRenderer	 glRenderer;

		ImGuiViewport* imguiViewport;
		usize width;
		usize height;

		void StartFrame();
		void EndFrame();
	};
}
