#include "SARibbonGallery.h"
#include <QIcon>
#include <QApplication>
#include <QResizeEvent>
#include <QDebug>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QLabel>
#include <QSizeGrip>
#include <QActionGroup>
#include <QScreen>
#include "SARibbonElementManager.h"

#define ICON_ARROW_UP QIcon(":/SARibbon/image/resource/ArrowUp.png")
#define ICON_ARROW_DOWN QIcon(":/SARibbon/image/resource/ArrowDown.png")
#define ICON_ARROW_MORE QIcon(":/SARibbon/image/resource/ArrowMore.png")
/**
 * @brief The SARibbonGalleryPrivate class
 */
class SARibbonGallery::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonGallery)
public:
    static int sGalleryButtonMaximumWidth;
    SARibbonGalleryButton* mButtonUp { nullptr };
    SARibbonGalleryButton* mButtonDown { nullptr };
    SARibbonGalleryButton* mButtonMore { nullptr };
#if 0
    SARibbonMenu *popupWidget;
#else
    SARibbonGalleryViewport* mPopupWidget { nullptr };
#endif
    SARibbonGalleryGroup* mCurrentViewportGroup { nullptr };
    QBoxLayout* mButtonLayout { nullptr };
    QBoxLayout* mLayout { nullptr };
    PrivateData(SARibbonGallery* p) : q_ptr(p)
    {
    }

    void init()
    {
        mButtonUp   = new SARibbonGalleryButton(q_ptr);
        mButtonDown = new SARibbonGalleryButton(q_ptr);
        mButtonMore = new SARibbonGalleryButton(q_ptr);
        mButtonUp->setToolButtonStyle(Qt::ToolButtonIconOnly);
        mButtonDown->setToolButtonStyle(Qt::ToolButtonIconOnly);
        mButtonMore->setToolButtonStyle(Qt::ToolButtonIconOnly);
        mButtonUp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        mButtonDown->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        mButtonMore->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        mButtonUp->setObjectName(QStringLiteral("SARibbonGalleryButtonUp"));
        mButtonDown->setObjectName(QStringLiteral("SARibbonGalleryButtonDown"));
        mButtonMore->setObjectName(QStringLiteral("SARibbonGalleryButtonMore"));
        mButtonUp->setMaximumWidth(sGalleryButtonMaximumWidth);
        mButtonDown->setMaximumWidth(sGalleryButtonMaximumWidth);
        mButtonMore->setMaximumWidth(sGalleryButtonMaximumWidth);
        mButtonUp->setIcon(ICON_ARROW_UP);
        mButtonDown->setIcon(ICON_ARROW_DOWN);
        mButtonMore->setIcon(ICON_ARROW_MORE);
        q_ptr->connect(mButtonUp, &QAbstractButton::clicked, q_ptr, &SARibbonGallery::pageUp);
        q_ptr->connect(mButtonDown, &QAbstractButton::clicked, q_ptr, &SARibbonGallery::pageDown);
        q_ptr->connect(mButtonMore, &QAbstractButton::clicked, q_ptr, &SARibbonGallery::showMoreDetail);
        // 信号转发
        q_ptr->connect(q_ptr, &SARibbonGallery::triggered, q_ptr, &SARibbonGallery::onTriggered);
        mPopupWidget          = nullptr;
        mCurrentViewportGroup = nullptr;
        mButtonLayout         = new QBoxLayout(QBoxLayout::TopToBottom);
        mButtonLayout->setSpacing(0);
        mButtonLayout->setContentsMargins(0, 0, 0, 0);
        mButtonLayout->addWidget(mButtonUp);
        mButtonLayout->addWidget(mButtonDown);
        mButtonLayout->addWidget(mButtonMore);
        mLayout = new QBoxLayout(QBoxLayout::RightToLeft);
        mLayout->setSpacing(0);
        mLayout->setContentsMargins(0, 0, 0, 0);
        mLayout->addLayout(mButtonLayout);
        mLayout->addStretch();
        q_ptr->setLayout(mLayout);
    }

    bool isValid() const
    {
        return (q_ptr != nullptr);
    }

    void createPopupWidget()
    {
        if (nullptr == mPopupWidget) {
            mPopupWidget = new SARibbonGalleryViewport(q_ptr);
        }
    }

    void setViewPort(const SARibbonGalleryGroup* v)
    {
        if (nullptr == mCurrentViewportGroup) {
            mCurrentViewportGroup = RibbonSubElementFactory->createRibbonGalleryGroup(q_ptr);
            mLayout->addWidget(mCurrentViewportGroup, 1);
        }

        mCurrentViewportGroup->setGalleryGroupStyle(v->galleryGroupStyle());
        mCurrentViewportGroup->setDisplayRow(v->displayRow());
        mCurrentViewportGroup->setSpacing(v->spacing());
        mCurrentViewportGroup->setGridMaximumWidth(v->gridMaximumWidth());
        mCurrentViewportGroup->setGridMinimumWidth(v->gridMinimumWidth());

        mCurrentViewportGroup->recalcGridSize(mCurrentViewportGroup->height());
        mCurrentViewportGroup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mCurrentViewportGroup->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mCurrentViewportGroup->setModel(v->model());
        mCurrentViewportGroup->show();
    }
};

