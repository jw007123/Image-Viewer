#pragma once

#include <string.h>

#include "stb_image.h"
#include "stb_image_write.h"

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"

#include "ImageProcessing/ImageTypes.h"

namespace ImageProcessing
{
	class Image
	{
	public:
		Image(Utility::HeapAllocator* heapAllocator_);
		~Image();

		void Clear();
		bool IsValid() const;

		bool Load(const char* filePath_);
		bool Save(const char* savePath_) const;

		Utility::MemoryBlock GetData()   const;
		usize				 GetWidth()  const;
		usize				 GetHeight() const;

	private:
		// imageData.size = width * height
		Utility::MemoryBlock imageData;
		usize				 width;
		usize				 height;
		FileType			 type;

		Utility::HeapAllocator* heapAllocator;
	};
}
