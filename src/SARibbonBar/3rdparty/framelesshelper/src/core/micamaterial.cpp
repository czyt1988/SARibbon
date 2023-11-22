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

#include "micamaterial.h"
#include "micamaterial_p.h"

#if FRAMELESSHELPER_CONFIG(mica_material)

#include "framelessmanager.h"
#include "utils.h"
#include "framelessconfig_p.h"
#include "framelesshelpercore_global_p.h"
#include <optional>
#include <memory>
#include <QtCore/qsysinfo.h>
#include <QtCore/qloggingcategory.h>
#if FRAMELESSHELPER_HAS_THREAD
#  include <QtCore/qmutex.h>
#endif
#include <QtGui/qpixmap.h>
#include <QtGui/qimage.h>
#include <QtGui/qimagereader.h>
#include <QtGui/qpainter.h>
#include <QtGui/qscreen.h>
#include <QtGui/qguiapplication.h>
#if FRAMELESSHELPER_CONFIG(private_qt)
#  include <QtGui/private/qmemrotate_p.h>
#endif

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcMicaMaterial, "wangwenx190.framelesshelper.core.micamaterial")
#  define INFO qCInfo(lcMicaMaterial)
#  define DEBUG qCDebug(lcMicaMaterial)
#  define WARNING qCWarning(lcMicaMaterial)
#  define CRITICAL qCCritical(lcMicaMaterial)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

[[maybe_unused]] static constexpr const QSize kMaximumPictureSize = { 1920, 1080 };
[[maybe_unused]] static constexpr const QImage::Format kDefaultImageFormat = QImage::Format_ARGB32_Premultiplied;

[[maybe_unused]] static constexpr const qreal kDefaultTintOpacity = 0.7;
[[maybe_unused]] static constexpr const qreal kDefaultNoiseOpacity = 0.04;
[[maybe_unused]] static constexpr const qreal kDefaultBlurRadius = 128.0;

[[maybe_unused]] static Q_COLOR_CONSTEXPR const QColor kDefaultSystemLightColor2 = {243, 243, 243}; // #F3F3F3

[[maybe_unused]] static Q_COLOR_CONSTEXPR const QColor kDefaultFallbackColorDark = {44, 44, 44}; // #2C2C2C
[[maybe_unused]] static Q_COLOR_CONSTEXPR const QColor kDefaultFallbackColorLight = {249, 249, 249}; // #F9F9F9

struct ImageData
{
    QPixmap blurredWallpaper = {};
    bool graphicsResourcesReady = false;
#if FRAMELESSHELPER_HAS_THREAD
    QMutex mutex{};
#endif
};

Q_GLOBAL_STATIC(ImageData, g_imageData)

#if FRAMELESSHELPER_CONFIG(private_qt)
template<const int shift>
[[nodiscard]] static inline constexpr int qt_static_shift(const int value)
{
    if constexpr (shift == 0) {
        return value;
    } else if constexpr (shift > 0) {
        return (value << (quint32(shift) & 0x1f));
    } else {
        return (value >> (quint32(-shift) & 0x1f));
    }
}

template<const int aprec, const int zprec>
static inline void qt_blurinner(uchar *bptr, int &zR, int &zG, int &zB, int &zA, const int alpha)
{
    const auto pixel = reinterpret_cast<QRgb *>(bptr);

#define Z_MASK (0xff << zprec)
    const int A_zprec = (qt_static_shift<zprec - 24>(*pixel) & Z_MASK);
    const int R_zprec = (qt_static_shift<zprec - 16>(*pixel) & Z_MASK);
    const int G_zprec = (qt_static_shift<zprec - 8>(*pixel)  & Z_MASK);
    const int B_zprec = (qt_static_shift<zprec>(*pixel)      & Z_MASK);
#undef Z_MASK

    const int zR_zprec = (zR >> aprec);
    const int zG_zprec = (zG >> aprec);
    const int zB_zprec = (zB >> aprec);
    const int zA_zprec = (zA >> aprec);

    zR += (alpha * (R_zprec - zR_zprec));
    zG += (alpha * (G_zprec - zG_zprec));
    zB += (alpha * (B_zprec - zB_zprec));
    zA += (alpha * (A_zprec - zA_zprec));

#define ZA_MASK (0xff << (zprec + aprec))
    *pixel = (qt_static_shift<24 - zprec - aprec>(zA & ZA_MASK)
        | qt_static_shift<16 - zprec - aprec>(zR & ZA_MASK)
        | qt_static_shift<8 - zprec - aprec>(zG & ZA_MASK)
        | qt_static_shift<-zprec - aprec>(zB & ZA_MASK));
#undef ZA_MASK
}

static constexpr const int alphaIndex = ((QSysInfo::ByteOrder == QSysInfo::BigEndian) ? 0 : 3);

