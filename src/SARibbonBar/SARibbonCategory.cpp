#include "SARibbonCategory.h"
#include <QList>
#include <QResizeEvent>
#include <QPainter>
#include <QLinearGradient>
#include <QDebug>
#include <QApplication>
#include "SARibbonToolButton.h"
#include <QHBoxLayout>
#include "SARibbonCategoryLayout.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonElementManager.h"

#include <QList>
#include <QMap>

/**
 * @brief The SARibbonCategoryItem class
 */
class SARibbonCategoryItem {
public:
	SARibbonCategoryItem()
		: pannelWidget(nullptr)
		, separatorWidget(nullptr)
	{
	}


	bool isEmpty() const
	{
		if (pannelWidget) {
			return (pannelWidget->isHidden());
		}
		return (true);
	}


	bool isNull() const
	{
		return (pannelWidget == nullptr);
	}


public:
	SARibbonPannel *pannelWidget;
	SARibbonSeparatorWidget *separatorWidget;
	QRect mWillSetGeometry;
	QRect mWillSetSeparatorGeometry;
};

class SARibbonCategoryPrivate
{
public:
	SARibbonCategoryPrivate(SARibbonCategory *parent);
	virtual ~SARibbonCategoryPrivate();

	SARibbonPannel *addPannel(const QString& title);
	SARibbonPannel *insertPannel(const QString& title, int index);
	void addPannel(SARibbonPannel *pannel);
	void insertPannel(int index, SARibbonPannel *pannel);

	bool takePannel(SARibbonPannel *pannel);

	bool removePannel(SARibbonPannel *pannel);
	void setBackgroundBrush(const QBrush& brush);
	SARibbonCategory *ribbonCategory();
	const SARibbonCategory *ribbonCategory() const;
	void setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m);
	SARibbonPannel::PannelLayoutMode ribbonPannelLayoutMode() const;

	QList<SARibbonPannel *> pannelList();

	int totalSizeHintWidth() const;

	QSize categoryContentSize() const;

	void updateItemGeometry();

	void doItemLayout();

	//
	void onLeftScrollButtonClicked();
	void onRightScrollButtonClicked();
	void doWheelEvent(QWheelEvent *event);

public:
	SARibbonCategory *mParent;
	SARibbonBar *mBar;
	SARibbonPannel::PannelLayoutMode mDefaultPannelLayoutMode;
	SARibbonCategoryScrollButton *mLeftScrollBtn;
	SARibbonCategoryScrollButton *mRightScrollBtn;
	QList<SARibbonCategoryItem> mItemList;
	QSize mSizeHint;
	int mTotalWidth;
	int mXBase;
	bool mIsRightScrollBtnShow;
	bool mIsLeftScrollBtnShow;
	QMargins mContentsMargins;
	bool mIsContextCategory;
	bool isCanCustomize;
};

SARibbonCategoryScrollButton::SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget *p)
	: QToolButton(p)
{
	setArrowType(arr);
}


SARibbonCategory::SARibbonCategory(QWidget *p)
	: QWidget(p)
	, m_d(new SARibbonCategoryPrivate(this))
{
	setAutoFillBackground(true);
	setBackgroundBrush(Qt::white);
	m_d->mLeftScrollBtn = new SARibbonCategoryScrollButton(Qt::LeftArrow, this);
	m_d->mRightScrollBtn = new SARibbonCategoryScrollButton(Qt::RightArrow, this);
	m_d->mLeftScrollBtn->setVisible(false);
	m_d->mRightScrollBtn->setVisible(false);
	connect(m_d->mLeftScrollBtn, &QToolButton::clicked, this, &SARibbonCategory::onLeftScrollButtonClicked);
	connect(m_d->mRightScrollBtn, &QToolButton::clicked, this, &SARibbonCategory::onRightScrollButtonClicked);
}


SARibbonCategory::~SARibbonCategory()
{
	delete m_d;
}

QString SARibbonCategory::categoryName() const
{
	return (windowTitle());
}


void SARibbonCategory::setCategoryName(const QString& title)
{
	setWindowTitle(title);
}


void SARibbonCategory::setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m)
{
	m_d->setRibbonPannelLayoutMode(m);
}


SARibbonPannel::PannelLayoutMode SARibbonCategory::ribbonPannelLayoutMode() const
{
	return (m_d->ribbonPannelLayoutMode());
}


