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
    QActionGroup* actionGroup;  //所有gallery管理的actions都由这个actiongroup管理
    QBoxLayout* btnLayout;
    QBoxLayout* layout;
    SARibbonGalleryPrivate() : Parent(nullptr)
    {
    }

    void init(SARibbonGallery* parent)
    {
        Parent      = parent;
        buttonUp    = new SARibbonControlButton(parent);
        buttonDown  = new SARibbonControlButton(parent);
        buttonMore  = new SARibbonControlButton(parent);
        actionGroup = new QActionGroup(parent);
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
        Parent->connect(buttonUp, &QAbstractButton::clicked, Parent, &SARibbonGallery::onPageUp);
        Parent->connect(buttonDown, &QAbstractButton::clicked, Parent, &SARibbonGallery::onPageDown);
        Parent->connect(buttonMore, &QAbstractButton::clicked, Parent, &SARibbonGallery::onShowMoreDetail);
        Parent->connect(actionGroup, &QActionGroup::triggered, Parent, &SARibbonGallery::onTriggered);
        //信号转发
        Parent->connect(actionGroup, &QActionGroup::triggered, Parent, &SARibbonGallery::triggered);
        Parent->connect(actionGroup, &QActionGroup::hovered, Parent, &SARibbonGallery::hovered);
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

void SARibbonGalleryViewport::addWidget(QWidget* w)
{
    m_layout->addWidget(w);
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
    // return (QSize(232, RibbonSubElementStyleOpt.galleryFixedHeight));
    return (QSize(0, 0));
}

SARibbonGalleryGroup* SARibbonGallery::addGalleryGroup()
{
    SARibbonGalleryGroup* group = RibbonSubElementDelegate->createRibbonGalleryGroup(this);

    addGalleryGroup(group);
    return (group);
}

void SARibbonGallery::addGalleryGroup(SARibbonGalleryGroup* group)
{
    SARibbonGalleryViewport* viewport = ensureGetPopupViewPort();
    SARibbonGalleryGroupModel* model  = new SARibbonGalleryGroupModel(group);

    group->setModel(model);
    viewport->addWidget(group);
    if (nullptr == m_d->viewportGroup) {
        setCurrentViewGroup(group);
    }
    connect(group, &QAbstractItemView::clicked, this, &SARibbonGallery::onItemClicked);
}

SARibbonGalleryGroup* SARibbonGallery::addCategoryActions(const QString& title, QList< QAction* > actions)
{
    SARibbonGalleryGroup* group      = RibbonSubElementDelegate->createRibbonGalleryGroup(this);
    SARibbonGalleryGroupModel* model = new SARibbonGalleryGroupModel(group);

    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    group->setModel(model);
    if (!title.isEmpty()) {
        group->setGroupTitle(title);
    }
    for (QAction* a : actions) {
        m_d->actionGroup->addAction(a);
    }
    group->addActionItemList(actions);
    connect(group, &QAbstractItemView::clicked, this, &SARibbonGallery::onItemClicked);
    SARibbonGalleryViewport* viewport = ensureGetPopupViewPort();

    viewport->addWidget(group);
    setCurrentViewGroup(group);
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

QActionGroup* SARibbonGallery::getActionGroup() const
{
    return (m_d->actionGroup);
}

/**
 * @brief 设置最右边三个控制按钮的最大宽度（默认15）
 * @param w
 */
void SARibbonGallery::setGalleryButtonMaximumWidth(int w)
{
    SARibbonGalleryPrivate::sGalleryButtonMaximumWidth = w;
}

void SARibbonGallery::onPageDown()
{
    if (m_d->viewportGroup) {
        QScrollBar* vscrollBar = m_d->viewportGroup->verticalScrollBar();
        int v                  = vscrollBar->value();
        v += vscrollBar->singleStep();
        vscrollBar->setValue(v);
    }
}

void SARibbonGallery::onPageUp()
{
    if (m_d->viewportGroup) {
        QScrollBar* vscrollBar = m_d->viewportGroup->verticalScrollBar();
        int v                  = vscrollBar->value();
        v -= vscrollBar->singleStep();
        vscrollBar->setValue(v);
    }
}

void SARibbonGallery::onShowMoreDetail()
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

void SARibbonGallery::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
}
