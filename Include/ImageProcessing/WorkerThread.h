#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"
#include "Utility/StackAllocator.h"

#include "GUI/LuminanceOptions.h"

#include "ImageProcessing/Image.h"
#include "ImageProcessing/LuminanceFilter.h"

namespace ImageProcessing
{
	class WorkerThread
	{
	public:
		struct Request
		{
			enum Type : u8
			{
				Luminance,
				Num
			} type;

			union
			{
				GUI::LuminanceOptions::Status lumValue;
			};

			Request();
		};

		struct InitialData
		{
			// Read-only!
			std::mutex inputMutex;
			Image&     inputImage;

			// If outputImage != nullptr, take it
			std::atomic<Image*> outputImage;

			// Push any requests on
			std::mutex			 requestsMutex;
			std::vector<Request> requests;

			std::atomic_bool threadReady;
			std::atomic_bool threadShutdown;

			InitialData(Image& inputImage_);
		};

		WorkerThread(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, InitialData& initialData_);
		~WorkerThread();

		static i32 Main(InitialData* initialData_);

		void Tick();
		
	private:
		Utility::HeapAllocator&  heapAllocator;
		Utility::StackAllocator& stackAllocator;
		InitialData&			 initialData;

		u8				requestFlags[Request::Num];
		LuminanceFilter luminanceFilter;

		// initialData.image is read-only in the main thread, so copying into this image is well-defined
		Image image;
	};
}
