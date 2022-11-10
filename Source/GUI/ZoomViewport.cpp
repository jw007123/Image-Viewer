#include "GUI/ZoomViewport.h"

namespace GUI
{
	ZoomViewport::ZoomViewport(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, Rendering::OpenGLRenderer& glRenderer_) :
							  glFramebuffer(SizeConsts::optionsPanelWidth, SizeConsts::zoomWiewportHeight),
							  camera((f32)SizeConsts::optionsPanelWidth / SizeConsts::zoomWiewportHeight),
							  glRenderer(glRenderer_),
							  heapAllocator(heapAllocator_),
							  stackAllocator(stackAllocator_)
	{
		imguiViewport = ImGui::GetMainViewport();
		width		  = SizeConsts::optionsPanelWidth;
		height		  = SizeConsts::optionsPanelWidth;
	}


	ZoomViewport::Status ZoomViewport::Draw(const Eigen::Vector3f& centrePos_)
	{
		Status viewportStatus;

		StartFrame(centrePos_);
		{
			// Additional ImGui code
		}
		EndFrame();

		return viewportStatus;
	}


	void ZoomViewport::StartFrame(const Eigen::Vector3f& centrePos_)
	{
		const ImVec2 winSize  = ImVec2(SizeConsts::optionsPanelWidth, SizeConsts::zoomWiewportHeight);
		const ImVec2 winPos   = ImVec2(imguiViewport->WorkSize.x - SizeConsts::optionsPanelWidth, imguiViewport->Pos.y + SizeConsts::optionsPanelHeight);

		ImGui::SetNextWindowPos(winPos);
		ImGui::SetNextWindowSize(winSize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("##Zoom viewport", NULL, ImGuiWindowFlags_NoResize	   |
											  ImGuiWindowFlags_NoTitleBar  |
											  ImGuiWindowFlags_NoScrollbar |
											  ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::PopStyleVar(3);

		// Resize internal structs if viewport resized by user
		if (winSize.x != width || winSize.y != height)
		{
			width = winSize.x;
			height = winSize.y;

			glFramebuffer.Resize(width, height);
		}

		// Update camera
		camera.UpdateFixed(centrePos_, Eigen::Vector2f(winPos.x, winPos.y), width, height);
	}


	void ZoomViewport::EndFrame()
	{
		// Render to texture. Could add framebuffer to renderer, but keep this way for genericism
		glFramebuffer.StartFrame();
		{
			glRenderer.RenderZoomView(camera);
		}
		glFramebuffer.EndFrame();

		ImGui::End();
	}
}
