#include "ImageProcessing/LuminanceFilter.h"

namespace ImageProcessing
{
	LuminanceFilter::LuminanceFilter()
	{
		memset(updatedFlags, 0, sizeof(updatedFlags));
	}


	void LuminanceFilter::UpdateRequests(const GUI::LuminanceOptions::Status& request_)
	{
		switch (request_.flags)
		{
			case GUI::LuminanceOptions::Status::RChange:
			{
				rgbwVals.x()	= request_.newLumValue;
				updatedFlags[0] = 1;
				break;
			}

			case GUI::LuminanceOptions::Status::GChange:
			{
				rgbwVals.y()	= request_.newLumValue;
				updatedFlags[1] = 1;
				break;
			}

			case GUI::LuminanceOptions::Status::BChange:
			{
				rgbwVals.z()	= request_.newLumValue;
				updatedFlags[2] = 1;
				break;
			}

			case GUI::LuminanceOptions::Status::WChange:
			{
				rgbwVals.w()	= request_.newLumValue;
				updatedFlags[3] = 1;
				break;
			}

			default:
				assert(0);
		}
	}


	void LuminanceFilter::ApplyFilter(Image& image_)
	{
		for (u8 i = 0; i < 4; ++i)
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
					const u8 newVal = std::clamp<u8>(oldVal * (rgbwVals(i) * 2.0f), 0, 255);
					image_.Set(j, k, i, newVal);
				}
			}
		}

		memset(updatedFlags, 0, sizeof(updatedFlags));
	}
}
