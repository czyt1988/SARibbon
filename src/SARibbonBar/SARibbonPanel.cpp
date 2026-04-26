#include "SARibbonPanel.h"
#include "SARibbonCategory.h"
#include "SARibbonElementManager.h"
#include "SARibbonGallery.h"
#include "SARibbonPanelLayout.h"
#include "SARibbonPanelOptionButton.h"
#include "SARibbonToolButton.h"
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFontMetrics>
#include <QGridLayout>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QResizeEvent>
#include <QWidgetAction>

#ifndef SARIBBONPANEL_DEBUG_PRINT
#define SARIBBONPANEL_DEBUG_PRINT 0
#endif

#if SARIBBONPANEL_DEBUG_PRINT
#ifndef SARIBBONPANELLABEL_HELP_DRAW_RECT
#define SARIBBONPANELLABEL_HELP_DRAW_RECT(p, rect)                                                                     \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashLine);                                                                                       \
        _pen.setColor(Qt::blue);                                                                                       \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)
#endif  // SARIBBONPANELLABEL_HELP_DRAW_RECT
#endif
//===============================================================
// SARibbonPanelLabel
//===============================================================

SARibbonPanelLabel::SARibbonPanelLabel(QWidget* parent) : QLabel(parent)
{
}

//===============================================================
// SARibbonPanel::PrivateData
//===============================================================

class SARibbonPanel::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonPanel)
public:
    PrivateData(SARibbonPanel* p);
    void createLayout();
    // 获取layout
    SARibbonPanelLayout* panelLayout() const;
    // 返回最后一个添加的action对应的button，前提是最后一个是toolbutton，否则返回nullptr
    SARibbonToolButton* lastAddActionButton();
    // 重置labe的字体，这个主要是为了让panel的标题字体能适应标题高度
    void resetTitleLabelFont();
    // 标题
    QString panelName() const;
    void setPanelName(const QString& title);

public:
    bool m_isCanCustomize { true };                                                    ///< 记录是否可自定义
    SARibbonPanel::PanelLayoutMode m_panelLayoutMode { SARibbonPanel::ThreeRowMode };  ///< panel 的布局模式，默认为 3 行模式 ThreeRowMode
    SARibbonPanelOptionButton* m_optionActionButton { nullptr };                       ///< 标题栏的 y 距离
    SARibbonPanelLabel* m_label { nullptr };
    bool enableIconRightText { false };                                                ///< 是否启用图标右侧文字模式
};

SARibbonPanel::PrivateData::PrivateData(SARibbonPanel* p) : q_ptr(p)
{
    createLayout();
}

void SARibbonPanel::PrivateData::createLayout()
{
    m_label = RibbonSubElementFactory->createRibbonPanelLabel(q_ptr);
    m_label->setAlignment(Qt::AlignCenter);
    SARibbonPanelLayout* lay = new SARibbonPanelLayout(q_ptr);
    lay->setPanelTitleLabel(m_label);
    lay->setSpacing(2);
    lay->setContentsMargins(2, 2, 2, 2);
}

SARibbonPanelLayout* SARibbonPanel::PrivateData::panelLayout() const
{
    return qobject_cast< SARibbonPanelLayout* >(q_ptr->layout());
}

SARibbonToolButton* SARibbonPanel::PrivateData::lastAddActionButton()
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        return (qobject_cast< SARibbonToolButton* >(lay->lastWidget()));
    }
    return (nullptr);
}

/**
 * @brief 重置label的字体
 *
 * @note 此函数必须在布局设置后调用
 */
void SARibbonPanel::PrivateData::resetTitleLabelFont()
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        int h = lay->panelTitleHeight();
        if (h > 1) {
            QFont f = q_ptr->font();
            f.setPixelSize(h * 0.8);
            if (m_label) {
                m_label->setFont(f);
            }
        }
    }
}

QString SARibbonPanel::PrivateData::panelName() const
{
    if (m_label) {
        return m_label->text();
    }
    return (QString());
}

void SARibbonPanel::PrivateData::setPanelName(const QString& title)
{
    if (m_label) {
        m_label->setText(title);
    }
}

//==================================================
// SARibbonPanel
//==================================================

/**
 * \if ENGLISH
 * @brief Constructs a SARibbonPanel with the given parent
 * @param parent The parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 使用给定的父对象构造SARibbonPanel
 * @param parent 父窗口部件
 * \endif
 */
/**
 * \if ENGLISH
 * @brief Constructor for SARibbonPanel
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonPanel构造函数
 * @param parent 父窗口部件
 * \endif
 */
SARibbonPanel::SARibbonPanel(QWidget* parent) : QFrame(parent), d_ptr(new SARibbonPanel::PrivateData(this))
{
    setPanelLayoutMode(ThreeRowMode);
}

/**
 * \if ENGLISH
 * @brief Constructs a SARibbonPanel with a name and the given parent
 * @param name The panel's display name
 * @param parent The parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 使用名称和给定的父对象构造SARibbonPanel
 * @param name 面板的显示名称
 * @param parent 父窗口部件
 * \endif
 */
SARibbonPanel::SARibbonPanel(const QString& name, QWidget* parent)
    : QFrame(parent), d_ptr(new SARibbonPanel::PrivateData(this))
{
    setPanelLayoutMode(ThreeRowMode);
    setPanelName(name);
}

/**
 * \if ENGLISH
 * @brief Destructor
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * \endif
 */
SARibbonPanel::~SARibbonPanel()
{
}

/**
 * \if ENGLISH
 * @brief Sets the row proportion property for an action
 *
 * This property determines how much vertical space the corresponding button will occupy within the panel.
 * It should be set before adding the action to the panel.
 *
 * @param action The action to modify
 * @param rp The row proportion (Large, Medium, Small)
 * @sa addAction, getActionRowProportionProperty
 * \endif
 *
 * \if CHINESE
 * @brief 为一个action设置行占比属性
 *
 * 此属性决定了对应按钮在面板内占据的垂直空间比例。
 * 应在将action添加到面板之前设置。
 *
 * @param action 要修改的action
 * @param rp 行占比（大、中、小）
 * @sa addAction, getActionRowProportionProperty
 * \endif
 */
void SARibbonPanel::setActionRowProportionProperty(QAction* action, SARibbonPanelItem::RowProportion rp)
{
    Q_CHECK_PTR(action);
    action->setProperty(SA_ActionPropertyName_RowProportion, static_cast< int >(rp));
}

/**
 * \if ENGLISH
 * @brief Gets the row proportion property from an action
 * @param action The action to query
 * @return The row proportion, defaults to Large if not set
 * @sa setActionRowProportionProperty
 * \endif
 *
 * \if CHINESE
 * @brief 从一个action获取行占比属性
 * @param action 要查询的action
 * @return 行占比，如果未设置则默认为Large
 * @sa setActionRowProportionProperty
 * \endif
 */
