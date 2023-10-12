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

#include <FramelessHelper/Quick/framelesshelperquick_global.h>
#include <optional>

QT_BEGIN_NAMESPACE
class QQuickItem;
QT_END_NAMESPACE

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(mica_material)
class QuickMicaMaterial;
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
class QuickWindowBorder;
#endif

class FramelessQuickHelper;
class FRAMELESSHELPER_QUICK_API FramelessQuickHelperPrivate : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_PRIVATE_QT_CLASS(FramelessQuickHelper)

public:
    explicit FramelessQuickHelperPrivate(FramelessQuickHelper *q);
    ~FramelessQuickHelperPrivate() override;

    void attach();
    void detach();

    void emitSignalForAllInstances(const char *signal);

    void setProperty(const char *name, const QVariant &value);
    Q_NODISCARD QVariant getProperty(const char *name, const QVariant &defaultValue = {});

#if FRAMELESSHELPER_CONFIG(mica_material)
    Q_NODISCARD QuickMicaMaterial *findOrCreateMicaMaterial() const;
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
    Q_NODISCARD QuickWindowBorder *findOrCreateWindowBorder() const;
#endif

    Q_NODISCARD static FramelessQuickHelper *findOrCreateFramelessHelper(QObject *object);

    void repaintAllChildren(const quint32 delay = 0) const;

    Q_NODISCARD quint32 readyWaitTime() const;
    void setReadyWaitTime(const quint32 time);

    Q_NODISCARD QRect mapItemGeometryToScene(const QQuickItem * const item) const;
    Q_NODISCARD bool isInSystemButtons(const QPoint &pos, QuickGlobal::SystemButtonType *button) const;
    Q_NODISCARD bool isInTitleBarDraggableArea(const QPoint &pos) const;
    Q_NODISCARD bool shouldIgnoreMouseEvents(const QPoint &pos) const;
    void setSystemButtonState(const QuickGlobal::SystemButtonType button, const QuickGlobal::ButtonState state);
    void rebindWindow();

    QColor savedWindowBackgroundColor = {};
    bool blurBehindWindowEnabled = false;
    std::optional<bool> extendIntoTitleBar = std::nullopt;
    bool qpaReady = false;
    quint32 qpaWaitTime = 0;
};

FRAMELESSHELPER_END_NAMESPACE
