#include "SARibbonBar.h"
#include "SAWindowButtonGroup.h"
#include <QApplication>
#include <QPainter>
#include "SARibbonApplicationButton.h"
#include "SARibbonTabBar.h"
#include <QSet>
#include <QResizeEvent>
#include <QVariant>
#include <QLinearGradient>
#include <QDebug>
#include <QApplication>
#include <QAction>
#include <QHoverEvent>
#include <QTimer>
#include <QStyleOptionMenuItem>
#include "SARibbonStackedWidget.h"
#include "SARibbonElementManager.h"
#include "SARibbonControlButton.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonQuickAccessBar.h"


#define HELP_DRAW_RECT(p, rect)			    \
    do{					    \
        p.save();			    \
        QPen _pen(Qt::DashDotDotLine);	    \
        _pen.setColor(QColor(219, 26, 59)); \
        p.setPen(_pen);			    \
        p.setBrush(QBrush());		    \
        p.drawRect(rect);		    \
        p.restore();			    \
    }while(0)

class _SAContextCategoryManagerData
{
public:
	SARibbonContextCategory *contextCategory;
	QList<int> tabPageIndex;
	bool operator ==(const SARibbonContextCategory *contextPage)
	{
		return (this->contextCategory == contextPage);
	}
};

class _SARibbonTabData {
public:
	SARibbonCategory *category;
	int index;
	_SARibbonTabData() : category(nullptr)
		, index(-1)
	{
	}
};
Q_DECLARE_METATYPE(_SARibbonTabData)

class SARibbonBarPrivate
{
public:
	SARibbonBar *MainClass;
	QAbstractButton *applicationButton;
	SARibbonTabBar *ribbonTabBar;
	SARibbonStackedWidget *stackedContainerWidget;
	QList<_SAContextCategoryManagerData> currentShowingContextCategory;
	QList<SARibbonContextCategory *> mContextCategoryList;
	QList<_SARibbonTabData> mHidedCategory;
	int iconRightBorderPosition;
	SARibbonControlButton *minimumCategoryButton;
	SARibbonButtonGroupWidget *rightButtonGroup;
	SARibbonQuickAccessBar *quickAccessBar;
	SARibbonBar::RibbonStyle ribbonStyle;
	SARibbonBar::RibbonStyle lastShowStyle;
	SARibbonBar::RibbonState currentRibbonMode;
	QSize windowButtonSize;
	QList<QColor> mContextCategoryColorList;
	int mContextCategoryColorListIndex;
	SARibbonBarPrivate(SARibbonBar *par)
		: applicationButton(nullptr)
		, ribbonTabBar(nullptr)
		, stackedContainerWidget(nullptr)
		, iconRightBorderPosition(1)
		, minimumCategoryButton(nullptr)
		, rightButtonGroup(nullptr)
		, ribbonStyle(SARibbonBar::OfficeStyle)
		, lastShowStyle(SARibbonBar::OfficeStyle)
		, currentRibbonMode(SARibbonBar::NormalRibbonMode)
		, windowButtonSize(100, RibbonSubElementStyleOpt.titleBarHight)
		, mContextCategoryColorListIndex(-1)
	{
		MainClass = par;
		mContextCategoryColorList << QColor(201, 89, 156)
			<< QColor(242, 203, 29)
			<< QColor(255, 157, 0)
			<< QColor(14, 81, 167)
			<< QColor(228, 0, 69)
			<< QColor(67, 148, 0)
			;
	}


	void init()
	{
		applicationButton = RibbonSubElementDelegate->createRibbonApplicationButton(MainClass);
		MainClass->connect(applicationButton, &QAbstractButton::clicked
			, MainClass, &SARibbonBar::applicationButtonClicked);
		ribbonTabBar = RibbonSubElementDelegate->createRibbonTabBar(MainClass);
		ribbonTabBar->setObjectName(QStringLiteral("objSARibbonTabBar"));
		ribbonTabBar->setDrawBase(false);
		MainClass->connect(ribbonTabBar, &QTabBar::currentChanged
			, MainClass, &SARibbonBar::onCurrentRibbonTabChanged);
		MainClass->connect(ribbonTabBar, &QTabBar::tabBarClicked
			, MainClass, &SARibbonBar::onCurrentRibbonTabClicked);
		MainClass->connect(ribbonTabBar, &QTabBar::tabBarDoubleClicked
			, MainClass, &SARibbonBar::onCurrentRibbonTabDoubleClicked);
		MainClass->connect(ribbonTabBar, &QTabBar::tabMoved
			, MainClass, &SARibbonBar::onTabMoved);
		//
		stackedContainerWidget = RibbonSubElementDelegate->createRibbonStackedWidget(MainClass);
		ribbonTabBar->setObjectName(QStringLiteral("objSAStackedContainerWidget"));
		stackedContainerWidget->connect(stackedContainerWidget, &SARibbonStackedWidget::hidWindow
			, MainClass, &SARibbonBar::onStackWidgetHided);
		stackedContainerWidget->installEventFilter(MainClass);
		setNormalMode();
		//
		quickAccessBar = RibbonSubElementDelegate->createQuickAccessBar(MainClass);
		quickAccessBar->setObjectName(QStringLiteral("objSARibbonQuickAccessBar"));
	}


	void setApplicationButton(QAbstractButton *btn)
	{
		if (applicationButton) {
			delete applicationButton;
		}
		if (btn) {
			if (btn->parent() != MainClass) {
				btn->setParent(MainClass);
			}
			btn->move(0, RibbonSubElementStyleOpt.titleBarHight);
			MainClass->connect(applicationButton, &QAbstractButton::clicked
				, MainClass, &SARibbonBar::applicationButtonClicked);
		}
		applicationButton = btn;
	}


	bool isContainContextCategoryInList(SARibbonContextCategory *contextCategory)
	{
		for (int i = 0; i < currentShowingContextCategory.size(); ++i)
		{
			if (currentShowingContextCategory[i] == contextCategory) {
				return (true);
			}
		}
		return (false);
	}


