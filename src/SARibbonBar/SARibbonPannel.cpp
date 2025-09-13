#include "SARibbonPannel.h"
#include "SARibbonCategory.h"
#include "SARibbonElementManager.h"
#include "SARibbonGallery.h"
#include "SARibbonPannelLayout.h"
#include "SARibbonPannelOptionButton.h"
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

#ifndef SARIBBONPANNEL_DEBUG_PRINT
#define SARIBBONPANNEL_DEBUG_PRINT 0
#endif

#if SARIBBONPANNEL_DEBUG_PRINT
#ifndef SARIBBONPANNELLABEL_HELP_DRAW_RECT
#define SARIBBONPANNELLABEL_HELP_DRAW_RECT(p, rect)                                                                    \
	do {                                                                                                               \
		p.save();                                                                                                      \
		QPen _pen(Qt::DashLine);                                                                                       \
		_pen.setColor(Qt::blue);                                                                                       \
		p.setPen(_pen);                                                                                                \
		p.setBrush(QBrush());                                                                                          \
		p.drawRect(rect);                                                                                              \
		p.restore();                                                                                                   \
	} while (0)
#endif  // SARIBBONPANNELLABEL_HELP_DRAW_RECT
#endif
//===============================================================
// SARibbonPannelLabel
//===============================================================

SARibbonPannelLabel::SARibbonPannelLabel(QWidget* parent) : QLabel(parent)
{
}

//===============================================================
// SARibbonPannel::PrivateData
//===============================================================

class SARibbonPannel::PrivateData
{
	SA_RIBBON_DECLARE_PUBLIC(SARibbonPannel)
public:
	PrivateData(SARibbonPannel* p);
	// 根据m_pannelLayoutMode返回gridLayout应该增加的行数
	int rowadded();
	void createLayout();
	// 获取layout
	SARibbonPannelLayout* pannelLayout() const;
	// 返回最后一个添加的action对应的button，前提是最后一个是toolbutton，否则返回nullptr
	SARibbonToolButton* lastAddActionButton();
	// 重置labe的字体，这个主要是为了让pannel的标题字体能适应标题高度
	void resetTitleLabelFont();
	// 标题
	QString pannelName() const;
	void setPannelName(const QString& title);

public:
	bool m_isCanCustomize { true };                                                        ///< 记录是否可自定义
	SARibbonPannel::PannelLayoutMode m_pannelLayoutMode { SARibbonPannel::ThreeRowMode };  ///< pannel的布局模式，默认为3行模式ThreeRowMode
	SARibbonPannelOptionButton* m_optionActionButton { nullptr };  ///< 标题栏的y距离
	SARibbonPannelLabel* m_label { nullptr };
};

SARibbonPannel::PrivateData::PrivateData(SARibbonPannel* p) : q_ptr(p)
{
	createLayout();
}

int SARibbonPannel::PrivateData::rowadded()
{
	switch (m_pannelLayoutMode) {
	case SARibbonPannel::ThreeRowMode:
		return (2);

	case SARibbonPannel::TwoRowMode:
		return (3);

	default:
		break;
	}
	return (2);
}

void SARibbonPannel::PrivateData::createLayout()
{
	m_label = new SARibbonPannelLabel(q_ptr);
	m_label->setAlignment(Qt::AlignCenter);
	SARibbonPannelLayout* lay = new SARibbonPannelLayout(q_ptr);
	lay->setPannelTitleLabel(m_label);
	lay->setSpacing(2);
	lay->setContentsMargins(2, 2, 2, 2);
}

SARibbonPannelLayout* SARibbonPannel::PrivateData::pannelLayout() const
{
	return qobject_cast< SARibbonPannelLayout* >(q_ptr->layout());
}

SARibbonToolButton* SARibbonPannel::PrivateData::lastAddActionButton()
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		return (qobject_cast< SARibbonToolButton* >(lay->lastWidget()));
	}
	return (nullptr);
}

/**
 * @brief 重置label的字体
 *
 * @note 此函数必须在布局设置后调用
 */
void SARibbonPannel::PrivateData::resetTitleLabelFont()
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		int h = lay->pannelTitleHeight();
		if (h > 1) {
			QFont f = q_ptr->font();
			f.setPixelSize(h * 0.8);
			if (m_label) {
				m_label->setFont(f);
			}
		}
	}
}

QString SARibbonPannel::PrivateData::pannelName() const
{
	if (m_label) {
		return m_label->text();
	}
	return (QString());
}

void SARibbonPannel::PrivateData::setPannelName(const QString& title)
{
	if (m_label) {
		m_label->setText(title);
	}
}

//==================================================
// SARibbonPannel
//==================================================

