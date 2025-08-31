//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Common/Siv3DEngine.hpp>
# include "CGamepad.hpp"

namespace s3d
{
	CGamepad::CGamepad()
		: m_states()
		, m_inputs{ {detail::Gamepad_impl(0), detail::Gamepad_impl(1),
					detail::Gamepad_impl(2), detail::Gamepad_impl(3),
					detail::Gamepad_impl(4), detail::Gamepad_impl(5),
					detail::Gamepad_impl(6), detail::Gamepad_impl(7),
					detail::Gamepad_impl(8), detail::Gamepad_impl(9),
					detail::Gamepad_impl(10), detail::Gamepad_impl(11),
					detail::Gamepad_impl(12), detail::Gamepad_impl(13),
					detail::Gamepad_impl(14), detail::Gamepad_impl(15) } }
	{
		initializeKeyMappings(); // キーマッピングを初期化
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

	void CGamepad::update()
	{
		// Androidでは外部からイベントが来るため、ここでは状態の更新のみ
		for (uint8 playerIndex = 0; playerIndex < Gamepad.MaxPlayerCount; ++playerIndex)
		{
			const auto& src = m_states[playerIndex];
			auto& dst = m_inputs[playerIndex];
			
			dst.axes = src.axes;
			dst.buttons.clear();
			
			for (uint32 i = 0; i < src.buttons.size(); ++i)
			{
				dst.buttons.emplace_back(InputDeviceType::Gamepad, static_cast<uint8>(i), playerIndex);
			}
		}
	}

	void CGamepad::initializeKeyMappings()
	{
		// Android KeyEventからゲームパッドボタンへのマッピング
		// 正しいAKEYCODE_プレフィックスを使用
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_A] = 0;        // A
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_B] = 1;        // B
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_X] = 2;        // X
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_Y] = 3;        // Y
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_L1] = 4;       // L1
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_R1] = 5;       // R1
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_L2] = 6;       // L2
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_R2] = 7;       // R2
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_THUMBL] = 8;   // Left Stick
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_THUMBR] = 9;   // Right Stick
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_START] = 10;   // Start
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_SELECT] = 11;  // Select
		m_keyCodeToButtonMap[AKEYCODE_BUTTON_MODE] = 12;    // Mode
		
		// D-pad
		m_keyCodeToButtonMap[AKEYCODE_DPAD_UP] = 0x80;      // D-pad Up
		m_keyCodeToButtonMap[AKEYCODE_DPAD_RIGHT] = 0x81;   // D-pad Right
		m_keyCodeToButtonMap[AKEYCODE_DPAD_DOWN] = 0x82;    // D-pad Down
		m_keyCodeToButtonMap[AKEYCODE_DPAD_LEFT] = 0x83;    // D-pad Left
	}

	void CGamepad::handleKeyEvent(int32 keyCode, int32 action, int32 deviceId)
	{
		auto keyIt = m_keyCodeToButtonMap.find(keyCode);
		if (keyIt == m_keyCodeToButtonMap.end())
		{
			// 未マッピングのキーコードをログ出力（デバッグ用）
			LOG_INFO(U" Unmapped key code: {} (device: {})"_fmt(keyCode, deviceId));
			return;
		}

		size_t playerIndex = getOrCreatePlayerIndex(deviceId);
		if (playerIndex >= Gamepad.MaxPlayerCount)
			return;

		uint32 buttonIndex = keyIt->second;
		bool pressed = (action == AKEY_EVENT_ACTION_DOWN);

		// ボタン状態の更新
		if (buttonIndex < 0x80) // 通常ボタン
		{
			if (buttonIndex >= m_states[playerIndex].buttons.size())
			{
				m_states[playerIndex].buttons.resize(buttonIndex + 1);
			}
			m_states[playerIndex].buttons[buttonIndex].update(pressed);
			
			LOG_INFO(U"🎮 Button {} {} (device: {}, player: {})"_fmt(
				buttonIndex, pressed ? U"pressed" : U"released", deviceId, playerIndex));
		}
		else // D-pad
		{
			uint32 povIndex = buttonIndex - 0x80;
			if (povIndex < 4)
			{
				m_states[playerIndex].povs[povIndex].update(pressed);
				
				LOG_INFO(U"🎮 D-pad {} {} (device: {}, player: {})"_fmt(
					povIndex, pressed ? U"pressed" : U"released", deviceId, playerIndex));
			}
		}

		// 接続状態の更新
		if (!m_states[playerIndex].connected)
		{
			m_states[playerIndex].connected = true;
			m_states[playerIndex].info.playerIndex = playerIndex;
			m_states[playerIndex].info.name = U"Android Gamepad";
			LOG_INFO(U"🎮 Gamepad({}) connected (device: {})"_fmt(playerIndex, deviceId));
		}
	}

	void CGamepad::handleMotionEvent(int32 source, int32 action, float x, float y, int32 deviceId)
	{
		// ゲームパッドのアナログスティック処理
		if (source == AINPUT_SOURCE_JOYSTICK)
		{
			size_t playerIndex = getOrCreatePlayerIndex(deviceId);
			if (playerIndex >= Gamepad.MaxPlayerCount)
				return;

			updateAxesFromMotionEvent(playerIndex, x, y);
		}
	}

	size_t CGamepad::getOrCreatePlayerIndex(int32 deviceId)
	{
		auto it = m_deviceToPlayerMap.find(deviceId);
		if (it != m_deviceToPlayerMap.end())
		{
			return it->second;
		}

		// 新しいプレイヤーインデックスを割り当て
		for (size_t i = 0; i < Gamepad.MaxPlayerCount; ++i)
		{
			if (!m_states[i].connected)
			{
				m_deviceToPlayerMap[deviceId] = i;
				return i;
			}
		}

		return Gamepad.MaxPlayerCount; // 接続不可
	}

	void CGamepad::updateAxesFromMotionEvent(size_t playerIndex, float x, float y)
	{
		// アナログスティックの値を更新
		if (m_states[playerIndex].axes.size() < 2)
		{
			m_states[playerIndex].axes.resize(2);
		}

		m_states[playerIndex].axes[0] = x; // X軸
		m_states[playerIndex].axes[1] = y; // Y軸
	}

	Array<GamepadInfo> CGamepad::enumerate()
	{
		Array<GamepadInfo> results;
		
		for (uint32 playerIndex = 0; playerIndex < Gamepad.MaxPlayerCount; ++playerIndex)
		{
			if (m_states[playerIndex].connected)
			{
				results << m_states[playerIndex].info;
			}
		}
		
		return results;
	}

	bool CGamepad::isConnected(const size_t playerIndex)
	{
		assert(playerIndex < Gamepad.MaxPlayerCount);
		return m_states[playerIndex].connected;
	}

	const GamepadInfo& CGamepad::getInfo(const size_t playerIndex)
	{
		assert(playerIndex < Gamepad.MaxPlayerCount);
		return m_states[playerIndex].info;
	}

	bool CGamepad::down(const size_t playerIndex, const uint32 index)
	{
		assert(playerIndex < Gamepad.MaxPlayerCount);

		if (index < m_states[playerIndex].buttons.size())
		{
			return m_states[playerIndex].buttons[index].down;
		}
		else if (InRange(index, 0x80u, 0x83u))
		{
			return m_states[playerIndex].povs[(index - 0x80u)].down;
		}

		return false;
	}

	bool CGamepad::pressed(const size_t playerIndex, const uint32 index)
	{
		assert(playerIndex < Gamepad.MaxPlayerCount);

		if (index < m_states[playerIndex].buttons.size())
		{
			return m_states[playerIndex].buttons[index].pressed;
		}
		else if (InRange(index, 0x80u, 0x83u))
		{
			return m_states[playerIndex].povs[(index - 0x80u)].pressed;
		}

		return false;
	}

	bool CGamepad::up(const size_t playerIndex, const uint32 index)
	{
		assert(playerIndex < Gamepad.MaxPlayerCount);

		if (index < m_states[playerIndex].buttons.size())
		{
			return m_states[playerIndex].buttons[index].up;
		}
		else if (InRange(index, 0x80u, 0x83u))
		{
			return m_states[playerIndex].povs[(index - 0x80u)].up;
		}

		return false;
	}

	Duration CGamepad::pressedDuration(const size_t playerIndex, const uint32 index)
	{
		assert(playerIndex < Gamepad.MaxPlayerCount);

		if (index < m_states[playerIndex].buttons.size())
		{
			return m_states[playerIndex].buttons[index].pressedDuration;
		}
		else if (InRange(index, 0x80u, 0x83u))
		{
			return m_states[playerIndex].povs[(index - 0x80u)].pressedDuration;
		}

		return Duration{ 0.0 };
	}

	Optional<int32> CGamepad::povDegree(const size_t playerIndex)
	{
		assert(playerIndex < Gamepad.MaxPlayerCount);
		return m_states[playerIndex].povDegree;
	}

	const detail::Gamepad_impl& CGamepad::getInput(const size_t playerIndex)
	{
		assert(playerIndex < Gamepad.MaxPlayerCount);
		return m_inputs[playerIndex];
	}
}
