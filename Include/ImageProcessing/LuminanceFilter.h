#pragma once

#include "Eigen/Core"

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"
#include "Utility/StackAllocator.h"

#include "GUI/LuminanceOptions.h"

#include "ImageProcessing/Image.h"

namespace ImageProcessing
{
	class LuminanceFilter
	{
	public:
		LuminanceFilter();

		void UpdateRequests(const GUI::LuminanceOptions::Status& request_);
		void ApplyFilter(Image& image_) const;

	private:
		u8				updatedFlags[4];
		Eigen::Vector4f rgbwVals;
	};
}
