﻿#include "SARibbonMainWindow.h"
#include <QWindowStateChangeEvent>
#include "SAFramelessHelper.h"
#include "SAWindowButtonGroup.h"
#include "SARibbonBar.h"
#include <QApplication>
#include <QDebug>
#include <QHash>
#include <QFile>

/**
 * @brief The SARibbonMainWindowPrivate class
 */
class SARibbonMainWindowPrivate
{
public:
	SARibbonMainWindowPrivate(SARibbonMainWindow *p);
	void init();

	SARibbonMainWindow *Parent;
	SARibbonBar *ribbonBar;
	SARibbonMainWindow::RibbonTheme currentRibbonTheme;
	SAWindowButtonGroup *windowButtonGroup;
	SAFramelessHelper *pFramelessHelper;
	bool useRibbon;
};

SARibbonMainWindowPrivate::SARibbonMainWindowPrivate(SARibbonMainWindow *p)
	: Parent(p)
	, ribbonBar(nullptr)
	, currentRibbonTheme(SARibbonMainWindow::Office2013)
	, windowButtonGroup(nullptr)
	, pFramelessHelper(nullptr)
	, useRibbon(true)
{
}


void SARibbonMainWindowPrivate::init()
{
}


SARibbonMainWindow::SARibbonMainWindow(QWidget *parent, bool useRibbon)
	: QMainWindow(parent)
	, m_d(new SARibbonMainWindowPrivate(this))
{
	m_d->init();
	m_d->useRibbon = useRibbon;
	qDebug() << windowFlags();
	if (useRibbon) {
		qDebug() << "sa ribbon version:" << SA_RIBBON_BAR_VERSION_STR;
		setRibbonTheme(ribbonTheme());
		setMenuWidget(new SARibbonBar(this));
#ifdef Q_OS_UNIX
		setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#endif
	}
}


SARibbonMainWindow::~SARibbonMainWindow()
{
	delete m_d;
}


const SARibbonBar *SARibbonMainWindow::ribbonBar() const
{
	return (m_d->ribbonBar);
}



SARibbonBar *SARibbonMainWindow::ribbonBar()
{
	return (m_d->ribbonBar);
}


SAFramelessHelper *SARibbonMainWindow::framelessHelper()
{
	return (m_d->pFramelessHelper);
}


void SARibbonMainWindow::setRibbonTheme(SARibbonMainWindow::RibbonTheme theme)
{
	switch (theme)
	{
	case NormalTheme:
		loadTheme(":/theme/resource/default.qss");
		break;

	case Office2013:
		loadTheme(":/theme/resource/office2013.qss");
		break;

	default:
		loadTheme(":/theme/resource/default.qss");
		break;
	}
}


SARibbonMainWindow::RibbonTheme SARibbonMainWindow::ribbonTheme() const
{
	return (m_d->currentRibbonTheme);
}


bool SARibbonMainWindow::isUseRibbon() const
{
	return (m_d->useRibbon);
}



void SARibbonMainWindow::updateWindowFlag(Qt::WindowFlags flags)
{
	if (isUseRibbon()) {
		m_d->windowButtonGroup->updateWindowFlag(flags);
	}
	repaint();
}



Qt::WindowFlags SARibbonMainWindow::windowButtonFlags() const
{
	if (isUseRibbon()) {
		return (m_d->windowButtonGroup->windowButtonFlags());
	}
	return (windowFlags());
}


void SARibbonMainWindow::setMenuWidget(QWidget *menubar)
{
	QMainWindow::setMenuWidget(menubar);
	SARibbonBar *bar = qobject_cast<SARibbonBar *>(menubar);

	if (bar) {
		m_d->ribbonBar = bar;
		m_d->ribbonBar->installEventFilter(this);
		if (nullptr == m_d->pFramelessHelper) {
			m_d->pFramelessHelper = new SAFramelessHelper(this);
		}
		m_d->pFramelessHelper->setTitleHeight(m_d->ribbonBar->titleBarHeight());
		if (nullptr == m_d->windowButtonGroup) {
			m_d->windowButtonGroup = new SAWindowButtonGroup(this);
		}
		QSize s = m_d->windowButtonGroup->size();
		s.setHeight(m_d->ribbonBar->titleBarHeight());
		m_d->windowButtonGroup->setFixedSize(s);
		m_d->windowButtonGroup->setWindowStates(windowState());
		m_d->useRibbon = true;
		m_d->windowButtonGroup->show();
	}
	else {
		m_d->ribbonBar = nullptr;
		m_d->useRibbon = false;
		if (m_d->pFramelessHelper) {
			delete m_d->pFramelessHelper;
			m_d->pFramelessHelper = nullptr;
		}
		if (m_d->windowButtonGroup) {
			m_d->windowButtonGroup->hide();
		}
	}
}


void SARibbonMainWindow::setMenuBar(QMenuBar *menuBar)
{
	QMainWindow::setMenuBar(menuBar);
	SARibbonBar *bar = qobject_cast<SARibbonBar *>(menuBar);

	if (bar) {
		m_d->ribbonBar = bar;
		m_d->ribbonBar->installEventFilter(this);
		if (nullptr == m_d->pFramelessHelper) {
			m_d->pFramelessHelper = new SAFramelessHelper(this);
		}
		m_d->pFramelessHelper->setTitleHeight(m_d->ribbonBar->titleBarHeight());
		if (nullptr == m_d->windowButtonGroup) {
			m_d->windowButtonGroup = new SAWindowButtonGroup(this);
		}
		QSize s = m_d->windowButtonGroup->size();
		s.setHeight(m_d->ribbonBar->titleBarHeight());
		m_d->windowButtonGroup->setFixedSize(s);
		m_d->windowButtonGroup->setWindowStates(windowState());
		m_d->useRibbon = true;
		m_d->windowButtonGroup->show();
	}
	else {
		m_d->ribbonBar = nullptr;
		m_d->useRibbon = false;
		if (m_d->pFramelessHelper) {
			delete m_d->pFramelessHelper;
			m_d->pFramelessHelper = nullptr;
		}
		if (m_d->windowButtonGroup) {
			m_d->windowButtonGroup->hide();
		}
	}
}


void SARibbonMainWindow::resizeEvent(QResizeEvent *event)
{
	if (m_d->ribbonBar) {
		if (m_d->ribbonBar->size().width() != this->size().width()) {
			m_d->ribbonBar->setFixedWidth(this->size().width());
		}
		if (m_d->windowButtonGroup) {
			m_d->ribbonBar->setWindowButtonSize(m_d->windowButtonGroup->size());
		}
	}
	QMainWindow::resizeEvent(event);
}


bool SARibbonMainWindow::eventFilter(QObject *obj, QEvent *e)
{
	if (obj == m_d->ribbonBar) {
		switch (e->type())
		{
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseMove:
		case QEvent::Leave:
		case QEvent::HoverMove:
		case QEvent::MouseButtonDblClick:
			QApplication::sendEvent(this, e);

		default:
			break;
		}
	}
	return (QMainWindow::eventFilter(obj, e));
}


bool SARibbonMainWindow::event(QEvent *e)
{
	if (e) {
		switch (e->type())
		{
		case QEvent::WindowStateChange:
		{
			if (isUseRibbon()) {
				m_d->windowButtonGroup->setWindowStates(windowState());
			}
		}
		break;

		default:
			break;
		}
	}
	return (QMainWindow::event(e));
}


void SARibbonMainWindow::loadTheme(const QString& themeFile)
{
	QFile file(themeFile);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}
	setStyleSheet(file.readAll());
}
