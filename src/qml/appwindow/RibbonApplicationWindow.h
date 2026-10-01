#ifndef RIBBONAPPLICATIONWINDOW_H
#define RIBBONAPPLICATIONWINDOW_H
#include "SARibbonQmlGlobal.h"
#include <QQuickItem>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Application window host: custom panel behind the application button
 * @details The QML counterpart of the widgets SARibbonApplicationWidget:
 *          a plain structural item the user fills with arbitrary content
 *          (list views, buttons, labels). Declared as a child of the bar, it
 *          shows in a popup below the application button when the button is
 *          clicked (widgets ApplicationWidget mode); Esc and outside clicks
 *          close it (popup semantics), and the close() invokable lets inner
 *          buttons close it programmatically.
 * \endif
 *
 * \if CHINESE
 * @brief 应用窗口宿主：应用按钮背后的自定义面板
 * @details 对应 widgets 侧 SARibbonApplicationWidget：一个透明结构项，
 *          用户在其中声明任意内容（列表、按钮、文本）。声明为 bar 的
 *          子项后，点击应用按钮在按钮下方弹出显示（widgets
 *          ApplicationWidget 模式）；Esc 与点击外部关闭（弹出语义），
 *          close() 可调用方法供内部按钮编程式关闭。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonApplicationWindow : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool popupVisible READ isPopupVisible WRITE setPopupVisible NOTIFY popupVisibleChanged)
public:
    explicit RibbonApplicationWindow(QQuickItem* parent = nullptr);
    ~RibbonApplicationWindow() override;

    // visibility while shown through the application button (tests + leaf)
    bool isPopupVisible() const;
    void setPopupVisible(bool on);

    // close from inside the content (buttons etc.); routes through the bar
    Q_INVOKABLE void close();

Q_SIGNALS:
    void popupVisibleChanged();
    void closeRequested();

private:
    bool mPopupVisible = false;
};

}

#endif  // RIBBONAPPLICATIONWINDOW_H
