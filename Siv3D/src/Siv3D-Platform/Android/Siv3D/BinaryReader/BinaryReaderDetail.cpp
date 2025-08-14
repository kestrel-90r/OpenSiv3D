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

# include <cstdio>
# include <Siv3D/Common.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Unicode.hpp>
# include <android/asset_manager.h>
# include <android/asset_manager_jni.h>
# include "BinaryReaderDetail.hpp"

namespace s3d
{
	extern AAssetManager* g_AssetManager;

	BinaryReader::BinaryReaderDetail::BinaryReaderDetail()
	{
		// デフォルトコンストラクタ
	}

	BinaryReader::BinaryReaderDetail::~BinaryReaderDetail()
	{
		close();
	}

	bool BinaryReader::BinaryReaderDetail::open(const FilePathView path)
	{
		if (isOpen())
		{
			close();
		}

		if (not path)
		{
			return false;
		}

		const FilePath fullPath = FileSystem::FullPath(path);
		m_fullPath = fullPath;

		const bool isShaderFile = fullPath.includes(U"shader/essl/") || fullPath.includes(U"shader\\essl\\");
		const bool isTextureFile = fullPath.includes(U"texture/") || fullPath.includes(U"texture\\") || 
		                         fullPath.includes(U"engine/texture/") || fullPath.includes(U"engine\\texture\\");
		const bool isExampleFile = fullPath.starts_with(U"/example/") || fullPath.starts_with(U"example/") || 
		                         fullPath.starts_with(U"\\example\\") || fullPath.starts_with(U"example\\");
		const bool isFontFile = fullPath.includes(U"font/") || fullPath.includes(U"font\\") ||
		                       fullPath.includes(U"engine/font/") || fullPath.includes(U"engine\\font\\");
		
		//LOG_INFO(U"📂 BinaryReader: Attempting to open file: `{}`"_fmt(fullPath));
		//LOG_INFO(U"📂 BinaryReader: isShaderFile = {}, isTextureFile = {}, isExampleFile = {}, isFontFile = {}"_fmt(isShaderFile, isTextureFile, isExampleFile, isFontFile));

		if (!g_AssetManager)
		{
			LOG_FAIL(U"❌ BinaryReader: AssetManager is not initialized");
		}

		if (isExampleFile && g_AssetManager)
		{
			String fileName = fullPath;
			if (fileName.starts_with(U"/example/"))
			{
				fileName = fileName.substr(9);
			}
			else if (fileName.starts_with(U"example/"))
			{
				fileName = fileName.substr(8);
			}
			else if (fileName.starts_with(U"\\example\\"))
			{
				fileName = fileName.substr(9);
			}
			else if (fileName.starts_with(U"example\\"))
			{
				fileName = fileName.substr(8);
			}
			
			//LOG_INFO(U"📂 BinaryReader: Example file name: {}"_fmt(fileName));
			
			const std::vector<std::string> pathPatterns = {
				"assets/example/" + fileName.narrow(),
				"assets/engine/example/" + fileName.narrow(),
				"engine/example/" + fileName.narrow(),
				"example/" + fileName.narrow(),
				fullPath.narrow(),
				fullPath.substr(1).narrow()
			};
			
			for (const auto& pattern : pathPatterns)
			{
				//LOG_INFO(U"📂 BinaryReader: Trying example path: {}"_fmt(Unicode::Widen(pattern)));
				m_pAsset = AAssetManager_open(g_AssetManager, pattern.c_str(), AASSET_MODE_RANDOM);
				
				if (m_pAsset)
				{
					LOG_INFO(U"✅ BinaryReader: Successfully opened example file with path: {}"_fmt(Unicode::Widen(pattern)));
					m_isAsset = true;
					m_size = AAsset_getLength(static_cast<AAsset*>(m_pAsset));
					m_fullSize = m_size;
					return true;
				}
			}
			
			LOG_FAIL(U"❌ BinaryReader: Failed to open example file after trying multiple paths: `{}`"_fmt(fullPath));
		}

        else if (isShaderFile && g_AssetManager)
		{
			String fileName;
			{
				size_t pos = fullPath.lastIndexOf(U'/');
				if (pos == String::npos)
				{
					pos = fullPath.lastIndexOf(U'\\');
				}
				if (pos != String::npos)
				{
					fileName = fullPath.substr(pos + 1);
				}
				else
				{
					fileName = fullPath;
				}
			}
			
			//LOG_INFO(U"📂 BinaryReader: Shader file name: {}, dir: shader/essl/{}"_fmt(fileName, fileName));
			
			std::string engineShaderPath = "engine/shader/essl/" + fileName.narrow();
			//LOG_INFO(U"📂 BinaryReader: Trying primary shader path: {}"_fmt(Unicode::Widen(engineShaderPath)));
			
			m_pAsset = AAssetManager_open(g_AssetManager, engineShaderPath.c_str(), AASSET_MODE_RANDOM);
			
			if (m_pAsset)
			{
				LOG_INFO(U"✅ BinaryReader: Successfully opened shader file with path: {}"_fmt(Unicode::Widen(engineShaderPath)));
				m_isAsset = true;
				m_size = AAsset_getLength(static_cast<AAsset*>(m_pAsset));
				m_fullSize = m_size;
				return true;
			}
			
			const std::vector<std::string> pathPatterns = {
				fullPath.narrow(),
				fileName.narrow(),
				"shader/essl/" + fileName.narrow(),
				"assets/shader/essl/" + fileName.narrow(),
				"assets/engine/shader/essl/" + fileName.narrow()
			};
			
			for (const auto& pattern : pathPatterns)
			{
				//LOG_INFO(U"📂 BinaryReader: Trying shader path: {}"_fmt(Unicode::Widen(pattern)));
				m_pAsset = AAssetManager_open(g_AssetManager, pattern.c_str(), AASSET_MODE_RANDOM);
				
				if (m_pAsset)
				{
					LOG_INFO(U"✅ BinaryReader: Successfully opened shader file with path: {}"_fmt(Unicode::Widen(pattern)));
					m_isAsset = true;
					m_size = AAsset_getLength(static_cast<AAsset*>(m_pAsset));
					m_fullSize = m_size;
					return true;
				}
			}
			
			LOG_FAIL(U"❌ BinaryReader: Failed to open shader file after trying multiple paths: `{}`"_fmt(fullPath));
		}

		else if (isTextureFile && g_AssetManager)
		{
			String fileName;
			{
				size_t pos = fullPath.lastIndexOf(U'/');
				if (pos == String::npos)
				{
					pos = fullPath.lastIndexOf(U'\\');
				}
				if (pos != String::npos)
				{
					fileName = fullPath.substr(pos + 1);
				}
				else
				{
					fileName = fullPath;
				}
			}
			
			size_t texturePos = fullPath.indexOf(U"texture/");
			if (texturePos == String::npos)
			{
				texturePos = fullPath.indexOf(U"texture\\");
			}
			
			String textureDir;
			String relativePath;
			
			if (texturePos != String::npos)
			{
				textureDir = fullPath.substr(texturePos);
				size_t lastSlash = textureDir.lastIndexOf(U'/');
				if (lastSlash != String::npos)
				{
					textureDir = textureDir.substr(0, lastSlash + 1);
				}
				
				if (fullPath.includes(U"engine/texture/"))
				{
					size_t engineTexturePos = fullPath.indexOf(U"engine/texture/");
					relativePath = fullPath.substr(engineTexturePos);
				}
				else
				{
					relativePath = textureDir + fileName;
				}
			}
			else
			{
				textureDir = U"texture/";
				relativePath = textureDir + fileName;
			}
			
			if (relativePath.starts_with(U'/'))
			{
				relativePath = relativePath.substr(1);
			}
			
			//LOG_INFO(U"📂 BinaryReader: Texture file name: {}, dir: {}, relative: {}"_fmt(fileName, textureDir, relativePath));
			std::string engineTexturePath = "engine/" + relativePath.narrow();

            //LOG_INFO(U"📂 BinaryReader: Trying primary texture path: {}"_fmt(Unicode::Widen(engineTexturePath)));
			m_pAsset = AAssetManager_open(g_AssetManager, engineTexturePath.c_str(), AASSET_MODE_RANDOM);
			
			if (m_pAsset)
			{
				LOG_INFO(U"✅ BinaryReader: Successfully opened texture file with path: {}"_fmt(Unicode::Widen(engineTexturePath)));
				m_isAsset = true;
				m_size = AAsset_getLength(static_cast<AAsset*>(m_pAsset));
				m_fullSize = m_size;
				return true;
			}
			
			const std::vector<std::string> pathPatterns = {
				relativePath.narrow(),
				"assets/" + relativePath.narrow(),
				"assets/engine/" + relativePath.narrow(),
				fileName.narrow(),
				fullPath.narrow(),
				fullPath.substr(1).narrow()
			};
			
			for (const auto& pattern : pathPatterns)
			{
				//LOG_INFO(U"📂 BinaryReader: Trying texture path: {}"_fmt(Unicode::Widen(pattern)));
				m_pAsset = AAssetManager_open(g_AssetManager, pattern.c_str(), AASSET_MODE_RANDOM);
				
				if (m_pAsset)
				{
					LOG_INFO(U"✅ BinaryReader: Successfully opened texture file with path: {}"_fmt(Unicode::Widen(pattern)));
					m_isAsset = true;
					m_size = AAsset_getLength(static_cast<AAsset*>(m_pAsset));
					m_fullSize = m_size;
					return true;
				}
			}
			
			LOG_FAIL(U"❌ BinaryReader: Failed to open texture file after trying multiple paths: `{}`"_fmt(fullPath));
		}

		else if (isFontFile && g_AssetManager)
		{
			String fileName;
			{
				size_t pos = fullPath.lastIndexOf(U'/');
				if (pos == String::npos)
				{
					pos = fullPath.lastIndexOf(U'\\');
				}
				if (pos != String::npos)
				{
					fileName = fullPath.substr(pos + 1);
				}
				else
				{
					fileName = fullPath;
				}
			}
			
			String relativePath;
			size_t fontPos = fullPath.indexOf(U"font/");
			if (fontPos == String::npos)
			{
				fontPos = fullPath.indexOf(U"font\\");
			}
			
			if (fontPos != String::npos)
			{
				relativePath = fullPath.substr(fontPos);
				if (fullPath.includes(U"engine/font/"))
				{
					size_t engineFontPos = fullPath.indexOf(U"engine/font/");
					relativePath = fullPath.substr(engineFontPos);
				}
			}
			else
			{
				relativePath = U"font/" + fileName;
			}
			
			if (relativePath.starts_with(U'/'))
			{
				relativePath = relativePath.substr(1);
			}
			
			//LOG_INFO(U"📂 BinaryReader: Font file name: {}, relative: {}"_fmt(fileName, relativePath));
			
			const std::vector<std::string> pathPatterns = {
				relativePath.narrow(),
				"assets/" + relativePath.narrow(),
				"assets/font/" + fileName.narrow(),
				"font/" + fileName.narrow(),
				fullPath.narrow(),
				fullPath.substr(1).narrow()
			};
			
			for (const auto& pattern : pathPatterns)
			{
				//LOG_INFO(U"📂 BinaryReader: Trying font path: {}"_fmt(Unicode::Widen(pattern)));
				m_pAsset = AAssetManager_open(g_AssetManager, pattern.c_str(), AASSET_MODE_RANDOM);
				
				if (m_pAsset)
				{
					LOG_INFO(U"✅ BinaryReader: Successfully opened font file with path: {}"_fmt(Unicode::Widen(pattern)));
					m_isAsset = true;
					m_size = AAsset_getLength(static_cast<AAsset*>(m_pAsset));
					m_fullSize = m_size;
					return true;
				}
			}
			
			LOG_FAIL(U"❌ BinaryReader: Failed to open font file after trying multiple paths: `{}`"_fmt(fullPath));
			
		}
		
		const bool hasAssetPrefix1 = fullPath.starts_with(U"file:///android_asset/");
		const bool hasAssetPrefix2 = fullPath.starts_with(U"/android_asset/");
		const bool hasAssetPrefix3 = fullPath.starts_with(U"assets/");
		
		m_isAsset = (hasAssetPrefix1 || hasAssetPrefix2 || hasAssetPrefix3);
		
		if (m_isAsset && g_AssetManager)
		{
			std::string assetPath;
			
			if (hasAssetPrefix1)
			{
				assetPath = fullPath.substr(22).narrow();
				//LOG_INFO(U"📂 BinaryReader: Using path with prefix1: {}"_fmt(Unicode::Widen(assetPath)));
			}
			else if (hasAssetPrefix2)
			{
				assetPath = fullPath.substr(15).narrow();
                //LOG_INFO(U"📂 BinaryReader: Using path with prefix2: {}"_fmt(Unicode::Widen(assetPath)));
			}
			else if (hasAssetPrefix3)
			{
				assetPath = fullPath.substr(7).narrow();
				//LOG_INFO(U"📂 BinaryReader: Using path with prefix3: {}"_fmt(Unicode::Widen(assetPath)));
			}
			else
			{
				assetPath = fullPath.narrow();
				//LOG_INFO(U"📂 BinaryReader: Using default path: {}"_fmt(Unicode::Widen(assetPath)));
			}
			
			//LOG_INFO(U"📂 BinaryReader: Trying to open asset: {}"_fmt(Unicode::Widen(assetPath)));
			m_pAsset = AAssetManager_open(g_AssetManager, assetPath.c_str(), AASSET_MODE_RANDOM);
			
			if (!m_pAsset)
			{
				std::string altPath = "assets/" + assetPath;
				//LOG_INFO(U"📂 BinaryReader: Trying alternative path: {}"_fmt(Unicode::Widen(altPath)));
				m_pAsset = AAssetManager_open(g_AssetManager, altPath.c_str(), AASSET_MODE_RANDOM);
			}
			
			if (m_pAsset)
			{
				//LOG_INFO(U"✅ BinaryReader: Successfully opened asset file!");
				m_size = AAsset_getLength(static_cast<AAsset*>(m_pAsset));
				m_fullSize = m_size;
				return true;
			}
			
			LOG_FAIL(U"❌ BinaryReader: Failed to open asset file: `{}`"_fmt(fullPath));
		}
		
		#if defined(SIV3D_TARGET_WINDOWS)
			const std::wstring wpath = fullPath.toWstr();
			::_wfopen_s(&m_pFile, wpath.c_str(), L"rb");
		#else
			
			const std::string utf8Path = fullPath.narrow();
			m_pFile = std::fopen(utf8Path.c_str(), "rb");
			
		#endif

		if (!m_pFile)
		{
			LOG_FAIL(U"❌ BinaryReader: Failed to open regular file: `{}`"_fmt(fullPath));
			return false;
		}

		m_isAsset = false;
		std::fseek(m_pFile, 0, SEEK_END);
		m_size = std::ftell(m_pFile);
		m_fullSize = m_size;
		std::fseek(m_pFile, 0, SEEK_SET);

		LOG_INFO(U"✅ BinaryReader: Successfully opened regular file!");
		return true;
	}

