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

#include <Siv3D/Common.hpp>
#include <Siv3D/EngineLog.hpp>
#include <Siv3D/Window/IWindow.hpp>
#include <Siv3D/Common/Siv3DEngine.hpp>
#include "CMouse.hpp"

#include <jni.h>

namespace s3d
{
    CMouse::CMouse()
    {
    }

    CMouse::~CMouse()
    {
        LOG_SCOPED_TRACE(U"CMouse::~CMouse()");
    }

    void CMouse::init()
    {
        LOG_SCOPED_TRACE(U"CMouse::init()");
        m_window = SIV3D_ENGINE(Window)->getHandle();

        m_buttonsInternal.fill(MouseButtonState::Released);
    }

    void CMouse::update()
    {
        std::lock_guard lock{m_buttonMutex};

        for (uint32 i = 0; i < InputState::MouseButtonCount; ++i)
        {
            auto &state = m_buttonsInternal[i];

            const bool pressed = (state == MouseButtonState::Pressed) || (state == MouseButtonState::Tapped);

            m_states[i].update(pressed);

            if (state == MouseButtonState::Tapped)
            {
                state = MouseButtonState::Released;
            }
        }

        {
            std::lock_guard lock{m_scrollMutex};

            m_scroll = std::exchange(m_scrollInternal, Vec2{0, 0});
        }

        {
            m_allInputs.clear();

            for (uint32 i = 0; i < InputState::MouseButtonCount; ++i)
            {
                const auto &state = m_states[i];

                if (state.pressed || state.up)
                {
                    m_allInputs.emplace_back(InputDeviceType::Mouse, static_cast<uint8>(i));
                }
            }
        }
    }

    bool CMouse::down(const uint32 index) const
    {
        assert(index < InputState::MouseButtonCount);
        bool result = m_states[index].down;
        return result;
    }

    bool CMouse::pressed(const uint32 index) const
    {
        assert(index < InputState::MouseButtonCount);
        return m_states[index].pressed;
    }

    bool CMouse::up(const uint32 index) const
    {
        assert(index < InputState::MouseButtonCount);
        return m_states[index].up;
    }

    Duration CMouse::pressedDuration(const uint32 index) const
    {
        assert(index < InputState::MouseButtonCount);
        return m_states[index].pressedDuration;
    }

    const Array<Input> &CMouse::getAllInput() const noexcept
    {
        return m_allInputs;
    }

    const Vec2 &CMouse::wheel() const noexcept
    {
        return m_scroll;
    }

    void CMouse::onMouseButtonUpdated(const int32 index, const bool pressed)
    {
        std::lock_guard lock{m_buttonMutex};

        auto &state = m_buttonsInternal[index];

        if (state == MouseButtonState::Released)
        {
            if (pressed)
            {
                state = MouseButtonState::Pressed;
            }
        }

        else if (state == MouseButtonState::Pressed)
        {
            if (!pressed)
            {
                state = MouseButtonState::Tapped;
            }
        }

        else
        {
            if (pressed)
            {
                state = MouseButtonState::Pressed;
            }
        }
    }

    void CMouse::onScroll(const double x, const double y)
    {
        std::lock_guard lock{m_scrollMutex};
        m_scrollInternal.moveBy(x, y);
    }

    void CMouse::onTouchEvent(const int action, const Point pos)
    {
        const bool pressed = (action == 0 || action == 2);
        onMouseButtonUpdated(0, pressed);
    }

    void CMouse::updateButtonDown(uint32 index)
    {
        if (index >= InputState::MouseButtonCount)
        {
            LOG_ERROR(U"CMouse::updateButtonDown: invalid index {}"_fmt(index));
            return;
        }
        
        std::lock_guard lock{m_buttonMutex};
        auto& state = m_buttonsInternal[index];
        if (state == MouseButtonState::Released)
        {
            state = MouseButtonState::Pressed;
        }
    }

    void CMouse::updateButtonUp(uint32 index)
    {
        if (index >= InputState::MouseButtonCount)
        {
            LOG_ERROR(U"CMouse::updateButtonUp: invalid index {}"_fmt(index));
            return;
        }
        
        std::lock_guard lock{m_buttonMutex};
        auto& state = m_buttonsInternal[index];
        if (state == MouseButtonState::Pressed)
        {
            state = MouseButtonState::Tapped;
        }
    }
}

using namespace s3d;
extern "C"
{
    JNIEXPORT void JNICALL
    Java_com_kestrel_opensiv3d_MainActivity_onMouseEventNative(
        JNIEnv *env, jobject /* this */, jint action, jint x, jint y)
    {
        if (auto *mouse = static_cast<s3d::CMouse *>(s3d::Siv3DEngine::Get<s3d::ISiv3DMouse>()))
        {
            mouse->onTouchEvent(action, s3d::Point{static_cast<int>(x), static_cast<int>(y)});
        }
    }
}
