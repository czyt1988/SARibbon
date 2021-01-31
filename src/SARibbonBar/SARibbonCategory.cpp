#include "SARibbonCategory.h"
#include <QList>
#include <QResizeEvent>
#include <QPainter>
#include <QLinearGradient>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include "SARibbonToolButton.h"
#include <QHBoxLayout>
#include "SARibbonCategoryLayout.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonElementManager.h"

#include <QList>
#include <QMap>

class SARibbonCategoryItem {
public:
    SARibbonCategoryItem()
        : pannelWidget(nullptr)
        , separatorWidget(nullptr)
    {
    }


    bool isEmpty() const
    {
        if (pannelWidget) {
            return (pannelWidget->isHidden());
        }
        return (true);
    }


    bool isNull() const
    {
        return (pannelWidget == nullptr);
    }


public:
    SARibbonPannel *pannelWidget;
    SARibbonSeparatorWidget *separatorWidget;
    QRect mWillSetGeometry;                 ///< pannel将要设置的Geometry
    QRect mWillSetSeparatorGeometry;        ///< pannel将要设置的Separator的Geometry
};

///
/// \brief ribbon页的代理类
/// 如果需要修改重绘SARibbonCategory，可以通过设置SARibbonCategory::setProxy
///
class SARibbonCategoryPrivate
{
public:
    SARibbonCategoryPrivate(SARibbonCategory *parent);
    virtual ~SARibbonCategoryPrivate();

    SARibbonPannel *addPannel(const QString& title);
    void addPannel(SARibbonPannel *pannel);
    void insertPannel(int index, SARibbonPannel *pannel);

    //把pannel从Category中移除，不会销毁，此时pannel的所有权归还操作者
    bool takePannel(SARibbonPannel *pannel);

    //移除Pannel，Category会直接回收SARibbonPannel内存
    bool removePannel(SARibbonPannel *pannel);
    void setBackgroundBrush(const QBrush& brush);
    SARibbonCategory *ribbonCategory();
    void setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m);
    SARibbonPannel::PannelLayoutMode ribbonPannelLayoutMode() const;

    //返回所有的Pannel
    QList<SARibbonPannel *> pannelList();

    //计算所有元素的SizeHint宽度总和
    int totalSizeHintWidth() const;

    //更新item的布局,此函数会调用doItemLayout
    void updateItemGeometry();

    //实际操作
    void doItemLayout();

    //
    void onLeftScrollButtonClicked();
    void onRightScrollButtonClicked();
    void doWheelEvent(QWheelEvent *event);

public:
    SARibbonCategory *mParent;
    SARibbonPannel::PannelLayoutMode mDefaultPannelLayoutMode;
    SARibbonCategoryScrollButton *mLeftScrollBtn;
    SARibbonCategoryScrollButton *mRightScrollBtn;
    QList<SARibbonCategoryItem> mItemList;
    QSize mSizeHint;
    int mTotalWidth;
    int mXBase;
    bool mIsRightScrollBtnShow;     ///< 标记右滚动按钮是否需要显示
    bool mIsLeftScrollBtnShow;      ///< 标记左滚动按钮是否需要显示
    QMargins mContentsMargins;
};

SARibbonCategoryScrollButton::SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget *p)
    : QToolButton(p)
{
    setArrowType(arr);
}


SARibbonCategory::SARibbonCategory(QWidget *parent)
    : QWidget(parent)
    , m_d(new SARibbonCategoryPrivate(this))
{
    setAutoFillBackground(true);
    setBackgroundBrush(Qt::white);
    m_d->mLeftScrollBtn = new SARibbonCategoryScrollButton(Qt::LeftArrow, this);
    m_d->mRightScrollBtn = new SARibbonCategoryScrollButton(Qt::RightArrow, this);
    m_d->mLeftScrollBtn->setVisible(false);
    m_d->mRightScrollBtn->setVisible(false);
    connect(m_d->mLeftScrollBtn, &QToolButton::clicked, this, &SARibbonCategory::onLeftScrollButtonClicked);
    connect(m_d->mRightScrollBtn, &QToolButton::clicked, this, &SARibbonCategory::onRightScrollButtonClicked);
}


SARibbonCategory::~SARibbonCategory()
{
    delete m_d;
}


