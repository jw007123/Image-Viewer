#pragma once

#include <malloc.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"

#include <nfd.h>

#include "imgui.h"

namespace GUI
{
	class FileMenu
	{
	public:
		struct Status
		{
			enum Flags : u8
			{
				NoOp = 0,
				Open = 1,
				Save = 2
			} flags;

			char (*openFilePathPtr)[PATH_MAX_LEN];
			char (*saveFilePathPtr)[PATH_MAX_LEN];

			Status();
		};

		Status Draw();

	private:
		char openFilePath[PATH_MAX_LEN];
		char saveFilePath[PATH_MAX_LEN];

		bool HandleOpen();
		bool HandleSave();
	};
}