/**
 * @brief Constructs a SARibbonPannel with the given parent / 使用给定的父对象构造SARibbonPannel
 * @param parent The parent widget / 父窗口部件
 */
SARibbonPannel::SARibbonPannel(QWidget* parent) : QFrame(parent), d_ptr(new SARibbonPannel::PrivateData(this))
{
	setPannelLayoutMode(ThreeRowMode);
}

/**
 * @brief Constructs a SARibbonPannel with a name and the given parent / 使用名称和给定的父对象构造SARibbonPannel
 * @param name The panel's display name / 面板的显示名称
 * @param parent The parent widget / 父窗口部件
 */
SARibbonPannel::SARibbonPannel(const QString& name, QWidget* parent)
    : QFrame(parent), d_ptr(new SARibbonPannel::PrivateData(this))
{
	setPannelLayoutMode(ThreeRowMode);
	setPannelName(name);
}

SARibbonPannel::~SARibbonPannel()
{
}

/**
 * @brief Sets the row proportion property for an action / 为一个action设置行占比属性
 *
 * This property determines how much vertical space the corresponding button will occupy within the panel.
 * It should be set before adding the action to the panel.
 *
 * 此属性决定了对应按钮在面板内占据的垂直空间比例。
 * 应在将action添加到面板之前设置。
 *
 * @param action The action to modify / 要修改的action
 * @param rp The row proportion (Large, Medium, Small) / 行占比（大、中、小）
 * @sa addAction, getActionRowProportionProperty
 */
void SARibbonPannel::setActionRowProportionProperty(QAction* action, SARibbonPannelItem::RowProportion rp)
{
	Q_CHECK_PTR(action);
	action->setProperty(SA_ActionPropertyName_RowProportion, static_cast< int >(rp));
}

/**
 * @brief Gets the row proportion property from an action / 从一个action获取行占比属性
 * @param action The action to query / 要查询的action
 * @return The row proportion, defaults to Large if not set / 行占比，如果未设置则默认为Large
 * @sa setActionRowProportionProperty
 */
SARibbonPannelItem::RowProportion SARibbonPannel::getActionRowProportionProperty(QAction* action)
{
	bool isok = false;
	int r     = action->property(SA_ActionPropertyName_RowProportion).toInt(&isok);

	if (isok) {
		return (static_cast< SARibbonPannelItem::RowProportion >(r));
	}
	return (SARibbonPannelItem::Large);
}

/**
 * @brief Sets the ToolButtonPopupMode property for an action / 为一个action设置ToolButtonPopupMode属性
 * @param action The action to modify / 要修改的action
 * @param popMode The popup mode (e.g., InstantPopup, MenuButtonPopup) / 弹出模式（例如，InstantPopup, MenuButtonPopup）
 * @sa getActionToolButtonPopupModeProperty
 */
void SARibbonPannel::setActionToolButtonPopupModeProperty(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
	Q_CHECK_PTR(action);
	action->setProperty(SA_ActionPropertyName_ToolButtonPopupMode, static_cast< int >(popMode));
}

/**
 * @brief Gets the ToolButtonPopupMode property from an action / 从一个action获取ToolButtonPopupMode属性
 * @param action The action to query / 要查询的action
 * @return The popup mode, defaults to InstantPopup if not set / 弹出模式，如果未设置则默认为InstantPopup
 * @sa setActionToolButtonPopupModeProperty
 */
QToolButton::ToolButtonPopupMode SARibbonPannel::getActionToolButtonPopupModeProperty(QAction* action)
{
	bool isok = false;
	int r     = action->property(SA_ActionPropertyName_ToolButtonPopupMode).toInt(&isok);

	if (isok) {
		return (static_cast< QToolButton::ToolButtonPopupMode >(r));
	}
	return (QToolButton::InstantPopup);
}

/**
 * @brief Sets the ToolButtonStyle property for an action / 为一个action设置ToolButtonStyle属性
 * @param action The action to modify / 要修改的action
 * @param buttonStyle The button style (e.g., ToolButtonIconOnly, ToolButtonTextBesideIcon) /
 * 按钮样式（例如，ToolButtonIconOnly, ToolButtonTextBesideIcon）
 * @sa getActionToolButtonStyleProperty
 */
void SARibbonPannel::setActionToolButtonStyleProperty(QAction* action, Qt::ToolButtonStyle buttonStyle)
{
	Q_CHECK_PTR(action);
	action->setProperty(SA_ActionPropertyName_ToolButtonStyle, static_cast< int >(buttonStyle));
}

/**
 * @brief Gets the ToolButtonStyle property from an action / 从一个action获取ToolButtonStyle属性
 * @param action The action to query / 要查询的action
 * @return The button style, defaults to ToolButtonIconOnly if not set / 按钮样式，如果未设置则默认为ToolButtonIconOnly
 * @sa setActionToolButtonStyleProperty
 */