// 静态变量初始化

/**
 * @brief SARibbonGalleryPrivate::sGalleryButtonMaximumWidth
 */
int SARibbonGallery::PrivateData::sGalleryButtonMaximumWidth = 15;

//===================================================
// SARibbonGalleryButton
//===================================================

SARibbonGalleryButton::SARibbonGalleryButton(QWidget* parent) : QToolButton(parent)
{
}

SARibbonGalleryButton::~SARibbonGalleryButton()
{
}
//===================================================
// SARibbonGalleryViewport
//===================================================

SARibbonGalleryViewport::SARibbonGalleryViewport(QWidget* parent) : QScrollArea(parent)
{
    setWindowFlags(Qt::Popup);
    setAttribute(Qt::WA_ShowWithoutActivating);
    m_contentWidget = new QWidget(this);
    m_layout        = new QVBoxLayout(m_contentWidget);
    m_sizeGrip      = new QSizeGrip(this);
    // 配置内容窗口
    m_contentWidget->setObjectName("contentWidget");
    m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(1, 1, 1, 20);  // 底部预留手柄空间
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 配置滚动区域
    setWidget(m_contentWidget);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 背景色
    QPalette pal = palette();
    pal.setBrush(QPalette::Window, pal.brush(QPalette::Base));
    setPalette(pal);
    m_contentWidget->setPalette(pal);  // 确保视口背景一致
    m_contentWidget->setAutoFillBackground(true);

    // 配置尺寸手柄
    m_sizeGrip->setFixedSize(16, 16);
    m_sizeGrip->setCursor(Qt::SizeFDiagCursor);

    // setCornerWidget(m_sizeGrip);会在滚动条隐藏时也隐藏，导致非滚动条状态无法改变尺寸
    m_sizeGrip->setParent(viewport());  // 手柄挂在 viewport 上
    m_sizeGrip->setFixedSize(16, 16);
    m_sizeGrip->setCursor(Qt::SizeFDiagCursor);
    m_sizeGrip->raise();  // 保证在最上层
}

/**
 * @brief 添加窗口不带标题
 * @param w
 */
void SARibbonGalleryViewport::addWidget(QWidget* w)
{
    if (!w) {
        return;
    }
    w->setParent(m_contentWidget);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    m_layout->addWidget(w);
    // 确保新添加的部件可见
    QCoreApplication::postEvent(this, new QEvent(QEvent::LayoutRequest));
}

/**
 * @brief 添加窗口，带标题
 * @param w
 * @param title
 */
void SARibbonGalleryViewport::addWidget(QWidget* w, const QString& title)
{
    if (!w) {
        return;
    }

    // 创建标题标签
    QLabel* label = new QLabel(title, m_contentWidget);
    label->setObjectName("titleLabel");
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    // 添加到布局
    if (w->parentWidget() != m_contentWidget) {
        w->setParent(m_contentWidget);
    }
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_layout->addWidget(label);
    m_layout->addWidget(w);

    // 记录映射关系
    m_widgetToTitleLabel[ w ] = label;

    // 确保新添加的部件可见
    QCoreApplication::postEvent(this, new QEvent(QEvent::LayoutRequest));
}

/**
 * @brief 通过SARibbonGalleryGroup获取对应的标题，用户可以通过此函数设置QLabel的属性
 * @param g
 * @return 如果没有管理group，将返回nullptr
 */
QLabel* SARibbonGalleryViewport::titleLabel(QWidget* w)
{
    return m_widgetToTitleLabel.value(w, nullptr);
}

/**
 * @brief 移除窗口
 * @param w
 */
void SARibbonGalleryViewport::removeWidget(QWidget* w)
{
    if (!w) {
        return;
    }

    // 移除标题标签（如果存在）
    if (QLabel* label = m_widgetToTitleLabel.take(w)) {
        m_layout->removeWidget(label);
        label->deleteLater();
    }

    // 移除主窗口
    m_layout->removeWidget(w);
    w->setParent(nullptr);  // 断开父子关系，由调用者管理

    // 触发布局更新
    QCoreApplication::postEvent(this, new QEvent(QEvent::LayoutRequest));
}

QList< SARibbonGalleryGroup* > SARibbonGalleryViewport::galleryGroupList() const
{
    return m_contentWidget->findChildren< SARibbonGalleryGroup* >(QString(), Qt::FindDirectChildrenOnly);
}

