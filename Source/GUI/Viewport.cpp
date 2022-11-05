#include "GUI/Viewport.h"

namespace GUI
{
	Viewport::Viewport(const char* name_, ImageProcessing::Image* image_)
			 : framebuffer(1280, 720),
			   camera((f32)1280 / 720)
	{
		image  = image_;
		width  = 1280;
		height = 720;

		strcpy(name, name_);
		imguiViewport = ImGui::GetMainViewport();
	}


	void Viewport::StartFrame()
	{
		const ImVec2 winSize = imguiViewport->WorkSize;
		const ImVec2 winPos  = imguiViewport->WorkPos;

		ImGui::SetNextWindowPos(imguiViewport->Pos);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin(name, NULL, ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::PopStyleVar(3);

		// Resize internal structs if viewport resized by user
		if (winSize.x != width || winSize.y != height)
		{
			width = winSize.x;
			height = winSize.y;

			framebuffer.Resize(width, height);
		}

		// Update camera
		if (ImGui::IsWindowHovered())
		{
			camera.Update(ImGui::GetIO(), Eigen::Vector2f(winPos.x, winPos.y), width, height);
		}

		// Render the scene
		framebuffer.Use();
	}


	void Viewport::EndFrame()
	{
		framebuffer.RenderToTexture();
		framebuffer.Release();

		ImGui::End();
	}


	const Camera& Viewport::GetCamera() const
	{
		return camera;
	}
}