SARibbonPannel *SARibbonCategory::addPannel(const QString& title)
{
	return (m_d->addPannel(title));
}


SARibbonPannel *SARibbonCategory::insertPannel(const QString& title, int index)
{
	return (m_d->insertPannel(title, index));
}


SARibbonPannel *SARibbonCategory::pannelByName(const QString& title) const
{
	for (SARibbonCategoryItem& i : m_d->mItemList)
	{
		if (i.pannelWidget) {
			if (i.pannelWidget->windowTitle() == title) {
				return (i.pannelWidget);
			}
		}
	}
	return (nullptr);
}


SARibbonPannel *SARibbonCategory::pannelByObjectName(const QString& objname) const
{
	for (SARibbonCategoryItem& i : m_d->mItemList)
	{
		if (i.pannelWidget) {
			if (i.pannelWidget->objectName() == objname) {
				return (i.pannelWidget);
			}
		}
	}
	return (nullptr);
}


SARibbonPannel *SARibbonCategory::pannelByIndex(int index) const
{
	return (m_d->mItemList.value(index).pannelWidget);
}


int SARibbonCategory::pannelIndex(SARibbonPannel *p) const
{
	int c = pannelCount();

	for (int i = 0; i < c; ++i)
	{
		if (m_d->mItemList[i].pannelWidget == p) {
			return (i);
		}
	}
	return (-1);
}


void SARibbonCategory::movePannel(int from, int to)
{
	if (from == to) {
		return;
	}
	if (to < 0) {
		to = 0;
	}
	if (to >= pannelCount()) {
		to = pannelCount() - 1;
	}
	m_d->mItemList.move(from, to);
	m_d->updateItemGeometry();
}


void SARibbonCategory::addPannel(SARibbonPannel *pannel)
{
	m_d->addPannel(pannel);
}


void SARibbonCategory::addPannel(QWidget *pannel)
{
	SARibbonPannel *p = qobject_cast<SARibbonPannel *>(pannel);

	if (p) {
		addPannel(p);
	}
}


bool SARibbonCategory::takePannel(SARibbonPannel *pannel)
{
	return (m_d->takePannel(pannel));
}


bool SARibbonCategory::removePannel(SARibbonPannel *pannel)
{
	return (m_d->removePannel(pannel));
}


bool SARibbonCategory::removePannel(int index)
{
	SARibbonPannel *p = pannelByIndex(index);

	if (nullptr == p) {
		return (false);
	}
	return (removePannel(p));
}


void SARibbonCategory::setBackgroundBrush(const QBrush& brush)
{
	m_d->setBackgroundBrush(brush);
}


QList<SARibbonPannel *> SARibbonCategory::pannelList() const
{
	return (m_d->pannelList());
}


QSize SARibbonCategory::sizeHint() const
{
	return (m_d->mSizeHint);
}


bool SARibbonCategory::isContextCategory() const
{
	return (m_d->mIsContextCategory);
}


int SARibbonCategory::pannelCount() const
{
	return (m_d->mItemList.size());
}


bool SARibbonCategory::isCanCustomize() const
{
	return (m_d->isCanCustomize);
}


void SARibbonCategory::setCanCustomize(bool b)
{
	m_d->isCanCustomize = b;
}


SARibbonBar *SARibbonCategory::ribbonBar() const
{
	return (m_d->mBar);
}


void SARibbonCategory::onLeftScrollButtonClicked()
{
	m_d->onLeftScrollButtonClicked();
}


void SARibbonCategory::onRightScrollButtonClicked()
{
	m_d->onRightScrollButtonClicked();
}


bool SARibbonCategory::event(QEvent *e)
{
	switch (e->type())
	{
	case QEvent::LayoutRequest:
	{
		m_d->updateItemGeometry();
	}
	break;

	default:
		break;
	}
	return (QWidget::event(e));
}


void SARibbonCategory::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	//    QSize delta = e->size() - e->oldSize();

	//    if (delta.isNull()) {
	//        return;
	//    }
	m_d->updateItemGeometry();
}


//////////////////////////////////////////////////////////////////

