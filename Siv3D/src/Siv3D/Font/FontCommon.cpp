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

# include <array>
# include <Siv3D/Resource.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Compression.hpp>
# include <Siv3D/Icon.hpp>
# include <Siv3D/CacheDirectory/CacheDirectory.hpp>
# include "FontCommon.hpp"

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

	FilePath GetAndroidAppCachePath()
	{
		const FilePath fileCachePath = U"/data/data/com.kestrel.opensiv3d/files/Siv3D/";
		FileSystem::CreateDirectories(fileCachePath);
		return fileCachePath;
	}
#endif

	namespace detail
	{
		struct EngineFontResource
		{
			StringView path;

			bool required = false;

			bool compressed = true;
		};

		static constexpr std::array<EngineFontResource, 15> EngineFontResources =
		{ {
			{ U"min/siv3d-min.woff"_sv, true, false },
			{ U"noto-cjk/NotoSansCJK-Regular.ttc"_sv, false },
			{ U"noto-cjk/NotoSansJP-Regular.otf"_sv, false },
			{ U"noto-emoji/NotoEmoji-Regular.ttf"_sv, false },
			{ U"noto-emoji/NotoColorEmoji.ttf"_sv, false },
			{ U"mplus/mplus-1p-thin.ttf"_sv, false },
			{ U"mplus/mplus-1p-light.ttf"_sv, false },
			{ U"mplus/mplus-1p-regular.ttf"_sv, false },
			{ U"mplus/mplus-1p-medium.ttf"_sv, false },
			{ U"mplus/mplus-1p-bold.ttf"_sv, false },
			{ U"mplus/mplus-1p-heavy.ttf"_sv, false },
			{ U"mplus/mplus-1p-black.ttf"_sv, false },
			{ U"fontawesome/fontawesome-solid.otf"_sv, false },
			{ U"fontawesome/fontawesome-brands.otf"_sv, false },
			{ U"materialdesignicons/materialdesignicons-webfont.ttf"_sv, false },
		} };

		// 実行ファイルに同梱されている、圧縮済みフォントファイルをキャッシュフォルダに展開する。
		// キャッシュフォルダに展開済みのフォントがある場合はスキップ。
		bool ExtractEngineFonts()
		{
			LOG_SCOPED_TRACE(U"detail::ExtractEngineFonts()");

# if SIV3D_PLATFORM(ANDROID)
			const FilePath fontCacheDirectory = GetAndroidAppCachePath() + U"font/";
		
			FileSystem::CreateDirectories(fontCacheDirectory);
#else
			const FilePath fontCacheDirectory = CacheDirectory::Engine() + U"font/";
#endif

			LOG_INFO(U"fontCacheDirectory: " + fontCacheDirectory);

			for (auto&&[name, required, compressed] : EngineFontResources)
			{
				const FilePath cachedFontPath = (fontCacheDirectory + name);
				const bool existsInCache = FileSystem::Exists(cachedFontPath);
				
				// 展開済みのフォントがある場合はスキップ
				if (existsInCache)
				{
					LOG_INFO(U"ℹ️ Engine font `{0}` found in the user cache directory"_fmt(name));
					continue;
				}

# if SIV3D_PLATFORM(ANDROID)
				FilePath fontResourcePath;
				bool existsInResource = false;

				if (g_AssetManager)
				{
					std::string primaryPath = "engine/font/" + name.narrow() + (compressed ? ".zstdcmp" : "");
					
					AAsset* asset = AAssetManager_open(g_AssetManager, primaryPath.c_str(), AASSET_MODE_RANDOM);
					if (asset)
					{
						AAsset_close(asset);
						fontResourcePath = Unicode::Widen(primaryPath);
						existsInResource = true;
					}
					
					else if (compressed)
					{
						std::string altPath = "assets/engine/font/" + name.narrow() + ".zstdcmp";
						
						AAsset* asset = AAssetManager_open(g_AssetManager, altPath.c_str(), AASSET_MODE_RANDOM);
						
						if (asset)
						{
							AAsset_close(asset);
							fontResourcePath = Unicode::Widen(altPath);
							existsInResource = true;
						}
					}
					
					if (existsInResource && compressed)
					{
						const FilePath tempFileName = U"temp_" + Format(Time::GetMillisec()) + (compressed ? U".zstdcmp" : U"");
						const FilePath tempFilePath = fontCacheDirectory + tempFileName;
						
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
									LOG_ERROR(U"✖ Failed to prepare the engine font `{0}`."_fmt(name));
								}
							}
							
							AAsset_close(asset);
						}
					}
				}

				if (not existsInResource)
				{
					fontResourcePath = Resource(U"engine/font/" + name + (compressed ? U".zstdcmp" : U""));
					existsInResource = FileSystem::Exists(fontResourcePath);
				}
#else
				const FilePath fontResourcePath = Resource(U"engine/font/" + name + (compressed ? U".zstdcmp" : U""));
				const bool existsInResource = FileSystem::Exists(fontResourcePath);