	void setHideMode()
	{
		currentRibbonMode = SARibbonBar::MinimumRibbonMode;
		this->stackedContainerWidget->setPopupMode();
		this->stackedContainerWidget->setFocusPolicy(Qt::NoFocus);
		this->stackedContainerWidget->clearFocus();
		this->ribbonTabBar->setFocus();
		this->stackedContainerWidget->hide();
		MainClass->setFixedHeight(ribbonTabBar->geometry().bottom());
	}


	void setNormalMode()
	{
		currentRibbonMode = SARibbonBar::NormalRibbonMode;
		this->stackedContainerWidget->setNormalMode();
		this->stackedContainerWidget->setFocus();
		this->stackedContainerWidget->show();
	}


	QColor getContextCategoryColor()
	{
		if (mContextCategoryColorList.isEmpty()) {
			mContextCategoryColorListIndex = -1;
			return (QColor());
		}
		++mContextCategoryColorListIndex;
		if ((mContextCategoryColorListIndex >= mContextCategoryColorList.size()) ||
			(mContextCategoryColorListIndex < 0)) {
			mContextCategoryColorListIndex = 0;
		}
		return (mContextCategoryColorList.at(mContextCategoryColorListIndex));
	}
};


bool SARibbonBar::isTwoRowStyle(SARibbonBar::RibbonStyle s)
{
	return ((s & 0xFF00) > 0);
}


bool SARibbonBar::isOfficeStyle(SARibbonBar::RibbonStyle s)
{
	return ((s & 0xFF) == 0);
}


SARibbonBar::SARibbonBar(QWidget *parent) : QMenuBar(parent)
, m_d(new SARibbonBarPrivate(this))
{
	m_d->init();
	if (parent) {
		connect(parent, &QWidget::windowTitleChanged, this, &SARibbonBar::onWindowTitleChanged);
	}
	setRibbonStyle(OfficeStyle);
}


SARibbonBar::~SARibbonBar()
{
	delete m_d;
}


QAbstractButton *SARibbonBar::applicationButton()
{
	return (m_d->applicationButton);
}


void SARibbonBar::setApplicationButton(QAbstractButton *btn)
{
	m_d->setApplicationButton(btn);
	if (btn) {
		if (btn->objectName().isEmpty()) {
			btn->setObjectName(QStringLiteral("SARibbonApplicationButton"));
		}
		btn->setVisible(true);
		//btn->setGeometry(applicationButtonGeometry());
	}
	//resize
	QApplication::postEvent(this, new QResizeEvent(size(), size()));
}


SARibbonTabBar *SARibbonBar::ribbonTabBar()
{
	return (m_d->ribbonTabBar);
}


SARibbonCategory *SARibbonBar::addCategoryPage(const QString& title)
{
	SARibbonCategory *category = RibbonSubElementDelegate->createRibbonCategory(this);

	//catagory->setFixedHeight(categoryHeight());
	category->setObjectName(title);
	category->setWindowTitle(title);
	addCategoryPage(category);
	return (category);
}

void SARibbonBar::addCategoryPage(SARibbonCategory *category)
{
	if (nullptr == category) {
		return;
	}
	int index = m_d->ribbonTabBar->addTab(category->windowTitle());

	category->setRibbonPannelLayoutMode(isTwoRowStyle() ? SARibbonPannel::TwoRowMode : SARibbonPannel::ThreeRowMode);

	_SARibbonTabData tabdata;

	tabdata.category = category;
	tabdata.index = index;
	category->setRibbonBar(this);
	m_d->ribbonTabBar->setTabData(index, QVariant::fromValue(tabdata));

	m_d->stackedContainerWidget->insertWidget(index, category);
	connect(category, &QWidget::windowTitleChanged, this, &SARibbonBar::onCategoryWindowTitleChanged);
	QApplication::postEvent(this, new QResizeEvent(size(), size()));
}


void SARibbonBar::addCategoryPage(QWidget *category)
{
	SARibbonCategory *c = qobject_cast<SARibbonCategory *>(category);

	if (c) {
		addCategoryPage(c);
	}
}


SARibbonCategory *SARibbonBar::insertCategoryPage(const QString& title, int index)
{
	SARibbonCategory *category = RibbonSubElementDelegate->createRibbonCategory(this);

	category->setObjectName(title);
	category->setWindowTitle(title);
	insertCategoryPage(category, index);
	return (category);
}


void SARibbonBar::insertCategoryPage(SARibbonCategory *category, int index)
{
	if (nullptr == category) {
		return;
	}
	category->setRibbonPannelLayoutMode(isTwoRowStyle() ? SARibbonPannel::TwoRowMode : SARibbonPannel::ThreeRowMode);
	int i = m_d->ribbonTabBar->insertTab(index, category->windowTitle());

	_SARibbonTabData tabdata;

	tabdata.category = category;
	tabdata.index = i;
	m_d->ribbonTabBar->setTabData(i, QVariant::fromValue(tabdata));
	m_d->stackedContainerWidget->insertWidget(index, category);

	connect(category, &QWidget::windowTitleChanged, this, &SARibbonBar::onCategoryWindowTitleChanged);
	QApplication::postEvent(this, new QResizeEvent(size(), size()));
}


SARibbonCategory *SARibbonBar::categoryByName(const QString& title) const
{
	int c = m_d->stackedContainerWidget->count();

	for (int i = 0; i < c; ++i)
	{
		SARibbonCategory *w = qobject_cast<SARibbonCategory *>(m_d->stackedContainerWidget->widget(i));
		if (w) {
			if (w->windowTitle() == title) {
				return (w);
			}
		}
	}
	return (nullptr);
}


SARibbonCategory *SARibbonBar::categoryByObjectName(const QString& objname) const
{
	int c = m_d->stackedContainerWidget->count();

	for (int i = 0; i < c; ++i)
	{
		SARibbonCategory *w = qobject_cast<SARibbonCategory *>(m_d->stackedContainerWidget->widget(i));
		if (w) {
			if (w->objectName() == objname) {
				return (w);
			}
		}
	}
	return (nullptr);
}


SARibbonCategory *SARibbonBar::categoryByIndex(int index) const
{
	QVariant var = m_d->ribbonTabBar->tabData(index);

	if (var.isValid()) {
		_SARibbonTabData p = var.value<_SARibbonTabData>();
		return (p.category);
	}
	return (nullptr);
}


