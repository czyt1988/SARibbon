#include "SARibbonCategoryLayout.h"
#include <QLayoutItem>
#include "SARibbonPanel.h"
#include "SARibbonElementManager.h"
#include "SARibbonSeparatorWidget.h"
#include <QApplication>
#include <QPropertyAnimation>

#ifndef SARibbonCategoryLayout_DEBUG_PRINT
#define SARibbonCategoryLayout_DEBUG_PRINT 0
#endif
#if SARibbonCategoryLayout_DEBUG_PRINT
#include <QDebug>
#endif
/**
 * @brief The SARibbonCategoryLayoutPrivate class
 */
class SARibbonCategoryLayout::PrivateData
{
	SA_RIBBON_DECLARE_PUBLIC(SARibbonCategoryLayout)
public:
	PrivateData(SARibbonCategoryLayout* p);
	// 计算所有元素的sizehint总宽度
	int totalSizeHintWidth() const;

public:
	bool mDirty { true };
	bool mIsRightScrollBtnShow { false };                       ///< 标记右滚动按钮是否需要显示
	bool mIsLeftScrollBtnShow { false };                        ///< 标记左滚动按钮是否需要显示
	SARibbonCategoryScrollButton* mLeftScrollBtn { nullptr };   ///< 在区域无法显示时显示的按钮
	SARibbonCategoryScrollButton* mRightScrollBtn { nullptr };  ///< 在区域无法显示时显示的按钮
	int mTotalWidth { 0 };
	int mXBase { 0 };
	QSize mSizeHint;
	QSize mMinSizeHint;
	QList< SARibbonCategoryLayoutItem* > mItemList;
	SARibbonAlignment mCategoryAlignment { SARibbonAlignment::AlignLeft };  ///< 对齐方式
	// 动画相关
	QPropertyAnimation* mScrollAnimation { nullptr };
	int mTargetScrollPosition { 0 };
};

//=============================================================
// SARibbonCategoryLayoutPrivate
//=============================================================

SARibbonCategoryLayout::PrivateData::PrivateData(SARibbonCategoryLayout* p) : q_ptr(p)
{
}

/**
 * @brief 计算所有元素的SizeHint宽度总和
 * @return
 */
int SARibbonCategoryLayout::PrivateData::totalSizeHintWidth() const
{
	int total    = 0;
	QMargins mag = q_ptr->contentsMargins();
#if SARibbonCategoryLayout_DEBUG_PRINT
	int debug_i__ = 0;
	QString debug_totalSizeHintWidth__;
#endif
	if (!mag.isNull()) {
		total += (mag.left() + mag.right());
	}
	// 先计算总长
	for (SARibbonCategoryLayoutItem* item : qAsConst(mItemList)) {
		if (item->isEmpty()) {
// 如果是hide就直接跳过
#if SARibbonCategoryLayout_DEBUG_PRINT
			++debug_i__;
			debug_totalSizeHintWidth__ +=
				QString("   [%1](%2)is empty skip\n").arg(debug_i__).arg(item->toPanelWidget()->panelName());
#endif
			continue;
		}
		// 这里要使用widget()->sizeHint()，因为panel的标题会影总体布局，此处需要修改
		//  TODO
		QSize panelSize = item->widget()->sizeHint();
		QSize SeparatorSize(0, 0);
		if (item->separatorWidget) {
			SeparatorSize = item->separatorWidget->sizeHint();
		}
		total += panelSize.width();
		total += SeparatorSize.width();
#if SARibbonCategoryLayout_DEBUG_PRINT
		++debug_i__;
		debug_totalSizeHintWidth__ += QString("|-[%1]panelSize=(%2,%3),SeparatorSize=(%4,%5),name=(%6) \n")
										  .arg(debug_i__)
										  .arg(panelSize.width())
										  .arg(panelSize.height())
										  .arg(SeparatorSize.width())
										  .arg(SeparatorSize.height())
										  .arg(item->toPanelWidget()->panelName());
#endif
	}
#if SARibbonCategoryLayout_DEBUG_PRINT
	qDebug() << "SARibbonCategoryLayout.totalSizeHintWidth=" << total;
	qDebug().noquote() << debug_totalSizeHintWidth__;
#endif
	return (total);
}

