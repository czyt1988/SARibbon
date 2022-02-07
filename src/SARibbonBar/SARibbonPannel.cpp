#include "SARibbonPannel.h"
#include "SARibbonToolButton.h"
#include <QApplication>
#include <QResizeEvent>
#include <QAction>
#include <QIcon>
#include <QDebug>
#include <QGridLayout>
#include <QFontMetrics>
#include <QPainter>
#include <QApplication>
#include <QWidgetAction>
#include <QMenu>
#include "SARibbonPannelLayout.h"
#include "SARibbonElementManager.h"
#include "SARibbonCategory.h"
#include "SARibbonPannelOptionButton.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonGallery.h"
#include "SARibbonElementManager.h"
#include "SARibbonMenu.h"

#define HELP_DRAW_RECT(p, rect)		 \
    do{				 \
        p.save();		 \
        QPen _pen(Qt::DashLine); \
        _pen.setColor(Qt::blue); \
        p.setPen(_pen);		 \
        p.setBrush(QBrush());	 \
        p.drawRect(rect);	 \
        p.restore();		 \
    }while(0)


class SARibbonPannelPrivate
{
public:
	SARibbonPannelPrivate(SARibbonPannel *p);
	int rowadded();
	void createLayout();

	SARibbonToolButton *lastAddActionButton();

	SARibbonPannel *Parent;
	SARibbonPannelLayout *m_layout;
	QPoint m_nextElementPosition;
	int m_row;
	SARibbonPannelOptionButton *m_optionActionButton;
	SARibbonPannel::PannelLayoutMode m_pannelLayoutMode;
	bool m_isCanCustomize;
};

SARibbonPannelPrivate::SARibbonPannelPrivate(SARibbonPannel *p)
	: Parent(p)
	, m_layout(nullptr)
	, m_nextElementPosition(3, 3)
	, m_row(0)
	, m_optionActionButton(nullptr)
	, m_pannelLayoutMode(SARibbonPannel::ThreeRowMode)
	, m_isCanCustomize(true)
{
	createLayout();
}


int SARibbonPannelPrivate::rowadded()
{
	switch (m_pannelLayoutMode)
	{
	case SARibbonPannel::ThreeRowMode:
		return (2);

	case SARibbonPannel::TwoRowMode:
		return (3);

	default:
		break;
	}
	return (2);
}


void SARibbonPannelPrivate::createLayout()
{
	m_layout = new SARibbonPannelLayout(Parent);
	m_layout->setSpacing(2);
	m_layout->setContentsMargins(2, 2, 2, 2);
}


SARibbonToolButton *SARibbonPannelPrivate::lastAddActionButton()
{
	QWidget *w = m_layout->lastWidget();

	return (qobject_cast<SARibbonToolButton *>(w));
}


//==================================================
// SARibbonPannel
//==================================================



SARibbonPannel::SARibbonPannel(QWidget *parent) : QWidget(parent)
, m_d(new SARibbonPannelPrivate(this))
{
	setPannelLayoutMode(ThreeRowMode);
}


SARibbonPannel::SARibbonPannel(const QString& name, QWidget *parent) : QWidget(parent)
, m_d(new SARibbonPannelPrivate(this))
{
	setPannelLayoutMode(ThreeRowMode);
	setPannelName(name);
}


SARibbonPannel::~SARibbonPannel()
{
	delete m_d;
}

void SARibbonPannel::setActionRowProportionProperty(QAction *action, SARibbonPannelItem::RowProportion rp)
{
	action->setProperty(SARibbonPannelItemRowProportionPropertyName, int(rp));
}



SARibbonPannelItem::RowProportion SARibbonPannel::getActionRowProportionProperty(QAction *action)
{
	bool isok = false;
	int r = action->property(SARibbonPannelItemRowProportionPropertyName).toInt(&isok);

	if (isok) {
		return (static_cast<SARibbonPannelItem::RowProportion>(r));
	}
	return (SARibbonPannelItem::Large);
}



void SARibbonPannel::setActionRowProportion(QAction *action, SARibbonPannelItem::RowProportion rp)
{
	SARibbonPannelLayout *lay = m_d->m_layout;

	setActionRowProportionProperty(action, rp);
	if (lay) {
		SARibbonPannelItem *it = lay->pannelItem(action);
		if (it) {
			it->rowProportion = rp;
			lay->invalidate();
		}
	}
}


