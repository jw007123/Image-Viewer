#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "Eigen/Core"

#include "Utility/Literals.h"

namespace Rendering
{
	class OpenGLTexture
	{
	public:
		OpenGLTexture();
		~OpenGLTexture();

		/// data_ assumed to be RGBA
		void Update(const u8* data_, const usize width_, const usize height_);

		GLuint GetTextureIdx() const;
		usize  GetWidth() const;
		usize  GetHeight() const;

	private:
		GLuint glTextIdx;

		usize width;
		usize height;
	};
}
