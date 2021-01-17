#include "SARibbonCategoryLayout.h"
#include <QLayoutItem>
#include "SARibbonPannel.h"
#include "SARibbonToolButton.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
class SARibbonReduceActionInfo
{
public:
        QPoint showStartPoint;
        SARibbonPannel *reduceModeShowPannel;
        SARibbonPannel *realShowPannel;
        SARibbonReduceActionInfo()
                : showStartPoint(0, 0)
                , reduceModeShowPannel(nullptr)
                , realShowPannel(nullptr)
        {
        }


        void release()
        {
                realShowPannel = nullptr;
                if (reduceModeShowPannel) {
                        delete reduceModeShowPannel;
                        reduceModeShowPannel = nullptr;
                }
        }
};

SARibbonCategoryLayout::SARibbonCategoryLayout(SARibbonCategory *parent) : QLayout(parent)
{
        setContentsMargins(2, 2, 2, 2);
}


SARibbonCategoryLayout::SARibbonCategoryLayout() : QLayout()
{
        setContentsMargins(2, 2, 2, 2);
}


SARibbonCategoryLayout::~SARibbonCategoryLayout()
{
        QLayoutItem *item = Q_NULLPTR;

        while ((item = takeAt(0)))
        {
                delete item;
        }
}


SARibbonCategory *SARibbonCategoryLayout::ribbonCategory()
{
        return (qobject_cast<SARibbonCategory *>(parent()));
}


void SARibbonCategoryLayout::addItem(QLayoutItem *item)
{
        itemList.append(item);
        invalidate();
}


QLayoutItem *SARibbonCategoryLayout::itemAt(int index) const
{
        return (itemList.value(index, Q_NULLPTR));
}


QLayoutItem *SARibbonCategoryLayout::takeAt(int index)
{
        if ((index >= 0) && (index < itemList.size())) {
                invalidate();
                QLayoutItem *item = itemList.value(index, Q_NULLPTR);
                if (item) {
                        if (item->widget()) {
                                item->widget()->hide();
                        }
                }
                return (itemList.takeAt(index));
        }else {
                return (Q_NULLPTR);
        }
}


int SARibbonCategoryLayout::count() const
{
        return (itemList.size());
}


QSize SARibbonCategoryLayout::sizeHint() const
{
        QSize size;

        // QLayoutItem *item;
        for (int i = 0; i < itemList.size(); ++i)
        {
                size = size.expandedTo(itemList[i]->minimumSize());
        }

        size += QSize(2*margin(), 2*margin());
        return (size);
}


QSize SARibbonCategoryLayout::minimumSize() const
{
        return (QSize(50, 10));
}


Qt::Orientations SARibbonCategoryLayout::expandingDirections() const
{
        return (Qt::Horizontal);
}


void SARibbonCategoryLayout::invalidate()
{
        m_isChanged = true;
        QLayout::invalidate();
}


QPoint SARibbonCategoryLayout::calcPopupPannelPosition(SARibbonCategory *category, SARibbonPannel *pannel, int x)
{
        QPoint absPosition = category->mapToGlobal(QPoint(x, category->height()));
        QRect r = QApplication::desktop()->availableGeometry(category);

        if ((absPosition.x() + pannel->sizeHint().width()) < r.width()) {
                return (absPosition);
        }
        return (QPoint(r.width()-  pannel->sizeHint().width(), absPosition.y()));
}


void SARibbonCategoryLayout::setGeometry(const QRect& rect)
{
}
