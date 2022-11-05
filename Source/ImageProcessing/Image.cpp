#include "ImageProcessing/Image.h"

namespace ImageProcessing
{
	Image::Image(Utility::HeapAllocator* heapAllocator_)
	{
		width  = 0;
		height = 0;

		heapAllocator = heapAllocator_;
	}


	Image::~Image()
	{
		Clear();
	}


	void Image::Clear()
	{
		if (imageData.ptr)
		{
			heapAllocator->Free(imageData);
		}

		width  = 0;
		height = 0;
		type   = FileType::Num;
	}


	bool Image::Load(const char* filePath_)
	{
		// Attempt to load file with stb. Force RGBA with '4'
		i16 widthStb  = 0;
		i16 heightStb = 0;
		i16 compStb   = 0;
		u8* dataStb   = stbi_load(filePath_, &widthStb, &heightStb, &compStb, 4);
		if (dataStb == nullptr)
		{
			return false;
		}

		// Remove any previous data
		Clear();

		// Determine type of image
		char typeBuff[32];
		strcpy(typeBuff, filePath_ + (strlen(filePath_) - 3));
		for (usize i = 0; i < FileType::Num; ++i)
		{
			if (!strcmp(typeBuff, FileTypeStrings[i]))
			{
				type = (FileType)i;
				break;
			}
		}

		// Unrecognised format. Fail
		if (type == FileType::Num)
		{
			return false;
		}

		// Get new allocation
		height	  = heightStb;
		width	  = widthStb;
		imageData = heapAllocator->Allocate(height * width * compStb);

		// Copy to our block and free stb's data
		memcpy(imageData.ptr, dataStb, height * width * compStb);
		stbi_image_free(dataStb);

		return true;
	}


	bool Image::Save(const char* savePath_) const
	{
		// Determine type of image
		FileType saveType;
		char typeBuff[32];
		strcpy(typeBuff, savePath_ + (strlen(savePath_) - 3));
		for (usize i = 0; i < FileType::Num; ++i)
		{
			if (!strcmp(typeBuff, FileTypeStrings[i]))
			{
				saveType = (FileType)i;
				break;
			}
		}

		// Use type format to determine which function to call
		bool success = false;
		switch (saveType)
		{
			case PNG:
			{
				success = stbi_write_png(savePath_, width, height, 4, imageData.ptr, 0);
				break;
			}
			case BMP:
			{
				success = stbi_write_bmp(savePath_, width, height, 4, imageData.ptr);
				break;
			}
			case JPG:
			{
				success = stbi_write_jpg(savePath_, width, height, 4, imageData.ptr, 100);
				break;
			}
			default:
				break;
		}
		
		return success;
	}
}