SARibbonPanelItem::RowProportion SARibbonPanel::getActionRowProportionProperty(QAction* action)
{
    bool isok = false;
    int r     = action->property(SA_ActionPropertyName_RowProportion).toInt(&isok);

    if (isok) {
        return (static_cast< SARibbonPanelItem::RowProportion >(r));
    }
    return (SARibbonPanelItem::Large);
}

/**
 * \if ENGLISH
 * @brief Sets the ToolButtonPopupMode property for an action
 * @param action The action to modify
 * @param popMode The popup mode (e.g., InstantPopup, MenuButtonPopup)
 * @sa getActionToolButtonPopupModeProperty
 * \endif
 *
 * \if CHINESE
 * @brief 为一个action设置ToolButtonPopupMode属性
 * @param action 要修改的action
 * @param popMode 弹出模式（例如，InstantPopup, MenuButtonPopup）
 * @sa getActionToolButtonPopupModeProperty
 * \endif
 */
void SARibbonPanel::setActionToolButtonPopupModeProperty(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    Q_CHECK_PTR(action);
    action->setProperty(SA_ActionPropertyName_ToolButtonPopupMode, static_cast< int >(popMode));
}

/**
 * \if ENGLISH
 * @brief Gets the ToolButtonPopupMode property from an action
 * @param action The action to query
 * @return The popup mode, defaults to InstantPopup if not set
 * @sa setActionToolButtonPopupModeProperty
 * \endif
 *
 * \if CHINESE
 * @brief 从一个action获取ToolButtonPopupMode属性
 * @param action 要查询的action
 * @return 弹出模式，如果未设置则默认为InstantPopup
 * @sa setActionToolButtonPopupModeProperty
 * \endif
 */
QToolButton::ToolButtonPopupMode SARibbonPanel::getActionToolButtonPopupModeProperty(QAction* action)
{
    bool isok = false;
    int r     = action->property(SA_ActionPropertyName_ToolButtonPopupMode).toInt(&isok);

    if (isok) {
        return (static_cast< QToolButton::ToolButtonPopupMode >(r));
    }
    return (QToolButton::InstantPopup);
}

/**
 * \if ENGLISH
 * @brief Sets the ToolButtonStyle property for an action
 * @param action The action to modify
 * @param buttonStyle The button style (e.g., ToolButtonIconOnly, ToolButtonTextBesideIcon)
 * @sa getActionToolButtonStyleProperty
 * \endif
 *
 * \if CHINESE
 * @brief 为一个action设置ToolButtonStyle属性
 * @param action 要修改的action
 * @param buttonStyle 按钮样式（例如，ToolButtonIconOnly, ToolButtonTextBesideIcon）
 * @sa getActionToolButtonStyleProperty
 * \endif
 */
void SARibbonPanel::setActionToolButtonStyleProperty(QAction* action, Qt::ToolButtonStyle buttonStyle)
{
    Q_CHECK_PTR(action);
    action->setProperty(SA_ActionPropertyName_ToolButtonStyle, static_cast< int >(buttonStyle));
}

/**
 * \if ENGLISH
 * @brief Gets the ToolButtonStyle property from an action
 * @param action The action to query
 * @return The button style, defaults to ToolButtonIconOnly if not set
 * @sa setActionToolButtonStyleProperty
 * \endif
 *
 * \if CHINESE
 * @brief 从一个action获取ToolButtonStyle属性
 * @param action 要查询的action
 * @return 按钮样式，如果未设置则默认为ToolButtonIconOnly
 * @sa setActionToolButtonStyleProperty
 * \endif
 */
Qt::ToolButtonStyle SARibbonPanel::getActionToolButtonStyleProperty(QAction* action)
{
    bool isok = false;
    int r     = action->property(SA_ActionPropertyName_ToolButtonStyle).toInt(&isok);

    if (isok) {
        return (static_cast< Qt::ToolButtonStyle >(r));
    }
    return (Qt::ToolButtonIconOnly);
}

/**
 * \if ENGLISH
 * @brief Adds an action to the panel
 *
 * This is the primary method for populating the panel. The button created for this action will use the
 * row proportion and popup mode previously set via the static property functions.
 *
 * @param action The action to add
 * @param rp The row proportion for this action
 * @sa addLargeAction, addMediumAction, addSmallAction
 * \endif
 *
 * \if CHINESE
 * @brief 向面板添加一个action
 *
 * 这是填充面板的主要方法。为此action创建的按钮将使用之前通过静态属性函数设置的行占比和弹出模式。
 *
 * @param action 要添加的action
 * @param rp 此action的行占比
 * @sa addLargeAction, addMediumAction, addSmallAction
 * \endif
 */
void SARibbonPanel::addAction(QAction* action, SARibbonPanelItem::RowProportion rowProportion)
{
    Q_CHECK_PTR(action);
    setActionRowProportionProperty(action, rowProportion);
    addAction(action);
}

/**
 * \if ENGLISH
 * @brief Adds an action with a specified popup mode
 * @param act The action to add
 * @param popMode The popup mode for the button
 * @param rp The row proportion for this action
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个具有指定弹出模式的action
 * @param act 要添加的action
 * @param popMode 按钮的弹出模式
 * @param rp 此action的行占比
 * \endif
 */
void SARibbonPanel::addAction(QAction* act,
                              QToolButton::ToolButtonPopupMode popMode,
                              SARibbonPanelItem::RowProportion rowProportion)
{
    Q_CHECK_PTR(act);
    setActionRowProportionProperty(act, rowProportion);
    setActionToolButtonPopupModeProperty(act, popMode);
    addAction(act);
}

/**
 * \if ENGLISH
 * @brief Adds a large action (occupies full height)
 * @param action The action to add
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个大action（占据整个高度）
 * @param action 要添加的action
 * \endif
 */
void SARibbonPanel::addLargeAction(QAction* action)
{
    addAction(action, SARibbonPanelItem::Large);
}

/**
 * \if ENGLISH
 * @brief Adds a medium action
 *
 * In ThreeRowMode, a medium action occupies two rows. In TwoRowMode, it is equivalent to a small action.
 *
 * @param action The action to add
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个中等action
 *
 * 在三行模式下，中等action占据两行。在两行模式下，它等同于一个小action。
 *
 * @param action 要添加的action
 * \endif
 */
void SARibbonPanel::addMediumAction(QAction* action)
{
    addAction(action, SARibbonPanelItem::Medium);
}

/**
 * \if ENGLISH
 * @brief Adds a small action
 * @param action The action to add
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个小action
 * @param action 要添加的action
 * \endif
 */
void SARibbonPanel::addSmallAction(QAction* action)
{
    addAction(action, SARibbonPanelItem::Small);
}

/**
 * \if ENGLISH
 * @brief Adds a small action with a specified popup mode
 * @param action The action to add
 * @param popMode The popup mode for the button
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个具有指定弹出模式的小action
 * @param action 要添加的action
 * @param popMode 按钮的弹出模式
 * \endif
 */
void SARibbonPanel::addSmallAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    addAction(action, popMode, SARibbonPanelItem::Small);
}

