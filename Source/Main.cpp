#include "Utility/Literals.h"
#include "Utility/Log.cpp"

#include "Rendering/OpenGLBackend.cpp"

i16 main()
{
	Rendering::OpenGLBackend glBackend;

	Utility::Log(Utility::LogFlag::Info, "Success!");
	return -1;
}
