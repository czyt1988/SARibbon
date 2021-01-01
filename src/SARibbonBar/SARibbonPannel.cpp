#include "SARibbonPannel.h"
#include "SARibbonToolButton.h"
#include <QApplication>
#include <QResizeEvent>
#include <QAction>
#include <QIcon>
#include <QDebug>
#include <QGridLayout>
#include <QFontMetrics>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include "SARibbonPannelOptionButton.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonGallery.h"
#include "SARibbonElementManager.h"
#include "SARibbonMenu.h"


#define HELP_DRAW_RECT(p, rect)		 \
    do{				 \
        p.save();		 \
        QPen _pen(Qt::DashLine); \
        _pen.setColor(Qt::blue); \
        p.setPen(_pen);		 \
        p.setBrush(QBrush());	 \
        p.drawRect(rect);	 \
        p.restore();		 \
    }while(0)

const int c_higherModehight = 98;
const int c_lowerModehight = 72;
const int c_iconHighForHigerLarge = 32;
const QSize c_iconSizeForHigerLarge = QSize(c_iconHighForHigerLarge, c_iconHighForHigerLarge);
const int c_iconHighForHigerSmall = 16;
const QSize c_iconSizeForHigerSmall = QSize(c_iconHighForHigerSmall, c_iconHighForHigerSmall);

class SARibbonPannelPrivate
{
public:
    SARibbonPannelPrivate(SARibbonPannel *p);
    //根据m_pannelLayoutMode返回gridLayout应该增加的行数
    int rowadded();
    void createLayout();
    void recalcTitleY();

    SARibbonPannel *Parent;
    QGridLayout *m_gridLayout;
    QPoint m_nextElementPosition;
    int m_row;                                              ///< 记录小action所在的gridLayout行数，gridLayout总共划分为6行，用于满足3行或2行的按钮需求
    SARibbonPannelOptionButton *m_optionActionButton;
    int m_titleY;                                           ///< 标题栏的y距离
    SARibbonToolButton *m_defaultReduceButton;              ///<在pannel无法显示全的时候，显示一个toolbutton用来弹出pannel
    SARibbonPannel::PannelLayoutMode m_pannelLayoutMode;    ///< pannel的布局模式，默认为3行模式ThreeRowMode
};

SARibbonPannelPrivate::SARibbonPannelPrivate(SARibbonPannel *p)
    : Parent(p)
    , m_gridLayout(nullptr)
    , m_nextElementPosition(3, 3)
    , m_row(0)
    , m_optionActionButton(nullptr)
    , m_titleY(77)
    , m_defaultReduceButton(nullptr)
    , m_pannelLayoutMode(SARibbonPannel::ThreeRowMode)
{
    createLayout();
    recalcTitleY();
}


int SARibbonPannelPrivate::rowadded()
{
    switch (m_pannelLayoutMode)
    {
    case SARibbonPannel::ThreeRowMode:
        return (2);

    case SARibbonPannel::TwoRowMode:
        return (3);

    default:
        break;
    }
    return (2);
}


void SARibbonPannelPrivate::createLayout()
{
    if (m_gridLayout) {
        delete m_gridLayout;
    }
    m_gridLayout = new QGridLayout(Parent);
    m_gridLayout->setSpacing(1);
    if (SARibbonPannel::TwoRowMode == m_pannelLayoutMode) {
        m_gridLayout->setContentsMargins(3, 2, 3, 2);
    }else{
        m_gridLayout->setContentsMargins(3, 2, 3, RibbonSubElementStyleOpt.pannelTitleHeight);
    }
}


void SARibbonPannelPrivate::recalcTitleY()
{
    if (SARibbonPannel::ThreeRowMode == m_pannelLayoutMode) {
        m_titleY = Parent->height()-RibbonSubElementStyleOpt.pannelTitleHeight;
    }else {
        m_titleY = Parent->height()*0.94;
    }
}


