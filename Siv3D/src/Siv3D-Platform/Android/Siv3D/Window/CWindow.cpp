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

#include <Siv3D/Error.hpp>
#include <Siv3D/EngineLog.hpp>
#include <Siv3D/FormatLiteral.hpp>
#include <Siv3D/Utility.hpp>
#include <Siv3D/UserAction.hpp>
#include <Siv3D/Scene.hpp>
#include <Siv3D/Monitor.hpp>
#include <Siv3D/Renderer/IRenderer.hpp>
#include <Siv3D/Profiler/IProfiler.hpp>
#include <Siv3D/UserAction/IUserAction.hpp>
#include <Siv3D/Common/Siv3DEngine.hpp>
#include "CWindow.hpp"

namespace s3d
{
    void *g_GameActivityHandle = nullptr;
    static jobject g_MainActivity = nullptr;

    namespace detail
    {
        static void ErrorCallback(const int error, const char *description)
        {
            std::cerr << U"Error: {}. "_fmt(error) << description << '\n';
        }
    }

    CWindow::CWindow()
    {
    }

    CWindow::~CWindow()
    {
        LOG_SCOPED_TRACE(U"CWindow::~CWindow()");
    }

    void CWindow::init()
    {
        LOG_SCOPED_TRACE(U"CWindow::init()");

        updateState();
    }

    void CWindow::update()
    {

        updateState();
    }

    void CWindow::setWindowTitle(const String &title)
    {
        m_title = title;
    }

    const String &CWindow::getWindowTitle() const noexcept
    {
        return m_title;
    }

    void *CWindow::getHandle() const noexcept
    {
        return m_window;
    }

    const WindowState &CWindow::getState() const noexcept
    {
        return m_state;
    }

    void CWindow::setStyle(const WindowStyle style)
    {
        m_state.style = style;
    }

    void CWindow::setPos(const Point &pos)
    {
    }

    void CWindow::maximize()
    {
    }

    void CWindow::restore()
    {
    }

    void CWindow::minimize()
    {
    }

    bool CWindow::resizeByVirtualSize(const Size &virtualSize)
    {
        LOG_TRACE(U"CWindow::resizeByVirtualSize({})"_fmt(virtualSize));
        m_state.virtualSize = virtualSize;
        return true;
    }

    bool CWindow::resizeByFrameBufferSize(const Size &frameBufferSize)
    {
        LOG_TRACE(U"CWindow::resizeByFrameBufferSize({})"_fmt(frameBufferSize));
        m_state.frameBufferSize = frameBufferSize;
        m_state.virtualSize = (frameBufferSize * (1.0 / m_state.scaling)).asPoint();
        return true;
    }

    void CWindow::setMinimumFrameBufferSize(const Size &size)
    {
        LOG_TRACE(U"CWindow::setMinimumFrameBufferSize({})"_fmt(size));
        m_state.minFrameBufferSize = size;
    }

    void CWindow::setFullscreen(const bool fullscreen, size_t monitorIndex)
    {

        m_state.fullscreen = true;

        if (Scene::GetResizeMode() != ResizeMode::Keep)
        {
            SIV3D_ENGINE(Renderer)->updateSceneSize();
        }
    }

    void CWindow::setToggleFullscreenEnabled(bool) {}

    bool CWindow::isToggleFullscreenEnabled() const
    {
        return false;
    }

    void CWindow::updateState()
    {
        m_state.focused = true;
        m_state.fullscreen = true;
        m_state.minimized = false;
        m_state.maximized = false;
        m_state.sizeMove = false;
    }

    void CWindow::updatePos(const Point &pos)
    {
        m_state.bounds.pos = pos;
    }

    void CWindow::updateSize(const Size &size)
    {
        m_state.bounds.size = size;
        m_state.virtualSize = size;
    }

    void CWindow::updateFrameBufferSize(const Size &size)
    {
        m_state.frameBufferSize = size;
    }

    void CWindow::updateScaling(double scaling)
    {
        m_state.scaling = scaling;
    }

    void CWindow::updateFocus(bool focused)
    {
        m_state.focused = focused;
    }

#if SIV3D_PLATFORM(ANDROID)

    void *CWindow::getNativeWindow() const noexcept
    {
        return g_GameActivityHandle;
    }

