#pragma once

#include <malloc.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"
#include "Utility/HeapAllocator.h"

#include "Eigen/Core"

#include "imgui.h"

namespace GUI
{
	class LuminanceOptions
	{
	public:
		struct Status
		{
			enum Flags : u8
			{
				NoOp	= 0,
				RChange = 1,
				BChange = 2,
				GChange = 3,
				WChange = 4,
				Num
			} flags;

			f32 newLumValue;

			Status();
		};

		LuminanceOptions();

		Status Draw();

	private:
		Eigen::Vector4f lumValsRGB;
	};
}
