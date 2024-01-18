#include "SARibbonPannel.h"
#include "SARibbonCategory.h"
#include "SARibbonElementManager.h"
#include "SARibbonGallery.h"
#include "SARibbonPannelLayout.h"
#include "SARibbonPannelOptionButton.h"
#include "SARibbonToolButton.h"
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFontMetrics>
#include <QGridLayout>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QResizeEvent>
#include <QWidgetAction>

#ifndef SARibbonPannel_DEBUG_PRINT
#define SARibbonPannel_DEBUG_PRINT 0
#endif

#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashLine);                                                                                       \
        _pen.setColor(Qt::blue);                                                                                       \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)
//===============================================================
// SARibbonPannelLabel
//===============================================================

SARibbonPannelLabel::SARibbonPannelLabel(QWidget* parent) : QLabel(parent)
{
}

//===============================================================
// SARibbonPannel::PrivateData
//===============================================================

class SARibbonPannel::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonPannel)
public:
    PrivateData(SARibbonPannel* p);
    // 根据m_pannelLayoutMode返回gridLayout应该增加的行数
    int rowadded();
    void createLayout();
    // 获取layout
    SARibbonPannelLayout* pannelLayout() const;
    // 返回最后一个添加的action对应的button，前提是最后一个是toolbutton，否则返回nullptr
    SARibbonToolButton* lastAddActionButton();
    // 重置labe的字体，这个主要是为了让pannel的标题字体能适应标题高度
    void resetTitleLabelFont();
    // 标题
    QString pannelName() const;
    void setPannelName(const QString& title);

public:
    bool m_isCanCustomize { true };                                                        ///< 记录是否可自定义
    SARibbonPannel::PannelLayoutMode m_pannelLayoutMode { SARibbonPannel::ThreeRowMode };  ///< pannel的布局模式，默认为3行模式ThreeRowMode
    SARibbonPannelOptionButton* m_optionActionButton { nullptr };  ///< 标题栏的y距离
    SARibbonPannelLabel* m_label { nullptr };
};

SARibbonPannel::PrivateData::PrivateData(SARibbonPannel* p) : q_ptr(p)
{
    createLayout();
}

int SARibbonPannel::PrivateData::rowadded()
{
    switch (m_pannelLayoutMode) {
    case SARibbonPannel::ThreeRowMode:
        return (2);

    case SARibbonPannel::TwoRowMode:
        return (3);

    default:
        break;
    }
    return (2);
}

void SARibbonPannel::PrivateData::createLayout()
{
    m_label = new SARibbonPannelLabel(q_ptr);
    m_label->setAlignment(Qt::AlignCenter);
    SARibbonPannelLayout* lay = new SARibbonPannelLayout(q_ptr);
    lay->setPannelTitleLabel(m_label);
    lay->setSpacing(2);
    lay->setContentsMargins(2, 2, 2, 2);
}

SARibbonPannelLayout* SARibbonPannel::PrivateData::pannelLayout() const
{
    return qobject_cast< SARibbonPannelLayout* >(q_ptr->layout());
}

SARibbonToolButton* SARibbonPannel::PrivateData::lastAddActionButton()
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return (qobject_cast< SARibbonToolButton* >(lay->lastWidget()));
    }
    return (nullptr);
}

/**
 * @brief 重置label的字体
 *
 * @note 此函数必须在布局设置后调用
 */
void SARibbonPannel::PrivateData::resetTitleLabelFont()
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        int h   = lay->pannelTitleHeight();
        QFont f = q_ptr->font();
        f.setPixelSize(h * 0.8);
        if (m_label) {
            m_label->setFont(f);
        }
    }
}

QString SARibbonPannel::PrivateData::pannelName() const
{
    if (m_label) {
        return m_label->text();
    }
    return (QString());
}

void SARibbonPannel::PrivateData::setPannelName(const QString& title)
{
    if (m_label) {
        m_label->setText(title);
    }
}

