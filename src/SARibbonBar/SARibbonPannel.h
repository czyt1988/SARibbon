#ifndef SARIBBONPANNEL_H
#define SARIBBONPANNEL_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QLayout>
#include "SARibbonToolButton.h"
class SARibbonMenu;
class SARibbonGallery;
class QGridLayout;
class SARibbonPannelOptionButton;
class SARibbonPannelPrivate;


/**
 * @brief 是对pannel所有子窗口的抽象，参考qt的toolbar
 *
 * 参考qt的toolbar，pannel所有子窗口内容都通过QAction进行抽象，包括gallery这些窗口，也是通过QAction进行抽象
 * QAction最终会转换为SARibbonPannelItem，每个SARibbonPannelItem都含有一个widget，SARibbonPannel的布局
 * 就基于SARibbonPannelItem
 *
 * 无窗口的action会在内部生成一个SARibbonToolButton，
 */
class SA_RIBBON_EXPORT SARibbonPannelItem : public QWidgetItem
{
public:

    /**
     * @brief 定义了行的占比，ribbon中有large，media和small三种占比
     */
    enum RowProportion {
        None            ///< 为定义占比，这时候将会依据expandingDirections来判断，如果能有Qt::Vertical，就等同于Large，否则就是Small
        , Large         ///< 大占比，一个widget的高度会充满整个pannel
        , Medium        ///< 中占比，在@ref SARibbonPannel::pannelLayoutMode 为 @ref SARibbonPannel::ThreeRowMode 时才会起作用，且要同一列里两个都是Medium时，会在三行中占据两行
        , Small         ///< 小占比，占SARibbonPannel的一行，Medium在不满足条件时也会变为Small，但不会变为Large
    };
    SARibbonPannelItem(QWidget *widget);
    bool isEmpty() const Q_DECL_OVERRIDE;

    short rowIndex;                                         ///< 记录当前item属于第几行，hide模式下为-1
    int columnIndex;                                        ///< 记录当前item属于第几列，hide模式下为-1
    QRect itemWillSetGeometry;                              ///< 在调用SARibbonPannelLayout::updateGeomArray会更新这个此处，实际设置的时候会QWidgetItem::setGeometry设置Geometry
    QAction *action;                                        /// < 记录action，参考QToolBarLayoutItem
    bool customWidget;                                      ///< 对于没有窗口的action，实际也会有一个SARibbonToolButton，在销毁时要delete掉
    SARibbonPannelItem::RowProportion rowProportion;        ///< 行的占比，ribbon中有large，media和small三种占比,见@ref RowProportion
};



/**
 * @brief pannel页窗口，pannel是ribbon的面板用于承放控件
 *
 * ribbon的pannel分为两行模式和三行模式，以office为代表的ribbon为3行模式，以WPS为代表的“紧凑派”就是2行模式，
 * SARibbon可通过SARibbonBar的@ref SARibbonBar::RibbonStyle 来指定模式(通过函数@ref SARibbonBar::setRibbonStyle )
 *
 * 在pannel中，可以通过@ref setExpanding 函数指定pannel水平扩展，如果pannel里面没有能水平扩展的控件，将会留白，因此，
 * 建议在pannel里面有水平扩展的控件如（@ref SARibbonGallery ）才指定这个函数
 *
 * pannel的布局通过@ref SARibbonPannelLayout 来实现，如果有其他布局，可以通过继承@ref SARibbonElementCreateDelegate::createRibbonPannel
 * 函数返回带有自己布局的pannel，但你必须继承对应的虚函数
 */
class SA_RIBBON_EXPORT SARibbonPannel : public QWidget
{
    Q_OBJECT
    friend class SARibbonCategory;
    friend class SARibbonCategoryPrivate;
public:
    SARibbonPannel(QWidget *parent = 0);
    ~SARibbonPannel();
    using QWidget::addAction;
    enum PannelLayoutMode {
        ThreeRowMode    ///< 三行布局模式，office就是三行布局模式，pannel能布置3行小toolbutton，默认模式
        , TwoRowMode    ///< 两行布局模式，wps的后续布局模式就是两行布局模式，pannel能布置2行小toolbutton
    };
    //设置action的行行为，行属性决定了ribbon pannel的显示方式
    void setActionRowProportion(QAction *action, SARibbonPannelItem::RowProportion rp);

    //把action加入到pannel
    SARibbonToolButton *addAction(QAction *action, SARibbonPannelItem::RowProportion rp);

    //把action加入到pannel，并以大图标显示
    SARibbonToolButton *addLargeAction(QAction *action);

    //把action加入到pannel，在三行模式下会以中图标显示
    SARibbonToolButton *addMediumAction(QAction *action);

    //把action加入到pannel，并以小图标显示
    SARibbonToolButton *addSmallAction(QAction *action);

    //生成并添加一个action
    QAction *addAction(const QString& text, const QIcon& icon
        , QToolButton::ToolButtonPopupMode popMode
        , SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::Large);

