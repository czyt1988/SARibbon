#include "SARibbonMainWindow.h"
#include "FramelessHelper.h"
#include "SAWindowButtonGroup.h"
#include "SARibbonBar.h"
#include <QApplication>
#include <QDebug>
#include <QHash>
#include <QFile>
class SARibbonMainWindowPrivate
{
public:
    SARibbonMainWindowPrivate(SARibbonMainWindow *p);
    void init();

    SARibbonMainWindow *Parent;
    SARibbonBar *ribbonBar;
    SARibbonMainWindow::RibbonTheme currentRibbonTheme;
    SAWindowButtonGroup *windowButtonGroup;
	bool useRibbon;
};

SARibbonMainWindowPrivate::SARibbonMainWindowPrivate(SARibbonMainWindow *p)
    : Parent(p)
	, ribbonBar(nullptr)
    , currentRibbonTheme(SARibbonMainWindow::NormalTheme)
	, windowButtonGroup(nullptr)
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
    if (useRibbon) {
		setRibbonTheme(ribbonTheme());
		//
		m_d->ribbonBar = new SARibbonBar(this);

		setMenuWidget(m_d->ribbonBar);
		m_d->ribbonBar->installEventFilter(this);
		//
		FramelessHelper *pHelper = new FramelessHelper(this);
        m_d->windowButtonGroup = new SAWindowButtonGroup(this);

		pHelper->setTitleHeight(m_d->ribbonBar->titleBarHeight()); //设置窗体的标题栏高度
	}
}


const SARibbonBar *SARibbonMainWindow::ribbonBar() const
{
    return (m_d->ribbonBar);
}


SARibbonBar *SARibbonMainWindow::ribbonBar()
{
    return (m_d->ribbonBar);
}


void SARibbonMainWindow::setRibbonTheme(SARibbonMainWindow::RibbonTheme theme)
{
    switch (theme)
    {
    case NormalTheme:
        loadTheme(":/theme/resource/default.qss");
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


void SARibbonMainWindow::loadTheme(const QString& themeFile)
{
    QFile file(themeFile);

    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return;
    }
    setStyleSheet(file.readAll());
}
