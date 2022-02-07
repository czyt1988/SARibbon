#include "SARibbonCategoryLayout.h"
#include <QLayoutItem>
#include "SARibbonPannel.h"
#include "SARibbonToolButton.h"
#include "SARibbonElementManager.h"
#include "SARibbonSeparatorWidget.h"
#include <QApplication>
#include <QDebug>

/**
 * @brief The SARibbonCategoryLayoutPrivate class
 */
class SARibbonCategoryLayoutPrivate {
public:
	SARibbonCategoryLayoutPrivate(SARibbonCategoryLayout *par);
	int totalSizeHintWidth() const;

public:
	SARibbonCategoryLayout *q_d;
	bool mDirty;
	QList<SARibbonCategoryLayoutItem *> mItemList;
	QSize mSizeHint;
	SARibbonCategoryScrollButton *mLeftScrollBtn;
	SARibbonCategoryScrollButton *mRightScrollBtn;
	int mTotalWidth;
	int mXBase;
	bool mIsRightScrollBtnShow;
	bool mIsLeftScrollBtnShow;
};

//=============================================================
//SARibbonCategoryLayoutItem
//=============================================================

SARibbonCategoryLayoutItem::SARibbonCategoryLayoutItem(SARibbonPannel *w) : QWidgetItem(w)
{
	separatorWidget = nullptr;
}


//=============================================================
//SARibbonCategoryLayoutPrivate
//=============================================================

SARibbonCategoryLayoutPrivate::SARibbonCategoryLayoutPrivate(SARibbonCategoryLayout *par)
	: q_d(par)
	, mDirty(true)
	, mSizeHint(50, 50)
	, mLeftScrollBtn(nullptr)
	, mRightScrollBtn(nullptr)
	, mIsRightScrollBtnShow(false)
	, mIsLeftScrollBtnShow(false)
{
}


int SARibbonCategoryLayoutPrivate::totalSizeHintWidth() const
{
	int total = 0;
	QMargins mag = q_d->contentsMargins();

	if (!mag.isNull()) {
		total += (mag.left() + mag.right());
	}
	for (SARibbonCategoryLayoutItem *item : mItemList)
	{
		if (item->isEmpty()) {
			continue;
		}
		QSize pannelSize = item->widget()->sizeHint();
		QSize SeparatorSize(0, 0);
		if (item->separatorWidget) {
			SeparatorSize = item->separatorWidget->sizeHint();
		}
		total += pannelSize.width();
		total += SeparatorSize.width();
	}
	return (total);
}


//=============================================================
//SARibbonCategoryLayout
//=============================================================

SARibbonCategoryLayout::SARibbonCategoryLayout(SARibbonCategory *parent) : QLayout(parent)
, m_d(new SARibbonCategoryLayoutPrivate(this))
{
	setContentsMargins(1, 1, 1, 1);
	m_d->mLeftScrollBtn = new SARibbonCategoryScrollButton(Qt::LeftArrow, parent);
	m_d->mRightScrollBtn = new SARibbonCategoryScrollButton(Qt::RightArrow, parent);
	m_d->mLeftScrollBtn->setVisible(false);
	m_d->mRightScrollBtn->setVisible(false);
	connect(m_d->mLeftScrollBtn, &QToolButton::clicked, this, &SARibbonCategoryLayout::onLeftScrollButtonClicked);
	connect(m_d->mRightScrollBtn, &QToolButton::clicked, this, &SARibbonCategoryLayout::onRightScrollButtonClicked);
}


SARibbonCategoryLayout::~SARibbonCategoryLayout()
{
	QLayoutItem *item = Q_NULLPTR;

	while ((item = takeAt(0)))
	{
		delete item;
	}
	delete m_d;
}


SARibbonCategory *SARibbonCategoryLayout::ribbonCategory()
{
	return (qobject_cast<SARibbonCategory *>(parentWidget()));
}


void SARibbonCategoryLayout::addItem(QLayoutItem *item)
{
	Q_UNUSED(item);
	qWarning() << tr("in SARibbonCategoryLayout cannot addItem,use addPannel instead");
	invalidate();
}

QLayoutItem *SARibbonCategoryLayout::itemAt(int index) const
{
	SARibbonCategoryLayoutItem *item = m_d->mItemList.value(index, nullptr);

	return (item);
}


QLayoutItem *SARibbonCategoryLayout::takeAt(int index)
{
	return (takePannelItem(index));
}