SARibbonToolButton *SARibbonPannel::addAction(QAction *action, SARibbonPannelItem::RowProportion rp)
{
	setActionRowProportionProperty(action, rp);
	addAction(action);

	return (m_d->lastAddActionButton());
}


SARibbonToolButton *SARibbonPannel::addLargeAction(QAction *action)
{
	return (addAction(action, SARibbonPannelItem::Large));
}


SARibbonToolButton *SARibbonPannel::addMediumAction(QAction *action)
{
	return (addAction(action, SARibbonPannelItem::Medium));
}


SARibbonToolButton *SARibbonPannel::addSmallAction(QAction *action)
{
	return (addAction(action, SARibbonPannelItem::Small));
}


QAction *SARibbonPannel::addAction(const QString& text, const QIcon& icon
	, QToolButton::ToolButtonPopupMode popMode
	, SARibbonPannelItem::RowProportion rp)
{
	QAction *action = new QAction(icon, text, this);

	setActionRowProportionProperty(action, rp);
	addAction(action);
	SARibbonToolButton *btn = m_d->lastAddActionButton();

	if (btn) {
		btn->setPopupMode(popMode);
	}
	return (action);
}


SARibbonToolButton *SARibbonPannel::addMenu(QMenu *menu, SARibbonPannelItem::RowProportion rp, QToolButton::ToolButtonPopupMode popMode)
{
	QAction *action = menu->menuAction();

	addAction(action, rp);
	SARibbonToolButton *btn = m_d->lastAddActionButton();

	btn->setPopupMode(popMode);
	return (btn);
}


SARibbonToolButton *SARibbonPannel::addActionMenu(QAction *action, QMenu *menu, SARibbonPannelItem::RowProportion rp)
{
	addAction(action, rp);
	SARibbonToolButton *btn = m_d->lastAddActionButton();

	btn->setMenu(menu);
	btn->setPopupMode(QToolButton::MenuButtonPopup);
	return (btn);
}


SARibbonToolButton *SARibbonPannel::addLargeActionMenu(QAction *action, QMenu *menu)
{
	return (addActionMenu(action, menu, SARibbonPannelItem::Large));
}


SARibbonToolButton *SARibbonPannel::addLargeMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode)
{
	return (addMenu(menu, SARibbonPannelItem::Large, popMode));
}


SARibbonToolButton *SARibbonPannel::addSmallMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode)
{
	return (addMenu(menu, SARibbonPannelItem::Small, popMode));
}


QAction *SARibbonPannel::addWidget(QWidget *w, SARibbonPannelItem::RowProportion rp)
{
	QWidgetAction *action = new QWidgetAction(this);

	action->setDefaultWidget(w);
	w->setAttribute(Qt::WA_Hover);
	setActionRowProportionProperty(action, rp);
	addAction(action);
	return (action);
}


QAction *SARibbonPannel::addSmallWidget(QWidget *w)
{
	return (addWidget(w, SARibbonPannelItem::Small));
}


QAction *SARibbonPannel::addLargeWidget(QWidget *w)
{
	return (addWidget(w, SARibbonPannelItem::Large));
}


SARibbonGallery *SARibbonPannel::addGallery()
{
	SARibbonGallery *gallery = RibbonSubElementDelegate->createRibbonGallery(this);

	addWidget(gallery, SARibbonPannelItem::Large);

	setExpanding();
	return (gallery);
}


QAction *SARibbonPannel::addSeparator(int top, int bottom)
{
	QAction *action = new QAction(this);

	action->setSeparator(true);
	setActionRowProportionProperty(action, SARibbonPannelItem::Large);
	addAction(action);
	QWidget *w = m_d->m_layout->lastWidget();
	SARibbonSeparatorWidget *sep = qobject_cast<SARibbonSeparatorWidget *>(w);

	if (sep) {
		sep->setTopBottomMargins(top, bottom);
	}
	return (action);
}


SARibbonToolButton *SARibbonPannel::actionToRibbonToolButton(QAction *action)
{
	SARibbonPannelLayout *lay = qobject_cast<SARibbonPannelLayout *>(layout());

	if (lay) {
		int index = lay->indexOf(action);
		if (index == -1) {
			return (nullptr);
		}
		QLayoutItem *item = lay->takeAt(index);
		SARibbonToolButton *btn = qobject_cast<SARibbonToolButton *>(item ? item->widget() : nullptr);
		return (btn);
	}
	return (nullptr);
}


