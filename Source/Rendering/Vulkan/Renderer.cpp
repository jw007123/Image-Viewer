#include "Rendering/Vulkan/Renderer.h"

namespace Rendering
{
	VRenderer::VRenderer(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, VBackend& backend_) :
		backend(backend_),
		stackAllocator(stackAllocator_),
		heapAllocator(heapAllocator_),
		mainViewportPipeline(heapAllocator_, stackAllocator_, backend.vulkanLogicalDevice, backend.vulkanSwapChain),
		vulkanFramebuffer(heapAllocator_, stackAllocator_, backend.vulkanLogicalDevice, backend.vulkanSwapChain)
	{
		if (!mainViewportPipeline.LoadShaderStage("vsTest", VulkanShader::Vertex)   ||
			!mainViewportPipeline.LoadShaderStage("fsTest", VulkanShader::Fragment) ||
			!mainViewportPipeline.LoadPipeline())
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
			vulkanFramebuffer.Destroy();
			mainViewportPipeline.Destroy();
			backend.vulkanSwapChain.Destroy();
			backend.vulkanSwapChain.Create(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

			mainViewportPipeline.LoadPipeline();
			vulkanFramebuffer.BindTo(mainViewportPipeline);

			return;
		}

		if (!backend.isMinimised)
		{
			const uint32_t imageIdx = backend.imageTargetIdx;
			const uint32_t frameIdx = backend.currentFrameIdx;
			backend.vulkanCommandPool[frameIdx].RecordToBuffer(mainViewportPipeline, vulkanFramebuffer, imageIdx);
		}
	}
}
