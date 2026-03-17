#ifndef SARIBBONCTROLCONTAINER_H
#define SARIBBONCTROLCONTAINER_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QScopedPointer>
class QStyleOption;

/**
 * \if ENGLISH
 * @brief Container for holding a widget with optional icon and title
 * @details This container fills the entire SARibbonCtrlContainer but reserves space for displaying an icon or title
 * @par Layout:
 * ----------------------
 * |icon|text|  widget  |
 * ----------------------
 * \endif
 *
 * \if CHINESE
 * @brief 用于装载一个窗体的容器，这个窗体会布满整个SARibbonCtrlContainer，但会预留空间显示icon或者title
 * @par 布局:
 * ----------------------
 * |icon|text|  widget  |
 * ----------------------
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCtrlContainer : public QWidget
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonCtrlContainer)
public:
    /// Constructor for SARibbonCtrlContainer
    explicit SARibbonCtrlContainer(QWidget* parent = nullptr);
    /// Destructor for SARibbonCtrlContainer
    ~SARibbonCtrlContainer();

    /// Enable or disable icon display
    void setEnableShowIcon(bool b);
    /// Enable or disable title display
    void setEnableShowTitle(bool b);
    /// Check if container widget exists
    bool hasContainerWidget() const;
    /// Set the icon
    void setIcon(const QIcon& i);
    /// Set the icon from pixmap
    void setIcon(const QPixmap& pixmap);
    /// Get the icon
    QIcon icon() const;
    /// Set the text
    void setText(const QString& t);
    /// Get the text
    QString text() const;
    /// Set the container widget
    void setContainerWidget(QWidget* w);
    /// Get the container widget
    QWidget* containerWidget();
    /// Get the container widget (const)
    const QWidget* containerWidget() const;
    /// Get the widget displaying the icon
    QWidget* iconWidget() const;
};

#endif  // SARIBBONCTROLCONTAINER_H