SARibbonPannel::SARibbonPannel(QWidget *parent) : QWidget(parent)
    , m_d(new SARibbonPannelPrivate(this))
{
    setPannelLayoutMode(ThreeRowMode);
}


SARibbonPannel::~SARibbonPannel()
{
    delete m_d;
}


SARibbonToolButton *SARibbonPannel::addLargeAction(QAction *action)
{
    SARibbonToolButton *btn = RibbonSubElementDelegate->createRibbonToolButton(this);

    btn->setButtonType(SARibbonToolButton::LargeButton);
    btn->setAutoRaise(true);
    btn->setDefaultAction(action);
    QSize iconSize = maxHightIconSize(action->icon().actualSize(c_iconSizeForHigerLarge), c_iconHighForHigerLarge);

    btn->setIconSize(iconSize);
    if (action->menu()) {
        btn->setPopupMode(QToolButton::MenuButtonPopup);
    }
    m_d->m_gridLayout->addWidget(btn, 0, m_d->m_gridLayout->columnCount(), 6, 1);
    m_d->m_row = 0;
    addAction(action);
    return (btn);
}


SARibbonToolButton *SARibbonPannel::addLargeToolButton(const QString& text, const QIcon& icon, QToolButton::ToolButtonPopupMode popMode)
{
    SARibbonToolButton *btn = RibbonSubElementDelegate->createRibbonToolButton(this);

    btn->setButtonType(SARibbonToolButton::LargeButton);
    btn->setAutoRaise(true);
    QSize iconSize = maxHightIconSize(icon.actualSize(c_iconSizeForHigerLarge), c_iconHighForHigerLarge);

    btn->setIconSize(iconSize);
    btn->setIcon(icon);
    btn->setPopupMode(popMode);
    btn->setText(text);
    m_d->m_gridLayout->addWidget(btn, 0, m_d->m_gridLayout->columnCount(), 6, 1);
    m_d->m_row = 0;
    return (btn);
}


SARibbonToolButton *SARibbonPannel::addSmallToolButton(const QString& text, const QIcon& icon, QToolButton::ToolButtonPopupMode popMode)
{
    SARibbonToolButton *btn = RibbonSubElementDelegate->createRibbonToolButton(this);

    btn->setButtonType(SARibbonToolButton::SmallButton);
    btn->setAutoRaise(true);
    QSize iconSize = maxHightIconSize(icon.actualSize(c_iconSizeForHigerSmall), c_iconHighForHigerSmall);

    btn->setIconSize(iconSize);
    btn->setIcon(icon);
    btn->setPopupMode(popMode);
    btn->setText(text);
    //
    if (0 == m_d->m_row) {
        m_d->m_gridLayout->addWidget(btn, m_d->m_row, m_d->m_gridLayout->columnCount(), 2, 1);
    }else{
        m_d->m_gridLayout->addWidget(btn, m_d->m_row, m_d->m_gridLayout->columnCount()-1, 2, 1);
    }
    m_d->m_row += (m_d->rowadded());
    if (m_d->m_row >= 5) {//
        m_d->m_row = 0;
    }
    return (btn);
}


SARibbonToolButton *SARibbonPannel::addSmallAction(QAction *action)
{
    SARibbonToolButton *btn = RibbonSubElementDelegate->createRibbonToolButton(this);

    btn->setButtonType(SARibbonToolButton::SmallButton);
    btn->setAutoRaise(true);
    btn->setDefaultAction(action);
    QSize iconSize = maxHightIconSize(action->icon().actualSize(c_iconSizeForHigerSmall), c_iconHighForHigerSmall);

    btn->setIconSize(iconSize);
    if (action->menu()) {
        btn->setPopupMode(QToolButton::MenuButtonPopup);
    }
    if (0 == m_d->m_row) {
        m_d->m_gridLayout->addWidget(btn, m_d->m_row, m_d->m_gridLayout->columnCount(), 2, 1);
    }else{
        m_d->m_gridLayout->addWidget(btn, m_d->m_row, m_d->m_gridLayout->columnCount()-1, 2, 1);
    }
    m_d->m_row += (m_d->rowadded());
    if (m_d->m_row >= 5) {
        m_d->m_row = 0;
    }
    addAction(action);
    return (btn);
}


