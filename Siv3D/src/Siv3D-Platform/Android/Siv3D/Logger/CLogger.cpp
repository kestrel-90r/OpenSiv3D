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

#include <array>
#include <Siv3D/String.hpp>
#include <Siv3D/FormatLiteral.hpp>
#include <Siv3D/Time.hpp>
#include <Siv3D/LogType.hpp>
#include <Siv3D/Utility.hpp>
#include <android/log.h>
#include "CLogger.hpp"

namespace s3d
{
    namespace detail
    {
        constexpr std::array<StringView, 7> LogTypeNames =
        {
            U"[error]   "_sv,
            U"[fail]    "_sv,
            U"[warning] "_sv,
            U""_sv,
            U"[info]    "_sv,
            U"[trace]   "_sv,
            U"[verbose] "_sv,
        };

        constexpr int LogTypeToPriority(LogType type)
        {
            switch (type)
            {
            case LogType::Error:    return ANDROID_LOG_ERROR;
            case LogType::Fail:     return ANDROID_LOG_ERROR;
            case LogType::Warning:  return ANDROID_LOG_WARN;
            case LogType::App:      return ANDROID_LOG_INFO;
            case LogType::Info:     return ANDROID_LOG_INFO;
            case LogType::Trace:    return ANDROID_LOG_DEBUG;
            case LogType::Verbose:  return ANDROID_LOG_VERBOSE;
            default:                return ANDROID_LOG_INFO;
            }
        }
    }

    CLogger::CLogger()
    {
    }

    CLogger::~CLogger()
    {
    }

    void CLogger::write(const LogType type, const StringView s)
    {
        if (not m_enabled)
        {
            return;
        }

        const int64 timeStamp = Time::GetMillisec();
        const StringView logTypeName = detail::LogTypeNames[FromEnum(type)];
        const String text = String(s);
        const std::string output = text.narrow();

        std::lock_guard lock{m_mutex};
        {
            __android_log_print(detail::LogTypeToPriority(type), "Siv3D", "%s", output.c_str());
        }
    }

    void CLogger::setEnabled(const bool enabled)
    {
        m_enabled = enabled;
    }
}