//==================================================
// SARibbonPannel
//==================================================

SARibbonPannel::SARibbonPannel(QWidget* parent) : QFrame(parent), d_ptr(new SARibbonPannel::PrivateData(this))
{
    setPannelLayoutMode(ThreeRowMode);
}

SARibbonPannel::SARibbonPannel(const QString& name, QWidget* parent)
    : QFrame(parent), d_ptr(new SARibbonPannel::PrivateData(this))
{
    setPannelLayoutMode(ThreeRowMode);
    setPannelName(name);
}

SARibbonPannel::~SARibbonPannel()
{
}

/**
 * @brief 把action的行属性设置进action中，action自身携带了行属性
 * @param action
 * @param rp
 */
void SARibbonPannel::setActionRowProportionProperty(QAction* action, SARibbonPannelItem::RowProportion rp)
{
    Q_CHECK_PTR(action);
    action->setProperty(SA_ActionPropertyName_RowProportion, static_cast< int >(rp));
}

/**
 * @brief 获取action的行属性
 * @param action
 * @return
 */
SARibbonPannelItem::RowProportion SARibbonPannel::getActionRowProportionProperty(QAction* action)
{
    bool isok = false;
    int r     = action->property(SA_ActionPropertyName_RowProportion).toInt(&isok);

    if (isok) {
        return (static_cast< SARibbonPannelItem::RowProportion >(r));
    }
    return (SARibbonPannelItem::Large);
}

/**
 * @brief 设置action的ToolButtonPopupMode属性
 * @param action
 * @param popMode
 */
void SARibbonPannel::setActionToolButtonPopupModeProperty(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    Q_CHECK_PTR(action);
    action->setProperty(SA_ActionPropertyName_ToolButtonPopupMode, static_cast< int >(popMode));
}

/**
 * @brief 获取action的ToolButtonPopupMode属性
 * @param action
 * @return
 */
QToolButton::ToolButtonPopupMode SARibbonPannel::getActionToolButtonPopupModeProperty(QAction* action)
{
    bool isok = false;
    int r     = action->property(SA_ActionPropertyName_ToolButtonPopupMode).toInt(&isok);

    if (isok) {
        return (static_cast< QToolButton::ToolButtonPopupMode >(r));
    }
    return (QToolButton::InstantPopup);
}

/**
 * @brief 设置action的ToolButtonStyle属性
 * @param action
 * @param buttonStyle
 */
void SARibbonPannel::setActionToolButtonStyleProperty(QAction* action, Qt::ToolButtonStyle buttonStyle)
{
    Q_CHECK_PTR(action);
    action->setProperty(SA_ActionPropertyName_ToolButtonStyle, static_cast< int >(buttonStyle));
}

/**
 * @brief 获取action的ToolButtonStyle属性
 * @param action
 * @return
 */
Qt::ToolButtonStyle SARibbonPannel::getActionToolButtonStyleProperty(QAction* action)
{
    bool isok = false;
    int r     = action->property(SA_ActionPropertyName_ToolButtonStyle).toInt(&isok);

    if (isok) {
        return (static_cast< Qt::ToolButtonStyle >(r));
    }
    return (Qt::ToolButtonIconOnly);
}

/**
 * @brief 添加action
 *
 * action实际对应了一个toolbutton，如果想找到对应的toolbutton，使用@ref actionToRibbonToolButton
 * @param action
 * @param rp 指定action的行占比
 * @param popMode 菜单弹出样式
 */
void SARibbonPannel::addAction(QAction* action, SARibbonPannelItem::RowProportion rp)
{
    Q_CHECK_PTR(action);
    setActionRowProportionProperty(action, rp);
    addAction(action);
}

/**
 * @brief 添加一个action
 * @param act
 * @param popMode 按钮的样式
 * @param rp action在pannel中的占位情况，默认是大图标
 */