#endif

				if (not existsInResource)
				{
					// 必須のフォントがキャッシュフォルダにも実行ファイルにも見つからない場合エラー
					if (required)
					{
						LOG_ERROR(U"✖ Engine font `{0}` not found"_fmt(name));
						return false;
					}

					continue;
				}

# if SIV3D_PLATFORM(ANDROID)
				try
				{
					if (compressed)
					{
						if (not Compression::DecompressFileToFile(fontResourcePath, cachedFontPath))
						{
							LOG_ERROR(U"✖ Engine font `{0}` decompression failed"_fmt(name));
							FileSystem::Remove(cachedFontPath);
							
							if (required)
							{
								return false;
							}
						}
					}
					else
					{
						FileSystem::Copy(fontResourcePath, cachedFontPath);
					}
				}
				catch (const std::exception&)
				{
					if (required)
					{
						return false;
					}
				}
				
				if (fontResourcePath.includes(U"temp_"))
				{
					try
					{
						FileSystem::Remove(fontResourcePath);
					}
					catch (const std::exception&)
					{
						LOG_ERROR(U"✖ Engine font `{0}` remove failed"_fmt(name));
					}
				}
#else
				// フォントファイルの展開に失敗したらエラー
				if (compressed)
				{
					if (not Compression::DecompressFileToFile(fontResourcePath, cachedFontPath))
					{
						LOG_ERROR(U"✖ Engine font `{0}` decompression failed"_fmt(name));
						FileSystem::Remove(cachedFontPath);
						return false;
					}
				}
