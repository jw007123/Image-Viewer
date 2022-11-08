#include "GUI/LuminanceOptions.h"

namespace GUI
{
	LuminanceOptions::Status::Status()
	{
		flags = Flags::NoOp;
	}


	LuminanceOptions::LuminanceOptions()
	{
		lumValsRGB.setOnes();
	}


	LuminanceOptions::Status LuminanceOptions::Draw()
	{
		Status optionStatus;

		if (ImGui::SliderFloat("Red", &lumValsRGB.x(), 0.0f, 1.0f))
		{
			optionStatus.flags		 = Status::Flags::RChange;
			optionStatus.newLumValue = lumValsRGB.x();
		}

		if (ImGui::SliderFloat("Green", &lumValsRGB.y(), 0.0f, 1.0f))
		{
			optionStatus.flags		 = Status::Flags::GChange;
			optionStatus.newLumValue = lumValsRGB.y();
		}

		if (ImGui::SliderFloat("Blue", &lumValsRGB.z(), 0.0f, 1.0f))
		{
			optionStatus.flags		 = Status::Flags::BChange;
			optionStatus.newLumValue = lumValsRGB.z();
		}

		if (ImGui::SliderFloat("White", &lumValsRGB.w(), 0.0f, 1.0f))
		{
			optionStatus.flags		 = Status::Flags::WChange;
			optionStatus.newLumValue = lumValsRGB.w();
		}

		return optionStatus;
	}
}
