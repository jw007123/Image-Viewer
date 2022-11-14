#include "ImageProcessing/Filters/Saturation.h"

namespace ImageProcessing::Filters
{
	Saturation::Saturation()
	{
		rgbwVals.setConstant(0.5f);

		memset(updatedFlags, 0, sizeof(updatedFlags));
	}


	void Saturation::UpdateRequests(const GUI::Options::Saturation::Status& request_)
	{
		if (request_.flags == GUI::Options::Saturation::Status::NoOp)
		{
			return;
		}

		// -1 to match RGBW
		rgbwVals(request_.flags - 1)	 = request_.newSatValue;
		updatedFlags[request_.flags - 1] = 1;
	}


	void Saturation::ApplyFilter(Image& image_)
	{

		memset(updatedFlags, 0, sizeof(updatedFlags));
	}
}
