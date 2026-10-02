#ifndef RIBBONTHEME_H
#define RIBBONTHEME_H
#include "SARibbonQmlGlobal.h"
#include <SARibbonCore/SARibbonThemeData.h>
#include <QObject>
#include <QColor>
#include <QUrl>
#include <functional>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief QML singleton bridge over the core theme data (plan-04 S2)
 * @details No color computation here — everything delegates to
 * SARibbon::Core::SARibbonThemeData::instance(); palette colors are exposed
 * as NOTIFY properties for QML bindings. Because a QML-only app never runs
 * the widgets theme manager, this bridge loads the default palette JSON of
 * the current theme (same files the widgets front end compiles in) so the
 * tokens resolve from the first frame; setCurrentTheme re-loads the
 * matching palette, mirroring applyRibbonTheme's non-QSS half.
 * On top of the read-only token view this bridge is the QML customization
 * entry point: setAccentColor/setContentBgColor/setTextColor override single
 * key colors of the loaded palette (the derived tokens are recomputed by
 * core), and loadPaletteFromJson/loadPaletteFromFile/customPaletteSource
 * install a whole user palette. RibbonThemeUserDefine deliberately has no
 * built-in palette JSON, so selecting it keeps whatever the user loaded —
 * hasCustomPalette tells the caller whether the colors are really custom or
 * leftovers of the previous built-in theme.
 * \endif
 *
 * \if CHINESE
 * @brief core 主题数据的 QML 单例桥（计划 04 S2）
 * @details 本类不做任何颜色计算——全部转发 SARibbon::Core::SARibbonThemeData::instance()；
 *          调色板颜色以 NOTIFY 属性暴露给 QML 绑定。纯 QML 应用不会运行 widgets
 *          主题管理器，故本桥在初始化时加载当前主题的默认调色板 JSON（与 widgets
 *          前端编译的是同一批文件），保证 token 从首帧起可解析；setCurrentTheme
 *          会重新加载对应调色板，对应 applyRibbonTheme 的非 QSS 一半。
 *          除只读的 token 视图外，本桥还是 QML 侧的主题自定义入口：
 *          setAccentColor/setContentBgColor/setTextColor 覆盖已加载调色板的单个键色
 *          （派生 token 由 core 重算），loadPaletteFromJson/loadPaletteFromFile/
 *          customPaletteSource 则整体装入用户调色板。RibbonThemeUserDefine 刻意没有
 *          内置调色板 JSON，选中它会保留用户已加载的调色板——hasCustomPalette 用于
 *          告知调用方当前色值究竟是用户自定义的，还是上一个内置主题的遗留。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonTheme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)
    Q_PROPERTY(bool dark READ isDark NOTIFY paletteChanged)
    Q_PROPERTY(QColor accent READ accent NOTIFY paletteChanged)
    Q_PROPERTY(QColor textColor READ textColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor contentBg READ contentBg NOTIFY paletteChanged)
    Q_PROPERTY(QColor contentHoverBg READ contentHoverBg NOTIFY paletteChanged)
    Q_PROPERTY(QColor contentPressedBg READ contentPressedBg NOTIFY paletteChanged)
    Q_PROPERTY(QColor subtitle READ subtitle NOTIFY paletteChanged)
    Q_PROPERTY(QColor separator READ separator NOTIFY paletteChanged)
    Q_PROPERTY(QColor borderColor READ borderColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor tabAccent READ tabAccent NOTIFY paletteChanged)
    Q_PROPERTY(QColor tabAccentHover READ tabAccentHover NOTIFY paletteChanged)
    Q_PROPERTY(QColor accentHover READ accentHover NOTIFY paletteChanged)
    Q_PROPERTY(QColor accentPressed READ accentPressed NOTIFY paletteChanged)
    Q_PROPERTY(QColor menuBorder READ menuBorder NOTIFY paletteChanged)
    Q_PROPERTY(QColor inputBorder READ inputBorder NOTIFY paletteChanged)
    Q_PROPERTY(QColor inputFocus READ inputFocus NOTIFY paletteChanged)
    Q_PROPERTY(QColor selectionBg READ selectionBg NOTIFY paletteChanged)
    Q_PROPERTY(QColor sysButtonHover READ sysButtonHover NOTIFY paletteChanged)
    Q_PROPERTY(QColor sysButtonPressed READ sysButtonPressed NOTIFY paletteChanged)
    Q_PROPERTY(bool rtl READ isRtl WRITE setRtl NOTIFY rtlChanged)
    Q_PROPERTY(QUrl customPaletteSource READ customPaletteSource WRITE setCustomPaletteSource NOTIFY customPaletteSourceChanged)
    Q_PROPERTY(bool hasCustomPalette READ hasCustomPalette NOTIFY hasCustomPaletteChanged)
    Q_PROPERTY(bool systemDarkMode READ isSystemDarkMode NOTIFY systemDarkModeChanged)
    Q_PROPERTY(bool followSystemDarkMode READ followSystemDarkMode WRITE setFollowSystemDarkMode NOTIFY followSystemDarkModeChanged)
