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
#include <Siv3D/Gamepad/IGamepad.hpp>
#include <Siv3D/Gamepad/GamepadState.hpp>
#include <Siv3D/Array.hpp>

namespace s3d
{
    class CGamepad final : public ISiv3DGamepad
    {
    private:
        struct GamepadState
        {
            bool connected = false;
            GamepadInfo info;
            Array<double> axes;
            Array<InputState> buttons;
            Array<InputState> povs;
            Optional<double> povDegree;

            void clear()
            {
                connected = false;
                info = {};
                axes.clear();
                buttons.clear();
                povs.clear();
                povDegree.reset();
            }
        };

        std::array<GamepadState, Gamepad.MaxPlayerCount> m_states;
        std::array<detail::Gamepad_impl, Gamepad.MaxPlayerCount> m_inputs;

        bool isGamepadConnected(uint32 playerIndex) const;
        void updateGamepadAxes(uint32 playerIndex, GamepadState &state);
        void updateGamepadButtons(uint32 playerIndex, GamepadState &state);
        void updateGamepadPOV(uint32 playerIndex, GamepadState &state);

    public:
        CGamepad();
        ~CGamepad() override;
        void init() override;
        void update() override;
        Array<GamepadInfo> enumerate() override;
        bool isConnected(size_t playerIndex) override;
        const GamepadInfo &getInfo(size_t playerIndex) override;
        bool down(size_t playerIndex, uint32 buttonIndex) override;
        bool pressed(size_t playerIndex, uint32 buttonIndex) override;
        bool up(size_t playerIndex, uint32 buttonIndex) override;
        Duration pressedDuration(size_t playerIndex, uint32 buttonIndex) override;
        Optional<int32> povDegree(size_t playerIndex) override;
        const detail::Gamepad_impl &getInput(size_t playerIndex) override;
    };
}