template<const int aprec, const int zprec>
static inline void qt_blurinner_alphaOnly(uchar *bptr, int &z, const int alpha)
{
    const int A_zprec = (int(*(bptr)) << zprec);
    const int z_zprec = (z >> aprec);
    z += (alpha * (A_zprec - z_zprec));
    *(bptr) = (z >> (zprec + aprec));
}

template<const int aprec, const int zprec, const bool alphaOnly>
static inline void qt_blurrow(QImage &im, const int line, const int alpha)
{
    uchar *bptr = im.scanLine(line);

    int zR = 0, zG = 0, zB = 0, zA = 0;

    QT_WARNING_PUSH
    QT_WARNING_DISABLE_MSVC(4127) // false alarm.
    if (alphaOnly && (im.format() != QImage::Format_Indexed8)) {
        bptr += alphaIndex;
    }
    QT_WARNING_POP

    const int stride = (im.depth() >> 3);
    const int im_width = im.width();
    for (int index = 0; index != im_width; ++index) {
        if (alphaOnly) {
            qt_blurinner_alphaOnly<aprec, zprec>(bptr, zA, alpha);
        } else {
            qt_blurinner<aprec, zprec>(bptr, zR, zG, zB, zA, alpha);
        }
        bptr += stride;
    }

    bptr -= stride;

    for (int index = (im_width - 2); index >= 0; --index) {
        bptr -= stride;
        if (alphaOnly) {
            qt_blurinner_alphaOnly<aprec, zprec>(bptr, zA, alpha);
        } else {
            qt_blurinner<aprec, zprec>(bptr, zR, zG, zB, zA, alpha);
        }
    }
}

/*
*  expblur(QImage &img, int radius)
*
*  Based on exponential blur algorithm by Jani Huhtanen
*
*  In-place blur of image 'img' with kernel
*  of approximate radius 'radius'.
*
*  Blurs with two sided exponential impulse
*  response.
*
*  aprec = precision of alpha parameter
*  in fixed-point format 0.aprec
*
*  zprec = precision of state parameters
*  zR,zG,zB and zA in fp format 8.zprec
*/
template<const int aprec, const int zprec, const bool alphaOnly>
static inline void expblur(QImage &img, qreal radius, const bool improvedQuality = false, const int transposed = 0)
{
    Q_ASSERT((img.format() == kDefaultImageFormat)
             || (img.format() == QImage::Format_RGB32)
             || (img.format() == QImage::Format_Indexed8)
             || (img.format() == QImage::Format_Grayscale8));
    if ((img.format() != kDefaultImageFormat)
        && (img.format() != QImage::Format_RGB32)
        && (img.format() != QImage::Format_Indexed8)
        && (img.format() != QImage::Format_Grayscale8)) {
        return;
    }

    // halve the radius if we're using two passes
    if (improvedQuality) {
        radius *= 0.5;
    }

    // choose the alpha such that pixels at radius distance from a fully
    // saturated pixel will have an alpha component of no greater than
    // the cutOffIntensity
    static constexpr const qreal cutOffIntensity = 2.0;
    const int alpha = ((radius <= qreal(1e-5)) ? ((1 << aprec) - 1) :
        std::round((1 << aprec) * (1 - qPow(cutOffIntensity / qreal(255), qreal(1) / radius))));

    int img_height = img.height();
    for (int row = 0; row != img_height; ++row) {
        for (int i = 0; i <= int(improvedQuality); ++i) {
            qt_blurrow<aprec, zprec, alphaOnly>(img, row, alpha);
        }
    }

    QImage temp(img.height(), img.width(), img.format());
    temp.setDevicePixelRatio(img.devicePixelRatio());

    if (transposed >= 0) {
        if (img.depth() == 8) {
            qt_memrotate270(reinterpret_cast<const quint8 *>(img.bits()),
                            img.width(), img.height(), img.bytesPerLine(),
                            reinterpret_cast<quint8 *>(temp.bits()),
                            temp.bytesPerLine());
        } else {
            qt_memrotate270(reinterpret_cast<const quint32 *>(img.bits()),
                            img.width(), img.height(), img.bytesPerLine(),
                            reinterpret_cast<quint32 *>(temp.bits()),
                            temp.bytesPerLine());
        }
    } else {
        if (img.depth() == 8) {
            qt_memrotate90(reinterpret_cast<const quint8 *>(img.bits()),
                           img.width(), img.height(), img.bytesPerLine(),
                           reinterpret_cast<quint8 *>(temp.bits()),
                           temp.bytesPerLine());
        } else {
            qt_memrotate90(reinterpret_cast<const quint32 *>(img.bits()),
                           img.width(), img.height(), img.bytesPerLine(),
                           reinterpret_cast<quint32 *>(temp.bits()),
                           temp.bytesPerLine());
        }
    }

    img_height = temp.height();
    for (int row = 0; row != img_height; ++row) {
        for (int i = 0; i <= int(improvedQuality); ++i) {
            qt_blurrow<aprec, zprec, alphaOnly>(temp, row, alpha);
        }
    }

    if (transposed == 0) {
        if (img.depth() == 8) {
            qt_memrotate90(reinterpret_cast<const quint8 *>(temp.bits()),
                           temp.width(), temp.height(), temp.bytesPerLine(),
                           reinterpret_cast<quint8 *>(img.bits()),
                           img.bytesPerLine());
        } else {
            qt_memrotate90(reinterpret_cast<const quint32 *>(temp.bits()),
                           temp.width(), temp.height(), temp.bytesPerLine(),
                           reinterpret_cast<quint32 *>(img.bits()),
                           img.bytesPerLine());
        }
    } else {
        img = temp;
    }
}

