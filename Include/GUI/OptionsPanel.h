#pragma once

#include <malloc.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"
#include "Utility/HeapAllocator.h"

#include "GUI/SizeConsts.h"
#include "GUI/LuminanceOptions.h"

#include "ImageProcessing/Image.h"

#include "imgui.h"

namespace GUI
{
	class OptionsPanel
	{
	public:
		struct Status
		{
			LuminanceOptions::Status lumStatus;
		};

		OptionsPanel(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, ImageProcessing::Image& image_);

		Status Draw();

	private:
		Utility::HeapAllocator&  heapAllocator;
		Utility::StackAllocator& stackAllocator;
		ImageProcessing::Image&  image;

		LuminanceOptions luminanceOptions;

		void DrawHSL(Status& status_);

		void StartFrame();
		void EndFrame();
	};
}
