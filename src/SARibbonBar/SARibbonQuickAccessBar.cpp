#include "SARibbonQuickAccessBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonSeparatorWidget.h"
#include <QGuiApplication>

/**
 * @brief The SARibbonQuickAccessBarPrivate class
 */
class SARibbonQuickAccessBar::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonQuickAccessBar)
public:
    PrivateData(SARibbonQuickAccessBar* p);

public:
    SARibbonButtonGroupWidget* groupWidget { nullptr };
};
SARibbonQuickAccessBar::PrivateData::PrivateData(SARibbonQuickAccessBar* p) : q_ptr(p)
{
}

//===================================================
// SARibbonQuickAccessBar
//===================================================
SARibbonQuickAccessBar::SARibbonQuickAccessBar(QWidget* parent)
    : SARibbonCtrlContainer(parent), d_ptr(new SARibbonQuickAccessBar::PrivateData(this))
{
    d_ptr->groupWidget = new SARibbonButtonGroupWidget(this);
    d_ptr->groupWidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    setContainerWidget(d_ptr->groupWidget);
}

SARibbonQuickAccessBar::~SARibbonQuickAccessBar()
{
}

void SARibbonQuickAccessBar::addSeparator()
{
    d_ptr->groupWidget->addSeparator();
}

void SARibbonQuickAccessBar::addAction(QAction* act)
{
    d_ptr->groupWidget->addAction(act);
}

void SARibbonQuickAccessBar::addWidget(QWidget* w)
{
    d_ptr->groupWidget->addWidget(w);
}

void SARibbonQuickAccessBar::addMenu(QMenu* m, QToolButton::ToolButtonPopupMode popMode)
{
    d_ptr->groupWidget->addMenu(m, popMode);
}

/**
 * @brief 获取内部管理的ButtonGroupWidget
 * @return
 */
SARibbonButtonGroupWidget* SARibbonQuickAccessBar::buttonGroupWidget()
{
    return d_ptr->groupWidget;
}

/**
 * @brief 获取内部管理的ButtonGroupWidget
 * @return
 */
const SARibbonButtonGroupWidget* SARibbonQuickAccessBar::buttonGroupWidget() const
{
    return d_ptr->groupWidget;
}

/**
 * @brief icon 尺寸设置
 * @param s
 */
void SARibbonQuickAccessBar::setIconSize(const QSize& s)
{
    d_ptr->groupWidget->setIconSize(s);
}

/**
 * @brief icon 尺寸
 * @return
 */
QSize SARibbonQuickAccessBar::iconSize() const
{
    return d_ptr->groupWidget->iconSize();
}