void SARibbonPannel::addAction(QAction* act, QToolButton::ToolButtonPopupMode popMode, SARibbonPannelItem::RowProportion rp)
{
    Q_CHECK_PTR(act);
    setActionRowProportionProperty(act, rp);
    setActionToolButtonPopupModeProperty(act, popMode);
    addAction(act);
}

/**
   @brief 添加大图标

   action实际对应了一个toolbutton，如果想找到对应的toolbutton，使用@ref actionToRibbonToolButton
   @param action
 */
void SARibbonPannel::addLargeAction(QAction* action)
{
    addAction(action, SARibbonPannelItem::Large);
}

/**
   @brief 在三栏模式下，强制加为2栏action
   @note 在两行模式下，Medium和Small等价
   主要应用在ThreeRowMode下

   action实际对应了一个toolbutton，如果想找到对应的toolbutton，使用@ref actionToRibbonToolButton
   @param action
 */
void SARibbonPannel::addMediumAction(QAction* action)
{
    addAction(action, SARibbonPannelItem::Medium);
}

/**
   @brief 添加小图标

   action实际对应了一个toolbutton，如果想找到对应的toolbutton，使用@ref actionToRibbonToolButton
   @param action
 */
void SARibbonPannel::addSmallAction(QAction* action)
{
    addAction(action, SARibbonPannelItem::Small);
}

void SARibbonPannel::addSmallAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    addAction(action, popMode, SARibbonPannelItem::Small);
}

void SARibbonPannel::addLargeAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    addAction(action, popMode, SARibbonPannelItem::Large);
}

void SARibbonPannel::addMediumAction(QAction* action, QToolButton::ToolButtonPopupMode popMode)
{
    addAction(action, popMode, SARibbonPannelItem::Medium);
}

/**
 * @brief 生成并添加一个action
 *
 * 如果不对此action做操作，SARibbonPannel将管理此action
 *
 * @note action的父对象将设置为SARibbonPannel，SARibbonPannel在删除时将会删除子对象，会把这个action也删除，
 * 如果不想此action也删除，需要对action重新设置父对象
 *
 * @param text action的文字
 * @param icon action的图标
 * @param popMode 按钮的样式
 * @param rp action在pannel中的占位情况，默认是大图标
 * @return 返回添加的action
 */
QAction* SARibbonPannel::addAction(const QString& text,
                                   const QIcon& icon,
                                   QToolButton::ToolButtonPopupMode popMode,
                                   SARibbonPannelItem::RowProportion rp)
{
    QAction* action = new QAction(icon, text, this);
    addAction(action, popMode, rp);
    return (action);
}

/**
 * @brief 添加一个普通菜单
 * @param menu
 * @param rp
 * @param popMode,菜单弹出模式，默认InstantPopup模式
 * @return
 */
void SARibbonPannel::addMenu(QMenu* menu, SARibbonPannelItem::RowProportion rp, QToolButton::ToolButtonPopupMode popMode)
{
    Q_CHECK_PTR(menu);
    QAction* action = menu->menuAction();
    action->setIcon(menu->icon());
    action->setText(menu->title());
    action->setObjectName("action." + menu->objectName());
    addAction(action, popMode, rp);
}

void SARibbonPannel::addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode)
{
    addMenu(menu, SARibbonPannelItem::Large, popMode);
}

void SARibbonPannel::addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode)
{
    addMenu(menu, SARibbonPannelItem::Small, popMode);
}

/**
 * @brief 添加窗口
 *
 * @param w
 * @param rp
 * @return 返回action(QWidgetAction)
 * @note SARibbonPannel并不会管理此窗口内存，在delete SARibbonPannel时，此窗口如果父对象不是SARibbonPannel将不会被删除
 */
QAction* SARibbonPannel::addWidget(QWidget* w, SARibbonPannelItem::RowProportion rp)
{
    QWidgetAction* action = new QWidgetAction(this);

    action->setDefaultWidget(w);
    action->setIcon(w->windowIcon());
    action->setText(w->windowTitle());
    action->setObjectName("action." + w->objectName());
    w->setAttribute(Qt::WA_Hover);
    setActionRowProportionProperty(action, rp);
    addAction(action);
    return (action);
}

