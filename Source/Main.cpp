#include "Utility/Literals.h"

#include "Utility/MemoryBlock.cpp"
#include "Utility/Log.cpp"
#include "Utility/StackAllocator.cpp"
#include "Utility/HeapAllocator.cpp"

#include "GUI/FileMenu.cpp"

#include "Rendering/OpenGLBackend.cpp"

i16 main()
{
	Rendering::OpenGLBackend glBackend;

	GUI::FileMenu fileMenu;

	while (glBackend.IsRunning())
	{
		glBackend.StartFrame();
		{
			ImGui::BeginMainMenuBar();
			{
				fileMenu.Draw();
			}
			ImGui::EndMainMenuBar();
		}
		glBackend.EndFrame();
	}

	return -1;
}
