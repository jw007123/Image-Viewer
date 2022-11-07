#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Utility/Literals.h"
#include "Utility/MemoryBlock.cpp"
#include "Utility/Log.cpp"
#include "Utility/StackAllocator.cpp"
#include "Utility/HeapAllocator.cpp"

#include "ImageProcessing/ImageTypes.h"
#include "ImageProcessing/Image.cpp"

#include "GUI/MainMenuBar.cpp"
#include "GUI/FileMenu.cpp"
#include "GUI/Camera.cpp"
#include "GUI/Viewport.cpp"

#include "Rendering/OpenGLBackend.cpp"
#include "Rendering/OpenGLShader.cpp"
#include "Rendering/OpenGLProgram.cpp"
#include "Rendering/OpenGLFramebuffer.cpp"
#include "Rendering/OpenGLTexture.cpp"
#include "Rendering/OpenGLRenderer.cpp"

i16 main()
{
	Utility::HeapAllocator  heapAllocator;
	Utility::StackAllocator stackAllocator(1024 * 1024 * 10);

	Rendering::OpenGLBackend  glBackend;
	Rendering::OpenGLRenderer glRenderer(&stackAllocator, &heapAllocator);

	ImageProcessing::Image image(&heapAllocator);

	GUI::MainMenuBar mainMenuBar(&image);
	GUI::Viewport    viewport("Main Viewport", &image);

	while (glBackend.IsRunning())
	{
		glBackend.StartFrame();
		{
			// If new file opened, need to update GL portions
			const GUI::MainMenuBar::Status barStatus = mainMenuBar.Draw();
			if (barStatus.flags == GUI::MainMenuBar::Status::Flags::NewTexture)
			{
				glRenderer.UpdateTexture((u8*)image.GetData().ptr, image.GetWidth(), image.GetHeight());
			}

			viewport.StartFrame();
			{
				glRenderer.Render(viewport.GetCamera(), viewport.GetAspectRatio());
			}
			viewport.EndFrame();
		}
		glBackend.EndFrame();
	}

	return -1;
}
