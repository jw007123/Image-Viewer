#pragma once

#include <GL/gl3w.h>       
#include <GLFW/glfw3.h>

#include <stdarg.h>
#include <unordered_map>

#include "Rendering/OpenGL/Shader.h"

namespace Rendering
{
	class Program
	{
	public:
		Program();
		~Program();

		bool Load(const Shader& vsShader_, const Shader& fsShader_);
		void Use() const;

		/// Gets a GLuint handle into a uniform variable for the program
		GLuint GetUniformLoc(const char* glslName_);

	private:
		GLuint							  glProgramIdx;
		std::unordered_map<usize, GLuint> glUniformLocMap;

		/// Hashing function used for the unordered_map
		usize HashFunc(const char* str_) const;

		/// Print linkage stage compilation errors
		bool FindAndPrintProgramLinkageErrors() const;
	};
}
