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

#include <jni.h>

#include <Siv3D/EngineLog.hpp>
#include <Siv3D/Unicode.hpp>
#include <Siv3D/Window/IWindow.hpp>
#include <Siv3D/Common/Siv3DEngine.hpp>
#include "CClipboard.hpp"

JNIEnv *GetJNIEnv()
{
    static JavaVM *jvm = nullptr;
    JNIEnv *env = nullptr;
    jvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    return env;
}

namespace s3d
{
    CClipboard::CClipboard() {}

    CClipboard::~CClipboard()
    {
        LOG_SCOPED_TRACE(U"CClipboard::~CClipboard()");
    }

    void CClipboard::init()
    {
        LOG_SCOPED_TRACE(U"CClipboard::init()");
        // Android specific initialization will be handled by Java side
    }

    bool CClipboard::hasChanged()
    {
        // [Siv3D ToDo]
        return false;
    }

    bool CClipboard::getText(String &text)
    {
        JNIEnv *env = GetJNIEnv();
        jclass clazz = env->FindClass("com/example/opensiv3d/ClipboardHelper");
        jmethodID methodID = env->GetStaticMethodID(clazz, "getClipboardText", "()Ljava/lang/String;");

        jstring jResult = static_cast<jstring>(env->CallStaticObjectMethod(clazz, methodID));
        if (!jResult)
        {
            text.clear();
            return false;
        }

        const char *utfChars = env->GetStringUTFChars(jResult, nullptr);
        text = Unicode::Widen(utfChars);
        env->ReleaseStringUTFChars(jResult, utfChars);
        env->DeleteLocalRef(jResult);

        if (env->ExceptionCheck())
        {
            env->ExceptionDescribe();
            env->ExceptionClear();
            LOG_INFO(U"CClipboard::getText(): Failed to get clipboard text");
            return false;
        }

        return (not text.isEmpty());
    }

    bool CClipboard::getImage(Image &image)
    {
        image.clear();

        // [Siv3D ToDo]

        return (not image.isEmpty());
    }

    bool CClipboard::getFilePaths(Array<FilePath> &paths)
    {
        paths.clear();

        // [Siv3D ToDo]

        return (not paths.isEmpty());
    }

    void CClipboard::setText(const String &text)
    {
        // ::glfwSetClipboardString(m_window, text.narrow().c_str());
        JNIEnv *env = GetJNIEnv();
        jclass cls = env->FindClass("com/kestrel/opensiv3d/ClipboardHelper");
        jmethodID methodID = env->GetStaticMethodID(cls, "setClipboardText", "(Ljava/lang/String;)V");

        jstring jText = env->NewStringUTF(text.narrow().c_str());
        env->CallStaticVoidMethod(cls, methodID, jText);
        env->DeleteLocalRef(jText);
    }

    void CClipboard::setImage(const Image &)
    {
        // [Siv3D ToDo]
    }

    void CClipboard::clear()
    {
        // [Siv3D ToDo]
    }
}
