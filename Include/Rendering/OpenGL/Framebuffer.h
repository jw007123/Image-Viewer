#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "imgui.h"

#include "Eigen/Core"

#include "Utility/Literals.h"

namespace Rendering
{
	class Framebuffer
	{
	public:
		Framebuffer(const usize width_, const usize height_);
		~Framebuffer();

		void Resize(const usize width_, const usize height_);

		/// Start/Stop rendering to this framebuffer
		void StartFrame();
		void EndFrame();

	private:
		GLuint glMSAAFBO;
		GLuint glFBO;
		GLuint glMSAAText;
		GLuint glFBOText;
		GLuint glRBO;

		usize width;
		usize height;
	};
}
