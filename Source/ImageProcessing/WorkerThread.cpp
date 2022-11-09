#include "ImageProcessing/WorkerThread.h"

namespace ImageProcessing
{
	WorkerThread::Request::Request()
	{
		type = Type::Num;
	}


	WorkerThread::InitialData::InitialData(Image& inputImage_, Image& outputImage_) :
										   inputImage(inputImage_),
										   outputImage(outputImage_)
	{
		outputReady.store(false);

		threadReady.store(false);
		threadShutdown.store(false);
	}


	WorkerThread::WorkerThread(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, InitialData& initialData_) :
							   heapAllocator(heapAllocator_),
							   stackAllocator(stackAllocator_),
							   initialData(initialData_),
							   image(heapAllocator_)
	{
		memset(requestFlags, 0, sizeof(requestFlags));
	}

	WorkerThread::~WorkerThread()
	{
		// Needs to be last thing to be performed
		initialData.threadReady.store(false);
	}

	i32 WorkerThread::Main(InitialData* initialData_)
	{
		Utility::HeapAllocator  heapAllocator;
		Utility::StackAllocator stackAllocator(1024 * 1024 * 10);

		WorkerThread thread(heapAllocator, stackAllocator, (*initialData_));

		// Ready!
		(*initialData_).threadReady.store(true);
		while (!(*initialData_).threadShutdown.load())
		{
			const auto startTime = std::chrono::system_clock::now();
			{
				thread.Tick();
			}
			const auto endTime    = std::chrono::system_clock::now();
			const auto timePassed = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

			if (timePassed.count() < Consts::tickTimeMs)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(Consts::tickTimeMs - timePassed.count()));
			}
		}

		return -1;
	}


	void WorkerThread::Tick()
	{
		// Try and find a job
		initialData.requestsMutex.lock();
		{
			for (usize i = 0; i < initialData.requests.size(); ++i)
			{
				// Use vector elements to set request flags and values to their new values
				requestFlags[initialData.requests[i].type]  = 1;

				// Depding on type, call respective class
				switch (initialData.requests[i].type)
				{
					case Request::Luminance:
					{
						luminanceFilter.UpdateRequests(initialData.requests[i].lumValue);
						break;
					}

					default:
						assert(0);
				}
			}

			// Allow new values to be placed into array
			initialData.requests.clear();
		}
		initialData.requestsMutex.unlock();

		constexpr u8 emptyArr[Request::Num] = { 0 };
		if (!memcmp((void*)requestFlags, emptyArr, sizeof(requestFlags)))
		{
			// Nothing to do
			return;
		}

		// Update our working image
		initialData.inputMutex.lock();
		{
			image.Copy(initialData.inputImage);
		}
		initialData.inputMutex.unlock();

		// Process the image
		for (usize i = 0; i < Request::Num; ++i)
		{
			if (!requestFlags[i])
			{
				continue;
			}

			// Call class depending on flag
			switch (i)
			{
				case Request::Luminance:
				{
					luminanceFilter.ApplyFilter(image);
					break;
				}

				default:
					assert(0);
			}

			requestFlags[i] = 0;
		}

		// Set output
		initialData.outputMutex.lock();
		{
			initialData.outputImage.Copy(image);
			initialData.outputReady.store(true);
		}
		initialData.outputMutex.unlock();
	}
}
