﻿#include "SARibbonBarLayout.h"
#include <QStyle>
#include <QApplication>
#include <QScreen>
#include "SARibbonBar.h"
#include "SARibbonTabBar.h"
#include "SARibbonStackedWidget.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonApplicationButton.h"
#include "SARibbonCategory.h"
#include "SARibbonElementManager.h"

#ifndef SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
#define SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT 0
#endif
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
#include <QDebug>
#else
#endif

class SARibbonBarLayout::PrivateData
{
public:
	SARibbonBar* ribbonBar;
	QList< QLayoutItem* > items;
	QRect titleRect;
	int titleBarHeight { 30 };     ///< 标题栏高度
	int tabBarHeight { 28 };       ///< tabbar高度
	int pannelTitleHeight { 15 };  ///< pannel的标题栏默认高度
	int categoryHeight { 60 };     ///< Category的高度

	int maxMinWidth { 1000 };  ///< 最大的最小宽度，这个一般是屏幕宽度的0.8，避免太大导致超过屏幕
	int minWidth { 500 };
	int minHeight { 0 };
	bool isTabOnTitle { false };  ///< 是否tab在标题栏上
	std::unique_ptr< int > userDefTitleBarHeight;  ///< 用户定义的标题栏高度，正常不使用用户设定的高度，而是使用自动计算的高度
	std::unique_ptr< int > userDefTabBarHeight;  ///< 用户定义的tabbar高度，正常不使用用户设定的高度，而是使用自动计算的高度
	std::unique_ptr< int > userDefCategoryHeight;  ///< 用户定义的Category的高度，正常不使用用户设定的高度，而是使用自动计算的高度
	QSize systemButtonSize;  ///< 由SARibbonMainWindow告诉窗口的关闭最大化等按钮的尺寸

public:
	PrivateData(SARibbonBar* bar) : ribbonBar(bar), systemButtonSize(0, 0)
	{
		// 获取主屏幕的尺寸
		QScreen* primaryScreen = QGuiApplication::primaryScreen();
		QRect screenGeometry   = primaryScreen->geometry();
		if (currentRibbonMode() == SARibbonBar::MinimumRibbonMode) {
			minHeight = getActualTitleBarHeight() + (isTabOnTitle ? 0 : getActualTabBarHeight());
		} else {
			minHeight = getActualTitleBarHeight() + getActualCategoryHeight()
						+ (isTabOnTitle ? 0 : getActualTabBarHeight());
		}
		maxMinWidth = screenGeometry.width() * 0.8;  // 屏幕宽度
	}

	SARibbonTabBar* ribbonTabBar() const
	{
		return ribbonBar->ribbonTabBar();
	}

	SARibbonStackedWidget* stackedContainerWidget() const
	{
		return ribbonBar->ribbonStackedWidget();
	}

	SARibbonQuickAccessBar* quickAccessBar() const
	{
		return ribbonBar->quickAccessBar();
	}

	SARibbonButtonGroupWidget* rightButtonGroup() const
	{
		return ribbonBar->rightButtonGroup();
	}

	QAbstractButton* applicationButton() const
	{
		return ribbonBar->applicationButton();
	}

	SARibbonBar::RibbonMode currentRibbonMode() const
	{
		return ribbonBar->currentRibbonState();
	}

	QMargins contentsMargins() const
	{
		return ribbonBar->contentsMargins();
	}

	SARibbonBar::RibbonStyles ribbonStyle() const
	{
		return ribbonBar->currentRibbonStyle();
	}

	void setTitleBarHeight(int h)
	{
		if (!userDefTitleBarHeight) {
			userDefTitleBarHeight = std::make_unique< int >(h);
		} else {
			*userDefTitleBarHeight = h;
		}
	}

	void setTabBarHeight(int h)
	{
		if (!userDefTabBarHeight) {
			userDefTabBarHeight = std::make_unique< int >(h);
		} else {
			*userDefTabBarHeight = h;
		}
	}

	void setCategoryHeight(int h)
	{
		if (!userDefCategoryHeight) {
			userDefCategoryHeight = std::make_unique< int >(h);
		} else {
			*userDefCategoryHeight = h;
		}
	}