    //添加menu
    SARibbonToolButton *addMenu(QMenu *menu
        , SARibbonPannelItem::RowProportion rp
        , QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    //添加action menu
    SARibbonToolButton *addActionMenu(QAction *action, QMenu *menu, SARibbonPannelItem::RowProportion rp);

    //action menu,action menu是一个特殊的menu,即可点击触发action，也可弹出菜单
    SARibbonToolButton *addLargeActionMenu(QAction *action, QMenu *menu);

    //添加普通大菜单
    SARibbonToolButton *addLargeMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    //添加普通小按钮菜单
    SARibbonToolButton *addSmallMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    //添加窗口
    QAction *addWidget(QWidget *w, SARibbonPannelItem::RowProportion rp);

    //添加窗口,占用ribbon的一行
    QAction *addSmallWidget(QWidget *w);

    //添加窗口，占用所有行
    QAction *addLargeWidget(QWidget *w);

    //添加一个Gallery
    SARibbonGallery *addGallery();

    //添加分割线
    QAction *addSeparator(int top = 6, int bottom = 6);

    //从pannel中把action对应的button提取出来，如果action没有对应的button，就返回nullptr
    SARibbonToolButton *actionToRibbonToolButton(QAction *action);

    //添加操作action，如果要去除，传入nullptr指针即可，SARibbonPannel不会对QAction的所有权进行管理
    void addOptionAction(QAction *action);

    //判断是否存在OptionAction
    bool isHaveOptionAction() const;

    //获取所有的buttons
    QList<SARibbonToolButton *> ribbonToolButtons() const;

    //获取PannelLayoutMode
    PannelLayoutMode pannelLayoutMode() const;

    //判断是否为2行模式
    bool isTwoRow() const { return (TwoRowMode == pannelLayoutMode()); }

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    //把pannel设置为扩展模式，此时会撑大水平区域
    void setExpanding(bool isExpanding = true);

    //是否是扩展模式
    bool isExpanding() const;

    //标题栏高度，仅在3行模式下生效
    virtual int titleHeight() const;

    //optionActionButton的尺寸
    virtual QSize optionActionButtonSize() const;

signals:

    /**
     * @brief 等同于QToolBar::actionTriggered
     * @param action
     */
    void actionTriggered(QAction *action);

protected:
    //设置PannelLayoutMode，此函数设置为protect避免误调用
    void setPannelLayoutMode(PannelLayoutMode mode);
    void resetLayout(PannelLayoutMode newmode);
    void resetLargeToolButtonStyle();
    static QSize maxHightIconSize(const QSize& size, int h);
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent *e) Q_DECL_OVERRIDE;

private:
    SARibbonPannelPrivate *m_d;
};

/**
 * @brief 针对SARibbonPannel的布局
 *
 * SARibbonPannelLayout实际是一个列布局，每一列有2~3行，看窗口定占几行
 *
 *
 * 核心函数： @ref SARibbonPannelLayout::createItem
 */
class SA_RIBBON_EXPORT SARibbonPannelLayout : public QLayout
{
    Q_OBJECT
public:

    SARibbonPannelLayout(QWidget *p = 0);
    ~SARibbonPannelLayout();
    virtual int indexOf(QAction *action) const;

    //SARibbonPannelLayout additem 无效
    void addItem(QLayoutItem *item) Q_DECL_OVERRIDE;

    //SARibbonPannel主要通过此函数来添加action
    void insertAction(int index, QAction *act, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::None);

    //
    QLayoutItem *itemAt(int index) const Q_DECL_OVERRIDE;
    QLayoutItem *takeAt(int index) Q_DECL_OVERRIDE;
    int count() const Q_DECL_OVERRIDE;

    bool isEmpty() const Q_DECL_OVERRIDE;
    void invalidate() Q_DECL_OVERRIDE;
    Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;

    void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
    QSize minimumSize() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    //通过action获取SARibbonPannelItem
    SARibbonPannelItem *pannelItem(QAction *action) const;

    //获取最后一个添加的item
    SARibbonPannelItem *lastItem() const;

    //获取最后生成的窗口
    QWidget *lastWidget() const;

    //根据pannel的默认参数得到的pannel高度
    int defaultPannelHeight() const;

protected:
    //布局action
    void layoutActions();

    //把action转换为item，对于纯Action，此函数会创建SARibbonToolButton,
    //rp用于告诉Layout生成什么样的窗口，详细见SARibbonPannelItem::RowProportion
    SARibbonPannelItem *createItem(QAction *action, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::None);
    void updateGeomArray(const QRect& setrect);

    //重新计算扩展条码，此函数必须在updateGeomArray函数之后调用
    void recalcExpandGeomArray(const QRect& setrect);

private:
    //返回所有列的区域
    //QMap<int,QRect> columnsGeometry() const;
    //根据列数，计算窗口的宽度，以及最大宽度
    void columnWidthInfo(int colindex, int& width, int& maximum) const;

private:
    QList<SARibbonPannelItem *> m_items;
    int m_columnCount;      ///< 记录有多少列
    bool m_expandFlag;      ///< 标记是否是会扩展的
    QSize m_sizeHint;       ///< sizeHint返回的尺寸
    bool m_dirty;           ///< 用于标记是否需要刷新元素，参考QToolBarLayout源码
    int m_largeHeight;      ///< 记录大图标的高度
};


#endif // SARIBBONPANNEL_H
