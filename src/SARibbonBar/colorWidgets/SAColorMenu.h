﻿#ifndef SACOLORMENU_H
#define SACOLORMENU_H
#include <QMenu>
#include "SAColorWidgetsGlobal.h"
class QWidgetAction;
class SAColorPaletteGridWidget;
class SAColorGridWidget;
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
    QWidgetAction* getThemeColorsPaletteAction() const;
    // CustomColorsWidget对应的action
    QWidgetAction* getCustomColorsWidgetAction() const;
    // 自定义颜色action
    QAction* getCustomColorAction() const;
    //获取ThemeColorsPalette
    SAColorPaletteGridWidget* getColorPaletteGridWidget() const;
    //获取自定义颜色grid
    SAColorGridWidget* getCustomColorsWidget() const;
    //构建无颜色action，默认无颜色action是没有的
    void enableNoneColorAction(bool on = true);
    //获取None Color Action,注意，enableNoneColorAction(true),之后才不是nullptr
    QAction* getNoneColorAction() const;
public slots:
    //这是一个辅助槽函数，为了让用户自定义的其他action也能关联menu，可以调用此槽函数，实现selectedColor信号以及menu的隐藏
    void emitSelectedColor(const QColor& c);
signals:
    /**
     * @brief 选择了颜色触发的信号
     * @param c
     */
    void selectedColor(const QColor& c);
private slots:
    void onCustomColorActionTriggered(bool on);
    void onNoneColorActionTriggered(bool on);

private:
    void init(const QList< QColor >& themeCls);
};

#endif  // SACOLORMENU_H