void SARibbonBar::hideCategory(SARibbonCategory *category)
{
	int tabcount = m_d->ribbonTabBar->count();

	for (int i = 0; i < tabcount; ++i)
	{
		QVariant var = m_d->ribbonTabBar->tabData(i);
		if (var.isValid()) {
			_SARibbonTabData p = var.value<_SARibbonTabData>();
			if (p.category == category) {
				m_d->mHidedCategory.append(p);
				m_d->ribbonTabBar->removeTab(i);//tab
			}
		}
	}
}


void SARibbonBar::showCategory(SARibbonCategory *category)
{
	for (auto i = m_d->mHidedCategory.begin(); i != m_d->mHidedCategory.end(); ++i)
	{
		if (i->category == category) {
			int index = m_d->ribbonTabBar->insertTab(i->index, i->category->windowTitle());
			i->index = index;
			m_d->ribbonTabBar->setTabData(index, QVariant::fromValue(*i));
			m_d->mHidedCategory.erase(i);
			return;
		}
	}
	raiseCategory(category);
}


bool SARibbonBar::isCategoryVisible(SARibbonCategory *category) const
{
	int tabindex = categoryIndex(category);

	return (tabindex >= 0);
}


int SARibbonBar::categoryIndex(SARibbonCategory *c) const
{
	int tabcount = m_d->ribbonTabBar->count();

	for (int i = 0; i < tabcount; ++i)
	{
		QVariant var = m_d->ribbonTabBar->tabData(i);
		if (var.isValid()) {
			_SARibbonTabData p = var.value<_SARibbonTabData>();
			if (p.category == c) {
				return (i);
			}
		}
	}
	return (-1);
}


void SARibbonBar::moveCategory(int from, int to)
{
	m_d->ribbonTabBar->moveTab(from, to);
	int tabcount = m_d->ribbonTabBar->count();

	for (int i = 0; i < tabcount; ++i)
	{
		QVariant var = m_d->ribbonTabBar->tabData(i);
		if (var.isValid()) {
			_SARibbonTabData p = var.value<_SARibbonTabData>();
			p.index = i;
			m_d->ribbonTabBar->setTabData(i, QVariant::fromValue(p));
		}
	}
	//tabMoved,stacked
}

QList<SARibbonCategory *> SARibbonBar::categoryPages(bool getAll) const
{
	int c = m_d->stackedContainerWidget->count();
	QList<SARibbonCategory *> res;

	for (int i = 0; i < c; ++i)
	{
		SARibbonCategory *w = qobject_cast<SARibbonCategory *>(m_d->stackedContainerWidget->widget(i));
		if (w) {
			if (!getAll && w->isContextCategory()) {
				//getall
				continue;
			}
			res.append(w);
		}
	}
	return (res);
}


void SARibbonBar::removeCategory(SARibbonCategory *category)
{
	int index = tabIndex(category);

	if (index >= 0) {
		m_d->ribbonTabBar->removeTab(index);
	}
	m_d->stackedContainerWidget->removeWidget(category);
	//category,contexcategory
	for (SARibbonContextCategory *c : m_d->mContextCategoryList)
	{
		c->takeCategory(category);
		updateContextCategoryManagerData();
	}
	repaint();
	QApplication::postEvent(this, new QResizeEvent(size(), size()));
}


SARibbonContextCategory *SARibbonBar::addContextCategory(const QString& title, const QColor& color, const QVariant& id)
{
	SARibbonContextCategory *context = RibbonSubElementDelegate->createRibbonContextCategory(this);

	context->setObjectName(title);
	context->setContextTitle(title);
	context->setId(id);
	context->setContextColor(color.isValid() ? color : m_d->getContextCategoryColor());
	addContextCategory(context);
	return (context);
}


void SARibbonBar::addContextCategory(SARibbonContextCategory *context)
{
	if (nullptr == context) {
		return;
	}
	connect(context, &SARibbonContextCategory::categoryPageAdded
		, this, &SARibbonBar::onContextsCategoryPageAdded);
	if (currentRibbonStyle() == WpsLiteStyle) {
		resizeInWpsLiteStyle();
	}
	m_d->mContextCategoryList.append(context);
}


void SARibbonBar::showContextCategory(SARibbonContextCategory *context)
{
	if (isContextCategoryVisible(context)) {
		return;
	}
	_SAContextCategoryManagerData contextCategoryData;

	contextCategoryData.contextCategory = context;
	for (int i = 0; i < context->categoryCount(); ++i)
	{
		SARibbonCategory *category = context->categoryPage(i);
		category->setRibbonPannelLayoutMode(isTwoRowStyle() ? SARibbonPannel::TwoRowMode : SARibbonPannel::ThreeRowMode);
		//category->setFixedHeight(categoryHeight());
		int index = m_d->ribbonTabBar->addTab(category->windowTitle());
		contextCategoryData.tabPageIndex.append(index);

		_SARibbonTabData tabdata;
		tabdata.category = category;
		tabdata.index = index;
		m_d->ribbonTabBar->setTabData(index, QVariant::fromValue(tabdata));
	}
	m_d->currentShowingContextCategory.append(contextCategoryData);
	QApplication::postEvent(this, new QResizeEvent(size(), size()));
}


void SARibbonBar::hideContextCategory(SARibbonContextCategory *context)
{
	bool ishide = false;
	int indexOffset = 0;

	for (int i = 0; i < m_d->currentShowingContextCategory.size(); ++i)
	{
		if (m_d->currentShowingContextCategory[i].contextCategory == context) {
			const QList<int>& indexs = m_d->currentShowingContextCategory[i].tabPageIndex;
			for (int j = indexs.size() - 1; j >= 0; --j)
			{
				m_d->ribbonTabBar->removeTab(indexs[j]);
				++indexOffset;
			}
			for (int j = i + 1; j < m_d->currentShowingContextCategory.size(); ++j)
			{
				for (int& oldindex : m_d->currentShowingContextCategory[j].tabPageIndex)
				{
					oldindex -= indexOffset;
				}
			}
			ishide = true;
			m_d->currentShowingContextCategory.removeAt(i);
			break;
		}
	}
	if (ishide) {
		QApplication::postEvent(this, new QResizeEvent(size(), size()));
	}
}


