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

SARibbonWidget::~SARibbonWidget()
{
}

/**
 * @brief 返回ribbonbar，如果不是使用ribbon模式，返回nullptr
 * @return
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
 * @brief 设置ribbonbar
 * @param bar
 */
void SARibbonWidget::setRibbonBar(SARibbonBar* bar)
{
	QLayout* lay = layout();
	if (lay) {
		lay->setMenuBar(bar);
	}
}

/**
 * @brief SARibbonMainWindow::setRibbonTheme
 *
 * 注意主题在构造函数设置主题会不完全生效，使用QTimer投放到队列最后执行即可
 * @code
 * QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
 * @endcode
 * @param theme
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

SARibbonTheme SARibbonWidget::ribbonTheme() const
{
	return (d_ptr->mCurrentRibbonTheme);
}

bool SARibbonWidget::isUseRibbon() const
{
	return (nullptr != ribbonBar());
}

/**
   @brief 设置窗口

   @param 窗口指针
   @note 窗口的所有权归SARibbonWidget管理
   @sa widget
   @note 原来设置的窗口会被delete
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
   @brief 获取设置的窗口

    @return 如果没有，返回nullptr
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
 * @brief 主屏幕切换触发的信号
 * @param screen
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
