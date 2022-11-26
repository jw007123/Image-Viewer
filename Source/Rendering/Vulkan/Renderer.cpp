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
		// Init pipelines
		if (!mainViewportPipeline.LoadShaderStage("vsTest", VulkanShader::Vertex)   ||
			!mainViewportPipeline.LoadShaderStage("fsTest", VulkanShader::Fragment) ||
			!mainViewportPipeline.LoadPipeline())
		{
			assert(0);
		}

		// Bind to pipeline
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
		uint32_t imageIdx = 0;
		if (!backend.GetImageTarget(imageIdx))
		{
			assert(0);
		}

		backend.vulkanCommandPool.RecordToBuffer(mainViewportPipeline, vulkanFramebuffer, imageIdx);
	}
}
