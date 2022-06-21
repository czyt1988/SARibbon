#include "SARibbonGallery.h"
#include "SARibbonControlButton.h"
#include <QIcon>
#include <QApplication>
#define ICON_ARROW_UP QIcon(":/image/resource/ArrowUp.png")
#define ICON_ARROW_DOWN QIcon(":/image/resource/ArrowDown.png")
#define ICON_ARROW_MORE QIcon(":/image/resource/ArrowMore.png")
#include "SARibbonMenu.h"
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
class SARibbonGalleryPrivate
{
public:
    static int sGalleryButtonMaximumWidth;
    SARibbonControlButton* buttonUp;
    SARibbonControlButton* buttonDown;
    SARibbonControlButton* buttonMore;
    SARibbonGallery* Parent;
#if 0
    SARibbonMenu *popupWidget;
#else
    SARibbonGalleryViewport* popupWidget;
#endif
    SARibbonGalleryGroup* viewportGroup;
    QBoxLayout* btnLayout;
    QBoxLayout* layout;
    SARibbonGalleryPrivate() : Parent(nullptr)
    {
    }

    void init(SARibbonGallery* parent)
    {
        Parent     = parent;
        buttonUp   = new SARibbonControlButton(parent);
        buttonDown = new SARibbonControlButton(parent);
        buttonMore = new SARibbonControlButton(parent);
        buttonUp->setToolButtonStyle(Qt::ToolButtonIconOnly);
        buttonDown->setToolButtonStyle(Qt::ToolButtonIconOnly);
        buttonMore->setToolButtonStyle(Qt::ToolButtonIconOnly);
        buttonUp->setObjectName(QStringLiteral("SARibbonGalleryButtonUp"));
        buttonDown->setObjectName(QStringLiteral("SARibbonGalleryButtonDown"));
        buttonMore->setObjectName(QStringLiteral("SARibbonGalleryButtonMore"));
        buttonUp->setMaximumWidth(sGalleryButtonMaximumWidth);
        buttonDown->setMaximumWidth(sGalleryButtonMaximumWidth);
        buttonMore->setMaximumWidth(sGalleryButtonMaximumWidth);
        buttonUp->setIcon(ICON_ARROW_UP);
        buttonDown->setIcon(ICON_ARROW_DOWN);
        buttonMore->setIcon(ICON_ARROW_MORE);
        Parent->connect(buttonUp, &QAbstractButton::clicked, Parent, &SARibbonGallery::pageUp);
        Parent->connect(buttonDown, &QAbstractButton::clicked, Parent, &SARibbonGallery::pageDown);
        Parent->connect(buttonMore, &QAbstractButton::clicked, Parent, &SARibbonGallery::showMoreDetail);
        //信号转发
        Parent->connect(Parent, &SARibbonGallery::triggered, Parent, &SARibbonGallery::onTriggered);
        popupWidget   = nullptr;
        viewportGroup = nullptr;
        btnLayout     = new QBoxLayout(QBoxLayout::TopToBottom);
        btnLayout->setSpacing(0);
        btnLayout->setContentsMargins(0, 0, 0, 0);
        btnLayout->addWidget(buttonUp);
        btnLayout->addWidget(buttonDown);
        btnLayout->addWidget(buttonMore);
        layout = new QBoxLayout(QBoxLayout::RightToLeft);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addLayout(btnLayout);
        layout->addStretch();
        parent->setLayout(layout);
    }

    bool isValid() const
    {
        return (Parent != nullptr);
    }

    void createPopupWidget()
    {
        if (nullptr == popupWidget) {
#if 0
            popupWidget = new SARibbonMenu(Parent);
#else
            popupWidget = new SARibbonGalleryViewport(Parent);
#endif
        }
    }

    void setViewPort(SARibbonGalleryGroup* v)
    {
        if (nullptr == viewportGroup) {
            viewportGroup = RibbonSubElementDelegate->createRibbonGalleryGroup(Parent);
            layout->addWidget(viewportGroup, 1);
        }
        viewportGroup->setRecalcGridSizeBlock(true);
        viewportGroup->setGalleryGroupStyle(v->getGalleryGroupStyle());
        viewportGroup->setDisplayRow(v->getDisplayRow());
        viewportGroup->setSpacing(v->spacing());
        viewportGroup->setGridMaximumWidth(v->getGridMaximumWidth());
        viewportGroup->setGridMinimumWidth(v->getGridMinimumWidth());
        viewportGroup->setRecalcGridSizeBlock(false);
        viewportGroup->recalcGridSize(viewportGroup->height());
        viewportGroup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        viewportGroup->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        viewportGroup->setModel(v->model());
        viewportGroup->show();
    }
};