SARibbonCategoryPrivate::SARibbonCategoryPrivate(SARibbonCategory *parent)
	: mParent(parent)
	, mBar(nullptr)
	, mDefaultPannelLayoutMode(SARibbonPannel::ThreeRowMode)
	, mLeftScrollBtn(nullptr)
	, mRightScrollBtn(nullptr)
	, mSizeHint(50, 50)
	, mIsRightScrollBtnShow(false)
	, mIsLeftScrollBtnShow(false)
	, mContentsMargins(1, 1, 1, 1)
	, mIsContextCategory(false)
	, isCanCustomize(true)
{
}


SARibbonCategoryPrivate::~SARibbonCategoryPrivate()
{
}


int SARibbonCategoryPrivate::totalSizeHintWidth() const
{
	int total = 0;
	QMargins mag = mContentsMargins;

	if (!mag.isNull()) {
		total += (mag.left() + mag.right());
	}
	for (SARibbonCategoryItem item : mItemList)
	{
		if (item.isEmpty()) {
			continue;
		}
		QSize pannelSize = item.pannelWidget->sizeHint();
		QSize SeparatorSize(0, 0);
		if (item.separatorWidget) {
			SeparatorSize = item.separatorWidget->sizeHint();
		}
		total += pannelSize.width();
		total += SeparatorSize.width();
	}
	return (total);
}


QSize SARibbonCategoryPrivate::categoryContentSize() const
{
	const SARibbonCategory *category = ribbonCategory();
	QSize s = category->size();
	QMargins mag = mContentsMargins;

	if (!mag.isNull()) {
		s.rheight() -= (mag.top() + mag.bottom());
		s.rwidth() -= (mag.right() + mag.left());
	}
	return (s);
}


SARibbonPannel *SARibbonCategoryPrivate::addPannel(const QString& title)
{
	return (insertPannel(title, mItemList.size()));
}


SARibbonPannel *SARibbonCategoryPrivate::insertPannel(const QString& title, int index)
{
	SARibbonPannel *pannel = RibbonSubElementDelegate->createRibbonPannel(ribbonCategory());

	pannel->setWindowTitle(title);
	pannel->setObjectName(title);
	pannel->setPannelLayoutMode(ribbonPannelLayoutMode());
	pannel->installEventFilter(mParent);
	pannel->setVisible(true);
	insertPannel(index, pannel);
	return (pannel);
}


void SARibbonCategoryPrivate::addPannel(SARibbonPannel *pannel)
{
	insertPannel(mItemList.size(), pannel);
}


void SARibbonCategoryPrivate::insertPannel(int index, SARibbonPannel *pannel)
{
	if (nullptr == pannel) {
		return;
	}
	if (pannel->parentWidget() != mParent) {
		pannel->setParent(mParent);
	}
	index = qMax(0, index);
	index = qMin(mItemList.size(), index);
	SARibbonCategoryItem item;

	item.pannelWidget = pannel;
	item.separatorWidget = RibbonSubElementDelegate->createRibbonSeparatorWidget(mParent);
	mItemList.insert(index, item);
	updateItemGeometry();
}


bool SARibbonCategoryPrivate::takePannel(SARibbonPannel *pannel)
{
	SARibbonCategoryItem item;

	for (int i = 0; i < mItemList.size(); ++i)
	{
		if (pannel == mItemList[i].pannelWidget) {
			item = mItemList.takeAt(i);
		}
	}
	if (item.isNull()) {
		return (false);
	}
	if (item.separatorWidget) {
		item.separatorWidget->hide();
		item.separatorWidget->deleteLater();
	}
	return (true);
}


bool SARibbonCategoryPrivate::removePannel(SARibbonPannel *pannel)
{
	if (takePannel(pannel)) {
		pannel->hide();
		pannel->deleteLater();
		return (true);
	}
	return (false);
}


void SARibbonCategoryPrivate::setBackgroundBrush(const QBrush& brush)
{
	QPalette p = ribbonCategory()->palette();

	p.setBrush(QPalette::Window, brush);
	ribbonCategory()->setPalette(p);
}


QList<SARibbonPannel *> SARibbonCategoryPrivate::pannelList()
{
	QList<SARibbonPannel *> res;

	for (const SARibbonCategoryItem& i : mItemList)
	{
		if (!i.isNull()) {
			res.append(i.pannelWidget);
		}
	}
	return (res);
}


