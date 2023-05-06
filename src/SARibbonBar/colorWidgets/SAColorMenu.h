#ifndef SACOLORMENU_H
#define SACOLORMENU_H
#include <QMenu>
#include "SAColorWidgetsGlobal.h"
class SAColorPaletteGridWidget;
class SAColorToolButton;
/**
 * @brief 标准颜色菜单
 */
class SA_COLOR_WIDGETS_API SAColorMenu : public QMenu
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorMenu)
public:
    explicit SAColorMenu(QWidget* parent = nullptr);
    explicit SAColorMenu(const QString& title, QWidget* parent = nullptr);
    ~SAColorMenu();
    //快速绑定colorbtn
    void bindToColorToolButton(SAColorToolButton* btn);
    // ThemeColorsPalette对应的action
    QAction* getThemeColorsPaletteAction() const;
    // 自定义颜色action
    QAction* getCustomColorAction() const;
    //获取ThemeColorsPalette
    SAColorPaletteGridWidget* getColorPaletteGridWidget() const;
signals:
    /**
     * @brief 选择了颜色触发的信号
     * @param c
     */
    void selectedColor(const QColor& c);
private slots:
    void onCustomColorActionTriggered(bool on);
    void onThemeColorsSelected(const QColor& c);

private:
    void init(const QList< QColor >& themeCls);
};

#endif  // SACOLORMENU_H