#define AVG(a,b)  ( ((((a)^(b)) & 0xfefefefeUL) >> 1) + ((a)&(b)) )
#define AVG16(a,b)  ( ((((a)^(b)) & 0xf7deUL) >> 1) + ((a)&(b)) )

[[nodiscard]] static inline QImage qt_halfScaled(const QImage &source)
{
    if ((source.width() < 2) || (source.height() < 2)) {
        return {};
    }

    QImage srcImage = source;

    if ((source.format() == QImage::Format_Indexed8)
        || (source.format() == QImage::Format_Grayscale8)) {
        // assumes grayscale
        QImage dest(source.width() / 2, source.height() / 2, srcImage.format());
        dest.setDevicePixelRatio(source.devicePixelRatio());

        auto src = reinterpret_cast<const uchar *>(const_cast<const QImage &>(srcImage).bits());
        const qsizetype sx = srcImage.bytesPerLine();
        const qsizetype sx2 = (sx << 1);

        auto dst = reinterpret_cast<uchar *>(dest.bits());
        const qsizetype dx = dest.bytesPerLine();
        const int ww = dest.width();
        const int hh = dest.height();

        for (int y = hh; y; --y, dst += dx, src += sx2) {
            const uchar *p1 = src;
            const uchar *p2 = (src + sx);
            uchar *q = dst;
            for (int x = ww; x; --x, ++q, p1 += 2, p2 += 2) {
                *q = (((int(p1[0]) + int(p1[1]) + int(p2[0]) + int(p2[1])) + 2) >> 2);
            }
        }

        return dest;
    }
    if (source.format() == QImage::Format_ARGB8565_Premultiplied) {
        QImage dest(source.width() / 2, source.height() / 2, srcImage.format());
        dest.setDevicePixelRatio(source.devicePixelRatio());

        auto src = reinterpret_cast<const uchar *>(const_cast<const QImage &>(srcImage).bits());
        const qsizetype sx = srcImage.bytesPerLine();
        const qsizetype sx2 = (sx << 1);

        auto dst = reinterpret_cast<uchar *>(dest.bits());
        const qsizetype dx = dest.bytesPerLine();
        const int ww = dest.width();
        const int hh = dest.height();

        for (int y = hh; y; --y, dst += dx, src += sx2) {
            const uchar *p1 = src;
            const uchar *p2 = (src + sx);
            uchar *q = dst;
            for (int x = ww; x; --x, q += 3, p1 += 6, p2 += 6) {
                // alpha
                q[0] = AVG(AVG(p1[0], p1[3]), AVG(p2[0], p2[3]));
                // rgb
                const quint16 p16_1 = ((p1[2] << 8) | p1[1]);
                const quint16 p16_2 = ((p1[5] << 8) | p1[4]);
                const quint16 p16_3 = ((p2[2] << 8) | p2[1]);
                const quint16 p16_4 = ((p2[5] << 8) | p2[4]);
                const quint16 result = AVG16(AVG16(p16_1, p16_2), AVG16(p16_3, p16_4));
                q[1] = (result & 0xff);
                q[2] = (result >> 8);
            }
        }

        return dest;
    }
    if ((source.format() != kDefaultImageFormat)
        && (source.format() != QImage::Format_RGB32)) {
        srcImage = source.convertToFormat(kDefaultImageFormat);
    }

    QImage dest(source.width() / 2, source.height() / 2, srcImage.format());
    dest.setDevicePixelRatio(source.devicePixelRatio());

    auto src = reinterpret_cast<const quint32 *>(const_cast<const QImage &>(srcImage).bits());
    const qsizetype sx = (srcImage.bytesPerLine() >> 2);
    const qsizetype sx2 = (sx << 1);

    auto dst = reinterpret_cast<quint32 *>(dest.bits());
    const qsizetype dx = (dest.bytesPerLine() >> 2);
    const int ww = dest.width();
    const int hh = dest.height();

    for (int y = hh; y; --y, dst += dx, src += sx2) {
        const quint32 *p1 = src;
        const quint32 *p2 = (src + sx);
        quint32 *q = dst;
        for (int x = ww; x; --x, q++, p1 += 2, p2 += 2) {
            *q = AVG(AVG(p1[0], p1[1]), AVG(p2[0], p2[1]));
        }
    }

    return dest;
}