Qt::ToolButtonStyle SARibbonPannel::getActionToolButtonStyleProperty(QAction* action)
{
	bool isok = false;
	int r     = action->property(SA_ActionPropertyName_ToolButtonStyle).toInt(&isok);

	if (isok) {
		return (static_cast< Qt::ToolButtonStyle >(r));
	}
	return (Qt::ToolButtonIconOnly);
}

/**
 * @brief Adds an action to the panel / 向面板添加一个action
 *
 * This is the primary method for populating the panel. The button created for this action will use the
 * row proportion and popup mode previously set via the static property functions.
 *
 * 这是填充面板的主要方法。为此action创建的按钮将使用之前通过静态属性函数设置的行占比和弹出模式。
 *
 * @param action The action to add / 要添加的action
 * @param rp The row proportion for this action / 此action的行占比
 * @sa addLargeAction, addMediumAction, addSmallAction
 */
void SARibbonPannel::addAction(QAction* action, SARibbonPannelItem::RowProportion rowProportion)
{
	Q_CHECK_PTR(action);
    setActionRowProportionProperty(action, rowProportion);
	addAction(action);
}

/**
 * @brief Adds an action with a specified popup mode / 添加一个具有指定弹出模式的action
 * @param act The action to add / 要添加的action
 * @param popMode The popup mode for the button / 按钮的弹出模式
 * @param rp The row proportion for this action / 此action的行占比
 */
void SARibbonPannel::addAction(QAction* act,
                               QToolButton::ToolButtonPopupMode popMode,
                               SARibbonPannelItem::RowProportion rowProportion)
{
	Q_CHECK_PTR(act);
    setActionRowProportionProperty(act, rowProportion);
	setActionToolButtonPopupModeProperty(act, popMode);
	addAction(act);
}

/**
 * @brief Adds a large action (occupies full height) / 添加一个大action（占据整个高度）
 * @param action The action to add / 要添加的action
 */
void SARibbonPannel::addLargeAction(QAction* action)
{
	addAction(action, SARibbonPannelItem::Large);
}

/**
 * @brief Adds a medium action / 添加一个中等action
 *
 * In ThreeRowMode, a medium action occupies two rows. In TwoRowMode, it is equivalent to a small action.
 *
 * 在三行模式下，中等action占据两行。在两行模式下，它等同于一个小action。
 *
 * @param action The action to add / 要添加的action
 */
void SARibbonPannel::addMediumAction(QAction* action)
{
	addAction(action, SARibbonPannelItem::Medium);
}

/**
 * @brief Adds a small action / 添加一个小action
 * @param action The action to add / 要添加的action
 */
void SARibbonPannel::addSmallAction(QAction* action)
{
	addAction(action, SARibbonPannelItem::Small);
}

/**
 * @brief Adds a small action with a specified popup mode / 添加一个具有指定弹出模式的小action
 * @param action The action to add / 要添加的action
 * @param popMode The popup mode for the button / 按钮的弹出模式
 */
void SARibbonPannel::addSmallAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
	addAction(action, popMode, SARibbonPannelItem::Small);
}

/**
 * @brief Adds a large action with a specified popup mode / 添加一个具有指定弹出模式的大action
 * @param action The action to add / 要添加的action
 * @param popMode The popup mode for the button / 按钮的弹出模式
 */
void SARibbonPannel::addLargeAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
	addAction(action, popMode, SARibbonPannelItem::Large);
}

/**
 * @brief Adds a medium action with a specified popup mode / 添加一个具有指定弹出模式的中等action
 * @param action The action to add / 要添加的action
 * @param popMode The popup mode for the button / 按钮的弹出模式
 */
void SARibbonPannel::addMediumAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
	addAction(action, popMode, SARibbonPannelItem::Medium);
}

/**
 * @brief Creates and adds a new action / 创建并添加一个新的action
 *
 * This is a convenience function that creates a new `QAction`, sets its properties, and adds it to the panel.
 * The panel will take ownership of the action.
 *
 * 这是一个便捷函数，用于创建一个新的 `QAction`，设置其属性，并将其添加到面板。
 * 面板将拥有此action的所有权。
 *
 * @param text The action's text / action的文字
 * @param icon The action's icon / action的图标
 * @param popMode The button's popup mode / 按钮的弹出模式
 * @param rp The action's row proportion / action的行占比
 * @return The newly created action / 新创建的action
 */
QAction* SARibbonPannel::addAction(const QString& text,
                                   const QIcon& icon,
                                   QToolButton::ToolButtonPopupMode popMode,
                                   SARibbonPannelItem::RowProportion rowProportion)
{
	QAction* action = new QAction(icon, text, this);
    addAction(action, popMode, rowProportion);
	return (action);
}

