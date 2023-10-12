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
#include <QtCore/qvariant.h>
#include <optional>

#ifdef Q_OS_WINDOWS

#ifndef REGISTRYKEY_FORCE_QSETTINGS
#  define REGISTRYKEY_FORCE_QSETTINGS (0)
#endif // REGISTRYKEY_FORCE_QSETTINGS

#ifndef REGISTRYKEY_IMPL
#  if ((QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)) && !(REGISTRYKEY_FORCE_QSETTINGS) && !defined(FRAMELESSHELPER_CORE_NO_PRIVATE))
#    define REGISTRYKEY_IMPL (1)
#  else // ((QT_VERSION < QT_VERSION_CHECK(5, 14, 0)) || REGISTRYKEY_FORCE_QSETTINGS || defined(FRAMELESSHELPER_CORE_NO_PRIVATE))
#    define REGISTRYKEY_IMPL (2)
#  endif // ((QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)) && !REGISTRYKEY_FORCE_QSETTINGS && !defined(FRAMELESSHELPER_CORE_NO_PRIVATE))
#endif // REGISTRYKEY_IMPL

#ifndef REGISTRYKEY_QWINREGISTRYKEY
#  define REGISTRYKEY_QWINREGISTRYKEY ((REGISTRYKEY_IMPL) == 1)
#endif // REGISTRYKEY_QWINREGISTRYKEY

#ifndef REGISTRYKEY_QSETTINGS
#  define REGISTRYKEY_QSETTINGS ((REGISTRYKEY_IMPL) == 2)
#endif // REGISTRYKEY_QSETTINGS

QT_BEGIN_NAMESPACE
class QWinRegistryKey;
class QSettings;
QT_END_NAMESPACE

FRAMELESSHELPER_BEGIN_NAMESPACE

class FRAMELESSHELPER_CORE_API RegistryKey : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_QT_CLASS(RegistryKey)

public:
    explicit RegistryKey(const Global::RegistryRootKey root, const QString &key, QObject *parent = nullptr);
    ~RegistryKey() override;

    Q_NODISCARD Global::RegistryRootKey rootKey() const;
    Q_NODISCARD QString subKey() const;

    Q_NODISCARD bool isValid() const;

    Q_NODISCARD QVariant value(const QString &name) const;
    template<typename T>
    Q_NODISCARD std::optional<T> value(const QString &name) const
    {
        const QVariant var = value(name);
        if (var.isValid() && !var.isNull()) {
            return qvariant_cast<T>(var);
        }
        return std::nullopt;
    }

private:
    Global::RegistryRootKey m_rootKey = Global::RegistryRootKey::CurrentUser;
    QString m_subKey = {};
#if REGISTRYKEY_QWINREGISTRYKEY
    QScopedPointer<QWinRegistryKey> m_registryKey;
#else
    QScopedPointer<QSettings> m_settings;
#endif
};

FRAMELESSHELPER_END_NAMESPACE

#endif // Q_OS_WINDOWS
