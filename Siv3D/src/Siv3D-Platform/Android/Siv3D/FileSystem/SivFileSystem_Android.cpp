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

#include <sys/stat.h>
#include <unistd.h>
#include <filesystem>
#include <Siv3D/String.hpp>
#include <Siv3D/FileSystem.hpp>
#include <Siv3D/BinaryReader.hpp>
#include <Siv3D/EnvironmentVariable.hpp>
#include <Siv3D/INI.hpp>

#if SIV3D_PLATFORM(ANDROID)
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#endif

namespace s3d
{
    namespace fs = std::filesystem;

#if SIV3D_PLATFORM(ANDROID)
    namespace detail
    {
        // Android AAssetManager instance (should be set by the main application)
        static AAssetManager* g_assetManager = nullptr;
        
        void SetAssetManager(AAssetManager* assetManager)
        {
            g_assetManager = assetManager;
        }
        
        AAssetManager* GetAssetManager()
        {
            return g_assetManager;
        }
        
        [[nodiscard]]
        static bool IsAssetPath(const FilePathView path)
        {
            // Assets are typically accessed without leading slash
            const String pathStr = String(path);
            return !pathStr.starts_with(U'/') || pathStr.starts_with(U"assets/");
        }
        
        [[nodiscard]]
        static String ToAssetPath(const FilePathView path)
        {
            String pathStr = String(path);
            
            // Remove leading slash if present
            if (pathStr.starts_with(U'/'))
            {
                pathStr = pathStr.substr(1);
            }
            
            // Remove "assets/" prefix if present
            if (pathStr.starts_with(U"assets/"))
            {
                pathStr = pathStr.substr(7);
            }
            
            return pathStr;
        }
        
        [[nodiscard]]
        static bool AssetExists(const FilePathView path)
        {
            if (!g_assetManager)
            {
                return false;
            }
            
            const String assetPath = ToAssetPath(path);
            const std::string narrowPath = assetPath.narrow();
            
            AAsset* asset = AAssetManager_open(g_assetManager, narrowPath.c_str(), AASSET_MODE_UNKNOWN);
            if (asset)
            {
                AAsset_close(asset);
                return true;
            }
            
            return false;
        }
        
        [[nodiscard]]
        static bool AssetIsDirectory(const FilePathView path)
        {
            if (!g_assetManager)
            {
                return false;
            }
            
            const String assetPath = ToAssetPath(path);
            const std::string narrowPath = assetPath.narrow();
            
            AAssetDir* assetDir = AAssetManager_openDir(g_assetManager, narrowPath.c_str());
            if (assetDir)
            {
                AAssetDir_close(assetDir);
                return true;
            }
            
            return false;
        }
        
        [[nodiscard]]
        static int64 AssetSize(const FilePathView path)
        {
            if (!g_assetManager)
            {
                return 0;
            }
            
            const String assetPath = ToAssetPath(path);
            const std::string narrowPath = assetPath.narrow();
            
            AAsset* asset = AAssetManager_open(g_assetManager, narrowPath.c_str(), AASSET_MODE_UNKNOWN);
            if (!asset)
            {
                return 0;
            }
            
            const off_t size = AAsset_getLength(asset);
            AAsset_close(asset);
            
            return static_cast<int64>(size);
        }
        
        [[nodiscard]]
        static Array<FilePath> AssetDirectoryContents(const FilePathView path, const Recursive recursive)
        {
            Array<FilePath> result;
            
            if (!g_assetManager)
            {
                return result;
            }
            
            const String assetPath = ToAssetPath(path);
            const std::string narrowPath = assetPath.narrow();
            
            AAssetDir* assetDir = AAssetManager_openDir(g_assetManager, narrowPath.c_str());
            if (!assetDir)
            {
                return result;
            }
            
            const char* filename;
            while ((filename = AAssetDir_getNextFileName(assetDir)) != nullptr)
            {
                const String fullPath = assetPath.isEmpty() ? 
                    Unicode::Widen(filename) : 
                    assetPath + U"/" + Unicode::Widen(filename);
                
                result.push_back(fullPath);
                
                // If recursive and this is a directory, recurse
                if (recursive && AssetIsDirectory(fullPath))
                {
                    const auto subContents = AssetDirectoryContents(fullPath, recursive);
                    result.insert(result.end(), subContents.begin(), subContents.end());
                }
            }
            
            AAssetDir_close(assetDir);
            return result;
        }
    }
#endif

