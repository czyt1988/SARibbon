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
#include <QtQml/qqml.h>
#if __has_include(<QtQml/qqmlregistration.h>)
#  include <QtQml/qqmlregistration.h>
#endif

#ifndef FRAMELESSHELPER_QUICK_API
#  if FRAMELESSHELPER_CONFIG(static_build)
#    define FRAMELESSHELPER_QUICK_API
#  else // FRAMELESSHELPER_QUICK_STATIC
#    ifdef FRAMELESSHELPER_QUICK_LIBRARY
#      define FRAMELESSHELPER_QUICK_API Q_DECL_EXPORT
#    else // FRAMELESSHELPER_QUICK_LIBRARY
#      define FRAMELESSHELPER_QUICK_API Q_DECL_IMPORT
#    endif // FRAMELESSHELPER_QUICK_LIBRARY
#  endif // FRAMELESSHELPER_QUICK_STATIC
#endif // FRAMELESSHELPER_QUICK_API

#ifndef FRAMELESSHELPER_QUICK_ENUM_VALUE
#  define FRAMELESSHELPER_QUICK_ENUM_VALUE(Enum, Value) \
     Value = static_cast<int>(FRAMELESSHELPER_PREPEND_NAMESPACE(Global)::Enum::Value),
#endif

#ifndef FRAMELESSHELPER_ENUM_CORE_TO_QUICK
#  define FRAMELESSHELPER_ENUM_CORE_TO_QUICK(Enum, Value) \
     static_cast<FRAMELESSHELPER_PREPEND_NAMESPACE(QuickGlobal)::Enum>(static_cast<int>(Value))
#endif

#ifndef FRAMELESSHELPER_ENUM_QUICK_TO_CORE
#  define FRAMELESSHELPER_ENUM_QUICK_TO_CORE(Enum, Value) \
     static_cast<FRAMELESSHELPER_PREPEND_NAMESPACE(Global)::Enum>(static_cast<int>(Value))
#endif

#ifndef FRAMELESSHELPER_FLAGS_CORE_TO_QUICK
#  define FRAMELESSHELPER_FLAGS_CORE_TO_QUICK(Enum, Value, In, Out) \
     if (In & FRAMELESSHELPER_PREPEND_NAMESPACE(Global)::Enum::Value) { \
         Out |= FRAMELESSHELPER_PREPEND_NAMESPACE(QuickGlobal)::Enum::Value; \
     }
#endif

#ifndef FRAMELESSHELPER_FLAGS_QUICK_TO_CORE
#  define FRAMELESSHELPER_FLAGS_QUICK_TO_CORE(Enum, Value, In, Out) \
     if (In & FRAMELESSHELPER_PREPEND_NAMESPACE(QuickGlobal)::Enum::Value) { \
         Out |= FRAMELESSHELPER_PREPEND_NAMESPACE(Global)::Enum::Value; \
     }
#endif

#ifndef FRAMELESSHELPER_IMPORT_QUICK_PLUGIN
#  define FRAMELESSHELPER_IMPORT_QUICK_PLUGIN Q_IMPORT_PLUGIN(org_wangwenx190_FramelessHelperPlugin)
#endif

FRAMELESSHELPER_BEGIN_NAMESPACE

[[maybe_unused]] inline constexpr const char FRAMELESSHELPER_QUICK_URI[] = "org.wangwenx190.FramelessHelper";
[[maybe_unused]] inline constexpr const int FRAMELESSHELPER_QUICK_VERSION_MAJOR = 1;
[[maybe_unused]] inline constexpr const int FRAMELESSHELPER_QUICK_VERSION_MINOR = 0;

FRAMELESSHELPER_QUICK_API void FramelessHelperQuickInitialize();
FRAMELESSHELPER_QUICK_API void FramelessHelperQuickUninitialize();
FRAMELESSHELPER_QUICK_API void FramelessHelperQuickRegisterTypes(QQmlEngine *);

class FRAMELESSHELPER_QUICK_API QuickGlobal : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_QT_CLASS(QuickGlobal)
#ifdef QML_NAMED_ELEMENT
    QML_NAMED_ELEMENT(FramelessHelperConstants)
#endif
#ifdef QML_UNCREATABLE
    QML_UNCREATABLE("The FramelessHelperConstants namespace is not creatable, you can only use it to access it's enums.")
#endif

public:
    explicit QuickGlobal(QObject *parent = nullptr);
    ~QuickGlobal() override;

    enum class SystemTheme : quint8
    {
        FRAMELESSHELPER_QUICK_ENUM_VALUE(SystemTheme, Unknown)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(SystemTheme, Light)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(SystemTheme, Dark)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(SystemTheme, HighContrast)
    };
    Q_ENUM(SystemTheme)

    enum class SystemButtonType : quint8
    {
        FRAMELESSHELPER_QUICK_ENUM_VALUE(SystemButtonType, Unknown)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(SystemButtonType, WindowIcon)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(SystemButtonType, Help)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(SystemButtonType, Minimize)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(SystemButtonType, Maximize)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(SystemButtonType, Restore)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(SystemButtonType, Close)
    };
    Q_ENUM(SystemButtonType)

    enum class ButtonState : quint8
    {
        FRAMELESSHELPER_QUICK_ENUM_VALUE(ButtonState, Normal)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(ButtonState, Hovered)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(ButtonState, Pressed)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(ButtonState, Released)
    };
    Q_ENUM(ButtonState)

    enum class BlurMode : quint8
    {
        FRAMELESSHELPER_QUICK_ENUM_VALUE(BlurMode, Disable)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(BlurMode, Default)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(BlurMode, Windows_Aero)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(BlurMode, Windows_Acrylic)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(BlurMode, Windows_Mica)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(BlurMode, Windows_MicaAlt)
    };
    Q_ENUM(BlurMode)

    enum class WindowEdge : quint8
    {
        FRAMELESSHELPER_QUICK_ENUM_VALUE(WindowEdge, Left)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(WindowEdge, Top)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(WindowEdge, Right)
        FRAMELESSHELPER_QUICK_ENUM_VALUE(WindowEdge, Bottom)
    };
    Q_ENUM(WindowEdge)
    Q_DECLARE_FLAGS(WindowEdges, WindowEdge)
    Q_FLAG(WindowEdges)
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QuickGlobal::WindowEdges)

namespace FramelessHelper::Quick
{
inline void initialize() { FramelessHelperQuickInitialize(); }
inline void uninitialize() { FramelessHelperQuickUninitialize(); }
} // namespace FramelessHelper::Quick

FRAMELESSHELPER_END_NAMESPACE
