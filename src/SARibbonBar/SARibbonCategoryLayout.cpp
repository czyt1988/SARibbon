#include "SARibbonCategoryLayout.h"
#include <QLayoutItem>
#include "SARibbonPannel.h"
#include "SARibbonToolButton.h"
#include "SARibbonElementManager.h"
#include "SARibbonSeparatorWidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

/**
 * @brief SARibbonCategoryLayoutItem，用于标识SARibbonCategoryLayout的item
 */
class SARibbonCategoryLayoutItem : public QWidgetItem
{
public:
    SARibbonCategoryLayoutItem(SARibbonPannel *w);
    SARibbonSeparatorWidget *separatorWidget;
    QRect mWillSetGeometry;                 ///< pannel将要设置的Geometry
    QRect mWillSetSeparatorGeometry;        ///< pannel将要设置的Separator的Geometry
};

class SARibbonCategoryLayoutPrivate {
public:
    SARibbonCategoryLayoutPrivate(SARibbonCategoryLayout *par);
    //计算所有元素的sizehint总宽度
    int totalSizeHintWidth() const;

public:
    SARibbonCategoryLayout *q_d;
    bool mDirty;
    QList<SARibbonCategoryLayoutItem *> mItemList;
    QSize mSizeHint;
    SARibbonCategoryScrollButton *mLeftScrollBtn;   ///< 在区域无法显示时显示的按钮
    SARibbonCategoryScrollButton *mRightScrollBtn;  ///< 在区域无法显示时显示的按钮
    int mTotalWidth;
    int mXBase;
    bool mIsRightScrollBtnShow;     ///< 标记右滚动按钮是否需要显示
    bool mIsLeftScrollBtnShow;      ///< 标记左滚动按钮是否需要显示
};

//=============================================================
//SARibbonCategoryLayoutItem
//=============================================================

SARibbonCategoryLayoutItem::SARibbonCategoryLayoutItem(SARibbonPannel *w) : QWidgetItem(w)
{
    separatorWidget = nullptr;
}


//=============================================================
//SARibbonCategoryLayoutPrivate
//=============================================================

SARibbonCategoryLayoutPrivate::SARibbonCategoryLayoutPrivate(SARibbonCategoryLayout *par)
    : q_d(par)
    , mDirty(true)
    , mSizeHint(50, 50)
    , mLeftScrollBtn(nullptr)
    , mRightScrollBtn(nullptr)
    , mIsRightScrollBtnShow(false)
    , mIsLeftScrollBtnShow(false)
{
}


/**
 * @brief 计算所有元素的SizeHint宽度总和
 * @return
 */
int SARibbonCategoryLayoutPrivate::totalSizeHintWidth() const
{
    int total = 0;
    QMargins mag = q_d->contentsMargins();

    if (!mag.isNull()) {
        total += (mag.left() + mag.right());
    }
    //先计算总长
    for (SARibbonCategoryLayoutItem *item : mItemList)
    {
        if (item->isEmpty()) {
            //如果是hide就直接跳过
            continue;
        }
        QSize pannelSize = item->widget()->sizeHint();
        QSize SeparatorSize(0, 0);
        if (item->separatorWidget) {
            SeparatorSize = item->separatorWidget->sizeHint();
        }
        total += pannelSize.width();
        total += SeparatorSize.width();
    }
    return (total);
}


//=============================================================
//SARibbonCategoryLayout
//=============================================================

SARibbonCategoryLayout::SARibbonCategoryLayout(SARibbonCategory *parent) : QLayout(parent)
    , m_d(new SARibbonCategoryLayoutPrivate(this))
{
    setContentsMargins(1, 1, 1, 1);
    m_d->mLeftScrollBtn = new SARibbonCategoryScrollButton(Qt::LeftArrow, parent);
    m_d->mRightScrollBtn = new SARibbonCategoryScrollButton(Qt::RightArrow, parent);
    m_d->mLeftScrollBtn->setVisible(false);
    m_d->mRightScrollBtn->setVisible(false);
    connect(m_d->mLeftScrollBtn, &QToolButton::clicked, this, &SARibbonCategoryLayout::onLeftScrollButtonClicked);
    connect(m_d->mRightScrollBtn, &QToolButton::clicked, this, &SARibbonCategoryLayout::onRightScrollButtonClicked);
}


SARibbonCategoryLayout::~SARibbonCategoryLayout()
{
    QLayoutItem *item = Q_NULLPTR;

    while ((item = takeAt(0)))
    {
        delete item;
    }
    delete m_d;
}


SARibbonCategory *SARibbonCategoryLayout::ribbonCategory()
{
    return (qobject_cast<SARibbonCategory *>(parentWidget()));
}


void SARibbonCategoryLayout::addItem(QLayoutItem *item)
{
    Q_UNUSED(item);
    qWarning() << tr("in SARibbonCategoryLayout cannot addItem,use addPannel instead");
    invalidate();
}


