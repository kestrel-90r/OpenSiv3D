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

#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <Siv3D/PowerStatus.hpp>
#include <Siv3D/EngineLog.hpp>

namespace s3d
{
    namespace detail
    {
        [[nodiscard]]
        static Optional<int32> ReadInt(const char *path)
        {
            std::ifstream ifs{path};

            if (not ifs)
            {
                return none;
            }

            int32 value;

            if (ifs >> value)
            {
                return value;
            }
            else
            {
                return none;
            }
        }

        [[nodiscard]]
        static Optional<std::string> ReadString(const char *path)
        {
            std::ifstream ifs{path};

            if (not ifs)
            {
                return none;
            }

            std::string value;

            if (ifs >> value)
            {
                return value;
            }
            else
            {
                return none;
            }
        }
    }

    namespace System
    {
        PowerStatus GetPowerStatus()
        {
            PowerStatus status;
            status.battery = BatteryStatus::NoBattery;
            bool hasBattery = false;

            if (auto value = detail::ReadInt("/sys/class/power_supply/AC/online"))
            {
                status.ac = (*value == 1) ? ACLineStatus::Online : ACLineStatus::Offline;
            }
            else if (auto value = detail::ReadInt("/sys/class/power_supply/ac/online"))
            {
                status.ac = (*value == 1) ? ACLineStatus::Online : ACLineStatus::Offline;
            }

            if (auto value = detail::ReadString("/sys/class/power_supply/battery/status"))
            {
                hasBattery = true;
                status.charging = (*value == "Charging");

                if (auto capacity = detail::ReadInt("/sys/class/power_supply/battery/capacity"))
                {
                    status.batteryLifePercent = capacity;
                }
            }
            else if (auto value = detail::ReadString("/sys/class/power_supply/BAT0/status"))
            {
                hasBattery = true;
                status.charging = (*value == "Charging");

                if (auto capacity = detail::ReadInt("/sys/class/power_supply/BAT0/capacity"))
                {
                    status.batteryLifePercent = capacity;
                }
            }

            if (hasBattery)
            {
                if (status.batteryLifePercent)
                {
                    const int32 percent = *status.batteryLifePercent;

                    status.battery = percent < +5    ? BatteryStatus::Critical
                                     : percent <= 33 ? BatteryStatus::Low
                                     : percent <= 66 ? BatteryStatus::Middle
                                                     : BatteryStatus::High;
                }
                else
                {
                    status.battery = BatteryStatus::Unknown;
                }
            }

            return status;
        }
    }
}
