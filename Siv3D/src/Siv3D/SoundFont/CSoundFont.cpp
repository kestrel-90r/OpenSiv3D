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

# include "CSoundFont.hpp"
# include <Siv3D/Error.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Compression.hpp>
# include <Siv3D/Resource.hpp>
# include <Siv3D/CacheDirectory/CacheDirectory.hpp>

# if SIV3D_PLATFORM(ANDROID)
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Time.hpp>
# include <Siv3D/BinaryWriter.hpp>
# include <Siv3D/Format.hpp>
# include <android/asset_manager.h>
#endif

namespace s3d
{
# if SIV3D_PLATFORM(ANDROID)
	extern AAssetManager* g_AssetManager;
	extern FilePath GetAndroidAppCachePath();
#endif

	namespace detail
	{
		// 実行ファイルに同梱されている、圧縮済みフォントファイルをキャッシュフォルダに展開する。
		// キャッシュフォルダに展開済みのフォントがある場合はスキップ。
		bool ExtractEngineSoundFonts()
		{
			LOG_SCOPED_TRACE(U"detail::ExtractEngineSoundFonts()");

# if SIV3D_PLATFORM(ANDROID)

			const FilePath soundfontCacheDirectory = GetAndroidAppCachePath() + U"soundfont/";

			FileSystem::CreateDirectories(soundfontCacheDirectory);

            LOG_INFO(U"soundfontCacheDirectory: " + soundfontCacheDirectory);
#endif


			{
				const FilePath name = U"GMGSx.sf2";
				const FilePath cachedSoundFontPath = (soundfontCacheDirectory + name);
				const bool existsInCache = FileSystem::Exists(cachedSoundFontPath);

				// 展開済みのフォントがある場合はスキップ
				if (existsInCache)
				{
					LOG_INFO(U"ℹ️ Engine font `{0}` found in the user cache directory"_fmt(name));
					return true;
				}

# if SIV3D_PLATFORM(ANDROID)
				FilePath fontResourcePath;
				bool existsInResource = false;

				if (g_AssetManager)
				{
					const std::string primaryPath = "engine/soundfont/GMGSx.sf2.zstdcmp";
					
					AAsset* asset = AAssetManager_open(g_AssetManager, primaryPath.c_str(), AASSET_MODE_RANDOM);
					if (asset)
					{
						AAsset_close(asset);
						fontResourcePath = Unicode::Widen(primaryPath);
						existsInResource = true;
					}
					else
					{
						const std::vector<std::string> pathPatterns = {
							"assets/engine/soundfont/GMGSx.sf2.zstdcmp",
							"soundfont/GMGSx.sf2.zstdcmp",
							"assets/soundfont/GMGSx.sf2.zstdcmp",
							"GMGSx.sf2.zstdcmp"
						};
						
						for (const auto& pattern : pathPatterns)
						{
							AAsset* asset = AAssetManager_open(g_AssetManager, pattern.c_str(), AASSET_MODE_RANDOM);
							
							if (asset)
							{
								AAsset_close(asset);
								fontResourcePath = Unicode::Widen(pattern);
								existsInResource = true;
								break;
							}
						}
					}
					
					if (existsInResource)
					{
						const FilePath tempFileName = U"temp_" + Format(Time::GetMillisec()) + U".sf2.zstdcmp";
						const FilePath tempFilePath = soundfontCacheDirectory + tempFileName;
						
						AAsset* asset = AAssetManager_open(g_AssetManager, fontResourcePath.narrow().c_str(), AASSET_MODE_BUFFER);
						if (asset)
						{
							const size_t size = AAsset_getLength(asset);
							const void* buffer = AAsset_getBuffer(asset);
							
							if (buffer && size > 0)
							{
								try
								{
									BinaryWriter writer(tempFilePath);
									if (writer)
									{
										writer.write(buffer, size);
										writer.close();
										
										fontResourcePath = tempFilePath;
									}
								}
								catch (const std::exception&)
								{
								    LOG_ERROR(U"✖ Failed to write engine soundfont  `{0}` to temporary file: {1}"_fmt(name, tempFilePath));
								}
							}
							
							AAsset_close(asset);
						}
					}
				}

				if (not existsInResource)
				{
					fontResourcePath = Resource(U"engine/soundfont/" + name + U".zstdcmp");
					existsInResource = FileSystem::Exists(fontResourcePath);
				}
#else
				const FilePath fontResourcePath = Resource(U"engine/soundfont/" + name + U".zstdcmp");
				const bool existsInResource = FileSystem::Exists(fontResourcePath);
#endif

				if (not existsInResource)
				{
					LOG_INFO(U"Engine soundfont `{0}` not found"_fmt(fontResourcePath));
					return false;
				}

				// フォントファイルの展開に失敗したらエラー
				try
				{
					if (not Compression::DecompressFileToFile(fontResourcePath, cachedSoundFontPath))
					{
						LOG_ERROR(U"✖ Engine soundfont `{0}` decompression failed"_fmt(name));
						FileSystem::Remove(cachedSoundFontPath);
						return false;
					}
				}
				catch (const std::exception&)
				{
					return false;
				}				
				
# if SIV3D_PLATFORM(ANDROID)
				if (fontResourcePath.includes(U"temp_"))
				{
					try
					{
						FileSystem::Remove(fontResourcePath);
					}
					catch (const std::exception&)
					{
						LOG_ERROR(U"✖ Engine soundfont `{0}` remove failed"_fmt(name));
					}
				}
#endif
			}

			return true;
		}
	}