/**
 * @brief 添加窗口,占用ribbon的一行
 * @param w
 * @return 返回action(QWidgetAction)
 */
QAction* SARibbonPannel::addSmallWidget(QWidget* w)
{
    return (addWidget(w, SARibbonPannelItem::Small));
}

/**
 * @brief 添加窗口,占用ribbon的一行
 * @param w
 * @return
 */
QAction* SARibbonPannel::addMediumWidget(QWidget* w)
{
    return (addWidget(w, SARibbonPannelItem::Medium));
}

/**
 * @brief 添加窗口，占用所有行
 * @param w
 * @return 返回action(QWidgetAction)
 */
QAction* SARibbonPannel::addLargeWidget(QWidget* w)
{
    return (addWidget(w, SARibbonPannelItem::Large));
}

/**
 * @brief SARibbonPannel::addGallery
 * @return
 * @note SARibbonPannel将拥有SARibbonGallery的管理权
 */
SARibbonGallery* SARibbonPannel::addGallery(bool expanding)
{
    SARibbonGallery* gallery = RibbonSubElementFactory->createRibbonGallery(this);

    addWidget(gallery, SARibbonPannelItem::Large);
    if (expanding) {
        setExpanding(expanding);
    }
    return (gallery);
}

/**
 * @brief 添加分割线
 * @param top 上边距 @default 6
 * @param bottom 下边距 @default 6
 */
QAction* SARibbonPannel::addSeparator()
{
    QAction* action = new QAction(this);

    action->setSeparator(true);
    setActionRowProportionProperty(action, SARibbonPannelItem::Large);
    addAction(action);
    return (action);
}

/**
 * @brief 从pannel中把action对应的button提取出来，如果action没有对应的button，就返回nullptr
 * @param action
 * @return 如果action没有对应的button，就返回nullptr
 */
SARibbonToolButton* SARibbonPannel::actionToRibbonToolButton(QAction* action)
{
#if 0
    SARibbonPannelLayout* lay = qobject_cast< SARibbonPannelLayout* >(layout());

    if (lay) {
        int index = lay->indexOf(action);
        if (index == -1) {
            return (nullptr);
        }
        QLayoutItem* item       = lay->takeAt(index);
        SARibbonToolButton* btn = qobject_cast< SARibbonToolButton* >(item ? item->widget() : nullptr);
        return (btn);
    }
    return (nullptr);
#else
    for (auto obj : qAsConst(children())) {
        if (obj->isWidgetType()) {
            if (SARibbonToolButton* btn = qobject_cast< SARibbonToolButton* >(obj)) {
                if (btn->defaultAction() == action) {
                    return btn;
                }
            }
        }
    }
    return (nullptr);
#endif
}

/**
 * @brief 获取pannel下面的所有toolbutton
 * @return
 */
QList< SARibbonToolButton* > SARibbonPannel::ribbonToolButtons() const
{
    const QObjectList& objs = children();
    QList< SARibbonToolButton* > res;

    for (QObject* o : objs) {
        SARibbonToolButton* b = qobject_cast< SARibbonToolButton* >(o);
        if (b) {
            res.append(b);
        }
    }
    return (res);
}

/**
 * @brief 设置PannelLayoutMode
 * @param mode
 */
void SARibbonPannel::setPannelLayoutMode(SARibbonPannel::PannelLayoutMode mode)
{
    if (d_ptr->m_pannelLayoutMode == mode) {
        return;
    }
    d_ptr->m_pannelLayoutMode = mode;
    updateItemGeometry();
}

SARibbonPannel::PannelLayoutMode SARibbonPannel::pannelLayoutMode() const
{
    return (d_ptr->m_pannelLayoutMode);
}

