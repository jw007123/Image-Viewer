#include "Rendering/API.h"

//#if defined(RENDERER_API_OPENGL)
	#include "Rendering/OpenGL/Backend.cpp"
	#include "Rendering/OpenGL/Framebuffer.cpp"
	#include "Rendering/OpenGL/Program.cpp"
	#include "Rendering/OpenGL/Renderer.cpp"
	#include "Rendering/OpenGL/Shader.cpp"
	#include "Rendering/OpenGL/Texture.cpp"
//#elif defined(RENDERER_API_VULKAN)
	#define VMA_IMPLEMENTATION

//#else
//	#error "Unknown rendering API!"
//#endif