QList<SARibbonToolButton *> SARibbonPannel::ribbonToolButtons() const
{
	const QObjectList& objs = children();
	QList<SARibbonToolButton *> res;

	for (QObject *o : objs)
	{
		SARibbonToolButton *b = qobject_cast<SARibbonToolButton *>(o);
		if (b) {
			res.append(b);
		}
	}
	return (res);
}


void SARibbonPannel::setPannelLayoutMode(SARibbonPannel::PannelLayoutMode mode)
{
	if (m_d->m_pannelLayoutMode == mode) {
		return;
	}
	m_d->m_pannelLayoutMode = mode;
	resetLayout(mode);
	resetLargeToolButtonStyle();
}


SARibbonPannel::PannelLayoutMode SARibbonPannel::pannelLayoutMode() const
{
	return (m_d->m_pannelLayoutMode);
}


void SARibbonPannel::addOptionAction(QAction *action)
{
	if (nullptr == action) {
		if (m_d->m_optionActionButton) {
			delete m_d->m_optionActionButton;
			m_d->m_optionActionButton = nullptr;
		}
		return;
	}
	if (nullptr == m_d->m_optionActionButton) {
		m_d->m_optionActionButton = RibbonSubElementDelegate->createRibbonPannelOptionButton(this);
	}
	m_d->m_optionActionButton->setFixedSize(optionActionButtonSize());
	m_d->m_optionActionButton->setIconSize(optionActionButtonSize() - QSize(-2, -2));
	m_d->m_optionActionButton->connectAction(action);
	updateGeometry();
	repaint();
}


bool SARibbonPannel::isHaveOptionAction() const
{
	return (m_d->m_optionActionButton != nullptr);
}


QSize SARibbonPannel::maxHightIconSize(const QSize& size, int h)
{
	if (size.height() < h) {
		return (size * ((float)h / size.height()));
	}
	return (size);
}


void SARibbonPannel::paintEvent(QPaintEvent *event)
{
	QPainter p(this);

#ifdef SA_RIBBON_DEBUG_HELP_DRAW
	HELP_DRAW_RECT(p, rect());
#endif
	if (ThreeRowMode == pannelLayoutMode()) {
		const int th = titleHeight();
		QFont f = font();
		f.setPixelSize(th * 0.6);
		p.setFont(f);
		if (m_d->m_optionActionButton) {
			p.drawText(1, height() - th
				, width() - m_d->m_optionActionButton->width() - 4
				, th
				, Qt::AlignCenter
				, windowTitle());
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
			QRect r = QRect(1, height() - th
				, width() - m_d->m_optionActionButton->width() - 4
				, th - 2);
			HELP_DRAW_RECT(p, r);
#endif
		}
		else {
			p.drawText(1, height() - th
				, width()
				, th
				, Qt::AlignCenter
				, windowTitle());
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
			QRect r = QRect(1, height() - th
				, width()
				, th);
			HELP_DRAW_RECT(p, r);
#endif
		}
	}

	QWidget::paintEvent(event);
}


QSize SARibbonPannel::sizeHint() const
{
	QSize laySize = layout()->sizeHint();
	int maxWidth = laySize.width() + 2;

	if (ThreeRowMode == pannelLayoutMode()) {
		QFontMetrics fm = fontMetrics();
		QSize titleSize = fm.size(Qt::TextShowMnemonic, windowTitle());
		if (m_d->m_optionActionButton) {
			titleSize.setWidth(titleSize.width() + m_d->m_optionActionButton->width() + 4);
		}
		maxWidth = qMax(maxWidth, titleSize.width());
	}
	return (QSize(maxWidth, laySize.height()));
}


QSize SARibbonPannel::minimumSizeHint() const
{
	return (layout()->minimumSize());
}


void SARibbonPannel::setExpanding(bool isExpanding)
{
	setSizePolicy(isExpanding ? QSizePolicy::Expanding : QSizePolicy::Preferred
		, QSizePolicy::Fixed);
}


