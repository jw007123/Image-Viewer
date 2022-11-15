#include "GUI/Viewport.h"

namespace GUI
{
	Viewport::Viewport(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, Rendering::OpenGLRenderer& glRenderer_) :
					   glFramebuffer(SizeConsts::viewportWidth, SizeConsts::viewportHeight),
					   camera((f32)SizeConsts::viewportWidth / SizeConsts::viewportHeight, -1.0f),
					   glRenderer(glRenderer_),
					   heapAllocator(heapAllocator_),
					   stackAllocator(stackAllocator_)
	{
		width  = SizeConsts::viewportWidth;
		height = SizeConsts::viewportHeight;

		imguiViewport = ImGui::GetMainViewport();

		zoomInfo.holdEnabled = false;
	}


	Viewport::Status Viewport::Draw()
	{
		Status viewportStatus;

		StartFrame(viewportStatus);
		{
			viewportStatus.cameraPos = HandleZoomInfo();
		}
		EndFrame(viewportStatus);

		return viewportStatus;
	}


	void Viewport::StartFrame(Status& status_)
	{
		// 1.25f for weird ImGui padding...
		ImVec2 winSize      = imguiViewport->WorkSize;
		winSize.x		   *= SizeConsts::viewportOptionsRatioX;
		const ImVec2 winPos = ImVec2(imguiViewport->WorkPos.x, imguiViewport->WorkPos.y * 1.25f);

		ImGui::SetNextWindowPos(winPos);
		ImGui::SetNextWindowSize(winSize);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("##Main viewport", NULL, ImGuiWindowFlags_NoResize	   |
											  ImGuiWindowFlags_NoTitleBar  |
											  ImGuiWindowFlags_NoScrollbar |
											  ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::PopStyleVar(3);

		// Resize internal structs if viewport resized by user
		if (winSize.x != width || winSize.y != height)
		{
			width  = winSize.x;
			height = winSize.y;

			glFramebuffer.Resize(width, height);
		}

		// Update camera
		if (ImGui::IsWindowHovered())
		{
			camera.UpdateFree(ImGui::GetIO(), Eigen::Vector2f(winPos.x, winPos.y), width, height);
		}
		else
		{
			camera.UpdateProjection(width, height);
		}
	}


	void Viewport::EndFrame(Status& status_)
	{
		// Render to texture. Could add framebuffer to renderer, but keep this way for genericism
		glFramebuffer.StartFrame();
		{
			glRenderer.RenderFullView(camera, (f32)width / height);
		}
		glFramebuffer.EndFrame();

		ImGui::End();
	}


	Eigen::Vector3f Viewport::HandleZoomInfo()
	{
		ImGuiIO& io				  = ImGui::GetIO();
		Eigen::Vector3f cameraPos = Eigen::Vector3f::Zero();

		if (ImGui::IsWindowHovered())
		{
			if (zoomInfo.holdEnabled)
			{
				cameraPos = zoomInfo.lastCameraPos;
			}
			else
			{
				const Eigen::Vector2f mousePos = Eigen::Vector2f(io.MousePos.x, io.MousePos.y);
				cameraPos					   = camera.CalculateMousePosition(mousePos, width, height);
				zoomInfo.lastCameraPos		   = cameraPos;
			}

			// Toggle position hold
			zoomInfo.holdEnabled = !io.MouseDown[0];
		}
		else
		{
			cameraPos = zoomInfo.lastCameraPos;
		}

		return cameraPos;
	}
}
