#include "GUI/Options/Luminance.h"

namespace GUI::Options
{
	Luminance::Status::Status()
	{
		flags = NoOp;
	}


	Luminance::Luminance()
	{
		lumValsRGBW.setConstant(0.5f);
	}


	Luminance::Status Luminance::Draw()
	{
		Status optionStatus;

		if (ImGui::SliderFloat("Red", &lumValsRGBW.x(), 0.0f, 2.0f))
		{
			optionStatus.flags		 = Status::RChange;
			optionStatus.newLumValue = lumValsRGBW.x();
		}

		if (ImGui::SliderFloat("Green", &lumValsRGBW.y(), 0.0f, 2.0f))
		{
			optionStatus.flags		 = Status::GChange;
			optionStatus.newLumValue = lumValsRGBW.y();
		}

		if (ImGui::SliderFloat("Blue", &lumValsRGBW.z(), 0.0f, 2.0f))
		{
			optionStatus.flags		 = Status::BChange;
			optionStatus.newLumValue = lumValsRGBW.z();
		}

		if (ImGui::SliderFloat("White", &lumValsRGBW.w(), 0.0f, 2.0f))
		{
			optionStatus.flags		 = Status::WChange;
			optionStatus.newLumValue = lumValsRGBW.w();
		}

		return optionStatus;
	}
}
