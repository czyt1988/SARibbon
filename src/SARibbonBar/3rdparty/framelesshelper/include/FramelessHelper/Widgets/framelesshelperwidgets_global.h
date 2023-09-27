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

#ifndef FRAMELESSHELPER_WIDGETS_API
#  if FRAMELESSHELPER_CONFIG(static_build)
#    define FRAMELESSHELPER_WIDGETS_API
#  else
#    ifdef FRAMELESSHELPER_WIDGETS_LIBRARY
#      define FRAMELESSHELPER_WIDGETS_API Q_DECL_EXPORT
#    else
#      define FRAMELESSHELPER_WIDGETS_API Q_DECL_IMPORT
#    endif
#  endif
#endif

FRAMELESSHELPER_BEGIN_NAMESPACE

FRAMELESSHELPER_WIDGETS_API void FramelessHelperWidgetsInitialize();
FRAMELESSHELPER_WIDGETS_API void FramelessHelperWidgetsUninitialize();

namespace FramelessHelper::Widgets
{
inline void initialize() { FramelessHelperWidgetsInitialize(); }
inline void uninitialize() { FramelessHelperWidgetsUninitialize(); }
} // namespace FramelessHelper::Widgets

FRAMELESSHELPER_END_NAMESPACE
