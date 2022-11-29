#include "Rendering/Vulkan/Backend.h"

namespace Rendering
{
	VBackend::VBackend(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_) :
					   vulkanGlfw(),
				       vulkanInstance(stackAllocator_),
					   vulkanPhysicalDevice(stackAllocator_, vulkanInstance),
					   vulkanQueueFamilies(stackAllocator_, vulkanPhysicalDevice),
					   vulkanLogicalDevice(stackAllocator_, vulkanQueueFamilies, vulkanPhysicalDevice),
					   vulkanVma(vulkanPhysicalDevice, vulkanLogicalDevice, vulkanInstance),
					   vulkanSurface(vulkanInstance, vulkanGlfw, vulkanQueueFamilies, vulkanPhysicalDevice, heapAllocator_),
					   vulkanSwapChain(heapAllocator_, vulkanPhysicalDevice, vulkanSurface, vulkanQueueFamilies, vulkanLogicalDevice, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
					   vulkanCommandPool
					   {
					       { heapAllocator_, stackAllocator_, vulkanLogicalDevice, vulkanQueueFamilies, vulkanSwapChain },
					       { heapAllocator_, stackAllocator_, vulkanLogicalDevice, vulkanQueueFamilies, vulkanSwapChain }
					   },
					   cameraDataUBO
					   {
						   { stackAllocator_, vulkanVma.GetVmaAllocator(), vulkanLogicalDevice },
						   { stackAllocator_, vulkanVma.GetVmaAllocator(), vulkanLogicalDevice }
					   },
					   modelDataPC
					   {
						   { heapAllocator_, vulkanPhysicalDevice },
						   { heapAllocator_, vulkanPhysicalDevice }
					   },
					   uniformBufferPool(heapAllocator_, stackAllocator_, vulkanLogicalDevice),
					   quadMeshData(stackAllocator_, vulkanVma.GetVmaAllocator()),
					   mainViewportPipeline(heapAllocator_, stackAllocator_, vulkanLogicalDevice, vulkanSwapChain),
					   vulkanFramebuffer(heapAllocator_, stackAllocator_, vulkanLogicalDevice, vulkanSwapChain),
					   vulkanImGui(vulkanGlfw),
					   stackAllocator(stackAllocator_),
					   heapAllocator(heapAllocator_)
	{
		// First vkFence call is invalid. Setting to 0 to avoid OoB access
		imageTargetIdx  = 0;
		currentFrameIdx = 0;
		isMinimised		= false;

		// Setup descriptor pools
		VulkanDescriptorPool::CreateInfo uniformPoolCreateInfo = {};
		uniformPoolCreateInfo.size							   = Consts::maxFramesInFlight;
		uniformPoolCreateInfo.type							   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformBufferPool.Create(uniformPoolCreateInfo);

		// Create objs that need to persist across multiple frames
		for (usize i = 0; i < Consts::maxFramesInFlight; ++i)
		{
			VkSemaphoreCreateInfo semCreateInfo = {};
			semCreateInfo.sType					= VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VULK_ASSERT_SUCCESS(vkCreateSemaphore, vulkanLogicalDevice.GetVkLogicalDevice(), &semCreateInfo, nullptr, &vulkImageAvailableSem[i]);
			VULK_ASSERT_SUCCESS(vkCreateSemaphore, vulkanLogicalDevice.GetVkLogicalDevice(), &semCreateInfo, nullptr, &vulkImageFinishedSem[i]);

			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType			  = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags			  = VK_FENCE_CREATE_SIGNALED_BIT;
			VULK_ASSERT_SUCCESS(vkCreateFence, vulkanLogicalDevice.GetVkLogicalDevice(), &fenceCreateInfo, nullptr, &vulkInFlightFence[i]);

			VulkanUBO::CreateInfo uboCreateInfo = {};
			uboCreateInfo.bindingNo			    = 0;
			uboCreateInfo.shaderStageFlags      = VK_SHADER_STAGE_VERTEX_BIT;
			uboCreateInfo.dataSize				= 2 * sizeof(Eigen::Matrix4f);
			cameraDataUBO[i].Create(uboCreateInfo);

			VulkanPushConstant::CreateInfo pcsCreateInfo = {};
			pcsCreateInfo.shaderStageFlags				 = VK_SHADER_STAGE_VERTEX_BIT;
			pcsCreateInfo.dataSize						 = sizeof(Eigen::Matrix4f);

			uniformBufferPool.Allocate(cameraDataUBO[i]);
		}

		// Load shaders
		if (!mainViewportPipeline.LoadShaderStage("vsTest", VulkanShader::Vertex) ||
			!mainViewportPipeline.LoadShaderStage("fsTest", VulkanShader::Fragment))
		{
			assert(0);
		}

		// Complete pipeline
		LoadTextureMesh();
		LoadPipeline();
	}


	VBackend::~VBackend()
	{
		vulkanLogicalDevice.WaitFor();

		for (usize i = 0; i < Consts::maxFramesInFlight; ++i)
		{
			vkDestroySemaphore(vulkanLogicalDevice.GetVkLogicalDevice(), vulkImageAvailableSem[i], nullptr);
			vkDestroySemaphore(vulkanLogicalDevice.GetVkLogicalDevice(), vulkImageFinishedSem[i], nullptr);
			vkDestroyFence(vulkanLogicalDevice.GetVkLogicalDevice(), vulkInFlightFence[i], nullptr);
		}
	}


	bool VBackend::IsRunning()
	{
		return vulkanGlfw.IsWindowOpen();
	}


	void VBackend::StartFrame()
	{
		vulkanGlfw.PollEvents();
		VULK_ASSERT_SUCCESS(vkWaitForFences, vulkanLogicalDevice.GetVkLogicalDevice(), 1, &vulkInFlightFence[currentFrameIdx], VK_TRUE, UINT64_MAX);

		// If window is minimised, do nothing
		i16 height;
		i16 width;
		glfwGetFramebufferSize(vulkanGlfw.GetGLWindow(), &height, &width);
		isMinimised = ((height * width) == 0);
		if (isMinimised)
		{
			return;
		}

		const VkResult imgResult = vkAcquireNextImageKHR(vulkanLogicalDevice.GetVkLogicalDevice(), vulkanSwapChain.GetVkSwapchainKHR(),
														 UINT64_MAX, vulkImageAvailableSem[currentFrameIdx], VK_NULL_HANDLE, &imageTargetIdx);

		// If image is out of date, recreate the full chain
		if (imgResult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			vulkanFramebuffer.Destroy();
			mainViewportPipeline.Destroy();
			vulkanSwapChain.Destroy();
			vulkanSwapChain.Create(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

			LoadPipeline();

			// Get next image
			vkAcquireNextImageKHR(vulkanLogicalDevice.GetVkLogicalDevice(), vulkanSwapChain.GetVkSwapchainKHR(),
								  UINT64_MAX, vulkImageAvailableSem[currentFrameIdx], VK_NULL_HANDLE, &imageTargetIdx);
		}

		VULK_ASSERT_SUCCESS(vkResetFences, vulkanLogicalDevice.GetVkLogicalDevice(), 1, &vulkInFlightFence[currentFrameIdx]);
		vulkanCommandPool[currentFrameIdx].Reset();
	}


	void VBackend::EndFrame()
	{
		if (isMinimised)
		{
			return;
		}

		VkSemaphore waitSems[]			  = { vulkImageAvailableSem[currentFrameIdx] };
		VkSemaphore signalSems[]		  = { vulkImageFinishedSem[currentFrameIdx] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSwapchainKHR swapChains[]		  = { vulkanSwapChain.GetVkSwapchainKHR() };

		VkSubmitInfo submitInfo		    = {};
		submitInfo.sType			    = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount   = 1;
		submitInfo.pWaitSemaphores      = waitSems;
		submitInfo.pWaitDstStageMask    = waitStages;
		submitInfo.commandBufferCount   = 1;
		submitInfo.pCommandBuffers	    = &vulkanCommandPool[currentFrameIdx].GetVkCommandBuffer();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores    = signalSems;

		VkQueue& graphicsQueue = vulkanLogicalDevice.GetVkQueue(VulkanQueueFamilies::Graphics);
		VkQueue& presentQueue  = vulkanLogicalDevice.GetVkQueue(VulkanQueueFamilies::Presenting);
		VULK_ASSERT_SUCCESS(vkQueueSubmit, graphicsQueue, 1, &submitInfo, vulkInFlightFence[currentFrameIdx]);

		VkPresentInfoKHR presentInfo   = {};
		presentInfo.sType			   = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores    = signalSems;
		presentInfo.swapchainCount	   = 1;
		presentInfo.pSwapchains        = swapChains;
		presentInfo.pImageIndices	   = &imageTargetIdx;
		presentInfo.pResults		   = nullptr;

		// If != VK_TRUE, window may be hidden
		vkQueuePresentKHR(presentQueue, &presentInfo);
		
		// Increment frame idx
		currentFrameIdx = (currentFrameIdx + 1) % Consts::maxFramesInFlight;
	}


	void VBackend::RenderFullView(const GUI::Camera& cam_, const f32 aspectRatio_)
	{
		if (isMinimised)
		{
			return;
		}

		// Update UBOs
		Eigen::Matrix4f camData[2];
		camData[0] = cam_.GetViewToProj();
		camData[1] = cam_.GetWorldToView();
		cameraDataUBO[currentFrameIdx].Update(Utility::MemoryBlock(sizeof(camData), (void*)&camData));

		// Update PCs
		Eigen::Matrix4f idMat4(Eigen::Matrix4f::Identity());
		modelDataPC[currentFrameIdx].Update(Utility::MemoryBlock(sizeof(Eigen::Matrix4f), (void*)&idMat4));

		RecordFullViewToBuffer();
	}


	void VBackend::LoadTextureMesh()
	{
		Utility::MemoryBlock pointsBuff		   = stackAllocator.Allocate<Eigen::Vector3f>(4);
		Utility::MemoryBlock textureCoordsBuff = stackAllocator.Allocate<Eigen::Vector2f>(4);
		Utility::MemoryBlock indicesBuff       = stackAllocator.Allocate<uint32_t>(6);

		Eigen::Vector3f* points		   = (Eigen::Vector3f*)pointsBuff.ptr;
		Eigen::Vector2f* textureCoords = (Eigen::Vector2f*)textureCoordsBuff.ptr;
		uint32_t* indices		       = (uint32_t*)indicesBuff.ptr;

		// Points
		{
			points[0] = Eigen::Vector3f( 0.5f,  0.5f, 0.0f);
			points[1] = Eigen::Vector3f( 0.5f, -0.5f, 0.0f);
			points[2] = Eigen::Vector3f(-0.5f, -0.5f, 0.0f);
			points[3] = Eigen::Vector3f(-0.5f,  0.5f, 0.0f);
		}

		// Texture Coords
		{
			textureCoords[0] = Eigen::Vector2f(1.0f, 0.0f);
			textureCoords[1] = Eigen::Vector2f(1.0f, 1.0f);
			textureCoords[2] = Eigen::Vector2f(0.0f, 1.0f);
			textureCoords[3] = Eigen::Vector2f(0.0f, 0.0f);
		}

		// Indices
		{
			indices[0] = 0;
			indices[1] = 3;
			indices[2] = 1;
			indices[3] = 1;
			indices[4] = 3;
			indices[5] = 2;
		}

		quadMeshData.Create(points, 4, textureCoords, 4, indices, 6);

		stackAllocator.Free(indicesBuff);
		stackAllocator.Free(textureCoordsBuff);
		stackAllocator.Free(pointsBuff);
	}


	void VBackend::LoadPipeline()
	{
		VulkanPipeline::VertexInfo vInfo;
		vInfo.bindings    = &quadMeshData.vulkBindingDescription;
		vInfo.nBindings   = 1;
		vInfo.attributes  = quadMeshData.vulkAttributeDescription;
		vInfo.nAttributes = 2;

		VulkanPipeline::UBOInfo uInfo;
		uInfo.layouts  = &cameraDataUBO[currentFrameIdx].uboLayout;
		uInfo.nLayouts = 1;

		VulkanPipeline::PCSInfo pcsInfo;
		pcsInfo.pcs  = &modelDataPC[currentFrameIdx].pushConstant;
		pcsInfo.nPcs = 1;

		mainViewportPipeline.LoadPipeline(vInfo, uInfo, pcsInfo);
		vulkanFramebuffer.BindTo(mainViewportPipeline);
	}


	void VBackend::RecordFullViewToBuffer()
	{
		VulkanCommandPool::DrawInfo dInfo;
		dInfo.meshData		 = &quadMeshData;
		dInfo.nMeshData		 = 1;
		dInfo.pushConstants  = &modelDataPC[currentFrameIdx];
		dInfo.nPushConstants = 1;

		vulkanCommandPool[currentFrameIdx].RecordToBuffer(mainViewportPipeline, vulkanFramebuffer, dInfo, imageTargetIdx);
	}
}
