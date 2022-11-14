#include "ImageProcessing/Filters/Luminance.h"

namespace ImageProcessing::Filters
{
	Luminance::Luminance()
	{
		rgbwVals.setConstant(0.5f);

		memset(updatedFlags, 0, sizeof(updatedFlags));
	}


	void Luminance::UpdateRequests(const GUI::Options::Luminance::Status& request_)
	{
		if (request_.flags == GUI::Options::Luminance::Status::NoOp)
		{
			return;
		}

		// -1 to match RGBW
		rgbwVals(request_.flags - 1)	 = request_.newLumValue;
		updatedFlags[request_.flags - 1] = 1;
	}


	void Luminance::ApplyFilter(Image& image_)
	{
		// RGB case
		for (u8 i = 0; i < 3; ++i)
		{
			if (!updatedFlags[i])
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
		if (updatedFlags[3])
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

		memset(updatedFlags, 0, sizeof(updatedFlags));
	}
}