/**
 * \if ENGLISH
 * @brief Adds a large action with a specified popup mode
 * @param action The action to add
 * @param popMode The popup mode for the button
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个具有指定弹出模式的大action
 * @param action 要添加的action
 * @param popMode 按钮的弹出模式
 * \endif
 */
void SARibbonPanel::addLargeAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    addAction(action, popMode, SARibbonPanelItem::Large);
}

/**
 * \if ENGLISH
 * @brief Adds a medium action with a specified popup mode
 * @param action The action to add
 * @param popMode The popup mode for the button
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个具有指定弹出模式的中等action
 * @param action 要添加的action
 * @param popMode 按钮的弹出模式
 * \endif
 */
void SARibbonPanel::addMediumAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    addAction(action, popMode, SARibbonPanelItem::Medium);
}

/**
 * \if ENGLISH
 * @brief Creates and adds a new action
 *
 * This is a convenience function that creates a new `QAction`, sets its properties, and adds it to the panel.
 * The panel will take ownership of the action.
 *
 * @param text The action's text
 * @param icon The action's icon
 * @param popMode The button's popup mode
 * @param rp The action's row proportion
 * @return The newly created action
 * \endif
 *
 * \if CHINESE
 * @brief 创建并添加一个新的action
 *
 * 这是一个便捷函数，用于创建一个新的 `QAction`，设置其属性，并将其添加到面板。
 * 面板将拥有此action的所有权。
 *
 * @param text action的文字
 * @param icon action的图标
 * @param popMode 按钮的弹出模式
 * @param rp action的行占比
 * @return 新创建的action
 * \endif
 */
QAction* SARibbonPanel::addAction(const QString& text,
                                  const QIcon& icon,
                                  QToolButton::ToolButtonPopupMode popMode,
                                  SARibbonPanelItem::RowProportion rowProportion)
{
    QAction* action = new QAction(icon, text, this);
    addAction(action, popMode, rowProportion);
    return (action);
}

/**
 * \if ENGLISH
 * @brief Adds a menu to the panel
 * @param menu The menu to add
 * @param rp The row proportion for the menu button
 * @param popMode The popup mode, defaults to InstantPopup
 * @note This function will modify some properties of menu->menuAction() (icon/text/objectName). If you need to preserve
 * custom values, please set them in advance.
 * \endif
 *
 * \if CHINESE
 * @brief 向面板添加一个菜单
 * @param menu 要添加的菜单
 * @param rp 菜单按钮的行占比
 * @param popMode 弹出模式，默认为InstantPopup
 * @note 本函数会修改 menu->menuAction() 的部分属性（icon/text/objectName），如需保留自定义值，请提前设置。
 * \endif
 */
void SARibbonPanel::addMenu(QMenu* menu, SARibbonPanelItem::RowProportion rowProportion, QToolButton::ToolButtonPopupMode popMode)
{
    Q_CHECK_PTR(menu);
    QAction* action = menu->menuAction();
    // 仅当未设置时才填充默认值，避免覆盖用户自定义
    if (action->icon().isNull()) {
        action->setIcon(menu->icon());
    }
    if (action->text().isEmpty()) {
        action->setText(menu->title());
    }
    if (action->objectName().isEmpty()) {
        action->setObjectName("action." + menu->objectName());
    }
    addAction(action, popMode, rowProportion);
}

/**
 * \if ENGLISH
 * @brief Adds a menu as a large button
 * @param menu The menu to add
 * @param popMode The popup mode
 * \endif
 *
 * \if CHINESE
 * @brief 以大按钮形式添加一个菜单
 * @param menu 要添加的菜单
 * @param popMode 弹出模式
 * \endif
 */
void SARibbonPanel::addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode)
{
    addMenu(menu, SARibbonPanelItem::Large, popMode);
}

/**
 * \if ENGLISH
 * @brief Adds a menu as a medium button
 * @param menu The menu to add
 * @param popMode The popup mode
 * \endif
 *
 * \if CHINESE
 * @brief 以中按钮形式添加一个菜单
 * @param menu 要添加的菜单
 * @param popMode 弹出模式
 * \endif
 */
void SARibbonPanel::addMediumMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode)
{
    addMenu(menu, SARibbonPanelItem::Medium, popMode);
}

/**
 * \if ENGLISH
 * @brief Adds a menu as a small button
 * @param menu The menu to add
 * @param popMode The popup mode
 * \endif
 *
 * \if CHINESE
 * @brief 以小按钮形式添加一个菜单
 * @param menu 要添加的菜单
 * @param popMode 弹出模式
 * \endif
 */
void SARibbonPanel::addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode)
{
    addMenu(menu, SARibbonPanelItem::Small, popMode);
}

/**
 * \if ENGLISH
 * @brief Adds a custom widget to the panel
 *
 * The panel will create a `QWidgetAction` to manage the widget. The panel does not take ownership of the widget;
 * its lifetime is managed by its parent or the caller.
 *
 * @param w The widget to add
 * @param rp The row proportion for the widget
 * @return The created QWidgetAction
 * \endif
 *
 * \if CHINESE
 * @brief 向面板添加一个自定义窗口部件
 *
 * 面板将创建一个 `QWidgetAction` 来管理此窗口部件。面板不拥有此窗口部件的所有权；
 * 其生命周期由其父对象或调用者管理。
 *
 * @param w 要添加的窗口部件
 * @param rp 窗口部件的行占比
 * @return 创建的QWidgetAction
 * \endif
 */
QAction* SARibbonPanel::addWidget(QWidget* w, SARibbonPanelItem::RowProportion rowProportion)
{
    QWidgetAction* action = new QWidgetAction(this);

    action->setDefaultWidget(w);

    action->setEnabled(w->isEnabled());
    // 建立 Action -> Widget 的单向同步
    connect(action, &QWidgetAction::changed, this, [ w, action ]() {
        if (w) {
            w->setEnabled(action->isEnabled());
        }
    });
    action->setIcon(w->windowIcon());
    action->setText(w->windowTitle());
    action->setObjectName("action." + w->objectName());
    w->setAttribute(Qt::WA_Hover);
    setActionRowProportionProperty(action, rowProportion);
    addAction(action);
    return (action);
}

/**
 * \if ENGLISH
 * @brief Adds a widget as a small button
 * @param w The widget to add
 * @return The created QWidgetAction
 * \endif
 *
 * \if CHINESE
 * @brief 以小按钮形式添加一个窗口部件
 * @param w 要添加的窗口部件
 * @return 创建的QWidgetAction
 * \endif
 */
QAction* SARibbonPanel::addSmallWidget(QWidget* w)
{
    return (addWidget(w, SARibbonPanelItem::Small));
}

/**
 * \if ENGLISH
 * @brief Adds a widget as a medium button
 * @param w The widget to add
 * @return The created QWidgetAction
 * \endif
 *
 * \if CHINESE
 * @brief 以中等按钮形式添加一个窗口部件
 * @param w 要添加的窗口部件
 * @return 创建的QWidgetAction
 * \endif
 */
QAction* SARibbonPanel::addMediumWidget(QWidget* w)
{
    return (addWidget(w, SARibbonPanelItem::Medium));
}

