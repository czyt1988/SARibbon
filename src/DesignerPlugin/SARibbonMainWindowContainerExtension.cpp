#include "SARibbonMainWindowContainerExtension.h"
#include "SARibbonMainWindow.h"
#include <QStatusBar>
#include <QDockWidget>
#include <QLayout>
#include <QToolBar>
#include <QMenuBar>
#include "SARibbonBar.h"
#include "SARibbonPluginDebugHelper.h"
using namespace SA_PLUGIN;
SARibbonMainWindowContainerExtension::SARibbonMainWindowContainerExtension(SARibbonMainWindow *w, QObject *p)
    : QObject(p)
    , m_mainwindow(w)
{
    m_mainwindow->setObjectName(QLatin1String("mainwindow"));
    m_mainwindow->setWindowTitle(tr("MainWindow"));
}


/**
 * @brief 参考QtnRibbon
 * @param widget
 */
void SARibbonMainWindowContainerExtension::addWidget(QWidget *widget)
{
    if (widget) {
        SA_PLUGIN_LOG("widget class name:%s", widget->metaObject()->className());
    }else{
        SA_PLUGIN_LOG("widget nullptr");
    }
    m_widgets.removeAll(widget);
    if (QToolBar *toolBar = qobject_cast<QToolBar *>(widget)) {
        m_widgets.append(widget);
        const QMainWindow *mw = qobject_cast<const QMainWindow *>(widget->parentWidget());
        if (!mw || !mw->layout() || (mw->layout()->indexOf(widget) == -1)) {
            m_mainwindow->addToolBar(Qt::TopToolBarArea, toolBar);
        }else{
            Qt::ToolBarArea area = Qt::TopToolBarArea;
            area = mw->toolBarArea(toolBar);
            m_mainwindow->addToolBar(area, toolBar);
            if (mw->toolBarBreak(toolBar)) {
                m_mainwindow->insertToolBarBreak(toolBar);
            }
        }
        toolBar->show();
    }else if (SARibbonBar *ribbonBar = qobject_cast<SARibbonBar *>(widget)) {
        SA_PLUGIN_LOG("begin add ribbon");
//        if (m_mainwindow->isUseRibbon()) {
//            SA_PLUGIN_LOG("isUseRibbon");
//            return;
//        }
        m_mainwindow->setMenuWidget(ribbonBar);
        if (!m_widgets.contains(widget)) {
            m_widgets.append(widget);
        }
        SA_PLUGIN_LOG("end add ribbon");
        ribbonBar->show();
        SA_PLUGIN_LOG("ribbonBar show");
    }else if (QMenuBar *menuBar = qobject_cast<QMenuBar *>(widget)) {
        m_mainwindow->setMenuBar(menuBar);
        m_widgets.append(widget);
        menuBar->show();
    }else if (QStatusBar *statusBar = qobject_cast<QStatusBar *>(widget)) {
        //if (statusBar != m_mainWindow->statusBar())
        m_mainwindow->setStatusBar(statusBar);
        if (!m_widgets.contains(widget)) {
            m_widgets.append(widget);
        }
        statusBar->show();
    }else if (QDockWidget *dockWidget = qobject_cast<QDockWidget *>(widget)) {
        m_widgets.append(widget);
        //下面这段判断dockarea参考了QtDesginer 源码
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
    SA_PLUGIN_LOG("remove index:%d", index);
    QWidget *widget = m_widgets.at(index);

    if (QToolBar *toolBar = qobject_cast<QToolBar *>(widget)) {
        m_mainwindow->removeToolBar(toolBar);
    }else if (SARibbonBar *ribbonBar = qobject_cast<SARibbonBar *>(widget)) {
        ribbonBar->hide();
        ribbonBar->setParent(nullptr);
        m_mainwindow->setMenuWidget(nullptr);
    }else if (QMenuBar *menuBar = qobject_cast<QMenuBar *>(widget)) {
        menuBar->hide();
        menuBar->setParent(nullptr);
        m_mainwindow->setMenuBar(nullptr);
    }else if (QStatusBar *statusBar = qobject_cast<QStatusBar *>(widget)) {
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
