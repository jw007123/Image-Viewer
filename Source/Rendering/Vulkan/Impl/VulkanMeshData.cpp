#include "Rendering/Vulkan/Impl/VulkanMeshData.h"

namespace Rendering
{
	VulkanMeshData::VulkanMeshData(Utility::StackAllocator& stackAllocator_, VmaAllocator& vulkAllocator_) :
								   stackAllocator(stackAllocator_),
								   vulkAllocator(vulkAllocator_)
	{
		vulkBindingDescription      = {};
		vulkAttributeDescription[0] = {};
		vulkAttributeDescription[1] = {};
	}


	VulkanMeshData::~VulkanMeshData()
	{
		if (vertAllocationInfo.size)
		{
			vmaDestroyBuffer(vulkAllocator, vertBuffer, vertAllocation);
		}

		if (indicesAllocationInfo.size)
		{
			vmaDestroyBuffer(vulkAllocator, indicesBuffer, indicesAllocation);
		}
	}


	void VulkanMeshData::Create(Eigen::Vector3f* points_,    const usize nPoints_,
								Eigen::Vector2f* texCoords_, const usize nTexCoords_,
								uint32_t*        indices_,   const usize nIndices_)
	{
		// NOTE: For the minute
		assert(nPoints_ && nTexCoords_);

		// Push data to interleved block
		Utility::MemoryBlock meshDataBlk = stackAllocator.Allocate(sizeof(Eigen::Vector3f) * nPoints_ + sizeof(Eigen::Vector2f) * nTexCoords_);
		usize bytesWritten = 0;
		for (usize i = 0; i < nPoints_; ++i)
		{
			*(Eigen::Vector3f*)((u8*)meshDataBlk.ptr + bytesWritten) = points_[i];
			bytesWritten											+= sizeof(Eigen::Vector3f);
			*(Eigen::Vector2f*)((u8*)meshDataBlk.ptr + bytesWritten) = texCoords_[i];
			bytesWritten											+= sizeof(Eigen::Vector2f);
		}

		// Setup bindings and attributes
		vulkBindingDescription.binding   = 0;
		vulkBindingDescription.stride    = (sizeof(Eigen::Vector3f) + sizeof(Eigen::Vector2f));
		vulkBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		vulkAttributeDescription[0].binding  = 0;
		vulkAttributeDescription[0].location = 0;
		vulkAttributeDescription[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
		vulkAttributeDescription[0].offset   = 0;

		vulkAttributeDescription[1].binding  = 0;
		vulkAttributeDescription[1].location = 1;
		vulkAttributeDescription[1].format   = VK_FORMAT_R32G32_SFLOAT;
		vulkAttributeDescription[1].offset   = sizeof(Eigen::Vector3f);

		// Create buffer to map vertex data to GPU using vma
		VkBufferCreateInfo vCreateInfo = {};
		vCreateInfo.sType			   = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vCreateInfo.size			   = bytesWritten;
		vCreateInfo.usage			   = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vCreateInfo.sharingMode		   = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo vAllocInfo = {};
		vAllocInfo.usage				   = VMA_MEMORY_USAGE_AUTO;
		vAllocInfo.flags				   = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
											 VMA_ALLOCATION_CREATE_MAPPED_BIT;
		vmaCreateBuffer(vulkAllocator, &vCreateInfo, &vAllocInfo, &vertBuffer, &vertAllocation, &vertAllocationInfo);

		// Obtain ptr to GPU mem and copy into it
		memcpy(vertAllocationInfo.pMappedData, meshDataBlk.ptr, meshDataBlk.size);

		// Create buffer to map indices data to GPU using vma
		VkBufferCreateInfo iCreateInfo = {};
		iCreateInfo.sType			   = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		iCreateInfo.size			   = sizeof(uint32_t) * nIndices_;
		iCreateInfo.usage			   = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		iCreateInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo iAllocInfo = {};
		iAllocInfo.usage				   = VMA_MEMORY_USAGE_AUTO;
		iAllocInfo.flags			       = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
											 VMA_ALLOCATION_CREATE_MAPPED_BIT;

		vmaCreateBuffer(vulkAllocator, &iCreateInfo, &iAllocInfo, &indicesBuffer, &indicesAllocation, &indicesAllocationInfo);

		// Copy into GPU mem as before
		memcpy(indicesAllocationInfo.pMappedData, indices_, sizeof(uint32_t) * nIndices_);

		// Cleanup
		stackAllocator.Free(meshDataBlk);
	}
}
