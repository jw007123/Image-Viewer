#include "GUI/FileMenu.h"

namespace GUI
{
	FileMenu::Status::Status()
	{
		flags		    = Status::Flags::NoOp;
		openFilePathPtr = nullptr;
		saveFilePathPtr = nullptr;
	}


	FileMenu::Status FileMenu::Draw()
	{
		Status menuStatus;

		if (ImGui::BeginMenu("File Options##FileMenu_ImGui"))
		{
			if (ImGui::MenuItem("Open##FileMenu_ImGui"))
			{
				if (HandleOpen())
				{
					menuStatus.flags		   = Status::Flags::Open;
					menuStatus.openFilePathPtr = &openFilePath;
				}
			}

			if (ImGui::MenuItem("Save##FileMenu_ImGui"))
			{
				if (HandleSave())
				{
					menuStatus.flags		   = Status::Flags::Save;
					menuStatus.saveFilePathPtr = &saveFilePath;
				}
			}

			ImGui::EndMenu();
		}

		return menuStatus;
	}


	bool FileMenu::HandleOpen()
	{
		// NFD_OpenDialog will stall execution until user input
		nfdchar_t* pathNfd			= nullptr;
		const nfdresult_t resultNfd = NFD_OpenDialog(nullptr, nullptr, &pathNfd);

		if (resultNfd == NFD_OKAY)
		{
			memcpy(openFilePath, pathNfd, sizeof(openFilePath));
		}
		else if (resultNfd == NFD_CANCEL)
		{
			// User cancelled. Nothing to do
		}
		else
		{
			Utility::Log(Utility::LogFlag::Error, "GUI::FileMenu::HandleOpen() NFD_OpenDialog error!");
			assert(0);
		}

		return (resultNfd == NFD_OKAY);
	}


	bool FileMenu::HandleSave()
	{
		// NFD_SaveDialog will stall execution until user input
		nfdchar_t* pathNfd		    = nullptr;
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
			Utility::Log(Utility::LogFlag::Error, "GUI::FileMenu::HandleOpen() NFD_OpenDialog error!");
			assert(0);
		}

		return (resultNfd == NFD_OKAY);
	}
}
