#include "Rendering/Vulkan/Impl/VulkanUBO.h"

namespace Rendering
{
	VulkanUBO::VulkanUBO(Utility::StackAllocator& stackAllocator_, VmaAllocator& vulkAllocator_, VulkanLogicalDevice& vulkLogicalDevice_) :
						 stackAllocator(stackAllocator_),
						 vulkAllocator(vulkAllocator_),
						 vulkLogicalDevice(vulkLogicalDevice_)
	{

	}


	VulkanUBO::~VulkanUBO()
	{
		if (created)
		{
			vkDestroyDescriptorSetLayout(vulkLogicalDevice.GetVkLogicalDevice(), uboLayout, nullptr);
		}
	}


	void VulkanUBO::Create(const CreateInfo& createInfo_, const Utility::MemoryBlock dataBlk_)
	{
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
		created = true;
	}


	void VulkanUBO::Update(const Utility::MemoryBlock dataBlk_)
	{

	}
}
