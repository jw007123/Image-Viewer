#include "Rendering/Vulkan/Impl/VulkanPushConstant.h"

namespace Rendering
{
	VulkanPushConstant::VulkanPushConstant(Utility::HeapAllocator& heapAllocator_, VulkanPhysicalDevice& vulkPhysicalDevice_) :
										   heapAllocator(heapAllocator_),
										   vulkPhysicalDevice(vulkPhysicalDevice_)
	{

	}


	VulkanPushConstant::~VulkanPushConstant()
	{
		if (dataBlk.size)
		{
			heapAllocator.Free(dataBlk);
		}
	}


	void VulkanPushConstant::Create(const CreateInfo& createInfo_)
	{
		static uint32_t globalPcsOffset			= 0;
		static VkShaderStageFlags globalFlagIdx = 0;

		// Check we haven't already set push constants for this stage
		assert((globalFlagIdx & createInfo_.shaderStageFlags) == 0);

		// Get push constant size limit for device
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(vulkPhysicalDevice.GetVkPhysicalDevice(), &deviceProperties);
		const uint32_t maxSize = deviceProperties.limits.maxPushConstantsSize;

		// Defined at https://registry.khronos.org/vulkan/specs/1.1/html/vkspec.html#VUID-VkPushConstantRange-offset-00294
		assert((globalPcsOffset + createInfo_.dataSize) < maxSize && (createInfo_.dataSize % 4) == 0);

		pushConstant			= {};
		pushConstant.offset		= globalPcsOffset;
		pushConstant.size		= createInfo_.dataSize;
		pushConstant.stageFlags = createInfo_.shaderStageFlags;

		// Allocate mem
		if (!dataBlk.size)
		{
			dataBlk = heapAllocator.Allocate(createInfo_.dataSize);
		}
		else
		{
			dataBlk = heapAllocator.Reallocate(dataBlk, createInfo_.dataSize);
		}

		// Update statics
		globalPcsOffset += createInfo_.dataSize;
		globalFlagIdx   |= createInfo_.shaderStageFlags;
	}


	void VulkanPushConstant::Update(const Utility::MemoryBlock dataBlk_)
	{
		assert(dataBlk.size == dataBlk_.size);

		memcpy(dataBlk.ptr, dataBlk_.ptr, dataBlk.size);
	}
}