bool SARibbonBar::isContextCategoryVisible(SARibbonContextCategory *context)
{
	return (m_d->isContainContextCategoryInList(context));
}


void SARibbonBar::setContextCategoryVisible(SARibbonContextCategory *context, bool visible)
{
	if (nullptr == context) {
		return;
	}
	if (visible) {
		showContextCategory(context);
	}
	else {
		hideContextCategory(context);
	}
}


QList<SARibbonContextCategory *> SARibbonBar::contextCategoryList() const
{
	return (m_d->mContextCategoryList);
}


void SARibbonBar::destroyContextCategory(SARibbonContextCategory *context)
{
	if (nullptr == context) {
		return;
	}
	if (isContextCategoryVisible(context)) {
		hideContextCategory(context);
	}
	m_d->mContextCategoryList.removeAll(context);
	QList<SARibbonCategory *> res = context->categoryList();

	for (SARibbonCategory *c : res)
	{
		c->hide();
		c->deleteLater();
	}
	context->deleteLater();
	QApplication::postEvent(this, new QResizeEvent(size(), size()));
}


void SARibbonBar::setMinimumMode(bool isMinimum)
{
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
	qDebug() << "SARibbonBar::setHideMode " << isMinimum;
#endif
	if (isMinimum) {
		m_d->setHideMode();
	}
	else {
		m_d->setNormalMode();
	}
	QResizeEvent resizeEvent(size(), size());
	QApplication::sendEvent(this, &resizeEvent);
}


bool SARibbonBar::isMinimumMode() const
{
	return (m_d->stackedContainerWidget->isPopupMode());
}

void SARibbonBar::showMinimumModeButton(bool isShow)
{
	if (isShow) {
		activeRightButtonGroup();
		if (nullptr == m_d->minimumCategoryButton) {
			m_d->minimumCategoryButton = RibbonSubElementDelegate->createHidePannelButton(this);
			m_d->minimumCategoryButton->ensurePolished();
			QAction *action = new QAction(m_d->minimumCategoryButton);
			action->setIcon(style()->standardIcon(isMinimumMode() ?
				QStyle::SP_TitleBarUnshadeButton : QStyle::SP_TitleBarShadeButton, 0, m_d->minimumCategoryButton));
			connect(action, &QAction::triggered, this, [=]() {
				this->setMinimumMode(!isMinimumMode());
				action->setIcon(style()->standardIcon(isMinimumMode() ?
					QStyle::SP_TitleBarUnshadeButton : QStyle::SP_TitleBarShadeButton, 0, m_d->minimumCategoryButton));
			});
			m_d->minimumCategoryButton->setDefaultAction(action);
			m_d->rightButtonGroup->addWidget(m_d->minimumCategoryButton);
			update();
		}
	}
	else {
		if (nullptr != m_d->minimumCategoryButton) {
			m_d->minimumCategoryButton->hide();
			m_d->minimumCategoryButton->deleteLater();
			m_d->minimumCategoryButton = nullptr;
		}
	}
	QResizeEvent resizeEvent(size(), size());

	QApplication::sendEvent(this, &resizeEvent);
}

bool SARibbonBar::haveShowMinimumModeButton() const
{
	return (nullptr != m_d->minimumCategoryButton);
}


int SARibbonBar::tabBarHeight() const
{
	return (RibbonSubElementStyleOpt.tabBarHight);
}


int SARibbonBar::titleBarHeight() const
{
	return (RibbonSubElementStyleOpt.titleBarHight);
}


void SARibbonBar::onWindowTitleChanged(const QString& title)
{
	Q_UNUSED(title);
	update();
}


void SARibbonBar::onCategoryWindowTitleChanged(const QString& title)
{
	SARibbonCategory *w = qobject_cast<SARibbonCategory *>(sender());

	if (nullptr == w) {
		return;
	}
	for (int i = 0; i < m_d->ribbonTabBar->count(); ++i)
	{
		QVariant var = m_d->ribbonTabBar->tabData(i);
		if (var.isValid()) {
			_SARibbonTabData p = var.value<_SARibbonTabData>();
			if (w == p.category) {
				m_d->ribbonTabBar->setTabText(i, title);
			}
		}
	}
}


void SARibbonBar::onStackWidgetHided()
{
	//m_d->ribbonTabBar->repaint();
}

void SARibbonBar::onCurrentRibbonTabChanged(int index)
{
	QVariant var = m_d->ribbonTabBar->tabData(index);
	SARibbonCategory *category = nullptr;

	if (var.isValid()) {
		_SARibbonTabData p = var.value<_SARibbonTabData>();
		category = p.category;
	}
	if (category) {
		if (m_d->stackedContainerWidget->currentWidget() != category) {
			m_d->stackedContainerWidget->setCurrentWidget(category);
		}
		if (isMinimumMode()) {
			m_d->ribbonTabBar->clearFocus();
			if (!m_d->stackedContainerWidget->isVisible()) {
				if (m_d->stackedContainerWidget->isPopupMode()) {
					QHoverEvent ehl(QEvent::HoverLeave
						, m_d->ribbonTabBar->mapToGlobal(QCursor::pos())
						, m_d->ribbonTabBar->mapToGlobal(QCursor::pos())
					);
					QApplication::sendEvent(m_d->ribbonTabBar, &ehl);
					resizeStackedContainerWidget();
					m_d->stackedContainerWidget->setFocus();
					m_d->stackedContainerWidget->exec();
				}
			}
			else {
			}
		}
	}
	emit currentRibbonTabChanged(index);
}


void SARibbonBar::onCurrentRibbonTabClicked(int index)
{
	if (index != m_d->ribbonTabBar->currentIndex()) {
		return;
	}
	if (this->isMinimumMode()) {
		if (!this->m_d->stackedContainerWidget->isVisible()) {
			if (this->m_d->stackedContainerWidget->isPopupMode()) {
				QHoverEvent ehl(QEvent::HoverLeave
					, m_d->ribbonTabBar->mapToGlobal(QCursor::pos())
					, m_d->ribbonTabBar->mapToGlobal(QCursor::pos())
				);
				QApplication::sendEvent(m_d->ribbonTabBar, &ehl);
				resizeStackedContainerWidget();
				this->m_d->stackedContainerWidget->setFocus();
				this->m_d->stackedContainerWidget->exec();
			}
		}
	}
}


