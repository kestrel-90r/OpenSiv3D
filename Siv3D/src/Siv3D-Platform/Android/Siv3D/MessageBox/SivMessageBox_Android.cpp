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

#include <Siv3D/MessageBox.hpp>
#include <Siv3D/AsyncTask.hpp>
#include <Siv3D/Monitor.hpp>
#include <Siv3D/Scene.hpp>
#include <Siv3D/Error.hpp>

namespace s3d
{
    namespace detail
    {
        enum class MessageBoxButtons
        {
            OK,
            OKCancel,
            YesNo,
        };

        static MessageBoxResult ShowMessageBox_impl(const char *title, const char *message, MessageBoxStyle style, MessageBoxButtons buttons)
        {
            std::string combinedMessage = std::string(title) + ": " + std::string(message);

            switch (buttons)
            {
            case MessageBoxButtons::OK:
                return MessageBoxResult::OK;
            case MessageBoxButtons::OKCancel:
                return MessageBoxResult::OK; 
            case MessageBoxButtons::YesNo:
                return MessageBoxResult::Yes;
            default:
                return MessageBoxResult::OK;
            }
        }

        static MessageBoxResult ShowMessageBox(const StringView title, const StringView text, MessageBoxStyle style, MessageBoxButtons buttons)
        {
            if (Scene::FrameCount() == 0)
            {
                throw Error{U"Currently, System::MessageBox~ cannot be called outside of a main loop in this platform (Android)"};
            }

            auto result = Async([=]()
            { return ShowMessageBox_impl(title.narrow().c_str(), text.narrow().c_str(), style, buttons); })
                              .get();

            return result;
        }
    }

    namespace System
    {
        MessageBoxResult MessageBoxOK(const StringView title, const StringView text, const MessageBoxStyle style)
        {
            return detail::ShowMessageBox(title, text, style, detail::MessageBoxButtons::OK);
        }

        MessageBoxResult MessageBoxOKCancel(const StringView title, const StringView text, const MessageBoxStyle style)
        {
            return detail::ShowMessageBox(title, text, style, detail::MessageBoxButtons::OKCancel);
        }

        MessageBoxResult MessageBoxYesNo(const StringView title, const StringView text, const MessageBoxStyle style)
        {
            return detail::ShowMessageBox(title, text, style, detail::MessageBoxButtons::YesNo);
        }
    }
}