//=============================================================
// SARibbonCategoryLayout
//=============================================================

SARibbonCategoryLayout::SARibbonCategoryLayout(SARibbonCategory* parent)
    : QLayout(parent), d_ptr(new SARibbonCategoryLayout::PrivateData(this))
{
	setContentsMargins(1, 1, 1, 1);
	d_ptr->mLeftScrollBtn  = new SARibbonCategoryScrollButton(Qt::LeftArrow, parent);
	d_ptr->mRightScrollBtn = new SARibbonCategoryScrollButton(Qt::RightArrow, parent);
	d_ptr->mLeftScrollBtn->setVisible(false);
	d_ptr->mRightScrollBtn->setVisible(false);
	connect(d_ptr->mLeftScrollBtn, &QToolButton::clicked, this, &SARibbonCategoryLayout::onLeftScrollButtonClicked);
	connect(d_ptr->mRightScrollBtn, &QToolButton::clicked, this, &SARibbonCategoryLayout::onRightScrollButtonClicked);
	setupAnimateScroll();
}

SARibbonCategoryLayout::~SARibbonCategoryLayout()
{
	while (auto item = takePanelItem(0)) {
		delete item;
	}
}

SARibbonCategory* SARibbonCategoryLayout::ribbonCategory() const
{
	return (qobject_cast< SARibbonCategory* >(parentWidget()));
}

void SARibbonCategoryLayout::addItem(QLayoutItem* item)
{
	Q_UNUSED(item);
	qWarning() << tr("in SARibbonCategoryLayout cannot addItem,use addPanel instead");
	invalidate();
}

/**
 * @brief 返回panel的layout
 * @param index 索引
 * @return
 * @note 注意，panel是和分割线一起的，但这个只返回一个panel对应的layout
 */
QLayoutItem* SARibbonCategoryLayout::itemAt(int index) const
{
	SARibbonCategoryLayoutItem* item = d_ptr->mItemList.value(index, nullptr);

	return (item);
}

/**
 * @brief 提取layout
 * @param index
 * @return
 */
QLayoutItem* SARibbonCategoryLayout::takeAt(int index)
{
	QLayoutItem* r = takePanelItem(index);
	invalidate();
	return r;
}

SARibbonCategoryLayoutItem* SARibbonCategoryLayout::takePanelItem(int index)
{
	if ((index >= 0) && (index < d_ptr->mItemList.size())) {
		SARibbonCategoryLayoutItem* item = d_ptr->mItemList.takeAt(index);
		if (item->widget()) {
			item->widget()->hide();
		}
		if (item->separatorWidget) {
			item->separatorWidget->hide();
		}
		return (item);
	}
	return (nullptr);
}

SARibbonCategoryLayoutItem* SARibbonCategoryLayout::takePanelItem(SARibbonPanel* panel)
{
	for (int i = 0; i < d_ptr->mItemList.size(); ++i) {
		SARibbonCategoryLayoutItem* item = d_ptr->mItemList[ i ];
		if (item->widget() == panel) {
			return (takePanelItem(i));
		}
	}
	return (nullptr);
}

/**
 * @brief 移除panel，对应的分割线也会删除
 * @param panel
 * @return
 */
bool SARibbonCategoryLayout::takePanel(SARibbonPanel* panel)
{
	SARibbonCategoryLayoutItem* i = takePanelItem(panel);
	if (i) {
		SARibbonSeparatorWidget* sp = i->separatorWidget;
		if (sp) {
			sp->deleteLater();
		}
		delete i;
		invalidate();
		return true;
	}
	return false;
}

int SARibbonCategoryLayout::count() const
{
	return (d_ptr->mItemList.size());
}

void SARibbonCategoryLayout::addPanel(SARibbonPanel* panel)
{
    insertPanel(d_ptr->mItemList.count(), panel);
}