	int getActualTitleBarHeight() const
	{
		if (userDefTitleBarHeight) {
			return *userDefTitleBarHeight;
		} else {
			return titleBarHeight;
		}
	}

	int getActualTabBarHeight() const
	{
		if (userDefTabBarHeight) {
			return *userDefTabBarHeight;
		} else {
			return tabBarHeight;
		}
	}

	int getActualCategoryHeight() const
	{
		if (userDefCategoryHeight) {
			return *userDefCategoryHeight;
		} else {
			return categoryHeight;
		}
	}

	void estimateSizeHint()
	{
		titleBarHeight = calcDefaultTitleBarHeight();
		// tabBarHeight有大于0的值说明用户设置了，就使用用户设置的值
		tabBarHeight   = calcDefaultTabBarHeight();
		categoryHeight = calcCategoryHeight();
	}

	int systemTabBarHeight() const
	{
		QStyle* style = ribbonBar->style();
		return style->pixelMetric(QStyle::PM_TabBarBaseHeight) + style->pixelMetric(QStyle::PM_TabBarTabHSpace)
			   + style->pixelMetric(QStyle::PM_TabBarTabOverlap);
	}

	/**
	 * @brief 估算tabbar的高度
	 * @param fm
	 * @return
	 */
	int calcDefaultTabBarHeight()
	{
		int defaultHeight = systemTabBarHeight();
		int fontHeight = ribbonBar->fontMetrics().lineSpacing();  // 不要用height，像宋体这种字体，height=12，lineSpacing=14，有些就无法显示
		int defaultHeight2 = fontHeight * 1.6;
		if (defaultHeight2 < fontHeight + 10) {
			defaultHeight2 = fontHeight + 10;  // 主要为了满足office2021主题，tab下有个4px的横杠
		}
		int r = qMax(defaultHeight, defaultHeight2);
		if (r < 20) {
			r = 20;
		}
		return r;
	}

	/**
	 * @brief 估算标题栏的高度
	 * @param fm
	 * @return
	 */
	int calcDefaultTitleBarHeight()
	{
		int defaultHeight  = ribbonBar->style()->pixelMetric(QStyle::PM_TitleBarHeight);
		int defaultHeight2 = ribbonBar->fontMetrics().height() * 1.8;
		int r              = qMax(defaultHeight, defaultHeight2);
		if (r < 25) {
			r = 25;
		}
		return r;
	}

	/**
	 * @brief 估算category的高度
	 * @note 经过对照，1.6行高和office的高度比较接近
	 * @param fm
	 * @param s
	 * @return
	 */
	int calcCategoryHeight()
	{
		int textH = ribbonBar->fontMetrics().lineSpacing();  // 这里用linespace，因为在换行的情况下，行距是不可忽略的，ribbon的大按钮默认是2行
		if (ribbonBar->isThreeRowStyle()) {
			// 5.5=（3*1.6+1） （三行）,1是给panneltitle预留的
			return textH * 4.8 + pannelTitleHeight;
		} else {
			// 3=2*1.6
			return textH * 3.2 + pannelTitleHeight;
		}
		return (textH * 4.8 + pannelTitleHeight);
	}

	// 计算MainBar高度
	static int
	calcMainBarHeight(int tabHegith, int titleHeight, int categoryHeight, bool tabOnTitle, SARibbonBar::RibbonMode rMode)
	{
		if (rMode == SARibbonBar::MinimumRibbonMode) {
			// 最小模式，没有categoryHeight
			if (tabOnTitle) {
				return titleHeight;
			} else {
				return titleHeight + tabHegith;
			}
		} else {
			if (tabOnTitle) {
				return titleHeight + categoryHeight;
			} else {
				return tabHegith + titleHeight + categoryHeight;
			}
		}
		return tabHegith + titleHeight + categoryHeight;
	}

