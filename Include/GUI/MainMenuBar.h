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
		struct Status
		{
			enum Flags : u8
			{
				NoOp	   = 0,
				NewTexture = 1
			} flags;

			Status();
		};

		MainMenuBar(ImageProcessing::Image* image_);

		Status Draw();

	private:
		ImageProcessing::Image* image;
		FileMenu				fileMenu;

		/// Returns true if 'image' was updated
		bool HandleFileMenuStatus(const FileMenu::Status& status_);
	};
}