/**
 * @brief 插入一个panel
 * @param index 索引
 * @param panel
 * @return 返回对应的分割线SARibbonSeparatorWidget
 * @note 在SARibbonCategoryLayout的布局中，一个panel会携带一个分割线
 */
void SARibbonCategoryLayout::insertPanel(int index, SARibbonPanel* panel)
{
	index                            = qMax(0, index);
	index                            = qMin(d_ptr->mItemList.count(), index);
	SARibbonCategoryLayoutItem* item = new SARibbonCategoryLayoutItem(panel);

	// 分割线
	item->separatorWidget = RibbonSubElementFactory->createRibbonSeparatorWidget(parentWidget());
	// 插入list中
	d_ptr->mItemList.insert(index, item);
	// 标记需要重新计算尺寸
	invalidate();
}

QSize SARibbonCategoryLayout::sizeHint() const
{
	if (d_ptr->mSizeHint.isNull()) {
		SARibbonCategoryLayout* that = const_cast< SARibbonCategoryLayout* >(this);
		that->updateGeometryArr();
	}
	return (d_ptr->mSizeHint);
}

QSize SARibbonCategoryLayout::minimumSize() const
{
	if (d_ptr->mMinSizeHint.isNull()) {
		SARibbonCategoryLayout* that = const_cast< SARibbonCategoryLayout* >(this);
		that->updateGeometryArr();
	}
	return (d_ptr->mMinSizeHint);
}

/**
 * @brief SARibbonCategory充满整个stacked widget
 * @return
 */
Qt::Orientations SARibbonCategoryLayout::expandingDirections() const
{
    return (Qt::Horizontal | Qt::Vertical);
}

void SARibbonCategoryLayout::invalidate()
{
	d_ptr->mDirty = true;
	QLayout::invalidate();
}
/**
 * @brief category的内容尺寸（把margins减去）
 * @return
 */
QSize SARibbonCategoryLayout::categoryContentSize() const
{
	SARibbonCategory* category = ribbonCategory();
	QSize s                    = category->size();
	QMargins mag               = contentsMargins();
	if (!mag.isNull()) {
		s.rheight() -= (mag.top() + mag.bottom());
		s.rwidth() -= (mag.right() + mag.left());
	}
	return (s);
}

/**
 * @brief 更新尺寸
 */