/**
 * @brief 添加操作action，如果要去除，传入nullptr指针即可，SARibbonPannel不会对QAction的所有权进行管理
 * @param action
 * @note 要去除OptionAction直接传入nullptr即可
 * @note SARibbonPannel不对QAction的destroy进行关联，如果外部对action进行delete，需要先传入nullptr给addOptionAction
 */
void SARibbonPannel::setOptionAction(QAction* action)
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        lay->setOptionAction(action);
    }
}

/**
 * @brief 判断是否存在OptionAction
 * @return 存在返回true
 */
bool SARibbonPannel::isHaveOptionAction() const
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return lay->isHaveOptionAction();
    }
    return false;
}

QSize SARibbonPannel::sizeHint() const
{
    int shWidth  = 500;
    int shHeight = 100;
    if (QLayout* lay = layout()) {
        QSize laySize = layout()->sizeHint();
        shWidth       = laySize.width();
        shHeight      = laySize.height();
    }
    return QSize(shWidth, shHeight);
}

QSize SARibbonPannel::minimumSizeHint() const
{
    return (layout()->minimumSize());
}

/**
 * @brief 把pannel设置为扩展模式，此时会撑大水平区域
 * @param isExpanding
 */
void SARibbonPannel::setExpanding(bool isExpanding)
{
    setSizePolicy(isExpanding ? QSizePolicy::Expanding : QSizePolicy::Preferred, QSizePolicy::Fixed);
}

/**
 * @brief 判断此pannel是否为（水平）扩展模式
 * @return 是扩展模式返回true
 */
bool SARibbonPannel::isExpanding() const
{
    QSizePolicy sp = sizePolicy();

    return (sp.horizontalPolicy() == QSizePolicy::Expanding);
}

/**
   @brief 设置标题栏的高度
   @param h
 */
void SARibbonPannel::setTitleHeight(int h)
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        lay->setPannelTitleHeight(h);
    }
}

/**
 * @brief 标题栏高度
 * @return
 */
int SARibbonPannel::titleHeight() const
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return lay->pannelTitleHeight();
    }
    return -1;
}

/**
 * @brief 判断是否显示标题，只有标题的高度被设置，才会显示标题
 * @return
 */
bool SARibbonPannel::isEnableShowTitle() const
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return lay->isEnableShowPannelTitle();
    }
    return false;
}

/**
 * @brief 是否显示标题，显示标题后，标题的高度需要设置，默认高度为15
 * @param on
 */
void SARibbonPannel::setEnableShowTitle(bool on)
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return lay->setEnableShowPannelTitle(on);
    }
}

/**
 * @brief action对应的布局index，此操作一般用于移动，其他意义不大
 * @param act
 * @return 没有查到返回-1
 */
int SARibbonPannel::actionIndex(QAction* act) const
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return (lay->indexByAction(act));
    }
    return (-1);
}

/**
 * @brief 移动action
 * @param from 要移动action的位置，通过@ref actionIndex 获取
 * @param to 要移动的位置
 */
void SARibbonPannel::moveAction(int from, int to)
{
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        return (lay->move(from, to));
    }
    updateGeometry();  // 通知layout进行重新布局
}

/**
 * @brief 判断是否可以自定义
 * @return
 */
bool SARibbonPannel::isCanCustomize() const
{
    return (d_ptr->m_isCanCustomize);
}

/**
 * @brief 设置是否可以自定义
 * @param b
 */
void SARibbonPannel::setCanCustomize(bool b)
{
    d_ptr->m_isCanCustomize = b;
}

/**
 * @brief pannel名称
 * @return
 */
QString SARibbonPannel::pannelName() const
{
    return (d_ptr->pannelName());
}

/**
 * @brief pannel的名称
 * @param title 标题
 * @note 此函数会触发@sa pannelNameChanged 信号
 */
void SARibbonPannel::setPannelName(const QString& title)
{
    QString oldName = pannelName();
    if (oldName != title) {
        d_ptr->setPannelName(title);
        emit pannelNameChanged(title);
    }
}

