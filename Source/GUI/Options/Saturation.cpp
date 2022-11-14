#include "GUI/Options/Saturation.h"

namespace GUI::Options
{
	Saturation::Status::Status()
	{
		flags = NoOp;
	}


	Saturation::Saturation()
	{
		satValsRGBW.setConstant(0.5f);
	}


	Saturation::Status Saturation::Draw()
	{
		Status optionStatus;

		if (ImGui::SliderFloat("Red", &satValsRGBW.x(), 0.0f, 2.0f))
		{
			optionStatus.flags		 = Status::RChange;
			optionStatus.newSatValue = satValsRGBW.x();
		}

		if (ImGui::SliderFloat("Green", &satValsRGBW.y(), 0.0f, 2.0f))
		{
			optionStatus.flags		 = Status::GChange;
			optionStatus.newSatValue = satValsRGBW.y();
		}

		if (ImGui::SliderFloat("Blue", &satValsRGBW.z(), 0.0f, 2.0f))
		{
			optionStatus.flags		 = Status::BChange;
			optionStatus.newSatValue = satValsRGBW.z();
		}

		if (ImGui::SliderFloat("White", &satValsRGBW.w(), 0.0f, 2.0f))
		{
			optionStatus.flags		 = Status::WChange;
			optionStatus.newSatValue = satValsRGBW.w();
		}

		return optionStatus;
	}
}