/**
 * @brief 设置pannel的模式
 *
 * 在@ref SARibbonBar 调用@ref SARibbonBar::setRibbonStyle 函数时，会对所有的SARibbonCategory调用此函数
 * 把新的SARibbonPannel::PannelLayoutMode设置进去
 * @param m
 */
void SARibbonCategory::setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m)
{
    m_d->setRibbonPannelLayoutMode(m);
}


SARibbonPannel::PannelLayoutMode SARibbonCategory::ribbonPannelLayoutMode() const
{
    return (m_d->ribbonPannelLayoutMode());
}


/**
 * @brief 添加pannel
 *
 * @note pannel的所有权由SARibbonCategory来管理，请不要在外部对其进行销毁
 * @param title pannel的标题，在office/wps的三行模式下会显示在pannel的下方
 * @return 返回生成的@ref SARibbonPannel 指针
 * @see 对SARibbonPannel的其他操作，参考 @ref SARibbonCategory::takePannel
 */
SARibbonPannel *SARibbonCategory::addPannel(const QString& title)
{
    return (m_d->addPannel(title));
}


/**
 * @brief 添加pannel
 * @param pannel的所有权SARibbonCategory来管理
 */
void SARibbonCategory::addPannel(SARibbonPannel *pannel)
{
    m_d->addPannel(pannel);
}


/**
 * @brief 把pannel脱离SARibbonCategory的管理
 * @param 需要提取的pannel
 * @return 成功返回true，否则返回false
 */
bool SARibbonCategory::takePannel(SARibbonPannel *pannel)
{
    return (m_d->takePannel(pannel));
}


/**
 * @brief 移除Pannel，Category会直接回收SARibbonPannel内存
 * @param pannel 需要移除的pannel
 * @note 移除后pannel为野指针，一般操作完建议把pannel指针设置为nullptr
 *
 * 此操作等同于：
 *
 * @code
 * SARibbonPannel* pannel;
 * ...
 * category->takePannel(pannel);
 * delete pannel;
 * @endcode
 */
bool SARibbonCategory::removePannel(SARibbonPannel *pannel)
{
    return (m_d->removePannel(pannel));
}


///
/// \brief SARibbonCategory::setBackgroundBrush
/// \param brush
///
void SARibbonCategory::setBackgroundBrush(const QBrush& brush)
{
    m_d->setBackgroundBrush(brush);
}


/**
 * @brief 返回Category下的所有pannel
 * @return
 */
QList<SARibbonPannel *> SARibbonCategory::pannelList() const
{
    return (m_d->pannelList());
}


QSize SARibbonCategory::sizeHint() const
{
    return (m_d->mSizeHint);
}


void SARibbonCategory::onLeftScrollButtonClicked()
{
    m_d->onLeftScrollButtonClicked();
}


void SARibbonCategory::onRightScrollButtonClicked()
{
    m_d->onRightScrollButtonClicked();
}


/**
 * @brief SARibbonCategory::event
 * @param event
 * @return
 */
bool SARibbonCategory::event(QEvent *e)
{
    switch (e->type())
    {
    case QEvent::LayoutRequest:
    {
        m_d->updateItemGeometry();
    }
    break;

    default:
        break;
    }
    return (QWidget::event(e));
}


void SARibbonCategory::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
//    QSize delta = e->size() - e->oldSize();

//    //尺寸没变就不用管
//    if (delta.isNull()) {
//        return;
//    }
    m_d->updateItemGeometry();
}


//////////////////////////////////////////////////////////////////

SARibbonCategoryPrivate::SARibbonCategoryPrivate(SARibbonCategory *parent)
    : mParent(parent)
    , mDefaultPannelLayoutMode(SARibbonPannel::ThreeRowMode)
    , mLeftScrollBtn(nullptr)
    , mRightScrollBtn(nullptr)
    , mSizeHint(50, 50)
    , mIsRightScrollBtnShow(false)
    , mIsLeftScrollBtnShow(false)
    , mContentsMargins(1, 1, 1, 1)
{
}


SARibbonCategoryPrivate::~SARibbonCategoryPrivate()
{
}


/**
 * @brief 计算所有元素的SizeHint宽度总和
 * @return
 */
