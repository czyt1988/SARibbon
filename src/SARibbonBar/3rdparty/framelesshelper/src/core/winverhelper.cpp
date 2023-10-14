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

#include "winverhelper_p.h"

#ifdef Q_OS_WINDOWS

#include "utils.h"
#include <array>

FRAMELESSHELPER_BEGIN_NAMESPACE

using namespace Global;

class WinVerHelper
{
    FRAMELESSHELPER_CLASS(WinVerHelper)

public:
    explicit WinVerHelper();
    ~WinVerHelper();

    [[nodiscard]] bool check(const WindowsVersion version) const;

private:
    void initialize();

private:
    std::array<bool, static_cast<int>(WindowsVersion::Latest) + 1> m_flags = {};
};

WinVerHelper::WinVerHelper()
{
    initialize();
}

WinVerHelper::~WinVerHelper() = default;

bool WinVerHelper::check(const WindowsVersion version) const
{
    return m_flags.at(static_cast<int>(version));
}

void WinVerHelper::initialize()
{
    const auto fill = [this](const int no) -> void {
        static const auto size = int(std::size(m_flags));
        if ((no <= 0) || (no >= size)) {
            return;
        }
        for (int i = 0; i != size; ++i) {
            m_flags.at(i) = (i <= no);
        }
    };
#define ELIF(Version) \
    else if (Utils::isWindowsVersionOrGreater(WindowsVersion::_##Version)) { \
        fill(static_cast<int>(WindowsVersion::_##Version)); \
    }
    if (false) { /* Dummy */ }
    ELIF(11_22H2)
    ELIF(11_21H2)
    ELIF(10_22H2)
    ELIF(10_21H2)
    ELIF(10_21H1)
    ELIF(10_20H2)
    ELIF(10_2004)
    ELIF(10_1909)
    ELIF(10_1903)
    ELIF(10_1809)
    ELIF(10_1803)
    ELIF(10_1709)
    ELIF(10_1703)
    ELIF(10_1607)
    ELIF(10_1511)
    ELIF(10_1507)
    ELIF(8_1_Update1)
    ELIF(8_1)
    ELIF(8)
    ELIF(7_SP1)
    ELIF(7)
    ELIF(Vista_SP2)
    ELIF(Vista_SP1)
    ELIF(Vista)
    ELIF(XP_64)
    ELIF(XP)
    ELIF(2000)
    else { /* Dummy */ }
#undef ELIF
}

Q_GLOBAL_STATIC(WinVerHelper, g_winVerHelper)

#define IMPL(Name, Version) \
  bool WindowsVersionHelper::isWin##Name##OrGreater() \
  { \
      static const bool result = g_winVerHelper()->check(WindowsVersion::_##Version); \
      return result; \
  }

IMPL(2K, 2000)
IMPL(XP, XP)
IMPL(XP64, XP_64)
IMPL(Vista, Vista)
IMPL(VistaSP1, Vista_SP1)
IMPL(VistaSP2, Vista_SP2)
IMPL(7, 7)
IMPL(7SP1, 7_SP1)
IMPL(8, 8)
IMPL(8Point1, 8_1)
IMPL(8Point1Update1, 8_1_Update1)
IMPL(10, 10)
IMPL(10TH1, 10_1507)
IMPL(10TH2, 10_1511)
IMPL(10RS1, 10_1607)
IMPL(10RS2, 10_1703)
IMPL(10RS3, 10_1709)
IMPL(10RS4, 10_1803)
IMPL(10RS5, 10_1809)
IMPL(1019H1, 10_1903)
IMPL(1019H2, 10_1909)
IMPL(1020H1, 10_2004)
IMPL(1020H2, 10_20H2)
IMPL(1021H1, 10_21H1)
IMPL(1021H2, 10_21H2)
IMPL(1022H2, 10_22H2)
IMPL(11, 11)
IMPL(1121H2, 11_21H2)
IMPL(1122H2, 11_22H2)

#undef IMPL

FRAMELESSHELPER_END_NAMESPACE

#endif // Q_OS_WINDOWS
