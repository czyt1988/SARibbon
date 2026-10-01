#ifndef RIBBONTHEME_H
#define RIBBONTHEME_H
#include "SARibbonQmlGlobal.h"
#include <SARibbonCore/SARibbonThemeData.h>
#include <QObject>
#include <QColor>

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
 * \endif
 *
 * \if CHINESE
 * @brief core 主题数据的 QML 单例桥（计划 04 S2）
 * @details 本类不做任何颜色计算——全部转发 SARibbon::Core::SARibbonThemeData::instance()；
 *          调色板颜色以 NOTIFY 属性暴露给 QML 绑定。纯 QML 应用不会运行 widgets
 *          主题管理器，故本桥在初始化时加载当前主题的默认调色板 JSON（与 widgets
 *          前端编译的是同一批文件），保证 token 从首帧起可解析；setCurrentTheme
 *          会重新加载对应调色板，对应 applyRibbonTheme 的非 QSS 一半。
 * \endif
 */
class RibbonTheme : public QObject
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

    // Palette token query for visual leaves: resolved against the current palette
    Q_INVOKABLE QColor tokenColor(const QString& name) const;

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

private:
    SARibbon::Core::SARibbonThemeData* coreData() const;
    QColor paletteColor(const char* tokenName) const;
    void applyThemePalette(SARibbonTheme theme);
};

}

#endif  // RIBBONTHEME_H