void SARibbonCategoryLayout::updateGeometryArr()
{
	SARibbonCategory* category = ribbonCategory();
	if (nullptr == category) {
		return;
	}
	int categoryWidth = category->width();
	QMargins mag      = contentsMargins();
	int height        = category->height();
	int y             = 0;

	if (!mag.isNull()) {
		y = mag.top();
		height -= (mag.top() + mag.bottom());
		// categoryWidth不能把mag减去，减去后会导致categoryWidth不是实际的categoryWidth
		// categoryWidth -= (mag.right() + mag.left());
	}
	// total 是总宽，不是x坐标系，x才是坐标系
	int total = d_ptr->totalSizeHintWidth();

	// 扩展的宽度
	int expandWidth = 0;

	// 判断是否需要滚动，总长度超过宽度就需要滚动
	bool needsScrolling = (total > categoryWidth);

#if SARibbonCategoryLayout_DEBUG_PRINT
	qDebug() << "SARibbonCategoryLayout::updateGeometryArr" << "\n  |-category name=" << category->categoryName()  //
			 << "\n  |-category height=" << height                                                                 //
			 << "\n  |-totalSizeHintWidth=" << total                                                               //
			 << "\n  |-y=" << y                                                                                    //
			 << "\n  |-expandWidth:" << expandWidth                                                                //
			 << "\n  |-mag=" << mag;
#endif

	if (needsScrolling) {
		// 超过总长度，需要显示滚动按钮
		if (0 == d_ptr->mXBase) {
			// 已经移动到最左，需要可以向右移动
			d_ptr->mIsRightScrollBtnShow = true;
			d_ptr->mIsLeftScrollBtnShow  = false;
		} else if (d_ptr->mXBase <= (categoryWidth - total)) {
			// 已经移动到最右，需要可以向左移动
			d_ptr->mIsRightScrollBtnShow = false;
			d_ptr->mIsLeftScrollBtnShow  = true;
		} else {
			// 移动到中间两边都可以动
			d_ptr->mIsRightScrollBtnShow = true;
			d_ptr->mIsLeftScrollBtnShow  = true;
		}
	} else {
		// 说明total 小于 categoryWidth
		// 记录可以扩展的数量
		int canExpandingCount        = 0;
		d_ptr->mIsRightScrollBtnShow = false;
		d_ptr->mIsLeftScrollBtnShow  = false;
		// 这个是避免一开始totalWidth > categorySize.width()，通过滚动按钮调整了m_d->mBaseX
		// 随之调整了窗体尺寸，调整后totalWidth < categorySize.width()导致category在原来位置
		// 无法显示，必须这里把mBaseX设置为0

		d_ptr->mXBase = 0;
		//

		for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
			if (SARibbonPanel* p = qobject_cast< SARibbonPanel* >(item->widget())) {
				if (p->isExpanding()) {
					// panel可扩展
					++canExpandingCount;
				}
			}
		}
		// 计算可扩展的宽度
		if (canExpandingCount > 0) {
			expandWidth = (categoryWidth - total) / canExpandingCount;
		} else {
			expandWidth = 0;
		}
	}
	int x = d_ptr->mXBase;
	if ((categoryAlignment() == SARibbonAlignment::AlignCenter) && (total < categoryWidth) && (0 == expandWidth)) {
		// 如果是居中对齐，同时没有伸缩的panel，同时总宽度没有超过category的宽度
		x = (categoryWidth - total) / 2;
	}
	total = 0;  // total重新计算
	// 先按照sizeHint设置所有的尺寸
	for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
		if (item->isEmpty()) {
			// 如果是hide就直接跳过
			if (item->separatorWidget) {
				// panel hide分割线也要hide
				item->separatorWidget->hide();
			}
			item->mWillSetGeometry          = QRect(0, 0, 0, 0);
			item->mWillSetSeparatorGeometry = QRect(0, 0, 0, 0);
			continue;
		}
		SARibbonPanel* p = item->toPanelWidget();
		if (nullptr == p) {
			qDebug() << "unknow widget in SARibbonCategoryLayout";
			continue;
		}
		// p->layout()->update();
		QSize panelSize = p->sizeHint();
		QSize SeparatorSize(0, 0);
		if (item->separatorWidget) {
			SeparatorSize = item->separatorWidget->sizeHint();
		}
		if (p->isExpanding()) {
			// 可扩展，就把panel扩展到最大
			panelSize.setWidth(panelSize.width() + expandWidth);
		}
		int w = panelSize.width();

		item->mWillSetGeometry = QRect(x, y, w, height);
		x += w;
		total += w;
		w                               = SeparatorSize.width();
		item->mWillSetSeparatorGeometry = QRect(x, y, w, height);
		x += w;
		total += w;
	}
	d_ptr->mTotalWidth  = total;
	d_ptr->mSizeHint    = QSize(d_ptr->mTotalWidth, height);
	d_ptr->mMinSizeHint = QSize(categoryWidth, height);
#if SARibbonCategoryLayout_DEBUG_PRINT
	qDebug() << "  SARibbonCategoryLayout updateGeometryArr,SizeHint=" << d_ptr->mSizeHint
			 << ",Category name=" << category->categoryName();
#endif
}

/**
 * @brief 执行layout调整
 */