/**
 * @brief Adds a menu to the panel / 向面板添加一个菜单
 * @param menu The menu to add / 要添加的菜单
 * @param rp The row proportion for the menu button / 菜单按钮的行占比
 * @param popMode The popup mode, defaults to InstantPopup / 弹出模式，默认为InstantPopup
 */
void SARibbonPannel::addMenu(QMenu* menu,
                             SARibbonPannelItem::RowProportion rowProportion,
                             QToolButton::ToolButtonPopupMode popMode)
{
	Q_CHECK_PTR(menu);
	QAction* action = menu->menuAction();
	action->setIcon(menu->icon());
	action->setText(menu->title());
	action->setObjectName("action." + menu->objectName());
    addAction(action, popMode, rowProportion);
}

/**
 * @brief Adds a menu as a large button / 以大按钮形式添加一个菜单
 * @param menu The menu to add / 要添加的菜单
 * @param popMode The popup mode / 弹出模式
 */
void SARibbonPannel::addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode)
{
	addMenu(menu, SARibbonPannelItem::Large, popMode);
}

/**
 * @brief Adds a menu as a small button / 以小按钮形式添加一个菜单
 * @param menu The menu to add / 要添加的菜单
 * @param popMode The popup mode / 弹出模式
 */
void SARibbonPannel::addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode)
{
	addMenu(menu, SARibbonPannelItem::Small, popMode);
}

/**
 * @brief Adds a custom widget to the panel / 向面板添加一个自定义窗口部件
 *
 * The panel will create a `QWidgetAction` to manage the widget. The panel does not take ownership of the widget;
 * its lifetime is managed by its parent or the caller.
 *
 * 面板将创建一个 `QWidgetAction` 来管理此窗口部件。面板不拥有此窗口部件的所有权；
 * 其生命周期由其父对象或调用者管理。
 *
 * @param w The widget to add / 要添加的窗口部件
 * @param rp The row proportion for the widget / 窗口部件的行占比
 * @return The created QWidgetAction / 创建的QWidgetAction
 */
QAction* SARibbonPannel::addWidget(QWidget* w, SARibbonPannelItem::RowProportion rowProportion)
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
 * @brief Adds a widget as a small button / 以小按钮形式添加一个窗口部件
 * @param w The widget to add / 要添加的窗口部件
 * @return The created QWidgetAction / 创建的QWidgetAction
 */
QAction* SARibbonPannel::addSmallWidget(QWidget* w)
{
	return (addWidget(w, SARibbonPannelItem::Small));
}

/**
 * @brief Adds a widget as a medium button / 以中等按钮形式添加一个窗口部件
 * @param w The widget to add / 要添加的窗口部件
 * @return The created QWidgetAction / 创建的QWidgetAction
 */
QAction* SARibbonPannel::addMediumWidget(QWidget* w)
{
	return (addWidget(w, SARibbonPannelItem::Medium));
}

/**
 * @brief Adds a widget as a large button / 以大按钮形式添加一个窗口部件
 * @param w The widget to add / 要添加的窗口部件
 * @return The created QWidgetAction / 创建的QWidgetAction
 */
QAction* SARibbonPannel::addLargeWidget(QWidget* w)
{
	return (addWidget(w, SARibbonPannelItem::Large));
}

/**
 * @brief Adds a gallery widget to the panel / 向面板添加一个画廊窗口部件
 *
 * A gallery is a specialized widget for displaying a collection of items. If `expanding` is true,
 * the panel's horizontal size policy will be set to `Expanding`.
 *
 * 画廊是一个用于显示项目集合的专用窗口部件。如果 `expanding` 为true，
 * 面板的水平尺寸策略将被设置为 `Expanding`。
 *
 * @param expanding If true, sets the panel's size policy to Expanding / 如果为true，将面板的尺寸策略设置为Expanding
 * @return The created SARibbonGallery / 创建的SARibbonGallery
 */
SARibbonGallery* SARibbonPannel::addGallery(bool expanding)
{
	SARibbonGallery* gallery = RibbonSubElementFactory->createRibbonGallery(this);

	addWidget(gallery, SARibbonPannelItem::Large);
	if (expanding) {
		setExpanding(expanding);
	}
	return (gallery);
}

/**
 * @brief Adds a separator / 添加一个分隔符
 * @return The created separator action / 创建的分隔符action
 */
QAction* SARibbonPannel::addSeparator()
{
	QAction* action = new QAction(this);

	action->setSeparator(true);
	setActionRowProportionProperty(action, SARibbonPannelItem::Large);
	addAction(action);
	return (action);
}

