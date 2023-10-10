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

#include "framelesshelperquick_global.h"
#include "framelessquickhelper.h"
#include "framelessquickutils.h"
#include "quickimageitem_p.h"
#if FRAMELESSHELPER_CONFIG(titlebar)
#  include "quickchromepalette.h"
#endif
#if FRAMELESSHELPER_CONFIG(mica_material)
#  include "quickmicamaterial.h"
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
#  include "quickwindowborder.h"
#endif
#if FRAMELESSHELPER_CONFIG(private_qt)
#  if FRAMELESSHELPER_CONFIG(window)
#    include "framelessquickwindow_p.h"
#  endif
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#    if FRAMELESSHELPER_CONFIG(window)
#      include "framelessquickapplicationwindow_p.h"
#    endif
#    if FRAMELESSHELPER_CONFIG(system_button)
#      include "quickstandardsystembutton_p.h"
#    endif
#    if FRAMELESSHELPER_CONFIG(titlebar)
#      include "quickstandardtitlebar_p.h"
#    endif
#  endif // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#endif
#include <QtCore/qloggingcategory.h>

#ifndef QUICK_URI_SHORT
#  define QUICK_URI_SHORT FRAMELESSHELPER_QUICK_URI, FRAMELESSHELPER_QUICK_VERSION_MAJOR
#endif

#ifndef QUICK_URI_FULL
#  define QUICK_URI_FULL QUICK_URI_SHORT, FRAMELESSHELPER_QUICK_VERSION_MINOR
#endif

#ifndef QUICK_URI_EXPAND
#  define QUICK_URI_EXPAND(name) QUICK_URI_FULL, name
#endif

#ifndef REG_META_TYPE
#  define REG_META_TYPE(Type) qRegisterMetaType<Type>(#Type)
#endif

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcQuickGlobal, "wangwenx190.framelesshelper.quick.global")
#  define INFO qCInfo(lcQuickGlobal)
#  define DEBUG qCDebug(lcQuickGlobal)
#  define WARNING qCWarning(lcQuickGlobal)
#  define CRITICAL qCCritical(lcQuickGlobal)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

QuickGlobal::QuickGlobal(QObject *parent) : QObject(parent)
{
}

QuickGlobal::~QuickGlobal() = default;

void FramelessHelperQuickInitialize()
{
    static bool inited = false;
    if (inited) {
        return;
    }
    inited = true;

    FramelessHelperCoreInitialize();

    // Registering meta types only causes troubles in Qt6.
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    REG_META_TYPE(QuickGlobal::SystemTheme);
    REG_META_TYPE(QuickGlobal::SystemButtonType);
    REG_META_TYPE(QuickGlobal::ButtonState);
    REG_META_TYPE(QuickGlobal::BlurMode);
    REG_META_TYPE(QuickGlobal::WindowEdge);
#endif
}

void FramelessHelperQuickUninitialize()
{
    static bool uninited = false;
    if (uninited) {
        return;
    }
    uninited = true;

    // ### TODO: The Quick module-specific uninitialization.

    FramelessHelperCoreUninitialize();
}

void FramelessHelperQuickRegisterTypes(QQmlEngine *engine)
{
    Q_ASSERT(engine);
    if (!engine) {
        return;
    }

    // In most cases we don't need to register the QML types for multiple times.
    static bool reg = false;
    if (reg) {
        return;
    }
    reg = true;

    // @uri org.wangwenx190.FramelessHelper
    qmlRegisterUncreatableType<QuickGlobal>(QUICK_URI_FULL, "FramelessHelperConstants",
        FRAMELESSHELPER_STRING_LITERAL("The FramelessHelperConstants namespace is not creatable, you can only use it to access it's enums."));

    qmlRegisterSingletonType<FramelessQuickUtils>(QUICK_URI_EXPAND("FramelessUtils"),
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine);
            Q_UNUSED(scriptEngine);
            return new FramelessQuickUtils;
        });

    qmlRegisterType<FramelessQuickHelper>(QUICK_URI_EXPAND("FramelessHelper"));
    qmlRegisterType<QuickImageItem>(QUICK_URI_EXPAND("ImageItem"));

#if FRAMELESSHELPER_CONFIG(titlebar)
    qmlRegisterAnonymousType<QuickChromePalette>(QUICK_URI_SHORT);
#endif
#if FRAMELESSHELPER_CONFIG(mica_material)
    qmlRegisterType<QuickMicaMaterial>(QUICK_URI_EXPAND("MicaMaterial"));
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
    qmlRegisterType<QuickWindowBorder>(QUICK_URI_EXPAND("WindowBorder"));
#endif

#if (FRAMELESSHELPER_CONFIG(private_qt) && FRAMELESSHELPER_CONFIG(window))
    qmlRegisterType<FramelessQuickWindow>(QUICK_URI_EXPAND("FramelessWindow"));
#else
    qmlRegisterTypeNotAvailable(QUICK_URI_EXPAND("FramelessWindow"),
        FRAMELESSHELPER_STRING_LITERAL("FramelessWindow is not available."));
#endif

#if ((QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && FRAMELESSHELPER_CONFIG(private_qt))
#  if FRAMELESSHELPER_CONFIG(window)
    qmlRegisterType<FramelessQuickApplicationWindow>(QUICK_URI_EXPAND("FramelessApplicationWindow"));
#  endif
#  if FRAMELESSHELPER_CONFIG(system_button)
    qmlRegisterType<QuickStandardSystemButton>(QUICK_URI_EXPAND("StandardSystemButton"));
#  endif
#  if FRAMELESSHELPER_CONFIG(titlebar)
    qmlRegisterType<QuickStandardTitleBar>(QUICK_URI_EXPAND("StandardTitleBar"));
#  endif
#else
#  if FRAMELESSHELPER_CONFIG(window)
    qmlRegisterTypeNotAvailable(QUICK_URI_EXPAND("FramelessApplicationWindow"),
        FRAMELESSHELPER_STRING_LITERAL("FramelessApplicationWindow is not available."));
#  endif
#  if FRAMELESSHELPER_CONFIG(system_button)
    qmlRegisterTypeNotAvailable(QUICK_URI_EXPAND("StandardSystemButton"),
        FRAMELESSHELPER_STRING_LITERAL("StandardSystemButton is not available."));
#  endif
#  if FRAMELESSHELPER_CONFIG(titlebar)
    qmlRegisterTypeNotAvailable(QUICK_URI_EXPAND("StandardTitleBar"),
        FRAMELESSHELPER_STRING_LITERAL("StandardTitleBar is not available."));
#  endif
#endif

    qmlRegisterModule(QUICK_URI_FULL);
}

FRAMELESSHELPER_END_NAMESPACE
