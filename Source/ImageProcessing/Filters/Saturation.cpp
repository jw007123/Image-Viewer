#include "ImageProcessing/Filters/Saturation.h"

namespace ImageProcessing::Filters
{
	Saturation::Saturation()
	{
		rgbwVals.setConstant(Consts::defaultValue);
	}


	void Saturation::UpdateRequests(const GUI::Options::Saturation::Status& request_)
	{
		if (request_.flags == GUI::Options::Saturation::Status::NoOp)
		{
			return;
		}

		// -1 to match RGBW
		rgbwVals(request_.flags - 1) = request_.newSatValue;
	}


	void Saturation::ApplyFilter(Image& image_)
	{

	}


	inline bool Saturation::ValueChanged(const f32 val_) const
	{
		return std::abs(val_ - Consts::defaultValue) > std::numeric_limits<f32>::epsilon();
	}
}