[[maybe_unused]] static inline void qt_blurImage(QPainter *p, QImage &blurImage,
    qreal radius, const bool quality, const bool alphaOnly, const int transposed = 0)
{
    if ((blurImage.format() != kDefaultImageFormat)
        && (blurImage.format() != QImage::Format_RGB32)) {
        blurImage = blurImage.convertToFormat(kDefaultImageFormat);
    }

    qreal scale = 1.0;
    if ((radius >= 4) && (blurImage.width() >= 2) && (blurImage.height() >= 2)) {
        blurImage = qt_halfScaled(blurImage);
        scale = 2.0;
        radius *= 0.5;
    }

    if (alphaOnly) {
        expblur<12, 10, true>(blurImage, radius, quality, transposed);
    } else {
        expblur<12, 10, false>(blurImage, radius, quality, transposed);
    }

    if (p) {
        p->save();
        // We need a blurry image anyway, we don't need high quality image processing.
        p->setRenderHint(QPainter::Antialiasing, false);
        p->setRenderHint(QPainter::TextAntialiasing, false);
        p->setRenderHint(QPainter::SmoothPixmapTransform, false);
        p->scale(scale, scale);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
        const QSize imageSize = blurImage.deviceIndependentSize().toSize();
#else
        const QSize imageSize = QSizeF(QSizeF(blurImage.size()) / blurImage.devicePixelRatio()).toSize();
#endif
        p->drawImage(QRect(QPoint(0, 0), imageSize), blurImage);
        p->restore();
    }
}

[[maybe_unused]] static inline void qt_blurImage(QImage &blurImage,
    const qreal radius, const bool quality, const int transposed = 0)
{
    if ((blurImage.format() == QImage::Format_Indexed8)
        || (blurImage.format() == QImage::Format_Grayscale8)) {
        expblur<12, 10, true>(blurImage, radius, quality, transposed);
    } else {
        expblur<12, 10, false>(blurImage, radius, quality, transposed);
    }
}
#endif

/*!
    Transforms an \a alignment of Qt::AlignLeft or Qt::AlignRight
    without Qt::AlignAbsolute into Qt::AlignLeft or Qt::AlignRight with
    Qt::AlignAbsolute according to the layout \a direction. The other
    alignment flags are left untouched.

    If no horizontal alignment was specified, the function returns the
    default alignment for the given layout \a direction.

    \sa QWidget::layoutDirection
*/
[[nodiscard]] static inline Qt::Alignment visualAlignment
    (const Qt::LayoutDirection direction, Qt::Alignment alignment)
{
    if (!(alignment & Qt::AlignHorizontal_Mask)) {
        alignment |= Qt::AlignLeft;
    }
    if (!(alignment & Qt::AlignAbsolute) && (alignment & (Qt::AlignLeft | Qt::AlignRight))) {
        if (direction == Qt::RightToLeft) {
            alignment ^= (Qt::AlignLeft | Qt::AlignRight);
        }
        alignment |= Qt::AlignAbsolute;
    }
    return alignment;
}

/*!
    Returns a new rectangle of the specified \a size that is aligned to the given
    \a rectangle according to the specified \a alignment and \a direction.
 */
[[nodiscard]] static inline QRect alignedRect(const Qt::LayoutDirection direction,
    Qt::Alignment alignment, const QSize &size, const QRect &rectangle)
{
    alignment = visualAlignment(direction, alignment);
    int x = rectangle.x();
    int y = rectangle.y();
    int w = size.width();
    int h = size.height();
    if ((alignment & Qt::AlignVCenter) == Qt::AlignVCenter) {
        y += ((rectangle.size().height() / 2) - (h / 2));
    } else if ((alignment & Qt::AlignBottom) == Qt::AlignBottom) {
        y += (rectangle.size().height() - h);
    }
    if ((alignment & Qt::AlignRight) == Qt::AlignRight) {
        x += (rectangle.size().width() - w);
    } else if ((alignment & Qt::AlignHCenter) == Qt::AlignHCenter) {
        x += ((rectangle.size().width() / 2) - (w / 2));
    }
    return {x, y, w, h};
}

WallpaperThread::WallpaperThread(QObject *parent) : FramelessHelperThreadClass(parent)
{
}

WallpaperThread::~WallpaperThread() = default;

