#include "SARibbonCategory.h"
#include <QList>
#include <QResizeEvent>
#include <QPainter>
#include <QLinearGradient>
#include <QDebug>
#include <QApplication>
#include <QHBoxLayout>
#include <QList>
#include <QMap>
#include <QResizeEvent>
#include "SARibbonCategoryLayout.h"
#include "SARibbonElementManager.h"

///
/// \brief ribbon页的代理类
/// 如果需要修改重绘SARibbonCategory，可以通过设置SARibbonCategory::setProxy
///
class SARibbonCategory::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonCategory)
public:
    PrivateData(SARibbonCategory* p);

    SARibbonPannel* addPannel(const QString& title);
    SARibbonPannel* insertPannel(const QString& title, int index);
    void addPannel(SARibbonPannel* pannel);
    void insertPannel(int index, SARibbonPannel* pannel);

    // 把pannel从Category中移除，不会销毁，此时pannel的所有权归还操作者
    bool takePannel(SARibbonPannel* pannel);

    // 移除Pannel，Category会直接回收SARibbonPannel内存
    bool removePannel(SARibbonPannel* pannel);
    SARibbonCategory* ribbonCategory();
    const SARibbonCategory* ribbonCategory() const;

    // 返回所有的Pannel
    QList< SARibbonPannel* > pannelList();

    // 更新item的布局,此函数会调用doItemLayout
    void updateItemGeometry();

    void doWheelEvent(QWheelEvent* event);

public:
    bool mEnableShowPannelTitle { true };  ///< 是否运行pannel的标题栏显示
    int mPannelTitleHeight { 15 };         ///< pannel的标题栏默认高度
    bool mIsContextCategory { false };     ///< 标记是否是上下文标签
    bool mIsCanCustomize { true };         ///< 标记是否可以自定义
    SARibbonPannel::PannelLayoutMode mDefaultPannelLayoutMode { SARibbonPannel::ThreeRowMode };
};
SARibbonCategory::PrivateData::PrivateData(SARibbonCategory* p) : q_ptr(p)
{
}

SARibbonPannel* SARibbonCategory::PrivateData::addPannel(const QString& title)
{
    if (SARibbonCategoryLayout* lay = q_ptr->categoryLayout()) {
        SARibbonPannel* p = insertPannel(title, lay->pannelCount());
        return p;
    }
    return nullptr;
}

SARibbonPannel* SARibbonCategory::PrivateData::insertPannel(const QString& title, int index)
{
    SARibbonPannel* pannel = RibbonSubElementDelegate->createRibbonPannel(ribbonCategory());

    pannel->setPannelName(title);
    pannel->setObjectName(title);
    insertPannel(index, pannel);
    return (pannel);
}

void SARibbonCategory::PrivateData::addPannel(SARibbonPannel* pannel)
{
    if (SARibbonCategoryLayout* lay = q_ptr->categoryLayout()) {
        insertPannel(lay->pannelCount(), pannel);
    }
}

/**
 * @brief 插入pannel到layout
 *
 * 所有的添加操作最终会调用此函数
 * @param index
 * @param pannel
 */
void SARibbonCategory::PrivateData::insertPannel(int index, SARibbonPannel* pannel)
{
    if (nullptr == pannel) {
        return;
    }
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(q_ptr->layout());
    if (nullptr == lay) {
        return;
    }
    if (pannel->parentWidget() != q_ptr) {
        pannel->setParent(q_ptr);
    }
    // 同步一些状态
    pannel->setEnableShowTitle(mEnableShowPannelTitle);
    pannel->setTitleHeight(mPannelTitleHeight);
    pannel->setPannelLayoutMode(mDefaultPannelLayoutMode);
    index = qMax(0, index);
    index = qMin(lay->pannelCount(), index);
    lay->insertPannel(index, pannel);
    pannel->setVisible(true);
}

bool SARibbonCategory::PrivateData::takePannel(SARibbonPannel* pannel)
{
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(q_ptr->layout());
    if (nullptr == lay) {
        return false;
    }
    return lay->takePannel(pannel);
}

bool SARibbonCategory::PrivateData::removePannel(SARibbonPannel* pannel)
{
    if (takePannel(pannel)) {
        pannel->hide();
        pannel->deleteLater();
        return (true);
    }
    return (false);
}

QList< SARibbonPannel* > SARibbonCategory::PrivateData::pannelList()
{
    if (SARibbonCategoryLayout* lay = q_ptr->categoryLayout()) {
        return lay->pannelList();
    }
    return QList< SARibbonPannel* >();
}