bool SARibbonCategory::eventFilter(QObject *watched, QEvent *event)
{
	if (nullptr == watched) {
		return (false);
	}
	SARibbonPannel *pannel = qobject_cast<SARibbonPannel *>(watched);

	if (pannel) {
		switch (event->type())
		{
		case QEvent::HideToParent:
		{
			//layout()->invalidate();
		}
		break;

		case QEvent::ShowToParent:
		{
			//layout()->invalidate();
		}
		break;

		default:
			break;
		}
	}
	return (false);
}


void SARibbonCategory::wheelEvent(QWheelEvent *event)
{
	m_d->doWheelEvent(event);
}


void SARibbonCategory::markIsContextCategory(bool isContextCategory)
{
	m_d->mIsContextCategory = isContextCategory;
}


void SARibbonCategory::setRibbonBar(SARibbonBar *bar)
{
	m_d->mBar = bar;
}


SARibbonCategory *SARibbonCategoryPrivate::ribbonCategory()
{
	return (mParent);
}


const SARibbonCategory *SARibbonCategoryPrivate::ribbonCategory() const
{
	return (mParent);
}


void SARibbonCategoryPrivate::setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m)
{
	if (mDefaultPannelLayoutMode == m) {
		return;
	}
	mDefaultPannelLayoutMode = m;
	QList<SARibbonPannel *> ps = pannelList();

	for (SARibbonPannel *p : ps)
	{
		p->setPannelLayoutMode(m);
	}
	updateItemGeometry();
}


SARibbonPannel::PannelLayoutMode SARibbonCategoryPrivate::ribbonPannelLayoutMode() const
{
	return (mDefaultPannelLayoutMode);
}


void SARibbonCategoryPrivate::updateItemGeometry()
{
	SARibbonCategory *category = ribbonCategory();
	QSize contentSize = categoryContentSize();
	int y = 0;

	if (!mContentsMargins.isNull()) {
		y = mContentsMargins.top();
	}
	int total = totalSizeHintWidth();
	int canExpandingCount = 0;
	int expandWidth = 0;

#ifdef SA_RIBBON_DEBUG_HELP_DRAW
	qDebug() << "\r\n\r\n============================================="
		<< "\r\nSARibbonCategoryPrivate::updateItemGeometry"
		<< "\r\npannel name:" << category->windowTitle()
		<< "\r\n height:" << height
		<< "\r\n first total:" << total
		<< "\r\n y:" << y
		<< "\r\n expandWidth:" << expandWidth
		;
#endif
	if (total <= contentSize.width()) {
		mXBase = 0;
		//
		for (const SARibbonCategoryItem& item : mItemList)
		{
			if (!item.isEmpty()) {
				if (item.pannelWidget->isExpanding()) {
					//pannel
					++canExpandingCount;
				}
			}
		}
		if (canExpandingCount > 0) {
			expandWidth = (contentSize.width() - total) / canExpandingCount;
		}
		else {
			expandWidth = 0;
		}
	}
	total = 0;// total
	int x = mXBase;

	for (SARibbonCategoryItem& item : mItemList)
	{
		if (item.isEmpty()) {
			//hide
			if (item.separatorWidget) {
				//pannel hide
				item.separatorWidget->hide();
			}
			item.mWillSetGeometry = QRect(0, 0, 0, 0);
			item.mWillSetSeparatorGeometry = QRect(0, 0, 0, 0);
			continue;
		}
		SARibbonPannel *p = item.pannelWidget;
		if (nullptr == p) {
			qDebug() << "unknow widget in SARibbonCategoryLayout";
			continue;
		}
		QSize pannelSize = p->sizeHint();
		QSize SeparatorSize(0, 0);
		if (item.separatorWidget) {
			SeparatorSize = item.separatorWidget->sizeHint();
		}
		if (p->isExpanding()) {
			pannelSize.setWidth(pannelSize.width() + expandWidth);
		}
		int w = pannelSize.width();
		item.mWillSetGeometry = QRect(x, y, w, contentSize.height());
		x += w;
		total += w;
		w = SeparatorSize.width();
		item.mWillSetSeparatorGeometry = QRect(x, y, w, contentSize.height());
		x += w;
		total += w;
	}
	mTotalWidth = total;
	if (total > contentSize.width()) {
		if (0 == mXBase) {
			mIsRightScrollBtnShow = true;
			mIsLeftScrollBtnShow = false;
		}
		else if (mXBase <= (contentSize.width() - total)) {
			mIsRightScrollBtnShow = false;
			mIsLeftScrollBtnShow = true;
		}
		else {
			mIsRightScrollBtnShow = true;
			mIsLeftScrollBtnShow = true;
		}
	}
	else {
		mIsRightScrollBtnShow = false;
		mIsLeftScrollBtnShow = false;
	}
	QWidget *cp = category->parentWidget();
	int parentHeight = (nullptr == cp) ? contentSize.height() : cp->height();
	int parentWidth = (nullptr == cp) ? total : cp->width();
	mSizeHint = QSize(parentWidth, parentHeight);
	doItemLayout();
}