void SARibbonBar::onCurrentRibbonTabDoubleClicked(int index)
{
	Q_UNUSED(index);
	setMinimumMode(!isMinimumMode());
}


void SARibbonBar::onContextsCategoryPageAdded(SARibbonCategory *category)
{
	Q_ASSERT_X(category != nullptr, "onContextsCategoryPageAdded", "add nullptr page");
	m_d->stackedContainerWidget->addWidget(category);
}

void SARibbonBar::onTabMoved(int from, int to)
{
	m_d->stackedContainerWidget->moveWidget(from, to);
}


int SARibbonBar::tabIndex(SARibbonCategory *obj)
{
	const int size = m_d->ribbonTabBar->count();

	for (int i = 0; i < size; ++i)
	{
		QVariant v = m_d->ribbonTabBar->tabData(i);
		if (v.isValid()) {
			_SARibbonTabData innervalue = v.value<_SARibbonTabData>();
			if (innervalue.category == obj) {
				return (i);
			}
		}
	}

	return (-1);
}


void SARibbonBar::updateRibbonElementGeometry()
{
	QList<SARibbonCategory *> categorys = categoryPages();

	for (SARibbonCategory *c : categorys)
	{
		c->setRibbonPannelLayoutMode(SARibbonBar::isTwoRowStyle(currentRibbonStyle()) ? SARibbonPannel::TwoRowMode : SARibbonPannel::ThreeRowMode);
	}

	if (NormalRibbonMode == currentRibbonState()) {
		setFixedHeight(mainBarHeight());
	}
	//    else if(MinimumRibbonMode == currentRibbonState()){
	//        setFixedHeight(m_d->ribbonTabBar->geometry().bottom());
	//    }
}


void SARibbonBar::activeRightButtonGroup()
{
	if (nullptr == m_d->rightButtonGroup) {
		m_d->rightButtonGroup = RibbonSubElementDelegate->craeteButtonGroupWidget(this);
		m_d->rightButtonGroup->setFrameShape(QFrame::NoFrame);
	}
	m_d->rightButtonGroup->show();
}


SARibbonButtonGroupWidget *SARibbonBar::rightButtonGroup()
{
	activeRightButtonGroup();
	return (m_d->rightButtonGroup);
}


SARibbonQuickAccessBar *SARibbonBar::quickAccessBar()
{
	return (m_d->quickAccessBar);
}


void SARibbonBar::setRibbonStyle(SARibbonBar::RibbonStyle v)
{
	m_d->ribbonStyle = v;
	m_d->lastShowStyle = v;
	m_d->quickAccessBar->setEnableShowIcon(isOfficeStyle(v));
	updateRibbonElementGeometry();
	QSize oldSize = size();
	QSize newSize(oldSize.width(), mainBarHeight());
	QResizeEvent es(newSize, oldSize);

	QApplication::sendEvent(this, &es);
	if (MinimumRibbonMode == currentRibbonState()) {
		setFixedHeight(m_d->ribbonTabBar->geometry().bottom());
	}
}


SARibbonBar::RibbonStyle SARibbonBar::currentRibbonStyle() const
{
	return (m_d->ribbonStyle);
}


SARibbonBar::RibbonState SARibbonBar::currentRibbonState() const
{
	return (m_d->currentRibbonMode);
}


void SARibbonBar::setCurrentIndex(int index)
{
	m_d->ribbonTabBar->setCurrentIndex(index);
	//onCurrentRibbonTabChanged(index);
}


int SARibbonBar::currentIndex()
{
	return (m_d->ribbonTabBar->currentIndex());
}


void SARibbonBar::raiseCategory(SARibbonCategory *category)
{
	int index = m_d->stackedContainerWidget->indexOf(category);

	if (index >= 0) {
		setCurrentIndex(index);
	}
}


bool SARibbonBar::isTwoRowStyle() const
{
	return (SARibbonBar::isTwoRowStyle(m_d->ribbonStyle));
}


bool SARibbonBar::isOfficeStyle() const
{
	return (SARibbonBar::isOfficeStyle(currentRibbonStyle()));
}


void SARibbonBar::setWindowButtonSize(const QSize& size)
{
	m_d->windowButtonSize = size;
}


