#ifndef SARIBBONTHEMEICONHELPER_H
#define SARIBBONTHEMEICONHELPER_H

#include "SARibbonGlobal.h"
#include <QIcon>
#include <QPixmap>
#include <QColor>
#include <QSize>
#include <QString>

namespace SA {

/**
 * @brief Helper class for creating themed icons from SVG files
 * 
 * Uses QPainter::CompositionMode_SourceIn to apply theme colors to monochrome SVGs,
 * with QPixmapCache for performance. Supports HiDPI scaling.
 */
class SA_RIBBON_EXPORT SARibbonIconHelper
{
public:
    /**
     * @brief Create a themed icon from an SVG file
     * @param svgPath Path to the SVG resource (e.g., ":/icons/menu.svg")
     * @param color Theme color to apply
     * @param size Target icon size (default 24x24)
     * @return QIcon with themed pixmap (includes @2x for HiDPI)
     */
    static QIcon themedIcon(const QString& svgPath, 
                           const QColor& color, 
                           const QSize& size = QSize(24, 24));

    /**
     * @brief Create themed icon with explicit cache key
     * @param svgPath Path to the SVG resource
     * @param color Theme color to apply
     * @param size Target icon size
     * @param cacheKey Unique cache identifier (auto-generated if empty)
     * @return QIcon with themed pixmap
     */
    static QIcon themedIcon(const QString& svgPath, 
                           const QColor& color, 
                           const QSize& size,
                           const QString& cacheKey);

    /**
     * @brief Clear all cached themed icons
     * 
     * Call this when the theme changes to prevent stale cached icons.
     */
    static void clearIconCache();

private:
    static QPixmap createThemedPixmap(const QString& svgPath,
                                     const QColor& color,
                                     const QSize& size,
                                     qreal dpr);
};

} // end namespace SA

#endif // SARIBBONTHEMEICONHELPER_H
