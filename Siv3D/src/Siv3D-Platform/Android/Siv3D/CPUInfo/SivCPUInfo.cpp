//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//	Copyright (c) 2025      kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

#include <Siv3D/Common.hpp>
#include <Siv3D/String.hpp>
#include <Siv3D/Unicode.hpp>

// AGDK-compatible CPU detection using Android standard APIs
#include <sys/auxv.h>
#include <asm/hwcap.h>
#include <fstream>
#include <string>
#include <algorithm>

namespace s3d
{
    struct CPUInfo
    {
        struct Features
        {
            bool mmx = false;
            bool sse = false;
            bool sse2 = false;
            bool sse3 = false;
            bool sse4_1 = false;
            bool sse4_2 = false;

            bool neon = false;
            bool vfpv3 = false;
            bool vfpv4 = false;
            bool aes = false;
            bool sha = false;
        };

        Features features;

        uint32 family = 0;
        uint32 model = 0;
        uint32 stepping = 0;

        String vendor;
        String brand;
    };

    namespace detail
    {
        // CPU architecture detection using compiler macros
        enum class CPUArchitecture
        {
            Unknown,
            ARM,
            ARM64,
            X86,
            X86_64
        };

        [[nodiscard]]
        static CPUArchitecture DetectCPUArchitecture() noexcept
        {
#if defined(__aarch64__)
            return CPUArchitecture::ARM64;
#elif defined(__arm__)
            return CPUArchitecture::ARM;
#elif defined(__x86_64__)
            return CPUArchitecture::X86_64;
#elif defined(__i386__)
            return CPUArchitecture::X86;
#else
            return CPUArchitecture::Unknown;
#endif
        }

        [[nodiscard]]
        static std::string ReadCPUInfoFromProc() noexcept
        {
            try
            {
                std::ifstream file("/proc/cpuinfo");
                if (!file.is_open())
                {
                    return "";
                }

                std::string content;
                std::string line;
                while (std::getline(file, line))
                {
                    content += line + "\n";
                }
                file.close();

                return content;
            }
            catch (...)
            {
                return "";
            }
        }

        // キー値ペアの抽出
        static std::string ExtractValueFromCpuInfo(const std::string& cpuInfo, const std::string& key)
        {
            size_t pos = cpuInfo.find(key);
            if (pos == std::string::npos)
            {
                return "";
            }

            size_t colonPos = cpuInfo.find(":", pos);
            if (colonPos == std::string::npos)
            {
                return "";
            }

            size_t newlinePos = cpuInfo.find("\n", colonPos);
            if (newlinePos == std::string::npos)
            {
                newlinePos = cpuInfo.length();
            }

            std::string value = cpuInfo.substr(colonPos + 1, newlinePos - colonPos - 1);
            
            // Trim whitespace
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            return value;
        }

        // 大文字小文字を区別しない文字列検索
        static bool StringContains(const std::string& str, const std::string& substr)
        {
            std::string lowerStr = str;
            std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
            
            std::string lowerSubstr = substr;
            std::transform(lowerSubstr.begin(), lowerSubstr.end(), lowerSubstr.begin(), ::tolower);
            
            return lowerStr.find(lowerSubstr) != std::string::npos;
        }

