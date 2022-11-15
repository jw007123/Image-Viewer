#include "Rendering/OpenGL/Program.h"

namespace Rendering
{
	Program::Program()
	{
		glProgramIdx = GL_INVALID_INDEX;
	}


	Program::~Program()
	{
		if (glProgramIdx != GL_INVALID_INDEX)
		{
			glDeleteProgram(glProgramIdx);
		}
	}


	bool Program::Load(const Shader& vsShader_, const Shader& fsShader_)
	{
		glProgramIdx = glCreateProgram();

		glAttachShader(glProgramIdx, vsShader_.GetShaderIdx());
		glAttachShader(glProgramIdx, fsShader_.GetShaderIdx());

		glLinkProgram(glProgramIdx);
		const bool linked = FindAndPrintProgramLinkageErrors();
		if (linked)
		{
			Utility::Log(Utility::Info, "Program consisting of 2 shaders linked successfully!");
		}

		glDetachShader(glProgramIdx, vsShader_.GetShaderIdx());
		glDetachShader(glProgramIdx, fsShader_.GetShaderIdx());

		return linked;
	}


	void Program::Use() const
	{
		assert(glProgramIdx != GL_INVALID_INDEX);
		glUseProgram(glProgramIdx);
	}


	GLuint Program::GetUniformLoc(const char* glslName_)
	{
		GLuint loc = -1;
		std::unordered_map<usize, GLuint>::const_iterator iter = glUniformLocMap.find(HashFunc(glslName_));
		if (iter == glUniformLocMap.end())
		{
			loc = glGetUniformLocation(glProgramIdx, glslName_);
			glUniformLocMap.insert(std::make_pair(HashFunc(glslName_), loc));

			return loc;
		}
		else
		{
			return iter->second;
		}
	}


	usize Program::HashFunc(const char* str_) const
	{
		// c.f. https://stackoverflow.com/questions/7666509/hash-function-for-string
		usize hash = 5381;
		usize c	   = 0;

		while (c = *str_++)
		{
			hash = ((hash << 5) + hash) + c;
		}

		return hash;
	}


	bool Program::FindAndPrintProgramLinkageErrors() const
	{
		i16 errors = -1;
		glGetProgramiv(glProgramIdx, GL_LINK_STATUS, &errors);
		if (GL_TRUE != errors)
		{
			char errorOutput[PATH_MAX_LEN];
			glGetProgramInfoLog(glProgramIdx, PATH_MAX_LEN, NULL, errorOutput);

			Utility::Log(Utility::Error, "Failed to link %u with error %s!", glProgramIdx, errorOutput);
			return false;
		}

		return true;
	}
}