/**
 * @brief 在三栏模式下，强制加为2栏action
 *
 * 主要应用在ThreeRowMode下
 * @param action
 * @return
 */
SARibbonToolButton *SARibbonPannel::addMediumAction(QAction *action)
{
    SARibbonToolButton *btn = RibbonSubElementDelegate->createRibbonToolButton(this);

    btn->setButtonType(SARibbonToolButton::SmallButton);
    btn->setAutoRaise(true);
    btn->setDefaultAction(action);
    QSize iconSize = maxHightIconSize(action->icon().actualSize(c_iconSizeForHigerSmall), c_iconHighForHigerSmall);

    btn->setIconSize(iconSize);
    if (action->menu()) {
        btn->setPopupMode(QToolButton::MenuButtonPopup);
    }
    if (0 == m_d->m_row) {
        m_d->m_gridLayout->addWidget(btn, m_d->m_row, m_d->m_gridLayout->columnCount(), 3, 1);
        m_d->m_row = 3;
    }else if (3 == m_d->m_row) {
        m_d->m_gridLayout->addWidget(btn, m_d->m_row, m_d->m_gridLayout->columnCount()-1, 3, 1);
        m_d->m_row += 3;
    }else {
        m_d->m_gridLayout->addWidget(btn, m_d->m_row, m_d->m_gridLayout->columnCount(), 3, 1);
        m_d->m_row = 0;
    }

    addAction(action);
    return (btn);
}


SARibbonToolButton *SARibbonPannel::addLargeMenu(SARibbonMenu *menu)
{
    SARibbonToolButton *btn = RibbonSubElementDelegate->createRibbonToolButton(this);

    btn->setButtonType(SARibbonToolButton::LargeButton);
    btn->setAutoRaise(true);
    if (!menu->icon().isNull()) {
        QSize iconSize = maxHightIconSize(menu->icon().actualSize(c_iconSizeForHigerLarge), c_iconHighForHigerLarge);
        btn->setIconSize(iconSize);
        btn->setIcon(menu->icon());
    }
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setText(menu->title());
    btn->setMenu(menu);
    m_d->m_gridLayout->addWidget(btn, 0, m_d->m_gridLayout->columnCount(), 6, 1);
    m_d->m_row = 0;
    return (btn);
}


SARibbonToolButton *SARibbonPannel::addSmallMenu(SARibbonMenu *menu)
{
    SARibbonToolButton *btn = RibbonSubElementDelegate->createRibbonToolButton(this);

    btn->setButtonType(SARibbonToolButton::SmallButton);
    QSize iconSize = maxHightIconSize(menu->icon().actualSize(c_iconSizeForHigerSmall), c_iconHighForHigerSmall);

    btn->setIconSize(iconSize);
    btn->setMenu(menu);
    btn->setPopupMode(QToolButton::InstantPopup);
    if (0 == m_d->m_row) {
        m_d->m_gridLayout->addWidget(btn, m_d->m_row, m_d->m_gridLayout->columnCount(), 2, 1);
    }else{
        m_d->m_gridLayout->addWidget(btn, m_d->m_row, m_d->m_gridLayout->columnCount()-1, 2, 1);
    }
    m_d->m_row += (m_d->rowadded());
    if (m_d->m_row >= 5) {
        m_d->m_row = 0;
    }
    return (btn);
}


SARibbonToolButton *SARibbonPannel::addLargeActionMenu(QAction *action, SARibbonMenu *menu)
{
    SARibbonToolButton *btn = addLargeAction(action);

    btn->setMenu(menu);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    return (btn);
}


