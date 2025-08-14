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
#include <Siv3D/Common/Siv3DEngine.hpp>

#include <Siv3D/Common.hpp>
#include <Siv3D/Image.hpp>
#include <Siv3D/WindowState.hpp>
#include <Siv3D/Window/IWindow.hpp>
#include <Siv3D/Common/Siv3DEngine.hpp>
#include <Siv3D/Renderer/IRenderer.hpp>
#include <Siv3D/EngineLog.hpp>
#include "CCursor.hpp"

namespace s3d
{
    namespace detail
    {
        static Point m_lastTouchPos;

        [[nodiscard]]
        static Point GetScreenPos(void *window)
        {
            return m_lastTouchPos;
        }

        [[nodiscard]]
        static Vec2 GetClientCursorPos(void *window)
        {
            return Vec2{m_lastTouchPos};
        }
    }

    CCursor::CCursor()
        : m_systemCursors{}
    {
    }

    CCursor::~CCursor()
    {
        LOG_SCOPED_TRACE(U"CCursor::~CCursor()");
        m_customCursors.clear();
    }

    void CCursor::init()
    {
        LOG_SCOPED_TRACE(U"CCursor::init()");

        const Vec2 frameBufferSize = SIV3D_ENGINE(Window)->getState().frameBufferSize;
        const Vec2 virtualSize = SIV3D_ENGINE(Window)->getState().virtualSize;
        const double uiScaling = (frameBufferSize.x / virtualSize.x);

        const Vec2 clientPos = virtualSize * 0.5;
        const Point screenPos = clientPos.asPoint();

        m_state.update(clientPos.asPoint(), clientPos / uiScaling, screenPos);
    }

    bool CCursor::update()
    {
        auto [s, viewRect] = SIV3D_ENGINE(Renderer)->getLetterboxComposition();
        m_transformScreen = Mat3x2::Scale(s).translated(viewRect.pos);
        m_transformAll = (m_transformLocal * m_transformCamera * m_transformScreen);
        m_transformAllInv = m_transformAll.inverse();

        const Vec2 clientRawPos = Vec2{m_state.raw.current};
        const Vec2 clientPos = m_transformAllInv.transformPoint(clientRawPos);
        const Point screenPos = m_state.screen.current;

        m_state.update(clientRawPos.asPoint(), clientPos.asPoint(), screenPos);

        return true;
    }

    const CursorState &CCursor::getState() const noexcept
    {
        return m_state;
    }

    void CCursor::setPos(const Point pos)
    {
        // Androidではカーソル位置の設定は無視
    }

    const Mat3x2 &CCursor::getLocalTransform() const noexcept
    {
        return m_transformLocal;
    }

    const Mat3x2 &CCursor::getCameraTransform() const noexcept
    {
        return m_transformCamera;
    }

    const Mat3x2 &CCursor::getScreenTransform() const noexcept
    {
        return m_transformScreen;
    }

    void CCursor::setLocalTransform(const Mat3x2 &matrix)
    {
        if (m_transformLocal == matrix)
        {
            return;
        }

        m_transformLocal = matrix;
        m_transformAll = (m_transformLocal * m_transformCamera * m_transformScreen);
        m_transformAllInv = m_transformAll.inverse();

        updateCursorPos();
    }

    void CCursor::setCameraTransform(const Mat3x2 &matrix)
    {
        if (m_transformCamera == matrix)
        {
            return;
        }

        m_transformCamera = matrix;
        m_transformAll = (m_transformLocal * m_transformCamera * m_transformScreen);
        m_transformAllInv = m_transformAll.inverse();

        updateCursorPos();
    }

    void CCursor::setScreenTransform(const Mat3x2 &matrix)
    {
        if (m_transformScreen == matrix)
        {
            return;
        }

        m_transformScreen = matrix;
        m_transformAll = (m_transformLocal * m_transformCamera * m_transformScreen);
        m_transformAllInv = m_transformAll.inverse();

        updateCursorPos();
    }

    bool CCursor::isClippedToWindow() const noexcept
    {
        return false; // Androidではクリップは常に無効
    }

    void CCursor::clipToWindow(const bool)
    {
        // Androidではクリップは無視
    }

    void CCursor::requestStyle(const CursorStyle)
    {
        // Androidではカーソルスタイルは無視
    }

    void CCursor::setDefaultStyle(const CursorStyle)
    {
        // Androidではカーソルスタイルは無視
    }

    bool CCursor::registerCursor(const StringView, const Image &, const Point)
    {
        // Androidではカスタムカーソルは無視
        return false;
    }

    void CCursor::requestStyle(const StringView)
    {
        // Androidではカーソルスタイルは無視
    }

    void CCursor::updateCursorPos()
    {
        m_state.vec2.previous = m_transformAllInv.transformPoint(m_state.raw.previous);
        m_state.vec2.current = m_transformAllInv.transformPoint(m_state.raw.current);
        m_state.vec2.delta = (m_state.vec2.current - m_state.vec2.previous);

        m_state.point.previous = m_state.vec2.previous.asPoint();
        m_state.point.current = m_state.vec2.current.asPoint();
        m_state.point.delta = m_state.vec2.delta.asPoint();
    }

    // タッチ入力をカーソル位置として扱う
    void CCursor::onTouchEvent(Point pos)
    {
        const Vec2 rawPos = Vec2{pos};
        const Vec2 clientPos = m_transformAllInv.transformPoint(rawPos);

        m_state.update(rawPos.asPoint(), clientPos.asPoint(), pos);
    }
}

using namespace s3d;

extern "C"
{
    JNIEXPORT void JNICALL
    Java_com_kestrel_opensiv3d_MainActivity_onCursorUpdateNative(
        JNIEnv *env, jobject /* this */, jint x, jint y)
    {
        if (auto *cursor = static_cast<s3d::CCursor *>(s3d::Siv3DEngine::Get<s3d::ISiv3DCursor>()))
        {
            cursor->onTouchEvent(s3d::Point{static_cast<int>(x), static_cast<int>(y)});
        }
    }
}
