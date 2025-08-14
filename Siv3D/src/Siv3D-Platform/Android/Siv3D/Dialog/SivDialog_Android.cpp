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

#include <Siv3D/Dialog.hpp>

#include <Siv3D/Unicode.hpp>
#include <Siv3D/FileSystem.hpp>
#include <Siv3D/Print.hpp>
#include <jni.h>

extern jobject g_mainActivityInstance;
extern JavaVM *g_JavaVM;

extern bool openFileSelectionDialog();
extern std::string waitForFileSelection();

namespace s3d
{
    namespace Dialog
    {

        Optional<FilePath> OpenFile(const Array<FileFilter> &filters, const FilePathView defaultPath, const StringView title)
        {
            if (!g_mainActivityInstance || !g_JavaVM)
            {
                return none;
            }

            if (openFileSelectionDialog())
            {
                std::string selectedPath = waitForFileSelection();

                if (!selectedPath.empty())
                {
                    return FileSystem::FullPath(Unicode::Widen(selectedPath));
                }
                else
                {
                    return none;
                }
            }
            else
            {
                return none;
            }
        }

        Array<FilePath> OpenFiles(const Array<FileFilter> &filters, const FilePathView defaultPath, const StringView title)
        {
            if (!g_mainActivityInstance || !g_JavaVM)
            {
                return {};
            }

            return {};
        }

        Optional<FilePath> SaveFile(const Array<FileFilter> &filters, const FilePathView defaultPath, const StringView title)
        {
            if (!g_mainActivityInstance || !g_JavaVM)
            {
                return none;
            }

            if (defaultPath)
            {
                return FileSystem::FullPath(defaultPath);
            }

            return none;
        }

        Optional<FilePath> SelectFolder(const FilePathView defaultPath, const StringView title)
        {
            if (!g_mainActivityInstance || !g_JavaVM)
            {
                return none;
            }
            if (defaultPath)
            {
                return FileSystem::FullPath(defaultPath);
            }

            return none;
        }
    }
}