SARibbonGallery *SARibbonPannel::addGallery()
{
    SARibbonGallery *gallery = RibbonSubElementDelegate->createRibbonGallery(this);

    m_d->m_gridLayout->addWidget(gallery, 0, m_d->m_gridLayout->columnCount(), 6, 1);
    m_d->m_row = 0;
    setExpanding();
    return (gallery);
}


void SARibbonPannel::setPannelLayoutMode(SARibbonPannel::PannelLayoutMode mode)
{
    if (m_d->m_pannelLayoutMode == mode) {
        return;
    }
    //PannelLayoutMode old = m_d->m_pannelLayoutMode;

    m_d->m_pannelLayoutMode = mode;
    int high = c_higherModehight;

    switch (mode)
    {
    case ThreeRowMode:
        high = c_higherModehight;
        break;

    case TwoRowMode:
        high = c_lowerModehight;
        break;

    default:
        high = c_higherModehight;
        break;
    }
    setFixedHeight(high);
    m_d->recalcTitleY();
    setMinimumWidth(50);
    resetLayout(mode);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    //QApplication::postEvent(this, new QResizeEvent(newSize, oldSize));
}


SARibbonPannel::PannelLayoutMode SARibbonPannel::pannelLayoutMode() const
{
    return (m_d->m_pannelLayoutMode);
}


void SARibbonPannel::addSeparator()
{
#if 0
    QAction *action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
#else
    SARibbonSeparatorWidget *sep = RibbonSubElementDelegate->createRibbonSeparatorWidget(height() - 10, this);
    m_d->m_gridLayout->addWidget(sep, 0, m_d->m_gridLayout->columnCount(), 6, 1);
    m_d->m_row = 0;
#endif
}


///
/// \brief 向pannel添加窗口，此窗口的所有权归SARibbonPannel所有
/// \param w
///
void SARibbonPannel::addSmallWidget(QWidget *w)
{
    if (w->parentWidget() != this) {
        w->setParent(this);
    }
    int col = m_d->m_gridLayout->columnCount();

    if (0 != m_d->m_row) {
        //如果没占满就在当前列里进行操作，col-1就是当前列，如果占满就新一列
        col -= 1;
    }
    m_d->m_gridLayout->addWidget(w, m_d->m_row, col, 2, 1);
    m_d->m_row += (m_d->rowadded());
    if (m_d->m_row >= 5) {
        m_d->m_row = 0;
    }
}


void SARibbonPannel::addLargeWidget(QWidget *w)
{
    if (w->parentWidget() != this) {
        w->setParent(this);
    }
    int col = m_d->m_gridLayout->columnCount();

    m_d->m_gridLayout->addWidget(w, 0, col, 6, 1);
    m_d->m_row = 0;
}


/**
 * @brief SARibbonPannel::addWidget
 * @param w
 * @param row gridlayout的行数，pannel的gridllayout一共有6行用于分割
 * @param rowSpan 窗口所占的行数，类似QGridLayout的rowSpan
 */
void SARibbonPannel::addWidget(QWidget *w, int row, int rowSpan)
{
    if (this != w->parentWidget()) {
        w->setParent(this);
    }
    int col = m_d->m_gridLayout->columnCount();

    if ((0 != row) && (0 != col)) {
        col -= 1;
    }
    m_d->m_gridLayout->addWidget(w, row, col, rowSpan, 1);
    m_d->m_row = row + rowSpan;
    if (m_d->m_row >= 5) {
        m_d->m_row = 0;
    }
}


/**
 * @brief SARibbonPannel::addWidget
 * @param w
 * @param row gridlayout的行数，pannel的gridllayout一共有6行用于分割
 * @param rowSpan 窗口所占的行数，类似QGridLayout的rowSpan
 * @param column 列位置，使用前先用 @sa gridLayoutColumnCount 返回当前的列数
 * @param columnSpan 窗口所占的列数，类似QGridLayout的columnSpan
 */
