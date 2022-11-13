#include "GUI/ZoomViewport.h"

namespace GUI
{
	ZoomViewport::ZoomViewport(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, Rendering::OpenGLRenderer& glRenderer_) :
							  glFramebuffer(SizeConsts::viewportWidth * (1.0f - SizeConsts::viewportOptionsRatioX),
											SizeConsts::viewportWidth * (1.0f - SizeConsts::viewportOptionsRatioX)),
							  camera(1.0f),
							  glRenderer(glRenderer_),
							  heapAllocator(heapAllocator_),
							  stackAllocator(stackAllocator_)
	{
		width  = SizeConsts::viewportWidth * (1.0f - SizeConsts::viewportOptionsRatioX);
		height = SizeConsts::viewportWidth * (1.0f - SizeConsts::viewportOptionsRatioX);
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
		ImGuiViewport* vp = ImGui::GetMainViewport();

		const ImVec2 winSize = ImVec2(vp->WorkSize.x * (1.0f - SizeConsts::viewportOptionsRatioX),
									  vp->WorkSize.x * (1.0f - SizeConsts::viewportOptionsRatioX));
		const ImVec2 winPos  = ImVec2(vp->WorkSize.x - winSize.x, vp->WorkSize.y - winSize.y);

		ImGui::SetNextWindowPos(winPos);
		ImGui::SetNextWindowSize(winSize);
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("##Zoom viewport", NULL, ImGuiWindowFlags_NoTitleBar  |
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

		// Update camera. Override z so that we get a zoomed-in look
		camera.UpdateFixed(Eigen::Vector3f(centrePos_.x(), centrePos_.y(), Consts::zoomValue),
						   Eigen::Vector2f(winPos.x, winPos.y), width, height);
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