bool SARibbonBar::eventFilter(QObject *obj, QEvent *e)
{
	if (obj) {
		if ((obj == cornerWidget(Qt::TopLeftCorner)) || (obj == cornerWidget(Qt::TopRightCorner))) {
			if ((QEvent::UpdateLater == e->type()) ||
				(QEvent::MouseButtonRelease == e->type()) ||
				(QEvent::WindowActivate == e->type())) {
				QApplication::postEvent(this, new QResizeEvent(size(), size()));
			}
		}
		else if (obj == m_d->stackedContainerWidget) {
			if ((QEvent::MouseButtonPress == e->type()) || (QEvent::MouseButtonDblClick == e->type())) {
				if (m_d->stackedContainerWidget->isPopupMode()) {
					QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
					if (!m_d->stackedContainerWidget->rect().contains(mouseEvent->pos())) {
						QWidget *clickedWidget = QApplication::widgetAt(mouseEvent->globalPos());
						if (clickedWidget == m_d->ribbonTabBar) {
							const QPoint targetPoint = clickedWidget->mapFromGlobal(mouseEvent->globalPos());
							QMouseEvent *evPress = new QMouseEvent(mouseEvent->type(), targetPoint, mouseEvent->globalPos(), mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
							QApplication::postEvent(clickedWidget, evPress);
							return (true);
						}
					}
				}
			}
		}
		//        if (SAWindowButtonGroup *g = qobject_cast<SAWindowButtonGroup *>(obj)) {
		//            if (e->type() == QEvent::Resize) {
		//                m_d->windowButtonSize = g->size();
		//            }
		//        }
	}
	return (QMenuBar::eventFilter(obj, e));
}


int SARibbonBar::calcMinTabBarWidth() const
{
	//20200831
	const QMargins& mg = m_d->ribbonTabBar->tabMargin();
	int mintabBarWidth = 0;

	for (int i = 0; i < m_d->ribbonTabBar->count(); ++i)
	{
		mintabBarWidth += m_d->ribbonTabBar->tabRect(i).width();
	}
	mintabBarWidth += (mg.left() + mg.right());
	return (mintabBarWidth);
}


int SARibbonBar::mainBarHeight() const
{
	switch (currentRibbonStyle())
	{
	case OfficeStyle:
		return (RibbonSubElementStyleOpt.mainbarHeightOfficeStyleThreeRow);

	case WpsLiteStyle:
		return (RibbonSubElementStyleOpt.mainbarHeightWPSStyleThreeRow);

	case OfficeStyleTwoRow:
		return (RibbonSubElementStyleOpt.mainbarHeightOfficeStyleTwoRow);

	case WpsLiteStyleTwoRow:
		return (RibbonSubElementStyleOpt.mainbarHeightWPSStyleTwoRow);

	default:
		break;
	}
	return (RibbonSubElementStyleOpt.mainbarHeightOfficeStyleThreeRow);
}


void SARibbonBar::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);
	if (isOfficeStyle()) {
		paintInNormalStyle();
	}
	else {
		paintInWpsLiteStyle();
	}
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
	QPainter p(this);
	HELP_DRAW_RECT(p, m_d->quickAccessBar->geometry());
	HELP_DRAW_RECT(p, m_d->ribbonTabBar->geometry());
	HELP_DRAW_RECT(p, m_d->stackedContainerWidget->geometry());
#endif
}


void SARibbonBar::paintInNormalStyle()
{
	QPainter p(this);

	paintBackground(p);
	p.save();
	QList<_SAContextCategoryManagerData> contextCategoryDataList = m_d->currentShowingContextCategory;
	bool isCurrentSelectContextCategoryPage = false;

	QPoint contextCategoryRegion(width(), -1);

	for (int i = 0; i < contextCategoryDataList.size(); ++i)
	{
		QRect contextTitleRect;
		QList<int> indexs = contextCategoryDataList[i].tabPageIndex;
		QColor clr = contextCategoryDataList[i].contextCategory->contextColor();
		if (!indexs.isEmpty()) {
			contextTitleRect = m_d->ribbonTabBar->tabRect(indexs.first());
			QRect endRect = m_d->ribbonTabBar->tabRect(indexs.last());
			contextTitleRect.setRight(endRect.right());
			contextTitleRect.translate(m_d->ribbonTabBar->x(), m_d->ribbonTabBar->y());
			contextTitleRect.setHeight(m_d->ribbonTabBar->height() - 1);
			contextTitleRect -= m_d->ribbonTabBar->tabMargin();
			contextTitleRect.setTop(RibbonSubElementStyleOpt.widgetBord.top());
			paintContextCategoryTab(p
				, contextCategoryDataList[i].contextCategory->contextTitle()
				, contextTitleRect
				, clr);
			if (contextTitleRect.left() < contextCategoryRegion.x()) {
				contextCategoryRegion.setX(contextTitleRect.left());
			}
			if (contextTitleRect.right() > contextCategoryRegion.y()) {
				contextCategoryRegion.setY(contextTitleRect.right());
			}
		}
		isCurrentSelectContextCategoryPage = indexs.contains(m_d->ribbonTabBar->currentIndex());
		if (isCurrentSelectContextCategoryPage) {
			QPen pen;
			pen.setColor(clr);
			pen.setWidth(1);
			p.setPen(pen);
			p.setBrush(Qt::NoBrush);
			p.drawRect(m_d->stackedContainerWidget->geometry());
			isCurrentSelectContextCategoryPage = false;
		}
	}
	p.restore();
	QWidget *parWindow = parentWidget();

	if (parWindow) {
		QRect titleRegion;
		if (contextCategoryRegion.y() < 0) {
			titleRegion.setRect(m_d->quickAccessBar->geometry().right() + 1
				, RibbonSubElementStyleOpt.widgetBord.top()
				, width() - m_d->iconRightBorderPosition - RibbonSubElementStyleOpt.widgetBord.right() - m_d->windowButtonSize.width() - m_d->quickAccessBar->geometry().right() - 1
				, RibbonSubElementStyleOpt.titleBarHight);
		}
		else {
			int leftwidth = contextCategoryRegion.x() - m_d->quickAccessBar->geometry().right() - m_d->iconRightBorderPosition;
			int rightwidth = width() - contextCategoryRegion.y() - m_d->windowButtonSize.width();
			//            if (width() - contextCategoryRegion.y() > contextCategoryRegion.x()-x) {
			if (rightwidth > leftwidth) {
				titleRegion.setRect(contextCategoryRegion.y()
					, RibbonSubElementStyleOpt.widgetBord.top()
					, rightwidth
					, RibbonSubElementStyleOpt.titleBarHight);
			}
			else {
				titleRegion.setRect(m_d->iconRightBorderPosition + m_d->quickAccessBar->geometry().right()
					, RibbonSubElementStyleOpt.widgetBord.top()
					, leftwidth
					, RibbonSubElementStyleOpt.titleBarHight);
			}
		}
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
		p.save();
		p.setBrush(QColor(255, 0, 0, 120));
		p.drawRect(titleRegion);
		p.restore();
#endif
		paintWindowTitle(p, parWindow->windowTitle(), titleRegion);
	}
}


