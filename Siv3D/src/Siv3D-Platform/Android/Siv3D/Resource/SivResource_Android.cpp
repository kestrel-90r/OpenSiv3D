//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//	Copyright (c) 2025 kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

#include <Siv3D/Resource.hpp>
#include <Siv3D/FileSystem.hpp>

namespace s3d
{
    namespace detail::init
    {
        const Array<FilePath> &GetResourceFilePaths() noexcept;
    }

    const Array<FilePath> &EnumResourceFiles() noexcept
    {
        return detail::init::GetResourceFilePaths();
    }

    FilePath Resource(const FilePathView path)
    {
        return FilePath(path);
    }
}