#endif

			}
			return true;
		}

		static const std::array<Array<TypefaceInfo>, 17> EngineTypefaceList =
		{ {
			{{ U"noto-cjk/NotoSansCJK-Regular.ttc", 0 }, { U"noto-cjk/NotoSansJP-Regular.otf", 0 }, { U"engine/font/min/siv3d-min.woff", 0, true }},
			{{ U"noto-cjk/NotoSansCJK-Regular.ttc", 1 }, { U"noto-cjk/NotoSansJP-Regular.otf", 0 }, { U"engine/font/min/siv3d-min.woff", 0, true }},
			{{ U"noto-cjk/NotoSansCJK-Regular.ttc", 2 }, { U"noto-cjk/NotoSansJP-Regular.otf", 0 }, { U"engine/font/min/siv3d-min.woff", 0, true }},
			{{ U"noto-cjk/NotoSansCJK-Regular.ttc", 3 }, { U"noto-cjk/NotoSansJP-Regular.otf", 0 }, { U"engine/font/min/siv3d-min.woff", 0, true }},
			{{ U"noto-cjk/NotoSansCJK-Regular.ttc", 4 }, { U"noto-cjk/NotoSansJP-Regular.otf", 0 }, { U"engine/font/min/siv3d-min.woff", 0, true }},
			{{ U"noto-emoji/NotoEmoji-Regular.ttf", 0 }, { U"noto-cjk/NotoSansJP-Regular.otf", 0 }, { U"engine/font/min/siv3d-min.woff", 0, true }},
			{{ U"noto-emoji/NotoColorEmoji.ttf", 0 }},
			{{ U"mplus/mplus-1p-thin.ttf", 0 }},
			{{ U"mplus/mplus-1p-light.ttf", 0}},
			{{ U"mplus/mplus-1p-regular.ttf", 0 }},
			{{ U"mplus/mplus-1p-medium.ttf", 0 }},
			{{ U"mplus/mplus-1p-bold.ttf", 0 }},
			{{ U"mplus/mplus-1p-heavy.ttf", 0 }},
			{{ U"mplus/mplus-1p-black.ttf", 0 }},
			{{ U"fontawesome/fontawesome-solid.otf", 0 }},
			{{ U"fontawesome/fontawesome-brands.otf", 0 }},
			{{ U"materialdesignicons/materialdesignicons-webfont.ttf", 0 }},
		} };

		TypefaceInfo GetTypefaceInfo(const Typeface typeface)
		{
# if SIV3D_PLATFORM(ANDROID)
			const FilePath fontCacheDirectory = (GetAndroidAppCachePath() + U"font/");
#else
			const FilePath fontCacheDirectory = (CacheDirectory::Engine() + U"font/");
#endif

			TypefaceInfo info;

			for (const auto& font : EngineTypefaceList[FromEnum(typeface)])
			{
				info = font;

				if (info.inResource)
				{
					info.path = Resource(info.path);
				}
				else
				{
					info.path.insert(0, fontCacheDirectory);
				}

				if (FileSystem::Exists(info.path))
				{
					break;
				}
			}

			return info;
		}

		bool IsAvailable(const Typeface typeface)
		{
			return FileSystem::Exists(GetTypefaceInfo(typeface).path);
		}

		std::unique_ptr<EmojiData> CreateDefaultEmoji(const FT_Library library)
		{
			const TypefaceInfo info = GetTypefaceInfo(Typeface::ColorEmoji);

			return std::make_unique<EmojiData>(library, info.path, info.faceIndex);
		}

		std::unique_ptr<IconData> CreateDefaultIcon(const FT_Library library, const Typeface typeface)
		{
			const TypefaceInfo info = GetTypefaceInfo(typeface);

			return std::make_unique<IconData>(library, info.path, info.faceIndex);
		}
	}

	namespace detail
	{
		bool HasIcon(const Array<std::unique_ptr<IconData>>& defaultIcons, const Icon::Type iconType, const char32 codePoint)
		{
			if (iconType == Icon::Type::Awesome)
			{
				return defaultIcons[0]->hasGlyph(codePoint)
					|| defaultIcons[1]->hasGlyph(codePoint);
			}
			else
			{
				return defaultIcons[2]->hasGlyph(codePoint);
			}
		}

		GlyphIndex GetIconGlyphIndex(const Array<std::unique_ptr<IconData>>& defaultIcons, const Icon::Type iconType, const char32 codePoint)
		{
			if (iconType == Icon::Type::Awesome)
			{
				GlyphIndex glyphIndex = defaultIcons[0]->getGlyphIndex(codePoint);

				if (glyphIndex == 0)
				{
					glyphIndex = defaultIcons[1]->getGlyphIndex(codePoint);
				}

				return glyphIndex;
			}
			else
			{
				return defaultIcons[2]->getGlyphIndex(codePoint);
			}
		}

		static Image RanderIcon(const FontMethod method, const GlyphIndex glyphIndex, const int32 fontPixelSize, int32 buffer, IconData& iconData)
		{
			if (method == FontMethod::Bitmap)
			{
				return iconData.renderBitmap(glyphIndex, fontPixelSize).image;
			}
			else if (method == FontMethod::SDF)
			{
				return iconData.renderSDF(glyphIndex, fontPixelSize, buffer).image;
			}
			else 
			{
				return iconData.renderMSDF(glyphIndex, fontPixelSize, buffer).image;
			}
		}

# if SIV3D_PLATFORM(ANDROID)
		static Image RenderIcon(const FontMethod method, const Array<std::unique_ptr<IconData>>& defaultIcons, const Icon::Type iconType, const char32 codePoint, const int32 fontPixelSize, const int32 buffer)
		{
			GlyphIndex glyphIndex;
			IconData* iconData = nullptr;

			if (iconType == Icon::Type::Awesome)
			{
				glyphIndex = defaultIcons[0]->getGlyphIndex(codePoint);

				if (glyphIndex != 0)
				{
					iconData = defaultIcons[0].get();
				}
				else
				{
					glyphIndex = defaultIcons[1]->getGlyphIndex(codePoint);

					if (glyphIndex != 0)
					{
						iconData = defaultIcons[1].get();
					}
				}
			}
			else
			{
				glyphIndex = defaultIcons[2]->getGlyphIndex(codePoint);

				if (glyphIndex != 0)
				{
					iconData = defaultIcons[2].get();
				}
			}

			if (not iconData)
			{
				return Image();
			}

			return RanderIcon(method, glyphIndex, fontPixelSize, buffer, *iconData);
		}
#else
		static Image RenderIcon(const FontMethod method, const Array<std::unique_ptr<IconData>>& defaultIcons, const Icon::Type iconType, const char32 codePoint, const int32 fontPixelSize, const int32 buffer)
		{
			if (iconType == Icon::Type::Awesome)
			{
				for (size_t i = 0; i < 2; ++i)
				{
					auto& iconData = defaultIcons[i];

					if (GlyphIndex glyphIndex = iconData->getGlyphIndex(codePoint))
					{
						return RanderIcon(method, glyphIndex, fontPixelSize, buffer, *iconData);
					}
				}
			}
			else
			{
				auto& iconData = defaultIcons[2];

				if (GlyphIndex glyphIndex = iconData->getGlyphIndex(codePoint))
				{
					return RanderIcon(method, glyphIndex, fontPixelSize, buffer, *iconData);
				}
			}

			return{};
		}
#endif

		Image RenderIconBitmap(const Array<std::unique_ptr<IconData>>& defaultIcons, const Icon::Type iconType, const char32 codePoint, const int32 fontPixelSize)
		{
			return RenderIcon(FontMethod::Bitmap, defaultIcons, iconType, codePoint, fontPixelSize, 0);
		}

		Image RenderIconSDF(const Array<std::unique_ptr<IconData>>& defaultIcons, const Icon::Type iconType, const char32 codePoint, const int32 fontPixelSize, const int32 buffer)
		{
			return RenderIcon(FontMethod::SDF, defaultIcons, iconType, codePoint, fontPixelSize, buffer);
		}

		Image RenderIconMSDF(const Array<std::unique_ptr<IconData>>& defaultIcons, const Icon::Type iconType, const char32 codePoint, const int32 fontPixelSize, const int32 buffer)
		{
			return RenderIcon(FontMethod::MSDF, defaultIcons, iconType, codePoint, fontPixelSize, buffer);
		}
	}
}

