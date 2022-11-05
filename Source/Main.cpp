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
#include "GUI/Window.cpp"

#include "Rendering/OpenGLBackend.cpp"
#include "Rendering/OpenGLShader.cpp"
#include "Rendering/OpenGLProgram.cpp"
#include "Rendering/OpenGLFramebuffer.cpp"
#include "Rendering/OpenGLTexture.cpp"

i16 main()
{
	Rendering::OpenGLBackend glBackend;

	Utility::HeapAllocator heapAllocator;
	ImageProcessing::Image image(&heapAllocator);

	GUI::MainMenuBar mainMenuBar(&image);
	GUI::Viewport    viewport(&image);

	while (glBackend.IsRunning())
	{
		glBackend.StartFrame();
		{
			mainMenuBar.Draw();
			viewport.Draw();
		}
		glBackend.EndFrame();
	}

	return -1;
}
