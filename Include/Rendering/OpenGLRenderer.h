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
		OpenGLRenderer(Utility::StackAllocator& stackAllocator_, Utility::HeapAllocator& heapAllocator_);

		void RenderFullView(const GUI::Camera& cam_, const f32 aspectRatio_);
		void RenderZoomView(const GUI::Camera& cam_);

		void UpdateTexture(const ImageProcessing::Image& image_);

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
			Texture      = 0,
			CrossBGround = 1,
			ZoomBGround	 = 2,
			NumPrograms
		};

		enum TextureID : u8
		{
			Nearest  = 0,
			Bilinear = 1,
			NumTextures		// Not Num because C++ committee knows best
		};

		Utility::HeapAllocator&  heapAllocator;
		Utility::StackAllocator& stackAllocator;

		OpenGLProgram programs[ProgramID::NumPrograms];
		OpenGLTexture texture[TextureID::NumTextures];

		void RenderTexture(const TextureID& type_, const GUI::Camera& cam_, const bool onTop_);
		void RenderCrossBGround(const GUI::Camera& cam_, const f32 aspectRatio_);
		void RenderZoomBGround(const GUI::Camera& cam_);

		void LoadTextureMesh();
	};
}
