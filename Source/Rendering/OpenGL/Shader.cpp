#include "Rendering/OpenGL/Shader.h"

namespace Rendering
{
	Shader::Shader()
	{
		glShaderIdx = GL_INVALID_INDEX;
	}


	Shader::~Shader()
	{
		if (glShaderIdx != GL_INVALID_INDEX)
		{
			glDeleteShader(glShaderIdx);
		}
	}


	bool Shader::Load(Utility::StackAllocator& stackAllocator_, const Type shaderType_, const char* fName_)
	{
		// Checks
		assert(PATH_MAX_LEN > (strlen(fName_) + strlen(shaderPath)));
		assert(strlen(fName_));
		assert(strlen(shaderPath));

		char concatPath[PATH_MAX_LEN];
		sprintf(concatPath, "%s%s.glsl", shaderPath, fName_);

		// Open file
		std::ifstream fStream(concatPath, std::ios::ate | std::ios::binary);
		if (!fStream.is_open())
		{
			Utility::Log(Utility::Error, "Shader file %s does not exist!", concatPath);
			return false;
		}

		// Read file and determine size
		const GLint fSize = (GLint)fStream.tellg();
		fStream.seekg(0, std::ios_base::beg);

		// Copy to block
		Utility::MemoryBlock fDataBlock = stackAllocator_.Allocate(sizeof(char) * fSize);
		fStream.read((char*)fDataBlock.ptr, fSize);
		fStream.close();

		// Create shader of correct type
		GLint shaderTypes[Type::Num] =
		{
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER
		};
		glShaderIdx = glCreateShader(shaderTypes[(usize)shaderType_]);

		// Compile shader
		shaderType		   = shaderType_;
		GLchar* sourcePtr  = (GLchar*)fDataBlock.ptr;	 // C++20
		glShaderSource(glShaderIdx, 1, &sourcePtr, &fSize);
		glCompileShader(glShaderIdx);

		const bool compiled = FindAndPrintShaderCompilationErrors();
		if (compiled)
		{
			Utility::Log(Utility::Info, "Successfuly compiled %s!", concatPath);
		}

		stackAllocator_.Free(fDataBlock);

		return compiled;
	}


	GLuint Shader::GetShaderIdx() const
	{
		return glShaderIdx;
	}


	Shader::Type Shader::GetShaderType() const
	{
		return shaderType;
	}


	bool Shader::FindAndPrintShaderCompilationErrors() const
	{
		i16 errors = -1;
		glGetShaderiv(glShaderIdx, GL_COMPILE_STATUS, &errors);
		if (GL_TRUE != errors)
		{
			char errorOutput[PATH_MAX_LEN];
			glGetShaderInfoLog(glShaderIdx, PATH_MAX_LEN, NULL, errorOutput);

			Utility::Log(Utility::Error, "Failed to compile %u with error %s!", glShaderIdx, errorOutput);
			return false;
		}

		return true;
	}
}
