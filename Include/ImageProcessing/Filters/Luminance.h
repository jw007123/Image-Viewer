#pragma once

#include "Eigen/Core"

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"
#include "Utility/StackAllocator.h"

#include "GUI/Options/Luminance.h"

#include "ImageProcessing/Image.h"

namespace ImageProcessing::Filters
{
	class Luminance
	{
	public:
		Luminance();

		void UpdateRequests(const GUI::Options::Luminance::Status& request_);
		void ApplyFilter(Image& image_);

	private:
		u8				updatedFlags[4];
		Eigen::Vector4f rgbwVals;
	};
}