int SARibbonGalleryViewport::galleryHeight() const
{
    if (parent()) {
        if (SARibbonGallery* gallery = qobject_cast< SARibbonGallery* >(parent())) {
            return gallery->height();
        }
    }
    return 40;
}

/**
 * @brief 根据宽度计算高度推荐值
 * @param w
 * @return
 */
int SARibbonGalleryViewport::heightHintForWidth(int w) const
{
    const QList< QWidget* > cws = m_contentWidget->findChildren< QWidget* >(QString(), Qt::FindDirectChildrenOnly);
    int h                       = 0;
    for (QWidget* widget : cws) {
        if (widget->hasHeightForWidth()) {
            h += widget->heightForWidth(w);
        } else {
            h += widget->sizeHint().height();
        }
    }
    QMargins mag = m_layout->contentsMargins();
    // 最后加上margin
    h += mag.top();
    h += mag.bottom();
    h += frameWidth() * 2;
    // 再加2是为了留2px余量避免显示滚动条
    return h + 2;
}


/**
 * @brief widget的标题改变
 * @param g
 * @param title
 */
void SARibbonGalleryViewport::onTitleChanged(QWidget* w, const QString& title)
{
    if (QLabel* label = titleLabel(w)) {
        label->setText(title);
    }
}

bool SARibbonGalleryViewport::eventFilter(QObject* o, QEvent* e)
{
    if (e->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast< QMouseEvent* >(e);
        if (isVisible()) {  // 一定要加是否可见
            if (!rect().contains(mapFromGlobal(me->globalPos()))) {
                close();
                return true;  // 吃掉事件，防止继续传递
            }
        }
    }
    return QScrollArea::eventFilter(o, e);
}

void SARibbonGalleryViewport::showEvent(QShowEvent* e)
{
    QScrollArea::showEvent(e);
    qApp->installEventFilter(this);  // 监听整个应用的所有事件
}

void SARibbonGalleryViewport::hideEvent(QHideEvent* e)
{
    QScrollArea::hideEvent(e);
    qApp->removeEventFilter(this);  // 隐藏后不监听,避免监听太多
}

void SARibbonGalleryViewport::resizeEvent(QResizeEvent* e)
{
    QScrollArea::resizeEvent(e);
    const int gripSize = 16;
    const int x        = viewport()->width() - gripSize;
    const int y        = viewport()->height() - gripSize;
    m_sizeGrip->move(x, y);
    m_sizeGrip->raise();  // 再保险一次
}

//===================================================
// SARibbonGallery
//===================================================

SARibbonGallery::SARibbonGallery(QWidget* parent) : QFrame(parent), d_ptr(new SARibbonGallery::PrivateData(this))
{
    d_ptr->init();
    setFrameShape(QFrame::Box);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumWidth(200);
}

SARibbonGallery::~SARibbonGallery()
{
}


/**
 * @brief 获取一个空白SARibbonGalleryGroup
 * @return
 */
SARibbonGalleryGroup* SARibbonGallery::addGalleryGroup()
{
    SARibbonGalleryGroup* group = RibbonSubElementFactory->createRibbonGalleryGroup(this);
    addGalleryGroup(group);
    return (group);
}

/**
 * @brief 添加一组SARibbonGalleryGroup
 * @param group
 */
void SARibbonGallery::addGalleryGroup(SARibbonGalleryGroup* group)
{
    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    SARibbonGalleryViewport* viewport = ensureGetPopupViewPort();
    viewport->addWidget(group, group->groupTitle());
    connect(group, &QAbstractItemView::clicked, this, &SARibbonGallery::onItemClicked);
    connect(group, &SARibbonGalleryGroup::groupTitleChanged, this, [ group, viewport ](const QString& t) {
        viewport->onTitleChanged(group, t);
    });
    connect(group, &SARibbonGalleryGroup::triggered, this, &SARibbonGallery::triggered);
    connect(group, &SARibbonGalleryGroup::hovered, this, &SARibbonGallery::hovered);
    if (!currentViewGroup()) {
        // 当前没有viewgroup才设置
        setCurrentViewGroup(group);
    }
}

/**
 * @brief 添加一组actions
 * @param title actions组的名字
 * @param actions
 * @return 返回SARibbonGalleryGroup，用户可以通过修改SARibbonGalleryGroup属性控制其显示方法
 */
SARibbonGalleryGroup* SARibbonGallery::addCategoryActions(const QString& title, QList< QAction* > actions)
{
    SARibbonGalleryGroup* group = RibbonSubElementFactory->createRibbonGalleryGroup(this);
    if (!title.isEmpty()) {
        group->setGroupTitle(title);
    }
    group->addActionItemList(actions);
    addGalleryGroup(group);
    return (group);
}

