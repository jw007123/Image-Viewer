#include "Rendering/Vulkan/Renderer.h"

namespace Rendering
{
	VRenderer::MeshVulkanData::MeshVulkanData(Utility::HeapAllocator& heapAllocator_, VmaAllocator& vulkAllocator_) :
											  heapAllocator(heapAllocator_),
											  vulkAllocator(vulkAllocator_)
	{
		vulkBindingDescription      = {};
		vulkAttributeDescription[0] = {};
		vulkAttributeDescription[1] = {};
	}


	VRenderer::MeshVulkanData::~MeshVulkanData()
	{
		if (meshDataBlk.size)
		{
			heapAllocator.Free(meshDataBlk);
			vmaDestroyBuffer(vulkAllocator, vertBuffer, vertAllocation);
		}
	}

	void VRenderer::MeshVulkanData::Create(Eigen::Vector3f* points_,    const usize nPoints_,
										   Eigen::Vector2f* texCoords_, const usize nTexCoords_)
	{
		// NOTE: For the minute
		assert(nPoints_ && nTexCoords_);

		// Push data to interleved block
		meshDataBlk		   = heapAllocator.Allocate(sizeof(Eigen::Vector3f) * nPoints_ + sizeof(Eigen::Vector2f) * nTexCoords_);
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
		VkBufferCreateInfo createInfo = {};
		createInfo.sType			  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size				  = bytesWritten;
		createInfo.usage			  = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		createInfo.sharingMode		  = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage					  = VMA_MEMORY_USAGE_AUTO;
		allocInfo.flags					  = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
		allocInfo.preferredFlags		  = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		vmaCreateBuffer(vulkAllocator, &createInfo, &allocInfo, &vertBuffer, &vertAllocation, nullptr);

		// Obtain ptr to GPU mem and copy into it
		void* gpuMem;
		vmaMapMemory(vulkAllocator, vertAllocation, &gpuMem);
		memcpy(gpuMem, meshDataBlk.ptr, meshDataBlk.size);
		vmaUnmapMemory(vulkAllocator, vertAllocation);
	}


	VRenderer::VRenderer(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, VBackend& backend_) :
		backend(backend_),
		stackAllocator(stackAllocator_),
		heapAllocator(heapAllocator_),
		quadMeshData(heapAllocator, backend.vulkanVma.GetVmaAllocator()),
		mainViewportPipeline(heapAllocator_, stackAllocator_, backend.vulkanLogicalDevice, backend.vulkanSwapChain),
		vulkanFramebuffer(heapAllocator_, stackAllocator_, backend.vulkanLogicalDevice, backend.vulkanSwapChain)
	{
		LoadTextureMesh();

		VulkanPipeline::VertexInfo vInfo;
		vInfo.bindings    = &quadMeshData.vulkBindingDescription;
		vInfo.nBindings   = 1;
		vInfo.attributes  = quadMeshData.vulkAttributeDescription;
		vInfo.nAttributes = 2;

		if (!mainViewportPipeline.LoadShaderStage("vsTest", VulkanShader::Vertex) ||
			!mainViewportPipeline.LoadShaderStage("fsTest", VulkanShader::Fragment) ||
			!mainViewportPipeline.LoadPipeline(vInfo))
		{
			assert(0);
		}

		if (!vulkanFramebuffer.BindTo(mainViewportPipeline))
		{
			assert(0);
		}
	}


	VRenderer::~VRenderer()
	{

	}


	void VRenderer::RenderFullView(const GUI::Camera& cam_, const f32 aspectRatio_)
	{
		if (backend.swapChainOoD)
		{
			VulkanPipeline::VertexInfo vInfo;
			vInfo.bindings    = &quadMeshData.vulkBindingDescription;
			vInfo.nBindings   = 1;
			vInfo.attributes  = quadMeshData.vulkAttributeDescription;
			vInfo.nAttributes = 2;

			vulkanFramebuffer.Destroy();
			mainViewportPipeline.Destroy();
			backend.vulkanSwapChain.Destroy();
			backend.vulkanSwapChain.Create(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

			mainViewportPipeline.LoadPipeline(vInfo);
			vulkanFramebuffer.BindTo(mainViewportPipeline);

			return;
		}

		if (!backend.isMinimised)
		{
			usize vertCnt = (quadMeshData.meshDataBlk.size / (sizeof(Eigen::Vector3f) + sizeof(Eigen::Vector2f)));
			VkDeviceSize vertOffset = 0;

			VulkanCommandPool::DrawInfo drawInfo = {};
			drawInfo.nVertBuffers				 = 1;
			drawInfo.vertBuffers				 = &quadMeshData.vertBuffer;
			drawInfo.verticesCnt				 = &vertCnt;
			drawInfo.vertOffsets				 = &vertOffset;

			const uint32_t imageIdx = backend.imageTargetIdx;
			const uint32_t frameIdx = backend.currentFrameIdx;
			backend.vulkanCommandPool[frameIdx].RecordToBuffer(mainViewportPipeline, vulkanFramebuffer, drawInfo, imageIdx);
		}
	}


	void VRenderer::LoadTextureMesh()
	{
		Utility::MemoryBlock pointsBuff		   = stackAllocator.Allocate<Eigen::Vector3f>(6);
		Utility::MemoryBlock textureCoordsBuff = stackAllocator.Allocate<Eigen::Vector2f>(6);

		Eigen::Vector3f* points		   = (Eigen::Vector3f*)pointsBuff.ptr;
		Eigen::Vector2f* textureCoords = (Eigen::Vector2f*)textureCoordsBuff.ptr;

		// Points
		{
			points[0] = Eigen::Vector3f( 0.5f,  0.5f, 0.0f);
			points[1] = Eigen::Vector3f(-0.5f,  0.5f, 0.0f);
			points[2] = Eigen::Vector3f( 0.5f, -0.5f, 0.0f);
			points[3] = Eigen::Vector3f( 0.5f, -0.5f, 0.0f);
			points[4] = Eigen::Vector3f(-0.5f,  0.5f, 0.0f);
			points[5] = Eigen::Vector3f(-0.5f, -0.5f, 0.0f);
		}

		// Texture Coords
		{
			textureCoords[0] = Eigen::Vector2f(1.0f, 0.0f);
			textureCoords[1] = Eigen::Vector2f(0.0f, 0.0f);
			textureCoords[2] = Eigen::Vector2f(1.0f, 1.0f);
			textureCoords[3] = Eigen::Vector2f(1.0f, 1.0f);
			textureCoords[4] = Eigen::Vector2f(0.0f, 0.0f);
			textureCoords[5] = Eigen::Vector2f(0.0f, 1.0f);
		}

		quadMeshData.Create(points, 6, textureCoords, 6);

		stackAllocator.Free(textureCoordsBuff);
		stackAllocator.Free(pointsBuff);
	}
}
