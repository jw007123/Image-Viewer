#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <shaderc/shaderc.hpp>

#include "Rendering/Vulkan/Impl/VulkanInstance.h"
#include "Rendering/Vulkan/Impl/VulkanGLFW.h"
#include "Rendering/Vulkan/Impl/VulkanQueueFamilies.h"

namespace Rendering
{
	class VulkanShader
	{
	public:
		enum Type : u8
		{
			Vertex   = 0,
			Fragment = 1,
			Geometry = 2,
			Num
		};

		VulkanShader(VulkanLogicalDevice& vulkanLogicalDevice_);
		~VulkanShader();

		bool Load(Utility::StackAllocator& stackAllocator_, const Type shaderType_, const char* fName_);
		bool IsValid() const;

		VkShaderModule& GetVkShaderModule();

	private:
		struct Consts
		{
			static constexpr const char* shaderPath = ".\\Resources\\Shaders\\Vulkan\\";
		};

		VkShaderModule vulkShader;
		Type		   shaderType;
		bool		   wasCreated;

		VulkanLogicalDevice& vulkanLogicalDevice;

		/// Creates the VkShaderModule
		bool CreateVkShader(const Utility::MemoryBlock spirvBlock_);

		/// Returned block.size > 0 implies OK. Block contains glsl source
		Utility::MemoryBlock ReadGLSL(Utility::StackAllocator& stackAllocator_, const char* fName_) const;

		/// Returned block.size > 0 implies OK. Block contains shaderc-pre-processed glsl source
		Utility::MemoryBlock PreprocessGLSL(Utility::StackAllocator& stackAllocator_, const Utility::MemoryBlock glslSource_, const char* fName_) const;

		/// Returned block.size > 0 implies OK. Block contains SPIR-V assembly
		Utility::MemoryBlock CompileToSPIRV(Utility::StackAllocator& stackAllocator_, const Utility::MemoryBlock preprocessedGlslBlock, const char* fName_) const;
	};
}