	void BinaryReader::BinaryReaderDetail::close()
	{
		if (m_isAsset && m_pAsset)
		{
			// Androidアセットファイルを閉じる
			AAsset_close(static_cast<AAsset*>(m_pAsset));
			m_pAsset = nullptr;
		}
		else if (m_pFile)
		{
			// 通常のファイルを閉じる
			std::fclose(m_pFile);
			m_pFile = nullptr;
		}
		
		m_size = 0;
		m_fullSize = 0;
		m_isAsset = false;
	}

	bool BinaryReader::BinaryReaderDetail::isOpen() const
	{
		return (m_isAsset ? (m_pAsset != nullptr) : (m_pFile != nullptr));
	}

	int64 BinaryReader::BinaryReaderDetail::size() const
	{
		return m_size;
	}

	int64 BinaryReader::BinaryReaderDetail::setPos(const int64 pos)
	{
		if (!isOpen())
		{
			return 0;
		}

		if (pos < 0 || m_size < pos)
		{
			return getPos();
		}
		
		if (m_isAsset)
		{
			// アセットファイルの場合
			const off_t result = AAsset_seek(static_cast<AAsset*>(m_pAsset), pos, SEEK_SET);
			return (result < 0) ? getPos() : pos;
		}
		else
		{
			// 通常のファイルの場合
			std::fseek(m_pFile, static_cast<std::int64_t>(pos), SEEK_SET);
			return getPos();
		}
	}