	/**
	 * @brief 重新计算尺寸
	 */
	void resetSize()
	{
		estimateSizeHint();
		int mainBarHeight = calcMainBarHeight(getActualTabBarHeight(),
											  getActualTitleBarHeight(),
											  getActualCategoryHeight(),
											  isTabOnTitle,
											  ribbonBar->currentRibbonState());
		// 处于最小模式下时，bar的高度为tabbar的bottom,这个调整必须在resize event之后
		ribbonBar->setFixedHeight(mainBarHeight);
		minHeight = mainBarHeight;  // minHeight和mainBarHeight一致
	}

	int minimumModeMainBarHeight()
	{
		return calcMainBarHeight(getActualTabBarHeight(),
								 getActualTitleBarHeight(),
								 getActualCategoryHeight(),
								 isTabOnTitle,
								 SARibbonBar::MinimumRibbonMode);
	}

	int normalModeMainBarHeight()
	{
		return calcMainBarHeight(getActualTabBarHeight(),
								 getActualTitleBarHeight(),
								 getActualCategoryHeight(),
								 isTabOnTitle,
								 SARibbonBar::NormalRibbonMode);
	}
};

SARibbonBarLayout::SARibbonBarLayout(SARibbonBar* parent) : QLayout(parent), d_ptr(new PrivateData(parent))
{
	init();
}

SARibbonBarLayout::~SARibbonBarLayout()
{
	while (!d_ptr->items.isEmpty()) {
		QLayoutItem* item = d_ptr->items.takeFirst();
		if (item->widget()) {
			item->widget()->hide();
		}
		delete item;
	}
}

void SARibbonBarLayout::init()
{
	// 不需要初始化子控件，它们会从ribbonBar获取
}

void SARibbonBarLayout::addItem(QLayoutItem* item)
{
	d_ptr->items.append(item);
}

QLayoutItem* SARibbonBarLayout::itemAt(int index) const
{
	if (index >= 0 && index < d_ptr->items.size()) {
		return d_ptr->items.at(index);
	}
	return nullptr;
}

QLayoutItem* SARibbonBarLayout::takeAt(int index)
{
	if (index < 0 || index >= d_ptr->items.count()) {
		return nullptr;
	}
	return d_ptr->items.takeAt(index);
}

int SARibbonBarLayout::count() const
{
	return d_ptr->items.size();
}

QSize SARibbonBarLayout::sizeHint() const
{
	int height = d_ptr->minHeight;
	int width  = d_ptr->minWidth;
	if (width > d_ptr->maxMinWidth) {
		width = d_ptr->maxMinWidth;
	}
	return QSize(width, height);
}

QSize SARibbonBarLayout::minimumSize() const
{
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
	qDebug() << "SARibbonBarLayout::minimumSize() = " << sizeHint();
#endif
	return sizeHint();
}

void SARibbonBarLayout::setGeometry(const QRect& rect)
{
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
	qDebug() << "setGeometry=" << rect;
#endif
	QLayout::setGeometry(rect);
	doLayout();
}

void SARibbonBarLayout::doLayout()
{
	if (isLooseStyle()) {
		resizeInLooseStyle();
	} else {
		resizeInCompactStyle();
	}
}

bool SARibbonBarLayout::isLooseStyle() const
{
	return SARibbonBar::isLooseStyle(d_ptr->ribbonStyle());
}

bool SARibbonBarLayout::isCompactStyle() const
{
	return SARibbonBar::isCompactStyle(d_ptr->ribbonStyle());
}

const QRect& SARibbonBarLayout::titleRect() const
{
	return d_ptr->titleRect;
}

/**
 * @brief 计算tabbar的最小尺寸
 *
 * 此函数的作用是在tabbar居中布局时，tabbar不是占满整个宽度，而是按照最小尺寸来占据宽度，让tabbar的长度刚刚好，
 * 这样能让出mainwindow的空间，接受鼠标事件，从而实现拖动等操作，否则tabbar占用整个顶栏，鼠标无法点击到mainwindow
 * @return
 */
int SARibbonBarLayout::calcMinTabBarWidth() const
{
	if (!d_ptr->ribbonTabBar()) {
		return 0;
	}
	const QMargins& mg = d_ptr->ribbonTabBar()->tabMargin();
	return d_ptr->ribbonTabBar()->sizeHint().width() + (mg.left() + mg.right());
}

