#include "SARibbonIconHelper.h"
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmapCache>
#include <QCryptographicHash>
#include <QBuffer>

namespace SA {

QIcon SARibbonIconHelper::themedIcon(const QString& svgPath, 
                                     const QColor& color, 
                                     const QSize& size)
{
    // Generate cache key from svgPath + color
    QString cacheKey = QString("sa_themed_%1_%2")
        .arg(svgPath)
        .arg(color.name(QColor::HexRgb));
    
    return themedIcon(svgPath, color, size, cacheKey);
}

QIcon SARibbonIconHelper::themedIcon(const QString& svgPath, 
                                     const QColor& color, 
                                     const QSize& size,
                                     const QString& cacheKey)
{
    QString key1x = cacheKey + "_1x";
    QString key2x = cacheKey + "_2x";
    
    QPixmap pm1x, pm2x;
    
    // Check cache for 1x pixmap
    if (!QPixmapCache::find(key1x, &pm1x)) {
        pm1x = createThemedPixmap(svgPath, color, size, 1.0);
        if (!pm1x.isNull()) {
            QPixmapCache::insert(key1x, pm1x);
        }
    }
    
    // Check cache for 2x pixmap
    if (!QPixmapCache::find(key2x, &pm2x)) {
        pm2x = createThemedPixmap(svgPath, color, size, 2.0);
        if (!pm2x.isNull()) {
            QPixmapCache::insert(key2x, pm2x);
        }
    }
    
    QIcon icon;
    if (!pm1x.isNull()) {
        icon.addPixmap(pm1x);
    }
    if (!pm2x.isNull()) {
        icon.addPixmap(pm2x);
    }
    
    return icon;
}

void SARibbonIconHelper::clearIconCache()
{
    QPixmapCache::clear();
}

QPixmap SARibbonIconHelper::createThemedPixmap(const QString& svgPath,
                                                const QColor& color,
                                                const QSize& size,
                                                qreal dpr)
{
    QSvgRenderer renderer(svgPath);
    if (!renderer.isValid()) {
        return QPixmap();
    }
    
    QSize pxSize = size * dpr;
    QPixmap px(pxSize);
    px.fill(Qt::transparent);
    px.setDevicePixelRatio(dpr);
    
    QPainter painter(&px);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    renderer.render(&painter);
    
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(px.rect(), color);
    painter.end();
    
    return px;
}

} // end namespace SA
