#include "Rendering/Vulkan/Impl/VulkanUBO.h"

namespace Rendering
{
	VulkanUBO::VulkanUBO(Utility::StackAllocator& stackAllocator_, VmaAllocator& vulkAllocator_, VulkanLogicalDevice& vulkLogicalDevice_) :
						 stackAllocator(stackAllocator_),
						 vulkAllocator(vulkAllocator_),
						 vulkLogicalDevice(vulkLogicalDevice_)
	{
		created = false;
	}


	VulkanUBO::~VulkanUBO()
	{
		if (created)
		{
			vkDestroyDescriptorSetLayout(vulkLogicalDevice.GetVkLogicalDevice(), uboLayout, nullptr);
			vmaDestroyBuffer(vulkAllocator, uboBuffer, uboAllocation);
		}
	}


	void VulkanUBO::Create(const CreateInfo& createInfo_)
	{
		created = true;

		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding					  = createInfo_.bindingNo;
		uboLayoutBinding.descriptorType				  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount			  = 1;
		uboLayoutBinding.stageFlags					  = createInfo_.shaderStageFlags;
		uboLayoutBinding.pImmutableSamplers			  = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
		layoutCreateInfo.sType							 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo.bindingCount					 = 1;
		layoutCreateInfo.pBindings						 = &uboLayoutBinding;

		VULK_ASSERT_SUCCESS(vkCreateDescriptorSetLayout, vulkLogicalDevice.GetVkLogicalDevice(), &layoutCreateInfo, nullptr, &uboLayout);

		// Move onto buffers...
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType				= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size				= createInfo_.dataSize;
		bufferCreateInfo.usage				= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferCreateInfo.sharingMode		= VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage					= VMA_MEMORY_USAGE_AUTO;
		allocCreateInfo.flags				    = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
								                  VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VULK_ASSERT_SUCCESS(vmaCreateBuffer, vulkAllocator, &bufferCreateInfo, &allocCreateInfo, &uboBuffer, &uboAllocation, &uboAllocationInfo);
	}


	void VulkanUBO::Update(const Utility::MemoryBlock dataBlk_)
	{
		assert(dataBlk_.size == uboAllocationInfo.size);
		memcpy(uboAllocationInfo.pMappedData, dataBlk_.ptr, dataBlk_.size);
	}
}
