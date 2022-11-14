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
		struct Consts
		{
			static constexpr f32 defaultValue = 0.5f;
		};

		Eigen::Vector4f rgbwVals;

		inline bool ValueChanged(const f32 val_) const;
	};
}
