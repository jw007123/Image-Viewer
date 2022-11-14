#pragma once

#include <malloc.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"

#include "ImageProcessing/Image.h"

#include <nfd.h>

#include "imgui.h"

namespace GUI
{
	class MainMenu
	{
	public:
		struct Status
		{
			enum Flags : u8
			{
				NoOp = 0,
				Open = 1,
				Save = 2
			} flags;

			char(*openFilePathPtr)[PATH_MAX_LEN];
			char(*saveFilePathPtr)[PATH_MAX_LEN];

			Status();
		};

		MainMenu(ImageProcessing::Image& inputImage_, ImageProcessing::Image& outputImage_);

		Status Draw();

	private:
		ImageProcessing::Image& inputImage;
		ImageProcessing::Image& outputImage;

		char openFilePath[PATH_MAX_LEN];
		char saveFilePath[PATH_MAX_LEN];

		bool HandleOpen();
		bool HandleSave();
	};
}
