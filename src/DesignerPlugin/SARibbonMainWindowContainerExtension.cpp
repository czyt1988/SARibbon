#include "SARibbonMainWindowContainerExtension.h"
#include "SARibbonMainWindow.h"
#include <QStatusBar>
#include <QDockWidget>
#include <QLayout>
using namespace SA_PLUGIN;
SARibbonMainWindowContainerExtension::SARibbonMainWindowContainerExtension(SARibbonMainWindow *w, QObject *p)
    : QObject(p)
    , m_mainwindow(w)
{
}


/**
 * @brief 参考QtnRibbon
 * @param widget
 */
void SARibbonMainWindowContainerExtension::addWidget(QWidget *widget)
{
    m_widgets.removeAll(widget);
    if (QStatusBar *statusBar = qobject_cast<QStatusBar *>(widget)) {
        //if (statusBar != m_mainWindow->statusBar())
        m_mainwindow->setStatusBar(statusBar);
        if (!m_widgets.contains(widget)) {
            m_widgets.append(widget);
        }
        statusBar->show();
    }else if (QDockWidget *dockWidget = qobject_cast<QDockWidget *>(widget)) {
        m_widgets.append(widget);
        //下面这段判断dockarea参考了QtnRibbon
        Qt::DockWidgetArea area = Qt::LeftDockWidgetArea;
        if (const QMainWindow *mw = qobject_cast<const QMainWindow *>(dockWidget->parentWidget())) {
            QList<QLayout *> candidates;
            if (mw->layout()) {
                candidates.append(mw->layout());
                candidates += mw->layout()->findChildren<QLayout *>();
            }
            for (QLayout *l : candidates)
            {
                if (l->indexOf(dockWidget) != -1) {
                    area = (mw->dockWidgetArea(dockWidget));
                }
            }
        }
        m_mainwindow->addDockWidget(area, dockWidget);
        dockWidget->show();
    }else if (widget) {
        m_widgets.prepend(widget);

        if (widget != m_mainwindow->centralWidget()) {
            m_widgets.removeAll(m_mainwindow->centralWidget());

            widget->setParent(m_mainwindow);
            m_mainwindow->setCentralWidget(widget);
        }
    }
}


int SARibbonMainWindowContainerExtension::count() const
{
    return (m_widgets.count());
}


int SARibbonMainWindowContainerExtension::currentIndex() const
{
    return (m_mainwindow->centralWidget() ? 0 : -1);
}


void SARibbonMainWindowContainerExtension::insertWidget(int index, QWidget *widget)
{
    m_widgets.insert(index, widget);
}


void SARibbonMainWindowContainerExtension::remove(int index)
{
    QWidget *widget = m_widgets.at(index);

    if (QStatusBar *statusBar = qobject_cast<QStatusBar *>(widget)) {
        statusBar->hide();
        statusBar->setParent(nullptr);
        m_mainwindow->setStatusBar(nullptr);
    }


    m_widgets.removeAt(index);
}


void SARibbonMainWindowContainerExtension::setCurrentIndex(int index)
{
    Q_UNUSED(index);
}


QWidget *SARibbonMainWindowContainerExtension::widget(int index) const
{
    if ((index < 0) || (index >= m_widgets.size())) {
        return (nullptr);
    }

    return (m_widgets.at(index));
}