void SARibbonBarLayout::setSystemButtonSize(const QSize& size)
{
	d_ptr->systemButtonSize = size;
}

void SARibbonBarLayout::setTabOnTitle(bool on)
{
	if (d_ptr->isTabOnTitle != on) {
		d_ptr->isTabOnTitle = on;
		d_ptr->resetSize();
	}
}

/**
 * @brief 设置tab在title上面，这样可以省略title区域
 * @param on
 */
bool SARibbonBarLayout::isTabOnTitle() const
{
    return d_ptr->isTabOnTitle;
}

/**
   @brief 最小模式下的高度
   @return
 */
int SARibbonBarLayout::minimumModeMainBarHeight() const
{
    return d_ptr->minimumModeMainBarHeight();
}

/**
 * @brief 正常模式下的高度
 *
 * 有可能SARibbonBar::height和mainBarHeight不相等，这种情况发生在RibbonState::MinimumRibbonMode状态下
 * @return 高度
 */
int SARibbonBarLayout::normalModeMainBarHeight() const
{
    return d_ptr->normalModeMainBarHeight();
}

/**
   @brief tabBar的高度
   @return
 */
int SARibbonBarLayout::tabBarHeight() const
{
    return d_ptr->getActualTabBarHeight();
}

/**
 * @brief 设置tabBar的高度
 * @param h
 */
void SARibbonBarLayout::setTabBarHeight(int h)
{
    d_ptr->setTabBarHeight(h);
}

/**
   @brief 返回标题栏高度
   @sa setTitleBarHeight
   @return
 */
int SARibbonBarLayout::titleBarHeight() const
{
    return d_ptr->getActualTitleBarHeight();
}

/**
   @brief 设置标题栏的高度
   @sa titleBarHeight
   @note 此操作会发射@ref titleBarHeightChanged 信号
   @param h
 */
void SARibbonBarLayout::setTitleBarHeight(int h)
{
    d_ptr->setTitleBarHeight(h);
}

/**
 * @brief category的高度
 * @return
 */
int SARibbonBarLayout::categoryHeight() const
{
    return d_ptr->getActualCategoryHeight();
}

/**
 * @brief 设置category的高度
 * @param h
 */
void SARibbonBarLayout::setCategoryHeight(int h)
{
    d_ptr->setCategoryHeight(h);
}

/**
 * @brief SARibbonBarLayout::pannelTitleHeight
 * @return
 */
int SARibbonBarLayout::pannelTitleHeight() const
{
    return d_ptr->pannelTitleHeight;
}

/**
 * @brief 设置pannel的高度
 * @param h
 */
void SARibbonBarLayout::setPannelTitleHeight(int h)
{
    d_ptr->pannelTitleHeight = h;
}

/**
 * @brief tab是否在title上面
 * @return
 */
SARibbonBar* SARibbonBarLayout::ribbonBar() const
{
    return d_ptr->ribbonBar;
}

SARibbonTabBar* SARibbonBarLayout::ribbonTabBar() const
{
    return d_ptr->ribbonTabBar();
}

SARibbonStackedWidget* SARibbonBarLayout::stackedContainerWidget() const
{
    return d_ptr->stackedContainerWidget();
}

SARibbonQuickAccessBar* SARibbonBarLayout::quickAccessBar() const
{
    return d_ptr->quickAccessBar();
}

SARibbonButtonGroupWidget* SARibbonBarLayout::rightButtonGroup() const
{
    return d_ptr->rightButtonGroup();
}

QAbstractButton* SARibbonBarLayout::applicationButton() const
{
    return d_ptr->applicationButton();
}