    namespace detail
    {
        [[nodiscard]]
        static fs::path ToPath(const FilePathView path)
        {
            return fs::path(path.toUTF8());
        }

        [[nodiscard]]
        static fs::file_status GetStatus(const FilePathView path)
        {
            return fs::status(detail::ToPath(path));
        }

        [[nodiscard]]
        static bool GetStat(const FilePathView path, struct stat &s)
        {
            return (::stat(FilePath(path).replaced(U'\\', U'/').narrow().c_str(), &s) == 0);
        }

        [[nodiscard]]
        static bool Exists(const FilePathView path)
        {
#if SIV3D_PLATFORM(ANDROID)
            // Check if it's an asset path first
            if (IsAssetPath(path))
            {
                return AssetExists(path);
            }
#endif
            struct stat s;
            return GetStat(path, s);
        }

        [[nodiscard]]
        static bool IsRegular(const FilePathView path)
        {
#if SIV3D_PLATFORM(ANDROID)
            // For assets, check if it exists and is not a directory
            if (IsAssetPath(path))
            {
                return AssetExists(path) && !AssetIsDirectory(path);
            }
#endif
            struct stat s;
            if (!GetStat(path, s))
            {
                return false;
            }

            return S_ISREG(s.st_mode);
        }

        [[nodiscard]]
        static bool IsDirectory(const FilePathView path)
        {
#if SIV3D_PLATFORM(ANDROID)
            // Check if it's an asset directory
            if (IsAssetPath(path))
            {
                return AssetIsDirectory(path);
            }
#endif
            struct stat s;
            if (!GetStat(path, s))
            {
                return false;
            }

            return S_ISDIR(s.st_mode);
        }

        [[nodiscard]]
        static FilePath NormalizePath(FilePath path, const bool skipDirectoryCheck = false)
        {
            path.replace(U'\\', U'/');

            if (!path.ends_with(U'/') && (skipDirectoryCheck || (GetStatus(path).type() == fs::file_type::directory)))
            {
                path.push_back(U'/');
            }

            return path;
        }

        [[nodiscard]]
        static DateTime ToDateTime(const ::timespec &tv)
        {
            ::tm lt;
            ::localtime_r(&tv.tv_sec, &lt);
            return {(1900 + lt.tm_year),
                    (1 + lt.tm_mon),
                    (lt.tm_mday),
                    lt.tm_hour,
                    lt.tm_min,
                    lt.tm_sec,
                    static_cast<int32>(tv.tv_nsec / (1'000'000))};
        }

        [[nodiscard]]
        inline constexpr std::filesystem::copy_options ToCopyOptions(const CopyOption copyOption) noexcept
        {
            switch (copyOption)
            {
            case CopyOption::SkipExisting:      return fs::copy_options::skip_existing;
            case CopyOption::OverwriteExisting: return fs::copy_options::overwrite_existing;
            case CopyOption::UpdateExisting:    return fs::copy_options::update_existing;
            default:                            return fs::copy_options::none;
            }
        }

        [[nodiscard]]
        static bool Linux_TrashFile(const char *path)
        {
#if SIV3D_PLATFORM(ANDROID)
            return (::remove(path) == 0);
#else
            GFile *gf = g_file_new_for_path(path);
            GError *ge;

            gboolean ret = g_file_trash(gf, nullptr, &ge);

            if (ge)
            {
                g_error_free(ge);
            }

            g_object_unref(gf);

            return ret;
#endif
        }

        [[nodiscard]]
        FilePath AddSlash(FilePath path)
        {
            if (path.ends_with(U'/'))
            {
                return path;
            }

            return path + U'/';
        }

        namespace init
        {
#if SIV3D_PLATFORM(ANDROID)
            // Android-specific initialization
            const static FilePath g_initialPath = U"/";
            static FilePath g_modulePath = U"/";
            
            void InitModulePath(const char *arg)
            {
                // On Android, we typically use the assets directory
                g_modulePath = U"assets/";
            }
            
