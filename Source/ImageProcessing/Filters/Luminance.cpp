#include "ImageProcessing/Filters/Luminance.h"

namespace ImageProcessing::Filters
{
	Luminance::Luminance()
	{
		Reset();
	}


	void Luminance::Reset()
	{
		rgbwVals.setConstant(Consts::defaultValue);
	}


	void Luminance::UpdateRequests(const GUI::Options::Luminance::Status& request_)
	{
		if (request_.flags == GUI::Options::Luminance::Status::NoOp)
		{
			return;
		}

		// -1 to match RGBW
		rgbwVals(request_.flags - 1) = request_.newLumValue;
	}


	void Luminance::ApplyFilter(Image& image_)
	{
		// RGB case
		for (u8 i = 0; i < 3; ++i)
		{
			if (!ValueChanged(rgbwVals(i)))
			{
				continue;
			}

			for (usize j = 0; j < image_.GetWidth(); ++j)
			{
				for (usize k = 0; k < image_.GetHeight(); ++k)
				{
					const u8 oldVal = image_.Get(j, k, i);
					const u8 newVal = std::clamp<f32>(oldVal * (rgbwVals(i) * 2.0f), 0.0f, 255.0f);
					image_.Set(j, k, i, newVal);
				}
			}
		}

		// White case
		if (ValueChanged(rgbwVals(3)))
		{
			for (usize i = 0; i < image_.GetWidth(); ++i)
			{
				for (usize j = 0; j < image_.GetHeight(); ++j)
				{
					for (u8 k = 0; k < 3; ++k)
					{
						const u8 oldVal = image_.Get(i, j, k);
						const u8 newVal = std::clamp<f32>(oldVal * (rgbwVals(3) * 2.0f), 0.0f, 255.0f);
						image_.Set(i, j,  k, newVal);
					}
				}
			}
		}
	}


	inline bool Luminance::ValueChanged(const f32 val_) const
	{
		return std::abs(val_ - Consts::defaultValue) > std::numeric_limits<f32>::epsilon();
	}
}