int SARibbonCategoryPrivate::totalSizeHintWidth() const
{
    int total = 0;
    QMargins mag = mContentsMargins;

    if (!mag.isNull()) {
        total += (mag.left() + mag.right());
    }
    //先计算总长
    for (SARibbonCategoryItem item : mItemList)
    {
        if (item.isEmpty()) {
            //如果是hide就直接跳过
            continue;
        }
        QSize pannelSize = item.pannelWidget->sizeHint();
        QSize SeparatorSize(0, 0);
        if (item.separatorWidget) {
            SeparatorSize = item.separatorWidget->sizeHint();
        }
        total += pannelSize.width();
        total += SeparatorSize.width();
    }
    return (total);
}


SARibbonPannel *SARibbonCategoryPrivate::addPannel(const QString& title)
{
    SARibbonPannel *pannel = RibbonSubElementDelegate->createRibbonPannel(ribbonCategory());

    pannel->setWindowTitle(title);
    pannel->setPannelLayoutMode(ribbonPannelLayoutMode());
    pannel->installEventFilter(mParent);
    addPannel(pannel);
    return (pannel);
}


void SARibbonCategoryPrivate::addPannel(SARibbonPannel *pannel)
{
    insertPannel(mItemList.size(), pannel);
}


void SARibbonCategoryPrivate::insertPannel(int index, SARibbonPannel *pannel)
{
    if (nullptr == pannel) {
        return;
    }
    if (pannel->parentWidget() != mParent) {
        pannel->setParent(mParent);
    }
    index = qMax(0, index);
    index = qMin(mItemList.size(), index);
    SARibbonCategoryItem item;

    item.pannelWidget = pannel;
    //分割线
    item.separatorWidget = RibbonSubElementDelegate->createRibbonSeparatorWidget(mParent);
    //插入list中
    mItemList.insert(index, item);
    //标记需要重新计算尺寸
    updateItemGeometry();
}


bool SARibbonCategoryPrivate::takePannel(SARibbonPannel *pannel)
{
    SARibbonCategoryItem item;

    for (int i = 0; i < mItemList.size(); ++i)
    {
        if (pannel == mItemList[i].pannelWidget) {
            item = mItemList.takeAt(i);
        }
    }
    if (item.isNull()) {
        return (false);
    }
    if (item.separatorWidget) {
        item.separatorWidget->hide();
        //对应的分割线删除，但pannel不删除
        item.separatorWidget->deleteLater();
    }
    return (true);
}


bool SARibbonCategoryPrivate::removePannel(SARibbonPannel *pannel)
{
    if (takePannel(pannel)) {
        delete pannel;
        return (true);
    }
    return (false);
}


void SARibbonCategoryPrivate::setBackgroundBrush(const QBrush& brush)
{
    QPalette p = ribbonCategory()->palette();

    p.setBrush(QPalette::Background, brush);
    ribbonCategory()->setPalette(p);
}


QList<SARibbonPannel *> SARibbonCategoryPrivate::pannelList()
{
    QList<SARibbonPannel *> res;

    for (const SARibbonCategoryItem& i : mItemList)
    {
        if (!i.isNull()) {
            res.append(i.pannelWidget);
        }
    }
    return (res);
}


bool SARibbonCategory::eventFilter(QObject *watched, QEvent *event)
{
    if (nullptr == watched) {
        return (false);
    }
    SARibbonPannel *pannel = qobject_cast<SARibbonPannel *>(watched);

    if (pannel) {
        switch (event->type())
        {
        case QEvent::HideToParent:
        {
            //隐藏和显示都要重新布局
            //layout()->invalidate();
        }
        break;

        case QEvent::ShowToParent:
        {
            //隐藏和显示都要重新布局
            //layout()->invalidate();
        }
        break;

        default:
            break;
        }
    }
    return (false);
}


/**
 * @brief 在超出边界情况下，滚轮可滚动pannel
 * @param event
 */
void SARibbonCategory::wheelEvent(QWheelEvent *event)
{
    m_d->doWheelEvent(event);
}


SARibbonCategory *SARibbonCategoryPrivate::ribbonCategory()
{
    return (mParent);
}


/**
 * @brief 设置pannel的模式
 *
 * 在@ref SARibbonBar调用@ref SARibbonBar::setRibbonStyle 函数时，会对所有的SARibbonCategory调用此函数
 * 把新的SARibbonPannel::PannelLayoutMode设置进去
 *
 * 此函数为SARibbonCategory::setRibbonPannelLayoutMode的代理，
 * 在SARibbonCategory类中，此函数为protected，主要在SARibbonBar::setRibbonStyle时才会触发
 * @param m
 */
