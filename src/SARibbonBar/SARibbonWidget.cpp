#include "SARibbonWidget.h"
#include "SARibbonBar.h"
#include "SARibbonTabBar.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QScreen>
#include "SARibbonUtil.h"
/**
 * @brief The SARibbonWidget::PrivateData class
 */
class SARibbonWidget::PrivateData
{
	SA_RIBBON_DECLARE_PUBLIC(SARibbonWidget)
public:
	PrivateData(SARibbonWidget* p);
	void installFrameless(SARibbonWidget* p);

public:
	SARibbonTheme mCurrentRibbonTheme { SARibbonTheme::RibbonThemeOffice2021Blue };
};

SARibbonWidget::PrivateData::PrivateData(SARibbonWidget* p) : q_ptr(p)
{
}

//===================================================
// SARibbonWidget
//===================================================
/**
 * \if ENGLISH
 * @brief Constructs a SARibbonWidget instance
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 构造一个 SARibbonWidget 实例
 * @param parent 父窗口部件
 * \endif
 */
SARibbonWidget::SARibbonWidget(QWidget* parent) : QWidget(parent), d_ptr(new SARibbonWidget::PrivateData(this))
{
	// 直接创建SARibbonBar
	QVBoxLayout* verticalLayout = new QVBoxLayout(this);
	verticalLayout->setSpacing(1);
	verticalLayout->setObjectName("verticalLayout");
	verticalLayout->setContentsMargins(0, 0, 0, 0);
	SARibbonBar* ribbon = new SARibbonBar(this);
	setRibbonBar(ribbon);
	connect(qApp, &QApplication::primaryScreenChanged, this, &SARibbonWidget::onPrimaryScreenChanged);
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
SARibbonWidget::~SARibbonWidget()
{
}

/**
 * \if ENGLISH
 * @brief Returns the ribbonbar, returns nullptr if not using ribbon mode
 * @return Pointer to the SARibbonBar, or nullptr if not using ribbon mode
 * \endif
 *
 * \if CHINESE
 * @brief 返回ribbonbar，如果不是使用ribbon模式，返回nullptr
 * @return 指向SARibbonBar的指针，如果不是使用ribbon模式，返回nullptr
 * \endif
 */
SARibbonBar* SARibbonWidget::ribbonBar() const
{
	QLayout* lay = layout();
	if (lay) {
		return qobject_cast< SARibbonBar* >(lay->menuBar());
	}
	return nullptr;
}

/**
 * \if ENGLISH
 * @brief Sets the ribbonbar
 * @param bar Pointer to the SARibbonBar to set
 * \endif
 *
 * \if CHINESE
 * @brief 设置ribbonbar
 * @param bar 要设置的SARibbonBar指针
 * \endif
 */
void SARibbonWidget::setRibbonBar(SARibbonBar* bar)
{
	QLayout* lay = layout();
	if (lay) {
		lay->setMenuBar(bar);
	}
}

/**
 * \if ENGLISH
 * @brief Sets the ribbon theme
 *
 * Note that setting the theme in the constructor may not take full effect. Use QTimer to put it at the end of the queue:
 * @code
 * QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
 * @endcode
 * @param theme The theme to set
 * \endif
 *
 * \if CHINESE
 * @brief 设置ribbon主题
 *
 * 注意主题在构造函数设置主题会不完全生效，使用QTimer投放到队列最后执行即可
 * @code
 * QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
 * @endcode
 * @param theme 要设置的主题
 * \endif
 */
void SARibbonWidget::setRibbonTheme(SARibbonTheme theme)
{
    SA::setBuiltInRibbonTheme(this, theme);
	d_ptr->mCurrentRibbonTheme = theme;
	if (SARibbonBar* bar = ribbonBar()) {
		auto theme = ribbonTheme();
		// 尺寸修正
		switch (theme) {
		case SARibbonTheme::RibbonThemeWindows7:
		case SARibbonTheme::RibbonThemeOffice2013:
		case SARibbonTheme::RibbonThemeOffice2016Blue:
		case SARibbonTheme::RibbonThemeDark:
		case SARibbonTheme::RibbonThemeDark2: {
			//! 在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
			//! office2013.qss的margin信息如下设置
			//! margin-top: 0px;
			//! margin-right: 0px;
			//! margin-left: 5px;
			//! margin-bottom: 0px;
			SARibbonTabBar* tab = bar->ribbonTabBar();
			if (!tab) {
				break;
			}
			tab->setTabMargin(QMargins(5, 0, 0, 0));
		} break;
		case SARibbonTheme::RibbonThemeOffice2021Blue: {
			SARibbonTabBar* tab = bar->ribbonTabBar();
			if (!tab) {
				break;
			}
			//! 在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
			//! office2021.qss的margin信息如下设置
			//! margin-top: 0px;
			//! margin-right: 5px;
			//! margin-left: 5px;
			//! margin-bottom: 0px;
			tab->setTabMargin(QMargins(5, 0, 5, 0));
		}
		default:
			break;
		}
		// 上下文标签颜色设置,以及基线颜色设置
		switch (theme) {
		case SARibbonTheme::RibbonThemeWindows7:
		case SARibbonTheme::RibbonThemeOffice2013:
		case SARibbonTheme::RibbonThemeDark:
			bar->setContextCategoryColorList(QList< QColor >());  //< 设置空颜色列表会重置为默认色系
			break;
		case SARibbonTheme::RibbonThemeOffice2016Blue:
			bar->setContextCategoryColorList(QList< QColor >() << QColor(18, 64, 120));  //< 设置空颜色列表会重置为默认色系
			break;
		case SARibbonTheme::RibbonThemeOffice2021Blue:
			bar->setContextCategoryColorList(QList< QColor >() << QColor(209, 207, 209));  //< 设置空颜色列表会重置为默认色系
			break;
		default:
			break;
		}
		// 基线颜色设置
		if (SARibbonTheme::RibbonThemeOffice2013 == theme) {
			bar->setTabBarBaseLineColor(QColor(186, 201, 219));
		} else {
			bar->setTabBarBaseLineColor(QColor());
		}
	}
}

/**
 * \if ENGLISH
 * @brief Gets the current ribbon theme
 * @return The current SARibbonTheme
 * \endif
 *
 * \if CHINESE
 * @brief 获取当前的ribbon主题
 * @return 当前的SARibbonTheme
 * \endif
 */
SARibbonTheme SARibbonWidget::ribbonTheme() const
{
	return (d_ptr->mCurrentRibbonTheme);
}

/**
 * \if ENGLISH
 * @brief Checks if ribbon is being used
 * @return true if ribbon is being used, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 检查是否使用ribbon
 * @return 如果使用ribbon则返回true，否则返回false
 * \endif
 */
bool SARibbonWidget::isUseRibbon() const
{
	return (nullptr != ribbonBar());
}

/**
 * \if ENGLISH
 * @brief Sets the widget
 * @param w Widget pointer
 * @note The ownership of the widget is managed by SARibbonWidget
 * @sa widget
 * @note The previously set widget will be deleted
 * \endif
 *
 * \if CHINESE
 * @brief 设置窗口
 * @param w 窗口指针
 * @note 窗口的所有权归SARibbonWidget管理
 * @sa widget
 * @note 原来设置的窗口会被delete
 * \endif
 */
void SARibbonWidget::setWidget(QWidget* w)
{
	QWidget* oldwidget = takeWidget();
	if (oldwidget) {
		oldwidget->hide();
		oldwidget->deleteLater();
	}
	if (QLayout* lay = layout()) {
		lay->addWidget(w);
	}
}

/**
 * \if ENGLISH
 * @brief Gets the set widget
 * @return The widget, or nullptr if none is set
 * \endif
 *
 * \if CHINESE
 * @brief 获取设置的窗口
 * @return 如果没有，返回nullptr
 * \endif
 */
QWidget* SARibbonWidget::widget() const
{
	QLayout* lay = layout();
	if (lay) {
		if (lay->count() == 0) {
			return nullptr;
		}
		return lay->itemAt(0)->widget();
	}
	return nullptr;
}

/**
 * \if ENGLISH
 * @brief Takes the widget from the layout
 * @return The widget that was taken, or nullptr if no widget was found
 * \endif
 *
 * \if CHINESE
 * @brief 从布局中取出窗口
 * @return 被取出的窗口，如果没有找到则返回nullptr
 * \endif
 */
QWidget* SARibbonWidget::takeWidget()
{
	QLayout* lay = layout();
	if (nullptr == lay) {
		return nullptr;
	}
	if (lay->count() == 0) {
		return nullptr;
	}
	auto item = lay->itemAt(0);
	if (nullptr == item) {
		return nullptr;
	}
	lay->removeItem(item);
	auto w = item->widget();
	delete item;
	if (!w) {
		return nullptr;
	}
	return w;
}

/**
 * \if ENGLISH
 * @brief Signal triggered when the primary screen changes
 * @param screen The new primary screen
 * \endif
 *
 * \if CHINESE
 * @brief 主屏幕切换触发的信号
 * @param screen 新的主屏幕
 * \endif
 */
void SARibbonWidget::onPrimaryScreenChanged(QScreen* screen)
{
	Q_UNUSED(screen);
	// 主屏幕切换后，从新计算所有尺寸
	if (SARibbonBar* bar = ribbonBar()) {
		qDebug() << "Primary Screen Changed";
		bar->updateRibbonGeometry();
	}
}
