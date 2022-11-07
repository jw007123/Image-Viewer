#include "Rendering/OpenGLRenderer.h"

namespace Rendering
{
	OpenGLRenderer::MeshOpenGLData::MeshOpenGLData()
	{
		VAO = GL_INVALID_INDEX;
		EBO = GL_INVALID_INDEX;

		vertexVBO  = GL_INVALID_INDEX;
		textureVBO = GL_INVALID_INDEX;
	}


	OpenGLRenderer::MeshOpenGLData::~MeshOpenGLData()
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


	void OpenGLRenderer::MeshOpenGLData::Create(Eigen::Vector3f* points_,		 const usize nPoints_,
												Eigen::Vector2f* textureCoords_, const usize nTextureCoords_,
												GLuint* indices_,				 const usize nIndices_)
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



	OpenGLRenderer::OpenGLRenderer(Utility::StackAllocator* stackAllocator_, Utility::HeapAllocator* heapAllocator_)
	{
		stackAllocator = stackAllocator_;
		heapAllocator  = heapAllocator_;

		{
			OpenGLShader vsTexture;
			vsTexture.Load(stackAllocator, OpenGLShader::Type::Vertex, "vsTexture");

			OpenGLShader fsTexture;
			fsTexture.Load(stackAllocator, OpenGLShader::Type::Fragment, "fsTexture");

			programs[ProgramID::Texture].Load(vsTexture, fsTexture);
		}

		{
			OpenGLShader vsBGround;
			vsBGround.Load(stackAllocator, OpenGLShader::Type::Vertex, "vsBGround");

			OpenGLShader fsBGround;
			fsBGround.Load(stackAllocator, OpenGLShader::Type::Fragment, "fsBGround");

			programs[ProgramID::BGround].Load(vsBGround, fsBGround);
		}

		LoadTextureMesh();
	}


	void OpenGLRenderer::Render(const GUI::Camera& cam_, const ImageProcessing::Image* const image_, const f32 aspectRatio_)
	{
		texture.Update((u8*)image_->GetData().ptr, image_->GetWidth(), image_->GetHeight());

		const GLuint textureIdx = texture.GetTextureIdx();
		if (textureIdx != GL_INVALID_INDEX)
		{
			RenderTexture(cam_);
		}

		RenderBGround(cam_, aspectRatio_);
	}


	void OpenGLRenderer::RenderTexture(const GUI::Camera& cam_)
	{
		// Use correct program
		programs[ProgramID::Texture].Use();

		glBindVertexArray(quadMeshData.VAO);
		{
			// Set uniforms
			const GLuint viewToProjLoc   = programs[ProgramID::Texture].GetUniformLoc("viewToProj");
			const GLuint worldToViewLoc  = programs[ProgramID::Texture].GetUniformLoc("worldToView");
			const GLuint modelToWorldLoc = programs[ProgramID::Texture].GetUniformLoc("modelToWorld");

			// Calculate texture model mat
			Eigen::Matrix4f modelToWorld = Eigen::Matrix4f::Identity();
			modelToWorld(0, 0)			 = (f32)texture.GetWidth() / texture.GetHeight();

			glUniformMatrix4fv(viewToProjLoc,   1, false, cam_.GetViewToProj().data());
			glUniformMatrix4fv(worldToViewLoc,  1, false, cam_.GetWorldToView().data());
			glUniformMatrix4fv(modelToWorldLoc, 1, false, modelToWorld.data());

			// Bind texture. For now, we're just binding one texture a time and can use the 0th unit
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture.GetTextureIdx());

			// Draw
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
	}


	void OpenGLRenderer::RenderBGround(const GUI::Camera& cam_, const f32 aspectRatio_)
	{
		// Use correct program
		programs[ProgramID::BGround].Use();

		glBindVertexArray(quadMeshData.VAO);
		{
			// Set uniforms
			const GLuint cameraPosLoc   = programs[ProgramID::BGround].GetUniformLoc("cameraPos");
			const GLuint aspectRatioLoc = programs[ProgramID::BGround].GetUniformLoc("aspectRatio");

			// Set uniforms
			const Eigen::Vector3f cameraPos = cam_.GetWorldToView().block<3, 1>(0, 3);
			glUniform3f(cameraPosLoc,	cameraPos.x(), cameraPos.y(), cameraPos.z());
			glUniform1f(aspectRatioLoc, aspectRatio_);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
	}


	void OpenGLRenderer::LoadTextureMesh()
	{
		Utility::MemoryBlock pointsBuff		   = stackAllocator->Allocate<Eigen::Vector3f>(4);
		Utility::MemoryBlock textureCoordsBuff = stackAllocator->Allocate<Eigen::Vector2f>(4);
		Utility::MemoryBlock indicesBuff	   = stackAllocator->Allocate<GLuint>(6);

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

		stackAllocator->Free(indicesBuff);
		stackAllocator->Free(textureCoordsBuff);
		stackAllocator->Free(pointsBuff);
	}
}
