#include "GUI/MainMenu.h"

namespace GUI
{
	MainMenu::Status::Status()
	{
		flags			= NoOp;
		openFilePathPtr = nullptr;
		saveFilePathPtr = nullptr;
	}


	MainMenu::MainMenu(ImageProcessing::Image& inputImage_, ImageProcessing::Image& outputImage_) :
					   inputImage(inputImage_),
					   outputImage(outputImage_)
	{
	}


	MainMenu::Status MainMenu::Draw()
	{
		Status menuStatus;

		ImGui::Begin("Main Menu");
		{
			if (ImGui::BeginMenu("File Options##FileMenu_ImGui"))
			{
				if (ImGui::MenuItem("Open##FileMenu_ImGui"))
				{
					if (HandleOpen())
					{
						menuStatus.flags		   = Status::Open;
						menuStatus.openFilePathPtr = &openFilePath;
					}
				}

				if (ImGui::MenuItem("Save##FileMenu_ImGui"))
				{
					if (HandleSave())
					{
						menuStatus.flags		   = Status::Save;
						menuStatus.saveFilePathPtr = &saveFilePath;
					}
				}

				ImGui::EndMenu();
			}
		}
		ImGui::End();

		if (menuStatus.flags == MainMenu::Status::Open)
		{
			if (!inputImage.Load((*menuStatus.openFilePathPtr)))
			{
				Utility::Log(Utility::Warn, "Failed to load %s", (*menuStatus.openFilePathPtr));
			}
		}
		else if (menuStatus.flags == MainMenu::Status::Save)
		{
			if (!outputImage.Save((*menuStatus.saveFilePathPtr)))
			{
				Utility::Log(Utility::Warn, "Failed to save %s", (*menuStatus.saveFilePathPtr));
			}
		}
		else
		{
			// Nothing to do
		}

		return menuStatus;
	}


	bool MainMenu::HandleOpen()
	{
		// NFD_OpenDialog will stall execution until user input
		nfdchar_t* pathNfd = nullptr;
		const nfdresult_t resultNfd = NFD_OpenDialog(nullptr, nullptr, &pathNfd);

		if (resultNfd == NFD_OKAY)
		{
			memcpy(openFilePath, pathNfd, sizeof(openFilePath));
			return true;
		}
		else if (resultNfd == NFD_CANCEL)
		{
			// User cancelled. Nothing to do
		}
		else
		{
			Utility::Log(Utility::Error, "GUI::MainMenu::HandleOpen() NFD_OpenDialog error!");
			assert(0);
		}

		return false;
	}


	bool MainMenu::HandleSave()
	{
		// NFD_SaveDialog will stall execution until user input
		nfdchar_t* pathNfd = nullptr;
		const nfdresult_t resultNfd = NFD_SaveDialog(nullptr, nullptr, &pathNfd);

		if (resultNfd == NFD_OKAY)
		{
			memcpy(saveFilePath, pathNfd, sizeof(saveFilePath));
		}
		else if (resultNfd == NFD_CANCEL)
		{
			// User cancelled. Nothing to do
		}
		else
		{
			Utility::Log(Utility::Error, "GUI::MainMenu::HandleOpen() NFD_OpenDialog error!");
			assert(0);
		}

		return (resultNfd == NFD_OKAY);
	}
}
