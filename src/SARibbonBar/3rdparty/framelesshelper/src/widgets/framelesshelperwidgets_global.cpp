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

#include "framelesshelperwidgets_global.h"
#include <QtCore/qloggingcategory.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcWidgetsGlobal, "wangwenx190.framelesshelper.widgets.global")
#  define INFO qCInfo(lcWidgetsGlobal)
#  define DEBUG qCDebug(lcWidgetsGlobal)
#  define WARNING qCWarning(lcWidgetsGlobal)
#  define CRITICAL qCCritical(lcWidgetsGlobal)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

void FramelessHelperWidgetsInitialize()
{
    static bool inited = false;
    if (inited) {
        return;
    }
    inited = true;

    FramelessHelperCoreInitialize();
}

void FramelessHelperWidgetsUninitialize()
{
    static bool uninited = false;
    if (uninited) {
        return;
    }
    uninited = true;

    // ### TODO: The Widgets module-specific uninitialization.

    FramelessHelperCoreUninitialize();
}

FRAMELESSHELPER_END_NAMESPACE
