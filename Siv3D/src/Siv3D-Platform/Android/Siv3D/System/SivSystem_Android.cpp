//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//	Copyright (c) 2025      kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <fcntl.h>
# include <signal.h>
# include <sys/wait.h>
# include <unistd.h>
# include <Siv3D/System.hpp>
# include <Siv3D/FileSystem.hpp>

namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		static bool Run(const char* program, char* argv[])
		{
			return false;
		}
	}

	namespace System
	{
		bool LaunchBrowser(const FilePathView _url)
		{
            return false;
		}

		bool ShowInFileManager(const FilePathView path)
		{
            return false;
		}
	}
}