/**
 * @brief Retrieves the SARibbonToolButton associated with an action / 获取与action关联的SARibbonToolButton
 * @param action The action to query / 要查询的action
 * @return The associated tool button, or nullptr if not found / 关联的工具按钮，如果未找到则返回nullptr
 */
SARibbonToolButton* SARibbonPannel::actionToRibbonToolButton(QAction* action)
{
	for (auto obj : qAsConst(children())) {
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
 * @brief Returns a list of all SARibbonToolButton children / 返回所有SARibbonToolButton子对象的列表
 * @return A list of pointers to SARibbonToolButton / SARibbonToolButton指针的列表
 */
QList< SARibbonToolButton* > SARibbonPannel::ribbonToolButtons() const
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
 * @brief Sets the panel's layout mode / 设置面板的布局模式
 * @param mode The new layout mode (ThreeRowMode or TwoRowMode) / 新的布局模式（三行模式或两行模式）
 * @sa pannelLayoutMode
 */
void SARibbonPannel::setPannelLayoutMode(SARibbonPannel::PannelLayoutMode mode)
{
	if (d_ptr->m_pannelLayoutMode == mode) {
		return;
	}
	d_ptr->m_pannelLayoutMode = mode;
	updateItemGeometry();
}

/**
 * @brief Gets the current layout mode / 获取当前的布局模式
 * @return The current layout mode / 当前的布局模式
 * @sa setPannelLayoutMode
 */
SARibbonPannel::PannelLayoutMode SARibbonPannel::pannelLayoutMode() const
{
	return (d_ptr->m_pannelLayoutMode);
}

/**
 * @brief Sets the option action for the panel / 为面板设置选项action
 *
 * The option action is typically displayed as a small button in the panel's title area.
 * Pass `nullptr` to remove the current option action.
 *
 * 选项action通常显示在面板标题区域的一个小按钮中。
 * 传入 `nullptr` 以移除当前的选项action。
 *
 * @param action The option action, or nullptr to remove / 选项action，或传入nullptr以移除
 * @sa isHaveOptionAction
 */
void SARibbonPannel::setOptionAction(QAction* action)
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		lay->setOptionAction(action);
	}
}

/**
 * @brief Checks if an option action is set / 检查是否设置了选项action
 * @return true if an option action exists; otherwise false / 如果存在选项action则返回true；否则返回false
 * @sa setOptionAction
 */
bool SARibbonPannel::isHaveOptionAction() const
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		return lay->isHaveOptionAction();
	}
	return false;
}

QSize SARibbonPannel::sizeHint() const
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

QSize SARibbonPannel::minimumSizeHint() const
{
	return (layout()->minimumSize());
}

/**
 * @brief Sets the icon size for all tool buttons in the panel / 设置面板内所有工具按钮的图标尺寸
 *
 * This sets the default icon size for newly created buttons and updates the icon size for existing buttons.
 *
 * 此函数设置新创建按钮的默认图标尺寸，并更新现有按钮的图标尺寸。
 *
 * @param s The new icon size / 新的图标尺寸
 * @sa toolButtonIconSize
 */
void SARibbonPannel::setToolButtonIconSize(const QSize& s)
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		lay->setToolButtonIconSize(s);
	}
	// 对已经管理的QToolButton设置为iconsize
	iterateButton([ s ](SARibbonToolButton* t) -> bool {
		t->setIconSize(s);
		return true;
	});
}

/**
 * @brief Gets the current default icon size for tool buttons / 获取工具按钮当前的默认图标尺寸
 * @return The current icon size / 当前的图标尺寸
 * @sa setToolButtonIconSize
 */
QSize SARibbonPannel::toolButtonIconSize() const
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		return lay->toolButtonIconSize();
	}
	return QSize();
}

/**
 * @brief Iterates over all SARibbonToolButton children / 遍历所有SARibbonToolButton子对象
 *
 * Executes the provided function pointer for each `SARibbonToolButton`. If the function returns `false`,
 * iteration stops immediately.
 *
 * 对每个 `SARibbonToolButton` 执行提供的函数指针。如果函数返回 `false`，
 * 迭代将立即停止。
 *
 * Example:
 * @code
 * myPannel->iterateButton([](SARibbonToolButton* btn) -> bool {
 *     btn->setToolTip("Visited");
 *     return true; // Continue to next button/继续下一个按钮
 * });
 * @endcode
 *
 * @param fp A function pointer that takes a SARibbonToolButton* and returns a bool / 一个接受SARibbonToolButton*并返回bool的函数指针
 * @return true if all buttons were iterated; false if iteration was stopped early / 如果遍历了所有按钮则返回true；如果提前停止则返回false
 */
