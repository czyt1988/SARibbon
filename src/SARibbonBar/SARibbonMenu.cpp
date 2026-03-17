#include "SARibbonMenu.h"
#include <QWidgetAction>

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonMenu
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonMenu 构造函数
 * @param parent 父窗口
 * \endif
 */
SARibbonMenu::SARibbonMenu(QWidget *parent) : QMenu(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonMenu with title
 * @param title Menu title
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonMenu 构造函数（带标题）
 * @param title 菜单标题
 * @param parent 父窗口
 * \endif
 */
SARibbonMenu::SARibbonMenu(const QString& title, QWidget *parent) : QMenu(title, parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonMenu
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonMenu 析构函数
 * \endif
 */
SARibbonMenu::~SARibbonMenu()
{
}

/**
 * \if ENGLISH
 * @brief Add a SARibbonMenu
 * @param menu SARibbonMenu pointer
 * @return QAction pointer
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个SARibbonMenu
 * @param menu SARibbonMenu指针
 * @return QAction指针
 * \endif
 */
QAction *SARibbonMenu::addRibbonMenu(SARibbonMenu *menu)
{
    return (QMenu::addMenu(menu));
}

/**
 * \if ENGLISH
 * @brief Add a SARibbonMenu with title
 * @param title Menu title
 * @return SARibbonMenu pointer
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个带标题的SARibbonMenu
 * @param title 菜单标题
 * @return SARibbonMenu指针
 * \endif
 */
SARibbonMenu *SARibbonMenu::addRibbonMenu(const QString& title)
{
    SARibbonMenu *menu = new SARibbonMenu(title, this);

    return (menu);
}

/**
 * \if ENGLISH
 * @brief Add a SARibbonMenu with icon and title
 * @param icon Menu icon
 * @param title Menu title
 * @return SARibbonMenu pointer
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个带图标和标题的SARibbonMenu
 * @param icon 菜单图标
 * @param title 菜单标题
 * @return SARibbonMenu指针
 * \endif
 */
SARibbonMenu *SARibbonMenu::addRibbonMenu(const QIcon& icon, const QString& title)
{
    SARibbonMenu *menu = new SARibbonMenu(title, this);

    menu->setIcon(icon);
    return (menu);
}

/**
 * \if ENGLISH
 * @brief Add a widget to the menu
 * @param w Widget to add
 * @return QAction pointer
 * \endif
 *
 * \if CHINESE
 * @brief 向菜单添加一个部件
 * @param w 要添加的部件
 * @return QAction指针
 * \endif
 */
QAction *SARibbonMenu::addWidget(QWidget *w)
{
    QWidgetAction *action = new QWidgetAction(this);

    action->setDefaultWidget(w);
    addAction(action);
    return (action);
}