            const static std::array<FilePath, 11> g_specialFolderPaths = []()
            {
                std::array<FilePath, 11> specialFolderPaths;

                // Android-specific paths
                specialFolderPaths[FromEnum(SpecialFolder::ProgramFiles)] = U"/system/";
                specialFolderPaths[FromEnum(SpecialFolder::LocalAppData)] = U"/data/data/";
                specialFolderPaths[FromEnum(SpecialFolder::SystemFonts)] = U"/system/fonts/";
                specialFolderPaths[FromEnum(SpecialFolder::UserProfile)] = U"/sdcard/";
                specialFolderPaths[FromEnum(SpecialFolder::Desktop)] = U"/sdcard/";
                specialFolderPaths[FromEnum(SpecialFolder::Documents)] = U"/sdcard/Documents/";
                specialFolderPaths[FromEnum(SpecialFolder::Music)] = U"/sdcard/Music/";
                specialFolderPaths[FromEnum(SpecialFolder::Pictures)] = U"/sdcard/Pictures/";
                specialFolderPaths[FromEnum(SpecialFolder::Videos)] = U"/sdcard/Movies/";

                return specialFolderPaths;
            }();
            
            const static Array<FilePath> g_resourceFilePaths = []()
            {
                Array<FilePath> paths;
                
#if SIV3D_PLATFORM(ANDROID)
                // Get resource files from assets
                if (detail::GetAssetManager())
                {
                    paths = detail::AssetDirectoryContents(U"resources/", Recursive::Yes);
                    paths.remove_if([](const FilePath& path) { 
                        return detail::AssetIsDirectory(path); 
                    });
                    paths.sort();
                }
#else
                paths = FileSystem::DirectoryContents(U"resources/", Recursive::Yes);
                paths.remove_if(FileSystem::IsDirectory);
                paths.sort();
#endif
                
                return paths;
            }();
#else
            // Original Linux initialization code
            const static FilePath g_initialPath = NormalizePath(Unicode::Widen(fs::current_path().string()));

            static FilePath g_modulePath;

            void InitModulePath(const char *arg)
            {
                g_modulePath = Unicode::Widen(arg);
            }

            const static std::array<FilePath, 11> g_specialFolderPaths = []()
            {
                std::array<FilePath, 11> specialFolderPaths;

                specialFolderPaths[FromEnum(SpecialFolder::ProgramFiles)] = U"/usr/";
                specialFolderPaths[FromEnum(SpecialFolder::LocalAppData)] = U"/var/cache/";
                specialFolderPaths[FromEnum(SpecialFolder::SystemFonts)] = U"/usr/share/fonts/";

                const FilePath homeDirectory = EnvironmentVariable::Get(U"HOME");

                if (not homeDirectory)
                {
                    return specialFolderPaths;
                }

                if (const FilePath localCacheDirectory = EnvironmentVariable::Get(U"XDG_CACHE_HOME"))
                {
                    specialFolderPaths[FromEnum(SpecialFolder::LocalAppData)] = (localCacheDirectory + U'/');
                }
                else
                {
                    specialFolderPaths[FromEnum(SpecialFolder::LocalAppData)] = (homeDirectory + U"/.cache/");
                }

                if (const FilePath localFontDirectory = homeDirectory + U"/usr/local/share/fonts/";
                    FileSystem::Exists(localFontDirectory))
                {
                    specialFolderPaths[FromEnum(SpecialFolder::LocalFonts)] = localFontDirectory;
                    specialFolderPaths[FromEnum(SpecialFolder::UserFonts)] = localFontDirectory;
                }

                specialFolderPaths[FromEnum(SpecialFolder::UserProfile)] = (homeDirectory + U'/');

                const FilePath iniFilePath = homeDirectory + U"/.config/user-dirs.dirs";

                if (not FileSystem::Exists(iniFilePath))
                {
                    return specialFolderPaths;
                }

                const INI ini(iniFilePath);
                specialFolderPaths[FromEnum(SpecialFolder::Desktop)] = AddSlash(ini[U"XDG_DESKTOP_DIR"].removed(U'\"').replaced(U"$HOME", homeDirectory));
                specialFolderPaths[FromEnum(SpecialFolder::Documents)] = AddSlash(ini[U"XDG_DOCUMENTS_DIR"].removed(U'\"').replaced(U"$HOME", homeDirectory));
                specialFolderPaths[FromEnum(SpecialFolder::Music)] = AddSlash(ini[U"XDG_MUSIC_DIR"].removed(U'\"').replaced(U"$HOME", homeDirectory));
                specialFolderPaths[FromEnum(SpecialFolder::Pictures)] = AddSlash(ini[U"XDG_PICTURES_DIR"].removed(U'\"').replaced(U"$HOME", homeDirectory));
                specialFolderPaths[FromEnum(SpecialFolder::Videos)] = AddSlash(ini[U"XDG_VIDEOS_DIR"].removed(U'\"').replaced(U"$HOME", homeDirectory));

                return specialFolderPaths;
            }();

