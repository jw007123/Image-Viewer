#pragma once

#include <malloc.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"

#include "GUI/Camera.h"

#include "ImageProcessing/Image.h"

#include "Rendering/OpenGLFramebuffer.h"

#include "imgui.h"

namespace GUI
{
	class Viewport
	{
	public:
		Viewport(const char* name_, ImageProcessing::Image* image_);

		void StartFrame();
		void EndFrame();

		const Camera& GetCamera() const;

	private:
		ImageProcessing::Image*		 image;
		GUI::Camera					 camera;
		Rendering::OpenGLFramebuffer framebuffer;

		ImGuiViewport* imguiViewport;
		usize width;
		usize height;
		char  name[PATH_MAX_LEN];
	};
}
