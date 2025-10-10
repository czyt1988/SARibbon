#include "SARibbonUtil.h"
#include <QFile>
#include <QWidget>
#include <QDebug>
#include <QDir>
#include <QApplication>
#include <QScreen>
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

/**
 * @brief 获取内置主题对应的qss
 * @param theme
 * @return
 */
QString getBuiltInRibbonThemeQss(SARibbonTheme theme)
{
    QFile file;
    switch (theme) {
    case SARibbonTheme::RibbonThemeWindows7:
        file.setFileName(":/SARibbonTheme/resource/theme-win7.qss");
        break;
    case SARibbonTheme::RibbonThemeOffice2013:
        file.setFileName(":/SARibbonTheme/resource/theme-office2013.qss");
        break;
    case SARibbonTheme::RibbonThemeOffice2016Blue:
        file.setFileName(":/SARibbonTheme/resource/theme-office2016-blue.qss");
        break;
    case SARibbonTheme::RibbonThemeOffice2021Blue:
        file.setFileName(":/SARibbonTheme/resource/theme-office2021-blue.qss");
        break;
    case SARibbonTheme::RibbonThemeDark:
        file.setFileName(":/SARibbonTheme/resource/theme-dark.qss");
        break;
    case SARibbonTheme::RibbonThemeDark2:
        file.setFileName(":/SARibbonTheme/resource/theme-dark2.qss");
        break;
    default:
        file.setFileName(":/SARibbonTheme/resource/theme-office2013.qss");
        break;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "can not load build in ribbon theme,reason is :" << file.errorString();
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

/**
 * @brief 设置内置的ribbon主题
 *
 * 之所以提供此函数，是因为在某些情况下，SARibbonBar不用在SARibbonMainWindow情况下的时候，也需要设置主题，
 * 但主题设置函数是SARibbonMainWindow的成员函数，因此，这里单独提供了这个函数给SARibbonWidget窗口使用
 *
 * @param w
 * @param theme
 */
void setBuiltInRibbonTheme(QWidget* w, SARibbonTheme theme)
{
    if (!w) {
        return;
    }
    w->setStyleSheet(getBuiltInRibbonThemeQss(theme));
}

/**
 * @brief 为 QIcon 生成指定 devicePixelRatio 的高分辨率 pixmap
 *
 * 之所以提供此函数，是因为 Qt 5 的 QIcon::pixmap() 没有 devicePixelRatio 参数，
 * 在高 DPI 多屏环境下，返回的 pixmap 其 devicePixelRatio() 恒为 1，
 * 导致 Qt 在绘制时再次拉伸，图标发糊。
 * 此函数先按物理像素尺寸索取图像，再手动打上正确的 DPR，
 * 使 Qt 5 项目无需升级到 Qt 6 也能获得“图标随屏清晰”的效果。
 *
 * @param icon   图标源
 * @param size   期望的逻辑像素大小（控件坐标系）
 * @param devicePixelRatio    目标屏幕的实时 devicePixelRatio（可用 QWindow::devicePixelRatio() 获取）
 * @param mode   图标模式（Normal/Disabled/Active/Selected）
 * @param state  图标状态（On/Off）
 * @return 已设置好 devicePixelRatio 的 QPixmap，可直接 drawPixmap 使用
 */
QPixmap iconToPixmap(const QIcon& icon, const QSize& size, qreal devicePixelRatio, QIcon::Mode mode, QIcon::State state)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return icon.pixmap(size, devicePixelRatio, mode, state);
#else
    if (qFuzzyIsNull(devicePixelRatio) || size.isEmpty()) {
        return QPixmap();
    }

    // 1. 换算成物理像素
    const QSize physical = size * devicePixelRatio;

    // 2. 让 QIcon 给出这么大的一张图
    QPixmap pm = icon.pixmap(physical, mode, state);

    // 3. 打上 DPR 戳，告诉 Qt “这张图已经高分辨率”
    if (!pm.isNull())
        pm.setDevicePixelRatio(devicePixelRatio);

    return pm;
#endif
}

/**
 * @brief 获取窗口当前所在屏幕的dpr
 * @param w
 * @return
 */
qreal widgetDevicePixelRatio(QWidget* w)
{
    if (!w) {
        return 1.0;
    }
    QScreen* sc = w->window()->screen();
    if (!sc) {
        // qApp->primaryScreen() 拿到的是“整个系统里被用户标记成 primary 的那一块屏,是“全局主屏”
        sc = QApplication::primaryScreen();
    }
    if (!sc) {
        return 1.0;
    }
    return sc->devicePixelRatio();
}

}
