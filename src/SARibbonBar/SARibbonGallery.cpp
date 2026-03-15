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
#include "SARibbonQt5Compat.hpp"
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
            if (!rect().contains(mapFromGlobal(SA::compat::eventGlobalPos(me)))) {
                close();
                return true;  // 防止继续传递
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

/**
 * \if ENGLISH
 * @brief Constructor for SARibbonGallery
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGallery构造函数
 * @param parent 父窗口部件
 * \endif
 */
SARibbonGallery::SARibbonGallery(QWidget* parent) : QFrame(parent), d_ptr(new SARibbonGallery::PrivateData(this))
{
    d_ptr->init();
    setFrameShape(QFrame::Box);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumWidth(200);
}

/**
 * \if ENGLISH
 * @brief Destructor for SARibbonGallery
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGallery析构函数
 * \endif
 */
SARibbonGallery::~SARibbonGallery()
{
}

/**
 * \if ENGLISH
 * @brief Get a blank SARibbonGalleryGroup
 * @return Pointer to the created SARibbonGalleryGroup
 * @details Creates and returns a new empty gallery group. The group is automatically added to the gallery.
 * \endif
 *
 * \if CHINESE
 * @brief 获取一个空白SARibbonGalleryGroup
 * @return 创建的SARibbonGalleryGroup指针
 * @details 创建并返回一个新的空白图库组。该组会自动添加到图库中。
 * \endif
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
/**
 * \if ENGLISH
 * @brief Add an existing SARibbonGalleryGroup to the gallery
 * @param group Pointer to the gallery group to add
 * @details Adds a pre-existing gallery group to the gallery. The group will be displayed in the popup viewport.
 * \endif
 *
 * \if CHINESE
 * @brief 添加一个已存在的SARibbonGalleryGroup到图库
 * @param group 要添加的图库组指针
 * @details 将已存在的图库组添加到图库中。该组将在弹出视口中显示。
 * \endif
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
/**
 * \if ENGLISH
 * @brief Quickly add a set of actions as a gallery group
 * @param title Title for the gallery group
 * @param actions List of actions to add to the group
 * @return Pointer to the created SARibbonGalleryGroup
 * @details Creates a new gallery group with the specified title and adds the provided actions to it.
 * \endif
 *
 * \if CHINESE
 * @brief 快速添加一组动作作为图库组
 * @param title 图库组标题
 * @param actions 要添加到组中的动作列表
 * @return 创建的SARibbonGalleryGroup指针
 * @details 创建一个具有指定标题的新图库组，并将提供的动作添加到其中。
 * \endif
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

/**
 * \if ENGLISH
 * @brief Set the currently displayed gallery group
 * @param group Pointer to the gallery group to display
 * @details Sets which gallery group should be displayed in the main gallery view (not the popup).
 * \endif
 *
 * \if CHINESE
 * @brief 设置当前显示的图库组
 * @param group 要显示的图库组指针
 * @details 设置哪个图库组应该在主图库视图中显示（非弹出窗口）。
 * \endif
 */
void SARibbonGallery::setCurrentViewGroup(SARibbonGalleryGroup* group)
{
    d_ptr->setViewPort(group);
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

/**
 * \if ENGLISH
 * @brief Get the currently displayed gallery group
 * @return Pointer to the current gallery group, or nullptr if none is set
 * \endif
 *
 * \if CHINESE
 * @brief 获取当前显示的图库组
 * @return 当前图库组指针，如果未设置则返回nullptr
 * \endif
 */
SARibbonGalleryGroup* SARibbonGallery::currentViewGroup() const
{
    return (d_ptr->mCurrentViewportGroup);
}

/**
 * @brief 获取弹出窗口
 * @return
 */
/**
 * \if ENGLISH
 * @brief Get the popup viewport widget
 * @return Pointer to the popup viewport, or nullptr if not created yet
 * @details The popup viewport displays all gallery groups when the "more" button is clicked.
 * \endif
 *
 * \if CHINESE
 * @brief 获取弹出视口部件
 * @return 弹出视口指针，如果尚未创建则返回nullptr
 * @details 弹出视口在点击"更多"按钮时显示所有图库组。
 * \endif
 */
SARibbonGalleryViewport* SARibbonGallery::getPopupViewPort() const
{
    return d_ptr->mPopupWidget;
}

/**
 * @brief 设置最右边三个控制按钮的最大宽度（默认15）
 * @param w
 */
/**
 * \if ENGLISH
 * @brief Set the maximum width for gallery control buttons
 * @param w Maximum width in pixels
 * @details Sets the maximum width for the up, down, and more buttons on the gallery.
 * \endif
 *
 * \if CHINESE
 * @brief 设置图库控制按钮的最大宽度
 * @param w 最大宽度（像素）
 * @details 设置图库上、下和更多按钮的最大宽度。
 * \endif
 */
void SARibbonGallery::setGalleryButtonMaximumWidth(int w)
{
    SARibbonGallery::PrivateData::sGalleryButtonMaximumWidth = w;
}

/**
 * @brief 上翻页
 */
/**
 * \if ENGLISH
 * @brief Scroll down one page in the current gallery group
 * @details Scrolls the current gallery group view down by one page (if a scrollbar is present).
 * \endif
 *
 * \if CHINESE
 * @brief 在当前图库组中向下滚动一页
 * @details 将当前图库组视图向下滚动一页（如果存在滚动条）。
 * \endif
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
/**
 * \if ENGLISH
 * @brief Scroll up one page in the current gallery group
 * @details Scrolls the current gallery group view up by one page (if a scrollbar is present).
 * \endif
 *
 * \if CHINESE
 * @brief 在当前图库组中向上滚动一页
 * @details 将当前图库组视图向上滚动一页（如果存在滚动条）。
 * \endif
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
/**
 * \if ENGLISH
 * @brief Show the popup viewport with all gallery groups
 * @details Displays the popup viewport containing all gallery groups when the "more" button is clicked.
 *          Users can override this function to implement custom popup behavior.
 * \endif
 *
 * \if CHINESE
 * @brief 显示包含所有图库组的弹出视口
 * @details 当点击"更多"按钮时，显示包含所有图库组的弹出视口。
 *          用户可以重写此函数以实现自定义的弹出行为。
 * \endif
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

/**
 * \if ENGLISH
 * @brief Slot called when an item in a gallery group is clicked
 * @param index Model index of the clicked item
 * @details Handles item click events from gallery groups and triggers the corresponding action.
 * \endif
 *
 * \if CHINESE
 * @brief 当图库组中的项目被点击时调用的槽函数
 * @param index 被点击项目的模型索引
 * @details 处理来自图库组的项目点击事件并触发相应的动作。
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Slot called when an action is triggered from the gallery
 * @param action Pointer to the triggered action
 * @details Handles action trigger events and closes the popup viewport if it's open.
 * \endif
 *
 * \if CHINESE
 * @brief 当从图库触发动作时调用的槽函数
 * @param action 被触发动作的指针
 * @details 处理动作触发事件，并在弹出视口打开时关闭它。
 * \endif
 */
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

/**
 * \if ENGLISH
 * @brief Handle resize events for the gallery
 * @param event Resize event
 * @details Updates the layout of gallery groups when the gallery is resized.
 * \endif
 *
 * \if CHINESE
 * @brief 处理图库的调整大小事件
 * @param event 调整大小事件
 * @details 在图库调整大小时更新图库组的布局。
 * \endif
 */
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