void SARibbonGallery::setCurrentViewGroup(SARibbonGalleryGroup* group)
{
    d_ptr->setViewPort(group);
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

SARibbonGalleryGroup* SARibbonGallery::currentViewGroup() const
{
    return (d_ptr->mCurrentViewportGroup);
}

/**
 * @brief 获取弹出窗口
 * @return
 */
SARibbonGalleryViewport* SARibbonGallery::getPopupViewPort() const
{
    return d_ptr->mPopupWidget;
}

/**
 * @brief 设置最右边三个控制按钮的最大宽度（默认15）
 * @param w
 */
void SARibbonGallery::setGalleryButtonMaximumWidth(int w)
{
    SARibbonGallery::PrivateData::sGalleryButtonMaximumWidth = w;
}

/**
 * @brief 上翻页
 */
void SARibbonGallery::pageDown()
{
    if (d_ptr->mCurrentViewportGroup) {
        QScrollBar* vscrollBar = d_ptr->mCurrentViewportGroup->verticalScrollBar();
        int v                  = vscrollBar->value();
        v += vscrollBar->singleStep();
        vscrollBar->setValue(v);
    }
}

/**
 * @brief 下翻页
 */
void SARibbonGallery::pageUp()
{
    if (d_ptr->mCurrentViewportGroup) {
        QScrollBar* vscrollBar = d_ptr->mCurrentViewportGroup->verticalScrollBar();
        int v                  = vscrollBar->value();
        v -= vscrollBar->singleStep();
        vscrollBar->setValue(v);
    }
}

/**
 * @brief 显示更多触发，默认弹出内部管理的SARibbonGalleryViewport，用户可重载此函数实现自定义的弹出
 */
void SARibbonGallery::showMoreDetail()
{
    if (Q_UNLIKELY(!d_ptr->mPopupWidget))
        return;

    const QPoint start = mapToGlobal(QPoint(0, 0));
    const QScreen* scr = QGuiApplication::screenAt(start);
    if (!scr)  // 罕见情况：跨屏拖动后原屏幕消失
        scr = QGuiApplication::primaryScreen();
    const QRect avail = scr->availableGeometry();  // 去掉任务栏/Dock

    int w = width();
    if (d_ptr->mCurrentViewportGroup)
        w = d_ptr->mCurrentViewportGroup->width();
    w += style()->pixelMetric(QStyle::PM_ScrollBarExtent);

    /* ---- 宽度不超过屏幕右边缘 ---- */
    w = qMin(w, avail.right() - start.x() + 1);
    w = qMax(w, 1);  // 保险：至少 1 px

    /* ---- 计算期望高度 ---- */
    int h = d_ptr->mPopupWidget->heightHintForWidth(w);

    /* ---- 高度不超过屏幕底部 ---- */
    int maxH = avail.bottom() - start.y() + 1;
    if (h > maxH) {
        h = maxH;
    }

    d_ptr->mPopupWidget->setGeometry(start.x(), start.y(), w, h);
    d_ptr->mPopupWidget->show();
}

void SARibbonGallery::onItemClicked(const QModelIndex& index)
{
    QObject* obj                = sender();
    SARibbonGalleryGroup* group = qobject_cast< SARibbonGalleryGroup* >(obj);
    if (group) {
        setCurrentViewGroup(group);
        SARibbonGalleryGroup* curGroup = currentViewGroup();
        curGroup->scrollTo(index);
        curGroup->setCurrentIndex(index);
    }
}

void SARibbonGallery::onTriggered(QAction* action)
{
    Q_UNUSED(action);
    // 点击后关闭弹出窗口
    if (d_ptr->mPopupWidget) {
        if (d_ptr->mPopupWidget->isVisible()) {
            d_ptr->mPopupWidget->hide();
        }
    }
}

SARibbonGalleryViewport* SARibbonGallery::ensureGetPopupViewPort()
{
    if (nullptr == d_ptr->mPopupWidget) {
        d_ptr->createPopupWidget();
    }
    return (d_ptr->mPopupWidget);
}

void SARibbonGallery::resizeEvent(QResizeEvent* event)
{
    QFrame::resizeEvent(event);
    // 对SARibbonGalleryViewport所有SARibbonGalleryGroup重置尺寸
    int h = layout()->contentsRect().height();
    if (d_ptr->mCurrentViewportGroup) {
        h = d_ptr->mCurrentViewportGroup->height();
        d_ptr->mCurrentViewportGroup->recalcGridSize();
    }
    // 尺寸变化后必须重新计算gridsize
    if (d_ptr->mPopupWidget) {
        const QList< SARibbonGalleryGroup* > groups = d_ptr->mPopupWidget->galleryGroupList();
        for (SARibbonGalleryGroup* group : groups) {
            group->recalcGridSize(h);
        }
    }
}
