#include "Rendering/OpenGL/Renderer.h"

namespace Rendering
{
	Renderer::MeshOpenGLData::MeshOpenGLData()
	{
		VAO = GL_INVALID_INDEX;
		EBO = GL_INVALID_INDEX;

		vertexVBO  = GL_INVALID_INDEX;
		textureVBO = GL_INVALID_INDEX;
	}


	Renderer::MeshOpenGLData::~MeshOpenGLData()
	{
		if (VAO != GL_INVALID_INDEX)
		{
			glDeleteVertexArrays(1, &VAO);
		}

		if (EBO != GL_INVALID_INDEX)
		{
			glDeleteBuffers(1, &EBO);
		}

		if (vertexVBO != GL_INVALID_INDEX)
		{
			glDeleteBuffers(1, &vertexVBO);
		}

		if (textureVBO != GL_INVALID_INDEX)
		{
			glDeleteBuffers(1, &textureVBO);
		}
	}


	void Renderer::MeshOpenGLData::Create(Eigen::Vector3f* points_,				 const usize nPoints_,
										  Eigen::Vector2f* textureCoords_,		 const usize nTextureCoords_,
										  GLuint* indices_,						 const usize nIndices_)
	{
		glGenVertexArrays(1, &VAO);

		// Generate buffs
		glGenBuffers(1, &vertexVBO);
		glGenBuffers(1, &textureVBO);
		glGenBuffers(1, &EBO);

		// Fill buffs
		glBindVertexArray(VAO);
		{
			glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Eigen::Vector3f) * nPoints_, points_, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Eigen::Vector2f) * nTextureCoords_, textureCoords_, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * nIndices_, indices_, GL_STATIC_DRAW);
		}
		glBindVertexArray(0);
	}



	Renderer::Renderer(Utility::StackAllocator& stackAllocator_, Utility::HeapAllocator& heapAllocator_) :
					   heapAllocator(heapAllocator_),
					   stackAllocator(stackAllocator_)
	{
		{
			Shader vsTexture;
			vsTexture.Load(stackAllocator, Shader::Vertex, "vsTexture");

			Shader fsTexture;
			fsTexture.Load(stackAllocator, Shader::Fragment, "fsTexture");

			programs[ProgramID::QuadTexture].Load(vsTexture, fsTexture);
		}

		{
			Shader vsCrossBGround;
			vsCrossBGround.Load(stackAllocator, Shader::Vertex, "vsCrossBGround");

			Shader fsCrossBGround;
			fsCrossBGround.Load(stackAllocator, Shader::Fragment, "fsCrossBGround");

			programs[ProgramID::CrossBGround].Load(vsCrossBGround, fsCrossBGround);
		}

		{
			Shader vsZoomBGround;
			vsZoomBGround.Load(stackAllocator, Shader::Vertex, "vsZoomBGround");

			Shader fsZoomBGround;
			fsZoomBGround.Load(stackAllocator, Shader::Fragment, "fsZoomBGround");

			programs[ProgramID::ZoomBGround].Load(vsZoomBGround, fsZoomBGround);
		}

		LoadTextureMesh();
	}


	void Renderer::RenderFullView(const GUI::Camera& cam_, const f32 aspectRatio_)
	{
		if (texture[TextureID::Bilinear].IsValid())
		{
			RenderTexture(TextureID::Bilinear, cam_, true);
		}

		RenderCrossBGround(cam_, aspectRatio_);
	}


	void Renderer::RenderZoomView(const GUI::Camera& cam_)
	{
		if (texture[TextureID::Nearest].IsValid())
		{
			RenderTexture(TextureID::Nearest, cam_, false);
		}

		RenderZoomBGround(cam_);
	}


	void Renderer::UpdateTexture(const ImageProcessing::Image& image_)
	{
		texture[TextureID::Nearest].Update(Texture::Nearest,   (u8*)image_.GetData().ptr, image_.GetWidth(), image_.GetHeight());
		texture[TextureID::Bilinear].Update(Texture::Bilinear, (u8*)image_.GetData().ptr, image_.GetWidth(), image_.GetHeight());
	}


	void Renderer::RenderTexture(const TextureID& type_, const GUI::Camera& cam_, const bool onTop_)
	{
		// Use correct program
		programs[ProgramID::QuadTexture].Use();

		glBindVertexArray(quadMeshData.VAO);
		{
			// Set uniforms
			const GLuint viewToProjLoc   = programs[ProgramID::QuadTexture].GetUniformLoc("viewToProj");
			const GLuint worldToViewLoc  = programs[ProgramID::QuadTexture].GetUniformLoc("worldToView");
			const GLuint modelToWorldLoc = programs[ProgramID::QuadTexture].GetUniformLoc("modelToWorld");
			const GLuint onTopLoc		 = programs[ProgramID::QuadTexture].GetUniformLoc("onTop");

			// Calculate texture model mat
			Eigen::Matrix4f modelToWorld = Eigen::Matrix4f::Identity();
			modelToWorld(0, 0)			 = (f32)texture[type_].GetWidth() / texture[type_].GetHeight();

			glUniformMatrix4fv(viewToProjLoc,   1, false, cam_.GetViewToProj().data());
			glUniformMatrix4fv(worldToViewLoc,  1, false, cam_.GetWorldToView().data());
			glUniformMatrix4fv(modelToWorldLoc, 1, false, modelToWorld.data());
			glUniform1i(onTopLoc, onTop_);

			// Bind texture. For now, we're just binding one texture a time and can use the 0th unit
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture[type_].GetTextureIdx());

			// Draw
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
	}


	void Renderer::RenderCrossBGround(const GUI::Camera& cam_, const f32 aspectRatio_)
	{
		// Use correct program
		programs[ProgramID::CrossBGround].Use();

		glBindVertexArray(quadMeshData.VAO);
		{
			// Set uniforms
			const GLuint cameraPosLoc   = programs[ProgramID::CrossBGround].GetUniformLoc("cameraPos");
			const GLuint aspectRatioLoc = programs[ProgramID::CrossBGround].GetUniformLoc("aspectRatio");

			// Set uniforms
			const Eigen::Vector3f cameraPos = cam_.GetWorldToView().block<3, 1>(0, 3);
			glUniform3f(cameraPosLoc,	cameraPos.x(), cameraPos.y(), cameraPos.z());
			glUniform1f(aspectRatioLoc, aspectRatio_);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
	}


	void Renderer::RenderZoomBGround(const GUI::Camera& cam_)
	{
		programs[ProgramID::ZoomBGround].Use();

		glBindVertexArray(quadMeshData.VAO);
		{
			// Get uniforms
			const GLuint cameraPosLoc = programs[ProgramID::CrossBGround].GetUniformLoc("cameraPos");

			// Set uniforms
			const Eigen::Vector3f cameraPos = cam_.GetWorldToView().block<3, 1>(0, 3);
			glUniform3f(cameraPosLoc, cameraPos.x(), cameraPos.y(), cameraPos.z());

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
	}


	void Renderer::LoadTextureMesh()
	{
		Utility::MemoryBlock pointsBuff		   = stackAllocator.Allocate<Eigen::Vector3f>(4);
		Utility::MemoryBlock textureCoordsBuff = stackAllocator.Allocate<Eigen::Vector2f>(4);
		Utility::MemoryBlock indicesBuff	   = stackAllocator.Allocate<GLuint>(6);

		Eigen::Vector3f* points		   = (Eigen::Vector3f*)pointsBuff.ptr;
		Eigen::Vector2f* textureCoords = (Eigen::Vector2f*)textureCoordsBuff.ptr;
		GLuint* indices				   = (GLuint*)indicesBuff.ptr;;

		// Points
		{
			points[0] = Eigen::Vector3f(0.5f,   0.5f, 0.0f);
			points[1] = Eigen::Vector3f(0.5f,  -0.5f, 0.0f);
			points[2] = Eigen::Vector3f(-0.5f, -0.5f, 0.0f);
			points[3] = Eigen::Vector3f(-0.5f,  0.5f, 0.0f);
		}

		// Texture Coords
		{
			textureCoords[0] = Eigen::Vector2f(1.0f, 0.0f);
			textureCoords[1] = Eigen::Vector2f(1.0f, 1.0f);
			textureCoords[2] = Eigen::Vector2f(0.0f, 1.0f);
			textureCoords[3] = Eigen::Vector2f(0.0f, 0.0f);
		}

		// Indices
		{
			indices[0] = 0;
			indices[1] = 3;
			indices[2] = 1;
			indices[3] = 1;
			indices[4] = 3;
			indices[5] = 2;
		}

		quadMeshData.Create(points, 4, textureCoords, 4, indices, 6);

		stackAllocator.Free(indicesBuff);
		stackAllocator.Free(textureCoordsBuff);
		stackAllocator.Free(pointsBuff);
	}
}