void SARibbonBarLayout::layoutTitleRect()
{
	SARibbonBar* ribbon                    = d_ptr->ribbonBar;
	SARibbonQuickAccessBar* quickAccessBar = d_ptr->quickAccessBar();
	SARibbonTabBar* ribbonTabBar           = d_ptr->ribbonTabBar();
	const QMargins border                  = d_ptr->contentsMargins();
	const int validTitleBarHeight          = d_ptr->getActualTitleBarHeight();

	// 计算标题栏区域
	if (isCompactStyle()) {
		// 紧凑模式,紧凑模式的标题栏在tabbar的剩余空间中
		int titleStart = ribbonTabBar->geometry().right();
		int titleWidth = quickAccessBar ? (quickAccessBar->x() - titleStart)
										: (ribbon->width() - titleStart - d_ptr->systemButtonSize.width());
		if (titleWidth > 10) {
			d_ptr->titleRect = QRect(titleStart, border.top(), titleWidth, validTitleBarHeight);
		} else {
			// 标题栏过小，就不显示
			d_ptr->titleRect = QRect();
		}
	} else {
		const int tabX = ribbonTabBar->geometry().x();
		// 三行宽松模式
		int contextRegionLeft  = ribbon->width();
		int contextRegionRight = -1;

		// 使用上下文标签的视觉数据
		// 上下文标签会占用宽松模式下的标题栏位置，因此，要计算此时标题栏应该在哪里显示
		QList< int > visibleContextIndex = ribbon->currentVisibleContextCategoryTabIndexs();
		if (!visibleContextIndex.empty()) {
			int edgeVal = ribbonTabBar->tabRect(visibleContextIndex.first()).left() + tabX;
			if (edgeVal < contextRegionLeft) {
				contextRegionLeft = edgeVal;
			}
			edgeVal = d_ptr->ribbonTabBar()->tabRect(visibleContextIndex.last()).right() + tabX;
			if (edgeVal > contextRegionRight) {
				contextRegionRight = edgeVal;
			}
		}
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
		qDebug() << "  contextRegionLeft=" << contextRegionLeft << ",contextRegionRight=" << contextRegionRight;
#endif
		int x1 = border.left();
		if (quickAccessBar) {
			x1 = quickAccessBar->geometry().right() + 1;
		}
		int x2 = ribbon->width() - d_ptr->systemButtonSize.width() - border.right();

		if (contextRegionRight < 0) {
			// 说明没有上下文标签，那么标题直接放在quickAccessBar到systembar之间
			d_ptr->titleRect = QRect(QPoint(x1, border.top()), QPoint(x2, validTitleBarHeight + border.top()));
		} else {
			int leftwidth  = contextRegionLeft - x1;
			int rightwidth = x2 - contextRegionRight;
			if (rightwidth > leftwidth) {
				d_ptr->titleRect =
					QRect(QPoint(contextRegionRight, border.top()), QPoint(x2, validTitleBarHeight + border.top()));
			} else {
				d_ptr->titleRect =
					QRect(QPoint(x1, border.top()), QPoint(contextRegionLeft, validTitleBarHeight + border.top()));
			}
		}
	}
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
	qDebug() << "updateTitleRect=" << d_ptr->titleRect;
#endif
}

void SARibbonBarLayout::resetSize()
{
	d_ptr->resetSize();
}

void SARibbonBarLayout::layoutStackedContainerWidget()
{
	SARibbonBar* ribbon                  = ribbonBar();
	SARibbonStackedWidget* stackedWidget = stackedContainerWidget();
	SARibbonTabBar* tabBar               = ribbonTabBar();
	if (!ribbon || !stackedWidget || !tabBar) {
		return;
	}
	QMargins border            = d_ptr->contentsMargins();
	QRect ribbonTabBarGeometry = tabBar->geometry();

	int x = border.left();
	int y = ribbonTabBarGeometry.bottom() + 1;
	int w = ribbon->width() - border.left() - border.right();
	int h = d_ptr->getActualCategoryHeight();

	if (stackedWidget->isPopupMode()) {
		QPoint absPosition = ribbon->mapToGlobal(QPoint(x, y));
		x                  = absPosition.x();
		y                  = absPosition.y();
	}
	// 受布局影响，这里不能使用stackedWidget->setGeometry(x, y, w, h);
	// stackedWidget->setGeometry(x, y, w, h);
	//    stackedWidget->move(x, y);
	//    stackedWidget->setFixedSize(QSize(w, h));
	stackedWidget->setNormalGeometry(QRect(x, y, w, h));
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
	qDebug() << "resizeStackedContainerWidget,stackedWidget Geometry:" << stackedWidget->geometry()
			 << "request set w=" << w << ",h=" << h;
#endif
}

