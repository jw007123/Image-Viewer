#include "Rendering/Vulkan/Impl/VulkanShader.h"

namespace Rendering
{
	VulkanShader::VulkanShader(VulkanLogicalDevice& vulkanLogicalDevice_) :
							   vulkanLogicalDevice(vulkanLogicalDevice_)
	{
		wasCreated = false;
	}


	VulkanShader::~VulkanShader()
	{
		if (wasCreated)
		{
			vkDestroyShaderModule(vulkanLogicalDevice.GetVkLogicalDevice(), vulkShader, nullptr);
		}
	}


	bool VulkanShader::Load(Utility::StackAllocator& stackAllocator_, const Type shaderType_, const char* fName_)
	{
		Utility::MemoryBlock glslBlock = ReadGLSL(stackAllocator_, fName_);
		if (!glslBlock.size)
		{
			return false;
		}

		// Set vars required for later funcs
		shaderType = shaderType_;

		Utility::MemoryBlock preprocessedGlslBlock = PreprocessGLSL(stackAllocator_, glslBlock, fName_);
		if (!preprocessedGlslBlock.size)
		{
			stackAllocator_.Free(glslBlock);
			return false;
		}

		Utility::MemoryBlock spirvBlock = CompileToSPIRV(stackAllocator_, preprocessedGlslBlock, fName_);
		if (!spirvBlock.size)
		{
			stackAllocator_.Free(preprocessedGlslBlock);
			stackAllocator_.Free(glslBlock);
			return false;
		}

		// Used in ~
		wasCreated = CreateVkShader(spirvBlock);

		// Free blocks and return
		stackAllocator_.Free(spirvBlock);
		stackAllocator_.Free(preprocessedGlslBlock);
		stackAllocator_.Free(glslBlock);

		return wasCreated;
	}


	bool VulkanShader::IsValid() const
	{
		return wasCreated;
	}


	VkShaderModule& VulkanShader::GetVkShaderModule()
	{
		if (!wasCreated)
		{
			// Please don't do this
			assert(0);
		}

		return vulkShader;
	}


	bool VulkanShader::CreateVkShader(const Utility::MemoryBlock spirvBlock_)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType					= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize				    = spirvBlock_.size;
		createInfo.pCode					= (uint32_t*)spirvBlock_.ptr;

		return VULK_CHECK_SUCCESS(vkCreateShaderModule, vulkanLogicalDevice.GetVkLogicalDevice(), &createInfo, nullptr, &vulkShader);
	}


	Utility::MemoryBlock VulkanShader::ReadGLSL(Utility::StackAllocator& stackAllocator_, const char* fName_) const
	{
		// Checks
		assert(PATH_MAX_LEN > (strlen(fName_) + strlen(Consts::shaderPath)));
		assert(strlen(fName_));
		assert(strlen(Consts::shaderPath));

		char concatPath[PATH_MAX_LEN];
		sprintf(concatPath, "%s%s.glsl", Consts::shaderPath, fName_);

		// Open file
		std::ifstream fStream(concatPath, std::ios::ate | std::ios::binary);
		if (!fStream.is_open())
		{
			Utility::Log(Utility::Error, "Shader file %s does not exist!", concatPath);
			return Utility::MemoryBlock();
		}

		// Read file and determine size
		const uint32_t fSize = (uint32_t)fStream.tellg();
		fStream.seekg(0, std::ios_base::beg);

		// Copy to block
		Utility::MemoryBlock fDataBlock = stackAllocator_.Allocate(sizeof(char) * fSize);
		fStream.read((char*)fDataBlock.ptr, fSize);
		fStream.close();

		return fDataBlock;
	}


	Utility::MemoryBlock VulkanShader::PreprocessGLSL(Utility::StackAllocator& stackAllocator_, const Utility::MemoryBlock glslSource_, const char* fName_) const
	{
		// Determine shaderc type
		shaderc_shader_kind shadercType;
		switch (shaderType)
		{
			case Type::Vertex:
			{
				shadercType = shaderc_glsl_vertex_shader;
				break;
			}

			case Type::Fragment:
			{
				shadercType = shaderc_glsl_fragment_shader;
				break;
			}

			default:
				assert(0);
		}

		// c.f. https://github.com/google/shaderc/blob/main/examples/online-compile/main.cc
		shaderc::Compiler		shadercCompiler;
		shaderc::CompileOptions shadercOptions;

		// Perform pre-process
		const shaderc::PreprocessedSourceCompilationResult shadercPPResult = shadercCompiler.PreprocessGlsl
		(
			(const char*)glslSource_.ptr, glslSource_.size, shadercType, fName_, shadercOptions
		);

		// Output
		Utility::MemoryBlock outputBlock;
		if (shadercPPResult.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			Utility::Log(Utility::Error, "Failed to pre-process %s in VulkanShader::PreprocessGLSL. Error: %s", fName_, shadercPPResult.GetErrorMessage().c_str());
		}
		else
		{
			outputBlock = stackAllocator_.Allocate(strlen(shadercPPResult.cbegin()));
			memcpy(outputBlock.ptr, shadercPPResult.cbegin(), outputBlock.size);
		}

		return outputBlock;
	}


	Utility::MemoryBlock VulkanShader::CompileToSPIRV(Utility::StackAllocator& stackAllocator_, const Utility::MemoryBlock preprocessedGlslBlock, const char* fName_) const
	{
		// Determine shaderc type
		shaderc_shader_kind shadercType;
		switch (shaderType)
		{
			case Type::Vertex:
			{
				shadercType = shaderc_glsl_vertex_shader;
				break;
			}

			case Type::Fragment:
			{
				shadercType = shaderc_glsl_fragment_shader;
				break;
			}

			default:
				assert(0);
		}

		// c.f. https://github.com/google/shaderc/blob/main/examples/online-compile/main.cc
		shaderc::Compiler		shadercCompiler;
		shaderc::CompileOptions shadercOptions;

		// Optimise shader in Release
#ifdef NDEBUG
		shadercOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
#else
		shadercOptions.SetOptimizationLevel(shaderc_optimization_level_zero);
#endif

		// Perform compilation
		const shaderc::CompilationResult shadercCompResult = shadercCompiler.CompileGlslToSpv
		(
			(const char*)preprocessedGlslBlock.ptr, preprocessedGlslBlock.size, shadercType, fName_, shadercOptions
		);

		// Output
		Utility::MemoryBlock outputBlock;
		if (shadercCompResult.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			Utility::Log(Utility::Error, "Failed to compile to SPIR-V %s in VulkanShader::CompileToSPIRV. Error: %s", fName_, shadercCompResult.GetErrorMessage().c_str());
		}
		else
		{
			outputBlock = stackAllocator_.Allocate<uint32_t>(shadercCompResult.cend() - shadercCompResult.cbegin());
			memcpy(outputBlock.ptr, shadercCompResult.cbegin(), outputBlock.size);
		}

		return outputBlock;
	}
}
