#include "SARibbonUtil.h"

namespace SA
{

/**
 * @brief makeColorVibrant 让颜色鲜艳
 * @param c 原来的颜色
 * @param saturationDelta 增加饱和度（上限255）
 * @param valueDelta 增加明度（上限255）
 * @return
 */
QColor makeColorVibrant(const QColor& c, int saturationDelta, int valueDelta)
{
    int h, s, v, a;
    c.getHsv(&h, &s, &v, &a);  // 分解HSV分量

    // 增加饱和度（上限255）
    s = qMin(s + saturationDelta, 255);
    // 增加明度（上限255）
    v = qMin(v + valueDelta, 255);

    return QColor::fromHsv(h, s, v, a);  // 重新生成颜色
}

/**
 * @brief 按照指定的新高度，保持宽高比缩放 QSize
 *
 * 此函数根据原始尺寸的宽高比，计算出在指定新高度下的对应宽度，
 * 并返回一个新的 QSize 对象。
 *
 * @param originalSize 原始尺寸。
 * @param newHeight    缩放后的新高度。
 * @return             按比例缩放后的 QSize。
 *
 * @par 示例：
 * @code
 * QSize original(800, 600);
 * QSize scaled = scaleSizeByHeight(original, 300);
 * // scaled 将是 (400, 300)
 * @endcode
 */
QSize scaleSizeByHeight(const QSize& originalSize, int newHeight)
{
    // 检查原始尺寸高度、宽度是否有效，以及目标高度是否有效
    if (originalSize.height() <= 0 || originalSize.width() < 0 || newHeight <= 0) {
        return QSize(0, 0);  // 无效输入返回零尺寸
    }

    // 计算宽高比并缩放
    float aspectRatio = static_cast< float >(originalSize.width()) / static_cast< float >(originalSize.height());
    int newWidth      = static_cast< int >(newHeight * aspectRatio);
    return QSize(newWidth, newHeight);
}

/**
 * @brief 按照指定的新高度，宽高比为1:factor缩放 QSize。
 *
 * 此函数根据原始尺寸的宽高比，计算出在指定新高度下的对应宽度，
 * 并返回一个新的 QSize 对象。
 *
 * @param originalSize 原始尺寸。
 * @param newHeight    缩放后的新高度。
 * @param factor    宽高比 1:factor factor=1时，此函数和scaleSizeByHeight的两参数版本一样，如果factor=0.5，则宽高比为1:0.5，也就是高度扩充2倍，宽度扩充1倍
 * @return             按比例缩放后的 QSize。
 *
 * @par 示例：
 * @code
 * QSize original(800, 600);
 * QSize scaled = scaleSizeByHeight(original, 300, 2);
 * // scaled 将是 (600, 300)
 * @endcode
 */
QSize scaleSizeByHeight(const QSize& originalSize, int newHeight, qreal factor)
{
    // 1. 参数合法性检查
    if (originalSize.height() <= 0 || originalSize.width() < 0 || newHeight <= 0 || qFuzzyIsNull(factor)) {
        return QSize(0, 0);
    }

    // 2. 计算原始宽高比
    const qreal originalAspect = static_cast< qreal >(originalSize.width()) / static_cast< qreal >(originalSize.height());

    // 3. 把用户提供的 1:factor 转换成真正的目标宽高比
    //    目标宽高比 = originalAspect / factor
    const qreal targetAspect = originalAspect / factor;

    // 4. 根据新高度反推宽度
    const int newWidth = qRound(newHeight * targetAspect);

    return QSize(newWidth, newHeight);
}

/**
 * @brief 按照指定的新宽度，保持宽高比缩放 QSize。
 *
 * 此函数根据原始尺寸的宽高比，计算出在指定新宽度下的对应高度，
 * 并返回一个新的 QSize 对象。
 *
 * @param originalSize 原始尺寸。
 * @param newWidth     缩放后的新宽度。
 * @return             按比例缩放后的 QSize。
 *
 * @par 示例：
 * @code
 * QSize original(800, 600);
 * QSize scaled = scaleSizeByWidth(original, 400);
 * // scaled 将是 (400, 300)
 * @endcode
 */
QSize scaleSizeByWidth(const QSize& originalSize, int newWidth)
{
    // 检查原始尺寸宽度、高度是否有效，以及目标宽度是否有效
    if (originalSize.width() <= 0 || originalSize.height() < 0 || newWidth <= 0) {
        return QSize(0, 0);  // 无效输入返回零尺寸
    }

    // 计算高宽比并缩放
    float aspectRatio = static_cast< float >(originalSize.height()) / static_cast< float >(originalSize.width());
    int newHeight     = static_cast< int >(newWidth * aspectRatio);
    return QSize(newWidth, newHeight);
}

}
