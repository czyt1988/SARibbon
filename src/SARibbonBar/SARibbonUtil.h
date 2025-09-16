#ifndef SARIBBONUTIL_H
#define SARIBBONUTIL_H
#include "SARibbonGlobal.h"
#include <QColor>
#include <QSize>
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
}
#endif  // SARIBBONUTIL_H