void SARibbonPannel::addWidget(QWidget *w, int row, int rowSpan, int column, int columnSpan)
{
    w->setParent(this);
    m_d->m_gridLayout->addWidget(w, row, column, rowSpan, columnSpan);
    m_d->m_row = row + rowSpan;
    if (row >= 5) {
        m_d->m_row = 0;
    }
}


int SARibbonPannel::gridLayoutColumnCount() const
{
    return (m_d->m_gridLayout->columnCount());
}


void SARibbonPannel::addOptionAction(QAction *action)
{
    if (nullptr == action) {
        if (m_d->m_optionActionButton) {
            delete m_d->m_optionActionButton;
            m_d->m_optionActionButton = nullptr;
        }
        return;
    }
    if (nullptr == m_d->m_optionActionButton) {
        m_d->m_optionActionButton = new SARibbonPannelOptionButton(this);
    }
    m_d->m_optionActionButton->connectAction(action);
    repaint();
}


QSize SARibbonPannel::maxHightIconSize(const QSize& size, int height)
{
    if (size.height() < height) {
        return (size * ((float)height/size.height()));
    }
    return (size);
}


void SARibbonPannel::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

#ifdef SA_RIBBON_DEBUG_HELP_DRAW
    HELP_DRAW_RECT(p, rect());
#endif
    if (ThreeRowMode == pannelLayoutMode()) {
        QFont f = font();
        f.setPixelSize(RibbonSubElementStyleOpt.pannelTitleHeight * 0.6);
        p.setFont(f);
        if (m_d->m_optionActionButton) {
            p.drawText(1, height()-RibbonSubElementStyleOpt.pannelTitleHeight
                , width()-m_d->m_optionActionButton->width() - RibbonSubElementStyleOpt.pannelTitleOptionButtonSpace
                , RibbonSubElementStyleOpt.pannelTitleHeight
                , Qt::AlignCenter
                , windowTitle());
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            QRect r = QRect(1, height()-RibbonSubElementStyleOpt.pannelTitleHeight
                , width()-m_d->m_optionActionButton->width() - RibbonSubElementStyleOpt.pannelTitleOptionButtonSpace
                , RibbonSubElementStyleOpt.pannelTitleHeight-2);
            qDebug() << r;
            HELP_DRAW_RECT(p, r);
#endif
        }else {
            p.drawText(1, height()-RibbonSubElementStyleOpt.pannelTitleHeight
                , width()
                , RibbonSubElementStyleOpt.pannelTitleHeight
                , Qt::AlignCenter
                , windowTitle());
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            QRect r = QRect(1, height()-RibbonSubElementStyleOpt.pannelTitleHeight
                , width()
                , RibbonSubElementStyleOpt.pannelTitleHeight);
            qDebug() << r;
            HELP_DRAW_RECT(p, r);
#endif
        }
    }

    QWidget::paintEvent(event);
}


QSize SARibbonPannel::sizeHint() const
{
    QSize laySize = layout()->sizeHint();
    QFontMetrics fm = fontMetrics();
    QSize titleSize = fm.size(Qt::TextShowMnemonic, windowTitle());
    int maxWidth = laySize.width();

    if (m_d->m_defaultReduceButton) {
        maxWidth = qMax(laySize.width(), titleSize.width()) + RibbonSubElementStyleOpt.pannelTitleOptionButtonSpace + m_d->m_defaultReduceButton->width();
    }else {
        maxWidth = qMax(laySize.width(), titleSize.width());
    }
    return (QSize(maxWidth, laySize.height()));
}


QSize SARibbonPannel::minimumSizeHint() const
{
    return (layout()->minimumSize());
}


void SARibbonPannel::setReduce(bool isReduce)
{
    if (isReduce) {
        setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    }else {
        setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    }
}


void SARibbonPannel::setExpanding(bool isExpanding)
{
    setSizePolicy(isExpanding ? QSizePolicy::Expanding : QSizePolicy::Preferred
        , QSizePolicy::Fixed);
}


