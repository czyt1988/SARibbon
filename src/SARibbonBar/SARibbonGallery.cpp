#include "SARibbonGallery.h"
#include <QIcon>
#include <QApplication>
#define ICON_ARROW_UP QIcon(":/SARibbon/image/resource/ArrowUp.png")
#define ICON_ARROW_DOWN QIcon(":/SARibbon/image/resource/ArrowDown.png")
#define ICON_ARROW_MORE QIcon(":/SARibbon/image/resource/ArrowMore.png")
#include <QResizeEvent>
#include <QDebug>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QLabel>
#include "SARibbonElementManager.h"
#include <QActionGroup>

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
    SARibbonGalleryGroup* mViewportGroup { nullptr };
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
        mPopupWidget   = nullptr;
        mViewportGroup = nullptr;
        mButtonLayout  = new QBoxLayout(QBoxLayout::TopToBottom);
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
#if 0
            popupWidget = new SARibbonMenu(Parent);
#else
            mPopupWidget = new SARibbonGalleryViewport(q_ptr);
#endif
        }
    }

    void setViewPort(SARibbonGalleryGroup* v)
    {
        if (nullptr == mViewportGroup) {
            mViewportGroup = RibbonSubElementFactory->createRibbonGalleryGroup(q_ptr);
            mLayout->addWidget(mViewportGroup, 1);
        }
        mViewportGroup->setRecalcGridSizeBlock(true);
        mViewportGroup->setGalleryGroupStyle(v->galleryGroupStyle());
        mViewportGroup->setDisplayRow(v->displayRow());
        mViewportGroup->setSpacing(v->spacing());
        mViewportGroup->setGridMaximumWidth(v->gridMaximumWidth());
        mViewportGroup->setGridMinimumWidth(v->gridMinimumWidth());
        mViewportGroup->setRecalcGridSizeBlock(false);
        mViewportGroup->recalcGridSize(mViewportGroup->height());
        mViewportGroup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mViewportGroup->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mViewportGroup->setModel(v->model());
        mViewportGroup->show();
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

SARibbonGalleryViewport::SARibbonGalleryViewport(QWidget* parent) : QWidget(parent)
{
    setWindowFlags(Qt::Popup);
    QPalette pl = palette();
    pl.setBrush(QPalette::Window, pl.brush(QPalette::Base));
    setPalette(pl);
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
}

/**
 * @brief 添加窗口不带标题
 * @param w
 */
void SARibbonGalleryViewport::addWidget(QWidget* w)
{
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layout->addWidget(w);
}

/**
 * @brief 添加窗口，带标题
 * @param w
 * @param title
 */
void SARibbonGalleryViewport::addWidget(QWidget* w, const QString& title)
{
    QLabel* label = new QLabel(this);
    label->setText(title);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layout->addWidget(label);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_layout->addWidget(w);
    _widgetToTitleLable[ w ] = label;
}

/**
 * @brief 通过SARibbonGalleryGroup获取对应的标题，用户可以通过此函数设置QLabel的属性
 * @param g
 * @return 如果没有管理group，将返回nullptr
 */
QLabel* SARibbonGalleryViewport::titleLabel(QWidget* w)
{
    return _widgetToTitleLable.value(w, nullptr);
}

/**
 * @brief 移除窗口
 * @param w
 */
void SARibbonGalleryViewport::removeWidget(QWidget* w)
{
    QLabel* label = titleLabel(w);
    if (label) {
        m_layout->removeWidget(label);
    }
    m_layout->removeWidget(w);
}

/**
 * @brief widget的标题改变
 * @param g
 * @param title
 */
void SARibbonGalleryViewport::onTitleChanged(QWidget* w, const QString& title)
{
    QLabel* l = titleLabel(w);
    if (l) {
        l->setText(title);
    }
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

QSize SARibbonGallery::sizeHint() const
{
    return (QSize(100, 62));
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
    setCurrentViewGroup(group);
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
    return (d_ptr->mViewportGroup);
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
    if (d_ptr->mViewportGroup) {
        QScrollBar* vscrollBar = d_ptr->mViewportGroup->verticalScrollBar();
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
    if (d_ptr->mViewportGroup) {
        QScrollBar* vscrollBar = d_ptr->mViewportGroup->verticalScrollBar();
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
    if (nullptr == d_ptr->mPopupWidget) {
        return;
    }
    QSize popupMenuSize = d_ptr->mPopupWidget->sizeHint();
    QPoint start        = mapToGlobal(QPoint(0, 0));

    int width = d_ptr->mViewportGroup->width();  // viewport

    width += qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);  // scrollbar
    d_ptr->mPopupWidget->setGeometry(start.x(), start.y(), width, popupMenuSize.height());
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
    if (d_ptr->mViewportGroup) {
        h = d_ptr->mViewportGroup->height();
        d_ptr->mViewportGroup->recalcGridSize();
    }
    if (d_ptr->mPopupWidget) {
        QLayout* lay = d_ptr->mPopupWidget->layout();
        if (!lay) {
            return;
        }
        int c = lay->count();
        for (int i = 0; i < c; ++i) {
            QLayoutItem* item = lay->itemAt(i);
            if (!item) {
                continue;
            }
            QWidget* w = item->widget();
            if (!w) {
                continue;
            }
            SARibbonGalleryGroup* g = qobject_cast< SARibbonGalleryGroup* >(w);
            if (!g) {
                continue;
            }
            g->recalcGridSize(h);
        }
    }
}

void SARibbonGallery::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
}
