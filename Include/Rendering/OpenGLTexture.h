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
		OpenGLTexture(const usize width_, const usize height_);
		~OpenGLTexture();

		void Resize(const usize width_, const usize height_);

		/// Renders the texture centred at (0, 0, 0) and of size (width, height)
		void Render();

	private:
		GLuint glTextIdx;

		usize width;
		usize height;
	};
}
