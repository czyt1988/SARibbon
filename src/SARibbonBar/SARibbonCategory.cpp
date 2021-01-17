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

public:
    SARibbonCategory *mParent;
    SARibbonCategoryLayout *mLayout;
    SARibbonPannel::PannelLayoutMode mDefaultPannelLayoutMode;
    QList<SARibbonSeparatorWidget *> mSeparatorLists;
    SARibbonCategoryScrollButton *mLeftScrollBtn;
    SARibbonCategoryScrollButton *mRightScrollBtn;
    int mBaseX;
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
    return (m_d->mLayout->pannels());
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
        qDebug() << " ||||||||   LayoutRequest   ";
        //layout()->invalidate();
    }
    break;

    default:
        break;
    }
    return (QWidget::event(e));
}


//////////////////////////////////////////////////////////////////

SARibbonCategoryPrivate::SARibbonCategoryPrivate(SARibbonCategory *parent)
    : mParent(parent)
    , mDefaultPannelLayoutMode(SARibbonPannel::ThreeRowMode)
    , mLeftScrollBtn(nullptr)
    , mRightScrollBtn(nullptr)
    , mBaseX(0)
{
    mLayout = new SARibbonCategoryLayout(parent);
}


SARibbonCategoryPrivate::~SARibbonCategoryPrivate()
{
}


SARibbonPannel *SARibbonCategoryPrivate::addPannel(const QString& title)
{
    SARibbonPannel *pannel = RibbonSubElementDelegate->createRibbonPannel(ribbonCategory());

    pannel->setWindowTitle(title);
    pannel->setPannelLayoutMode(ribbonPannelLayoutMode());
    pannel->installEventFilter(mParent);
    mLayout->addPannel(pannel);

    return (pannel);
}


void SARibbonCategoryPrivate::addPannel(SARibbonPannel *pannel)
{
    if (pannel->parent() != ribbonCategory()) {
        pannel->setParent(ribbonCategory());
    }
    pannel->setPannelLayoutMode(ribbonPannelLayoutMode());
    mLayout->addPannel(pannel);

    //建立监控，监控show,hide事件，用于触发重新布局
    pannel->installEventFilter(mParent);
}


bool SARibbonCategoryPrivate::takePannel(SARibbonPannel *pannel)
{
    SARibbonCategoryLayoutItem *item = mLayout->takePannel(pannel);

    if (item->separatorWidget) {
        //对应的分割线删除，但pannel不删除
        item->separatorWidget->deleteLater();
    }
    bool isSucTake = (item != nullptr);

    delete item;
    return (isSucTake);
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
    return (mLayout->pannels());
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
            layout()->invalidate();
        }
        break;

        case QEvent::ShowToParent:
        {
            //隐藏和显示都要重新布局
            layout()->invalidate();
        }
        break;

        default:
            break;
        }
    }
    return (false);
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
    mLayout->invalidate();
}


SARibbonPannel::PannelLayoutMode SARibbonCategoryPrivate::ribbonPannelLayoutMode() const
{
    return (mDefaultPannelLayoutMode);
}
