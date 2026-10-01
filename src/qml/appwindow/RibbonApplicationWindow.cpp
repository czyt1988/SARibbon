#include "RibbonApplicationWindow.h"

namespace SARibbonQml {

RibbonApplicationWindow::RibbonApplicationWindow(QQuickItem* parent) : QQuickItem(parent)
{
    setAcceptedMouseButtons(Qt::NoButton);  // structural container
    setVisible(false);                      // shows only through the app button
}

RibbonApplicationWindow::~RibbonApplicationWindow()
{
}

bool RibbonApplicationWindow::isPopupVisible() const
{
    return mPopupVisible;
}

void RibbonApplicationWindow::setPopupVisible(bool on)
{
    if (mPopupVisible == on) {
        return;
    }
    mPopupVisible = on;
    setVisible(on);
    Q_EMIT popupVisibleChanged();
}

void RibbonApplicationWindow::close()
{
    Q_EMIT closeRequested();
}

}
