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

#include <Siv3D/EngineLog.hpp>
#include <Siv3D/Unicode.hpp>
#include <Siv3D/Common/Siv3DEngine.hpp>
#include "CGamepad.hpp"

namespace s3d
{
    namespace detail
    {
        static constexpr size_t MaxGamepadButtons = 16;
    }

    CGamepad::CGamepad()
        : m_states(), m_inputs{{detail::Gamepad_impl(0), detail::Gamepad_impl(1),
                                detail::Gamepad_impl(2), detail::Gamepad_impl(3),
                                detail::Gamepad_impl(4), detail::Gamepad_impl(5),
                                detail::Gamepad_impl(6), detail::Gamepad_impl(7),
                                detail::Gamepad_impl(8), detail::Gamepad_impl(9),
                                detail::Gamepad_impl(10), detail::Gamepad_impl(11),
                                detail::Gamepad_impl(12), detail::Gamepad_impl(13),
                                detail::Gamepad_impl(14), detail::Gamepad_impl(15)}}
    {
    }

    CGamepad::~CGamepad()
    {
        LOG_SCOPED_TRACE(U"CGamepad::~CGamepad()");
    }

    void CGamepad::init()
    {
        LOG_SCOPED_TRACE(U"CGamepad::init()");
        update();
    }

    bool CGamepad::isGamepadConnected(uint32 playerIndex) const
    {
        // TODO: Android固有のゲームパッド接続確認処理
        return false;
    }

    void CGamepad::updateGamepadAxes(uint32 playerIndex, GamepadState &state)
    {
        // TODO: Android固有の軸状態更新処理
        state.axes << 0.0 << 0.0;
    }

    void CGamepad::updateGamepadButtons(uint32 playerIndex, GamepadState &state)
    {
        // TODO: Android固有のボタン状態更新処理
        state.buttons.resize(detail::MaxGamepadButtons);
        for (auto &button : state.buttons)
        {
            button.update(false);
        }
    }

    void CGamepad::updateGamepadPOV(uint32 playerIndex, GamepadState &state)
    {
        // TODO: Android固有のPOV状態更新処理
        state.povs.resize(1);
        for (auto &pov : state.povs)
        {
            pov.update(false);
        }
        state.povDegree = none;
    }

    void CGamepad::update()
    {
        for (uint32 playerIndex = 0; playerIndex < Gamepad.MaxPlayerCount; ++playerIndex)
        {
            auto &state = m_states[playerIndex];

            if (isGamepadConnected(playerIndex))
            {
                if (not state.connected)
                {
                    // 新規接続の処理
                    state.info.playerIndex = playerIndex;
                    state.info.vendorID = 0;  // TODO: 実際のベンダーID
                    state.info.productID = 0; // TODO: 実際のプロダクトID
                    state.info.name = U"Android Gamepad " + ToString(playerIndex);

                    state.connected = true;

                    LOG_INFO(U"🎮 Gamepad({}) `{}` connected"_fmt(playerIndex, state.info.name));
                }

                // 軸の状態更新
                state.axes.clear();
                updateGamepadAxes(playerIndex, state);

                // ボタンの状態更新
                updateGamepadButtons(playerIndex, state);

                // POVの状態更新
                updateGamepadPOV(playerIndex, state);
            }
            else
            {
                if (state.connected)
                {
                    LOG_INFO(U"🎮 Gamepad({}) `{}` disconnected"_fmt(playerIndex, state.info.name));
                    state.clear();
                }
            }
        }

        // 入力情報の更新
        for (uint8 playerIndex = 0; playerIndex < Gamepad.MaxPlayerCount; ++playerIndex)
        {
            const auto &src = m_states[playerIndex];
            auto &dst = m_inputs[playerIndex];

            dst.axes = src.axes;
            dst.buttons.clear();

            for (uint32 i = 0; i < src.buttons.size(); ++i)
            {
                dst.buttons.emplace_back(InputDeviceType::Gamepad, static_cast<uint8>(i), playerIndex);
            }
        }
    }

    Array<GamepadInfo> CGamepad::enumerate()
    {
        Array<GamepadInfo> results;

        for (uint32 playerIndex = 0; playerIndex < Gamepad.MaxPlayerCount; ++playerIndex)
        {
            if (isGamepadConnected(playerIndex))
            {
                GamepadInfo info;
                info.playerIndex = playerIndex;
                info.vendorID = 0;  // TODO: 実際のベンダーID
                info.productID = 0; // TODO: 実際のプロダクトID
                info.name = U"Android Gamepad " + ToString(playerIndex);
                results << info;
            }
        }

        return results;
    }

    bool CGamepad::isConnected(const size_t playerIndex)
    {
        assert(playerIndex < Gamepad.MaxPlayerCount);

        return m_states[playerIndex].connected;
    }

    const GamepadInfo &CGamepad::getInfo(const size_t playerIndex)
    {
        assert(playerIndex < Gamepad.MaxPlayerCount);

        return m_states[playerIndex].info;
    }

    bool CGamepad::down(const size_t playerIndex, const uint32 buttonIndex)
    {
        const auto &state = m_states[playerIndex];
        return state.connected && buttonIndex < state.buttons.size() && state.buttons[buttonIndex].down;
    }

    bool CGamepad::pressed(const size_t playerIndex, const uint32 buttonIndex)
    {
        const auto &state = m_states[playerIndex];
        return state.connected && buttonIndex < state.buttons.size() && state.buttons[buttonIndex].pressed;
    }

    bool CGamepad::up(const size_t playerIndex, const uint32 buttonIndex)
    {
        const auto &state = m_states[playerIndex];
        return state.connected && buttonIndex < state.buttons.size() && state.buttons[buttonIndex].up;
    }

    Duration CGamepad::pressedDuration(const size_t playerIndex, const uint32 buttonIndex)
    {
        const auto &state = m_states[playerIndex];

        if (!state.connected || buttonIndex >= state.buttons.size())
        {
            return Duration{0};
        }

        return state.buttons[buttonIndex].pressedDuration;
    }

    Optional<int32> CGamepad::povDegree(const size_t playerIndex)
    {
        const auto &state = m_states[playerIndex];

        if (!state.connected || !state.povDegree)
        {
            return none;
        }

        return static_cast<int32>(state.povDegree.value());
    }

    const detail::Gamepad_impl &CGamepad::getInput(const size_t playerIndex)
    {
        assert(playerIndex < Gamepad.MaxPlayerCount);

        return m_inputs[playerIndex];
    }
}
