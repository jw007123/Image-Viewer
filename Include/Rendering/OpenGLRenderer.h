#pragma once

#include <vector>

#include "Eigen/Core"
#include "Eigen/Geometry"

#include <GL/gl3w.h>       
#include <GLFW/glfw3.h>

#include "Utility/HeapAllocator.h"
#include "Utility/StackAllocator.h"

#include "ImageProcessing/Image.h"

#include "GUI/Camera.h"

#include "Rendering/OpenGLShader.h"
#include "Rendering/OpenGLProgram.h"
#include "Rendering/OpenGLTexture.h"

namespace Rendering
{
	class OpenGLRenderer
	{
	public:
		OpenGLRenderer(Utility::StackAllocator* stackAllocator_, Utility::HeapAllocator* heapAllocator_);

		/// Renders the scene from cam_'s view
		void Render(const GUI::Camera& cam_, const ImageProcessing::Image* const image_, const f32 aspectRatio_);

	private:
		struct MeshOpenGLData
		{
			MeshOpenGLData();
			~MeshOpenGLData();

			void Create(Eigen::Vector3f* points_,		 const usize nPoints_,
						Eigen::Vector2f* textureCoords_, const usize nTextureCoords_,
						GLuint* indices_,				 const usize nIndices_);

			GLuint VAO;
			GLuint EBO;

			GLuint vertexVBO;
			GLuint textureVBO;

		} quadMeshData;

		enum ProgramID : u8
		{
			Texture = 0,
			BGround = 1,
			Num
		};

		Utility::HeapAllocator*  heapAllocator;
		Utility::StackAllocator* stackAllocator;

		OpenGLProgram programs[ProgramID::Num];
		OpenGLTexture texture;

		void RenderTexture(const GUI::Camera& cam_);
		void RenderBGround(const GUI::Camera& cam_, const f32 aspectRatio_);

		void LoadTextureMesh();
	};
}
