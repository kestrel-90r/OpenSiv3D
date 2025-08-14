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

# include <Siv3D/Common.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/UserAction.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Common/Siv3DEngine.hpp>
# include <Siv3D/Keyboard/FallbackKeyName.hpp>
# include "CKeyboard.hpp"

namespace s3d
{
	namespace AndroidKeyTable
	{
		#define SIV3D_KEY_BACKSPACE     0x08
		#define SIV3D_KEY_TAB           0x09
		#define SIV3D_KEY_ENTER         0x0D
		#define SIV3D_KEY_SHIFT         0x10
		#define SIV3D_KEY_CONTROL       0x11
		#define SIV3D_KEY_ALT           0x12
		#define SIV3D_KEY_PAUSE         0x13
		#define SIV3D_KEY_ESCAPE        0x1B
		#define SIV3D_KEY_SPACE         0x20
		#define SIV3D_KEY_PAGE_UP       0x21
		#define SIV3D_KEY_PAGE_DOWN     0x22
		#define SIV3D_KEY_END           0x23
		#define SIV3D_KEY_HOME          0x24
		#define SIV3D_KEY_LEFT          0x25
		#define SIV3D_KEY_UP            0x26
		#define SIV3D_KEY_RIGHT         0x27
		#define SIV3D_KEY_DOWN          0x28
		#define SIV3D_KEY_INSERT        0x2D
		#define SIV3D_KEY_DELETE        0x2E
		#define SIV3D_KEY_0             0x30
		#define SIV3D_KEY_1             0x31
		#define SIV3D_KEY_2             0x32
		#define SIV3D_KEY_3             0x33
		#define SIV3D_KEY_4             0x34
		#define SIV3D_KEY_5             0x35
		#define SIV3D_KEY_6             0x36
		#define SIV3D_KEY_7             0x37
		#define SIV3D_KEY_8             0x38
		#define SIV3D_KEY_9             0x39
		#define SIV3D_KEY_A             0x41
		#define SIV3D_KEY_B             0x42
		#define SIV3D_KEY_C             0x43
		#define SIV3D_KEY_D             0x44
		#define SIV3D_KEY_E             0x45
		#define SIV3D_KEY_F             0x46
		#define SIV3D_KEY_G             0x47
		#define SIV3D_KEY_H             0x48
		#define SIV3D_KEY_I             0x49
		#define SIV3D_KEY_J             0x4A
		#define SIV3D_KEY_K             0x4B
		#define SIV3D_KEY_L             0x4C
		#define SIV3D_KEY_M             0x4D
		#define SIV3D_KEY_N             0x4E
		#define SIV3D_KEY_O             0x4F
		#define SIV3D_KEY_P             0x50
		#define SIV3D_KEY_Q             0x51
		#define SIV3D_KEY_R             0x52
		#define SIV3D_KEY_S             0x53
		#define SIV3D_KEY_T             0x54
		#define SIV3D_KEY_U             0x55
		#define SIV3D_KEY_V             0x56
		#define SIV3D_KEY_W             0x57
		#define SIV3D_KEY_X             0x58
		#define SIV3D_KEY_Y             0x59
		#define SIV3D_KEY_Z             0x5A
		