bool SARibbonPannel::isExpanding() const
{
    QSizePolicy sp = sizePolicy();

    return (sp.horizontalPolicy() == QSizePolicy::Expanding);
}


void SARibbonPannel::resetLayout(PannelLayoutMode newmode)
{
    QGridLayout *layout = m_d->m_gridLayout;
    int c = layout->count();
    struct _tmp_layoutinfo {
        QWidget *	w;
        int		index;
        int		row;
        int		column;
        int		rowSpan;
        int		columnSpan;
    };
    QList<_tmp_layoutinfo> oldlayoutinfo;

    for (int i = 0; i < c; ++i)
    {
        _tmp_layoutinfo s;
        s.w = layout->itemAt(i)->widget();
        s.index = i;
        //获取当前layout的信息
        layout->getItemPosition(i, &s.row, &s.column, &s.rowSpan, &s.columnSpan);
        oldlayoutinfo.append(s);
    }
    //删除布局的所有内容
    m_d->createLayout();
    layout = m_d->m_gridLayout;
    //重新布局
    qDebug() << layout->count();
    if (newmode == ThreeRowMode) {
        //从2行变到3行
        for (_tmp_layoutinfo& info : oldlayoutinfo)
        {
            if (6 == info.rowSpan) {
                //说明占满行
                addLargeWidget(info.w);
                if (qobject_cast<SARibbonToolButton *>(info.w)) {
                    info.w->setFixedHeight(78);
                }
            }else {
                //原来是2行布局，正常add到pannel
                addSmallWidget(info.w);
                if (qobject_cast<SARibbonToolButton *>(info.w)) {
                    info.w->setFixedHeight(24);
                }
            }

            _tmp_layoutinfo ttt;
            layout->getItemPosition(info.index, &ttt.row, &ttt.column, &ttt.rowSpan, &ttt.columnSpan);
            qDebug()	<< "info[" << info.index
                    << "] r:" << info.row
                    << " c:" << info.column
                    << " rspan:" << info.rowSpan
                    << " colspan:"<<info.columnSpan
                    << " --> r:" << ttt.row
                    << " c:" << ttt.column
                    << " rspan:" << ttt.rowSpan
                    << " colspan:"<<ttt.columnSpan;
        }
    }else {
        //从3行变到2行
        for (_tmp_layoutinfo& info : oldlayoutinfo)
        {
            if (6 == info.rowSpan) {
                //说明占满行
                addLargeWidget(info.w);
                //两行的最高不超过64
                if (qobject_cast<SARibbonToolButton *>(info.w)) {
                    info.w->setFixedHeight(64);
                }
            }else {
                //原来是3行布局，正常add到pannel
                addSmallWidget(info.w);
                if (qobject_cast<SARibbonToolButton *>(info.w)) {
                    info.w->setFixedHeight(28);
                }
            }
            _tmp_layoutinfo ttt;
            layout->getItemPosition(info.index, &ttt.row, &ttt.column, &ttt.rowSpan, &ttt.columnSpan);
            qDebug()	<< "info[" << info.index
                    << "] r:" << info.row
                    << " c:" << info.column
                    << " rspan:" << info.rowSpan
                    << " colspan:"<<info.columnSpan
                    << " --> r:" << ttt.row
                    << " c:" << ttt.column
                    << " rspan:" << ttt.rowSpan
                    << " colspan:"<<ttt.columnSpan;
        }
    }
}


void SARibbonPannel::resizeEvent(QResizeEvent *event)
{
    if (m_d->m_optionActionButton) {
        m_d->m_optionActionButton->move(width()-RibbonSubElementStyleOpt.pannelTitleOptionButtonSpace/2 - m_d->m_optionActionButton->width()
            , height()-RibbonSubElementStyleOpt.pannelTitleHeight
            +(RibbonSubElementStyleOpt.pannelTitleHeight-m_d->m_optionActionButton->height())/2);
    }
    return (QWidget::resizeEvent(event));
}