#if FRAMELESSHELPER_HAS_THREAD
void WallpaperThread::run()
#else
void WallpaperThread::start()
#endif
{
    const QString wallpaperFilePath = Utils::getWallpaperFilePath();
    if (wallpaperFilePath.isEmpty()) {
        WARNING << "Failed to retrieve the wallpaper file path.";
        return;
    }
    // QImageReader allows us read the image size before we actually loading it, this behavior
    // can help us avoid consume too much memory if the image resolution is very large, eg, 4K.
    QImageReader reader(wallpaperFilePath);
    if (!reader.canRead()) {
        WARNING << "Qt can't read the wallpaper file:" << reader.errorString();
        return;
    }
    const QSize actualSize = reader.size();
    if (actualSize.isEmpty()) {
        WARNING << "The wallpaper picture size is invalid.";
        return;
    }
    const QSize correctedSize = (actualSize > kMaximumPictureSize ? kMaximumPictureSize : actualSize);
    if (correctedSize != actualSize) {
        DEBUG << "The wallpaper picture size is greater than 1920x1080, it will be shrinked to reduce memory consumption.";
        reader.setScaledSize(correctedSize);
    }
    QImage image(correctedSize, kDefaultImageFormat);
    if (!reader.read(&image)) {
        WARNING << "Failed to read the wallpaper image:" << reader.errorString();
        return;
    }
    if (image.isNull()) {
        WARNING << "The obtained image data is null.";
        return;
    }
    WallpaperAspectStyle aspectStyle = Utils::getWallpaperAspectStyle();
    const QSize wallpaperSize = QGuiApplication::primaryScreen()->size();
    QImage buffer(wallpaperSize, kDefaultImageFormat);
#ifdef Q_OS_WINDOWS
    if (aspectStyle == WallpaperAspectStyle::Center) {
        buffer.fill(kDefaultBlackColor);
    }
#endif
    if ((aspectStyle == WallpaperAspectStyle::Stretch)
        || (aspectStyle == WallpaperAspectStyle::Fit)
        || (aspectStyle == WallpaperAspectStyle::Fill)) {
        Qt::AspectRatioMode mode = Qt::KeepAspectRatioByExpanding;
        if (aspectStyle == WallpaperAspectStyle::Stretch) {
            mode = Qt::IgnoreAspectRatio;
        } else if (aspectStyle == WallpaperAspectStyle::Fit) {
            mode = Qt::KeepAspectRatio;
        }
        QSize newSize = image.size();
        newSize.scale(wallpaperSize, mode);
        image = image.scaled(newSize);
    }
    static constexpr const QPoint desktopOriginPoint = {0, 0};
    const QRect desktopRect = {desktopOriginPoint, wallpaperSize};
    if (aspectStyle == WallpaperAspectStyle::Tile) {
        QPainter bufferPainter(&buffer);
        // Same as above, we prefer speed than quality here.
        bufferPainter.setRenderHint(QPainter::Antialiasing, false);
        bufferPainter.setRenderHint(QPainter::TextAntialiasing, false);
        bufferPainter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        bufferPainter.fillRect(desktopRect, QBrush(image));
    } else {
        QPainter bufferPainter(&buffer);
        // Same here.
        bufferPainter.setRenderHint(QPainter::Antialiasing, false);
        bufferPainter.setRenderHint(QPainter::TextAntialiasing, false);
        bufferPainter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        const QRect rect = alignedRect(Qt::LeftToRight, Qt::AlignCenter, image.size(), desktopRect);
        bufferPainter.drawImage(rect.topLeft(), image);
    }
    {
#if FRAMELESSHELPER_HAS_THREAD
        const QMutexLocker locker(&g_imageData()->mutex);
#endif
        g_imageData()->blurredWallpaper = QPixmap(wallpaperSize);
        g_imageData()->blurredWallpaper.fill(kDefaultTransparentColor);
        QPainter painter(&g_imageData()->blurredWallpaper);
        // Same here.
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setRenderHint(QPainter::TextAntialiasing, false);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
#if FRAMELESSHELPER_CONFIG(private_qt)
        qt_blurImage(&painter, buffer, kDefaultBlurRadius, false, false);
#else // !FRAMELESSHELPER_CONFIG(private_qt)
        painter.drawImage(desktopOriginPoint, buffer);
#endif // FRAMELESSHELPER_CONFIG(private_qt)
    }
    Q_EMIT imageUpdated();
}

struct ThreadData
{
    std::unique_ptr<WallpaperThread> thread = nullptr;
#if FRAMELESSHELPER_HAS_THREAD
    QMutex mutex{};
#endif
};
Q_GLOBAL_STATIC(ThreadData, g_threadData)

#if FRAMELESSHELPER_HAS_THREAD
static inline void threadCleaner()
{
    const QMutexLocker locker(&g_threadData()->mutex);
    if (g_threadData()->thread && g_threadData()->thread->isRunning()) {
        g_threadData()->thread->requestInterruption();
        g_threadData()->thread->quit();
        g_threadData()->thread->wait();
    }
}
#endif

