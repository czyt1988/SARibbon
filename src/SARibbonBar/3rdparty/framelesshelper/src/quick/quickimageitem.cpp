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

#include "quickimageitem_p.h"
#include <QtCore/qloggingcategory.h>
#include <QtGui/qpainter.h>
#include <QtGui/qimage.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qicon.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcQuickImageItem, "wangwenx190.framelesshelper.quick.quickimageitem")
#  define INFO qCInfo(lcQuickImageItem)
#  define DEBUG qCDebug(lcQuickImageItem)
#  define WARNING qCWarning(lcQuickImageItem)
#  define CRITICAL qCCritical(lcQuickImageItem)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

FRAMELESSHELPER_STRING_CONSTANT2(QrcPrefix, "qrc:")
FRAMELESSHELPER_STRING_CONSTANT2(FileSystemPrefix, ":")
FRAMELESSHELPER_STRING_CONSTANT2(UrlPrefix, ":///")
FRAMELESSHELPER_STRING_CONSTANT2(FilePathPrefix, ":/")

QuickImageItem::QuickImageItem(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    setAntialiasing(true);
    setSmooth(true);
    setMipmap(true);
    setClip(true);
}

QuickImageItem::~QuickImageItem() = default;

void QuickImageItem::paint(QPainter *painter)
{
    Q_ASSERT(painter);
    if (!painter) {
        return;
    }
    if (!m_source.isValid() || m_source.isNull()) {
        return;
    }
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing |
        QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    switch (m_source.userType()) {
    case QMetaType::QUrl:
        fromUrl(m_source.toUrl(), painter);
        break;
    case QMetaType::QString:
        fromString(m_source.toString(), painter);
        break;
    case QMetaType::QImage:
        fromImage(qvariant_cast<QImage>(m_source), painter);
        break;
    case QMetaType::QPixmap:
        fromPixmap(qvariant_cast<QPixmap>(m_source), painter);
        break;
    case QMetaType::QIcon:
        fromIcon(qvariant_cast<QIcon>(m_source), painter);
        break;
    default:
        WARNING << "Unsupported type:" << m_source.typeName();
        break;
    }
    painter->restore();
}

QVariant QuickImageItem::source() const
{
    return m_source;
}

void QuickImageItem::setSource(const QVariant &value)
{
    Q_ASSERT(value.isValid());
    Q_ASSERT(!value.isNull());
    if (!value.isValid() || value.isNull()) {
        return;
    }
    if (m_source == value) {
        return;
    }
    m_source = value;
    update();
    Q_EMIT sourceChanged();
}

void QuickImageItem::fromUrl(const QUrl &value, QPainter *painter) const
{
    Q_ASSERT(value.isValid());
    Q_ASSERT(painter);
    if (!value.isValid() || !painter) {
        return;
    }
    fromString((value.isLocalFile() ? value.toLocalFile() : value.toString()), painter);
}

void QuickImageItem::fromString(const QString &value, QPainter *painter) const
{
    Q_ASSERT(!value.isEmpty());
    Q_ASSERT(painter);
    if (value.isEmpty() || !painter) {
        return;
    }
    return fromPixmap(QPixmap([&value]() -> QString {
                          // For most Qt classes, the "qrc:///" prefix won't be recognized as a valid
                          // file system path, unless it accepts a QUrl object. For QString constructors
                          // we can only use ":/" to represent the file system path.
                          QString path = value;
                          if (path.startsWith(kQrcPrefix, Qt::CaseInsensitive)) {
                              path.replace(kQrcPrefix, kFileSystemPrefix, Qt::CaseInsensitive);
                          }
                          if (path.startsWith(kUrlPrefix, Qt::CaseInsensitive)) {
                              path.replace(kUrlPrefix, kFilePathPrefix, Qt::CaseInsensitive);
                          }
                          return path;
                      }()), painter);
}

void QuickImageItem::fromImage(const QImage &value, QPainter *painter) const
{
    Q_ASSERT(!value.isNull());
    Q_ASSERT(painter);
    if (value.isNull() || !painter) {
        return;
    }
    fromPixmap(QPixmap::fromImage(value), painter);
}

void QuickImageItem::fromPixmap(const QPixmap &value, QPainter *painter) const
{
    Q_ASSERT(!value.isNull());
    Q_ASSERT(painter);
    if (value.isNull() || !painter) {
        return;
    }
    const QRectF paintRect = paintArea();
    const QSize paintSize = paintRect.size().toSize();
    painter->drawPixmap(paintRect.topLeft(), (value.size() == paintSize ? value : value.scaled(paintSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
}

void QuickImageItem::fromIcon(const QIcon &value, QPainter *painter) const
{
    Q_ASSERT(!value.isNull());
    Q_ASSERT(painter);
    if (value.isNull() || !painter) {
        return;
    }
    fromPixmap(value.pixmap(paintArea().size().toSize()), painter);
}

QRectF QuickImageItem::paintArea() const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const QSizeF s = size();
#else
    const QSizeF s = {width(), height()};
#endif
    return {QPointF(0, 0), s};
}

void QuickImageItem::classBegin()
{
    QQuickPaintedItem::classBegin();
}

void QuickImageItem::componentComplete()
{
    QQuickPaintedItem::componentComplete();
}

FRAMELESSHELPER_END_NAMESPACE
