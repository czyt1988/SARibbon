#ifndef SARIBBONAPPLICATIONWIDGET_H
#define SARIBBONAPPLICATIONWIDGET_H
#include "SARibbonGlobal.h"
#include <QFrame>
class QShowEvent;
class QKeyEvent;
class SARibbonMainWindow;
/**
 * \if ENGLISH
 * @brief Widget for application button popup
 * @details This widget is used to display a file window similar to Office software when the application button is clicked. It covers the entire MainWindow and handles some border messages.
 * \endif
 * 
 * \if CHINESE
 * @brief 应用程序按钮弹出窗口
 * @details 这个窗口是用于applicationButton点击后弹出类似office系列软件的文件窗口，此窗口会把整个MainWindow覆盖，同时会接管边框的一些消息
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonApplicationWidget : public QFrame
{
    Q_OBJECT
public:
    // Constructor for SARibbonApplicationWidget
    SARibbonApplicationWidget(SARibbonMainWindow* parent);

protected:
    // Resizes the widget to match the parent's size
    virtual void resizeToParent(const QSize& parentSize);

protected:
    // Event filter to handle parent window events
    bool eventFilter(QObject* obj, QEvent* ev) override;
    // Handles the show event
    void showEvent(QShowEvent* event) override;
    // Handles key press events
    void keyPressEvent(QKeyEvent* ev) override;
};

#endif  // SARIBBONAPPLICATIONWIDGET_H