MicaMaterialPrivate::MicaMaterialPrivate(MicaMaterial *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
    initialize();
}

MicaMaterialPrivate::~MicaMaterialPrivate() = default;

MicaMaterialPrivate *MicaMaterialPrivate::get(MicaMaterial *q)
{
    Q_ASSERT(q);
    if (!q) {
        return nullptr;
    }
    return q->d_func();
}

const MicaMaterialPrivate *MicaMaterialPrivate::get(const MicaMaterial *q)
{
    Q_ASSERT(q);
    if (!q) {
        return nullptr;
    }
    return q->d_func();
}

void MicaMaterialPrivate::maybeGenerateBlurredWallpaper(const bool force)
{
#if FRAMELESSHELPER_HAS_THREAD
    g_imageData()->mutex.lock();
#endif
    if (!g_imageData()->blurredWallpaper.isNull() && !force) {
#if FRAMELESSHELPER_HAS_THREAD
        g_imageData()->mutex.unlock();
#endif
        return;
    }
#if FRAMELESSHELPER_HAS_THREAD
    g_imageData()->mutex.unlock();
    const QMutexLocker locker(&g_threadData()->mutex);
    if (g_threadData()->thread->isRunning()) {
        g_threadData()->thread->requestInterruption();
        g_threadData()->thread->quit();
        g_threadData()->thread->wait();
    }
    g_threadData()->thread->start(QThread::LowPriority);
#else
    g_threadData()->thread->start();
#endif
}

void MicaMaterialPrivate::updateMaterialBrush()
{
#if FRAMELESSHELPER_CONFIG(bundle_resource)
    FramelessHelperCoreInitResource();
    static const QImage noiseTexture = QImage(FRAMELESSHELPER_STRING_LITERAL(":/org.wangwenx190.FramelessHelper/resources/images/noise.png"));
#endif // FRAMELESSHELPER_CORE_NO_BUNDLE_RESOURCE
    QImage micaTexture = QImage(QSize(64, 64), kDefaultImageFormat);
    QColor fillColor = ((FramelessManager::instance()->systemTheme() == SystemTheme::Dark) ? kDefaultSystemDarkColor : kDefaultSystemLightColor2);
    fillColor.setAlphaF(0.9f);
    micaTexture.fill(fillColor);
    QPainter painter(&micaTexture);
    // Same as above. We need speed, not quality.
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::TextAntialiasing, false);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setOpacity(tintOpacity);
    const QRect rect = {QPoint(0, 0), micaTexture.size()};
    painter.fillRect(rect, tintColor);
    painter.setOpacity(noiseOpacity);
#if FRAMELESSHELPER_CONFIG(bundle_resource)
    painter.fillRect(rect, QBrush(noiseTexture));
#endif // FRAMELESSHELPER_CORE_NO_BUNDLE_RESOURCE
    micaBrush = QBrush(micaTexture);
    if (initialized) {
        Q_Q(MicaMaterial);
        Q_EMIT q->shouldRedraw();
    }
}

void MicaMaterialPrivate::forceRebuildWallpaper()
{
    wallpaperSize = QGuiApplication::primaryScreen()->size();
    maybeGenerateBlurredWallpaper(true);
}

void MicaMaterialPrivate::initialize()
{
#if FRAMELESSHELPER_HAS_THREAD
    g_threadData()->mutex.lock();
#endif
    if (!g_threadData()->thread) {
        g_threadData()->thread = std::make_unique<WallpaperThread>();
#if FRAMELESSHELPER_HAS_THREAD
        qAddPostRoutine(threadCleaner);
#endif
    }
    connect(g_threadData()->thread.get(), &WallpaperThread::imageUpdated, this, [this](){
        if (initialized) {
            Q_Q(MicaMaterial);
            Q_EMIT q->shouldRedraw();
        }
    });
#if FRAMELESSHELPER_HAS_THREAD
    g_threadData()->mutex.unlock();
#endif

    wallpaperSize = QGuiApplication::primaryScreen()->size();

    tintColor = kDefaultTransparentColor;
    tintOpacity = kDefaultTintOpacity;
    // Leave fallbackColor invalid, we need to use this state to judge
    // whether we should use the system color instead.
    noiseOpacity = kDefaultNoiseOpacity;

    updateMaterialBrush();

    connect(FramelessManager::instance(), &FramelessManager::systemThemeChanged,
        this, &MicaMaterialPrivate::updateMaterialBrush);
    connect(FramelessManager::instance(), &FramelessManager::wallpaperChanged,
        this, &MicaMaterialPrivate::forceRebuildWallpaper);
    connect(qGuiApp, &QGuiApplication::primaryScreenChanged,
        this, &MicaMaterialPrivate::forceRebuildWallpaper);

    if (FramelessConfig::instance()->isSet(Option::DisableLazyInitializationForMicaMaterial)) {
        prepareGraphicsResources();
    }

    initialized = true;
}

