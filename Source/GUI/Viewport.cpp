#include "GUI/Viewport.h"

namespace GUI
{
	Viewport::Viewport(ImageProcessing::Image* image_)
			 : framebuffer(1280, 720),
			   camera((f32)1280 / 720)
	{
		image  = image_;
		width  = 1280;
		height = 720;
	}


	void Viewport::Draw()
	{

	}
}