void SARibbonCategoryPrivate::doItemLayout()
{
	SARibbonCategory *category = ribbonCategory();

	mLeftScrollBtn->setGeometry(0, 0, 12, category->height());
	mRightScrollBtn->setGeometry(category->width() - 12, 0, 12, category->height());
	QList<QWidget *> showWidgets, hideWidgets;

	for (const SARibbonCategoryItem& item : mItemList)
	{
		if (item.isNull()) {
			continue;
		}
		if (item.isEmpty()) {
			hideWidgets << item.pannelWidget;
			if (item.separatorWidget) {
				hideWidgets << item.separatorWidget;
			}
		}
		else {
			//            item.pannelWidget->setFixedSize(item.mWillSetGeometry.size());
			//            item.pannelWidget->move(item.mWillSetGeometry.topLeft());
			item.pannelWidget->setGeometry(item.mWillSetGeometry);
			showWidgets << item.pannelWidget;
			if (item.separatorWidget) {
				item.separatorWidget->setGeometry(item.mWillSetSeparatorGeometry);
				showWidgets << item.separatorWidget;
			}
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
			qDebug() << "SARibbonCategoryLayout::doLayout() =";
			qDebug() << "\r\n     pannel:" << item.pannelWidget->windowTitle()
				<< "\r\n     pannel geo:" << item.mWillSetGeometry
				<< "\r\n     sep geo:" << item.mWillSetSeparatorGeometry
				;
#endif
		}
	}


	mRightScrollBtn->setVisible(mIsRightScrollBtnShow);
	mLeftScrollBtn->setVisible(mIsLeftScrollBtnShow);
	if (mIsRightScrollBtnShow) {
		mRightScrollBtn->raise();
	}
	if (mIsLeftScrollBtnShow) {
		mLeftScrollBtn->raise();
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


void SARibbonCategoryPrivate::onLeftScrollButtonClicked()
{
	int width = categoryContentSize().width();
	int totalWidth = mTotalWidth;

	if (totalWidth > width) {
		int tmp = mXBase + width;
		if (tmp > 0) {
			tmp = 0;
		}
		mXBase = tmp;
	}
	else {
		mXBase = 0;
	}
	updateItemGeometry();
}


void SARibbonCategoryPrivate::onRightScrollButtonClicked()
{
	int width = categoryContentSize().width();
	int totalWidth = mTotalWidth;

	if (totalWidth > width) {
		int tmp = mXBase - width;
		if (tmp < (width - totalWidth)) {
			tmp = width - totalWidth;
		}
		mXBase = tmp;
	}
	else {
		mXBase = 0;
	}
	updateItemGeometry();
}


void SARibbonCategoryPrivate::doWheelEvent(QWheelEvent *event)
{
	QSize contentSize = categoryContentSize();
	int totalWidth = mTotalWidth;

	if (totalWidth > contentSize.width()) {
		QPoint numPixels = event->pixelDelta();
		QPoint numDegrees = event->angleDelta() / 8;
		int scrollpix = 0;
		if (!numPixels.isNull())
			scrollpix = numPixels.x() / 4;
		else if (!numDegrees.isNull())
			scrollpix = numDegrees.x() / 15;
		else {}
		if (scrollpix > 0) { //SARibbonCategory
			int tmp = mXBase - scrollpix;
			if (tmp < (contentSize.width() - totalWidth)) {
				tmp = contentSize.width() - totalWidth;
			}
			mXBase = tmp;
		}
		else {                                 //SARibbonCategory
			int tmp = mXBase - scrollpix;
			if (tmp > 0) {
				tmp = 0;
			}
			mXBase = tmp;
		}
		updateItemGeometry();
	}
	else {
		event->ignore();
		mXBase = 0;
	}
}
