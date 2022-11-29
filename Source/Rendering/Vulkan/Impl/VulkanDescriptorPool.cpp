#include "Rendering/Vulkan/Impl/VulkanDescriptorPool.h"

namespace Rendering
{
	VulkanDescriptorPool::VulkanDescriptorPool(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, VulkanLogicalDevice& vulkanLogicalDevice_) :
											   heapAllocator(heapAllocator_),
											   stackAllocator(stackAllocator_),
											   vulkanLogicalDevice(vulkanLogicalDevice_)
	{
		created = false;
	}


	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
		if (created)
		{
			vkDestroyDescriptorPool(vulkanLogicalDevice.GetVkLogicalDevice(), vulkDescriptorPool, nullptr);
		}
	}


	void VulkanDescriptorPool::Create(const CreateInfo& createInfo_)
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.descriptorCount	  = createInfo_.size;
		poolSize.type				  = createInfo_.type;

		VkDescriptorPoolCreateInfo descCreateInfo = {};
		descCreateInfo.sType					  = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descCreateInfo.poolSizeCount			  = 1;
		descCreateInfo.pPoolSizes				  = &poolSize;
		descCreateInfo.maxSets					  = poolSize.descriptorCount;

		VULK_ASSERT_SUCCESS(vkCreateDescriptorPool, vulkanLogicalDevice.GetVkLogicalDevice(), &descCreateInfo, nullptr, &vulkDescriptorPool);
		created = true;
	}


	void VulkanDescriptorPool::Allocate(const VulkanUBO& ubo_)
	{

	}
}
