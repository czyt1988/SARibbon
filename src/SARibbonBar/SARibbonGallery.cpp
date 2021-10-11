#include "SARibbonGallery.h"
#include "SARibbonControlButton.h"
#include <QIcon>
#include <QApplication>
#define ICON_ARROW_UP		QIcon(":/image/resource/ArrowUp.png")
#define ICON_ARROW_DOWN		QIcon(":/image/resource/ArrowDown.png")
#define ICON_ARROW_MORE		QIcon(":/image/resource/ArrowMore.png")
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
    SARibbonControlButton *buttonUp;
    SARibbonControlButton *buttonDown;
    SARibbonControlButton *buttonMore;
    SARibbonGallery *Parent;
#if 0
    SARibbonMenu *popupWidget;
#else
    RibbonGalleryViewport *popupWidget;
#endif
    SARibbonGalleryGroup *viewportGroup;
    QActionGroup *actionGroup;//所有gallery管理的actions都由这个actiongroup管理
    QBoxLayout *btnLayout;
    QBoxLayout *layout;
    SARibbonGalleryPrivate() : Parent(nullptr)
    {
    }


    void init(SARibbonGallery *parent)
    {
        Parent = parent;
        buttonUp = new SARibbonControlButton(parent);
        buttonDown = new SARibbonControlButton(parent);
        buttonMore = new SARibbonControlButton(parent);
        actionGroup = new QActionGroup(parent);
        buttonUp->setToolButtonStyle(Qt::ToolButtonIconOnly);
        buttonDown->setToolButtonStyle(Qt::ToolButtonIconOnly);
        buttonMore->setToolButtonStyle(Qt::ToolButtonIconOnly);
        buttonUp->setObjectName(QStringLiteral("SARibbonGalleryButtonUp"));
        buttonDown->setObjectName(QStringLiteral("SARibbonGalleryButtonDown"));
        buttonMore->setObjectName(QStringLiteral("SARibbonGalleryButtonMore"));
        buttonUp->setMaximumWidth(RibbonSubElementStyleOpt.galleryButtonMaximumWidth);
        buttonDown->setMaximumWidth(RibbonSubElementStyleOpt.galleryButtonMaximumWidth);
        buttonMore->setMaximumWidth(RibbonSubElementStyleOpt.galleryButtonMaximumWidth);
        buttonUp->setIcon(ICON_ARROW_UP);
        buttonDown->setIcon(ICON_ARROW_DOWN);
        buttonMore->setIcon(ICON_ARROW_MORE);
        Parent->connect(buttonUp, &QAbstractButton::clicked
            , Parent, &SARibbonGallery::onPageUp);
        Parent->connect(buttonDown, &QAbstractButton::clicked
            , Parent, &SARibbonGallery::onPageDown);
        Parent->connect(buttonMore, &QAbstractButton::clicked
            , Parent, &SARibbonGallery::onShowMoreDetail);
        //信号转发
        Parent->connect(actionGroup, &QActionGroup::triggered
            , Parent, &SARibbonGallery::triggered);
        Parent->connect(actionGroup, &QActionGroup::hovered
            , Parent, &SARibbonGallery::hovered);
        popupWidget = nullptr;
        viewportGroup = nullptr;
        btnLayout = new QBoxLayout(QBoxLayout::TopToBottom);
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
            popupWidget = new RibbonGalleryViewport(Parent);
#endif
        }
    }


    void setViewPort(SARibbonGalleryGroup *v)
    {
        if (nullptr == viewportGroup) {
            viewportGroup = RibbonSubElementDelegate->createRibbonGalleryGroup(Parent);
            layout->addWidget(viewportGroup, 1);
        }
        viewportGroup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        viewportGroup->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        viewportGroup->setModel(v->model());
        viewportGroup->setEnableIconText(v->enableIconText());
        viewportGroup->show();
    }
};


//////////////////////////////////////////////


RibbonGalleryViewport::RibbonGalleryViewport(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::Popup);
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
}


void RibbonGalleryViewport::addWidget(QWidget *w)
{
    m_layout->addWidget(w);
}


//////////////////////////////////////////////

SARibbonGallery::SARibbonGallery(QWidget *parent) : QFrame(parent)
    , m_d(new SARibbonGalleryPrivate)
{
    m_d->init(this);
    setFrameShape(QFrame::Box);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(RibbonSubElementStyleOpt.galleryFixedHeight);
    setMinimumWidth(RibbonSubElementStyleOpt.galleryMinimumWidth);
}


SARibbonGallery::~SARibbonGallery()
{
    delete m_d;
}


QSize SARibbonGallery::sizeHint() const
{
    return (QSize(232, RibbonSubElementStyleOpt.galleryFixedHeight));
}


