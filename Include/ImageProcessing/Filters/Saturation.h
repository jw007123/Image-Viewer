#pragma once

#include "Eigen/Core"

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"
#include "Utility/StackAllocator.h"

#include "GUI/Options/Saturation.h"

#include "ImageProcessing/Image.h"

namespace ImageProcessing::Filters
{
	class Saturation
	{
	public:
		Saturation();

		void UpdateRequests(const GUI::Options::Saturation::Status& request_);
		void ApplyFilter(Image& image_);

	private:
		u8				updatedFlags[4];
		Eigen::Vector4f rgbwVals;
	};
}
