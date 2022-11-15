#pragma once

#include <malloc.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"

#include "GUI/SizeConsts.h"
#include "GUI/Camera.h"

#include "ImageProcessing/Image.h"

#include "Rendering/API.h"

#include "imgui.h"

namespace GUI
{
	class Viewport
	{
	public:
		struct Status
		{
			Eigen::Vector3f cameraPos;
		};

		Viewport(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, Rendering::Renderer& renderer_);

		Status Draw();

	private:
		struct ZoomInfo
		{
			Eigen::Vector3f lastCameraPos;
			bool			holdEnabled;
		} zoomInfo;

		Utility::HeapAllocator&  heapAllocator;
		Utility::StackAllocator& stackAllocator;

		GUI::Camera			   camera;
		Rendering::Framebuffer framebuffer;
		Rendering::Renderer&   renderer;

		ImGuiViewport* imguiViewport;
		usize		   width;
		usize		   height;

		void StartFrame(Status& status_);
		void EndFrame(Status& status_);

		Eigen::Vector3f HandleZoomInfo();
	};
}
