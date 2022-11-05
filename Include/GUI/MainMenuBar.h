#pragma once

#include <malloc.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"

#include "GUI/FileMenu.h"

#include "ImageProcessing/Image.h"

#include "imgui.h"

namespace GUI
{
	class MainMenuBar
	{
	public:
		MainMenuBar(ImageProcessing::Image* image_);

		void Draw();

	private:
		ImageProcessing::Image* image;
		FileMenu				fileMenu;

		void HandleFileMenuStatus(const FileMenu::Status& status_);
	};
}