void MicaMaterialPrivate::prepareGraphicsResources()
{
#if FRAMELESSHELPER_HAS_THREAD
    g_imageData()->mutex.lock();
#endif
    if (g_imageData()->graphicsResourcesReady) {
#if FRAMELESSHELPER_HAS_THREAD
        g_imageData()->mutex.unlock();
#endif
        return;
    }
    g_imageData()->graphicsResourcesReady = true;
#if FRAMELESSHELPER_HAS_THREAD
    g_imageData()->mutex.unlock();
#endif
    maybeGenerateBlurredWallpaper();
}

QColor MicaMaterialPrivate::systemFallbackColor()
{
    return ((FramelessManager::instance()->systemTheme() == SystemTheme::Dark) ? kDefaultFallbackColorDark : kDefaultFallbackColorLight);
}

QPoint MicaMaterialPrivate::mapToWallpaper(const QPoint &pos) const
{
    if (pos.isNull()) {
        return {};
    }
    QPointF result = pos;
    // Make sure the position is always inside the wallpaper rectangle.
    while (result.x() < qreal(0)) {
        result.setX(result.x() + wallpaperSize.width());
    }
    while ((result.x() > wallpaperSize.width()) || qFuzzyCompare(result.x(), wallpaperSize.width())) {
        result.setX(result.x() - wallpaperSize.width());
    }
    while (result.y() < qreal(0)) {
        result.setY(result.y() + wallpaperSize.height());
    }
    while ((result.y() > wallpaperSize.height()) || qFuzzyCompare(result.y(), wallpaperSize.height())) {
        result.setY(result.y() - wallpaperSize.height());
    }
    return result.toPoint();
}

QSize MicaMaterialPrivate::mapToWallpaper(const QSize &size) const
{
    if (size.isEmpty()) {
        return {};
    }
    QSizeF result = size;
    // Make sure we don't get a size larger than the wallpaper's size.
    if (result.width() > wallpaperSize.width()) {
        result.setWidth(wallpaperSize.width());
    }
    if (result.height() > wallpaperSize.height()) {
        result.setHeight(wallpaperSize.height());
    }
    return result.toSize();
}

QRect MicaMaterialPrivate::mapToWallpaper(const QRect &rect) const
{
    const auto wallpaperRect = QRectF{ QPointF{ 0, 0 }, wallpaperSize };
    const auto mappedRect = QRectF{ mapToWallpaper(rect.topLeft()), mapToWallpaper(rect.size()) };
    if (!Utils::isValidGeometry(mappedRect)) {
        WARNING << "The calculated mapped rectangle is not valid.";
        return wallpaperRect.toRect();
    }
    return mappedRect.toRect();
}

MicaMaterial::MicaMaterial(QObject *parent)
    : QObject(parent), d_ptr(std::make_unique<MicaMaterialPrivate>(this))
{
}

MicaMaterial::~MicaMaterial() = default;

QColor MicaMaterial::tintColor() const
{
    Q_D(const MicaMaterial);
    return d->tintColor;
}

void MicaMaterial::setTintColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(MicaMaterial);
    if (d->tintColor == value) {
        return;
    }
    d->prepareGraphicsResources();
    d->tintColor = value;
    d->updateMaterialBrush();
    Q_EMIT tintColorChanged();
}

qreal MicaMaterial::tintOpacity() const
{
    Q_D(const MicaMaterial);
    return d->tintOpacity;
}

void MicaMaterial::setTintOpacity(const qreal value)
{
    Q_D(MicaMaterial);
    if (qFuzzyCompare(d->tintOpacity, value)) {
        return;
    }
    d->prepareGraphicsResources();
    d->tintOpacity = value;
    d->updateMaterialBrush();
    Q_EMIT tintOpacityChanged();
}

QColor MicaMaterial::fallbackColor() const
{
    Q_D(const MicaMaterial);
    return d->fallbackColor;
}

void MicaMaterial::setFallbackColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(MicaMaterial);
    if (d->fallbackColor == value) {
        return;
    }
    d->prepareGraphicsResources();
    d->fallbackColor = value;
    d->updateMaterialBrush();
    Q_EMIT fallbackColorChanged();
}

qreal MicaMaterial::noiseOpacity() const
{
    Q_D(const MicaMaterial);
    return d->noiseOpacity;
}

void MicaMaterial::setNoiseOpacity(const qreal value)
{
    Q_D(MicaMaterial);
    if (qFuzzyCompare(d->noiseOpacity, value)) {
        return;
    }
    d->prepareGraphicsResources();
    d->noiseOpacity = value;
    d->updateMaterialBrush();
    Q_EMIT noiseOpacityChanged();
}

bool MicaMaterial::isFallbackEnabled() const
{
    Q_D(const MicaMaterial);
    return d->fallbackEnabled;
}