void SARibbonBar::paintInWpsLiteStyle()
{
	QPainter p(this);

	paintBackground(p);
	p.save();
	QList<_SAContextCategoryManagerData> contextCategoryDataList = m_d->currentShowingContextCategory;
	bool isCurrentSelectContextCategoryPage = false;

	for (int i = 0; i < contextCategoryDataList.size(); ++i)
	{
		QRect contextTitleRect;
		QList<int> indexs = contextCategoryDataList[i].tabPageIndex;
		QColor clr = contextCategoryDataList[i].contextCategory->contextColor();
		if (!indexs.isEmpty()) {
			contextTitleRect = m_d->ribbonTabBar->tabRect(indexs.first());
			QRect endRect = m_d->ribbonTabBar->tabRect(indexs.last());
			contextTitleRect.setRight(endRect.right());
			contextTitleRect.translate(m_d->ribbonTabBar->x(), m_d->ribbonTabBar->y());
			contextTitleRect.setHeight(m_d->ribbonTabBar->height() - 1);
			contextTitleRect -= m_d->ribbonTabBar->tabMargin();
			contextTitleRect.setTop(RibbonSubElementStyleOpt.widgetBord.top());
			paintContextCategoryTab(p
				, QString()
				, contextTitleRect
				, clr);
		}
		isCurrentSelectContextCategoryPage = indexs.contains(m_d->ribbonTabBar->currentIndex());
		if (isCurrentSelectContextCategoryPage) {
			QPen pen;
			pen.setColor(clr);
			pen.setWidth(1);
			p.setPen(pen);
			p.setBrush(Qt::NoBrush);
			p.drawRect(m_d->stackedContainerWidget->geometry());
			isCurrentSelectContextCategoryPage = false;
		}
	}
	p.restore();


	QWidget *parWindow = parentWidget();

	if (parWindow) {
		int start = m_d->ribbonTabBar->x() + m_d->ribbonTabBar->width();
		int width = m_d->quickAccessBar->x() - start;
		if (width > 20) {
			QRect titleRegion(start
				, RibbonSubElementStyleOpt.widgetBord.top()
				, width
				, RibbonSubElementStyleOpt.titleBarHight);
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
			p.save();
			p.setBrush(QColor(255, 0, 0, 120));
			p.drawRect(titleRegion);
			p.restore();
#endif
			paintWindowTitle(p, parWindow->windowTitle(), titleRegion);
		}
	}
}


void SARibbonBar::resizeStackedContainerWidget()
{
	if (m_d->stackedContainerWidget->isPopupMode()) {
		QPoint absPosition = mapToGlobal(QPoint(RibbonSubElementStyleOpt.widgetBord.left(), m_d->ribbonTabBar->geometry().bottom() + 1));
		m_d->stackedContainerWidget->setGeometry(absPosition.x()
			, absPosition.y()
			, width() - RibbonSubElementStyleOpt.widgetBord.left() - RibbonSubElementStyleOpt.widgetBord.right()
			, mainBarHeight() - m_d->ribbonTabBar->geometry().bottom() - RibbonSubElementStyleOpt.widgetBord.bottom() - 1);
	}
	else {
		m_d->stackedContainerWidget->setGeometry(RibbonSubElementStyleOpt.widgetBord.left()
			, m_d->ribbonTabBar->geometry().bottom() + 1
			, width() - RibbonSubElementStyleOpt.widgetBord.left() - RibbonSubElementStyleOpt.widgetBord.right()
			, mainBarHeight() - m_d->ribbonTabBar->geometry().bottom() - RibbonSubElementStyleOpt.widgetBord.bottom() - 1);
	}
}

void SARibbonBar::updateContextCategoryManagerData()
{
	const int c = m_d->ribbonTabBar->count();

	for (_SAContextCategoryManagerData& cd : m_d->currentShowingContextCategory)
	{
		cd.tabPageIndex.clear();
		for (int i = 0; i < cd.contextCategory->categoryCount(); ++i)
		{
			SARibbonCategory *category = cd.contextCategory->categoryPage(i);
			for (int t = 0; t < c; ++t)
			{
				QVariant v = m_d->ribbonTabBar->tabData(t);
				if (v.isValid()) {
					_SARibbonTabData d = v.value<_SARibbonTabData>();
					if (d.category == category) {
						cd.tabPageIndex.append(t);
					}
				}
				else {
					cd.tabPageIndex.append(-1);
				}
			}
		}
	}
}

void SARibbonBar::paintContextCategoryTab(QPainter& painter, const QString& title, QRect contextRect, const QColor& color)
{
	painter.save();
	painter.setPen(Qt::NoPen);
	painter.setBrush(color);
	painter.drawRect(QRect(contextRect.x(), RibbonSubElementStyleOpt.widgetBord.top(), contextRect.width(), 5));

	QColor gColor = color.lighter(190);

	contextRect -= QMargins(0, 5, 0, 0);
	painter.fillRect(contextRect, gColor);

	if (isOfficeStyle()) {
		if (!title.isEmpty()) {
			contextRect.setBottom(m_d->ribbonTabBar->geometry().top());
			painter.setPen(color);
			painter.drawText(contextRect, Qt::AlignCenter, title);
		}
	}
	painter.restore();
}


void SARibbonBar::resizeEvent(QResizeEvent *e)
{
	Q_UNUSED(e);
	if (isOfficeStyle()) {
		resizeInOfficeStyle();
	}
	else {
		resizeInWpsLiteStyle();
	}
	update();
}


void SARibbonBar::moveEvent(QMoveEvent *event)
{
	if (m_d->stackedContainerWidget) {
		if (m_d->stackedContainerWidget->isPopupMode()) {
			qDebug() << "moveEvent";
			resizeStackedContainerWidget();
		}
	}
	QWidget::moveEvent(event);
}