public:
    explicit RibbonTheme(QObject* parent = nullptr);
    ~RibbonTheme() override;

    // Singleton (Meyers static; registration via the callback form, plan-04 S1-3)
    static RibbonTheme* instance();

    // Application layout direction mirror (QGuiApplication::setLayoutDirection;
    // the core engines read it through SA::saIsRTL() — this QML setter lets
    // examples toggle RTL like the widgets example's "Switch to RTL" action)
    bool isRtl() const;
    void setRtl(bool on);

    int currentTheme() const;
    void setCurrentTheme(int theme);

    bool isDark() const;

    // Declarative palette override: loads the JSON at the URL (qrc or local file)
    QUrl customPaletteSource() const;
    void setCustomPaletteSource(const QUrl& source);

    // True once a user palette is in effect (loaded JSON or an overridden key color)
    bool hasCustomPalette() const;

    // Operating system color scheme (SA::isOperatingSystemInDarkMode)
    bool isSystemDarkMode() const;

    // Construction-time dark theme auto switch flag (SA::setEnableSystemDarkModeAutoSwitch)
    bool followSystemDarkMode() const;
    void setFollowSystemDarkMode(bool on);

    // Palette token query for visual leaves: resolved against the current palette
    Q_INVOKABLE QColor tokenColor(const QString& name) const;

    // Override the accent key color of the current palette (derived tokens follow)
    Q_INVOKABLE void setAccentColor(const QColor& color);

    // Override the content background key color of the current palette
    Q_INVOKABLE void setContentBgColor(const QColor& color);

    // Override the text key color of the current palette
    Q_INVOKABLE void setTextColor(const QColor& color);

    // Load a palette from JSON text; returns false and keeps the old palette on error
    Q_INVOKABLE bool loadPaletteFromJson(const QString& json);

    // Load a palette from a JSON file (qrc or local path); returns false on error
    Q_INVOKABLE bool loadPaletteFromFile(const QString& path);

    // Getters of the common token properties (all resolved against the core palette)
    QColor accent() const;
    QColor textColor() const;
    QColor contentBg() const;
    QColor contentHoverBg() const;
    QColor contentPressedBg() const;
    QColor subtitle() const;
    QColor separator() const;
    QColor borderColor() const;
    QColor tabAccent() const;
    QColor tabAccentHover() const;
    QColor accentHover() const;
    QColor accentPressed() const;
    QColor menuBorder() const;
    QColor inputBorder() const;
    QColor inputFocus() const;
    QColor selectionBg() const;
    QColor sysButtonHover() const;
    QColor sysButtonPressed() const;

Q_SIGNALS:
    void currentThemeChanged();
    void paletteChanged();
    void rtlChanged();

    /**
     * \if ENGLISH
     * @brief The declarative palette source URL changed
     * \endif
     *
     * \if CHINESE
     * @brief 声明式调色板来源 URL 发生变化
     * \endif
     */
    void customPaletteSourceChanged();

    /**
     * \if ENGLISH
     * @brief A user palette took over from (or gave way to) a built-in theme palette
     * \endif
     *
     * \if CHINESE
     * @brief 用户调色板与内置主题调色板之间发生接管/让位
     * \endif
     */
    void hasCustomPaletteChanged();

    /**
     * \if ENGLISH
     * @brief The operating system color scheme changed (Qt 6.5+ only; earlier
     *        versions have no notification source and the property is read once)
     * \endif
     *
     * \if CHINESE
     * @brief 操作系统颜色模式发生变化（仅 Qt 6.5+；更早版本没有通知来源，该属性只读取一次）
     * \endif
     */
    void systemDarkModeChanged();

    /**
     * \if ENGLISH
     * @brief The construction-time dark theme auto switch flag changed
     * \endif
     *
     * \if CHINESE
     * @brief 构造期暗色主题自动切换开关发生变化
     * \endif
     */
    void followSystemDarkModeChanged();

private:
    SARibbon::Core::SARibbonThemeData* coreData() const;
    QColor paletteColor(const char* tokenName) const;
    void applyThemePalette(SARibbonTheme theme);
    void mutatePalette(const std::function< void(SA::SARibbonThemePalette&) >& fn);
    void setHasCustomPalette(bool on);

    QUrl mCustomPaletteSource;
    bool mHasCustomPalette { false };
};

}

#endif  // RIBBONTHEME_H