    extern "C"
    {

        JNIEXPORT void JNICALL
        Java_com_kestrel_opensiv3d_MainActivity_onMoveNative(JNIEnv *env, jobject /* this */, jint x, jint y)
        {
            LOG_TRACE(U"CWindow::OnMove({})"_fmt(s3d::Point(x, y)));

            auto *const pWindow = static_cast<s3d::CWindow *>(s3d::SIV3D_ENGINE(Window));
            if (!pWindow)
            {
                std::cerr << "Warning: nativeOnMove called before engine initialization" << std::endl;
                return;
            }

            pWindow->updatePos(s3d::Point(x, y));
        }

        JNIEXPORT void JNICALL
        Java_com_kestrel_opensiv3d_MainActivity_onResizeNative(JNIEnv *env, jobject /* this */, jint width, jint height)
        {
            LOG_TRACE(U"CWindow::OnResize({})"_fmt(s3d::Size(width, height)));

            auto *const pWindow = static_cast<s3d::CWindow *>(s3d::SIV3D_ENGINE(Window));
            if (!pWindow)
            {
                std::cerr << "Warning: nativeOnResize called before engine initialization" << std::endl;
                return;
            }

            pWindow->updateSize(s3d::Size(width, height));

            if (s3d::Scene::GetResizeMode() != s3d::ResizeMode::Keep)
            {
                s3d::SIV3D_ENGINE(Renderer)->updateSceneSize();
            }
        }

        JNIEXPORT void JNICALL
        Java_com_kestrel_opensiv3d_MainActivity_onFrameBufferSizeNative(JNIEnv *env, jobject /* this */, jint width, jint height)
        {
            LOG_TRACE(U"JNI_CALL: nativeOnFrameBufferSize({}, {})"_fmt(width, height));

            auto *const pWindow = static_cast<s3d::CWindow *>(s3d::SIV3D_ENGINE(Window));
            if (!pWindow)
            {
                std::cerr << "Warning: nativeOnFrameBufferSize called before engine initialization"
                          << std::endl;
                return;
            }

            pWindow->updateFrameBufferSize(s3d::Size(width, height));
        }

        JNIEXPORT void JNICALL
        Java_com_kestrel_opensiv3d_MainActivity_onScalingChangeNative(JNIEnv *env, jobject /* this */, jfloat sx, jfloat sy)
        {
            LOG_TRACE(U"JNI_CALL: nativeOnScalingChange({}, {})"_fmt(sx, sy));

            auto *const pWindow = static_cast<s3d::CWindow *>(s3d::SIV3D_ENGINE(Window));
            if (!pWindow)
            {
                std::cerr << "Warning: nativeOnScalingChange called before engine initialization"
                          << std::endl;
                return;
            }

            pWindow->updateScaling(s3d::Max(sx, sy));
        }

        JNIEXPORT void JNICALL
        Java_com_kestrel_opensiv3d_MainActivity_onPauseNative(JNIEnv *env, jobject /* this */)
        {
            LOG_TRACE(U"JNI_CALL: nativeOnPause()");

            auto *const pWindow = static_cast<s3d::CWindow *>(s3d::SIV3D_ENGINE(Window));
            if (!pWindow)
            {
                std::cerr << "Warning: nativeOnPause called before engine initialization" << std::endl;
                return;
            }

            pWindow->updateFocus(false);
            s3d::SIV3D_ENGINE(UserAction)->reportUserActions(s3d::UserAction::WindowDeactivated);
        }

        JNIEXPORT void JNICALL
        Java_com_kestrel_opensiv3d_MainActivity_onResumeNative(JNIEnv *env, jobject /* this */)
        {
            LOG_TRACE(U"JNI_CALL: nativeOnResume()");

            auto *const pWindow = static_cast<s3d::CWindow *>(s3d::SIV3D_ENGINE(Window));
            if (!pWindow)
            {
                std::cerr << "Warning: nativeOnResume called before engine initialization" << std::endl;
                return;
            }

            pWindow->updateFocus(true);
        }

        JNIEXPORT void JNICALL
        Java_com_kestrel_opensiv3d_MainActivity_SetMainActivityNative(JNIEnv *env, jobject thiz)
        {
            LOG_TRACE(U"JNI_CALL: nativeSetMainActivity()");

            if (g_MainActivity)
            {
                env->DeleteGlobalRef(g_MainActivity);
                g_MainActivity = nullptr;
            }

            g_MainActivity = env->NewGlobalRef(thiz);
            LOG_TRACE(U"MainActivity reference stored: {}"_fmt(
                static_cast<size_t>(reinterpret_cast<std::uintptr_t>(g_MainActivity))));
        }
    }

#endif
}
