#include "GUI/MainMenuBar.h"

namespace GUI
{
	MainMenuBar::MainMenuBar(std::mutex& imageReadMutex_, ImageProcessing::Image& image_) :
							 imageReadMutex(imageReadMutex_),
							 image(image_)
	{
	}


	MainMenuBar::Status MainMenuBar::Draw()
	{
		Status barStatus;

		if (ImGui::BeginMainMenuBar())
		{
			barStatus.fileStatus = fileMenu.Draw();
			HandleFileMenuStatus(barStatus.fileStatus);

			ImGui::EndMainMenuBar();
		}

		return barStatus;
	}


	bool MainMenuBar::HandleFileMenuStatus(const FileMenu::Status& status_)
	{
		if (status_.flags == FileMenu::Status::Flags::Open)
		{
			imageReadMutex.lock();
			{
				if (!image.Load((*status_.openFilePathPtr)))
				{
					Utility::Log(Utility::LogFlag::Warn, "Failed to load %s", (*status_.openFilePathPtr));
				}
			}
			imageReadMutex.unlock();

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
