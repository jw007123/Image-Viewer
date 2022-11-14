#pragma once

#include <malloc.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"
#include "Utility/HeapAllocator.h"

#include "Eigen/Core"

#include "imgui.h"

namespace GUI::Options
{
	class Saturation
	{
	public:
		struct Status
		{
			enum Flags : u8
			{
				NoOp	= 0,
				RChange = 1,
				GChange = 2,
				BChange = 3,
				WChange = 4,
				Num
			} flags;

			f32 newSatValue;

			Status();
		};

		Saturation();

		Status Draw();

	private:
		Eigen::Vector4f satValsRGBW;
	};
}