/**
 * \if ENGLISH
 * @brief Adds a widget as a large button
 * @param w The widget to add
 * @return The created QWidgetAction
 * \endif
 *
 * \if CHINESE
 * @brief 以大按钮形式添加一个窗口部件
 * @param w 要添加的窗口部件
 * @return 创建的QWidgetAction
 * \endif
 */
QAction* SARibbonPanel::addLargeWidget(QWidget* w)
{
    return (addWidget(w, SARibbonPanelItem::Large));
}

/**
 * \if ENGLISH
 * @brief Adds a gallery widget to the panel
 *
 * A gallery is a specialized widget for displaying a collection of items. If `expanding` is true,
 * the panel's horizontal size policy will be set to `Expanding`.
 *
 * @param expanding If true, sets the panel's size policy to Expanding
 * @return The created SARibbonGallery
 * \endif
 *
 * \if CHINESE
 * @brief 向面板添加一个画廊窗口部件
 *
 * 画廊是一个用于显示项目集合的专用窗口部件。如果 `expanding` 为true，
 * 面板的水平尺寸策略将被设置为 `Expanding`。
 *
 * @param expanding 如果为true，将面板的尺寸策略设置为Expanding
 * @return 创建的SARibbonGallery
 * \endif
 */
SARibbonGallery* SARibbonPanel::addGallery(bool expanding)
{
    SARibbonGallery* gallery = RibbonSubElementFactory->createRibbonGallery(this);

    addWidget(gallery, SARibbonPanelItem::Large);
    if (expanding) {
        setExpanding(expanding);
    }
    return (gallery);
}

/**
 * \if ENGLISH
 * @brief Adds a separator
 * @return The created separator action
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个分隔符
 * @return 创建的分隔符action
 * \endif
 */
QAction* SARibbonPanel::addSeparator()
{
    QAction* action = new QAction(this);

    action->setSeparator(true);
    setActionRowProportionProperty(action, SARibbonPanelItem::Large);
    addAction(action);
    return (action);
}

/**
 * \if ENGLISH
 * @brief Retrieves the SARibbonToolButton associated with an action
 * @param action The action to query
 * @return The associated tool button, or nullptr if not found
 * \endif
 *
 * \if CHINESE
 * @brief 获取与action关联的SARibbonToolButton
 * @param action 要查询的action
 * @return 关联的工具按钮，如果未找到则返回nullptr
 * \endif
 */
SARibbonToolButton* SARibbonPanel::actionToRibbonToolButton(QAction* action)
{
    for (auto obj : sa_as_const(children())) {
        if (obj->isWidgetType()) {
            if (SARibbonToolButton* btn = qobject_cast< SARibbonToolButton* >(obj)) {
                if (btn->defaultAction() == action) {
                    return btn;
                }
            }
        }
    }
    return (nullptr);
}

/**
 * \if ENGLISH
 * @brief Returns a list of all SARibbonToolButton children
 * @return A list of pointers to SARibbonToolButton
 * \endif
 *
 * \if CHINESE
 * @brief 返回所有SARibbonToolButton子对象的列表
 * @return SARibbonToolButton指针的列表
 * \endif
 */
QList< SARibbonToolButton* > SARibbonPanel::ribbonToolButtons() const
{
    const QObjectList& objs = children();
    QList< SARibbonToolButton* > res;

    for (QObject* o : objs) {
        SARibbonToolButton* b = qobject_cast< SARibbonToolButton* >(o);
        if (b) {
            res.append(b);
        }
    }
    return (res);
}

/**
 * \if ENGLISH
 * @brief Sets the panel's layout mode
 * @param mode The new layout mode (ThreeRowMode or TwoRowMode)
 * @sa panelLayoutMode
 * \endif
 *
 * \if CHINESE
 * @brief 设置面板的布局模式
 * @param mode 新的布局模式（三行模式或两行模式）
 * @sa panelLayoutMode
 * \endif
 */
void SARibbonPanel::setPanelLayoutMode(SARibbonPanel::PanelLayoutMode mode)
{
    if (d_ptr->m_panelLayoutMode == mode) {
        return;
    }
    d_ptr->m_panelLayoutMode = mode;
    updateItemGeometry();
}

/**
 * \if ENGLISH
 * @brief Gets the current layout mode
 * @return The current layout mode
 * @sa setPanelLayoutMode
 * \endif
 *
 * \if CHINESE
 * @brief 获取当前的布局模式
 * @return 当前的布局模式
 * @sa setPanelLayoutMode
 * \endif
 */
SARibbonPanel::PanelLayoutMode SARibbonPanel::panelLayoutMode() const
{
    return (d_ptr->m_panelLayoutMode);
}

/**
 * \if ENGLISH
 * @brief Sets the option action for the panel
 *
 * The option action is typically displayed as a small button in the panel's title area.
 * Pass `nullptr` to remove the current option action.
 *
 * @param action The option action, or nullptr to remove
 * @sa isHaveOptionAction
 * \endif
 *
 * \if CHINESE
 * @brief 为面板设置选项action
 *
 * 选项action通常显示在面板标题区域的一个小按钮中。
 * 传入 `nullptr` 以移除当前的选项action。
 *
 * @param action 选项action，或传入nullptr以移除
 * @sa isHaveOptionAction
 * \endif
 */
void SARibbonPanel::setOptionAction(QAction* action)
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        lay->setOptionAction(action);
    }
}

/**
 * \if ENGLISH
 * @brief Checks if an option action is set
 * @return true if an option action exists; otherwise false
 * @sa setOptionAction
 * \endif
 *
 * \if CHINESE
 * @brief 检查是否设置了选项action
 * @return 如果存在选项action则返回true；否则返回false
 * @sa setOptionAction
 * \endif
 */
bool SARibbonPanel::isHaveOptionAction() const
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        return lay->isHaveOptionAction();
    }
    return false;
}

/**
 * \if ENGLISH
 * @brief Returns the recommended size for the widget
 * @return The recommended size for the widget
 * \endif
 *
 * \if CHINESE
 * @brief 返回窗口部件的推荐大小
 * @return 窗口部件的推荐大小
 * \endif
 */
QSize SARibbonPanel::sizeHint() const
{
    int shWidth  = 500;
    int shHeight = 100;
    if (QLayout* lay = layout()) {
        QSize laySize = layout()->sizeHint();
        shWidth       = laySize.width();
        shHeight      = laySize.height();
    }
    return QSize(shWidth, shHeight);
}

/**
 * \if ENGLISH
 * @brief Returns the minimum recommended size for the widget
 * @return The minimum recommended size for the widget
 * \endif
 *
 * \if CHINESE
 * @brief 返回窗口部件的最小推荐大小
 * @return 窗口部件的最小推荐大小
 * \endif
 */
QSize SARibbonPanel::minimumSizeHint() const
{
    return (layout()->minimumSize());
}

