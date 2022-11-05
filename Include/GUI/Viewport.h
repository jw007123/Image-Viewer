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
		Viewport(ImageProcessing::Image* image_);

		void Draw();

	private:
		ImageProcessing::Image*		 image;
		GUI::Camera					 camera;
		Rendering::OpenGLFramebuffer framebuffer;

		usize width;
		usize height;
	};
}
