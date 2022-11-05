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

#include "Rendering/OpenGLBackend.cpp"

i16 main()
{
	Utility::HeapAllocator heapAllocator;
	ImageProcessing::Image image(&heapAllocator);

	Rendering::OpenGLBackend glBackend;

	GUI::MainMenuBar mainMenuBar(&image);

	while (glBackend.IsRunning())
	{
		glBackend.StartFrame();
		{
			mainMenuBar.Draw();
		}
		glBackend.EndFrame();
	}

	return -1;
}