void MicaMaterial::setFallbackEnabled(const bool value)
{
    Q_D(MicaMaterial);
    if (d->fallbackEnabled == value) {
        return;
    }
    d->prepareGraphicsResources();
    d->fallbackEnabled = value;
    d->updateMaterialBrush();
    Q_EMIT fallbackEnabledChanged();
}

void MicaMaterial::paint(QPainter *painter, const QRect &rect, const bool active)
{
    Q_ASSERT(painter);
    if (!painter) {
        return;
    }
    Q_D(MicaMaterial);
    d->prepareGraphicsResources();
    static constexpr const auto originPoint = QPoint{ 0, 0 };
    const QRect wallpaperRect = { originPoint, d->wallpaperSize };
    const QRect mappedRect = d->mapToWallpaper(rect);
    painter->save();
    // Same as above. Speed is more important here.
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setRenderHint(QPainter::TextAntialiasing, false);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
    if (active) {
        const QRect intersectedRect = wallpaperRect.intersected(mappedRect);
#if FRAMELESSHELPER_HAS_THREAD
        g_imageData()->mutex.lock();
#endif
        painter->drawPixmap(originPoint, g_imageData()->blurredWallpaper, intersectedRect);
#if FRAMELESSHELPER_HAS_THREAD
        g_imageData()->mutex.unlock();
#endif
        if (intersectedRect != mappedRect) {
            static constexpr const auto xOffset = QPoint{ 1, 0 };
            if (mappedRect.y() + mappedRect.height() <= wallpaperRect.height()) {
                const QRect outerRect = { intersectedRect.topRight() + xOffset, QSize{ mappedRect.width() - intersectedRect.width(), intersectedRect.height() } };
                const QPoint outerRectOriginPoint = originPoint + QPoint{ intersectedRect.width(), 0 } + xOffset;
                const QRect mappedOuterRect = d->mapToWallpaper(outerRect);
#if FRAMELESSHELPER_HAS_THREAD
                const QMutexLocker locker(&g_imageData()->mutex);
#endif
                painter->drawPixmap(outerRectOriginPoint, g_imageData()->blurredWallpaper, mappedOuterRect);
            } else {
                static constexpr const auto yOffset = QPoint{ 0, 1 };
                const QRect outerRectBottom = { intersectedRect.bottomLeft() + yOffset, QSize{ intersectedRect.width(), mappedRect.height() - intersectedRect.height() } };
                const QPoint outerRectBottomOriginPoint = originPoint + QPoint{ 0, intersectedRect.height() } + yOffset;
                const QRect mappedOuterRectBottom = d->mapToWallpaper(outerRectBottom);
#if FRAMELESSHELPER_HAS_THREAD
                g_imageData()->mutex.lock();
#endif
                painter->drawPixmap(outerRectBottomOriginPoint, g_imageData()->blurredWallpaper, mappedOuterRectBottom);
#if FRAMELESSHELPER_HAS_THREAD
                g_imageData()->mutex.unlock();
#endif
                if (mappedRect.x() + mappedRect.width() > wallpaperRect.width()) {
                    const QRect outerRectRight = { intersectedRect.topRight() + xOffset, QSize{ mappedRect.width() - intersectedRect.width(), intersectedRect.height() } };
                    const QPoint outerRectRightOriginPoint = originPoint + QPoint{ intersectedRect.width(), 0 } + xOffset;
                    const QRect mappedOuterRectRight = d->mapToWallpaper(outerRectRight);
                    const QRect outerRectCorner = { intersectedRect.bottomRight() + xOffset + yOffset, QSize{ outerRectRight.width(), outerRectBottom.height() } };
                    const QPoint outerRectCornerOriginPoint = originPoint + QPoint{ intersectedRect.width(), intersectedRect.height() } + xOffset + yOffset;
                    const QRect mappedOuterRectCorner = d->mapToWallpaper(outerRectCorner);
#if FRAMELESSHELPER_HAS_THREAD
                    const QMutexLocker locker(&g_imageData()->mutex);
#endif
                    painter->drawPixmap(outerRectRightOriginPoint, g_imageData()->blurredWallpaper, mappedOuterRectRight);
                    painter->drawPixmap(outerRectCornerOriginPoint, g_imageData()->blurredWallpaper, mappedOuterRectCorner);
                }
            }
        }
    }
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->setOpacity(qreal(1));
    painter->fillRect(QRect{originPoint, mappedRect.size()}, [d, active]() -> QBrush {
        if (!d->fallbackEnabled || active) {
            return d->micaBrush;
        }
        if (d->fallbackColor.isValid()) {
            return d->fallbackColor;
        }
        return d->systemFallbackColor();
    }());
    painter->restore();
}

FRAMELESSHELPER_END_NAMESPACE

#endif
