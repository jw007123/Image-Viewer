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

			case GUI::LuminanceOptions::Status::RChange:
			{
				rgbwVals.y()	= request_.newLumValue;
				updatedFlags[1] = 1;
				break;
			}

			case GUI::LuminanceOptions::Status::RChange:
			{
				rgbwVals.z()	= request_.newLumValue;
				updatedFlags[2] = 1;
				break;
			}

			case GUI::LuminanceOptions::Status::RChange:
			{
				rgbwVals.w()	= request_.newLumValue;
				updatedFlags[3] = 1;
				break;
			}

			default:
				assert(0);
		}
	}


	void LuminanceFilter::ApplyFilter(Image& image_) const
	{
		// Do stuff

		memset(updatedFlags, 0, sizeof(updatedFlags));
	}
}