            const static Array<FilePath> g_resourceFilePaths = []()
            {
                Array<FilePath> paths = FileSystem::DirectoryContents(U"resources/", Recursive::Yes);

                paths.remove_if(FileSystem::IsDirectory);

                paths.sort();

                return paths;
            }();
#endif

            const Array<FilePath> &GetResourceFilePaths() noexcept
            {
                return g_resourceFilePaths;
            }
        }
    }

    namespace FileSystem
    {
#if SIV3D_PLATFORM(ANDROID)
        void InitializeAndroid(AAssetManager* assetManager)
        {
            detail::SetAssetManager(assetManager);
        }
#endif

        bool IsResourcePath(const FilePathView path) noexcept
        {
#if SIV3D_PLATFORM(ANDROID)
            return detail::IsAssetPath(path) || String(path).starts_with(U"resources/");
#else
            const FilePath resourceDirectory = FileSystem::ModulePath() + U"/resources/";

            return FullPath(path).starts_with(resourceDirectory);
#endif
        }

        bool Exists(const FilePathView path)
        {
            if (not path)
                SIV3D_UNLIKELY
                {
                    return false;
                }

            return detail::Exists(path);
        }

        bool IsDirectory(const FilePathView path)
        {
            if (not path)
                SIV3D_UNLIKELY
                {
                    return false;
                }

            return detail::IsDirectory(path);
        }

        bool IsFile(const FilePathView path)
        {
            if (!path) return false;

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
                {
                return detail::AssetExists(path) && !detail::AssetIsDirectory(path);
                }
#endif

            BinaryReader reader(path);
            bool exists = reader.isOpen();
            if (exists) reader.close();

            return exists;
        }

        bool IsResource(const FilePathView path)
        {
            return IsResourcePath(path) && detail::Exists(path);
        }

        bool Remove(const FilePathView path, const AllowUndo allowUndo)
        {
            if (not path)
                SIV3D_UNLIKELY
                {
                    return false;
                }

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
            {
                return false;  // Assets cannot be removed
            }
#endif

            if (IsResourcePath(path))
            {
                return false;
                }

            // Android doesn't support trash/recycle bin, so always permanently delete
            // Ignore allowUndo parameter
            try
            {
                fs::remove_all(detail::ToPath(path));
                return true;
            }
            catch (const fs::filesystem_error&)
            {
                return false;
            }
        }

        bool RemoveContents(const FilePathView path, const AllowUndo allowUndo)
        {
            if (not IsDirectory(path))
                SIV3D_UNLIKELY
                {
                    return false;
                }

            if (not Remove(path, allowUndo))
        {
                return false;
        }

            return CreateDirectories(path);
        }

        bool Rename(const FilePathView from, const FilePathView to)
        {
            if ((not from) || (not to))
                SIV3D_UNLIKELY
                {
                    return false;
                }

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(from) || detail::IsAssetPath(to))
            {
                return false;  // Assets cannot be renamed
            }
#endif

            if (IsResourcePath(from) || IsResourcePath(to))
            {
                return false;
            }

            std::error_code error;
            fs::rename(detail::ToPath(from), detail::ToPath(to), error);

            return (error.value() == 0);
        }

