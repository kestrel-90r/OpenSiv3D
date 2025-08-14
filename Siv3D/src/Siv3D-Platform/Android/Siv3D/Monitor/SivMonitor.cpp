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

#include <Siv3D/Monitor.hpp>
#include <Siv3D/Unicode.hpp>
#include <Siv3D/Common/OpenGLES.hpp>

namespace s3d
{
    namespace System
    {
        Array<MonitorInfo> EnumerateMonitors()
        {
            Array<MonitorInfo> results;

            MonitorInfo primary;
            primary.isPrimary = true;
            primary.displayDeviceName = U"Android Display";
            primary.name = U"Default Display";

            // スクリーンの実サイズを設定
            // TODO: JNIを使用してディスプレイメトリクスを取得する場合は
            // ここで設定する
            primary.displayRect.x = 0;
            primary.displayRect.y = 0;
            primary.displayRect.w = 1920;
            primary.displayRect.h = 1080;

            results.push_back(primary);
            return results;
        }

        size_t GetCurrentMonitorIndex()
        {
            return 0;
        }

        MonitorInfo GetCurrentMonitor()
        {
            static const MonitorInfo monitor = EnumerateMonitors()[0];
            return monitor;
        }
    }
}