/**
 * \if ENGLISH
 * @brief Sets the icon size for all tool buttons in the panel
 *
 * This sets the default icon size for newly created buttons and updates the icon size for existing buttons.
 *
 * @param smallSize The new small icon size
 * @param largeSize The new large icon size
 * @sa toolButtonIconSize
 * \endif
 *
 * \if CHINESE
 * @brief 设置面板内所有工具按钮的图标尺寸
 *
 * 此函数设置新创建按钮的默认图标尺寸，并更新现有按钮的图标尺寸。
 *
 * @param smallSize 新的小图标尺寸
 * @param largeSize 新的大图标尺寸
 * @sa toolButtonIconSize
 * \endif
 */
void SARibbonPanel::setToolButtonIconSize(const QSize& smallSize, const QSize& largeSize)
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        lay->setToolButtonIconSize(smallSize, largeSize);
    }
    // 对已经管理的QToolButton设置为iconsize
    iterateButton([ smallSize, largeSize ](SARibbonToolButton* t) -> bool {
        t->setIconSize(smallSize);
        t->setLargeIconSize(largeSize);
        return true;
    });
}

/**
 * \if ENGLISH
 * @brief Gets the current default icon size for tool buttons
 * @return The current icon size (small size, large size)
 * @sa setToolButtonIconSize
 * \endif
 *
 * \if CHINESE
 * @brief 获取工具按钮当前的默认图标尺寸
 * @return 当前的图标尺寸（小图标尺寸，大图标尺寸）
 * @sa setToolButtonIconSize
 * \endif
 */
QPair< QSize, QSize > SARibbonPanel::toolButtonIconSize() const
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        return lay->toolButtonIconSize();
    }
    return QPair< QSize, QSize >();
}

/**
 * \if ENGLISH
 * @brief Sets the large icon size for tool buttons
 * @param largeSize The new large icon size
 * \endif
 *
 * \if CHINESE
 * @brief 设置工具按钮的大图标尺寸
 * @param largeSize 新的大图标尺寸
 * \endif
 */
void SARibbonPanel::setLargeIconSize(const QSize& largeSize)
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        lay->setLargeIconSize(largeSize);
    }
    // 对已经管理的QToolButton设置为iconsize
    iterateButton([ largeSize ](SARibbonToolButton* t) -> bool {
        t->setLargeIconSize(largeSize);
        return true;
    });
}

/**
 * \if ENGLISH
 * @brief Gets the large icon size for tool buttons
 * @return The current large icon size
 * \endif
 *
 * \if CHINESE
 * @brief 获取工具按钮的大图标尺寸
 * @return 当前的大图标尺寸
 * \endif
 */
QSize SARibbonPanel::largeIconSize() const
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        return lay->largeIconSize();
    }
    return QSize();
}

/**
 * \if ENGLISH
 * @brief Sets the small icon size for tool buttons
 * @param smallSize The new small icon size
 * \endif
 *
 * \if CHINESE
 * @brief 设置工具按钮的小图标尺寸
 * @param smallSize 新的小图标尺寸
 * \endif
 */
void SARibbonPanel::setSmallIconSize(const QSize& smallSize)
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        lay->setSmallIconSize(smallSize);
    }
    // 对已经管理的QToolButton设置为iconsize
    iterateButton([ smallSize ](SARibbonToolButton* t) -> bool {
        t->setSmallIconSize(smallSize);
        return true;
    });
}

/**
 * \if ENGLISH
 * @brief Gets the small icon size for tool buttons
 * @return The current small icon size
 * \endif
 *
 * \if CHINESE
 * @brief 获取工具按钮的小图标尺寸
 * @return 当前的小图标尺寸
 * \endif
 */
QSize SARibbonPanel::smallIconSize() const
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        return lay->smallIconSize();
    }
    return QSize();
}

/**
 * \if ENGLISH
 * @brief Iterates over all SARibbonToolButton children
 *
 * Executes the provided function pointer for each `SARibbonToolButton`. If the function returns `false`,
 * iteration stops immediately.
 *
 * Example:
 * @code
 * myPanel->iterateButton([](SARibbonToolButton* btn) -> bool {
 *     btn->setToolTip("Visited");
 *     return true; // Continue to next button
 * });
 * @endcode
 *
 * @param fp A function pointer that takes a SARibbonToolButton* and returns a bool
 * @return true if all buttons were iterated; false if iteration was stopped early
 * \endif
 *
 * \if CHINESE
 * @brief 遍历所有SARibbonToolButton子对象
 *
 * 对每个 `SARibbonToolButton` 执行提供的函数指针。如果函数返回 `false`，
 * 迭代将立即停止。
 *
 * 示例：
 * @code
 * myPanel->iterateButton([](SARibbonToolButton* btn) -> bool {
 *     btn->setToolTip("Visited");
 *     return true; // 继续下一个按钮
 * });
 * @endcode
 *
 * @param fp 一个接受SARibbonToolButton*并返回bool的函数指针
 * @return 如果遍历了所有按钮则返回true；如果提前停止则返回false
 * \endif
 */
bool SARibbonPanel::iterateButton(SARibbonPanel::FpRibbonToolButtonIterate fp) const
{
    const QObjectList& ols = children();
    for (QObject* obj : ols) {
        if (SARibbonToolButton* t = qobject_cast< SARibbonToolButton* >(obj)) {
            if (!fp(t)) {
                return false;
            }
        }
    }
    return true;
}

/**
 * \if ENGLISH
 * @brief Sets the panel to expanding mode
 *
 * In expanding mode, the panel will try to occupy all available horizontal space in its parent layout.
 *
 * @param isExpanding If true, sets the horizontal size policy to Expanding
 * @sa isExpanding
 * \endif
 *
 * \if CHINESE
 * @brief 将面板设置为扩展模式
 *
 * 在扩展模式下，面板将尝试占据其父布局中所有可用的水平空间。
 *
 * @param isExpanding 如果为true，将水平尺寸策略设置为Expanding
 * @sa isExpanding
 * \endif
 */
void SARibbonPanel::setExpanding(bool isExpanding)
{
    setSizePolicy(isExpanding ? QSizePolicy::Expanding : QSizePolicy::Preferred, QSizePolicy::Fixed);
}

/**
 * \if ENGLISH
 * @brief Checks if the panel is in expanding mode
 * @return true if the horizontal size policy is Expanding; otherwise false
 * @sa setExpanding
 * \endif
 *
 * \if CHINESE
 * @brief 检查面板是否处于扩展模式
 * @return 如果水平尺寸策略为Expanding则返回true；否则返回false
 * @sa setExpanding
 * \endif
 */
bool SARibbonPanel::isExpanding() const
{
    QSizePolicy sp = sizePolicy();

    return (sp.horizontalPolicy() == QSizePolicy::Expanding);
}

/**
 * \if ENGLISH
 * @brief Sets the height of the panel's title
 *
 * This function is typically called by `SARibbonBar` and is protected to prevent direct user manipulation.
 *
 * @param h The new title height
 * @sa SARibbonBar::setPanelTitleHeight
 * \endif
 *
 * \if CHINESE
 * @brief 设置面板标题的高度
 *
 * 此函数通常由 `SARibbonBar` 调用，为防止用户直接操作而设为protected。
 *
 * @param h 新的标题高度
 * @sa SARibbonBar::setPanelTitleHeight
 * \endif
 */
