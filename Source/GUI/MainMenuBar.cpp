#include "GUI/MainMenuBar.h"

namespace GUI
{
	MainMenuBar::MainMenuBar(ImageProcessing::Image& image_) :
							 image(image_)
	{
	}


	MainMenuBar::Status MainMenuBar::Draw()
	{
		Status barStatus;

		if (ImGui::Begin("Main Menu"))
		{
			barStatus.fileStatus = fileMenu.Draw();
			HandleFileMenuStatus(barStatus.fileStatus);

			ImGui::End();
		}

		return barStatus;
	}


	bool MainMenuBar::HandleFileMenuStatus(const FileMenu::Status& status_)
	{
		if (status_.flags == FileMenu::Status::Flags::Open)
		{
			if (!image.Load((*status_.openFilePathPtr)))
			{
				Utility::Log(Utility::LogFlag::Warn, "Failed to load %s", (*status_.openFilePathPtr));
			}

			return true;
		}
		else if (status_.flags == FileMenu::Status::Flags::Save)
		{
			if (!image.Save((*status_.saveFilePathPtr)))
			{
				Utility::Log(Utility::LogFlag::Warn, "Failed to save %s", (*status_.saveFilePathPtr));
			}
		}
		else
		{
			// Nothing to do
		}

		return false;
	}
}