void SARibbonCategoryLayout::doLayout()
{
	if (d_ptr->mDirty) {
		updateGeometryArr();
	}
	if (d_ptr->mItemList.isEmpty()) {
		if (d_ptr->mLeftScrollBtn->isVisible()) {
			d_ptr->mLeftScrollBtn->hide();
		}
		if (d_ptr->mRightScrollBtn->isVisible()) {
			d_ptr->mRightScrollBtn->hide();
		}
		return;
	}
	SARibbonCategory* category = ribbonCategory();
	// 两个滚动按钮的位置永远不变
	d_ptr->mLeftScrollBtn->setGeometry(0, 0, 12, category->height());
	d_ptr->mRightScrollBtn->setGeometry(category->width() - 12, 0, 12, category->height());
	QList< QWidget* > showWidgets, hideWidgets;
#if SARibbonCategoryLayout_DEBUG_PRINT
	int debug_i__(0);
	qDebug() << "SARibbonCategoryLayout::doLayout(),name=" << category->categoryName();
#endif
	const int itemsize = d_ptr->mItemList.size();
	for (int i = 0; i < itemsize; ++i) {
		SARibbonCategoryLayoutItem* item = d_ptr->mItemList[ i ];
		if (item->isEmpty()) {
			hideWidgets << item->widget();
			if (item->separatorWidget) {
				hideWidgets << item->separatorWidget;
			}
#if SARibbonCategoryLayout_DEBUG_PRINT
			qDebug() << "  |-[" << debug_i__ << "]panelName(" << item->toPanelWidget()->panelName() << ",will hide";
			++debug_i__;
#endif
		} else {
			//! 这里不能用item->setGeometry(item->mWillSetGeometry);这样会得到一个很奇怪的显示效果
			//! 就是窗口的最左边不会移出去，而是把最右边压缩，
			item->widget()->setGeometry(item->mWillSetGeometry);
			//            item->widget()->setFixedSize(item->mWillSetGeometry.size());
			//            item->widget()->move(item->mWillSetGeometry.topLeft());
			//            item->setGeometry(item->mWillSetGeometry);
			showWidgets << item->widget();
			if (item->separatorWidget) {
				item->separatorWidget->setGeometry(item->mWillSetSeparatorGeometry);
				if (i == itemsize - 1) {
					// 最后一个panel的分割线隐藏
					hideWidgets << item->separatorWidget;
				} else {
					showWidgets << item->separatorWidget;
				}
			}
#if SARibbonCategoryLayout_DEBUG_PRINT
			qDebug() << "  |-[" << debug_i__ << "]panelName(" << item->toPanelWidget()->panelName()
					 << "),willSetGeometry:" << item->mWillSetGeometry
					 << ",WillSetSeparatorGeometry:" << item->mWillSetSeparatorGeometry;
			++debug_i__;
#endif
		}
	}

	d_ptr->mRightScrollBtn->setVisible(d_ptr->mIsRightScrollBtnShow);
	d_ptr->mLeftScrollBtn->setVisible(d_ptr->mIsLeftScrollBtnShow);
	if (d_ptr->mIsRightScrollBtnShow) {
		d_ptr->mRightScrollBtn->raise();
	}
	if (d_ptr->mIsLeftScrollBtnShow) {
		d_ptr->mLeftScrollBtn->raise();
	}
	// 不在上面那里进行show和hide因为这会触发SARibbonPanelLayout的重绘，导致循环绘制，非常影响效率
	for (QWidget* w : qAsConst(showWidgets)) {
		if (!w->isVisible()) {
			w->show();
		}
	}
	for (QWidget* w : qAsConst(hideWidgets)) {
		if (w->isVisible()) {
			w->hide();
		}
	}
	// 最后一个分割线隐藏
}

/**
 * @brief 返回所有panels
 * @return
 */
QList< SARibbonPanel* > SARibbonCategoryLayout::panels() const
{
	QList< SARibbonPanel* > res;

	for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
		SARibbonPanel* p = item->toPanelWidget();
		res.append(p);
	}
	return (res);
}

/**
 * @brief 通过ObjectName查找panel
 * @param objname
 * @return
 */
SARibbonPanel* SARibbonCategoryLayout::panelByObjectName(const QString& objname) const
{
	for (SARibbonCategoryLayoutItem* item : d_ptr->mItemList) {
		if (SARibbonPanel* panel = item->toPanelWidget()) {
			if (panel->objectName() == objname) {
				return panel;
			}
		}
	}
	return nullptr;
}

/**
 * @brief 通过名字查找panel
 * @param title
 * @return 如果有重名，只会返回第一个符合条件的
 */