void SARibbonCategoryPrivate::setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m)
{
    if (mDefaultPannelLayoutMode == m) {
        return;
    }
    mDefaultPannelLayoutMode = m;
    QList<SARibbonPannel *> ps = pannelList();

    for (SARibbonPannel *p : ps)
    {
        p->setPannelLayoutMode(m);
    }
    updateItemGeometry();
}


SARibbonPannel::PannelLayoutMode SARibbonCategoryPrivate::ribbonPannelLayoutMode() const
{
    return (mDefaultPannelLayoutMode);
}


void SARibbonCategoryPrivate::updateItemGeometry()
{
    SARibbonCategory *category = ribbonCategory();

    int categoryWidth = category->width();
    QMargins mag = mContentsMargins;
    int height = category->height();
    int y = 0;

    if (!mag.isNull()) {
        y = mag.top();
        height -= (mag.top() + mag.bottom());
        categoryWidth -= (mag.right() + mag.left());
    }
    //total 是总宽，不是x坐标系，x才是坐标系
    int total = totalSizeHintWidth();
    //记录可以扩展的数量
    int canExpandingCount = 0;
    //扩展的宽度
    int expandWidth = 0;

    //如果total < categoryWidth,m_d->mXBase可以设置为0
    //判断是否超过总长度
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
    qDebug()	<< "\r\n\r\n============================================="
            << "\r\nSARibbonCategoryPrivate::updateItemGeometry"
            << "\r\npannel name:" << category->windowTitle()
            << "\r\n height:" << height
            << "\r\n first total:" <<total
            << "\r\n y:"<<y
            << "\r\n expandWidth:" << expandWidth
    ;
#endif
    if (total > categoryWidth) {
        //超过总长度，需要显示滚动按钮
        if (0 == mXBase) {
            //已经移动到最左，需要可以向右移动
            mIsRightScrollBtnShow = true;
            mIsLeftScrollBtnShow = false;
        }else if (mXBase <= (categoryWidth - total)) {
            //已经移动到最右，需要可以向左移动
            mIsRightScrollBtnShow = false;
            mIsLeftScrollBtnShow = true;
        }else {
            //移动到中间两边都可以动
            mIsRightScrollBtnShow = true;
            mIsLeftScrollBtnShow = true;
        }
    }else{
        //说明total 小于 categoryWidth
        mIsRightScrollBtnShow = false;
        mIsLeftScrollBtnShow = false;
        //这个是避免一开始totalWidth > categorySize.width()，通过滚动按钮调整了m_d->mBaseX
        //随之调整了窗体尺寸，调整后totalWidth < categorySize.width()导致category在原来位置
        //无法显示，必须这里把mBaseX设置为0
        mXBase = 0;
        //

        for (const SARibbonCategoryItem& item : mItemList)
        {
            if (!item.isEmpty()) {
                if (item.pannelWidget->isExpanding()) {
                    //pannel可扩展
                    ++canExpandingCount;
                }
            }
        }
        //计算可扩展的宽度
        if (canExpandingCount > 0) {
            expandWidth = (categoryWidth - total) / canExpandingCount;
        }else{
            expandWidth = 0;
        }
    }
    total = 0;// total重新计算
    int x = mXBase;

    //先按照sizeHint设置所有的尺寸
    for (SARibbonCategoryItem& item : mItemList)
    {
        if (item.isEmpty()) {
            //如果是hide就直接跳过
            if (item.separatorWidget) {
                //pannel hide分割线也要hide
                item.separatorWidget->hide();
            }
            item.mWillSetGeometry = QRect(0, 0, 0, 0);
            item.mWillSetSeparatorGeometry = QRect(0, 0, 0, 0);
            continue;
        }
        SARibbonPannel *p = item.pannelWidget;
        if (nullptr == p) {
            qDebug() << "unknow widget in SARibbonCategoryLayout";
            continue;
        }
        QSize pannelSize = p->sizeHint();
        QSize SeparatorSize(0, 0);
        if (item.separatorWidget) {
            SeparatorSize = item.separatorWidget->sizeHint();
        }
        if (p->isExpanding()) {
            //可扩展，就把pannel扩展到最大
            pannelSize.setWidth(pannelSize.width()+expandWidth);
        }
        int w = pannelSize.width();
        item.mWillSetGeometry = QRect(x, y, w, height);
        x += w;
        total += w;
        w = SeparatorSize.width();
        item.mWillSetSeparatorGeometry = QRect(x, y, w, height);
        x += w;
        total += w;
    }
    mTotalWidth = total;
    QWidget *cp = category->parentWidget();
    int parentHeight = (nullptr == cp) ? height : cp->height();
    int parentWidth = (nullptr == cp) ? total : cp->width();
    mSizeHint = QSize(parentWidth, parentHeight);
    doItemLayout();
}


