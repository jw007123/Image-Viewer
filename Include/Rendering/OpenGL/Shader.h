#pragma once

#include <GL/gl3w.h>       
#include <GLFW/glfw3.h>

#include <fstream>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"

namespace Rendering
{
	class Shader
	{
	public:
		enum Type : u8
		{
			Vertex	 = 0,
			Fragment = 1,
			Num
		};

		Shader();
		~Shader();

		bool Load(Utility::StackAllocator& stackAllocator_, const Type shaderType_, const char* fName_);

		GLuint GetShaderIdx()  const;
		Type   GetShaderType() const;

	private:
		inline static const char* shaderPath = ".\\Resources\\Shaders\\";

		GLuint glShaderIdx;
		Type   shaderType;

		bool FindAndPrintShaderCompilationErrors() const;
	};
}