SARibbonPanel* SARibbonCategoryLayout::panelByName(const QString& panelname) const
{
	for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
		if (SARibbonPanel* panel = item->toPanelWidget()) {
			if (panel->panelName() == panelname) {
				return (panel);
			}
		}
	}
	return (nullptr);
}

/**
 * @brief 通过索引找到panel，如果超过索引范围，会返回nullptr
 * @param i
 * @return
 */
SARibbonPanel* SARibbonCategoryLayout::panelByIndex(int i) const
{
	if (i >= 0 && i < d_ptr->mItemList.size()) {
		return d_ptr->mItemList[ i ]->toPanelWidget();
	}
	return nullptr;
}

/**
 * @brief 移动panel
 * @param from
 * @param to
 */
void SARibbonCategoryLayout::movePanel(int from, int to)
{
	d_ptr->mItemList.move(from, to);
	doLayout();
}

/**
 * @brief 返回panel的个数
 * @return
 */
int SARibbonCategoryLayout::panelCount() const
{
    return d_ptr->mItemList.size();
}

/**
 * @brief 查找panel对应的索引
 * @param p
 * @return 如果找不到，返回-1
 */
int SARibbonCategoryLayout::panelIndex(SARibbonPanel* p) const
{
	int c = panelCount();

	for (int i = 0; i < c; ++i) {
		if (d_ptr->mItemList[ i ]->toPanelWidget() == p) {
			return (i);
		}
	}
	return (-1);
}

/**
 * @brief 获取所有的panel
 * @return
 */
QList< SARibbonPanel* > SARibbonCategoryLayout::panelList() const
{
	QList< SARibbonPanel* > res;

	for (SARibbonCategoryLayoutItem* i : qAsConst(d_ptr->mItemList)) {
		if (SARibbonPanel* p = i->toPanelWidget()) {
			res.append(p);
		}
	}
	return (res);
}

/**
 * @brief 执行滚动
 * @param px
 */
void SARibbonCategoryLayout::scroll(int px)
{
	// 计算新位置
	int targetX = d_ptr->mXBase + px;
	// 直接设置位置
	scrollTo(targetX);
}

/**
 * @brief 滚动到指定位置
 * @param targetX
 */
void SARibbonCategoryLayout::scrollTo(int targetX)
{
    setScrollPosition(targetX);
}

/**
 * @brief 带动画的滚动
 * @param px
 */
void SARibbonCategoryLayout::scrollByAnimate(int px)
{
	int targetX = d_ptr->mXBase + px;
	scrollToByAnimate(targetX);
}

/**
 * @brief 滚动到指定位置，带动画
 * @param px
 */
void SARibbonCategoryLayout::scrollToByAnimate(int targetX)
{
	QPropertyAnimation* animation = d_ptr->mScrollAnimation;
	if (!animation) {
		scrollTo(targetX);
	}
	if (isAnimatingScroll() && targetX == d_ptr->mTargetScrollPosition) {
		return;  // 已经是目标位置
	}
	// 计算边界
	const int availableWidth     = categoryContentSize().width();
	const int minBase            = qMin(availableWidth - d_ptr->mTotalWidth, 0);
	d_ptr->mTargetScrollPosition = qBound(minBase, targetX, 0);

	// 如果动画正在进行，停止当前动画
	if (animation->state() == QPropertyAnimation::Running) {
		animation->stop();
	}

	// 设置动画参数
	animation->setStartValue(d_ptr->mXBase);
	animation->setEndValue(d_ptr->mTargetScrollPosition);
	animation->start();
}

/**
 * @brief 滚动后的位置
 * @return
 */
int SARibbonCategoryLayout::scrollPosition() const
{
    return d_ptr->mXBase;
}

/**
 * @brief 设置滚动位置
 * @param px
 */
void SARibbonCategoryLayout::setScrollPosition(int pos)
{
	// 边界检查
	const int availableWidth = categoryContentSize().width();
	const int minBase        = qMin(availableWidth - d_ptr->mTotalWidth, 0);
	const int newXBase       = qBound(minBase, pos, 0);

	if (d_ptr->mXBase != newXBase) {
		d_ptr->mXBase = newXBase;
		invalidate();  // 标记需要重新布局

		// 立即执行布局更新（而不是等待事件循环）
		if (parentWidget()) {
			// setGeometry(parentWidget()->geometry());
			parentWidget()->update();
		}
	}
}

