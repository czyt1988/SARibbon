#include "RibbonTheme.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>
#include <QDebug>

namespace SARibbonQml {

namespace {
/**
 * \if ENGLISH
 * @brief Resolve a QUrl to a path QFile can open (local file, qrc resource or plain string)
 * \endif
 *
 * \if CHINESE
 * @brief 把 QUrl 解析成 QFile 能打开的路径（本地文件、qrc 资源或原样字符串）
 * \endif
 */
QString urlToLoadablePath(const QUrl& url)
{
    if (url.isLocalFile()) {
        return url.toLocalFile();
    }
    if (url.scheme() == QLatin1String("qrc")) {
        // "qrc:/SARibbonTheme/x.json" -> ":/SARibbonTheme/x.json"
        return QLatin1Char(':') + url.path();
    }
    return url.toString();
}
}  // namespace

RibbonTheme::RibbonTheme(QObject* parent) : QObject(parent)
{
    // bridge the core signal source: any theme/palette change re-emits both
    // notify signals so every QML binding refreshes (v2 §3.2-1)
    connect(SARibbon::Core::SARibbonThemeData::instance(), &SARibbon::Core::SARibbonThemeData::themeChanged,
            this, [this]() { Q_EMIT currentThemeChanged(); });
    connect(SARibbon::Core::SARibbonThemeData::instance(), &SARibbon::Core::SARibbonThemeData::paletteChanged,
            this, [this]() { Q_EMIT paletteChanged(); Q_EMIT currentThemeChanged(); });
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    // Qt 6.5+ is the first version with an OS color scheme notification; on
    // earlier versions systemDarkMode is a read-once snapshot (no signal source)
    if (QGuiApplication::instance()) {
        connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this,
                [this]() { Q_EMIT systemDarkModeChanged(); });
    }
#endif
    // QML-only apps never run the widgets theme manager, so the core palette
    // arrives empty and every token would fall back to the flat application
    // palette: load the current theme's default palette once, up front
    if (coreData()->palette().variables().isEmpty()) {
        applyThemePalette(coreData()->theme());
    }
}

RibbonTheme::~RibbonTheme()
{
}

RibbonTheme* RibbonTheme::instance()
{
    static RibbonTheme s_instance;
    return &s_instance;
}

SARibbon::Core::SARibbonThemeData* RibbonTheme::coreData() const
{
    return SARibbon::Core::SARibbonThemeData::instance();
}

/**
 * \if ENGLISH
 * @brief Install the built-in palette of a theme
 * @details The theme -> palette mapping lives in core (SARibbonThemeData), so the
 *          QML front end and the widgets front end can never drift apart. A theme
 *          without a built-in palette (RibbonThemeUserDefine) keeps the palette the
 *          user loaded; if none was loaded but customPaletteSource is declared, that
 *          source is applied so switching to the user-defined theme is not a silent
 *          no-op on the colors. paletteChanged is emitted either way so bindings that
 *          also depend on the theme re-read their tokens.
 * \endif
 *
 * \if CHINESE
 * @brief 装入某个主题的内置调色板
 * @details 主题 -> 调色板映射在 core（SARibbonThemeData）里，QML 前端与 widgets 前端
 *          因此不会分叉。没有内置调色板的主题（RibbonThemeUserDefine）保留用户已加载
 *          的调色板；若尚未加载但声明了 customPaletteSource，则改用该来源，使切到
 *          自定义主题不会在颜色上变成静默空操作。两种情况都会发一次 paletteChanged，
 *          让同时依赖主题的绑定重读 token。
 * \endif
 */
void RibbonTheme::applyThemePalette(SARibbonTheme theme)
{
    const QString path = SARibbon::Core::SARibbonThemeData::themePalettePath(theme);
    if (path.isEmpty()) {
        if (!mHasCustomPalette && !mCustomPaletteSource.isEmpty()) {
            loadPaletteFromFile(urlToLoadablePath(mCustomPaletteSource));
        }
        Q_EMIT paletteChanged();
        return;
    }
    SA::SARibbonThemePalette pal;
    if (pal.loadFromFile(path)) {
        coreData()->setPalette(pal);  // Q_EMITs paletteChanged -> all bindings refresh
        setHasCustomPalette(false);
    }
}

int RibbonTheme::currentTheme() const
{
    return int(coreData()->theme());
}

/**
 * \if ENGLISH
 * @brief Switch the theme enum together with its palette
 * @details Mirrors applyRibbonTheme's non-QSS half: the palette is installed first so
 *          the hosts that polish on themeChanged already read the new tokens (the QSS
 *          half has no QML equivalent — rendering here is token-driven).
 * \endif
 *
 * \if CHINESE
 * @brief 同时切换主题枚举与其调色板
 * @details 对应 applyRibbonTheme 的非 QSS 一半：先装调色板，使在 themeChanged 上 polish
 *          的宿主读到的已是新 token（QSS 那一半在 QML 无对应物——这里渲染由 token 驱动）。
 * \endif
 */