void SARibbonPanel::setTitleHeight(int h)
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        lay->setPanelTitleHeight(h);
    }
    d_ptr->resetTitleLabelFont();
}

/**
 * \if ENGLISH
 * @brief Gets the height of the panel's title
 * @return The current title height
 * \endif
 *
 * \if CHINESE
 * @brief 获取面板标题的高度
 * @return 当前的标题高度
 * \endif
 */
int SARibbonPanel::titleHeight() const
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        return lay->panelTitleHeight();
    }
    return -1;
}

/**
 * \if ENGLISH
 * @brief Checks if the panel's title is visible
 * @return true if the title is visible; otherwise false
 * \endif
 *
 * \if CHINESE
 * @brief 检查面板标题是否可见
 * @return 如果标题可见则返回true；否则返回false
 * \endif
 */
bool SARibbonPanel::isEnableShowTitle() const
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        return lay->isEnableShowPanelTitle();
    }
    return false;
}

/**
 * \if ENGLISH
 * @brief Sets whether the panel's title is visible
 *
 * This function is typically called by `SARibbonBar` and is protected to prevent direct user manipulation.
 *
 * @param on If true, the title is visible
 * @sa SARibbonBar::setEnableShowPanelTitle
 * \endif
 *
 * \if CHINESE
 * @brief 设置面板标题是否可见
 *
 * 此函数通常由 `SARibbonBar` 调用，为防止用户直接操作而设为protected。
 *
 * @param on 如果为true，则标题可见
 * @sa SARibbonBar::setEnableShowPanelTitle
 * \endif
 */
void SARibbonPanel::setEnableShowTitle(bool on)
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        return lay->setEnableShowPanelTitle(on);
    }
}

/**
 * \if ENGLISH
 * @brief Gets the index of an action within the panel
 * @param act The action to find
 * @return The index of the action, or -1 if not found
 * \endif
 *
 * \if CHINESE
 * @brief 获取一个action在面板内的索引
 * @param act 要查找的action
 * @return action的索引，如果未找到则返回-1
 * \endif
 */
int SARibbonPanel::actionIndex(QAction* act) const
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        return (lay->indexByAction(act));
    }
    return (-1);
}

/**
 * \if ENGLISH
 * @brief Moves an action to a new position
 * @param from The current index of the action
 * @param to The new index for the action
 * \endif
 *
 * \if CHINESE
 * @brief 将一个action移动到新位置
 * @param from action的当前索引
 * @param to action的新索引
 * \endif
 */
void SARibbonPanel::moveAction(int from, int to)
{
    if (SARibbonPanelLayout* lay = panelLayout()) {
        lay->move(from, to);
        lay->activate();
    }
}

/**
 * \if ENGLISH
 * @brief Checks if customization is allowed
 * @return true if customization is allowed; otherwise false
 * @sa setCanCustomize
 * \endif
 *
 * \if CHINESE
 * @brief 检查是否允许自定义
 * @return 如果允许自定义则返回true；否则返回false
 * @sa setCanCustomize
 * \endif
 */
bool SARibbonPanel::isCanCustomize() const
{
    return (d_ptr->m_isCanCustomize);
}

/**
 * \if ENGLISH
 * @brief Sets whether customization is allowed
 * @param b If true, customization is allowed
 * @sa isCanCustomize
 * \endif
 *
 * \if CHINESE
 * @brief 设置是否允许自定义
 * @param b 如果为true，则允许自定义
 * @sa isCanCustomize
 * \endif
 */
void SARibbonPanel::setCanCustomize(bool b)
{
    d_ptr->m_isCanCustomize = b;
}

/**
 * \if ENGLISH
 * @brief Gets the panel's name
 * @return The panel's name
 * @sa setPanelName
 * \endif
 *
 * \if CHINESE
 * @brief 获取面板的名称
 * @return 面板的名称
 * @sa setPanelName
 * \endif
 */
QString SARibbonPanel::panelName() const
{
    return (d_ptr->panelName());
}

/**
 * \if ENGLISH
 * @brief Sets the panel's name
 *
 * Setting the name will also emit the `panelNameChanged` signal.
 *
 * @param title The new name for the panel
 * @sa panelName
 * \endif
 *
 * \if CHINESE
 * @brief 设置面板的名称
 *
 * 设置名称还会发出 `panelNameChanged` 信号。
 *
 * @param title 面板的新名称
 * @sa panelName
 * \endif
 */
void SARibbonPanel::setPanelName(const QString& title)
{
    QString oldName = panelName();
    if (oldName != title) {
        d_ptr->setPanelName(title);
        Q_EMIT panelNameChanged(title);
    }
}

/**
 * \if ENGLISH
 * @brief Gets the height of the large buttons in this panel
 * @return The height of large buttons
 * \endif
 *
 * \if CHINESE
 * @brief 获取此面板中大按钮的高度
 * @return 大按钮的高度
 * \endif
 */
int SARibbonPanel::largeButtonHeight() const
{
    const QMargins& mag = contentsMargins();
    return height() - mag.top() - mag.bottom() - titleHeight();
}

/**
 * \if ENGLISH
 * @brief Gets the panel's layout object
 * @return A pointer to the SARibbonPanelLayout
 * \endif
 *
 * \if CHINESE
 * @brief 获取面板的布局对象
 * @return 指向SARibbonPanelLayout的指针
 * \endif
 */
SARibbonPanelLayout* SARibbonPanel::panelLayout() const
{
    return d_ptr->panelLayout();
}

/**
 * \if ENGLISH
 * @brief Updates the geometry of all items in the panel
 *
 * This function forces a recalculation of the layout and size hints for all buttons and items.
 *
 * \endif
 *
 * \if CHINESE
 * @brief 更新面板内所有项目的几何形状
 *
 * 此函数强制重新计算所有按钮和项目的布局及尺寸提示。
 *
 * \endif
 */
void SARibbonPanel::updateItemGeometry()
{
#if SARIBBONPANEL_DEBUG_PRINT
    qDebug() << "SARibbonPanel updateItemGeometry,panelName=" << panelName();
#endif
    // 此函数需要添加，否则SARibbonBar::setEnableWordWrap无法刷新按钮
    // resetToolButtonSize();
    if (SARibbonPanelLayout* lay = panelLayout()) {
        lay->updateGeomArray();
    }
}

/**
 * \if ENGLISH
 * @brief Gets the parent SARibbonCategory
 * @return The parent category, or nullptr if not found
 * \endif
 *
 * \if CHINESE
 * @brief 获取父SARibbonCategory
 * @return 父category，如果未找到则返回nullptr
 * \endif
 */
SARibbonCategory* SARibbonPanel::category() const
{
    return qobject_cast< SARibbonCategory* >(parent());
}

/**
 * \if ENGLISH
 * @brief Gets the top-level SARibbonBar
 * @return The ribbon bar, or nullptr if not found
 * \endif
 *
 * \if CHINESE
 * @brief 获取顶层的SARibbonBar
 * @return Ribbon栏，如果未找到则返回nullptr
 * \endif
 */