/**
 * @brief 获取大图标的高度
 * @return
 */
int SARibbonPannel::largeButtonHeight() const
{
    const QMargins& mag = contentsMargins();
    return height() - mag.top() - mag.bottom() - titleHeight();
}

/**
 * @brief 获取pannel layout
 * @return
 */
SARibbonPannelLayout* SARibbonPannel::pannelLayout() const
{
    return d_ptr->pannelLayout();
}

/**
 * @brief 更新布局
 */
void SARibbonPannel::updateItemGeometry()
{
#if SA_DEBUG_PRINT_SIZE_HINT
    qDebug() << "SARibbonPannel updateItemGeometry,pannelName=" << pannelName();
#endif
    // 此函数需要添加，否则SARibbonBar::setEnableWordWrap无法刷新按钮
    resetToolButtonSize();
    if (SARibbonPannelLayout* lay = pannelLayout()) {
        lay->updateGeomArray();
    }
}

/**
   @brief 获取category指针，如果没有parent，或者不在category管理，返回nullptr
   @return
 */
SARibbonCategory* SARibbonPannel::category() const
{
    return qobject_cast< SARibbonCategory* >(parent());
}

/**
   @brief 获取ribbonBar指针，如果没有返回nullptr
   @return
 */
SARibbonBar* SARibbonPannel::ribbonBar() const
{
    if (SARibbonCategory* c = category()) {
        return c->ribbonBar();
    }
    return nullptr;
}

/**
 * @brief pannel高度推荐
 * @param fm
 * @param layMode
 * @param pannelTitleHeight
 * @return
 */
int SARibbonPannel::pannelHeightHint(const QFontMetrics& fm, PannelLayoutMode layMode, int pannelTitleHeight)
{
    int textH = fm.lineSpacing();  // 这里用linespace，因为在换行的情况下，行距是不可忽略的，ribbon的大按钮默认是2行
    switch (layMode) {
    case SARibbonPannel::ThreeRowMode: {
        // 5.5=（3*1.6+1） （三行）,1是给panneltitle预留的
        return textH * 4.8 + pannelTitleHeight;
    } break;
    case SARibbonPannel::TwoRowMode: {
        // 3=2*1.6
        return textH * 3.2 + pannelTitleHeight;
    } break;
    default: {
        qWarning() << "unknow SARibbonPannel::PannelLayoutMode:" << layMode;
    }
    }
    return (textH * 4.8 + pannelTitleHeight);
}

/**
 * @brief 重置按钮的尺寸，在布局改变后（尤其高度变更），按钮的尺寸需要手动变更
 */
void SARibbonPannel::resetToolButtonSize()
{
    QList< SARibbonToolButton* > btns = ribbonToolButtons();

    for (SARibbonToolButton* b : qAsConst(btns)) {
        if ((nullptr == b)) {
            continue;
        }
        b->updateRect();
    }
}

bool SARibbonPannel::event(QEvent* e)
{
#if SA_DEBUG_PRINT_EVENT
    if (e->type() != QEvent::Paint) {
        qDebug() << "SARibbonPannel event(" << e->type() << "),name=" << pannelName();
    }
#endif
    // if (SARibbonPannelLayout* lay = pannelLayout()) {
    //     if (lay->isDirty() && e->type() == QEvent::LayoutRequest) {
    //         if (QWidget* parw = parentWidget()) {
    //             if (QLayout* pl = parw->layout()) {
    //                 pl->invalidate();
    //             }
    //         }
    //         lay->m_dirty = false;
    //     }
    // }
    return QWidget::event(e);
}

/**
 * @brief 处理action的事件
 *
 * 这里处理了ActionAdded，ActionChanged，ActionRemoved三个事件
 *
 * ActionAdded时向布局请求，添加action，布局中同时触发了@ref SARibbonPannelLayout::createItem 函数
 * 此函数用于生成窗口，例如QRibbonToolButton
 *
 * ActionChanged时会让布局重新计算尺寸，并向category请求重新布局，有可能category的所有要重新调整尺寸
 *
 * ActionRemoved会移除布局管理的QLayoutItem，并进行内存清理，这时窗口也会隐藏，同时销毁
 *
 * @param e
 * @note 所有action事件都会向category请求重新布局
 *
 */
