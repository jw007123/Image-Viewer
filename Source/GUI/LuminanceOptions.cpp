#include "GUI/LuminanceOptions.h"

namespace GUI
{
	LuminanceOptions::Status::Status()
	{
		flags = Flags::NoOp;
	}


	LuminanceOptions::LuminanceOptions()
	{
		lumValsRGBW.setConstant(0.5f);
	}


	LuminanceOptions::Status LuminanceOptions::Draw()
	{
		Status optionStatus;

		if (ImGui::SliderFloat("Red", &lumValsRGBW.x(), 0.0f, 1.0f))
		{
			optionStatus.flags		 = Status::Flags::RChange;
			optionStatus.newLumValue = lumValsRGBW.x();
		}

		if (ImGui::SliderFloat("Green", &lumValsRGBW.y(), 0.0f, 1.0f))
		{
			optionStatus.flags		 = Status::Flags::GChange;
			optionStatus.newLumValue = lumValsRGBW.y();
		}

		if (ImGui::SliderFloat("Blue", &lumValsRGBW.z(), 0.0f, 1.0f))
		{
			optionStatus.flags		 = Status::Flags::BChange;
			optionStatus.newLumValue = lumValsRGBW.z();
		}

		if (ImGui::SliderFloat("White", &lumValsRGBW.w(), 0.0f, 1.0f))
		{
			optionStatus.flags		 = Status::Flags::WChange;
			optionStatus.newLumValue = lumValsRGBW.w();
		}

		return optionStatus;
	}
}
