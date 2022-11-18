#include "GUI/ZoomViewport.h"

namespace GUI
{
	ZoomViewport::ZoomViewport(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, Rendering::Renderer& renderer_) :
							  framebuffer(SizeConsts::viewportWidth * (1.0f - SizeConsts::viewportOptionsRatioX),
											SizeConsts::viewportWidth * (1.0f - SizeConsts::viewportOptionsRatioX)),
							  camera(1.0f, Consts::zoomValue),
							  renderer(renderer_),
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

			framebuffer.Resize(width, height);
		}

		// Update camera. If window hovered, let user control zoom
		if (ImGui::IsWindowHovered())
		{
			camera.UpdateFixed(ImGui::GetIO(), Eigen::Vector2f(centrePos_.x(), centrePos_.y()),
											   Eigen::Vector2f(winPos.x, winPos.y), width, height);
		}
		else
		{
			camera.UpdateFixed(Eigen::Vector3f(centrePos_.x(), centrePos_.y(), camera.GetWorldToView()(2, 3)),
							   Eigen::Vector2f(winPos.x, winPos.y), width, height);
		}
	}


	void ZoomViewport::EndFrame()
	{
		// Render to texture. Could add framebuffer to renderer, but keep this way for genericism
		framebuffer.StartFrame();
		{
			renderer.RenderZoomView(camera);
		}
		framebuffer.EndFrame();

		ImGui::End();
	}
}