/**
 * @brief 判断是否在滚动动画中
 * @return
 */
bool SARibbonCategoryLayout::isAnimatingScroll() const
{
    return d_ptr->mScrollAnimation->state() == QPropertyAnimation::Running;
}

/**
 * @brief 判断是否滚动过
 * @return
 */
bool SARibbonCategoryLayout::isScrolled() const
{
    return (d_ptr->mXBase != 0);
}

/**
 * @brief 这个宽度是实际内容的宽度，有可能大于size.width，也有可能小于
 * @return
 */
int SARibbonCategoryLayout::categoryTotalWidth() const
{
    return d_ptr->mTotalWidth;
}

/**
   @brief 设置Category的对齐方式

   居中对齐会让panel以居中进行对齐
   @param al
 */
void SARibbonCategoryLayout::setCategoryAlignment(SARibbonAlignment al)
{
	if (d_ptr->mCategoryAlignment != al) {
		d_ptr->mCategoryAlignment = al;
	}
}

/**
   @brief Category的对齐方式
   @return
 */
SARibbonAlignment SARibbonCategoryLayout::categoryAlignment() const
{
    return d_ptr->mCategoryAlignment;
}

/**
 * @brief 设置动画的持续时间
 * @param duration 毫秒
 */
void SARibbonCategoryLayout::setAnimationDuration(int duration)
{
	if (d_ptr->mScrollAnimation) {
		d_ptr->mScrollAnimation->setDuration(qMax(50, duration));  // 最小50ms
	}
}

/**
 * @brief 动画的持续时间，默认300ms
 * @return 如果没有设置动画，返回-1
 */
int SARibbonCategoryLayout::animationDuration() const
{
	if (d_ptr->mScrollAnimation) {
		return d_ptr->mScrollAnimation->duration();
	}
	return -1;
}

void SARibbonCategoryLayout::setupAnimateScroll()
{
	if (!d_ptr->mScrollAnimation) {
		// 初始化滚动动画
		d_ptr->mScrollAnimation = new QPropertyAnimation(this, "scrollPosition", this);
		d_ptr->mScrollAnimation->setDuration(300);                       // 动画时长300ms
		d_ptr->mScrollAnimation->setEasingCurve(QEasingCurve::OutQuad);  // 缓动曲线
		d_ptr->mTargetScrollPosition = d_ptr->mXBase;
	}
}

void SARibbonCategoryLayout::onLeftScrollButtonClicked()
{
	SARibbonCategory* category = qobject_cast< SARibbonCategory* >(parentWidget());
	int width                  = category->width();
	width /= 2;
	scrollByAnimate(width);
}

void SARibbonCategoryLayout::onRightScrollButtonClicked()
{
	SARibbonCategory* category = qobject_cast< SARibbonCategory* >(parentWidget());
	int width                  = category->width();
	width /= 2;
	scrollByAnimate(-width);
}

void SARibbonCategoryLayout::setGeometry(const QRect& rect)
{
	QRect old = geometry();
	if (old == rect) {
		return;
	}
#if SARibbonCategoryLayout_DEBUG_PRINT
	qDebug() << "===========SARibbonCategoryLayout.setGeometry(" << rect << "(" << ribbonCategory()->categoryName()
			 << ")=======";
#endif
	QLayout::setGeometry(rect);
	d_ptr->mDirty = false;
	updateGeometryArr();
	doLayout();
}
//=============================================================
// SARibbonCategoryLayoutItem
//=============================================================

SARibbonCategoryLayoutItem::SARibbonCategoryLayoutItem(SARibbonPanel* w) : QWidgetItem(w)
{
	separatorWidget = nullptr;
}

SARibbonCategoryLayoutItem::~SARibbonCategoryLayoutItem()
{
}

SARibbonPanel* SARibbonCategoryLayoutItem::toPanelWidget()
{
	return qobject_cast< SARibbonPanel* >(widget());
}
