#ifndef SARIBBONQT5COMPAT_HPP
#define SARIBBONQT5COMPAT_HPP
#include <QtCore/QtGlobal>
#include <QtCore/QObject>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QFontMetrics>
#include <QtGui/QFontMetricsF>

namespace SA
{

/**
 * @brief 处理Qt5与Qt6中的差异
 *
 */
namespace compat
{

/**
 * @brief 获取事件的位置（QPoint）
 * @tparam EventType 事件类型（需支持pos()或position()方法）
 * @param event 事件指针
 * @return 事件位置的QPoint表示
 */
template< typename EventType >
inline QPoint eventGlobalPos(EventType* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return event->globalPos();
#else
    return event->globalPosition().toPoint();
#endif
}

/**
 * @brief 获取事件的位置（QPoint）
 * @tparam EventType 事件类型（需支持pos()或position()方法）
 * @param event 事件指针
 * @return 事件位置的QPoint表示
 */
template< typename EventType >
inline QPoint eventPos(EventType* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return event->pos();
#else
    return event->position().toPoint();
#endif
}

/**
 * @brief 获取事件的x坐标
 * @tparam EventType 事件类型
 * @param event 事件指针
 * @return x坐标（整数）
 */
template< typename EventType >
inline int eventPosX(EventType* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return event->pos().x();
#else
    return static_cast< int >(event->position().x());
#endif
}

/**
 * @brief 获取事件的y坐标
 * @tparam EventType 事件类型
 * @param event 事件指针
 * @return y坐标（整数）
 */
template< typename EventType >
inline int eventPosY(EventType* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return event->pos().y();
#else
    return static_cast< int >(event->position().y());
#endif
}

/**
 * @brief 计算字符串的水平宽度（整数版本）
 * @param fm QFontMetrics对象
 * @param str 目标字符串
 * @return 宽度（整数）
 */
template< typename strType >
inline int horizontalAdvance(const QFontMetrics& fm, const strType& str)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    return fm.width(str);
#else
    return fm.horizontalAdvance(str);
#endif
}

/**
 * @brief 计算字符串的水平宽度（浮点数版本）
 * @param fm QFontMetricsF对象
 * @param str 目标字符串
 * @return 宽度（浮点数）
 */
template< typename strType >
inline qreal horizontalAdvanceF(const QFontMetricsF& fm, const strType& str)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    return fm.width(str);
#else
    return fm.horizontalAdvance(str);
#endif
}

/**
 * @brief Get vertical wheel delta value compatible with Qt5 and Qt6
 *
 * This function provides a unified interface to retrieve the vertical scroll delta
 * from a QWheelEvent, supporting both Qt5 (using delta()) and Qt6 (using angleDelta().y())
 * without changing the calling code.
 *
 * The return value represents the vertical scroll amount:
 * - Positive value: Wheel scrolled up
 * - Negative value: Wheel scrolled down
 * - The magnitude follows the standard wheel step (typically ±120 per notch)
 *
 * @param e Pointer to the QWheelEvent object (must not be nullptr)
 * @return Integer delta value of vertical wheel movement
 * @note The function only returns vertical wheel delta (ignores horizontal scroll via angleDelta().x())
 * @warning Ensure the input QWheelEvent pointer is valid to avoid null pointer dereference
 */
inline int wheelEventDelta(QWheelEvent* e)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return e->delta();
#else
    return e->angleDelta().y();
#endif
}
}  // namespace   compat
}  // namespace   qwt
#endif  // SARIBBONQT5COMPAT_HPP