QSize SARibbonGallery::minimumSizeHint() const
{
    return (QSize(RibbonSubElementStyleOpt.galleryMinimumWidth, RibbonSubElementStyleOpt.galleryFixedHeight));
}


SARibbonGalleryGroup *SARibbonGallery::addGalleryGroup()
{
    SARibbonGalleryGroup *group = RibbonSubElementDelegate->createRibbonGalleryGroup(this);

    addGalleryGroup(group);
    return (group);
}


void SARibbonGallery::addGalleryGroup(SARibbonGalleryGroup *group)
{
    RibbonGalleryViewport *viewport = ensureGetPopupViewPort();
    SARibbonGalleryGroupModel *model = new SARibbonGalleryGroupModel(this);

    group->setModel(model);
    viewport->addWidget(group);
    if (nullptr == m_d->viewportGroup) {
        setCurrentViewGroup(group);
    }
    connect(group, &QAbstractItemView::clicked
        , this, &SARibbonGallery::onItemClicked);
}


SARibbonGalleryGroup *SARibbonGallery::addCategoryActions(const QString& title, QList<QAction *> actions)
{
    SARibbonGalleryGroup *group = RibbonSubElementDelegate->createRibbonGalleryGroup(this);
    SARibbonGalleryGroupModel *model = new SARibbonGalleryGroupModel(this);

    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    group->setModel(model);
    if (!title.isEmpty()) {
        group->setGroupTitle(title);
    }
    for (QAction *a : actions)
    {
        m_d->actionGroup->addAction(a);
    }
    group->addActionItemList(actions);
    connect(group, &QAbstractItemView::clicked
        , this, &SARibbonGallery::onItemClicked);
    RibbonGalleryViewport *viewport = ensureGetPopupViewPort();

    viewport->addWidget(group);
    setCurrentViewGroup(group);
    return (group);
}


void SARibbonGallery::setCurrentViewGroup(SARibbonGalleryGroup *group)
{
    m_d->setViewPort(group);
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}


SARibbonGalleryGroup *SARibbonGallery::currentViewGroup() const
{
    return (m_d->viewportGroup);
}


QActionGroup *SARibbonGallery::getActionGroup() const
{
    return (m_d->actionGroup);
}


void SARibbonGallery::onPageDown()
{
    if (m_d->viewportGroup) {
        QScrollBar *vscrollBar = m_d->viewportGroup->verticalScrollBar();
        int v = vscrollBar->value();
        v += vscrollBar->singleStep();
        vscrollBar->setValue(v);
    }
}


void SARibbonGallery::onPageUp()
{
    if (m_d->viewportGroup) {
        QScrollBar *vscrollBar = m_d->viewportGroup->verticalScrollBar();
        int v = vscrollBar->value();
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
    QPoint start = mapToGlobal(QPoint(0, 0));

    int width = m_d->viewportGroup->width();                                // viewport

    width += qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);        // scrollbar
    m_d->popupWidget->setGeometry(start.x(), start.y(), width, popupMenuSize.height());
    m_d->popupWidget->show();
}


void SARibbonGallery::onItemClicked(const QModelIndex& index)
{
    QObject *obj = sender();
    SARibbonGalleryGroup *group = qobject_cast<SARibbonGalleryGroup *>(obj);

    if (group) {
        SARibbonGalleryGroup *curGroup = currentViewGroup();
        if (nullptr == curGroup) {
            setCurrentViewGroup(group);
            curGroup = currentViewGroup();
        }
        if (curGroup->model() != group->model()) {
            curGroup->setModel(group->model());
        }
        curGroup->scrollTo(index);
        curGroup->setCurrentIndex(index);
        curGroup->repaint();
    }
}


RibbonGalleryViewport *SARibbonGallery::ensureGetPopupViewPort()
{
    if (nullptr == m_d->popupWidget) {
        m_d->createPopupWidget();
    }
    return (m_d->popupWidget);
}


void SARibbonGallery::resizeEvent(QResizeEvent *event)
{
//    if (!m_d->isValid()) {
//        return;
//    }
//    const QSize r = event->size();
//    int subW = 0;

//    m_d->buttonUp->move(r.width() - m_d->buttonUp->width(), 0);
//    subW = qMax(subW, m_d->buttonUp->width());
//    m_d->buttonDown->move(r.width() - m_d->buttonDown->width(), m_d->buttonUp->height());
//    subW = qMax(subW, m_d->buttonDown->width());
//    m_d->buttonMore->move(r.width() - m_d->buttonMore->width(), m_d->buttonDown->geometry().bottom()+1);
//    subW = qMax(subW, m_d->buttonMore->width());
//    if (m_d->viewportGroup) {
//        m_d->viewportGroup->setGeometry(0, 0, r.width()-subW, r.height());
//    }
    QFrame::resizeEvent(event);
}


void SARibbonGallery::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
}
