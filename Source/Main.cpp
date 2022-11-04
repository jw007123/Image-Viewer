#include "Utility/Literals.h"

#include "Utility/MemoryBlock.cpp"
#include "Utility/Log.cpp"
#include "Utility/StackAllocator.cpp"
#include "Utility/HeapAllocator.cpp"

#include "Rendering/OpenGLBackend.cpp"

i16 main()
{
	Rendering::OpenGLBackend glBackend;

	while (glBackend.IsRunning())
	{
		glBackend.StartFrame();
		{
			ImGui::ShowDemoWindow();
		}
		glBackend.EndFrame();
	}

	return -1;
}