//静态变量初始化

/**
 * @brief SARibbonGalleryPrivate::sGalleryButtonMaximumWidth
 */
int SARibbonGalleryPrivate::sGalleryButtonMaximumWidth = 15;

//////////////////////////////////////////////

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
QLabel* SARibbonGalleryViewport::getWidgetTitleLabel(QWidget* w)
{
    return _widgetToTitleLable.value(w, nullptr);
}

/**
 * @brief 移除窗口
 * @param w
 */
void SARibbonGalleryViewport::removeWidget(QWidget* w)
{
    QLabel* label = getWidgetTitleLabel(w);
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
void SARibbonGalleryViewport::widgetTitleChanged(QWidget* w, const QString& title)
{
    QLabel* l = getWidgetTitleLabel(w);
    if (l) {
        l->setText(title);
    }
}

//////////////////////////////////////////////

SARibbonGallery::SARibbonGallery(QWidget* parent) : QFrame(parent), m_d(new SARibbonGalleryPrivate)
{
    m_d->init(this);
    setFrameShape(QFrame::Box);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumWidth(200);
}

SARibbonGallery::~SARibbonGallery()
{
    delete m_d;
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
    SARibbonGalleryGroup* group = RibbonSubElementDelegate->createRibbonGalleryGroup(this);
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
    viewport->addWidget(group, group->getGroupTitle());
    connect(group, &QAbstractItemView::clicked, this, &SARibbonGallery::onItemClicked);
    connect(group, &SARibbonGalleryGroup::groupTitleChanged, this, [ group, viewport ](const QString& t) {
        viewport->widgetTitleChanged(group, t);
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
    SARibbonGalleryGroup* group = RibbonSubElementDelegate->createRibbonGalleryGroup(this);
    if (!title.isEmpty()) {
        group->setGroupTitle(title);
    }
    group->addActionItemList(actions);
    addGalleryGroup(group);
    return (group);
}

void SARibbonGallery::setCurrentViewGroup(SARibbonGalleryGroup* group)
{
    m_d->setViewPort(group);
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

SARibbonGalleryGroup* SARibbonGallery::currentViewGroup() const
{
    return (m_d->viewportGroup);
}

/**
 * @brief 获取弹出窗口
 * @return
 */
SARibbonGalleryViewport* SARibbonGallery::getPopupViewPort() const
{
    return m_d->popupWidget;
}

/**
 * @brief 设置最右边三个控制按钮的最大宽度（默认15）
 * @param w
 */
void SARibbonGallery::setGalleryButtonMaximumWidth(int w)
{
    SARibbonGalleryPrivate::sGalleryButtonMaximumWidth = w;
}

/**
 * @brief 上翻页
 */
void SARibbonGallery::pageDown()
{
    if (m_d->viewportGroup) {
        QScrollBar* vscrollBar = m_d->viewportGroup->verticalScrollBar();
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
    if (m_d->viewportGroup) {
        QScrollBar* vscrollBar = m_d->viewportGroup->verticalScrollBar();
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
    if (nullptr == m_d->popupWidget) {
        return;
    }
    QSize popupMenuSize = m_d->popupWidget->sizeHint();
    QPoint start        = mapToGlobal(QPoint(0, 0));

    int width = m_d->viewportGroup->width();  // viewport

    width += qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);  // scrollbar
    m_d->popupWidget->setGeometry(start.x(), start.y(), width, popupMenuSize.height());
    m_d->popupWidget->show();
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
    //点击后关闭弹出窗口
    if (m_d->popupWidget) {
        if (m_d->popupWidget->isVisible()) {
            m_d->popupWidget->hide();
        }
    }
}

SARibbonGalleryViewport* SARibbonGallery::ensureGetPopupViewPort()
{
    if (nullptr == m_d->popupWidget) {
        m_d->createPopupWidget();
    }
    return (m_d->popupWidget);
}

void SARibbonGallery::resizeEvent(QResizeEvent* event)
{
    QFrame::resizeEvent(event);
    //对SARibbonGalleryViewport所有SARibbonGalleryGroup重置尺寸
    int h = layout()->contentsRect().height();
    if (m_d->viewportGroup) {
        h = m_d->viewportGroup->height();
        m_d->viewportGroup->recalcGridSize();
    }
    if (m_d->popupWidget) {
        QLayout* lay = m_d->popupWidget->layout();
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
