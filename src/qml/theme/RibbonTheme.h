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
 * SARibbon::Core::SARibbonThemeData::instance(); palette colors are exposed as
 * NOTIFY properties for QML bindings.
 * \endif
 *
 * \if CHINESE
 * @brief core 主题数据的 QML 单例桥（计划 04 S2）
 * @details 本类不做任何颜色计算——全部转发 SARibbon::Core::SARibbonThemeData::instance()；
 *          调色板颜色以 NOTIFY 属性暴露给 QML 绑定。
 * \endif
 */
class RibbonTheme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)
    Q_PROPERTY(bool dark READ isDark NOTIFY paletteChanged)
public:
    explicit RibbonTheme(QObject* parent = nullptr);
    ~RibbonTheme() override;

    // Singleton (Meyers static; registration via the callback form, plan-04 S1-3)
    static RibbonTheme* instance();

    int currentTheme() const;
    void setCurrentTheme(int theme);

    bool isDark() const;

    // Palette token query for visual leaves: resolved against the current palette
    Q_INVOKABLE QColor tokenColor(const QString& name) const;

Q_SIGNALS:
    void currentThemeChanged();
    void paletteChanged();

private:
    SARibbon::Core::SARibbonThemeData* coreData() const;
};

}
#endif  // RIBBONTHEME_H