bool SARibbonPannel::isExpanding() const
{
	QSizePolicy sp = sizePolicy();

	return (sp.horizontalPolicy() == QSizePolicy::Expanding);
}


int SARibbonPannel::titleHeight() const
{
	return (isTwoRow() ? 0 : RibbonSubElementStyleOpt.pannelTitleHeight);
}


QSize SARibbonPannel::optionActionButtonSize() const
{
	return (isTwoRow() ? QSize(12, 12) : QSize(16, 16));
}


int SARibbonPannel::actionIndex(QAction *act) const
{
	return (m_d->m_layout->indexOf(act));
}


void SARibbonPannel::moveAction(int from, int to)
{
	m_d->m_layout->move(from, to);
	updateGeometry();
}


bool SARibbonPannel::isCanCustomize() const
{
	return (m_d->m_isCanCustomize);
}


void SARibbonPannel::setCanCustomize(bool b)
{
	m_d->m_isCanCustomize = b;
}


QString SARibbonPannel::pannelName() const
{
	return (windowTitle());
}


void SARibbonPannel::setPannelName(const QString& title)
{
	setWindowTitle(title);
	update();
}


void SARibbonPannel::resetLayout(PannelLayoutMode newmode)
{
	Q_UNUSED(newmode);
	layout()->setSpacing(TwoRowMode == newmode ? 4 : 2);
	updateGeometry();
}


void SARibbonPannel::resetLargeToolButtonStyle()
{
	QList<SARibbonToolButton *> btns = ribbonToolButtons();

	for (SARibbonToolButton *b : btns)
	{
		if ((nullptr == b) || (SARibbonToolButton::LargeButton != b->buttonType())) {
			continue;
		}
		if (ThreeRowMode == pannelLayoutMode()) {
			if (SARibbonToolButton::Normal != b->largeButtonType()) {
				b->setLargeButtonType(SARibbonToolButton::Normal);
			}
		}
		else {
			if (SARibbonToolButton::Lite != b->largeButtonType()) {
				b->setLargeButtonType(SARibbonToolButton::Lite);
			}
		}
	}
}


void SARibbonPannel::resizeEvent(QResizeEvent *event)
{
	if (m_d->m_optionActionButton) {
		if (ThreeRowMode == pannelLayoutMode()) {
			m_d->m_optionActionButton->move(width() - m_d->m_optionActionButton->width() - 2
				, height() - titleHeight()
				+ (titleHeight() - m_d->m_optionActionButton->height()) / 2);
		}
		else {
			m_d->m_optionActionButton->move(width() - m_d->m_optionActionButton->width()
				, height() - m_d->m_optionActionButton->height());
		}
	}
	return (QWidget::resizeEvent(event));
}


void SARibbonPannel::actionEvent(QActionEvent *e)
{
	QAction *action = e->action();
	QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action);

	switch (e->type())
	{
	case QEvent::ActionAdded:
	{
		SARibbonPannelLayout *lay = m_d->m_layout;
		if (nullptr != widgetAction) {
			if (widgetAction->parent() != this) {
				widgetAction->setParent(this);
			}
		}
		int index = layout()->count();
		if (e->before()) {
			index = lay->indexOf(action);
			if (-1 == index) {
				index = layout()->count();
			}
		}
		lay->insertAction(index, action, getActionRowProportionProperty(action));
		if (parentWidget()) {
			QApplication::postEvent(parentWidget(), new QEvent(QEvent::LayoutRequest));
		}
	}
	break;

	case QEvent::ActionChanged:
	{
		layout()->invalidate();
		if (parentWidget()) {
			QApplication::postEvent(parentWidget(), new QEvent(QEvent::LayoutRequest));
		}
	}
	break;

	case QEvent::ActionRemoved:
	{
		SARibbonPannelLayout *lay = m_d->m_layout;
		action->disconnect(this);
		int index = lay->indexOf(action);
		if (index != -1) {
			QLayoutItem *item = lay->takeAt(index);
			delete  item;
		}
		if (parentWidget()) {
			QApplication::postEvent(parentWidget(), new QEvent(QEvent::LayoutRequest));
		}
	}
	break;

	default:
		break;
	}
}


const QList<SARibbonPannelItem *>& SARibbonPannel::ribbonPannelItem() const
{
	return (m_d->m_layout->m_items);
}
