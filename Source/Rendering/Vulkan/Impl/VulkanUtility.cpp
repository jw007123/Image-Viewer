#include "Rendering/Vulkan/Impl/VulkanUtility.h"

namespace Rendering
{
	template <typename Function, typename ...Args>
	inline void VULK_ASSERT_SUCCESS(Function&& vulkFunc_, Args&& ...args_)
	{
		const VkResult res = std::invoke(vulkFunc_, std::forward<Args>(args_)...);
		if (res != VK_SUCCESS)
		{
			assert(0);
		}
	}


	template <typename Function, typename ...Args>
	inline bool VULK_CHECK_SUCCESS(Function&& vulkFunc_, Args&& ...args_)
	{
		const VkResult res = std::invoke(vulkFunc_, std::forward<Args>(args_)...);
		return (res == VK_SUCCESS);
	}
}
