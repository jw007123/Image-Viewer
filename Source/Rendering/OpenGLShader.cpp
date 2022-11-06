#include "Rendering/OpenGLShader.h"

namespace Rendering
{
	OpenGLShader::OpenGLShader()
	{
		glShaderIdx = GL_INVALID_INDEX;
	}


	OpenGLShader::~OpenGLShader()
	{
		if (glShaderIdx != GL_INVALID_INDEX)
		{
			glDeleteShader(glShaderIdx);
		}
	}


	bool OpenGLShader::Load(Utility::StackAllocator* stackAllocator_, const Type shaderType_, const char* fName_)
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
			Utility::Log(Utility::LogFlag::Error, "Shader file %s does not exist!", concatPath);
			return false;
		}

		// Read file and determine size
		const GLint fSize = (GLint)fStream.tellg();
		fStream.seekg(0, std::ios_base::beg);

		// Copy to block
		Utility::MemoryBlock fDataBlock = stackAllocator_->Allocate(sizeof(char) * fSize);
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
		shaderType = shaderType_;
		glShaderSource(glShaderIdx, 1, &(GLchar*)fDataBlock.ptr, &fSize);
		glCompileShader(glShaderIdx);

		const bool compiled = FindAndPrintShaderCompilationErrors();
		if (compiled)
		{
			Utility::Log(Utility::LogFlag::Info, "Successfuly compiled %s!", concatPath);
		}

		stackAllocator_->Free(fDataBlock);

		return compiled;
	}


	GLuint OpenGLShader::GetShaderIdx() const
	{
		return glShaderIdx;
	}


	OpenGLShader::Type OpenGLShader::GetShaderType() const
	{
		return shaderType;
	}


	bool OpenGLShader::FindAndPrintShaderCompilationErrors() const
	{
		i16 errors = -1;
		glGetShaderiv(glShaderIdx, GL_COMPILE_STATUS, &errors);
		if (GL_TRUE != errors)
		{
			char errorOutput[PATH_MAX_LEN];
			glGetShaderInfoLog(glShaderIdx, PATH_MAX_LEN, NULL, errorOutput);

			Utility::Log(Utility::LogFlag::Error, "Failed to compile %u with error %s!", glShaderIdx, errorOutput);
			return false;
		}

		return true;
	}
}