        [[nodiscard]]
        static CPUInfo InitCPUInfo() noexcept
        {
            CPUInfo result;

            CPUArchitecture arch = DetectCPUArchitecture();
            std::string cpuInfo = ReadCPUInfoFromProc();

            // Set default vendor and brand based on architecture
            switch (arch)
            {
            case CPUArchitecture::ARM:
                result.vendor = U"ARM";
                result.brand = U"ARM Processor";
                break;
            case CPUArchitecture::ARM64:
                result.vendor = U"ARM64";
                result.brand = U"ARM64 Processor";
                break;
            case CPUArchitecture::X86:
                result.vendor = U"x86";
                result.brand = U"x86 Processor";
                break;
            case CPUArchitecture::X86_64:
                result.vendor = U"x86_64";
                result.brand = U"x86_64 Processor";
                break;
            default:
                result.vendor = U"Unknown";
                result.brand = U"Unknown Processor";
                break;
            }

            // Enhanced CPU info parsing from /proc/cpuinfo
            if (!cpuInfo.empty())
            {
                // Extract processor name
                std::string modelName = ExtractValueFromCpuInfo(cpuInfo, "model name");
                if (modelName.empty())
                {
                    modelName = ExtractValueFromCpuInfo(cpuInfo, "Processor");
                }
                if (modelName.empty())
                {
                    modelName = ExtractValueFromCpuInfo(cpuInfo, "Hardware");
                }
                
                if (!modelName.empty())
                {
                    result.brand = Unicode::FromUTF8(modelName);
                }

                // Extract vendor info
                std::string vendorId = ExtractValueFromCpuInfo(cpuInfo, "vendor_id");
                if (vendorId.empty())
                {
                    vendorId = ExtractValueFromCpuInfo(cpuInfo, "CPU implementer");
                }
                
                if (!vendorId.empty())
                {
                    result.vendor = Unicode::FromUTF8(vendorId);
                }
            }

            // Feature detection based on architecture
            if (arch == CPUArchitecture::ARM || arch == CPUArchitecture::ARM64)
            {
                // ARM/ARM64 feature detection using getauxval
                unsigned long hwcaps = getauxval(AT_HWCAP);

                // NEON detection
#ifdef HWCAP_NEON
                result.features.neon = (hwcaps & HWCAP_NEON) != 0;
#elif defined(HWCAP_ASIMD)
                result.features.neon = (hwcaps & HWCAP_ASIMD) != 0;
#endif

                // VFP detection
#ifdef HWCAP_VFPv3
                result.features.vfpv3 = (hwcaps & HWCAP_VFPv3) != 0;
#endif

#ifdef HWCAP_VFPv4
                result.features.vfpv4 = (hwcaps & HWCAP_VFPv4) != 0;
#endif

                // Crypto features
#ifdef HWCAP_AES
                result.features.aes = (hwcaps & HWCAP_AES) != 0;
#endif

#ifdef HWCAP_SHA1
                result.features.sha = (hwcaps & HWCAP_SHA1) != 0;
#endif

#ifdef HWCAP_SHA2
                result.features.sha = result.features.sha || ((hwcaps & HWCAP_SHA2) != 0);
#endif

                // Fallback: Parse /proc/cpuinfo for additional ARM features
                if (!cpuInfo.empty())
                {
                    if (StringContains(cpuInfo, "neon"))
                    {
                        result.features.neon = true;
                    }
                    if (StringContains(cpuInfo, "vfpv3"))
                    {
                        result.features.vfpv3 = true;
                    }
                    if (StringContains(cpuInfo, "vfpv4"))
                    {
                        result.features.vfpv4 = true;
                    }
                    if (StringContains(cpuInfo, "aes"))
                    {
                        result.features.aes = true;
                    }
                    if (StringContains(cpuInfo, "sha1") || StringContains(cpuInfo, "sha2"))
                    {
                        result.features.sha = true;
                    }
                }

                // Clear x86 features for ARM
                result.features.mmx = false;
                result.features.sse = false;
                result.features.sse2 = false;
                result.features.sse3 = false;
                result.features.sse4_1 = false;
                result.features.sse4_2 = false;
            }
            else if (arch == CPUArchitecture::X86 || arch == CPUArchitecture::X86_64)
            {
                // x86/x86_64 feature detection from /proc/cpuinfo
                if (!cpuInfo.empty())
                {
                    result.features.mmx = StringContains(cpuInfo, "mmx");
                    result.features.sse = StringContains(cpuInfo, "sse");
                    result.features.sse2 = StringContains(cpuInfo, "sse2");
                    result.features.sse3 = StringContains(cpuInfo, "sse3") || StringContains(cpuInfo, "pni");
                    result.features.sse4_1 = StringContains(cpuInfo, "sse4_1");
                    result.features.sse4_2 = StringContains(cpuInfo, "sse4_2");
                }
                else
                {
                    // Safe defaults for modern x86 processors
                    result.features.mmx = true;
                    result.features.sse = true;
                    result.features.sse2 = true;
                    result.features.sse3 = true;
                    result.features.sse4_1 = false;  // Conservative
                    result.features.sse4_2 = false;  // Conservative
                }

                // Clear ARM features for x86
                result.features.neon = false;
                result.features.vfpv3 = false;
                result.features.vfpv4 = false;
                result.features.aes = false;
                result.features.sha = false;
            }
            else
            {
                // Unknown architecture - set all features to false
                result.features = {};
            }

            return result;
        }
    }

    const CPUInfo g_CPUInfo = detail::InitCPUInfo();

    const CPUInfo &GetCPUInfo() noexcept
    {
        return g_CPUInfo;
    }
}