SARibbonBar* SARibbonPanel::ribbonBar() const
{
    if (SARibbonCategory* c = category()) {
        return c->ribbonBar();
    }
    return nullptr;
}

/**
 * \if ENGLISH
 * @brief Sets the spacing between elements in the panel
 *
 * This function is typically called by `SARibbonBar` and is protected to prevent direct user manipulation.
 *
 * @param n The new spacing value
 * @sa SARibbonBar::setPanelSpacing
 * \endif
 *
 * \if CHINESE
 * @brief 设置面板内元素之间的间距
 *
 * 此函数通常由 `SARibbonBar` 调用，为防止用户直接操作而设为protected。
 *
 * @param n 新的间距值
 * @sa SARibbonBar::setPanelSpacing
 * \endif
 */
void SARibbonPanel::setSpacing(int n)
{
    if (auto lay = layout()) {
        lay->setSpacing(n);
    }
}

/**
 * \if ENGLISH
 * @brief Gets the spacing between elements in the panel
 * @return The current spacing value
 * @sa setSpacing
 * \endif
 *
 * \if CHINESE
 * @brief 获取面板内元素之间的间距
 * @return 当前的间距值
 * @sa setSpacing
 * \endif
 */
int SARibbonPanel::spacing() const
{
    if (auto lay = layout()) {
        return lay->spacing();
    }
    return 0;
}

/**
 * \if ENGLISH
 * @brief Gets the panel's recommended height
 *
 * This is a static helper function used internally for layout calculations.
 *
 * @param fm The font metrics to use for calculation
 * @param layMode The layout mode
 * @param panelTitleHeight The height of the panel's title
 * @return The recommended height
 * \endif
 *
 * \if CHINESE
 * @brief 获取面板的推荐高度
 *
 * 这是一个用于内部布局计算的静态辅助函数。
 *
 * @param fm 用于计算的字体度量
 * @param layMode 布局模式
 * @param panelTitleHeight 面板标题的高度
 * @return 推荐的高度
 * \endif
 */
int SARibbonPanel::panelHeightHint(const QFontMetrics& fm, PanelLayoutMode layMode, int panelTitleHeight)
{
    int textH = fm.lineSpacing();  // 这里用linespace，因为在换行的情况下，行距是不可忽略的，ribbon的大按钮默认是2行
    switch (layMode) {
    case SARibbonPanel::ThreeRowMode: {
        // 5.5=（3*1.6+1） （三行）,1是给paneltitle预留的
        return textH * 4.8 + panelTitleHeight;
    } break;
    case SARibbonPanel::TwoRowMode: {
        // 3=2*1.6
        return textH * 3.2 + panelTitleHeight;
    } break;
    default: {
        qWarning() << "unknow SARibbonPanel::PanelLayoutMode:" << layMode;
    }
    }
    return (textH * 4.8 + panelTitleHeight);
}

/**
 * \if ENGLISH
 * @brief Resets the size of buttons, after layout changes (especially height changes), the button size needs to be manually changed
 * \endif
 *
 * \if CHINESE
 * @brief 重置按钮的尺寸，在布局改变后（尤其高度变更），按钮的尺寸需要手动变更
 * \endif
 */
void SARibbonPanel::resetToolButtonSize()
{
    QList< SARibbonToolButton* > btns = ribbonToolButtons();

    for (SARibbonToolButton* b : sa_as_const(btns)) {
        if ((nullptr == b)) {
            continue;
        }
        b->invalidateSizeHint();
    }
}

/**
 * \if ENGLISH
 * @brief Checks if the panel is in two-row mode
 * @return true if the panel is in two-row mode, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 检查面板是否处于两行模式
 * @return 如果面板处于两行模式返回true，否则返回false
 * \endif
 */
bool SARibbonPanel::isTwoRow() const
{
    return (TwoRowMode == panelLayoutMode());
}

/**
 * \if ENGLISH
 * @brief Handles action events (add, remove, change)
 *
 * This function is called by Qt when actions are added, removed, or changed. It ensures the panel's
 * layout is updated accordingly.
 *
 * @param e The action event
 * \endif
 *
 * \if CHINESE
 * @brief 处理action事件（添加、移除、更改）
 *
 * 当action被添加、移除或更改时，Qt会调用此函数。它确保面板的布局相应更新。
 *
 * @param e action事件
 * \endif
 */
void SARibbonPanel::actionEvent(QActionEvent* e)
{
    QAction* action             = e->action();
    QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(action);

    switch (e->type()) {
    case QEvent::ActionAdded: {
        SARibbonPanelLayout* lay = panelLayout();
        if (nullptr != widgetAction) {
            if (widgetAction->parent() != this) {
                widgetAction->setParent(this);
            }
        }
        int index = lay->count();
        if (e->before()) {
            // 说明是插入
            index = lay->indexByAction(e->before());
            if (-1 == index) {
                index = lay->count();  // 找不到的时候就插入到最后
            }
        }
        lay->insertAction(index, action, getActionRowProportionProperty(action));
        // 通知父布局这个控件的尺寸提示(sizeHint())可能已改变
        updateGeometry();
    } break;

    case QEvent::ActionChanged: {
        // 让布局重新绘制
        // 通知父布局这个控件的尺寸提示(sizeHint())可能已改变
        updateGeometry();
        // 只处理 QWidgetAction 的情况
        if (QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(e->action())) {
            // 安全获取关联控件
            if (QWidget* w = widgetAction->defaultWidget()) {
                w->setEnabled(widgetAction->isEnabled());
            }
        }
    } break;

    case QEvent::ActionRemoved: {
        SARibbonPanelLayout* lay = panelLayout();
        action->disconnect(this);
        int index = lay->indexByAction(action);
        if (index != -1) {
            QLayoutItem* item = lay->takeAt(index);
            delete item;
        }
        updateGeometry();
    } break;

    default:
        break;
    }
}

/**
 * \if ENGLISH
 * @brief Handles change events (e.g., font change)
 *
 * This function is called when properties like the font change. It propagates the change to child
 * widgets and invalidates the layout.
 *
 * @param e The change event
 * \endif
 *
 * \if CHINESE
 * @brief 处理更改事件（例如，字体更改）
 *
 * 当字体等属性更改时会调用此函数。它将更改传播到子窗口部件并使布局失效。
 *
 * @param e 更改事件
 * \endif
 */
void SARibbonPanel::changeEvent(QEvent* e)
{
    if (nullptr == e) {
        return;
    }
    switch (e->type()) {
    case QEvent::FontChange: {
        if (d_ptr->m_label) {
            d_ptr->m_label->setFont(font());
        }
        if (QLayout* lay = layout()) {
            lay->invalidate();
        }
        d_ptr->resetTitleLabelFont();
    } break;
    case QEvent::LayoutDirectionChange: {
        // 布局方向改变（如 LTR→RTL），失效布局并触发重绘
        if (QLayout* lay = layout()) {
            lay->invalidate();
        }
        update();
    } break;
    default:
        break;
    }
    QWidget::changeEvent(e);
}