        int64 Size(const FilePathView path)
        {
            if (not path)
                SIV3D_UNLIKELY
                {
                    return 0;
                }

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
            {
                if (detail::AssetIsDirectory(path))
                {
                    // Calculate total size of all files in the directory
                    int64 totalSize = 0;
                    const auto contents = detail::AssetDirectoryContents(path, Recursive::Yes);
                    for (const auto& filePath : contents)
                    {
                        if (!detail::AssetIsDirectory(filePath))
                        {
                            totalSize += detail::AssetSize(filePath);
                        }
                    }
                    return totalSize;
                }
                else
                {
                    return detail::AssetSize(path);
                }
            }
#endif

            struct stat s;
            if (!detail::GetStat(FilePath(path), s))
            {
                return 0;
            }

            if (S_ISREG(s.st_mode))
            {
                return s.st_size;
            }
            else if (S_ISDIR(s.st_mode))
            {
                int64 result = 0;

                for (const auto &v : fs::recursive_directory_iterator(path.narrow()))
                {
                    struct stat s;

                    if (::stat(v.path().c_str(), &s) != 0 || S_ISDIR(s.st_mode))
                    {
                        continue;
                    }

                    result += s.st_size;
                }

                return result;
            }
            else
            {
                return 0;
            }
        }

        int64 FileSize(const FilePathView path)
        {
            if (not path)
                SIV3D_UNLIKELY
                {
                    return 0;
                }

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
            {
                return detail::AssetSize(path);
            }
#endif

            struct stat s;
            if (!detail::GetStat(path, s))
            {
                return 0;
            }

            if (!S_ISREG(s.st_mode))
            {
                return 0;
            }

            return s.st_size;
        }

        Array<FilePath> DirectoryContents(const FilePathView path, const Recursive recursive)
        {
            Array<FilePath> paths;

            if (not path)
                SIV3D_UNLIKELY
                {
                    return paths;
                }

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
            {
                return detail::AssetDirectoryContents(path, recursive);
            }
#endif

            if (detail::GetStatus(path).type() != fs::file_type::directory)
            {
                return paths;
            }

            if (recursive)
            {
                for (const auto &v : fs::recursive_directory_iterator(path.narrow()))
                {
                    paths.push_back(detail::NormalizePath(Unicode::Widen(fs::weakly_canonical(v.path()).string())));
                }
            }
            else
            {
                for (const auto &v : fs::directory_iterator(path.narrow()))
                {
                    paths.push_back(detail::NormalizePath(Unicode::Widen(fs::weakly_canonical(v.path()).string())));
                }
            }

            return paths;
        }

        FilePath FullPath(const FilePathView path)
        {
            if (not path)
                SIV3D_UNLIKELY
        {
                    return FilePath{};
        }

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
        {
                // For assets, return the normalized asset path
                return U"assets/" + detail::ToAssetPath(path);
        }
#endif

            if (path.includes(U'/'))
            {
                return detail::NormalizePath(Unicode::Widen(fs::weakly_canonical(detail::ToPath(path)).string()));
            }
            else
        {
                return detail::NormalizePath(Unicode::Widen(fs::weakly_canonical(detail::ToPath(U"./" + path)).string()));
            }
        }

        Platform::NativeFilePath NativePath(const FilePathView path)
        {
            if (not path)
                SIV3D_UNLIKELY
        {
                    return Platform::NativeFilePath{};
                }

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
            {
                // For assets, return the asset path as UTF-8 string
                return detail::ToAssetPath(path).narrow();
            }
#endif

            return fs::weakly_canonical(detail::ToPath(path)).string();
        }

        FilePath VolumePath(const FilePathView)
        {
            return U"/";
        }

        FilePath PathAppend(const FilePathView lhs, const FilePathView rhs)
        {
#if SIV3D_PLATFORM(ANDROID)
            // Special handling for asset paths
            if (detail::IsAssetPath(lhs) || detail::IsAssetPath(rhs))
        {
                String leftPath = detail::ToAssetPath(lhs);
                String rightPath = detail::ToAssetPath(rhs);

                if (leftPath.isEmpty())
        {
                    return rightPath;
                }
                if (rightPath.isEmpty())
            {
                    return leftPath;
            }

                return leftPath + U"/" + rightPath;
            }
#endif
            return FilePath{(detail::ToPath(lhs) / detail::ToPath(rhs)).u32string()}.replace(U'\\', U'/');
        }

