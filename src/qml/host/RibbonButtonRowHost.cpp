#include "RibbonButtonRowHost.h"
#include "../button/RibbonToolButton.h"
#include "../metrics/RibbonMetrics.h"

namespace SARibbonQml {

namespace {
constexpr int kRowSpacing = 2;
}  // namespace

RibbonButtonRowHost::RibbonButtonRowHost(QQuickItem* parent) : QQuickItem(parent)
{
    setAcceptedMouseButtons(Qt::NoButton);  // structural container
}

RibbonButtonRowHost::~RibbonButtonRowHost()
{
}

int RibbonButtonRowHost::rowWidth() const
{
    return mRowWidth;
}

void RibbonButtonRowHost::registerButton(RibbonToolButton* btn)
{
    if (btn && !mButtons.contains(btn)) {
        mButtons.append(btn);
        // hint changes re-flow the row (text/icon edits)
        connect(btn, &QQuickItem::implicitWidthChanged, this, [this]() { polish(); });
        connect(btn, &QQuickItem::implicitHeightChanged, this, [this]() { polish(); });
        polish();
    }
}

void RibbonButtonRowHost::unregisterButton(RibbonToolButton* btn)
{
    if (mButtons.removeOne(btn)) {
        disconnect(btn, nullptr, this, nullptr);
        polish();
    }
}

int RibbonButtonRowHost::rowHeight() const
{
    return RibbonMetrics::instance()->titleBarHeight();
}

void RibbonButtonRowHost::itemChange(ItemChange change, const ItemChangeData& data)
{
    if (change == QQuickItem::ItemChildAddedChange) {
        if (RibbonToolButton* btn = qobject_cast< RibbonToolButton* >(data.item)) {
            registerButton(btn);
        }
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonToolButton* btn = qobject_cast< RibbonToolButton* >(data.item)) {
            unregisterButton(btn);
        }
    }
    QQuickItem::itemChange(change, data);
}

void RibbonButtonRowHost::updatePolish()
{
    layoutButtons();
}

void RibbonButtonRowHost::layoutButtons()
{
    // buttons flow horizontally, vertically centered in the row; sizes come
    // from the metrics-derived sizeHints (widgets quick-access/button-group
    // internal row layout parity)
    const int h0 = rowHeight();
    int x = 0;
    for (RibbonToolButton* btn : mButtons) {
        const QSize hint = btn->sizeHint();
        const int h      = qMin(hint.height(), h0);
        btn->setPosition(QPointF(x, (qreal(h0) - h) / 2.0));
        btn->setSize(QSizeF(hint.width(), h));
        x += hint.width() + kRowSpacing;
    }
    const int newWidth = qMax(x - kRowSpacing, 0);
    if (newWidth != mRowWidth) {
        mRowWidth = newWidth;
        setImplicitWidth(qreal(mRowWidth));
        Q_EMIT rowWidthChanged();
    }
}

}
