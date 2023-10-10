/*
 * MIT License
 *
 * Copyright (C) 2021-2023 by wangwenx190 (Yuhang Zhao)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <FramelessHelper/Core/framelesshelpercore_global.h>
#include <array>

FRAMELESSHELPER_BEGIN_NAMESPACE

struct VersionNumber
{
    unsigned long Major = 0;
    unsigned long Minor = 0;
    unsigned long Patch = 0;
    unsigned long Tweak = 0;

    [[nodiscard]] friend constexpr bool operator==(const VersionNumber &lhs, const VersionNumber &rhs) noexcept
    {
        return ((lhs.Major == rhs.Major) && (lhs.Minor == rhs.Minor) && (lhs.Patch == rhs.Patch) && (lhs.Tweak == rhs.Tweak));
    }

    [[nodiscard]] friend constexpr bool operator!=(const VersionNumber &lhs, const VersionNumber &rhs) noexcept
    {
        return !operator==(lhs, rhs);
    }

    [[nodiscard]] friend constexpr bool operator>(const VersionNumber &lhs, const VersionNumber &rhs) noexcept
    {
        if (operator==(lhs, rhs)) {
            return false;
        }
        if (lhs.Major > rhs.Major) {
            return true;
        }
        if (lhs.Major < rhs.Major) {
            return false;
        }
        if (lhs.Minor > rhs.Minor) {
            return true;
        }
        if (lhs.Minor < rhs.Minor) {
            return false;
        }
        if (lhs.Patch > rhs.Patch) {
            return true;
        }
        if (lhs.Patch < rhs.Patch) {
            return false;
        }
        return (lhs.Tweak > rhs.Tweak);
    }

    [[nodiscard]] friend constexpr bool operator<(const VersionNumber &lhs, const VersionNumber &rhs) noexcept
    {
        return (operator!=(lhs, rhs) && !operator>(lhs, rhs));
    }

    [[nodiscard]] friend constexpr bool operator>=(const VersionNumber &lhs, const VersionNumber &rhs) noexcept
    {
        return (operator>(lhs, rhs) || operator==(lhs, rhs));
    }

    [[nodiscard]] friend constexpr bool operator<=(const VersionNumber &lhs, const VersionNumber &rhs) noexcept
    {
        return (operator<(lhs, rhs) || operator==(lhs, rhs));
    }
};

#ifdef Q_OS_WINDOWS
[[maybe_unused]] inline constexpr const std::array<VersionNumber, static_cast<int>(Global::WindowsVersion::Latest) + 1> WindowsVersions =
{
    VersionNumber{  5, 0,  2195 }, // Windows 2000
    VersionNumber{  5, 1,  2600 }, // Windows XP
    VersionNumber{  5, 2,  3790 }, // Windows XP x64 Edition or Windows Server 2003
    VersionNumber{  6, 0,  6000 }, // Windows Vista
    VersionNumber{  6, 0,  6001 }, // Windows Vista with Service Pack 1 or Windows Server 2008
    VersionNumber{  6, 0,  6002 }, // Windows Vista with Service Pack 2
    VersionNumber{  6, 1,  7600 }, // Windows 7 or Windows Server 2008 R2
    VersionNumber{  6, 1,  7601 }, // Windows 7 with Service Pack 1 or Windows Server 2008 R2 with Service Pack 1
    VersionNumber{  6, 2,  9200 }, // Windows 8 or Windows Server 2012
    VersionNumber{  6, 3,  9200 }, // Windows 8.1 or Windows Server 2012 R2
    VersionNumber{  6, 3,  9600 }, // Windows 8.1 with Update 1
    VersionNumber{ 10, 0, 10240 }, // Windows 10 Version 1507 (TH1)
    VersionNumber{ 10, 0, 10586 }, // Windows 10 Version 1511 (November Update) (TH2)
    VersionNumber{ 10, 0, 14393 }, // Windows 10 Version 1607 (Anniversary Update) (RS1) or Windows Server 2016
    VersionNumber{ 10, 0, 15063 }, // Windows 10 Version 1703 (Creators Update) (RS2)
    VersionNumber{ 10, 0, 16299 }, // Windows 10 Version 1709 (Fall Creators Update) (RS3)
    VersionNumber{ 10, 0, 17134 }, // Windows 10 Version 1803 (April 2018 Update) (RS4)
    VersionNumber{ 10, 0, 17763 }, // Windows 10 Version 1809 (October 2018 Update) (RS5) or Windows Server 2019
    VersionNumber{ 10, 0, 18362 }, // Windows 10 Version 1903 (May 2019 Update) (19H1)
    VersionNumber{ 10, 0, 18363 }, // Windows 10 Version 1909 (November 2019 Update) (19H2)
    VersionNumber{ 10, 0, 19041 }, // Windows 10 Version 2004 (May 2020 Update) (20H1)
    VersionNumber{ 10, 0, 19042 }, // Windows 10 Version 20H2 (October 2020 Update) (20H2)
    VersionNumber{ 10, 0, 19043 }, // Windows 10 Version 21H1 (May 2021 Update) (21H1)
    VersionNumber{ 10, 0, 19044 }, // Windows 10 Version 21H2 (November 2021 Update) (21H2)
    VersionNumber{ 10, 0, 19045 }, // Windows 10 Version 22H2 (October 2022 Update) (22H2)
    VersionNumber{ 10, 0, 22000 }, // Windows 11 Version 21H2 (21H2)
    VersionNumber{ 10, 0, 22621 }  // Windows 11 Version 22H2 (October 2022 Update) (22H2)
};
#endif // Q_OS_WINDOWS

FRAMELESSHELPER_END_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
QT_BEGIN_NAMESPACE
FRAMELESSHELPER_CORE_API QDebug operator<<(QDebug, const FRAMELESSHELPER_PREPEND_NAMESPACE(VersionNumber) &);
QT_END_NAMESPACE
#endif // QT_NO_DEBUG_STREAM