void SARibbonPannel::actionEvent(QActionEvent* e)
{
    QAction* action             = e->action();
    QWidgetAction* widgetAction = qobject_cast< QWidgetAction* >(action);

    switch (e->type()) {
    case QEvent::ActionAdded: {
        SARibbonPannelLayout* lay = pannelLayout();
        if (nullptr != widgetAction) {
            if (widgetAction->parent() != this) {
                widgetAction->setParent(this);
            }
        }
        int index = layout()->count();
        if (e->before()) {
            // 说明是插入
            index = lay->indexByAction(e->before());
            if (-1 == index) {
                index = layout()->count();  // 找不到的时候就插入到最后
            }
        }
        lay->insertAction(index, action, getActionRowProportionProperty(action));
        // 由于pannel的尺寸发生变化，需要让category也调整
        // if (QWidget* parw = parentWidget()) {
        //     if (QLayout* pl = parw->layout()) {
        //         pl->invalidate();
        //     }
        // }
    } break;

    case QEvent::ActionChanged: {
        // 让布局重新绘制
        layout()->invalidate();

        // updateGeometry();
        // 由于pannel的尺寸发生变化，需要让category也调整
        if (QWidget* parw = parentWidget()) {
            if (QLayout* pl = parw->layout()) {
                pl->invalidate();
            }
            //! 强制发送一个resizeevent，让Category能重绘，如果没有这个函数，发现Category的layout虽然设置了invalidate（标记缓存失效）
            //! 但并没有按顺序在pannel尺寸更新后更新Category的尺寸，导致有些pannel的尺寸识别出现异常
            //! 重打印信息上看，pannel的尺寸有进行更新，category的尺寸也进行了更新，但更新的次数和调用invalidate的次数不一样，需要手动触发ResizeEvent
            //! 尝试过调用QEvent::LayoutRequest没有效果：
            //! @code
            //! QEvent* el = new QEvent(QEvent::LayoutRequest);
            //! QApplication::postEvent(parw, el);
            //! @endcode
            //!
            //! 调用parw->updateGeometry();也没有效果，目前看使用resizeevent是最有效果的
            //!
            QResizeEvent* ersize = new QResizeEvent(parw->size(), QSize());
            QApplication::postEvent(parw, ersize);
        }
    } break;

    case QEvent::ActionRemoved: {
        SARibbonPannelLayout* lay = pannelLayout();
        action->disconnect(this);
        int index = lay->indexByAction(action);
        if (index != -1) {
            QLayoutItem* item = lay->takeAt(index);
            delete item;
        }
        // 由于pannel的尺寸发生变化，需要让category也调整
        // if (QWidget* parw = parentWidget()) {
        //     if (QLayout* pl = parw->layout()) {
        //         pl->invalidate();
        //     }
        // }
    } break;

    default:
        break;
    }
}

/**
 * @brief 追踪字体改变事件，把新字体设置到childwidget中
 * @param e
 */
void SARibbonPannel::changeEvent(QEvent* e)
{
    if (nullptr == e) {
        return;
    }
    if (e->type() == QEvent::FontChange) {
        QFont f                       = font();
        QList< QWidget* > listWidgets = findChildren< QWidget* >();
        for (QWidget* w : listWidgets) {
            w->setFont(f);
        }
        if (QLayout* lay = layout()) {
            lay->invalidate();
        }
        d_ptr->resetTitleLabelFont();
    }
    QWidget::changeEvent(e);
}

/**
 * @brief 获取布局item
 * @return
 */
const QList< SARibbonPannelItem* >& SARibbonPannel::ribbonPannelItem() const
{
    return (pannelLayout()->m_items);
}
