#pragma once

#include <chrono>
#include <thread>

#include "Utility/Literals.h"

namespace Utility
{
	class TimeInterval
	{
	public:
		TimeInterval(const f32 s_);

		void Wait();

	private:
		usize muSecWaitTicks;

		std::chrono::system_clock::time_point tpStart;
	};
}
