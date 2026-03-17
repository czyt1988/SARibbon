#ifndef SACOLORMENU_H
#define SACOLORMENU_H
#include <QMenu>
#include "SAColorWidgetsGlobal.h"
class QWidgetAction;
class SAColorPaletteGridWidget;
class SAColorGridWidget;
class SAColorToolButton;
/**
 * \if ENGLISH
 * @brief Standard color menu
 * \endif
 *
 * \if CHINESE
 * @brief 标准颜色菜单
 * \endif
 */
class SA_COLOR_WIDGETS_API SAColorMenu : public QMenu
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorMenu)
public:
    /// Constructor for SAColorMenu
    explicit SAColorMenu(QWidget* parent = nullptr);
    /// Constructor for SAColorMenu with title
    explicit SAColorMenu(const QString& title, QWidget* parent = nullptr);
    /// Destructor for SAColorMenu
    ~SAColorMenu();
    /// Quick bind to color button
    void bindToColorToolButton(SAColorToolButton* btn);
    /// Get theme colors palette action
    QWidgetAction* themeColorsPaletteAction() const;
    /// Get custom colors widget action
    QWidgetAction* getCustomColorsWidgetAction() const;
    /// Get custom color action
    QAction* customColorAction() const;
    /// Get theme colors palette widget
    SAColorPaletteGridWidget* colorPaletteGridWidget() const;
    /// Get custom colors widget
    SAColorGridWidget* customColorsWidget() const;
    /// Enable none color action, disabled by default
    void enableNoneColorAction(bool on = true);
    /// Get none color action, returns nullptr until enableNoneColorAction(true) is called
    QAction* noneColorAction() const;
public Q_SLOTS:
    /// Helper slot function to emit selectedColor signal and hide menu
    void emitSelectedColor(const QColor& c);
Q_SIGNALS:
    /**
     * \if ENGLISH
     * @brief Signal emitted when a color is selected
     * @param c Selected color
     * \endif
     *
     * \if CHINESE
     * @brief 选择了颜色触发的信号
     * @param c 选中的颜色
     * \endif
     */
    void selectedColor(const QColor& c);
private Q_SLOTS:
    void onCustomColorActionTriggered(bool on);
    void onNoneColorActionTriggered(bool on);

private:
    void init(const QList< QColor >& themeCls);
};

#endif  // SACOLORMENU_H