void SARibbonBarLayout::resizeInLooseStyle()
{

	QMargins border = d_ptr->contentsMargins();
	int x           = border.left();
	int y           = border.top();

	const int validTitleBarHeight = d_ptr->getActualTitleBarHeight();
	const int titleBarControlHeight = validTitleBarHeight - 2;  // 标题栏上的控件高度是标题栏高度-2，上下各减1px
	const int tabH                = d_ptr->getActualTabBarHeight();
	const int tabBarControlHeight = tabH - 2;  // tabbar上面的控件高度是tabbar高度-2，上下各减1px
	int barMinWidth               = 0;         ///< 记录ribbonBar的最小宽度，这个用于给推荐宽度
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
	qDebug() << "resizeInLooseStyle,validTitleBarHeight=" << validTitleBarHeight << ",tabH=" << tabH;
#endif
	SARibbonBar* ribbon = ribbonBar();
	/// 1. 布局corner widget
	x += 5;
	if (QWidget* connerL = ribbon->cornerWidget(Qt::TopLeftCorner)) {
		if (connerL->isVisibleTo(ribbon)) {
			QSize connerSize = connerL->sizeHint();
			connerSize       = SA::scaleSizeByHeight(connerSize, validTitleBarHeight);
			connerL->setGeometry(x, y, connerSize.width(), connerSize.height());
			x = connerL->geometry().right() + 5;
		}
	}
	/// 2. 布局quick access bar
	if (auto qb = quickAccessBar()) {
		if (qb->isVisibleTo(ribbon)) {
			QSize quickAccessBarSize = qb->sizeHint();
			quickAccessBarSize       = SA::scaleSizeByHeight(quickAccessBarSize, titleBarControlHeight);
			// 上下留1px的边线
			qb->setGeometry(x, y + 1, quickAccessBarSize.width(), quickAccessBarSize.height());
		}
	}

	/// titleBar上的元素布局完成，开始布局第二行
	x = border.left();
	y += validTitleBarHeight;  // 此时，y值在titlebar下面

	/// 3. 布局 applicationButton
	if (auto appBtn = applicationButton()) {
		if (appBtn->isVisibleTo(ribbon)) {
			QSize appBtnSize = appBtn->sizeHint();
			appBtnSize       = SA::scaleSizeByHeight(appBtnSize, tabBarControlHeight);
			appBtn->setGeometry(x, y + 1, appBtnSize.width(), appBtnSize.height());
			x = appBtn->geometry().right();
			// 累加到最小宽度中
			barMinWidth += appBtnSize.width();
		}
	}

	/// 4. 布局TopRightCorner窗口
	/// cornerWidget(Qt::TopRightCorner)是一定要配置的，对于多文档窗口，子窗口的缩放恢复按钮就是通过这个窗口实现，
	/// 由于这个窗口一定要在最右，因此先对这个窗口进行布局
	int endX = ribbonBar()->width() - border.right();

	if (QWidget* connerTR = ribbon->cornerWidget(Qt::TopRightCorner)) {
		if (connerTR->isVisibleTo(ribbon)) {
			QSize connerSize = connerTR->sizeHint();
			connerSize       = SA::scaleSizeByHeight(connerSize, tabBarControlHeight);
			endX -= connerSize.width();
			connerTR->setGeometry(endX, y + 1, connerSize.width(), connerSize.height());
			// 累加到最小宽度中
			barMinWidth += connerSize.width();
		}
	}

	/// 5. tabBar右边的附加按钮组rightButtonGroup，这里一般会附加一些类似登录等按钮组
	if (auto rightBtnGroup = rightButtonGroup()) {
		if (rightBtnGroup->isVisibleTo(ribbon)) {
			QSize rightBtnGroupSize = rightBtnGroup->sizeHint();
			rightBtnGroupSize       = SA::scaleSizeByHeight(rightBtnGroupSize, tabBarControlHeight);
			endX -= 1;  // 先偏移1px
			endX -= rightBtnGroupSize.width();
			d_ptr->rightButtonGroup()->setGeometry(endX, y + 1, rightBtnGroupSize.width(), rightBtnGroupSize.height());
			// 累加到最小宽度中
			barMinWidth += rightBtnGroupSize.width();
		}
	}

	/// 6.布局tabbar，此时已经确定了applicationButton的位置，以及最右边rightButtonGroup的位置，剩下的区域都给tabbar

	int tabBarWidth = endX - x;
	// 这里极度压缩有可能导致负数因此不能小于0
	if (tabBarWidth < 10) {
		tabBarWidth = 10;
	}
	int mintabBarWidth = calcMinTabBarWidth();
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
	qDebug() << "mintabBarWidth=" << mintabBarWidth;
#endif
	// 累加到最小宽度中
	barMinWidth += mintabBarWidth;
	if (auto tabbar = ribbonTabBar()) {
		if (ribbon->ribbonAlignment() == SARibbonAlignment::AlignLeft) {
			// 左对齐的tabbar，直接设置位置
			tabbar->setGeometry(x, y, tabBarWidth, tabH);
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
			qDebug() << " tabbar->setGeometry x=" << x << ",y=" << y << ",tabBarWidth=" << tabBarWidth << ",tabH=" << tabH;
#endif
		} else {
			// 居中对齐的情况下，Tab要居中显示
			// 得到tab的推荐尺寸
			if (mintabBarWidth >= tabBarWidth) {
				// 这时tabbar没有居中对齐的必要性，空间位置不够了
				tabbar->setGeometry(x, y, tabBarWidth, tabH);
			} else {
				// 说明tabbar的宽度有居中的可能性
				int xoffset = (tabBarWidth - mintabBarWidth) / 2;
				tabbar->setGeometry(x + xoffset, y, mintabBarWidth, tabH);
			}
		}
	}

	d_ptr->minWidth = barMinWidth;
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
	qDebug() << "minWidth=" << barMinWidth;
#endif
	// 5. 更新标题区域
	layoutTitleRect();

	// 6. 调整 stackedContainerWidget
	layoutStackedContainerWidget();
}