SARibbonCategory* SARibbonCategory::PrivateData::ribbonCategory()
{
    return (q_ptr);
}

const SARibbonCategory* SARibbonCategory::PrivateData::ribbonCategory() const
{
    return (q_ptr);
}

void SARibbonCategory::PrivateData::updateItemGeometry()
{
#if SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "SARibbonCategory::PrivateData::updateItemGeometry,categoryName=" << q_ptr->categoryName();
#endif
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(q_ptr->layout());
    if (!lay) {
        return;
    }
    const QList< SARibbonPannel* > pannels = lay->pannelList();
    for (auto pannel : pannels) {
        pannel->updateItemGeometry();
    }
    lay->updateGeometryArr();
    return;
}

void SARibbonCategory::PrivateData::doWheelEvent(QWheelEvent* event)
{
    SARibbonCategoryLayout* lay = q_ptr->categoryLayout();
    if (nullptr == lay) {
        return;
    }
    QSize contentSize = lay->categoryContentSize();
    // 求总宽
    int totalWidth = lay->categoryTotalWidth();

    if (totalWidth > contentSize.width()) {
        // 这个时候滚动有效
        int scrollpix = 40;
        // Qt6 取消了QWheelEvent::delta函数
        // 是要下面方法可兼容qt5/6
        QPoint numPixels  = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;
        if (!numPixels.isNull()) {
            if (numDegrees.y() < 0) {
                scrollpix = -scrollpix;
            }
        } else if (!numDegrees.isNull()) {
            if (numDegrees.y() < 0) {
                scrollpix = -scrollpix;
            }
        }
        lay->scroll(scrollpix);
    } else {
        // 这时候无需处理事件，把滚动事件上发让父级也能接收
        event->ignore();
        // 如滚动过就还原
        if (lay->isScrolled()) {
            lay->scroll(0);
        }
    }
}

//----------------------------------------------------
// SARibbonCategory
//----------------------------------------------------

SARibbonCategory::SARibbonCategory(QWidget* p) : QWidget(p), d_ptr(new SARibbonCategory::PrivateData(this))
{
    setAttribute(Qt::WA_StyledBackground);
    setLayout(new SARibbonCategoryLayout(this));
}

SARibbonCategory::SARibbonCategory(const QString& name, QWidget* p)
    : QWidget(p), d_ptr(new SARibbonCategory::PrivateData(this))
{
    setAttribute(Qt::WA_StyledBackground);
    setLayout(new SARibbonCategoryLayout(this));
    setCategoryName(name);
}

SARibbonCategory::~SARibbonCategory()
{
}

/**
 * @brief category的名字,等同windowTitle函数
 * @return
 */
QString SARibbonCategory::categoryName() const
{
    return (windowTitle());
}

/**
 * @brief 设置category名字，等同setWindowTitle
 * @param title
 */
void SARibbonCategory::setCategoryName(const QString& title)
{
    setWindowTitle(title);
}

bool SARibbonCategory::event(QEvent* e)
{
#if SA_DEBUG_PRINT_EVENT
    if (e->type() != QEvent::Paint) {
        qDebug() << "SARibbonCategory event(" << e->type() << "),name=" << categoryName();
    }
#endif
    return QWidget::event(e);
}

/**
 * @brief pannel的模式
 * @return
 */
SARibbonPannel::PannelLayoutMode SARibbonCategory::ribbonPannelLayoutMode() const
{
    return (d_ptr->mDefaultPannelLayoutMode);
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
    d_ptr->mDefaultPannelLayoutMode = m;
    iterate([ m ](SARibbonPannel* p) -> bool {
        p->setPannelLayoutMode(m);
        return true;
    });
    updateItemGeometry();
}

/**
 * @brief 添加pannel
 *
 * @note pannel的所有权由SARibbonCategory来管理，请不要在外部对其进行销毁
 * @param title pannel的标题，在office/wps的三行模式下会显示在pannel的下方
 * @return 返回生成的@ref SARibbonPannel 指针
 * @see 对SARibbonPannel的其他操作，参考 @ref SARibbonCategory::takePannel
 */
SARibbonPannel* SARibbonCategory::addPannel(const QString& title)
{
    return (d_ptr->addPannel(title));
}

/**
 * @brief 添加pannel
 * @param pannel pannel的所有权SARibbonCategory来管理
 */
void SARibbonCategory::addPannel(SARibbonPannel* pannel)
{
    d_ptr->addPannel(pannel);
}

