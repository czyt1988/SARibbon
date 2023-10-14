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
#include <functional>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#  include <QtCore/qscopeguard.h>
#else // (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))

FRAMELESSHELPER_BEGIN_NAMESPACE

using ScopeGuardCallback = std::function<void()>;

class [[nodiscard]] ScopeGuard
{
    FRAMELESSHELPER_CLASS(ScopeGuard)

public:
    ScopeGuard()
    {
    }

    explicit ScopeGuard(const ScopeGuardCallback &cb)
    {
        // For the ctor, we require a non-null function.
        Q_ASSERT(cb);
        if (!cb) {
            return;
        }
        m_callback = cb;
    }

    explicit ScopeGuard(ScopeGuardCallback &&cb)
    {
        // For the ctor, we require a non-null function.
        Q_ASSERT(cb);
        if (!cb) {
            return;
        }
        m_callback = std::move(cb);
    }

    ScopeGuard(ScopeGuard &&other) : m_callback(std::move(other.m_callback))
    {
    }

    ~ScopeGuard()
    {
        if (m_callback) {
            m_callback();
        }
    }

    [[nodiscard]] ScopeGuardCallback callback() const
    {
        return m_callback;
    }

    void setCallback(const ScopeGuardCallback &cb)
    {
        // But we allow null functions here.
        m_callback = cb;
    }

private:
    ScopeGuardCallback m_callback = nullptr;
};

[[nodiscard]] inline ScopeGuard make_guard(ScopeGuardCallback &&cb)
{
    return ScopeGuard(cb);
}

#define qScopeGuard make_guard

FRAMELESSHELPER_END_NAMESPACE

#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