	CSoundFont::CSoundFont()
	{

	}

	CSoundFont::~CSoundFont()
	{
		LOG_SCOPED_TRACE(U"CSoundFont::~CSoundFont()");
	}

	void CSoundFont::init()
	{
		LOG_SCOPED_TRACE(U"CSoundFont::init()");

		// エンジンサウンドフォントの展開
		{
			m_hasGMGSx = detail::ExtractEngineSoundFonts();
		}
	}

	Wave CSoundFont::render(const GMInstrument instrument, const uint8 key, const Duration& noteOn, const Duration& noteOff, const double velocity, const Arg::sampleRate_<uint32> sampleRate)
	{
		if (not m_hasGMGSx)
		{
			return{};
		}

# if SIV3D_PLATFORM(ANDROID)
		const FilePath standardSoundFont = GetAndroidAppCachePath() + U"soundfont/GMGSx.sf2";
#else
		const FilePath standardSoundFont = CacheDirectory::Engine() + U"soundfont/GMGSx.sf2";
#endif

		SoundFont soundFont{ standardSoundFont };

		if (not soundFont)
		{
			return{};
		}

		return soundFont.render(instrument, key, noteOn, noteOff, velocity, sampleRate);
	}

	Wave CSoundFont::renderMIDI(const FilePathView path, std::array<Array<MIDINote>, 16>& midiScore, const Arg::sampleRate_<uint32> sampleRate, const Duration& tail)
	{
		if (not m_hasGMGSx)
		{
			return{};
		}

# if SIV3D_PLATFORM(ANDROID)
		const FilePath standardSoundFont = GetAndroidAppCachePath() + U"soundfont/GMGSx.sf2";
#else
		const FilePath standardSoundFont = CacheDirectory::Engine() + U"soundfont/GMGSx.sf2";
#endif

		SoundFont soundFont{ standardSoundFont };

		if (not soundFont)
		{
			return{};
		}

		return soundFont.renderMIDI(path, midiScore, tail, sampleRate);
	}

	Wave CSoundFont::renderMIDI(IReader& reader, std::array<Array<MIDINote>, 16>& midiScore, const Arg::sampleRate_<uint32> sampleRate, const Duration& tail)
	{
		if (not m_hasGMGSx)
		{
			return{};
		}

# if SIV3D_PLATFORM(ANDROID)
		const FilePath standardSoundFont = GetAndroidAppCachePath() + U"soundfont/GMGSx.sf2";
#else
		const FilePath standardSoundFont = CacheDirectory::Engine() + U"soundfont/GMGSx.sf2";
#endif

		SoundFont soundFont{ standardSoundFont };

		if (not soundFont)
		{
			return{};
		}

		return soundFont.renderMIDI(reader, midiScore, tail, sampleRate);
	}
}