void SARibbonCategoryPrivate::doItemLayout()
{
    SARibbonCategory *category = ribbonCategory();

    //两个滚动按钮的位置永远不变
    mLeftScrollBtn->setGeometry(5, 4, 12, category->height()-8);
    mRightScrollBtn->setGeometry(category->width()-12-5, 4, 12, category->height()-8);
    QList<QWidget *> showWidgets, hideWidgets;

    for (const SARibbonCategoryItem& item : mItemList)
    {
        if (item.isNull()) {
            continue;
        }
        if (item.isEmpty()) {
            hideWidgets << item.pannelWidget;
            if (item.separatorWidget) {
                hideWidgets << item.separatorWidget;
            }
        }else{
//            item.pannelWidget->setFixedSize(item.mWillSetGeometry.size());
//            item.pannelWidget->move(item.mWillSetGeometry.topLeft());
            item.pannelWidget->setGeometry(item.mWillSetGeometry);
            showWidgets << item.pannelWidget;
            if (item.separatorWidget) {
                item.separatorWidget->setGeometry(item.mWillSetSeparatorGeometry);
                showWidgets << item.separatorWidget;
            }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            qDebug() <<"SARibbonCategoryLayout::doLayout() =";
            qDebug()	<< "\r\n     pannel:" << item.pannelWidget->windowTitle()
                    << "\r\n     pannel geo:" << item.mWillSetGeometry
                    << "\r\n     sep geo:" << item.mWillSetSeparatorGeometry
            ;
#endif
        }
    }


    mRightScrollBtn->setVisible(mIsRightScrollBtnShow);
    mLeftScrollBtn->setVisible(mIsLeftScrollBtnShow);
    if (mIsRightScrollBtnShow) {
        mRightScrollBtn->raise();
    }
    if (mIsLeftScrollBtnShow) {
        mLeftScrollBtn->raise();
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


void SARibbonCategoryPrivate::onLeftScrollButtonClicked()
{
    SARibbonCategory *category = ribbonCategory();
    int width = category->width();
    //求总宽
    int totalWidth = mTotalWidth;

    if (totalWidth > width) {
        int tmp = mXBase + width;
        if (tmp > 0) {
            tmp = 0;
        }
        mXBase = tmp;
    }else{
        mXBase = 0;
    }
    updateItemGeometry();
}


void SARibbonCategoryPrivate::onRightScrollButtonClicked()
{
    SARibbonCategory *category = ribbonCategory();
    int width = category->width();
    //求总宽
    int totalWidth = mTotalWidth;

    if (totalWidth > width) {
        int tmp = mXBase - width;
        if (tmp < (width - totalWidth)) {
            tmp = width - totalWidth;
        }
        mXBase = tmp;
    }else {
        mXBase = 0;
    }
    updateItemGeometry();
}


void SARibbonCategoryPrivate::doWheelEvent(QWheelEvent *event)
{
    SARibbonCategory *category = ribbonCategory();
    int width = category->width();
    //求总宽
    int totalWidth = mTotalWidth;

    if (totalWidth > width) {
        //这个时候滚动有效
        int scrollpix = event->delta() / 4;
        if (scrollpix > 0) { //当滚轮向上滑，SARibbonCategory向左走
            int tmp = mXBase - scrollpix;
            if (tmp < (width - totalWidth)) {
                tmp = width - totalWidth;
            }
            mXBase = tmp;
        }else {                                 //当滚轮向下滑，SARibbonCategory向右走
            int tmp = mXBase - scrollpix;   //此时numDegrees为负数
            if (tmp > 0) {
                tmp = 0;
            }
            mXBase = tmp;
        }
        updateItemGeometry();
    }else {
        //这时候无需处理事件，把滚动事件上发让父级也能接收
        event->ignore();
        mXBase = 0;
    }
}
