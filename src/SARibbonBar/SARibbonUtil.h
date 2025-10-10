#ifndef SARIBBONUTIL_H
#define SARIBBONUTIL_H
#include "SARibbonGlobal.h"
#include <QColor>
#include <QSize>
#include <QIcon>
namespace SA
{

// 让颜色鲜艳
QColor SA_RIBBON_EXPORT makeColorVibrant(const QColor& c, int saturationDelta = 150, int valueDelta = 30);

// 按照指定的新高度，保持宽高比缩放 QSize
QSize SA_RIBBON_EXPORT scaleSizeByHeight(const QSize& originalSize, int newHeight);

// 按照指定的新高度，宽高比为1:factor缩放 QSize。
QSize SA_RIBBON_EXPORT scaleSizeByHeight(const QSize& originalSize, int newHeight, qreal factor);

// 按照指定的新宽度，保持宽高比缩放 QSize
QSize SA_RIBBON_EXPORT scaleSizeByWidth(const QSize& originalSize, int newWidth);

// 获取内置的ribbon主题对应的qss内容
QString SA_RIBBON_EXPORT getBuiltInRibbonThemeQss(SARibbonTheme theme);

// 给窗口设置内置的ribbon主题
void SA_RIBBON_EXPORT setBuiltInRibbonTheme(QWidget* w, SARibbonTheme theme);

// 提供类似QIcon::pixmap(const QSize &size, qreal devicePixelRatio, Mode mode, State state) const（Qt6新增）的兼容函数
QPixmap iconToPixmap(const QIcon& icon,
                     const QSize& size,
                     qreal devicePixelRatio,
                     QIcon::Mode mode   = QIcon::Normal,
                     QIcon::State state = QIcon::Off);

// 获取窗口当前所在屏幕的dpr
qreal widgetDevicePixelRatio(QWidget* w);
}
#endif  // SARIBBONUTIL_H
