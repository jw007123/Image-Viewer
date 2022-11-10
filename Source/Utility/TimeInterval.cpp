#include "Utility/TimeInterval.h"

namespace Utility
{
	TimeInterval::TimeInterval(const f32 s_)
	{
		// Convert from seconds to microseconds. Slight loss in prec due to usize cast
		muSecWaitTicks = s_ * 1e6;
		tpStart		   = std::chrono::system_clock::now();
	}


	void TimeInterval::Wait()
	{
		using namespace std;
		using namespace chrono;

		const system_clock::time_point tpEnd = system_clock::now();
		const usize muSecTicks				 = duration_cast<microseconds>(tpEnd - tpStart).count();

		if (muSecTicks < muSecWaitTicks)
		{
			this_thread::sleep_for(microseconds(muSecWaitTicks - muSecTicks));
		}

		tpStart = system_clock::now();
	}
}
