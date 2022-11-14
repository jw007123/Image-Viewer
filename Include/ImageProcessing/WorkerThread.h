#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"
#include "Utility/StackAllocator.h"
#include "Utility/TimeInterval.h"

#include "GUI/Options/Luminance.h"
#include "GUI/Options/Saturation.h"

#include "ImageProcessing/Image.h"
#include "ImageProcessing/Filters/Luminance.h"
#include "ImageProcessing/Filters/Saturation.h"

namespace ImageProcessing
{
	class WorkerThread
	{
	public:
		struct Request
		{
			enum Type : u8
			{
				Luminance  = 0,
				Saturation = 1,
				Num
			} type;

			union
			{
				GUI::Options::Luminance::Status  lumValue;
				GUI::Options::Saturation::Status satValue;
			};

			Request();
		};

		struct InitialData
		{
			// Read-only
			std::mutex inputMutex;
			Image&     inputImage;

			// Read-only
			std::atomic_bool outputReady;
			std::mutex		 outputMutex;
			Image&			 outputImage;

			// Push any requests on
			std::mutex			 requestsMutex;
			std::vector<Request> requests;

			std::atomic_bool threadReady;
			std::atomic_bool threadShutdown;

			InitialData(Image& inputImage_, Image& outputImage_);
		};

		WorkerThread(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, InitialData& initialData_);
		~WorkerThread();

		static i32  Main(InitialData* initialData_);
		static void SendLuminanceRequest(const GUI::Options::Luminance::Status&   status_, std::vector<Request>& addTo_);
		static void SendSaturationRequest(const GUI::Options::Saturation::Status& status_, std::vector<Request>& addTo_);

		void Tick();
		
	private:
		struct Consts
		{
			static constexpr f32 waitTimeS = 0.025f;
		};

		Utility::HeapAllocator&  heapAllocator;
		Utility::StackAllocator& stackAllocator;
		InitialData&			 initialData;

		u8					activeFilters[Request::Num];
		Filters::Luminance  luminanceFilter;
		Filters::Saturation saturationFilter;

		// initialData.image is read-only in the main thread, so copying into this image is well-defined
		Image image;
	};
}