bool SARibbonPannel::iterateButton(SARibbonPannel::FpRibbonToolButtonIterate fp) const
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
 * @brief Sets the panel to expanding mode / 将面板设置为扩展模式
 *
 * In expanding mode, the panel will try to occupy all available horizontal space in its parent layout.
 *
 * 在扩展模式下，面板将尝试占据其父布局中所有可用的水平空间。
 *
 * @param isExpanding If true, sets the horizontal size policy to Expanding / 如果为true，将水平尺寸策略设置为Expanding
 * @sa isExpanding
 */
void SARibbonPannel::setExpanding(bool isExpanding)
{
	setSizePolicy(isExpanding ? QSizePolicy::Expanding : QSizePolicy::Preferred, QSizePolicy::Fixed);
}

/**
 * @brief Checks if the panel is in expanding mode / 检查面板是否处于扩展模式
 * @return true if the horizontal size policy is Expanding; otherwise false / 如果水平尺寸策略为Expanding则返回true；否则返回false
 * @sa setExpanding
 */
bool SARibbonPannel::isExpanding() const
{
	QSizePolicy sp = sizePolicy();

	return (sp.horizontalPolicy() == QSizePolicy::Expanding);
}

/**
 * @brief Sets the height of the panel's title / 设置面板标题的高度
 *
 * This function is typically called by `SARibbonBar` and is protected to prevent direct user manipulation.
 *
 * 此函数通常由 `SARibbonBar` 调用，为防止用户直接操作而设为protected。
 *
 * @param h The new title height / 新的标题高度
 * @sa SARibbonBar::setPannelTitleHeight
 */
void SARibbonPannel::setTitleHeight(int h)
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		lay->setPannelTitleHeight(h);
	}
	d_ptr->resetTitleLabelFont();
}

/**
 * @brief Gets the height of the panel's title / 获取面板标题的高度
 * @return The current title height / 当前的标题高度
 */
int SARibbonPannel::titleHeight() const
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		return lay->pannelTitleHeight();
	}
	return -1;
}

/**
 * @brief Checks if the panel's title is visible / 检查面板标题是否可见
 * @return true if the title is visible; otherwise false / 如果标题可见则返回true；否则返回false
 */
bool SARibbonPannel::isEnableShowTitle() const
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		return lay->isEnableShowPannelTitle();
	}
	return false;
}

/**
 * @brief Sets whether the panel's title is visible / 设置面板标题是否可见
 *
 * This function is typically called by `SARibbonBar` and is protected to prevent direct user manipulation.
 *
 * 此函数通常由 `SARibbonBar` 调用，为防止用户直接操作而设为protected。
 *
 * @param on If true, the title is visible / 如果为true，则标题可见
 * @sa SARibbonBar::setEnableShowPannelTitle
 */
void SARibbonPannel::setEnableShowTitle(bool on)
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		return lay->setEnableShowPannelTitle(on);
	}
}

/**
 * @brief Gets the index of an action within the panel / 获取一个action在面板内的索引
 * @param act The action to find / 要查找的action
 * @return The index of the action, or -1 if not found / action的索引，如果未找到则返回-1
 */
int SARibbonPannel::actionIndex(QAction* act) const
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		return (lay->indexByAction(act));
	}
	return (-1);
}

/**
 * @brief Moves an action to a new position / 将一个action移动到新位置
 * @param from The current index of the action / action的当前索引
 * @param to The new index for the action / action的新索引
 */
void SARibbonPannel::moveAction(int from, int to)
{
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		lay->move(from, to);
		lay->activate();
	}
}

/**
 * @brief Checks if customization is allowed / 检查是否允许自定义
 * @return true if customization is allowed; otherwise false / 如果允许自定义则返回true；否则返回false
 * @sa setCanCustomize
 */
bool SARibbonPannel::isCanCustomize() const
{
	return (d_ptr->m_isCanCustomize);
}

/**
 * @brief Sets whether customization is allowed / 设置是否允许自定义
 * @param b If true, customization is allowed / 如果为true，则允许自定义
 * @sa isCanCustomize
 */
void SARibbonPannel::setCanCustomize(bool b)
{
	d_ptr->m_isCanCustomize = b;
}

/**
 * @brief Gets the panel's name / 获取面板的名称
 * @return The panel's name / 面板的名称
 * @sa setPannelName
 */
QString SARibbonPannel::pannelName() const
{
	return (d_ptr->pannelName());
}

/**
 * @brief Sets the panel's name / 设置面板的名称
 *
 * Setting the name will also emit the `pannelNameChanged` signal.
 *
 * 设置名称还会发出 `pannelNameChanged` 信号。
 *
 * @param title The new name for the panel / 面板的新名称
 * @sa pannelName
 */
