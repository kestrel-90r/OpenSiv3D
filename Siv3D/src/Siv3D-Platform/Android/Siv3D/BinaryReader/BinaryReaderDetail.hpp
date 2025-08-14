//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//	Copyright (c) 2025 Kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/BinaryReader.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/NonNull.hpp>
# include <android/asset_manager.h>

namespace s3d
{
	extern AAssetManager* g_AssetManager;
	
	class BinaryReader::BinaryReaderDetail
	{
	private:

		bool m_isAsset = false;

		FILE* m_pFile = nullptr;

		void* m_pAsset = nullptr;
		
		int64 m_size = 0;
		
		int64 m_fullSize = 0;
		
		FilePath m_fullPath;

	public:

		BinaryReaderDetail();

		~BinaryReaderDetail();

		bool open(FilePathView path);

		void close();

		bool isOpen() const;

		int64 size() const;

		int64 setPos(int64 pos);

		int64 getPos();

		int64 read(void* buffer, int64 size);
		
		int64 read(const NonNull<void*>& buffer, int64 size);

		int64 read(void* buffer, int64 pos, int64 size);
		
		int64 read(const NonNull<void*>& buffer, int64 pos, int64 size);

		int64 lookahead(void* buffer, int64 size);
		
		int64 lookahead(const NonNull<void*>& buffer, int64 size);

		int64 lookahead(void* buffer, int64 pos, int64 size);
		
		int64 lookahead(const NonNull<void*>& buffer, int64 pos, int64 size);

		const FilePath& path() const;
	};
} 