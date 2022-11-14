#pragma once

#include <malloc.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"
#include "Utility/HeapAllocator.h"

#include "GUI/SizeConsts.h"
#include "GUI/Options/Luminance.h"
#include "GUI/Options/Saturation.h"

#include "ImageProcessing/Image.h"

#include "imgui.h"

namespace GUI
{
	class OptionsPanel
	{
	public:
		struct Status
		{
			Options::Luminance::Status  lumStatus;
			Options::Saturation::Status satStatus;
		};

		OptionsPanel(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_);

		Status Draw();

	private:
		Utility::HeapAllocator&  heapAllocator;
		Utility::StackAllocator& stackAllocator;

		Options::Luminance  luminanceOptions;
		Options::Saturation saturationOptions;

		void DrawHSL(Status& status_);

		void StartFrame();
		void EndFrame();
	};
}
