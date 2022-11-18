#pragma once

#include <functional>
#include <type_traits>
#include <concepts>

#include "vulkan/vulkan.h"

#include "Utility/Literals.h"

namespace Rendering
{
	template <typename Function, typename ...Args>
	concept HasVkReturn = requires(Function&& f_, Args&& ...a_)
	{
		{ std::invoke(f_, std::forward<Args>(a_)...) } -> std::convertible_to<VkResult>;
	};

	template <typename Function, typename ...Args> requires HasVkReturn<Function, (Args)...>
	inline void VULK_ASSERT_SUCCESS(Function&& vulkFunc_, Args&& ...args_);

	template <typename Function, typename ...Args> requires HasVkReturn<Function, (Args)...>
	inline bool VULK_CHECK_SUCCESS(Function&& vulkFunc_, Args&& ...args_);
}