/**
 * @brief 返回pannel的layout
 * @param index 索引
 * @return
 * @note 注意，pannel是和分割线一起的，但这个只返回一个pannel对应的layout
 */
QLayoutItem *SARibbonCategoryLayout::itemAt(int index) const
{
    SARibbonCategoryLayoutItem *item = m_d->mItemList.value(index, nullptr);

    return (item);
}


/**
 * @brief 提取layout
 * @param index
 * @return
 */
QLayoutItem *SARibbonCategoryLayout::takeAt(int index)
{
    if ((index >= 0) && (index < m_d->mItemList.size())) {
        invalidate();
        SARibbonCategoryLayoutItem *item = m_d->mItemList.takeAt(index);
        if (item->widget()) {
            item->widget()->hide();
        }
        if (item->separatorWidget) {
            item->separatorWidget->hide();
        }
        return (item);
    }
    return (nullptr);
}


int SARibbonCategoryLayout::count() const
{
    return (m_d->mItemList.size());
}


void SARibbonCategoryLayout::addPannel(SARibbonPannel *pannel)
{
    insertPannel(m_d->mItemList.count(), pannel);
}


/**
 * @brief 插入一个pannel
 * @param index 索引
 * @param pannel
 * @note 在SARibbonCategoryLayout的布局中，一个pannel会携带一个分割线
 */
void SARibbonCategoryLayout::insertPannel(int index, SARibbonPannel *pannel)
{
    index = qMax(0, index);
    index = qMin(m_d->mItemList.count(), index);
    SARibbonCategoryLayoutItem *item = new SARibbonCategoryLayoutItem(pannel);

    //分割线
    item->separatorWidget = RibbonSubElementDelegate->createRibbonSeparatorWidget(parentWidget());
    //插入list中
    m_d->mItemList.insert(index, item);
    //标记需要重新计算尺寸
    invalidate();
}


QSize SARibbonCategoryLayout::sizeHint() const
{
    return (m_d->mSizeHint);
}


QSize SARibbonCategoryLayout::minimumSize() const
{
    return (m_d->mSizeHint);
}


/**
 * @brief SARibbonCategory充满整个stacked widget
 * @return
 */
Qt::Orientations SARibbonCategoryLayout::expandingDirections() const
{
    return (Qt::Horizontal | Qt::Vertical);
}


void SARibbonCategoryLayout::invalidate()
{
    m_d->mDirty = true;
    QLayout::invalidate();
}


/**
 * @brief 更新尺寸
 */
void SARibbonCategoryLayout::updateGeometryArr()
{
    SARibbonCategory *category = qobject_cast<SARibbonCategory *>(parentWidget());

    int categoryWidth = category->width();
    QMargins mag = contentsMargins();
    int height = category->height();
    int y = 0;

    if (!mag.isNull()) {
        y = mag.top();
        height -= (mag.top() + mag.bottom());
        categoryWidth -= (mag.right() + mag.left());
    }
    //total 是总宽，不是x坐标系，x才是坐标系
    int total = m_d->totalSizeHintWidth();
    //记录可以扩展的数量
    int canExpandingCount = 0;
    //扩展的宽度
    int expandWidth = 0;

    //如果total < categoryWidth,m_d->mXBase可以设置为0
    //判断是否超过总长度
    if (total > categoryWidth) {
        //超过总长度，需要显示滚动按钮
        if (0 == m_d->mXBase) {
            //已经移动到最左，需要可以向右移动
            m_d->mIsRightScrollBtnShow = true;
            m_d->mIsLeftScrollBtnShow = false;
        }else if (m_d->mXBase <= (categoryWidth - total)) {
            //已经移动到最右，需要可以向左移动
            m_d->mIsRightScrollBtnShow = false;
            m_d->mIsLeftScrollBtnShow = true;
        }else {
            //移动到中间两边都可以动
            m_d->mIsRightScrollBtnShow = true;
            m_d->mIsLeftScrollBtnShow = true;
        }
    }else{
        //说明total 小于 categoryWidth
        m_d->mIsRightScrollBtnShow = false;
        m_d->mIsLeftScrollBtnShow = false;
        //这个是避免一开始totalWidth > categorySize.width()，通过滚动按钮调整了m_d->mBaseX
        //随之调整了窗体尺寸，调整后totalWidth < categorySize.width()导致category在原来位置
        //无法显示，必须这里把mBaseX设置为0
        m_d->mXBase = 0;
        //

        for (SARibbonCategoryLayoutItem *item : m_d->mItemList)
        {
            if (SARibbonPannel *p = qobject_cast<SARibbonPannel *>(item->widget())) {
                if (p->isExpanding()) {
                    //pannel可扩展
                    ++canExpandingCount;
                }
            }
        }
        //计算可扩展的宽度
        expandWidth = (categoryWidth - total) / canExpandingCount;
    }
    total = 0;// total重新计算
    int x = m_d->mXBase;

    //先按照sizeHint设置所有的尺寸
    for (SARibbonCategoryLayoutItem *item : m_d->mItemList)
    {
        if (item->isEmpty()) {
            //如果是hide就直接跳过
            item->mWillSetGeometry = QRect(0, 0, 0, 0);
            item->mWillSetSeparatorGeometry = QRect(0, 0, 0, 0);
            continue;
        }
        SARibbonPannel *p = qobject_cast<SARibbonPannel *>(item->widget());
        if (nullptr == p) {
            qDebug() << "unknow widget in SARibbonCategoryLayout";
            continue;
        }
        QSize pannelSize = p->sizeHint();
        QSize SeparatorSize(0, 0);
        if (item->separatorWidget) {
            SeparatorSize = item->separatorWidget->sizeHint();
        }
        if (p->isExpanding()) {
            //可扩展，就把pannel扩展到最大
            pannelSize.setWidth(pannelSize.width()+expandWidth);
        }
        int w = pannelSize.width();
        item->mWillSetGeometry = QRect(x, y, w, height);
        x += w;
        total += w;
        w = SeparatorSize.width();
        item->mWillSetSeparatorGeometry = QRect(x, y, w, height);
        x += w;
        total += w;
    }
    m_d->mTotalWidth = total;
    QWidget *cp = category->parentWidget();
    int parentHeight = (nullptr == cp) ? height : cp->height();
    int parentWidth = (nullptr == cp) ? total : cp->width();

    m_d->mSizeHint = QSize(parentWidth, parentHeight);
}


