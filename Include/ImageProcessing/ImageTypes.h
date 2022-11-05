#pragma once

#include <cstddef>

namespace ImageProcessing
{
	enum FileType : u8
	{
		PNG = 0,
		BMP = 1,
		JPG = 2,
		Num
	};


	static const char* FileTypeStrings[FileType::Num] =
	{
		"png",
		"bmp",
		"jpg"
	};
}
