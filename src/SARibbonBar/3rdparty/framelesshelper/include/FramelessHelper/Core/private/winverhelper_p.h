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

#ifdef Q_OS_WINDOWS

FRAMELESSHELPER_BEGIN_NAMESPACE

namespace WindowsVersionHelper
{

#define DECL(Name) [[nodiscard]] FRAMELESSHELPER_CORE_API bool isWin##Name##OrGreater();

DECL(2K)
DECL(XP)
DECL(XP64)
DECL(Vista)
DECL(VistaSP1)
DECL(VistaSP2)
DECL(7)
DECL(7SP1)
DECL(8)
DECL(8Point1)
DECL(8Point1Update1)
DECL(10)
DECL(10TH1)
DECL(10TH2)
DECL(10RS1)
DECL(10RS2)
DECL(10RS3)
DECL(10RS4)
DECL(10RS5)
DECL(1019H1)
DECL(1019H2)
DECL(1020H1)
DECL(1020H2)
DECL(1021H1)
DECL(1021H2)
DECL(1022H2)
DECL(11)
DECL(1121H2)
DECL(1122H2)

#undef DECL

} // namespace WindowsVersionHelper

FRAMELESSHELPER_END_NAMESPACE

#endif // Q_OS_WINDOWS
