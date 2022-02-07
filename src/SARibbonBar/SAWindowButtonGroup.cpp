#include "SAWindowButtonGroup.h"
#include <QToolButton>
#include <QResizeEvent>
#include <QStyle>
#include <QDebug>
#include <QScopedPointer>

class SAWindowButtonGroupPrivate
{
public:
	SAWindowButtonGroup *q_d;
	SAWindowToolButton *buttonClose;
	SAWindowToolButton *buttonMinimize;
	SAWindowToolButton *buttonMaximize;
	int mCloseStretch;
	int mMaxStretch;
	int mMinStretch;
	qreal mIconscale;
	Qt::WindowFlags mFlags;
	SAWindowButtonGroupPrivate(SAWindowButtonGroup *p)
		: q_d(p)
		, buttonClose(nullptr)
		, buttonMinimize(nullptr)
		, buttonMaximize(nullptr)
		, mCloseStretch(4)
		, mMaxStretch(3)
		, mMinStretch(3)
		, mIconscale(0.5)
	{
	}


	void setupMinimizeButton(bool on)
	{
		SAWindowButtonGroup *par = q_d;

		if (on) {
			if (buttonMinimize) {
				buttonMinimize->deleteLater();
				buttonMinimize = nullptr;
			}
			buttonMinimize = new SAWindowToolButton(par);
			buttonMinimize->setObjectName(QStringLiteral("SAMinimizeWindowButton"));
			buttonMinimize->setFixedSize(30, 28);
			buttonMinimize->setFocusPolicy(Qt::NoFocus);//避免铺抓到
			buttonMinimize->setIconSize(buttonMinimize->size()*mIconscale);
			buttonMinimize->show();
			par->connect(buttonMinimize, &QAbstractButton::clicked
				, par, &SAWindowButtonGroup::minimizeWindow);
		}
		else {
			if (buttonMinimize) {
				delete buttonMinimize;
				buttonMinimize = nullptr;
			}
		}
		updateSize();
	}


	void setupMaximizeButton(bool on)
	{
		SAWindowButtonGroup *par = q_d;

		if (on) {
			if (buttonMaximize) {
				buttonMaximize->deleteLater();
				buttonMaximize = nullptr;
			}
			buttonMaximize = new SAWindowToolButton(par);
			buttonMaximize->setObjectName(QStringLiteral("SAMaximizeWindowButton"));
			buttonMaximize->setFixedSize(30, 28);
			buttonMaximize->setCheckable(true);
			buttonMaximize->setFocusPolicy(Qt::NoFocus);
			buttonMaximize->setIconSize(buttonMaximize->size()*mIconscale);
			buttonMaximize->show();
			par->connect(buttonMaximize, &QAbstractButton::clicked
				, par, &SAWindowButtonGroup::maximizeWindow);
		}
		else {
			if (buttonMaximize) {
				delete buttonMaximize;
				buttonMaximize = nullptr;
			}
		}
		updateSize();
	}


	void setupCloseButton(bool on)
	{
		SAWindowButtonGroup *par = q_d;

		if (on) {
			if (buttonClose) {
				buttonClose->deleteLater();
				buttonClose = nullptr;
			}
			buttonClose = new SAWindowToolButton(par);
			buttonClose->setObjectName(QStringLiteral("SACloseWindowButton"));
			buttonClose->setFixedSize(40, 28);
			buttonClose->setFocusPolicy(Qt::NoFocus);
			//buttonClose->setFlat(true);
			par->connect(buttonClose, &QAbstractButton::clicked
				, par, &SAWindowButtonGroup::closeWindow);
			buttonClose->setIconSize(buttonClose->size()*mIconscale);
			buttonClose->show();
		}
		else {
			if (buttonClose) {
				delete buttonClose;
				buttonClose = nullptr;
			}
		}
		updateSize();
	}


	void updateSize()
	{
		q_d->setFixedSize(sizeHint());
		resize(q_d->size());
		//        int span = 0;

		//        if (buttonClose) {
		//            buttonClose->move(q_d->width()-buttonClose->width()
		//                , q_d->y());
		//            span = buttonClose->width();
		//        }
		//        if (buttonMaximize) {
		//            buttonMaximize->move(q_d->width()-buttonMaximize->width()-span
		//                , q_d->y());
		//            span += buttonMaximize->width();
		//        }
		//        if (buttonMinimize) {
		//            buttonMinimize->move(q_d->width()-buttonMinimize->width()-span
		//                , q_d->y());
		//        }
	}


	void resize(QSize size)
	{
		qreal tw = 0;

		if (buttonClose) {
			tw += mCloseStretch;
		}
		if (buttonMaximize) {
			tw += mMaxStretch;
		}
		if (buttonMinimize) {
			tw += mMinStretch;
		}

		int x = 0;

		if (buttonMinimize) {
			int w = (mMinStretch / tw) * size.width();
			//buttonMinimize->setGeometry(x, 0, w, size.height());
			buttonMinimize->setFixedSize(w, size.height());
			buttonMinimize->move(x, 0);
			x += w;
		}
		if (buttonMaximize) {
			int w = (mMaxStretch / tw) * size.width();
			//buttonMaximize->setGeometry(x, 0, w, size.height());
			buttonMaximize->setFixedSize(w, size.height());
			buttonMaximize->move(x, 0);
			x += w;
		}
		if (buttonClose) {
			int w = (mCloseStretch / tw) * size.width();
			//buttonClose->setGeometry(x, 0, w, size.height());
			buttonClose->setFixedSize(w, size.height());
			buttonClose->move(x, 0);
		}
	}


