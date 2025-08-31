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

#pragma once
#include <mutex>
#include <Siv3D/Mouse/IMouse.hpp>
#include <Siv3D/Input/InputState.hpp>
#include <Siv3D/PointVector.hpp>
#include <Siv3D/Common/OpenGLES.hpp>

namespace s3d
{
    class CMouse final : public ISiv3DMouse
    {
    private:
        enum class MouseButtonState
        {
            Released,

            Pressed,

            Tapped,
        };

        void *m_window = nullptr;

        //
        // Buttons
        //

        std::mutex m_buttonMutex;

        std::array<MouseButtonState, InputState::MouseButtonCount> m_buttonsInternal;

        std::array<InputState, InputState::MouseButtonCount> m_states;

        Array<Input> m_allInputs;

        //
        // Scroll
        //

        std::mutex m_scrollMutex;

        Vec2 m_scrollInternal{0.0, 0.0};

        Vec2 m_scroll{0.0, 0.0};

    public:
        CMouse();

        ~CMouse() override;

        void init() override;

        void update() override;

        bool down(uint32 index) const override;

        bool pressed(uint32 index) const override;

        bool up(uint32 index) const override;

        Duration pressedDuration(uint32 index) const override;

        const Array<Input> &getAllInput() const noexcept override;

        const Vec2 &wheel() const noexcept override;

        void onMouseButtonUpdated(int32 index, bool pressed) override;

        void onScroll(double v, double h) override;

        void onTouchEvent(int action, Point pos);

        void updateButtonDown(uint32 index);

        void updateButtonUp(uint32 index);


    };
}