/**
 * @brief qt designer专用
 * @param pannel
 */
void SARibbonCategory::addPannel(QWidget* pannel)
{
    SARibbonPannel* p = qobject_cast< SARibbonPannel* >(pannel);
    if (p) {
        addPannel(p);
    }
}

/**
 * @brief 新建一个pannel，并插入到index位置
 * @param title pannel的title
 * @param index 插入的位置，如果index超出category里pannel的个数，将插入到最后
 * @return 返回生成的@ref SARibbonPannel 指针
 * @note 如果
 */
SARibbonPannel* SARibbonCategory::insertPannel(const QString& title, int index)
{
    return (d_ptr->insertPannel(title, index));
}

/**
 * @brief 通过名字查找pannel
 * @param title
 * @return 如果有重名，只会返回第一个符合条件的
 */
SARibbonPannel* SARibbonCategory::pannelByName(const QString& title) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->pannelByName(title);
    }
    return nullptr;
}

/**
 * @brief 通过ObjectName查找pannel
 * @param objname
 * @return
 */
SARibbonPannel* SARibbonCategory::pannelByObjectName(const QString& objname) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->pannelByObjectName(objname);
    }
    return nullptr;
}

/**
 * @brief 通过索引找到pannel，如果超过索引范围，会返回nullptr
 * @param index
 * @return 如果超过索引范围，会返回nullptr
 */
SARibbonPannel* SARibbonCategory::pannelByIndex(int index) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->pannelByIndex(index);
    }
    return nullptr;
}

/**
 * @brief 查找pannel对应的索引
 * @param p
 * @return 如果找不到，返回-1
 */
int SARibbonCategory::pannelIndex(SARibbonPannel* p) const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->pannelIndex(p);
    }
    return -1;
}

/**
 * @brief 移动一个Pannel从from index到to index
 * @param from 要移动pannel的index
 * @param to 要移动到的位置
 */
void SARibbonCategory::movePannel(int from, int to)
{
    if (from == to) {
        return;
    }
    if (to < 0) {
        to = 0;
    }
    if (to >= pannelCount()) {
        to = pannelCount() - 1;
    }
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        lay->movePannel(from, to);
    }
}

/**
 * @brief 把pannel脱离SARibbonCategory的管理
 * @param pannel 需要提取的pannel
 * @return 成功返回true，否则返回false
 */
bool SARibbonCategory::takePannel(SARibbonPannel* pannel)
{
    return (d_ptr->takePannel(pannel));
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
 * pannel->hide();
 * pannel->deleteLater();
 * @endcode
 */
bool SARibbonCategory::removePannel(SARibbonPannel* pannel)
{
    return (d_ptr->removePannel(pannel));
}

/**
 * @brief 移除pannel
 * @param index pannel的索引，如果超出会返回false
 * @return 成功返回true
 */
bool SARibbonCategory::removePannel(int index)
{
    SARibbonPannel* p = pannelByIndex(index);

    if (nullptr == p) {
        return (false);
    }
    return (removePannel(p));
}

/**
 * @brief 返回Category下的所有pannel
 * @return
 */
QList< SARibbonPannel* > SARibbonCategory::pannelList() const
{
    return (d_ptr->pannelList());
}

QSize SARibbonCategory::sizeHint() const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->sizeHint();
    }
    return QSize(1000, 100);
}

/**
 * @brief 如果是ContextCategory，此函数返回true
 * @return
 */
bool SARibbonCategory::isContextCategory() const
{
    return (d_ptr->mIsContextCategory);
}

/**
 * @brief 返回pannel的个数
 * @return
 */
int SARibbonCategory::pannelCount() const
{
    if (SARibbonCategoryLayout* lay = categoryLayout()) {
        return lay->pannelCount();
    }
    return -1;
}

/**
 * @brief 判断是否可以自定义
 * @return
 */
bool SARibbonCategory::isCanCustomize() const
{
    return (d_ptr->mIsCanCustomize);
}

/**
 * @brief 设置是否可以自定义
 * @param b
 */
void SARibbonCategory::setCanCustomize(bool b)
{
    d_ptr->mIsCanCustomize = b;
}

/**
 * @brief pannel标题栏的高度
 * @return
 */
int SARibbonCategory::pannelTitleHeight() const
{
    return d_ptr->mPannelTitleHeight;
}
/**
 * @brief 设置pannel的高度
 * @param h
 */
