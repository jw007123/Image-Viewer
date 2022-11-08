#include "ImageProcessing/WorkerThread.h"

namespace ImageProcessing
{
	WorkerThread::InitialData::InitialData(Image& inputImage_) :
										   inputImage(inputImage_)
	{
		outputImage.store(nullptr);

		threadReady.store(false);
		threadShutdown.store(false);
	}


	WorkerThread::WorkerThread(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, InitialData& initialData_) :
							   heapAllocator(heapAllocator_),
							   stackAllocator(stackAllocator_),
							   initialData(initialData_),
							   image(heapAllocator_)
	{
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
			thread.Tick();

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		return -1;
	}


	void WorkerThread::Tick()
	{
		// Try and find a job
		bool haveJob = false;
		initialData.requestsMutex.lock();
		{
			haveJob = initialData.requests.size();
		}
		initialData.requestsMutex.unlock();

		if (!haveJob)
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

		//
		// Do processing
		//

		// Set output
		initialData.outputImage.store(&image);
	}
}
