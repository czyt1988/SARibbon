#ifndef SARIBBONMENU_H
#define SARIBBONMENU_H
#include "SARibbonGlobal.h"
#include <QMenu>

/**
 * \if ENGLISH
 * @brief Menu used in Ribbon interface
 * Can be styled via StyleSheet without affecting QMenu
 * \endif
 *
 * \if CHINESE
 * @brief 用在ribbon的menu
 * 可以通过StyleSheet设置样式而不影响QMenu
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonMenu : public QMenu
{
    Q_OBJECT
public:
    /// Constructor for SARibbonMenu
    explicit SARibbonMenu(QWidget* parent = nullptr);
    /// Constructor for SARibbonMenu with title
    explicit SARibbonMenu(const QString& title, QWidget* parent = nullptr);
    /// Destructor for SARibbonMenu
    ~SARibbonMenu();
    /// Add a submenu to this menu
    QAction* addRibbonMenu(SARibbonMenu* menu);
    /// Create and add a submenu with the given title
    SARibbonMenu* addRibbonMenu(const QString& title);
    /// Create and add a submenu with the given icon and title
    SARibbonMenu* addRibbonMenu(const QIcon& icon, const QString& title);
    /// Add a custom widget to the menu
    QAction* addWidget(QWidget* w);
};

#endif  // SARIBBONMENU_H