void RibbonTheme::setCurrentTheme(int theme)
{
    const SARibbonTheme t = SARibbonTheme(theme);
    applyThemePalette(t);
    coreData()->setTheme(t);
}

bool RibbonTheme::isRtl() const
{
    return QGuiApplication::layoutDirection() == Qt::RightToLeft;
}

void RibbonTheme::setRtl(bool on)
{
    // the core engines read the app layout direction through SA::saIsRTL();
    // flipping it relayouts every host on the next polish pass (widgets
    // "Switch to RTL" parity)
    const Qt::LayoutDirection dir = on ? Qt::RightToLeft : Qt::LeftToRight;
    if (QGuiApplication::layoutDirection() == dir) {
        return;
    }
    QGuiApplication::setLayoutDirection(dir);
    Q_EMIT rtlChanged();
}

bool RibbonTheme::isDark() const
{
    return coreData()->palette().isDark();
}

QUrl RibbonTheme::customPaletteSource() const
{
    return mCustomPaletteSource;
}

/**
 * \if ENGLISH
 * @brief Declarative form of loadPaletteFromFile
 * @details Accepts a local file URL or a qrc URL; an empty URL only clears the
 *          recorded source and leaves the active palette untouched. Loading is
 *          immediate so a palette declared on the singleton takes effect before
 *          the first frame, exactly like the constructor's built-in load.
 * \endif
 *
 * \if CHINESE
 * @brief loadPaletteFromFile 的声明式写法
 * @details 接受本地文件 URL 或 qrc URL；空 URL 只清除记录的来源，不动当前生效的调色板。
 *          加载是立即执行的，因此声明在单例上的调色板会在首帧之前生效，与构造函数里
 *          那次内置加载一致。
 * \endif
 */
void RibbonTheme::setCustomPaletteSource(const QUrl& source)
{
    if (mCustomPaletteSource == source) {
        return;
    }
    mCustomPaletteSource = source;
    Q_EMIT customPaletteSourceChanged();
    if (source.isEmpty()) {
        return;
    }
    loadPaletteFromFile(urlToLoadablePath(source));
}

bool RibbonTheme::hasCustomPalette() const
{
    return mHasCustomPalette;
}

bool RibbonTheme::isSystemDarkMode() const
{
    return SA::isOperatingSystemInDarkMode();
}

bool RibbonTheme::followSystemDarkMode() const
{
    return SA::isEnableSystemDarkModeAutoSwitch();
}

/**
 * \if ENGLISH
 * @brief Enable or disable the construction-time dark theme auto switch
 * @details Thin wrapper over SA::setEnableSystemDarkModeAutoSwitch. Like the widgets
 *          front end, the check itself runs when the ribbon bar is constructed, so
 *          opting out has to happen before the bar exists.
 * \endif
 *
 * \if CHINESE
 * @brief 开启或关闭构造期的暗色主题自动切换
 * @details 对 SA::setEnableSystemDarkModeAutoSwitch 的薄封装。与 widgets 前端一样，
 *          检测本身发生在 ribbon bar 构造时，因此关闭必须在 bar 创建之前完成。
 * \endif
 */
void RibbonTheme::setFollowSystemDarkMode(bool on)
{
    if (SA::isEnableSystemDarkModeAutoSwitch() == on) {
        return;
    }
    SA::setEnableSystemDarkModeAutoSwitch(on);
    Q_EMIT followSystemDarkModeChanged();
}

/**
 * \if ENGLISH
 * @brief Override one key color of the active palette and recompute the derived tokens
 * @details SARibbonThemeData hands out the palette by value, so the mutation always
 *          goes copy -> modify -> setPalette, which is what re-emits paletteChanged.
 *          An empty palette (a bridge used before any theme was applied) is seeded
 *          from the current theme's built-in JSON first, otherwise the key color
 *          would have no derived tokens to recompute.
 * \endif
 *
 * \if CHINESE
 * @brief 覆盖当前调色板的一个键色并重算派生 token
 * @details SARibbonThemeData 按值交出调色板，故修改一律走 拷贝 -> 改写 -> setPalette，
 *          paletteChanged 正是由 setPalette 发出的。若调色板为空（桥在任何主题生效前
 *          就被使用），先按当前主题的内置 JSON 播种，否则键色没有派生 token 可重算。
 * \endif
 */
void RibbonTheme::mutatePalette(const std::function< void(SA::SARibbonThemePalette&) >& fn)
{
    if (coreData()->palette().variables().isEmpty()) {
        applyThemePalette(coreData()->theme());
    }
    SA::SARibbonThemePalette pal = coreData()->palette();
    fn(pal);
    coreData()->setPalette(pal);
    setHasCustomPalette(true);
}

