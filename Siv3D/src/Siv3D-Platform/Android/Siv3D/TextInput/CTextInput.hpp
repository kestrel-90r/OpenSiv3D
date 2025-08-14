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

#pragma once
#include <mutex>
#include <Siv3D/TextInput/ITextInput.hpp>
#include <Siv3D/Stopwatch.hpp>
#include <Siv3D/Common/OpenGLES.hpp>

namespace s3d
{
    class CTextInput final : public ISiv3DTextInput
    {
    public:
        CTextInput();

        ~CTextInput() override;

        void init() override;

        void update() override;

        void pushChar(uint32 ch) override;

        const String &getChars() const override;

        const String &getEditingText() const override;

        void enableIME(bool enabled) override;

        void disableIME();

        std::pair<int32, int32> getCursorIndex() const override;

        const Array<String> &getCandidates() const override;

        const Array<UnderlineStyle> &getEditingTextStyle() const override;

        const String &getCurrentText() const;

        void setCurrentText(const String &text);

    private:
        std::mutex m_mutexChars;

        String m_internalChars;

        String m_chars;

        String m_currentText;

        bool m_imeEnabled = false;
    };
}