void SARibbonPannel::setPannelName(const QString& title)
{
	QString oldName = pannelName();
	if (oldName != title) {
		d_ptr->setPannelName(title);
		Q_EMIT pannelNameChanged(title);
	}
}

/**
 * @brief Gets the height of the large buttons in this panel / 获取此面板中大按钮的高度
 * @return The height of large buttons / 大按钮的高度
 */
int SARibbonPannel::largeButtonHeight() const
{
	const QMargins& mag = contentsMargins();
	return height() - mag.top() - mag.bottom() - titleHeight();
}

/**
 * @brief Gets the panel's layout object / 获取面板的布局对象
 * @return A pointer to the SARibbonPannelLayout / 指向SARibbonPannelLayout的指针
 */
SARibbonPannelLayout* SARibbonPannel::pannelLayout() const
{
	return d_ptr->pannelLayout();
}

/**
 * @brief Updates the geometry of all items in the panel / 更新面板内所有项目的几何形状
 *
 * This function forces a recalculation of the layout and size hints for all buttons and items.
 *
 * 此函数强制重新计算所有按钮和项目的布局及尺寸提示。
 */
void SARibbonPannel::updateItemGeometry()
{
#if SARIBBONPANNEL_DEBUG_PRINT
	qDebug() << "SARibbonPannel updateItemGeometry,pannelName=" << pannelName();
#endif
	// 此函数需要添加，否则SARibbonBar::setEnableWordWrap无法刷新按钮
    // resetToolButtonSize();
	if (SARibbonPannelLayout* lay = pannelLayout()) {
		lay->updateGeomArray();
	}
}

/**
 * @brief Gets the parent SARibbonCategory / 获取父SARibbonCategory
 * @return The parent category, or nullptr if not found / 父category，如果未找到则返回nullptr
 */
SARibbonCategory* SARibbonPannel::category() const
{
	return qobject_cast< SARibbonCategory* >(parent());
}

/**
 * @brief Gets the top-level SARibbonBar / 获取顶层的SARibbonBar
 * @return The ribbon bar, or nullptr if not found / Ribbon栏，如果未找到则返回nullptr
 */
SARibbonBar* SARibbonPannel::ribbonBar() const
{
	if (SARibbonCategory* c = category()) {
		return c->ribbonBar();
	}
	return nullptr;
}

/**
 * @brief Sets the spacing between elements in the panel / 设置面板内元素之间的间距
 *
 * This function is typically called by `SARibbonBar` and is protected to prevent direct user manipulation.
 *
 * 此函数通常由 `SARibbonBar` 调用，为防止用户直接操作而设为protected。
 *
 * @param n The new spacing value / 新的间距值
 * @sa SARibbonBar::setPannelSpacing
 */
void SARibbonPannel::setSpacing(int n)
{
	if (auto lay = layout()) {
		lay->setSpacing(n);
	}
}

/**
 * @brief Gets the spacing between elements in the panel / 获取面板内元素之间的间距
 * @return The current spacing value / 当前的间距值
 * @sa setSpacing
 */
int SARibbonPannel::spacing() const
{
	if (auto lay = layout()) {
		return lay->spacing();
	}
	return 0;
}

/**
 * @brief Gets the panel's recommended height / 获取面板的推荐高度
 *
 * This is a static helper function used internally for layout calculations.
 *
 * 这是一个用于内部布局计算的静态辅助函数。
 *
 * @param fm The font metrics to use for calculation / 用于计算的字体度量
 * @param layMode The layout mode / 布局模式
 * @param pannelTitleHeight The height of the panel's title / 面板标题的高度
 * @return The recommended height / 推荐的高度
 */
int SARibbonPannel::pannelHeightHint(const QFontMetrics& fm, PannelLayoutMode layMode, int pannelTitleHeight)
{
	int textH = fm.lineSpacing();  // 这里用linespace，因为在换行的情况下，行距是不可忽略的，ribbon的大按钮默认是2行
	switch (layMode) {
	case SARibbonPannel::ThreeRowMode: {
		// 5.5=（3*1.6+1） （三行）,1是给panneltitle预留的
		return textH * 4.8 + pannelTitleHeight;
	} break;
	case SARibbonPannel::TwoRowMode: {
		// 3=2*1.6
		return textH * 3.2 + pannelTitleHeight;
	} break;
	default: {
		qWarning() << "unknow SARibbonPannel::PannelLayoutMode:" << layMode;
	}
	}
	return (textH * 4.8 + pannelTitleHeight);
}

/**
 * @brief 重置按钮的尺寸，在布局改变后（尤其高度变更），按钮的尺寸需要手动变更
 */