	int64 BinaryReader::BinaryReaderDetail::getPos()
	{
		if (!isOpen())
		{
			return 0;
		}
		
		if (m_isAsset)
		{
			return AAsset_getLength(static_cast<AAsset*>(m_pAsset)) - AAsset_getRemainingLength(static_cast<AAsset*>(m_pAsset));
		}
		else
		{
			return std::ftell(m_pFile);
		}
	}

	int64 BinaryReader::BinaryReaderDetail::read(void* buffer, const int64 size)
	{
		if (!isOpen() || size <= 0)
		{
			return 0;
		}
		
		if (m_isAsset)
		{
			return AAsset_read(static_cast<AAsset*>(m_pAsset), buffer, size);
		}
		else
		{
			return std::fread(buffer, 1, static_cast<size_t>(size), m_pFile);
		}
	}
	
	int64 BinaryReader::BinaryReaderDetail::read(const NonNull<void*>& buffer, const int64 size)
	{
		return read(buffer.pointer, size);
	}

	int64 BinaryReader::BinaryReaderDetail::read(void* buffer, const int64 pos, const int64 size)
	{
		if (!isOpen() || size <= 0)
		{
			return 0;
		}
		
		const int64 previousPos = getPos();
		
		setPos(pos);
		
		const int64 readSize = read(buffer, size);
		
		setPos(previousPos);
		
		return readSize;
	}
	
	int64 BinaryReader::BinaryReaderDetail::read(const NonNull<void*>& buffer, const int64 pos, const int64 size)
	{
		return read(buffer.pointer, pos, size);
	}

	int64 BinaryReader::BinaryReaderDetail::lookahead(void* buffer, const int64 size)
	{
		const int64 previousPos = getPos();
		
		const int64 readSize = read(buffer, size);
		
		setPos(previousPos);
		
		return readSize;
	}
	
	int64 BinaryReader::BinaryReaderDetail::lookahead(const NonNull<void*>& buffer, const int64 size)
	{
		return lookahead(buffer.pointer, size);
	}

	int64 BinaryReader::BinaryReaderDetail::lookahead(void* buffer, const int64 pos, const int64 size)
	{
		return read(buffer, pos, size);
	}
	
	int64 BinaryReader::BinaryReaderDetail::lookahead(const NonNull<void*>& buffer, const int64 pos, const int64 size)
	{
		return read(buffer.pointer, pos, size);
	}

	const FilePath& BinaryReader::BinaryReaderDetail::path() const
	{
		return m_fullPath;
	}
} 