		inline std::pair<int32_t, uint8> KeyConversionTable[] =
		{
			{ AKEYCODE_DPAD_UP,      SIV3D_KEY_UP },
			{ AKEYCODE_DPAD_DOWN,    SIV3D_KEY_DOWN },
			{ AKEYCODE_DPAD_LEFT,    SIV3D_KEY_LEFT },
			{ AKEYCODE_DPAD_RIGHT,   SIV3D_KEY_RIGHT },
			{ AKEYCODE_ENTER,        SIV3D_KEY_ENTER },
			{ AKEYCODE_DEL,          SIV3D_KEY_BACKSPACE },
			{ AKEYCODE_FORWARD_DEL,  SIV3D_KEY_DELETE },
			{ AKEYCODE_TAB,          SIV3D_KEY_TAB },
			{ AKEYCODE_SPACE,        SIV3D_KEY_SPACE },
			{ AKEYCODE_ESCAPE,       SIV3D_KEY_ESCAPE },
			{ AKEYCODE_MOVE_HOME,    SIV3D_KEY_HOME },
			{ AKEYCODE_MOVE_END,     SIV3D_KEY_END },
			{ AKEYCODE_PAGE_UP,      SIV3D_KEY_PAGE_UP },
			{ AKEYCODE_PAGE_DOWN,    SIV3D_KEY_PAGE_DOWN },
			{ AKEYCODE_INSERT,       SIV3D_KEY_INSERT },
			// 数字キー
			{ AKEYCODE_0,            SIV3D_KEY_0 },
			{ AKEYCODE_1,            SIV3D_KEY_1 },
			{ AKEYCODE_2,            SIV3D_KEY_2 },
			{ AKEYCODE_3,            SIV3D_KEY_3 },
			{ AKEYCODE_4,            SIV3D_KEY_4 },
			{ AKEYCODE_5,            SIV3D_KEY_5 },
			{ AKEYCODE_6,            SIV3D_KEY_6 },
			{ AKEYCODE_7,            SIV3D_KEY_7 },
			{ AKEYCODE_8,            SIV3D_KEY_8 },
			{ AKEYCODE_9,            SIV3D_KEY_9 },
			// テンキー
			{ AKEYCODE_NUMPAD_0,     SIV3D_KEY_0 },
			{ AKEYCODE_NUMPAD_1,     SIV3D_KEY_1 },
			{ AKEYCODE_NUMPAD_2,     SIV3D_KEY_2 },
			{ AKEYCODE_NUMPAD_3,     SIV3D_KEY_3 },
			{ AKEYCODE_NUMPAD_4,     SIV3D_KEY_4 },
			{ AKEYCODE_NUMPAD_5,     SIV3D_KEY_5 },
			{ AKEYCODE_NUMPAD_6,     SIV3D_KEY_6 },
			{ AKEYCODE_NUMPAD_7,     SIV3D_KEY_7 },
			{ AKEYCODE_NUMPAD_8,     SIV3D_KEY_8 },
			{ AKEYCODE_NUMPAD_9,     SIV3D_KEY_9 },
			// アルファベット
			{ AKEYCODE_A,            SIV3D_KEY_A },
			{ AKEYCODE_B,            SIV3D_KEY_B },
			{ AKEYCODE_C,            SIV3D_KEY_C },
			{ AKEYCODE_D,            SIV3D_KEY_D },
			{ AKEYCODE_E,            SIV3D_KEY_E },
			{ AKEYCODE_F,            SIV3D_KEY_F },
			{ AKEYCODE_G,            SIV3D_KEY_G },
			{ AKEYCODE_H,            SIV3D_KEY_H },
			{ AKEYCODE_I,            SIV3D_KEY_I },
			{ AKEYCODE_J,            SIV3D_KEY_J },
			{ AKEYCODE_K,            SIV3D_KEY_K },
			{ AKEYCODE_L,            SIV3D_KEY_L },
			{ AKEYCODE_M,            SIV3D_KEY_M },
			{ AKEYCODE_N,            SIV3D_KEY_N },
			{ AKEYCODE_O,            SIV3D_KEY_O },
			{ AKEYCODE_P,            SIV3D_KEY_P },
			{ AKEYCODE_Q,            SIV3D_KEY_Q },
			{ AKEYCODE_R,            SIV3D_KEY_R },
			{ AKEYCODE_S,            SIV3D_KEY_S },
			{ AKEYCODE_T,            SIV3D_KEY_T },
			{ AKEYCODE_U,            SIV3D_KEY_U },
			{ AKEYCODE_V,            SIV3D_KEY_V },
			{ AKEYCODE_W,            SIV3D_KEY_W },
			{ AKEYCODE_X,            SIV3D_KEY_X },
			{ AKEYCODE_Y,            SIV3D_KEY_Y },
			{ AKEYCODE_Z,            SIV3D_KEY_Z },
			// 修飾キー
			{ AKEYCODE_SHIFT_LEFT,   SIV3D_KEY_SHIFT },
			{ AKEYCODE_SHIFT_RIGHT,  SIV3D_KEY_SHIFT },
			{ AKEYCODE_CTRL_LEFT,    SIV3D_KEY_CONTROL },
			{ AKEYCODE_CTRL_RIGHT,   SIV3D_KEY_CONTROL },
			{ AKEYCODE_ALT_LEFT,     SIV3D_KEY_ALT },
			{ AKEYCODE_ALT_RIGHT,    SIV3D_KEY_ALT },
		};
		
