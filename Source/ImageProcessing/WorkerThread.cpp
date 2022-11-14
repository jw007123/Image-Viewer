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
		memset(activeFilters, 0, sizeof(activeFilters));
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

		WorkerThread		  thread(heapAllocator, stackAllocator, (*initialData_));
		Utility::TimeInterval interval(Consts::waitTimeS);

		// Ready!
		(*initialData_).threadReady.store(true);
		while (!(*initialData_).threadShutdown.load())
		{
			thread.Tick();
			interval.Wait();
		}

		return -1;
	}


	void WorkerThread::SendLuminanceRequest(const GUI::Options::Luminance::Status& status_, std::vector<Request>& addTo_)
	{
		if (status_.flags == GUI::Options::Luminance::Status::NoOp)
		{
			return;
		}

		Request request;
		request.lumValue = status_;
		request.type	 = Request::Luminance;

		addTo_.push_back(request);
	}


	void WorkerThread::SendSaturationRequest(const GUI::Options::Saturation::Status& status_, std::vector<Request>& addTo_)
	{
		if (status_.flags == GUI::Options::Saturation::Status::NoOp)
		{
			return;
		}

		Request request;
		request.satValue = status_;
		request.type	 = Request::Saturation;

		addTo_.push_back(request);
	}


	void WorkerThread::Tick()
	{
		// Try and find a job
		bool anyJobs = false;
		initialData.requestsMutex.lock();
		{
			anyJobs = initialData.requests.size();

			for (usize i = 0; i < initialData.requests.size(); ++i)
			{
				// Use vector elements to set active flags and values to their new values
				activeFilters[initialData.requests[i].type] = 1;

				// Depding on type, call respective class
				switch (initialData.requests[i].type)
				{
					case Request::Luminance:
					{
						luminanceFilter.UpdateRequests(initialData.requests[i].lumValue);
						break;
					}

					case Request::Saturation:
					{
						saturationFilter.UpdateRequests(initialData.requests[i].satValue);
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

		if (!anyJobs)
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
			if (!activeFilters[i])
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

				case Request::Saturation:
				{
					saturationFilter.ApplyFilter(image);
					break;
				}

				default:
					assert(0);
			}
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
