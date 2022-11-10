#include "GUI/Viewport.h"

namespace GUI
{
	Viewport::Viewport(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, Rendering::OpenGLRenderer& glRenderer_) :
					   glFramebuffer(SizeConsts::viewportWidth, SizeConsts::viewportHeight),
					   camera((f32)SizeConsts::viewportWidth / SizeConsts::viewportHeight),
					   glRenderer(glRenderer_),
					   heapAllocator(heapAllocator_),
					   stackAllocator(stackAllocator_)
	{
		width  = SizeConsts::viewportWidth;
		height = SizeConsts::viewportHeight;

		imguiViewport = ImGui::GetMainViewport();
	}


	Viewport::Status Viewport::Draw()
	{
		Status viewportStatus;

		StartFrame();
		{
			viewportStatus.cameraPos = camera.GetWorldToView().block<3, 1>(0, 3);
		}
		EndFrame();

		return viewportStatus;
	}


	void Viewport::StartFrame()
	{
		ImVec2 winSize      = imguiViewport->WorkSize;
		winSize.x		   -= SizeConsts::optionsPanelWidth;
		const ImVec2 winPos = imguiViewport->WorkPos;

		ImGui::SetNextWindowPos(winPos);
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


	void Viewport::EndFrame()
	{
		// Render to texture. Could add framebuffer to renderer, but keep this way for genericism
		glFramebuffer.StartFrame();
		{
			glRenderer.RenderFullView(camera, (f32)width / height);
		}
		glFramebuffer.EndFrame();

		ImGui::End();
	}
}
