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

/**
 * \if ENGLISH
 * @brief Check if the application layout direction is Right-to-Left (RTL)
 * @return true if layout direction is Qt::RightToLeft, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 检查应用程序布局方向是否为从右到左（RTL）
 * @return 如果布局方向为 Qt::RightToLeft 返回 true，否则返回 false
 * \endif
 */
bool SA_RIBBON_EXPORT saIsRTL();

/**
 * \if ENGLISH
 * @brief Mirror X coordinate for RTL layout support
 * @param x The original X coordinate
 * @param containerWidth The width of the container
 * @param elementWidth The width of the element
 * @return containerWidth - x - elementWidth when RTL, x unchanged when LTR
 * \endif
 *
 * \if CHINESE
 * @brief 为 RTL 布局支持镜像 X 坐标
 * @param x 原始 X 坐标
 * @param containerWidth 容器宽度
 * @param elementWidth 元素宽度
 * @return RTL 时返回 containerWidth - x - elementWidth，LTR 时返回 x 不变
 * \endif
 */
int SA_RIBBON_EXPORT saMirrorX(int x, int containerWidth, int elementWidth);
}
#endif  // SARIBBONUTIL_H
