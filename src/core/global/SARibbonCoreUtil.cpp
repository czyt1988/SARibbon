#include "SARibbonCoreUtil.h"
#include <QDebug>
#include <QGuiApplication>
#include <QStyleHints>
#include <QSettings>
#include <QProcess>

// 计划 02 S1：函数体自 src/widgets/SARibbonUtil.cpp 纯 move（仅 saIsRTL 的
// widgets 版的 QApplication 调用换成行为等价的 QGuiApplication 调用）。
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
 * @brief 为 QIcon 生成指定 devicePixelRatio 的高分辨率 pixmap
 *
 * 之所以提供此函数，是因为 Qt5 的 QIcon::pixmap() 没有 devicePixelRatio 参数，而Qt6做了适配，为此专门提供一个兼容函数
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
    Q_UNUSED(devicePixelRatio);
    return icon.pixmap(size, mode, state);
#endif
}

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
bool saIsRTL()
{
    return QGuiApplication::layoutDirection() == Qt::RightToLeft;
}

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
int saMirrorX(int x, int containerWidth, int elementWidth)
{
    if (saIsRTL()) {
        return containerWidth - x - elementWidth;
    }
    return x;
}

/**
 * @brief Check if the operating system uses dark mode
 *
 * Detects dark mode via three tiers:
 * 1. Qt 6.5+: Uses QGuiApplication::styleHints()->colorScheme()
 * 2. Qt < 6.5 Windows: Reads registry AppsUseLightTheme
 * 3. Qt < 6.5 macOS: Runs 'defaults read -g AppleInterfaceStyle'
 * 4. Qt < 6.5 Linux: Runs 'gsettings get org.gnome.desktop.interface color-scheme'
 * 5. All other cases: Returns false (assume light mode)
 *
 * @return true if OS dark mode is active, false otherwise
 */
bool isOperatingSystemInDarkMode()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    // Qt 6.5+ has native cross-platform API
    return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#elif defined(Q_OS_WIN32)
    // Windows: Read registry AppsUseLightTheme
    // DWORD value 0 = dark mode, 1 = light mode
    // Registry path: HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        QSettings::NativeFormat);
    bool ok = false;
    int value = settings.value("AppsUseLightTheme", 1).toInt(&ok);
    return (ok && value == 0);
#elif defined(Q_OS_MACOS)
    // macOS: Run 'defaults read -g AppleInterfaceStyle'
    // Returns "Dark" if dark mode is active, empty otherwise
    QProcess process;
    process.start("/usr/bin/defaults", QStringList() << "read" << "-g" << "AppleInterfaceStyle");
    process.waitForFinished(3000);
    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    return output.contains("Dark", Qt::CaseInsensitive);
#elif defined(Q_OS_LINUX)
    // Linux: Try gsettings for GNOME 42+
    // Returns "dark" or "prefer-dark" in dark mode
    QProcess process;
    process.start("gsettings", QStringList() << "get" << "org.gnome.desktop.interface" << "color-scheme");
    process.waitForFinished(3000);
    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    return output.contains("dark", Qt::CaseInsensitive);
#else
    // Unknown platform or headless: assume light mode
    return false;
#endif
}

/**
 * @brief 系统暗色模式自动切换开关的存储
 *
 * 使用函数内静态变量而非全局静态变量，避免静态初始化顺序问题
 * @return 开关变量的引用
 */
static bool& systemDarkModeAutoSwitchFlag()
{
    static bool enable = true;
    return enable;
}

/**
 * \if ENGLISH
 * @brief Enable or disable automatic theme switching by system dark mode
 * @details SARibbonMainWindow and SARibbonWidget check the operating system color scheme
 * during construction: when the system is in dark mode and the theme is still the default
 * RibbonThemeOffice2021Blue, the theme is automatically switched to RibbonThemeDark.
 * Call this function with @c false before constructing the window to keep the default
 * theme regardless of the system color scheme.
 * @param on true to enable the automatic switching (default), false to disable it
 * @sa isEnableSystemDarkModeAutoSwitch()
 * \endif
 *
 * \if CHINESE
 * @brief 开启或关闭系统暗色模式触发的自动主题切换
 * @details SARibbonMainWindow 和 SARibbonWidget 在构造时会检测操作系统的颜色模式：
 * 当系统处于暗色模式且主题仍为默认的 RibbonThemeOffice2021Blue 时，会自动把主题切换为
 * RibbonThemeDark。若不希望此行为，可在构造窗口之前调用本函数并传入 @c false，
 * 此后无论系统处于何种颜色模式，默认主题都保持 RibbonThemeOffice2021Blue 不变。
 * @param on true 开启自动切换（默认），false 关闭自动切换
 * @sa isEnableSystemDarkModeAutoSwitch()
 * \endif
 */
void setEnableSystemDarkModeAutoSwitch(bool on)
{
    systemDarkModeAutoSwitchFlag() = on;
}

/**
 * \if ENGLISH
 * @brief Query whether automatic theme switching by system dark mode is enabled
 * @return true if the automatic switching is enabled (default), false otherwise
 * @sa setEnableSystemDarkModeAutoSwitch()
 * \endif
 *
 * \if CHINESE
 * @brief 查询系统暗色模式触发的自动主题切换是否处于开启状态
 * @return 开启返回true（默认），关闭返回false
 * @sa setEnableSystemDarkModeAutoSwitch()
 * \endif
 */
bool isEnableSystemDarkModeAutoSwitch()
{
    return systemDarkModeAutoSwitchFlag();
}

/**
 * \if ENGLISH
 * @brief Gallery grid cell size shared by both front ends
 * @details Body moved verbatim from SARibbonGalleryGroup::recalcGridSize
 *          (grid size part); the icon-size part stays in the widgets group
 *          because it depends on widget fontMetrics.
 * \endif
 *
 * \if CHINESE
 * @brief 画廊网格单元尺寸（双前端共用）
 * @details 函数体自 SARibbonGalleryGroup::recalcGridSize 纯 move（网格
 *          尺寸部分）；图标尺寸部分依赖 widgets 侧 fontMetrics，留在原处。
 * \endif
 */
QSize calcGalleryGridCellSize(int galleryHeight, int displayRow, int gridMinimumWidth, int gridMaximumWidth)
{
    // 首先通过DisplayRow计算GridSize
    int dr = displayRow;
    if (dr < 1) {
        dr = 1;
    } else if (dr > 3) {
        dr = 3;
    }
    int h = galleryHeight / dr;
    if (h <= 1) {
        h = galleryHeight;
    }
    int w = h;
    if (gridMinimumWidth > 0) {
        if (w < gridMinimumWidth) {
            w = gridMinimumWidth;
        }
    }
    if (gridMaximumWidth > 0) {
        if (w > gridMaximumWidth) {
            w = gridMaximumWidth;
        }
    }
    return QSize(w, h);
}

}
