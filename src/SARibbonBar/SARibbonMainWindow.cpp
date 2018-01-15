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
    SARibbonMainWindowPrivate(SARibbonMainWindow* p);
    void init();
    SARibbonMainWindow* Parent;
    SARibbonBar* ribbonBar;
    SARibbonMainWindow::RibbonTheme currentRibbonTheme;
#if 0
    QHash<SARibbonMainWindow::RibbonElement,QString> ribbonStyleSheet;
#endif
};

SARibbonMainWindowPrivate::SARibbonMainWindowPrivate(SARibbonMainWindow *p)
    :Parent(p)
    ,currentRibbonTheme(SARibbonMainWindow::NormalTheme)
{

}

void SARibbonMainWindowPrivate::init()
{
#if 0
    ribbonStyleSheet[SARibbonMainWindow::RibbonBar]
            = QString("SARibbonBar{"
                      " background-color: #E3E6E8;"
                      "}"
                      );

    ribbonStyleSheet[SARibbonMainWindow::RibbonCategory]
            = QString("SARibbonCategory{"
                      "background-color: white;"
                      "}"
                      "SARibbonCategory:focus{outline: none;}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonStackedWidget]
            = QString("SARibbonStackedWidget{"
                      "background-color: white;"
                      "}"
                      "SARibbonStackedWidget:focus{outline: none;}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonApplicationButton]
            = QString("SARibbonApplicationButton{"
                      "color:white;  "
                      "border: 1px solid #416ABD;  "
                      "border-top-left-radius: 2px;  "
                      "border-top-right-radius: 2px;  "
                      "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,   "
                      "        stop:0 #467FBD, stop:0.5 #2A5FAC,stop:0.51 #1A4088, stop:1 #419ACF);"
                      "}"
                      "SARibbonApplicationButton::hover{  "
                      "    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,   "
                      "        stop:0 #7BB2EB, stop:0.5 #477ECD,stop:0.51 #114ECF, stop:1 #80E1FF);  "
                      "}"
                      "SARibbonApplicationButton::pressed{  "
                      "    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,   "
                      "        stop:0 #467BBB, stop:0.5 #2F63AE,stop:0.51 #1C438A, stop:1 #358BC9);  "
                      "}  "
                      "SARibbonApplicationButton:focus{outline: none;}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonTabBar]
            = QString(""
                      "SARibbonTabBar{"
                      " background: transparent;"
                      "}"
                     "SARibbonTabBar::tab "
                     "{"
                     " color:#444444;"
                     " border:none;"
                     " background: transparent;"
                     " margin-top: 0px;"
                     " margin-right: 0px;"
                     " margin-left: 6px;"
                     " margin-bottom: 0px;"
                     " min-width:60px;"
                     " max-width:200px;"
                     " min-height:30px;"
                     " max-height:30px;"
                     " padding-left:1px;"
                     " padding-right:1px;"
                     " padding-top:1px;"
                     " padding-bottom:1px;"
                     "}"
                     "SARibbonTabBar::tab:selected, SARibbonTabBar::tab:hover "
                     "{ "
                     " border-top-left-radius: 2px;"
                     " border-top-right-radius: 2px;"
                     "}"
                     "SARibbonTabBar::tab:selected{"
                     " color:#000000;"
                     " border: 1px solid #BAC9DB; "
                     " background: white;"
                     " border-bottom-color: #FFFFFF;"
                     "}"
                     "SARibbonTabBar::tab:hover:!selected"
                     "{"
                     " border: 1px solid #ECBC3D;"
                     " color: #000000;"
                     "}"
                     "SARibbonTabBar::tab:!selected "
                     "{"
                     " margin-top: 0px;"
                     "}"
                     "");
    ribbonStyleSheet[SARibbonMainWindow::RibbonLineEdit]
            = QString(
                      "SARibbonLineEdit {"
                      " border: 1px solid #C0C2C4;"
                      " background: #FFF;"
                      " selection-background-color: #9BBBF7;"
                      " selection-color: #000;"
                      "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonToolButton]
            = QString(""
                      "SARibbonToolButton{"
                      " border:none;"
                      " color:#444444;"
                      " background-color:transparent;"
                      "}"
                      "SARibbonToolButton::pressed{"
                      " color:#444444;"
                      " border: 1px solid #FFBF3E;"
                      " background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEDA3, stop:0.1282 #FDD36A,stop:0.8333 #FCD57C, stop:1 #FDFDEB);"
                      "}"
                      "SARibbonToolButton::checked{"
                      " color:#444444;"
                      " border: 1px solid #f2ca58;"
                      " background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);"
                      "}"
                      "SARibbonToolButton::hover {"
                      " color:#000000;"
                      " border: 1px solid #f2ca58;"
                      " background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);"
                      "}"
                      "");
    ribbonStyleSheet[SARibbonMainWindow::RibbonControlButton]
            = QString(""
                      "SARibbonControlButton{"
                      "  background-color:transparent;"
                      "  border: none;"
                      "  color:#444444;"
                       "}"
                      "SARibbonControlButton#SARibbonGalleryButtonUp,#SARibbonGalleryButtonDown,#SARibbonGalleryButtonMore{"
                      "  border: 1px solid #C0C2C4;"
                      "}"
                      "SARibbonControlButton#SARibbonBarHidePannelButton{"
                      "  border: none;"
                      "}"
                     "SARibbonControlButton::pressed{"
                     "  border: 1px solid #f2ca58;"
                     "  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);"
                     "}"
                     "SARibbonControlButton::checked{"
                     "  border: 1px solid #f2ca58;"
                     "  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);"
                     "}"
                     "SARibbonControlButton::hover {"
                     "  border: 1px solid #f2ca58;"
                     "  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);"
                     " }"
                     "");
    ribbonStyleSheet[SARibbonMainWindow::RibbonMenu]
            = QString(
                    "SARibbonMenu { "
                    "      color:#444444;"
                    "    background-color: #FCFCFC;  "
                    "    border: 1px solid #8492A6;  "
                    "}"
                    "SARibbonMenu::item {  "
                    "    padding: 5px 25px 5px 25px;"
                    "    background-color: transparent;  "
                    "}"
                    "SARibbonMenu::item:selected {   "
                    "    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FEF9F4, stop:0.38 #FDE0BD,stop:0.39 #FFCE69, stop:1 #FFFFE7);"
                    "}"
                    "SARibbonMenu::item:hover {   "
                    "      color:#000;"
                    "    border: 1px solid #FFB700;"
                    "}  "
                    "SARibbonMenu::icon{"
                    "margin-left: 5px;"
                    "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonPannelOptionButton]
            = QString(""
                      "SARibbonPannelOptionButton{"
                      " background-color:transparent;"
                      " color:#444444;"
                      "}"
                      "SARibbonPannelOptionButton::hover {  "
                      "    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);  "
                      "    border: 0px;"
                      "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonPannel]
            = QString(
                      "SARibbonPannel {  "
                      "    background-color: #FFFFFF;  "
                      "    border: 0px;"
                      "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonGallery]
            = QString(
                      "SARibbonGallery {  "
                      " background-color: transparent;"
                      " color: #444444;"
                      " border: 1px solid #C0C2C4;"
                      "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonGalleryGroup]
            = QString(
                      "SARibbonGalleryGroup {  "
                      " show-decoration-selected: 1;"
                      " background-color: transparent;"
                      " color: #444444;"
                      "}"
                      "SARibbonGalleryGroup::item:selected {  "
                      "    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);  "
                      "}"
                      "SARibbonGalleryGroup::item:hover {  "
                      "    border: 2px solid #FDEEB3;"
                      "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonComboBox]
            = QString(
                      "SARibbonComboBox {  "
                      " border: 1px solid #C0C2C4;"
                      //" background: white;"
                      "}"
                      "SARibbonComboBox:hover{  "
                      "  border: 1px solid #FDEEB3;"
                      "  color : #000;"
                      //" background: white;"
                      "}"
                      "SARibbonComboBox:editable {"
                      "  color : #000;"
                      "  background: white;"
                      "  selection-background-color: #9BBBF7;"
                      "  selection-color: #000;"
                      "}"
                      "SARibbonComboBox::drop-down {"
                      "      subcontrol-origin: padding;"
                      "      subcontrol-position: top right;"
                      "      width: 15px;"
                      "      border-left: none;"
                      "      border-top-right-radius: 0px;"
                      "      border-bottom-right-radius: 0px;"
                      "}"
                      "SARibbonComboBox::drop-down:hover {"
                      "    border: 1px solid #FDEEB3;"
                      "    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);  "
                      "}"
                        "SARibbonComboBox::down-arrow {"
                        "    image: url(:/image/resource/ArrowDown.png);"
                        "}"
//                    "SARibbonComboBox QAbstractItemView {"
//                    "   border: 1px solid #C0C2C4;"
//                    "   selection-color:#000;"
//                    "   selection-background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FEF9F4, stop:0.38 #FDE0BD,stop:0.39 #FFCE69, stop:1 #FFFFE7);"
//                    "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonSeparatorWidget]
            = QString("SARibbonSeparatorWidget{"
                      " background-color: #E3E6E8;"
                      "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonCtrlContainer]
            = QString("SARibbonCtrlContainer{"
                      " background-color: #E3E6E8;"
                      "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonQuickAccessBar]
            = QString("SARibbonQuickAccessBar{"
                      " background-color: #E3E6E8;"
                      "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonButtonGroupWidget]
            = QString("SARibbonButtonGroupWidget{"
                      " background-color: #E3E6E8;"
                      "}"
                      );

    Parent->setStyleSheet("");
#endif
}

SARibbonMainWindow::SARibbonMainWindow(QWidget *parent)
    :QMainWindow(parent)
    ,m_d(new SARibbonMainWindowPrivate(this))
{
    m_d->init();
    setRibbonTheme(ribbonTheme());
    //
    m_d->ribbonBar = new SARibbonBar(this);
    setMenuWidget(m_d->ribbonBar);
    m_d->ribbonBar->installEventFilter(this);
    //
    new SAWindowButtonGroup(this);
    FramelessHelper *pHelper = new FramelessHelper(this);
    pHelper->setTitleHeight(m_d->ribbonBar->titleBarHeight());  //设置窗体的标题栏高度
}

const SARibbonBar *SARibbonMainWindow::ribbonBar() const
{
    return m_d->ribbonBar;
}

SARibbonBar *SARibbonMainWindow::ribbonBar()
{
    return m_d->ribbonBar;
}

void SARibbonMainWindow::setRibbonTheme(SARibbonMainWindow::RibbonTheme theme)
{
    switch(ribbonTheme())
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
    return m_d->currentRibbonTheme;
}

#if 0
void SARibbonMainWindow::setStyleSheet(const QString &styleSheet)
{
    QString s = styleSheet;
    for(auto i = m_d->ribbonStyleSheet.begin();i!=m_d->ribbonStyleSheet.end();++i)
    {
        s += i.value();
    }
    qDebug() << s;
    this->QMainWindow::setStyleSheet(s);
}
QString SARibbonMainWindow::ribbonElementStyleSheet(SARibbonMainWindow::RibbonElement element) const
{
    return m_d->ribbonStyleSheet.value(element);
}

void SARibbonMainWindow::setRibbonElementStyleSheet(SARibbonMainWindow::RibbonElement element, const QString &styleSheet)
{
    m_d->ribbonStyleSheet.insert(element,styleSheet);
}
#endif




void SARibbonMainWindow::resizeEvent(QResizeEvent *event)
{
    if(m_d->ribbonBar)
    {
        if(m_d->ribbonBar->size().width() != this->size().width())
        {
            m_d->ribbonBar->setFixedWidth(this->size().width());
        }
    }
    QMainWindow::resizeEvent(event);
}

bool SARibbonMainWindow::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == m_d->ribbonBar)
    {
        switch (e->type())
        {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::Leave:
        case QEvent::HoverMove:
        case QEvent::MouseButtonDblClick:
            QApplication::sendEvent(this,e);
        default:
            break;
        }
    }
    return QMainWindow::eventFilter(obj,e);
}


void SARibbonMainWindow::loadTheme(const QString& themeFile)
{
    QFile file(themeFile);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return;
    }
    setStyleSheet(file.readAll());
}