	QSize sizeHint() const
	{
		int width = 0;
		int height = 28;

		if (buttonClose) {
			width += 40;
		}
		if (buttonMaximize) {
			width += 30;
		}
		if (buttonMinimize) {
			width += 30;
		}
		return (QSize(width, height));
	}
};

SAWindowToolButton::SAWindowToolButton(QWidget *p) : QPushButton(p)
{
	setFlat(true);
}


SAWindowButtonGroup::SAWindowButtonGroup(QWidget *parent) : QWidget(parent)
, m_d(new SAWindowButtonGroupPrivate(this))
{
	updateWindowFlag();
	if (parent) {
		parent->installEventFilter(this);
	}
}



SAWindowButtonGroup::SAWindowButtonGroup(QWidget *parent, Qt::WindowFlags flags) : QWidget(parent)
, m_d(new SAWindowButtonGroupPrivate(this))
{
	m_d->mFlags = flags;
	updateWindowFlag();
	if (parent) {
		parent->installEventFilter(this);
	}
}


SAWindowButtonGroup::~SAWindowButtonGroup()
{
	delete m_d;
}


void SAWindowButtonGroup::setupMinimizeButton(bool on)
{
	m_d->setupMinimizeButton(on);
}


void SAWindowButtonGroup::setupMaximizeButton(bool on)
{
	m_d->setupMaximizeButton(on);
}


void SAWindowButtonGroup::setupCloseButton(bool on)
{
	m_d->setupCloseButton(on);
}


void SAWindowButtonGroup::updateWindowFlag()
{
	Qt::WindowFlags flags = parentWidget()->windowFlags();

	m_d->mFlags = flags;

	setupMinimizeButton(flags & Qt::WindowMinimizeButtonHint);

	setupMaximizeButton(flags & Qt::WindowMaximizeButtonHint);

	setupCloseButton(flags & Qt::WindowCloseButtonHint);
}



void SAWindowButtonGroup::updateWindowFlag(Qt::WindowFlags flags)
{
	if (flags & Qt::WindowCloseButtonHint) {
		m_d->mFlags |= Qt::WindowCloseButtonHint;
	}
	else {
		m_d->mFlags &= (~Qt::WindowCloseButtonHint);
	}

	if (flags & Qt::WindowMaximizeButtonHint) {
		m_d->mFlags |= Qt::WindowMaximizeButtonHint;
	}
	else {
		m_d->mFlags &= (~Qt::WindowMaximizeButtonHint);
	}

	if (flags & Qt::WindowMinimizeButtonHint) {
		m_d->mFlags |= Qt::WindowMinimizeButtonHint;
	}
	else {
		m_d->mFlags &= (~Qt::WindowMinimizeButtonHint);
	}
	setupMinimizeButton(flags & Qt::WindowMinimizeButtonHint);

	setupMaximizeButton(flags & Qt::WindowMaximizeButtonHint);

	setupCloseButton(flags & Qt::WindowCloseButtonHint);
}



void SAWindowButtonGroup::setButtonWidthStretch(int close, int max, int min)
{
	m_d->mMaxStretch = max;
	m_d->mMinStretch = min;
	m_d->mCloseStretch = close;
}



void SAWindowButtonGroup::setIconScale(qreal iconscale)
{
	m_d->mIconscale = iconscale;
}



void SAWindowButtonGroup::setWindowStates(Qt::WindowStates s)
{
	if (m_d->buttonMaximize) {
		bool on = s.testFlag(Qt::WindowMaximized);
		m_d->buttonMaximize->setChecked(on);
		m_d->buttonMaximize->setToolTip(on ? tr("Restore") : tr("Maximize"));
	}
}



Qt::WindowFlags SAWindowButtonGroup::windowButtonFlags() const
{
	Qt::WindowFlags f = Qt::Widget;

	if (m_d->mFlags & Qt::WindowCloseButtonHint) {
		f |= Qt::WindowCloseButtonHint;
	}
	if (m_d->mFlags & Qt::WindowMaximizeButtonHint) {
		f |= Qt::WindowMaximizeButtonHint;
	}
	if (m_d->mFlags & Qt::WindowMinimizeButtonHint) {
		f |= Qt::WindowMinimizeButtonHint;
	}
	return (f);
}


QSize SAWindowButtonGroup::sizeHint() const
{
	return (m_d->sizeHint());
}


bool SAWindowButtonGroup::eventFilter(QObject *watched, QEvent *e)
{

	if (watched == parentWidget()) {
		switch (e->type())
		{
		case QEvent::Resize:
			parentResize();
			break;

		default:
			break;
		}
	}
	return (false);
}


void SAWindowButtonGroup::parentResize()
{
	QWidget *par = parentWidget();

	if (par) {
		QSize parSize = par->size();
		move(parSize.width() - width() - 1, 1);
	}
}


void SAWindowButtonGroup::resizeEvent(QResizeEvent *e)
{
	m_d->resize(e->size());
}


void SAWindowButtonGroup::closeWindow()
{
	if (parentWidget()) {
		parentWidget()->close();
	}
}


void SAWindowButtonGroup::minimizeWindow()
{
	if (parentWidget()) {
		parentWidget()->showMinimized();
	}
}


void SAWindowButtonGroup::maximizeWindow()
{
	QWidget *par = parentWidget();

	if (par) {
		if (par->isMaximized()) {
			par->showNormal();
		}
		else {
			par->showMaximized();
		}
	}
}