/**
 * \if ENGLISH
 * @brief Gets the layout items
 * @return Reference to the list of SARibbonPanelItem
 * \endif
 *
 * \if CHINESE
 * @brief 获取布局item
 * @return SARibbonPanelItem的列表引用
 * \endif
 */
const QList< SARibbonPanelItem* >& SARibbonPanel::ribbonPanelItem() const
{
    return (panelLayout()->mItems);
}

/**
 * \if ENGLISH
 * @brief Gets the last added button
 * @return Pointer to the last added button
 * \endif
 *
 * \if CHINESE
 * @brief 获取最近添加的按钮
 * @return 刚才添加的按钮的指针
 * \endif
 */
SARibbonToolButton* SARibbonPanel::lastAddActionButton()
{
    return d_ptr->lastAddActionButton();
}

/**
 * \if ENGLISH
 * @brief Gets the title label widget
 * @return A pointer to the SARibbonPanelLabel
 * \endif
 *
 * \if CHINESE
 * @brief 获取标题标签窗口部件
 * @return 指向SARibbonPanelLabel的指针
 * \endif
 */
SARibbonPanelLabel* SARibbonPanel::titleLabel() const
{
    return d_ptr->m_label;
}

/**
 * \if ENGLISH
 * @brief Sets whether the panel's button text allows line wrapping
 * @param on If true, enables word wrapping for button text
 * \endif
 *
 * \if CHINESE
 * @brief 设置 panel 的按钮文字允许换行
 * @param on 如果为 true，启用按钮文字换行
 * \endif
 */
void SARibbonPanel::setEnableWordWrap(bool on)
{
    if (SARibbonPanelLayout* lay = qobject_cast< SARibbonPanelLayout* >(layout())) {
        // 此函数会自动设置所有按钮的换行状态
        lay->setEnableWordWrap(on);
        updateGeometry();
    }
}

/**
 * \if ENGLISH
 * @brief Checks if the panel's text allows line wrapping
 * @return true if word wrapping is enabled for button text, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 判断 panel 的文字是否允许换行
 * @return 如果按钮文字换行已启用返回 true，否则返回 false
 * \endif
 */
bool SARibbonPanel::isEnableWordWrap() const
{
    if (SARibbonPanelLayout* lay = qobject_cast< SARibbonPanelLayout* >(layout())) {
        return lay->isEnableWordWrap();
    }
    return false;
}

/**
 * \if ENGLISH
 * @brief Set whether button text is displayed to the right of the icon
 * @param on If true, all toolbuttons will show text to the right of icon
 * @details This cascades to all SARibbonToolButton widgets in this panel.
 *           When enabled, Large buttons are rendered with horizontal layout
 *           (icon-left, text-right) instead of the default vertical layout.
 * \endif
 *
 * \if CHINESE
 * @brief 设置按钮文字是否显示在图标右侧
 * @param on 如果为 true，所有 toolbutton 将显示图标在左、文字在右
 * @details 此属性会级联传递到 panel 中所有 SARibbonToolButton。
 *           启用时，Large 按钮使用水平布局（图标左、文字右）代替默认的垂直布局。
 * \endif
 */
void SARibbonPanel::setEnableIconRightText(bool on)
{
    SA_D(d);
    if (d->enableIconRightText == on) {
        return;
    }
    d->enableIconRightText = on;
    iterateButton([on](SARibbonToolButton* btn) {
        btn->setEnableIconRightText(on);
        return true;
    });
}

/**
 * \if ENGLISH
 * @brief Check if icon-right-text mode is enabled
 * @return true if icon-right-text mode is enabled
 * \endif
 *
 * \if CHINESE
 * @brief 检查图标右侧文字模式是否启用
 * @return 如果启用图标右侧文字模式返回 true
 * \endif
 */
bool SARibbonPanel::isEnableIconRightText() const
{
    SA_DC(d);
    return d->enableIconRightText;
}
}

/**
 * \if ENGLISH
 * @brief Checks if the panel's text allows line wrapping
 * @return true if word wrapping is enabled for button text, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 判断panel的文字是否允许换行
 * @return 如果按钮文字换行已启用返回true，否则返回false
 * \endif
 */
bool SARibbonPanel::isEnableWordWrap() const
{
    if (SARibbonPanelLayout* lay = qobject_cast< SARibbonPanelLayout* >(layout())) {
        return lay->isEnableWordWrap();
    }
    return false;
}

/**
 * \if ENGLISH
 * @brief Sets the button maximum aspect ratio, this coefficient determines the maximum width of the button
 *
 * The maximum width of the button is button height * this coefficient. For example, if the button height is h,
 * then the maximum button width maxw = h * buttonMaximumAspectRatio
 * If the text cannot be fully displayed at this width, the button will not continue to expand horizontally,
 * and ... will be used instead of the incompletely displayed text
 *
 * @see buttonMaximumAspectRatio
 *
 * @note Users should not call @ref SARibbonPanel::setButtonMaximumAspectRatio to set,
 * but call @ref SARibbonBar::setButtonMaximumAspectRatio to set the aspect ratio
 *
 * @param fac New maximum aspect ratio
 * \endif
 *
 * \if CHINESE
 * @brief 设置按钮最大宽高比，这个系数决定按钮的最大宽度
 *
 * 按钮的最大宽度为按钮高度*此系数，例如按钮高度为h，那么按钮最大宽度maxw=h*buttonMaximumAspectRatio
 * 如果在此宽度下文字还无法完全显示，那么按钮将不会继续横向扩展，将使用...替代未完全显示的文字
 *
 * @see buttonMaximumAspectRatio
 *
 * @note 用户不应该调用@ref SARibbonPanel::setButtonMaximumAspectRatio 来设置，
 * 而是调用@ref SARibbonBar::setButtonMaximumAspectRatio 设置宽高比
 *
 * @param fac 新的最大宽高比
 * \endif
 */
void SARibbonPanel::setButtonMaximumAspectRatio(qreal fac)
{
    if (SARibbonPanelLayout* lay = qobject_cast< SARibbonPanelLayout* >(layout())) {
        // 此函数会自动设置所有按钮的换行状态
        lay->setButtonMaximumAspectRatio(fac);
    }
}

/**
 * \if ENGLISH
 * @brief Button maximum aspect ratio, this coefficient determines the maximum width of the button
 * @return Current button maximum aspect ratio
 * @see setButtonMaximumAspectRatio
 * \endif
 *
 * \if CHINESE
 * @brief 按钮最大宽高比，这个系数决定按钮的最大宽度
 * @return 按钮最大宽高比
 * @see setButtonMaximumAspectRatio
 * \endif
 */
qreal SARibbonPanel::buttonMaximumAspectRatio() const
{
    if (SARibbonPanelLayout* lay = qobject_cast< SARibbonPanelLayout* >(layout())) {
        return lay->buttonMaximumAspectRatio();
    }
    return 1.4;
}