void SARibbonBar::resizeInOfficeStyle()
{
	updateRibbonElementGeometry();
	int x = RibbonSubElementStyleOpt.widgetBord.left();
	int y = RibbonSubElementStyleOpt.widgetBord.top();

	//cornerWidget - TopLeftCorner
	const int titleH = titleBarHeight();
	const int validTitleBarHeight = titleH - RibbonSubElementStyleOpt.widgetBord.top();
	const int tabH = tabBarHeight();

	x += m_d->iconRightBorderPosition + 5;
	if (QWidget *connerL = cornerWidget(Qt::TopLeftCorner)) {
		if (connerL->isVisible()) {
			QSize connerSize = connerL->sizeHint();
			if (connerSize.height() < validTitleBarHeight) {
				int detal = (validTitleBarHeight - connerSize.height()) / 2;
				connerL->setGeometry(x, y + detal, connerSize.width(), connerSize.height());
			}
			else {
				connerL->setGeometry(x, y, connerSize.width(), validTitleBarHeight);
			}
			x = connerL->geometry().right() + 5;
		}
	}
	//quick access bar
	if (m_d->quickAccessBar) {
		if (m_d->quickAccessBar->isVisible()) {
			QSize quickAccessBarSize = m_d->quickAccessBar->sizeHint();
			m_d->quickAccessBar->setGeometry(x, y, quickAccessBarSize.width(), validTitleBarHeight);
		}
	}
	x = RibbonSubElementStyleOpt.widgetBord.left();
	y = titleH + RibbonSubElementStyleOpt.widgetBord.top();
	//applicationButton 
	if (m_d->applicationButton) {
		if (m_d->applicationButton->isVisible()) {
			m_d->applicationButton->setGeometry(x, y, RibbonSubElementStyleOpt.applicationButtonWidth, tabH);
			x = m_d->applicationButton->geometry().right();
		}
	}
	//top right
	//cornerWidget - TopRightCorner
	int endX = width() - RibbonSubElementStyleOpt.widgetBord.right();

	if (QWidget *connerW = cornerWidget(Qt::TopRightCorner)) {
		if (connerW->isVisible()) {
			QSize connerSize = connerW->sizeHint();
			endX -= connerSize.width();
			if (connerSize.height() < tabH) {
				int detal = (tabH - connerSize.height()) / 2;
				connerW->setGeometry(endX, y + detal, connerSize.width(), connerSize.height());
			}
			else {
				connerW->setGeometry(endX, y, connerSize.width(), tabH);
			}
		}
	}
	//applicationButton和TopRightCorner,tab bar

	if (m_d->rightButtonGroup && m_d->rightButtonGroup->isVisible()) {
		QSize wSize = m_d->rightButtonGroup->sizeHint();
		endX -= wSize.width();
		m_d->rightButtonGroup->setGeometry(endX, y, wSize.width(), tabH);
	}
	int tabBarWidth = endX - x;

	m_d->ribbonTabBar->setGeometry(x, y, tabBarWidth, tabH);


	resizeStackedContainerWidget();
}


void SARibbonBar::resizeInWpsLiteStyle()
{
	updateRibbonElementGeometry();
	int x = RibbonSubElementStyleOpt.widgetBord.left();
	int y = RibbonSubElementStyleOpt.widgetBord.top();
	const int titleH = titleBarHeight();
	const int validTitleBarHeight = titleH - RibbonSubElementStyleOpt.widgetBord.top();

	//cornerWidget - TopRightCorner
	int endX = width() - RibbonSubElementStyleOpt.widgetBord.right() - m_d->windowButtonSize.width();

	if (QWidget *connerW = cornerWidget(Qt::TopRightCorner)) {
		if (connerW->isVisible()) {
			QSize connerSize = connerW->sizeHint();
			endX -= connerSize.width();
			if (connerSize.height() < validTitleBarHeight) {
				int detal = (validTitleBarHeight - connerSize.height()) / 2;
				connerW->setGeometry(endX, y + detal, connerSize.width(), connerSize.height());
			}
			else {
				connerW->setGeometry(endX, y, connerSize.width(), validTitleBarHeight);
			}
		}
	}

	//tabBar 
	if (m_d->rightButtonGroup && m_d->rightButtonGroup->isVisible()) {
		QSize wSize = m_d->rightButtonGroup->sizeHint();
		endX -= wSize.width();
		m_d->rightButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
	}
	//quick access bar
	if (m_d->quickAccessBar) {
		if (m_d->quickAccessBar->isVisible()) {
			QSize quickAccessBarSize = m_d->quickAccessBar->sizeHint();
			endX -= quickAccessBarSize.width();
			m_d->quickAccessBar->setGeometry(endX, y, quickAccessBarSize.width(), validTitleBarHeight);
		}
	}
	//cornerWidget - TopLeftCorner
	if (QWidget *connerL = cornerWidget(Qt::TopLeftCorner)) {
		if (connerL->isVisible()) {
			QSize connerSize = connerL->sizeHint();
			endX -= connerSize.width();
			if (connerSize.height() < validTitleBarHeight) {
				int detal = (validTitleBarHeight - connerSize.height()) / 2;
				connerL->setGeometry(endX, y + detal, connerSize.width(), connerSize.height());
			}
			else {
				connerL->setGeometry(endX, y, connerSize.width(), validTitleBarHeight);
			}
		}
	}

	//tab
	int tabH = tabBarHeight();

	if (tabH > validTitleBarHeight) {
		tabH = validTitleBarHeight;
	}
	y = y + validTitleBarHeight - tabH;

	if (m_d->applicationButton) {
		if (m_d->applicationButton->isVisible()) {
			m_d->applicationButton->setGeometry(x, y, RibbonSubElementStyleOpt.applicationButtonWidth, tabH);
			x = m_d->applicationButton->geometry().right() + 2;
		}
	}

	int tabBarWidth = endX - x;
	//20200831
	int mintabBarWidth = calcMinTabBarWidth();

	if (mintabBarWidth < tabBarWidth) {
		tabBarWidth = mintabBarWidth;
	}
	m_d->ribbonTabBar->setGeometry(x, y, tabBarWidth, tabH);

	resizeStackedContainerWidget();
}


void SARibbonBar::paintBackground(QPainter& painter)
{
	painter.save();
	const int lineY = m_d->ribbonTabBar->geometry().bottom();
	QPen pen(RibbonSubElementStyleOpt.tabBarBaseLineColor);

	pen.setWidth(1);
	pen.setStyle(Qt::SolidLine);
	painter.setPen(pen);
	painter.drawLine(QPoint(RibbonSubElementStyleOpt.widgetBord.left(), lineY), QPoint(width() - RibbonSubElementStyleOpt.widgetBord.right() - 1, lineY));
	painter.restore();
}


void SARibbonBar::paintWindowTitle(QPainter& painter, const QString& title, const QRect& titleRegion)
{
	painter.save();
	painter.setPen(RibbonSubElementStyleOpt.titleTextColor);
	painter.drawText(titleRegion, Qt::AlignCenter, title);
	painter.restore();
}
