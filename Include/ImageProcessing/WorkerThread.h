#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"
#include "Utility/StackAllocator.h"

#include "ImageProcessing/Image.h"

namespace ImageProcessing
{
	class WorkerThread
	{
	public:
		struct Request
		{

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

		// initialData.image is read-only in the main thread, so copying into this image is well-defined
		Image image;
	};
}
