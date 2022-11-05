#pragma once

#include <GL/gl3w.h>       
#include <GLFW/glfw3.h>

#include <fstream>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"

namespace Rendering
{
	class OpenGLShader
	{
	public:
		enum class Type : u8
		{
			Vertex	 = 0,
			Fragment = 1,
			Num
		};

		OpenGLShader(Utility::StackAllocator* stackAllocator_, const Type shaderType_, const char* fName_);
		~OpenGLShader();

		GLuint GetShaderIdx()  const;
		Type   GetShaderType() const;

	private:
		inline static const char* shaderPath = "..\\Resources\\Shaders\\";

		GLuint glShaderIdx;
		Type   shaderType;

		bool FindAndPrintShaderCompilationErrors() const;
	};
}