        bool IsEmptyDirectory(const FilePathView path)
        {
            if (not path)
                SIV3D_UNLIKELY
            {
                    return false;
            }

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
            {
                if (!detail::AssetIsDirectory(path))
            {
                    return false;
            }

                const auto contents = detail::AssetDirectoryContents(path, Recursive::No);
                return contents.empty();
            }
#endif

            struct stat s;
            if (!detail::GetStat(path, s))
            {
                return false;
            }

            if (S_ISDIR(s.st_mode))
            {
                return (fs::directory_iterator(fs::path(path.toUTF8())) == fs::directory_iterator());
            }
            else
            {
                return false;
            }
        }

        Optional<DateTime> CreationTime(const FilePathView path)
        {
#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
            {
                // Assets don't have meaningful timestamps
                return none;
            }
#endif

            struct stat s;
            if (!detail::GetStat(path, s))
            {
                return none;
            }

            return detail::ToDateTime(s.st_ctim);
            }

        Optional<DateTime> WriteTime(const FilePathView path)
        {
#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
            {
                // Assets don't have meaningful timestamps
                return none;
            }
#endif

            struct stat s;
            if (!detail::GetStat(path, s))
            {
                return none;
            }

            return detail::ToDateTime(s.st_mtim);
        }

        Optional<DateTime> AccessTime(const FilePathView path)
        {
#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
            {
                // Assets don't have meaningful timestamps
                return none;
            }
#endif

            struct stat s;
            if (!detail::GetStat(path, s))
            {
                return none;
            }

            return detail::ToDateTime(s.st_atim);
        }

        const FilePath &InitialDirectory() noexcept
            {
            return detail::init::g_initialPath;
            }

        const FilePath &ModulePath() noexcept
        {
            return detail::init::g_modulePath;
        }

        FilePath CurrentDirectory()
        {
#if SIV3D_PLATFORM(ANDROID)
            return U"/";  // Android doesn't have a meaningful current directory concept
#else
            return detail::NormalizePath(Unicode::Widen(fs::current_path().string()));
#endif
        }

        bool ChangeCurrentDirectory(const FilePathView path)
        {
#if SIV3D_PLATFORM(ANDROID)
            return false;  // Not supported on Android
#else
            if (not IsDirectory(path))
            {
                return false;
            }

            return (chdir(path.narrow().c_str()) == 0);
#endif
            }

        const FilePath &GetFolderPath(const SpecialFolder folder)
        {
            assert(FromEnum(folder) < static_cast<int32>(std::size(detail::init::g_specialFolderPaths)));

            return detail::init::g_specialFolderPaths[FromEnum(folder)];
        }

        FilePath TemporaryDirectoryPath()
        {
#if SIV3D_PLATFORM(ANDROID)
            return U"/data/local/tmp/";
#else
            return (GetFolderPath(SpecialFolder::LocalAppData) + U"Temp/");
#endif
        }

        bool CreateDirectories(const FilePathView path)
        {
            if (not path)
                SIV3D_UNLIKELY
            {
                return false;
            }

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
            {
                return false;  // Assets are read-only
            }
#endif

            if (IsResourcePath(path))
            {
                return false;
            }

                try
                {
                fs::create_directories(detail::ToPath(path));
                    return true;
                }
            catch (const fs::filesystem_error&)
                {
                    return false;
                }
            }

        bool CreateParentDirectories(const FilePathView path)
        {
            if (not path)
                SIV3D_UNLIKELY
                {
                    return false;
        }

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(path))
        {
                return false;  // Assets are read-only
            }
#endif

            if (IsResourcePath(path))
            {
                return false;
            }

            return CreateDirectories(ParentPath(path));
        }

        bool Copy(const FilePathView from, const FilePathView to, const CopyOption copyOption)
        {
            if ((not from) || (not to))
                SIV3D_UNLIKELY
            {
                return false;
            }

#if SIV3D_PLATFORM(ANDROID)
            if (detail::IsAssetPath(to))
            {
                return false;  // Cannot copy to assets (read-only)
        }

            // Can copy from assets, but need special handling
            if (detail::IsAssetPath(from))
            {
                return false;
            }
#endif

            if (IsResourcePath(from) || IsResourcePath(to))
            {
                return false;
            }

            CreateParentDirectories(to);

            const auto options = detail::ToCopyOptions(copyOption) | fs::copy_options::recursive;
            std::error_code error;
            fs::copy(detail::ToPath(from), detail::ToPath(to), options, error);

            return (error.value() == 0);
        }
    }
}