void SARibbonCategory::setPannelTitleHeight(int h)
{
    d_ptr->mPannelTitleHeight = h;
    iterate([ h ](SARibbonPannel* p) -> bool {
        p->setTitleHeight(h);
        return true;
    });
}

/**
 * @brief 是否pannel显示标题栏
 * @return
 */
bool SARibbonCategory::isEnableShowPannelTitle() const
{
    return d_ptr->mEnableShowPannelTitle;
}

/**
 * @brief 设置显示pannel标题
 * @param on
 */
void SARibbonCategory::setEnableShowPannelTitle(bool on)
{
    d_ptr->mEnableShowPannelTitle = on;
    iterate([ on ](SARibbonPannel* p) -> bool {
        p->setEnableShowTitle(on);
        return true;
    });
}

/**
 * @brief 获取对应的ribbonbar
 * @return 如果没有加入ribbonbar的管理，此值为null
 */
SARibbonBar* SARibbonCategory::ribbonBar() const
{
    // 第一个par是stackwidget
    if (QWidget* par = parentWidget()) {
        // 理论此时是ribbonbar
        par = par->parentWidget();
        while (par) {
            if (SARibbonBar* ribbon = qobject_cast< SARibbonBar* >(par)) {
                return ribbon;
            }
            par = par->parentWidget();
        }
    }
    return nullptr;
}

/**
 * @brief 刷新category的布局，适用于改变ribbon的模式之后调用
 */
void SARibbonCategory::updateItemGeometry()
{
#if SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "SARibbonCategory name=" << categoryName() << " updateItemGeometry";
#endif
    d_ptr->updateItemGeometry();
}

/**
 * @brief 此函数会遍历Category下的所有pannel,执行函数指针
 * @param fp 函数指针返回false则停止迭代
 * @return 返回false代表没有迭代完所有的category，中途接收到回调函数的false返回而中断迭代
 */
bool SARibbonCategory::iterate(FpPannelIterate fp)
{
    const QList< SARibbonPannel* > ps = pannelList();
    for (SARibbonPannel* p : ps) {
        if (!fp(p)) {
            return false;
        }
    }
    return true;
}

/**
   @brief 设置Category的对齐方式
   @param al
 */
void SARibbonCategory::setCategoryAlignment(SARibbonAlignment al)
{
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(layout());
    if (lay) {
        lay->setCategoryAlignment(al);
    }
}

/**
   @brief Category的对齐方式
   @return
 */
SARibbonAlignment SARibbonCategory::categoryAlignment() const
{
    SARibbonCategoryLayout* lay = qobject_cast< SARibbonCategoryLayout* >(layout());
    if (lay) {
        return lay->categoryAlignment();
    }
    return SARibbonAlignment::AlignLeft;
}

/**
 * @brief 在超出边界情况下，滚轮可滚动pannel
 * @param event
 */
void SARibbonCategory::wheelEvent(QWheelEvent* event)
{
    d_ptr->doWheelEvent(event);
}

void SARibbonCategory::changeEvent(QEvent* event)
{
    switch (event->type()) {
    case QEvent::StyleChange: {
        if (layout()) {
#if SA_DEBUG_PRINT_SIZE_HINT
            qDebug() << "SARibbonCategory changeEvent(StyleChange),categoryName=" << categoryName();
#endif
            layout()->invalidate();
        }
    } break;
    case QEvent::FontChange: {
#if SA_DEBUG_PRINT_SIZE_HINT
        qDebug() << "SARibbonCategory changeEvent(FontChange),categoryName=" << categoryName();
#endif
        QFont f = font();
        iterate([ f ](SARibbonPannel* p) -> bool {
            p->setFont(f);
            return true;
        });
        if (layout()) {
            layout()->invalidate();
        }
    } break;
    default:
        break;
    };
    return QWidget::changeEvent(event);
}

/**
 * @brief 标记这个是上下文标签
 * @param isContextCategory
 */
void SARibbonCategory::markIsContextCategory(bool isContextCategory)
{
    d_ptr->mIsContextCategory = isContextCategory;
}

/**
 * @brief 获取SARibbonCategoryLayoutlayout
 * @return
 */
SARibbonCategoryLayout* SARibbonCategory::categoryLayout() const
{
    return qobject_cast< SARibbonCategoryLayout* >(layout());
}

//===================================================
// SARibbonCategoryScrollButton
//===================================================
SARibbonCategoryScrollButton::SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget* p) : QToolButton(p)
{
    setArrowType(arr);
}
