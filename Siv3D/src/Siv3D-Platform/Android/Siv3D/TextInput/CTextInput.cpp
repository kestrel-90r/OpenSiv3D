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

#include <Siv3D/EngineLog.hpp>
#include <Siv3D/Keyboard.hpp>
#include <Siv3D/Unicode.hpp>
#include <Siv3D/Window/IWindow.hpp>
#include <Siv3D/Common/Siv3DEngine.hpp>
#include "CTextInput.hpp"

namespace s3d
{
    CTextInput::CTextInput()
    {
    }

    CTextInput::~CTextInput()
    {
        LOG_SCOPED_TRACE(U"CTextInput::~CTextInput()");
    }

    void CTextInput::init()
    {
        LOG_SCOPED_TRACE(U"CTextInput::init()");
    }

    void CTextInput::update()
    {
        {
            std::lock_guard lock{m_mutexChars};

            m_chars = m_internalChars;

            m_internalChars.clear();
        }
    }

    void CTextInput::pushChar(const uint32 ch)
    {
        LOG_INFO(U"CTextInput::pushChar called with: {} (0x{:X}, '{}')"_fmt(ch, ch, static_cast<char32>(ch)));

        std::lock_guard lock{m_mutexChars};

        m_internalChars.push_back(static_cast<char32>(ch));

        LOG_INFO(U"Character added to internal buffer. Buffer size: {}"_fmt(m_internalChars.size()));

        if (m_imeEnabled && ch != '\b')
        {
            m_currentText.push_back(static_cast<char32>(ch));
            LOG_INFO(U"Character added to current text (IME enabled)");
        }
        else if (m_imeEnabled && ch == '\b' && !m_currentText.isEmpty())
        {
            m_currentText.pop_back();
            LOG_INFO(U"Backspace processed (IME enabled)");
        }
    }

    const String &CTextInput::getChars() const
    {
        return m_chars;
    }

    const String &CTextInput::getEditingText() const
    {
        static const String empty;
        return empty;
    }

    void CTextInput::enableIME(const bool enabled)
    {
        m_imeEnabled = enabled;

        if (enabled)
        {
            m_currentText.clear();
        }

        LOG_TRACE(U"TextInput IME: {}"_fmt(enabled ? U"enabled" : U"disabled"));
    }

    void CTextInput::disableIME()
    {
        enableIME(false);
    }

    std::pair<int32, int32> CTextInput::getCursorIndex() const
    {
        return {0, 0};
    }

    const Array<String> &CTextInput::getCandidates() const
    {
        static const Array<String> dummy;
        return dummy;
    }

    const Array<UnderlineStyle> &CTextInput::getEditingTextStyle() const
    {
        static const Array<UnderlineStyle> dummy;
        return dummy;
    }

    const String &CTextInput::getCurrentText() const
    {
        return m_currentText;
    }

    void CTextInput::setCurrentText(const String &text)
    {
        m_currentText = text;
    }
}