/**
 * @brief 执行layout调整
 */
void SARibbonCategoryLayout::doLayout()
{
    if (m_d->mDirty) {
        updateGeometryArr();
    }
    SARibbonCategory *category = qobject_cast<SARibbonCategory *>(parentWidget());

    //两个滚动按钮的位置永远不变
    m_d->mLeftScrollBtn->setGeometry(0, 0, 12, category->height());
    m_d->mRightScrollBtn->setGeometry(category->width()-12, 0, 12, category->height());
    QList<QWidget *> showWidgets, hideWidgets;

    for (SARibbonCategoryLayoutItem *item:m_d->mItemList)
    {
        if (item->isEmpty()) {
            hideWidgets << item->widget();
            if (item->separatorWidget) {
                hideWidgets << item->separatorWidget;
            }
        }else{
            item->setGeometry(item->mWillSetGeometry);
            showWidgets << item->widget();
            if (item->separatorWidget) {
                item->separatorWidget->setGeometry(item->mWillSetSeparatorGeometry);
                showWidgets << item->separatorWidget;
            }
        }
    }


    m_d->mRightScrollBtn->setVisible(m_d->mIsRightScrollBtnShow);
    m_d->mLeftScrollBtn->setVisible(m_d->mIsLeftScrollBtnShow);
    if (m_d->mIsRightScrollBtnShow) {
        m_d->mRightScrollBtn->raise();
    }
    if (m_d->mIsLeftScrollBtnShow) {
        m_d->mLeftScrollBtn->raise();
    }
    // 不在上面那里进行show和hide因为这会触发SARibbonPannelLayout的重绘，导致循环绘制，非常影响效率
    for (QWidget *w : showWidgets)
    {
        w->show();
    }
    for (QWidget *w : hideWidgets)
    {
        w->hide();
    }
}


void SARibbonCategoryLayout::onLeftScrollButtonClicked()
{
    SARibbonCategory *category = qobject_cast<SARibbonCategory *>(parentWidget());
    int width = category->width();
    //求总宽
    int totalWidth = m_d->mTotalWidth;

    if (totalWidth > width) {
        int tmp = m_d->mXBase + width;
        if (tmp > 0) {
            tmp = 0;
        }
        m_d->mXBase = tmp;
    }else{
        m_d->mXBase = 0;
    }
    category->updateGeometry();
}


void SARibbonCategoryLayout::onRightScrollButtonClicked()
{
    SARibbonCategory *category = qobject_cast<SARibbonCategory *>(parentWidget());
    int width = category->width();
    //求总宽
    int totalWidth = m_d->mTotalWidth;

    if (totalWidth > width) {
        int tmp = m_d->mXBase - width;
        if (tmp < (width - totalWidth)) {
            tmp = width - totalWidth;
        }
        m_d->mXBase = tmp;
    }else {
        m_d->mXBase = 0;
    }
    category->updateGeometry();
}


void SARibbonCategoryLayout::setGeometry(const QRect& rect)
{
    m_d->mDirty = false;
    updateGeometryArr();
    QLayout::setGeometry(rect);
    doLayout();
}