void SARibbonPannel::resetToolButtonSize()
{
	QList< SARibbonToolButton* > btns = ribbonToolButtons();

	for (SARibbonToolButton* b : qAsConst(btns)) {
		if ((nullptr == b)) {
			continue;
		}
        b->invalidateSizeHint();
	}
}

bool SARibbonPannel::isTwoRow() const
{
    return (TwoRowMode == pannelLayoutMode());
}

/**
 * @brief Handles action events (add, remove, change) / 处理action事件（添加、移除、更改）
 *
 * This function is called by Qt when actions are added, removed, or changed. It ensures the panel's
 * layout is updated accordingly.
 *
 * 当action被添加、移除或更改时，Qt会调用此函数。它确保面板的布局相应更新。
 *
 * @param e The action event / action事件
 */
void SARibbonPannel::actionEvent(QActionEvent* e)
{
	QAction* action             = e->action();
	QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(action);

	switch (e->type()) {
	case QEvent::ActionAdded: {
		SARibbonPannelLayout* lay = pannelLayout();
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
		SARibbonPannelLayout* lay = pannelLayout();
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
 * @brief Handles change events (e.g., font change) / 处理更改事件（例如，字体更改）
 *
 * This function is called when properties like the font change. It propagates the change to child
 * widgets and invalidates the layout.
 *
 * 当字体等属性更改时会调用此函数。它将更改传播到子窗口部件并使布局失效。
 *
 * @param e The change event / 更改事件
 */
void SARibbonPannel::changeEvent(QEvent* e)
{
	if (nullptr == e) {
		return;
	}
	if (e->type() == QEvent::FontChange) {
        if (d_ptr->m_label) {
            d_ptr->m_label->setFont(font());
        }
		if (QLayout* lay = layout()) {
			lay->invalidate();
		}
		d_ptr->resetTitleLabelFont();
	}
	QWidget::changeEvent(e);
}

/**
 * @brief 获取布局item
 * @return
 */
const QList< SARibbonPannelItem* >& SARibbonPannel::ribbonPannelItem() const
{
	return (pannelLayout()->mItems);
}

/**
 * @brief 获取最近添加的按钮
 * @return 刚才添加的按钮的指针
 */
SARibbonToolButton* SARibbonPannel::lastAddActionButton()
{
	return d_ptr->lastAddActionButton();
}

/**
 * @brief Gets the title label widget / 获取标题标签窗口部件
 * @return A pointer to the SARibbonPannelLabel / 指向SARibbonPannelLabel的指针
 */
SARibbonPannelLabel* SARibbonPannel::titleLabel() const
{
    return d_ptr->m_label;
}

/**
 * @brief 设置pannel的按钮文字允许换行
 * @param on
 */
void SARibbonPannel::setEnableWordWrap(bool on)
{
    if (SARibbonPannelLayout* lay = qobject_cast< SARibbonPannelLayout* >(layout())) {
        // 此函数会自动设置所有按钮的换行状态
        lay->setEnableWordWrap(on);
        updateGeometry();
    }
}

/**
 * @brief 判断pannel的文字是否允许换行
 * @return
 */
bool SARibbonPannel::isEnableWordWrap() const
{
    if (SARibbonPannelLayout* lay = qobject_cast< SARibbonPannelLayout* >(layout())) {
        return lay->isEnableWordWrap();
    }
    return false;
}

/**
 * @brief 设置按钮最大宽高比，这个系数决定按钮的最大宽度
 *
 * 按钮的最大宽度为按钮高度*此系数，例如按钮高度为h，那么按钮最大宽度maxw=h*buttonMaximumAspectRatio
 * 如果在此宽度下文字还无法完全显示，那么按钮将不会继续横向扩展，将使用...替代未完全显示的文字
 *
 * @see buttonMaximumAspectRatio
 *
 * @note 用户不应该调用@ref SARibbonPannel::setButtonMaximumAspectRatio 来设置，
 * 而是调用@ref SARibbonBar::setButtonMaximumAspectRatio 设置宽高比
 */
void SARibbonPannel::setButtonMaximumAspectRatio(qreal fac)
{
    if (SARibbonPannelLayout* lay = qobject_cast< SARibbonPannelLayout* >(layout())) {
        // 此函数会自动设置所有按钮的换行状态
        lay->setButtonMaximumAspectRatio(fac);
    }
}

/**
 * @brief 按钮最大宽高比，这个系数决定按钮的最大宽度
 * @return 按钮最大宽高比
 * @see setButtonMaximumAspectRatio
 */
qreal SARibbonPannel::buttonMaximumAspectRatio() const
{
    if (SARibbonPannelLayout* lay = qobject_cast< SARibbonPannelLayout* >(layout())) {
        return lay->buttonMaximumAspectRatio();
    }
    return 1.4;
}
