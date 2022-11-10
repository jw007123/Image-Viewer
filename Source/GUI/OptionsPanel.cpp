#include "GUI/OptionsPanel.h"

namespace GUI
{
	OptionsPanel::OptionsPanel(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_) :
							   heapAllocator(heapAllocator_),
							   stackAllocator(stackAllocator_)
	{
	}


	OptionsPanel::Status OptionsPanel::Draw()
	{
		Status panelStatus;

		StartFrame();
		{
			ImGui::Text("Image Filters and Options");
			ImGui::Separator();

			DrawHSL(panelStatus);
		}
		EndFrame();

		return panelStatus;
	}


	void OptionsPanel::DrawHSL(Status& status_)
	{
		if (ImGui::TreeNode("HSL##OptionsPanel"))
		{
			if (ImGui::TreeNode("Hue##OptionsPanel"))
			{
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Saturation##OptionsPanel"))
			{
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Luminance##OptionsPanel"))
			{
				status_.lumStatus = luminanceOptions.Draw();

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}


	void OptionsPanel::StartFrame()
	{
		ImGuiViewport* vp = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(ImVec2(SizeConsts::viewportWidth, vp->WorkPos.y));
		ImGui::SetNextWindowSize(ImVec2(SizeConsts::optionsPanelWidth, SizeConsts::optionsPanelHeight - vp->WorkPos.y));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("##Options Panel", nullptr, ImGuiWindowFlags_NoResize   |
											     ImGuiWindowFlags_NoTitleBar |
											     ImGuiWindowFlags_NoMove	 );
		ImGui::PopStyleVar(3);
	}


	void OptionsPanel::EndFrame()
	{
		ImGui::End();
	}
}