		// キー名テーブル
		inline const char* KeyNameTable[256] = {
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			"BackSpace", "Tab", nullptr, nullptr, nullptr, "Enter", nullptr, nullptr,
			"Shift", "Ctrl", "Alt", "Pause", "CapsLock", nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, "Esc", nullptr, nullptr, nullptr, nullptr,
			"Space", "PageUp", "PageDown", "End", "Home", "Left", "Up", "Right",
			"Down", nullptr, nullptr, nullptr, "PrintScreen", "Insert", "Delete", nullptr,
			"0", "1", "2", "3", "4", "5", "6", "7",
			"8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, "A", "B", "C", "D", "E", "F", "G",
			"H", "I", "J", "K", "L", "M", "N", "O",
			"P", "Q", "R", "S", "T", "U", "V", "W",
			"X", "Y", "Z", nullptr, nullptr, nullptr, nullptr, nullptr,
			"NumPad0", "NumPad1", "NumPad2", "NumPad3", "NumPad4", "NumPad5", "NumPad6", "NumPad7",
			"NumPad8", "NumPad9", "NumPad*", "NumPad+", nullptr, "NumPad-", "NumPad.", "NumPad/",
			"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8",
			"F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16",
			"F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24",
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			"NumLock", "ScrollLock", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			"LShift", "RShift", "LCtrl", "RCtrl", "LAlt", "RAlt", nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, ":", "+", ",", "-", ".", "?",
			"~", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			"[", "\\", "]", "'", nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "_", nullptr,
		};
	}

	CKeyboard::CKeyboard()
	{

	}

	CKeyboard::~CKeyboard()
	{
		LOG_SCOPED_TRACE(U"CKeyboard::~CKeyboard()");
	}

	void CKeyboard::init()
	{
		LOG_SCOPED_TRACE(U"CKeyboard::init()");

		m_window = SIV3D_ENGINE(Window)->getHandle();
		
		for (size_t i = 0; i < InputState::KeyCount; ++i)
		{
			if (i < std::size(AndroidKeyTable::KeyNameTable) && AndroidKeyTable::KeyNameTable[i])
			{
				m_names[i] = Unicode::Widen(AndroidKeyTable::KeyNameTable[i]);
			}
			else
			{
				m_names[i] = U"{:#04x}"_fmt(i);
			}
			
			if (m_names[i].size() == 1 && IsLower(m_names[i].front()))
			{
				m_names[i].uppercase();
			}
		}
	}

	void CKeyboard::update()
	{
		if (not m_window)
		{
			return;
		}
		
	
        m_allInputs.clear();
        
        for (uint32 i = 8; i < 0xEF; ++i)
        {
            const auto& state = m_states[i];

            if (state.pressed || state.up)
            {
                m_allInputs.emplace_back(InputDeviceType::Keyboard, static_cast<uint8>(i));
            }
        }
		
        if (m_states[0x1B].down)
        {
            SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::AnyKeyDown | UserAction::EscapeKeyDown);
        }
        else
        {
            for (const auto& input : m_allInputs)
            {
                if (input.down())
                {
                    SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::AnyKeyDown);
                    break;
                }
            }
        }
	}

	bool CKeyboard::down(const uint32 index) const
	{
		assert(index < InputState::KeyCount);
		return m_states[index].down;
	}

	bool CKeyboard::pressed(const uint32 index) const
	{
		assert(index < InputState::KeyCount);
		return m_states[index].pressed;
	}

	bool CKeyboard::up(const uint32 index) const
	{
		assert(index < InputState::KeyCount);
		return m_states[index].up;
	}

	Duration CKeyboard::pressedDuration(const uint32 index) const
	{
		assert(index < InputState::KeyCount);
		return m_states[index].pressedDuration;
	}

	const String& CKeyboard::name(const uint32 index) const
	{
		assert(index < InputState::KeyCount);
		return m_names[index];
	}

	const Array<Input>& CKeyboard::getAllInput() const noexcept
	{
		return m_allInputs;
	}

	Array<KeyEvent> CKeyboard::getEvents() const noexcept
	{
		static const Array<KeyEvent> _empty;
		return _empty;
	}
	
	uint8 CKeyboard::convertAndroidKeyToSiv3D(int32_t keyCode) const
	{
		for (const auto& pair : AndroidKeyTable::KeyConversionTable)
		{
			if (pair.first == keyCode)
			{
				return pair.second;
			}
		}
		return 0;
	}
	
	void CKeyboard::onKeyEvent(int32_t keyCode, bool isPressed)
	{
		uint8 siv3dKeyCode = convertAndroidKeyToSiv3D(keyCode);
		
		if (siv3dKeyCode != 0)
		{
			m_states[siv3dKeyCode].update(isPressed);
		}
		else
		{
			LOG_TRACE(U"Unhandled key event: Android keyCode={}, state={}"_fmt(
				        keyCode, isPressed ? U"pressed" : U"released"));
		}
	}
}
