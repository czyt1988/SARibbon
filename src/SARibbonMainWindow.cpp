#include "SARibbonMainWindow.h"
#include "FramelessHelper.h"
#include "SAWindowButtonGroup.h"
#include "SARibbonBar.h"
#include <QApplication>
#include <QDebug>
#include <QHash>

class SARibbonMainWindowPrivate
{
public:
    void init();
    SARibbonBar* ribbonBar;
    QHash<SARibbonMainWindow::RibbonElement,QString> ribbonStyleSheet;
};

void SARibbonMainWindowPrivate::init()
{
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
                     "SARibbonTabBar::tab "
                     "{"
                     " background: transparent;"
                     " margin-top: 0px;"
                     " margin-right: 0px;"
                     " margin-left: 6px;"
                     " margin-bottom: 0px;"
                     " min-width:60px;"
                     " max-width:200px;"
                     " min-height:30px;"
                     " max-height:30px;"
                     ""
                     "}"
                     "SARibbonTabBar::tab:selected, QTabBar::tab:hover "
                     "{ "
                     " border-top-left-radius: 2px;"
                     " border-top-right-radius: 2px;"
                     "}"
                     "SARibbonTabBar::tab:selected{"
                     " border: 1px solid #BAC9DB; "
                     " background: white;"
                     " border-bottom-color: #FFFFFF;"
                     "}"
                     "SARibbonTabBar::tab:hover:!selected"
                     "{"
                     " border: 1px solid #ECBC3D;"
                     "}"
                     "SARibbonTabBar::tab:!selected "
                     "{"
                     " margin-top: 0px;"
                     "}"
                     "");
    ribbonStyleSheet[SARibbonMainWindow::RibbonToolButton]
            = QString(""
                     "SARibbonToolButton::pressed{"
                     " border: 1px solid #FFBF3E;"
                     " background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEDA3, stop:0.1282 #FDD36A,stop:0.8333 #FCD57C, stop:1 #FDFDEB);"
                     "}"
                     "SARibbonToolButton::checked{"
                     " border: 1px solid #f2ca58;"
                     " background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);"
                     "}"
                     "SARibbonToolButton::hover {"
                     " border: 1px solid #f2ca58;"
                     " background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);"
                     " }"
                     "");
    ribbonStyleSheet[SARibbonMainWindow::RibbonControlButton]
            = QString(""
                      "SARibbonControlButton{"
                       " border: none;"
                       "}"
                      "SARibbonControlButton#SARibbonGalleryButtonUp,#SARibbonGalleryButtonDown,#SARibbonGalleryButtonMore{"
                      "  border: 1px solid #C0C2C4;"
                      "}"
                      "SARibbonControlButton#SARibbonBarHidePannelButton{"
                      "  border: none;"
                      "  background-color:none;"
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
                    "SARibbonMenu {  "
                      "    background-color: #FCFCFC;  "
                      "    border: 1px solid #8492A6;  "
                      "}"
                      "SARibbonMenu::item {  "
                      "    padding: 5px 25px 5px 25px;"
                      "    background-color: transparent;  "
                      "}"
                      "SARibbonMenu::item:selected {   "
                      "    background-color: #654321;"
                      "    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FEF9F4, stop:0.38 #FDE0BD,stop:0.39 #FFCE69, stop:1 #FFFFE7);"
                      "}"
                      "SARibbonMenu::item:hover {   "
                      "    border: 1px solid #FFB700;"
                      "}  "
                      "SARibbonMenu::icon{"
                      "margin-left: 5px;"
                      "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonPannelOptionButton]
            = QString(
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
                      " border: 1px solid #C0C2C4;"
                      "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::RibbonGalleryGroup]
            = QString(
                      "SARibbonGalleryGroup {  "
                      " show-decoration-selected: 1;"
                      "}"
                      "SARibbonGalleryGroup::item:selected {  "
                      "    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);  "
                      "}"
                      "SARibbonGalleryGroup::item:hover {  "
                      "    border: 2px solid #FDEEB3;"
                      "}"
                      );
    ribbonStyleSheet[SARibbonMainWindow::Q_ComboBox]
            = QString(
                      "QComboBox {  "
                      " border: 1px solid #C0C2C4;"
                      //" background: white;"
                      "}"
                      "QComboBox:hover{  "
                      "  border: 1px solid #FDEEB3;"
                      "  color : #000;"

                      //" background: white;"
                      "}"
                      "QComboBox:editable {"
                      "  color : #000;"
                      "  background: white;"
                      "}"

                      "QComboBox::drop-down {"
                      "      subcontrol-origin: padding;"
                      "      subcontrol-position: top right;"
                      "      width: 15px;"
                      "      border-left: none;"
                      "      border-top-right-radius: 0px;"
                      "      border-bottom-right-radius: 0px;"
                      "}"
                      "QComboBox::drop-down:hover {"
                      "    border: 1px solid #FDEEB3;"
                      "    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FDEEB3, stop:0.1282 #FDE38A,stop:0.8333 #FCE58C, stop:1 #FDFDEB);  "
                      "}"
                        "QComboBox::down-arrow {"
                        "    image: url(:/image/resource/ArrowDown.png);"
                        "}"
                    "QComboBox QAbstractItemView {"
                    "   border: 1px solid #C0C2C4;"
                    "   selection-color:#000;"
                    "   selection-background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #FEF9F4, stop:0.38 #FDE0BD,stop:0.39 #FFCE69, stop:1 #FFFFE7);"
                    "}"
                      );
}

SARibbonMainWindow::SARibbonMainWindow(QWidget *parent)
    :QMainWindow(parent)
    ,m_d(new SARibbonMainWindowPrivate)
{
    m_d->init();
    this->setStyleSheet("");
    FramelessHelper *pHelper = new FramelessHelper(this);
    pHelper->setTitleHeight(30);  //设置窗体的标题栏高度
    //
    m_d->ribbonBar = new SARibbonBar(this);
    setMenuWidget(m_d->ribbonBar);
    //
    new SAWindowButtonGroup(this);
}

const SARibbonBar *SARibbonMainWindow::ribbonBar() const
{
    return m_d->ribbonBar;
}

SARibbonBar *SARibbonMainWindow::ribbonBar()
{
    return m_d->ribbonBar;
}

void SARibbonMainWindow::setStyleSheet(const QString &styleSheet)
{
    QString s = styleSheet;
    for(auto i = m_d->ribbonStyleSheet.begin();i!=m_d->ribbonStyleSheet.end();++i)
    {
        s += i.value();
    }
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