void SARibbonBarLayout::resizeInCompactStyle()
{
	QMargins border               = d_ptr->contentsMargins();
	int x                         = border.left();
	int y                         = border.top();
	SARibbonBar* ribbon           = ribbonBar();
	int barMinWidth               = 0;  ///< 记录ribbonBar的最小宽度，这个用于给推荐宽度
	const int validTitleBarHeight = d_ptr->getActualTitleBarHeight();
	const int titleBarControlHeight = validTitleBarHeight - 2;  // 标题栏上的控件高度是标题栏高度-2，上下各减1px
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
	qDebug() << "resizeInCompactStyle,validTitleBarHeight=" << validTitleBarHeight;
#endif
	/// 1.  布局corner widget - TopLeftCorner
	if (QWidget* connerL = ribbon->cornerWidget(Qt::TopRightCorner)) {
		if (connerL->isVisibleTo(ribbon)) {
			QSize connerSize = connerL->sizeHint();
			connerSize       = SA::scaleSizeByHeight(connerSize, validTitleBarHeight);
			connerL->setGeometry(x, y, connerSize.width(), connerSize.height());
			x = connerL->geometry().right();
			// 累加到最小宽度中
			barMinWidth += connerSize.width();
		}
	}

	/// 2.  布局applicationButton
	if (auto appBtn = applicationButton()) {
		if (appBtn->isVisibleTo(ribbon)) {
			QSize appBtnSize = appBtn->sizeHint();
			appBtnSize       = SA::scaleSizeByHeight(appBtnSize, titleBarControlHeight);
			appBtn->setGeometry(x, y + 1, appBtnSize.width(), appBtnSize.height());
			x += appBtn->geometry().right();
			// 累加到最小宽度中
			barMinWidth += appBtnSize.width();
		}
	}

	int endX = ribbon->width() - border.right();
	if (ribbon->isUseRibbonFrame()) {
		endX -= d_ptr->systemButtonSize.width();
	}

	/// 3. 布局corner widget TopRightCorner
	if (QWidget* connerW = ribbon->cornerWidget(Qt::TopRightCorner)) {
		if (connerW->isVisibleTo(ribbon)) {
			QSize connerSize = connerW->sizeHint();
			connerSize       = SA::scaleSizeByHeight(connerSize, validTitleBarHeight);
			endX -= connerSize.width();
			connerW->setGeometry(endX, y, connerSize.width(), connerSize.height());
			// 累加到最小宽度中
			barMinWidth += connerSize.width();
		}
	}

	/// 4. 布局右边按钮组rightButtonGroup
	if (auto rightBtnGroup = rightButtonGroup()) {
		if (rightBtnGroup->isVisibleTo(ribbon)) {
			QSize rightBtnGroupSize = rightBtnGroup->sizeHint();
			rightBtnGroupSize       = SA::scaleSizeByHeight(rightBtnGroupSize, titleBarControlHeight);
			endX -= rightBtnGroupSize.width();
			d_ptr->rightButtonGroup()->setGeometry(endX, y + 1, rightBtnGroupSize.width(), rightBtnGroupSize.height());
			// 累加到最小宽度中
			barMinWidth += rightBtnGroupSize.width();
		}
	}

	/// 5. 布局quick access bar
	if (auto qb = quickAccessBar()) {
		if (qb->isVisibleTo(ribbon)) {
			QSize quickAccessBarSize = qb->sizeHint();
			quickAccessBarSize       = SA::scaleSizeByHeight(quickAccessBarSize, titleBarControlHeight);
			endX -= quickAccessBarSize.width();
			qb->setGeometry(endX, y + 1, quickAccessBarSize.width(), quickAccessBarSize.height());
			// 累加到最小宽度中
			barMinWidth += quickAccessBarSize.width();
		}
	}

	/// 6.布局 tab bar
	int tabH = d_ptr->getActualTabBarHeight();
	if (tabH > validTitleBarHeight) {
		// 这种直接把tabH设置为validTitleBarHeight
		tabH = validTitleBarHeight;
	}
	int tabBarWidth = endX - x;
	// 这里极度压缩有可能导致负数因此不能小于0
	if (tabBarWidth < 10) {
		tabBarWidth = 10;
	}
	// 紧凑模式下，tabbar要用最小宽度布局，否则会遮挡标题栏的点击，导致无法移动
	int mintabBarWidth = calcMinTabBarWidth();
	// 累加到最小宽度中
	barMinWidth += mintabBarWidth;
	if (auto tabbar = ribbonTabBar()) {
		if (ribbon->ribbonAlignment() == SARibbonAlignment::AlignLeft) {
			// 左对齐的tabbar，直接设置位置
			if (mintabBarWidth < tabBarWidth) {
				tabBarWidth = mintabBarWidth;
			}
			tabbar->setGeometry(x, y, tabBarWidth, tabH);
		} else {
			// 居中对齐的情况下，Tab要居中显示
			// 得到tab的推荐尺寸
			int mintabBarWidth = calcMinTabBarWidth();
			if (mintabBarWidth >= tabBarWidth) {
				// 这时tabbar没有居中对齐的必要性，空间位置不够了
				tabbar->setGeometry(x, y, tabBarWidth, tabH);
			} else {
				// 说明tabbar的宽度有居中的可能性
				int xoffset = (tabBarWidth - mintabBarWidth) / 2;
				tabbar->setGeometry(x + xoffset, y, mintabBarWidth, tabH);
			}
		}
	}

	d_ptr->minWidth = barMinWidth;
#if SARIBBONBARLAYOUT_ENABLE_DEBUG_PRINT
	qDebug() << "minWidth=" << barMinWidth;
#endif
	// 5. 更新标题区域
	layoutTitleRect();
	// 6. 调整 stackedContainerWidget
	layoutStackedContainerWidget();
}