SARibbonCategoryLayoutItem *SARibbonCategoryLayout::takePannelItem(int index)
{
	if ((index >= 0) && (index < m_d->mItemList.size())) {
		invalidate();
		SARibbonCategoryLayoutItem *item = m_d->mItemList.takeAt(index);
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


SARibbonCategoryLayoutItem *SARibbonCategoryLayout::takePannel(SARibbonPannel *pannel)
{
	for (int i = 0; i < m_d->mItemList.size(); ++i)
	{
		SARibbonCategoryLayoutItem *item = m_d->mItemList[i];
		if (item->widget() == pannel) {
			return (takePannelItem(i));
		}
	}
	return (nullptr);
}


int SARibbonCategoryLayout::count() const
{
	return (m_d->mItemList.size());
}


void SARibbonCategoryLayout::addPannel(SARibbonPannel *pannel)
{
	insertPannel(m_d->mItemList.count(), pannel);
}


void SARibbonCategoryLayout::insertPannel(int index, SARibbonPannel *pannel)
{
	index = qMax(0, index);
	index = qMin(m_d->mItemList.count(), index);
	SARibbonCategoryLayoutItem *item = new SARibbonCategoryLayoutItem(pannel);

	item->separatorWidget = RibbonSubElementDelegate->createRibbonSeparatorWidget(parentWidget());
	m_d->mItemList.insert(index, item);
	invalidate();
}


QSize SARibbonCategoryLayout::sizeHint() const
{
	return (m_d->mSizeHint);
}


QSize SARibbonCategoryLayout::minimumSize() const
{
	return (m_d->mSizeHint);
}


Qt::Orientations SARibbonCategoryLayout::expandingDirections() const
{
	return (Qt::Horizontal | Qt::Vertical);
}


void SARibbonCategoryLayout::invalidate()
{
	m_d->mDirty = true;
	QLayout::invalidate();
}


void SARibbonCategoryLayout::updateGeometryArr()
{
	SARibbonCategory *category = qobject_cast<SARibbonCategory *>(parentWidget());

	int categoryWidth = category->width();
	QMargins mag = contentsMargins();
	int height = category->height();
	int y = 0;

	if (!mag.isNull()) {
		y = mag.top();
		height -= (mag.top() + mag.bottom());
		categoryWidth -= (mag.right() + mag.left());
	}
	int total = m_d->totalSizeHintWidth();
	int canExpandingCount = 0;
	int expandWidth = 0;

#ifdef SA_RIBBON_DEBUG_HELP_DRAW
	qDebug() << "\r\n\r\n============================================="
		<< "\r\nSARibbonCategoryLayout::updateGeometryArr"
		<< "\r\npannel name:" << category->windowTitle()
		<< "\r\n height:" << height
		<< "\r\n first total:" << total
		<< "\r\n y:" << y
		<< "\r\n expandWidth:" << expandWidth
		;
#endif
	if (total > categoryWidth) {
		if (0 == m_d->mXBase) {
			m_d->mIsRightScrollBtnShow = true;
			m_d->mIsLeftScrollBtnShow = false;
		}
		else if (m_d->mXBase <= (categoryWidth - total)) {
			m_d->mIsRightScrollBtnShow = false;
			m_d->mIsLeftScrollBtnShow = true;
		}
		else {
			m_d->mIsRightScrollBtnShow = true;
			m_d->mIsLeftScrollBtnShow = true;
		}
	}
	else {
		m_d->mIsRightScrollBtnShow = false;
		m_d->mIsLeftScrollBtnShow = false;
		m_d->mXBase = 0;
		//

		for (SARibbonCategoryLayoutItem *item : m_d->mItemList)
		{
			if (SARibbonPannel *p = qobject_cast<SARibbonPannel *>(item->widget())) {
				if (p->isExpanding()) {
					++canExpandingCount;
				}
			}
		}
		if (canExpandingCount > 0) {
			expandWidth = (categoryWidth - total) / canExpandingCount;
		}
		else {
			expandWidth = 0;
		}
	}
	total = 0;
	int x = m_d->mXBase;

	for (SARibbonCategoryLayoutItem *item : m_d->mItemList)
	{
		if (item->isEmpty()) {
			if (item->separatorWidget) {
				item->separatorWidget->hide();
			}
			item->mWillSetGeometry = QRect(0, 0, 0, 0);
			item->mWillSetSeparatorGeometry = QRect(0, 0, 0, 0);
			continue;
		}
		SARibbonPannel *p = qobject_cast<SARibbonPannel *>(item->widget());
		if (nullptr == p) {
			qDebug() << "unknow widget in SARibbonCategoryLayout";
			continue;
		}
		QSize pannelSize = p->sizeHint();
		QSize SeparatorSize(0, 0);
		if (item->separatorWidget) {
			SeparatorSize = item->separatorWidget->sizeHint();
		}
		if (p->isExpanding()) {
			pannelSize.setWidth(pannelSize.width() + expandWidth);
		}
		int w = pannelSize.width();
		item->mWillSetGeometry = QRect(x, y, w, height);
		x += w;
		total += w;
		w = SeparatorSize.width();
		item->mWillSetSeparatorGeometry = QRect(x, y, w, height);
		x += w;
		total += w;
	}
	m_d->mTotalWidth = total;
	QWidget *cp = category->parentWidget();
	int parentHeight = (nullptr == cp) ? height : cp->height();
	int parentWidth = (nullptr == cp) ? total : cp->width();
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
	qDebug() << "\r\n mSizeHint:[ " << parentHeight << "," << parentWidth;
	for (int i = 0; i < m_d->mItemList.size(); ++i)
	{
		qDebug() << "\r\n [ " << i << "]"
			<< " geo:" << m_d->mItemList[i]->mWillSetGeometry
			<< " sep geo:" << m_d->mItemList[i]->mWillSetSeparatorGeometry
			;
	}
#endif
	m_d->mSizeHint = QSize(parentWidth, parentHeight);
}


void SARibbonCategoryLayout::doLayout()
{
	if (m_d->mDirty) {
		updateGeometryArr();
	}
	SARibbonCategory *category = qobject_cast<SARibbonCategory *>(parentWidget());

	m_d->mLeftScrollBtn->setGeometry(0, 0, 12, category->height());
	m_d->mRightScrollBtn->setGeometry(category->width() - 12, 0, 12, category->height());
	QList<QWidget *> showWidgets, hideWidgets;

	for (SARibbonCategoryLayoutItem *item : m_d->mItemList)
	{
		if (item->isEmpty()) {
			hideWidgets << item->widget();
			if (item->separatorWidget) {
				hideWidgets << item->separatorWidget;
			}
		}
		else {
			item->widget()->setFixedSize(item->mWillSetGeometry.size());
			item->widget()->move(item->mWillSetGeometry.topLeft());
			//            item->setGeometry(item->mWillSetGeometry);
			showWidgets << item->widget();
			if (item->separatorWidget) {
				item->separatorWidget->setGeometry(item->mWillSetSeparatorGeometry);
				showWidgets << item->separatorWidget;
			}
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
			qDebug() << "SARibbonCategoryLayout::doLayout() =";
			qDebug() << "\r\n     pannel:" << item->widget()->windowTitle()
				<< "\r\n     pannel geo:" << item->mWillSetGeometry
				<< "\r\n     sep geo:" << item->mWillSetSeparatorGeometry
				;
#endif
		}
	}


	m_d->mRightScrollBtn->setVisible(m_d->mIsRightScrollBtnShow);
	m_d->mLeftScrollBtn->setVisible(m_d->mIsLeftScrollBtnShow);
	if (m_d->mIsRightScrollBtnShow) {
		m_d->mRightScrollBtn->raise();
	}
	if (m_d->mIsLeftScrollBtnShow) {
		m_d->mLeftScrollBtn->raise();
	}
	for (QWidget *w : showWidgets)
	{
		w->show();
	}
	for (QWidget *w : hideWidgets)
	{
		w->hide();
	}
}


QList<SARibbonPannel *> SARibbonCategoryLayout::pannels() const
{
	QList<SARibbonPannel *> res;

	for (SARibbonCategoryLayoutItem *item : m_d->mItemList)
	{
		SARibbonPannel *p = qobject_cast<SARibbonPannel *>(item->widget());
		res.append(p);
	}
	return (res);
}


void SARibbonCategoryLayout::onLeftScrollButtonClicked()
{
	SARibbonCategory *category = qobject_cast<SARibbonCategory *>(parentWidget());
	int width = category->width();
	int totalWidth = m_d->mTotalWidth;

	if (totalWidth > width) {
		int tmp = m_d->mXBase + width;
		if (tmp > 0) {
			tmp = 0;
		}
		m_d->mXBase = tmp;
	}
	else {
		m_d->mXBase = 0;
	}
	invalidate();
}


void SARibbonCategoryLayout::onRightScrollButtonClicked()
{
	SARibbonCategory *category = qobject_cast<SARibbonCategory *>(parentWidget());
	int width = category->width();
	int totalWidth = m_d->mTotalWidth;

	if (totalWidth > width) {
		int tmp = m_d->mXBase - width;
		if (tmp < (width - totalWidth)) {
			tmp = width - totalWidth;
		}
		m_d->mXBase = tmp;
	}
	else {
		m_d->mXBase = 0;
	}
	invalidate();
}


void SARibbonCategoryLayout::setGeometry(const QRect& rect)
{
	m_d->mDirty = false;
	updateGeometryArr();
	QLayout::setGeometry(rect);
	doLayout();
}
