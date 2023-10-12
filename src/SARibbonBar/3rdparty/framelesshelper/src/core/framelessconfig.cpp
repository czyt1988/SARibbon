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

#include "framelessconfig_p.h"
#include <array>
#include <memory>
#include <QtCore/qdir.h>
#include <QtCore/qsettings.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qtimer.h>
#include <QtCore/qloggingcategory.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcFramelessConfig, "wangwenx190.framelesshelper.core.framelessconfig")
#  define INFO qCInfo(lcFramelessConfig)
#  define DEBUG qCDebug(lcFramelessConfig)
#  define WARNING qCWarning(lcFramelessConfig)
#  define CRITICAL qCCritical(lcFramelessConfig)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

struct FramelessConfigEntry
{
    const char *env = nullptr;
    const char *cfg = nullptr;
};

static constexpr const std::array<FramelessConfigEntry, static_cast<int>(Option::Last) + 1> FramelessOptionsTable =
{
    FramelessConfigEntry{ "FRAMELESSHELPER_USE_CROSS_PLATFORM_QT_IMPLEMENTATION", "Options/UseCrossPlatformQtImplementation" },
    FramelessConfigEntry{ "FRAMELESSHELPER_FORCE_HIDE_WINDOW_FRAME_BORDER", "Options/ForceHideWindowFrameBorder" },
    FramelessConfigEntry{ "FRAMELESSHELPER_FORCE_SHOW_WINDOW_FRAME_BORDER", "Options/ForceShowWindowFrameBorder" },
    FramelessConfigEntry{ "FRAMELESSHELPER_DISABLE_WINDOWS_SNAP_LAYOUT", "Options/DisableWindowsSnapLayout" },
    FramelessConfigEntry{ "FRAMELESSHELPER_WINDOW_USE_ROUND_CORNERS", "Options/WindowUseRoundCorners" },
    FramelessConfigEntry{ "FRAMELESSHELPER_CENTER_WINDOW_BEFORE_SHOW", "Options/CenterWindowBeforeShow" },
    FramelessConfigEntry{ "FRAMELESSHELPER_ENABLE_BLUR_BEHIND_WINDOW", "Options/EnableBlurBehindWindow" },
    FramelessConfigEntry{ "FRAMELESSHELPER_FORCE_NON_NATIVE_BACKGROUND_BLUR", "Options/ForceNonNativeBackgroundBlur" },
    FramelessConfigEntry{ "FRAMELESSHELPER_DISABLE_LAZY_INITIALIZATION_FOR_MICA_MATERIAL", "Options/DisableLazyInitializationForMicaMaterial" },
    FramelessConfigEntry{ "FRAMELESSHELPER_FORCE_NATIVE_BACKGROUND_BLUR", "Options/ForceNativeBackgroundBlur" },
    FramelessConfigEntry{ "FRAMELESSHELPER_WINDOW_USE_SQUARE_CORNERS", "Options/WindowUseSquareCorners" }
};

static constexpr const auto OptionCount = std::size(FramelessOptionsTable);

struct FramelessConfigData
{
    bool loaded = false;
    std::array<bool, OptionCount> options = {};
    bool disableEnvVar = false;
    bool disableCfgFile = false;
};

Q_GLOBAL_STATIC(FramelessConfigData, g_framelessConfigData)

#if FRAMELESSHELPER_CONFIG(debug_output)
static inline void warnInappropriateOptions()
{
    const FramelessConfig * const cfg = FramelessConfig::instance();
    if (cfg->isSet(Option::UseCrossPlatformQtImplementation)) {
        WARNING << "Option::UseCrossPlatformQtImplementation is deprecated and has no effect now. It will be removed in a future version.";
    }
#ifdef Q_OS_WINDOWS
    if (cfg->isSet(Option::DisableWindowsSnapLayout)) {
        WARNING << "Option::DisableWindowsSnapLayout is deprecated and has no effect now. It will be removed in a future version.";
    }
#else
    if (cfg->isSet(Option::ForceHideWindowFrameBorder)) {
        WARNING << "Option::ForceHideWindowFrameBorder is only available on Windows.";
    }
    if (cfg->isSet(Option::ForceShowWindowFrameBorder)) {
        WARNING << "Option::ForceShowWindowFrameBorder is only available on Windows.";
    }
    if (cfg->isSet(Option::DisableWindowsSnapLayout)) {
        WARNING << "Option::DisableWindowsSnapLayout is only available on Windows.";
    }
#endif // Q_OS_WINDOWS
    if (cfg->isSet(Option::ForceHideWindowFrameBorder) && cfg->isSet(Option::ForceShowWindowFrameBorder)) {
        WARNING << "Option::ForceHideWindowFrameBorder and Option::ForceShowWindowFrameBorder can't be both enabled.";
    }
    if (cfg->isSet(Option::ForceNonNativeBackgroundBlur) && cfg->isSet(Option::ForceNativeBackgroundBlur)) {
        WARNING << "Option::ForceNonNativeBackgroundBlur and Option::ForceNativeBackgroundBlur can't be both enabled.";
    }
    if (cfg->isSet(Option::WindowUseRoundCorners)) {
        WARNING << "Option::WindowUseRoundCorners has not been implemented yet.";
    }
}
#endif

FramelessConfig::FramelessConfig(QObject *parent) : QObject(parent)
{
    reload();
}

FramelessConfig::~FramelessConfig() = default;

FramelessConfig *FramelessConfig::instance()
{
    static FramelessConfig config;
    return &config;
}

void FramelessConfig::reload(const bool force)
{
    if (g_framelessConfigData()->loaded && !force) {
        return;
    }
    const auto configFile = []() -> std::unique_ptr<QSettings> {
        if (!qApp) {
            return nullptr;
        }
        const QDir appDir(QCoreApplication::applicationDirPath());
        return std::make_unique<QSettings>(appDir.filePath(FRAMELESSHELPER_STRING_LITERAL(".framelesshelper.ini")), QSettings::IniFormat);
    }();
    for (int i = 0; i != OptionCount; ++i) {
        const bool envVar = (!g_framelessConfigData()->disableEnvVar
            && qEnvironmentVariableIsSet(FramelessOptionsTable.at(i).env)
            && (qEnvironmentVariableIntValue(FramelessOptionsTable.at(i).env) > 0));
        const bool cfgFile = (!g_framelessConfigData()->disableCfgFile && configFile
            && configFile->value(QUtf8String(FramelessOptionsTable.at(i).cfg), false).toBool());
        g_framelessConfigData()->options.at(i) = (envVar || cfgFile);
    }
    g_framelessConfigData()->loaded = true;
#if FRAMELESSHELPER_CONFIG(debug_output)
    QTimer::singleShot(0, this, [](){ warnInappropriateOptions(); });
#endif
}

void FramelessConfig::set(const Option option, const bool on)
{
    g_framelessConfigData()->options.at(static_cast<int>(option)) = on;
}

bool FramelessConfig::isSet(const Option option) const
{
    return g_framelessConfigData()->options.at(static_cast<int>(option));
}

void FramelessConfig::setLoadFromEnvironmentVariablesDisabled(const bool on)
{
    g_framelessConfigData()->disableEnvVar = on;
}

void FramelessConfig::setLoadFromConfigurationFileDisabled(const bool on)
{
    g_framelessConfigData()->disableCfgFile = on;
}

FRAMELESSHELPER_END_NAMESPACE