void RibbonTheme::setHasCustomPalette(bool on)
{
    if (mHasCustomPalette == on) {
        return;
    }
    mHasCustomPalette = on;
    Q_EMIT hasCustomPaletteChanged();
}

void RibbonTheme::setAccentColor(const QColor& color)
{
    if (!color.isValid()) {
        qWarning("RibbonTheme::setAccentColor: ignored an invalid color");
        return;
    }
    mutatePalette([&color](SA::SARibbonThemePalette& pal) { pal.setAccentColor(color); });
}

void RibbonTheme::setContentBgColor(const QColor& color)
{
    if (!color.isValid()) {
        qWarning("RibbonTheme::setContentBgColor: ignored an invalid color");
        return;
    }
    mutatePalette([&color](SA::SARibbonThemePalette& pal) { pal.setContentBgColor(color); });
}

void RibbonTheme::setTextColor(const QColor& color)
{
    if (!color.isValid()) {
        qWarning("RibbonTheme::setTextColor: ignored an invalid color");
        return;
    }
    mutatePalette([&color](SA::SARibbonThemePalette& pal) { pal.setTextColor(color); });
}

bool RibbonTheme::loadPaletteFromJson(const QString& json)
{
    SA::SARibbonThemePalette pal;
    if (!pal.loadFromJson(json.toUtf8())) {
        qWarning("RibbonTheme::loadPaletteFromJson: invalid JSON (%d characters), palette unchanged", int(json.size()));
        return false;
    }
    coreData()->setPalette(pal);
    setHasCustomPalette(true);
    return true;
}

bool RibbonTheme::loadPaletteFromFile(const QString& path)
{
    SA::SARibbonThemePalette pal;
    if (!pal.loadFromFile(path)) {
        qWarning("RibbonTheme::loadPaletteFromFile: cannot load \"%s\", palette unchanged", qPrintable(path));
        return false;
    }
    coreData()->setPalette(pal);
    setHasCustomPalette(true);
    return true;
}

QColor RibbonTheme::paletteColor(const char* tokenName) const
{
    const QString raw = coreData()->palette().rawValue(tokenName);
    QColor c(raw);
    if (c.isValid()) {
        return c;
    }
    // QML-only apps never run the widgets theme manager, so the core palette
    // can stay empty: fall back to the application palette for the core token
    // names, keeping the leaves literal-color-free (plan-04 S3 theme rule)
    if (QGuiApplication::instance()) {
        const QPalette pal = QGuiApplication::palette();
        const QString name = QLatin1String(tokenName);
        if (name == QLatin1String("text-color")) {
            return pal.color(QPalette::Active, QPalette::Text);
        }
        if (name == QLatin1String("content-bg")) {
            return pal.color(QPalette::Active, QPalette::Window);
        }
        if (name == QLatin1String("accent")) {
            return pal.color(QPalette::Active, QPalette::Highlight);
        }
        if (name == QLatin1String("accent-text")) {
            return pal.color(QPalette::Active, QPalette::HighlightedText);
        }
    }
    return QColor();
}

QColor RibbonTheme::tokenColor(const QString& name) const
{
    return paletteColor(name.toUtf8().constData());
}

QColor RibbonTheme::accent() const
{
    return paletteColor("accent");
}

QColor RibbonTheme::textColor() const
{
    return paletteColor("text-color");
}

QColor RibbonTheme::contentBg() const
{
    return paletteColor("content-bg");
}

QColor RibbonTheme::contentHoverBg() const
{
    return paletteColor("content-hover-bg");
}

QColor RibbonTheme::contentPressedBg() const
{
    return paletteColor("content-pressed-bg");
}

QColor RibbonTheme::subtitle() const
{
    return paletteColor("subtitle");
}

QColor RibbonTheme::separator() const
{
    return paletteColor("separator");
}

QColor RibbonTheme::borderColor() const
{
    return paletteColor("border-color");
}

QColor RibbonTheme::tabAccent() const
{
    return paletteColor("tab-accent");
}

QColor RibbonTheme::tabAccentHover() const
{
    return paletteColor("tab-accent-hover");
}

QColor RibbonTheme::accentHover() const
{
    return paletteColor("accent-hover");
}

QColor RibbonTheme::accentPressed() const
{
    return paletteColor("accent-pressed");
}

QColor RibbonTheme::menuBorder() const
{
    return paletteColor("menu-border");
}

QColor RibbonTheme::inputBorder() const
{
    return paletteColor("input-border");
}

QColor RibbonTheme::inputFocus() const
{
    return paletteColor("input-focus");
}

QColor RibbonTheme::selectionBg() const
{
    return paletteColor("selection-bg");
}

QColor RibbonTheme::sysButtonHover() const
{
    return paletteColor("sys-button-hover");
}

QColor RibbonTheme::sysButtonPressed() const
{
    return paletteColor("sys-button-pressed");
}

}
