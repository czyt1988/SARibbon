#ifndef SARIBBONPANNEL_H
#define SARIBBONPANNEL_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QLayout>
#include "SARibbonToolButton.h"
#include "SARibbonPannelLayout.h"
#include "SARibbonPannelItem.h"
class SARibbonMenu;
class SARibbonGallery;
class QGridLayout;
class SARibbonPannelOptionButton;
class SARibbonPannelPrivate;

/**
 * @brief pannel页窗口，pannel是ribbon的面板用于承放控件
 *
 * ribbon的pannel分为两行模式和三行模式，以office为代表的ribbon为3行模式，以WPS为代表的“紧凑派”就是2行模式，
 * SARibbon可通过SARibbonBar的@ref SARibbonBar::RibbonStyle 来指定模式(通过函数@ref SARibbonBar::setRibbonStyle )
 *
 * 在pannel中，可以通过@ref setExpanding 函数指定pannel水平扩展，如果pannel里面没有能水平扩展的控件，将会留白，因此，
 * 建议在pannel里面有水平扩展的控件如（@ref SARibbonGallery ）才指定这个函数
 *
 * pannel的布局通过@ref SARibbonPannelLayout 来实现，如果有其他布局，可以通过继承@ref
 * SARibbonElementCreateDelegate::createRibbonPannel 函数返回带有自己布局的pannel，但你必须继承对应的虚函数
 */
class SA_RIBBON_EXPORT SARibbonPannel : public QWidget
{
    Q_OBJECT
    friend class SARibbonCategory;
    friend class SARibbonCategoryPrivate;
    friend class SARibbonCustomizeWidgetPrivate;
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(bool isExpanding READ isExpanding WRITE setExpanding)
    Q_PROPERTY(QString pannelName READ pannelName WRITE setPannelName)
public:
    SARibbonPannel(QWidget* parent = nullptr);
    SARibbonPannel(const QString& name, QWidget* parent = nullptr);
    ~SARibbonPannel() Q_DECL_OVERRIDE;
    using QWidget::addAction;
    enum PannelLayoutMode
    {
        ThreeRowMode  ///< 三行布局模式，office就是三行布局模式，pannel能布置3行小toolbutton，默认模式
        ,
        TwoRowMode  ///< 两行布局模式，wps的后续布局模式就是两行布局模式，pannel能布置2行小toolbutton
    };

    //把action的行属性设置进action中，action自身携带了行属性
    static void setActionRowProportionProperty(QAction* action, SARibbonPannelItem::RowProportion rp);

    //获取action的行属性
    static SARibbonPannelItem::RowProportion getActionRowProportionProperty(QAction* action);

    //设置action的行行为，行属性决定了ribbon pannel的显示方式
    void setActionRowProportion(QAction* action, SARibbonPannelItem::RowProportion rp);

    //把action加入到pannel
    SARibbonToolButton* addAction(QAction* action, SARibbonPannelItem::RowProportion rp);

    //把action加入到pannel，并以大图标显示
    SARibbonToolButton* addLargeAction(QAction* action);

    //把action加入到pannel，在三行模式下会以中图标显示
    SARibbonToolButton* addMediumAction(QAction* action);

    //把action加入到pannel，并以小图标显示
    SARibbonToolButton* addSmallAction(QAction* action);

    //生成并添加一个action
    void addAction(QAction* act, QToolButton::ToolButtonPopupMode popMode, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::Large);

    QAction* addAction(const QString& text,
                       const QIcon& icon,
                       QToolButton::ToolButtonPopupMode popMode,
                       SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::Large);

    //添加menu
    SARibbonToolButton* addMenu(QMenu* menu,
                                SARibbonPannelItem::RowProportion rp,
                                QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    //添加action menu
    SARibbonToolButton* addActionMenu(QAction* action, QMenu* menu, SARibbonPannelItem::RowProportion rp);

    // action menu,action menu是一个特殊的menu,即可点击触发action，也可弹出菜单
    SARibbonToolButton* addLargeActionMenu(QAction* action, QMenu* menu);

    //添加普通大菜单
    SARibbonToolButton* addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    //添加普通小按钮菜单
    SARibbonToolButton* addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    //添加窗口
    QAction* addWidget(QWidget* w, SARibbonPannelItem::RowProportion rp);

    //添加窗口,占用ribbon的一行
    QAction* addSmallWidget(QWidget* w);

    //添加窗口，占用所有行
    QAction* addLargeWidget(QWidget* w);

    //添加一个Gallery
    SARibbonGallery* addGallery();

    //添加分割线
    QAction* addSeparator(int top = 6, int bottom = 6);

    //从pannel中把action对应的button提取出来，如果action没有对应的button，就返回nullptr
    SARibbonToolButton* actionToRibbonToolButton(QAction* action);

    //添加操作action，如果要去除，传入nullptr指针即可，SARibbonPannel不会对QAction的所有权进行管理
    void addOptionAction(QAction* action);

    //判断是否存在OptionAction
    bool isHaveOptionAction() const;

    //获取所有的buttons
    QList< SARibbonToolButton* > ribbonToolButtons() const;

    //获取PannelLayoutMode
    PannelLayoutMode pannelLayoutMode() const;

    //判断是否为2行模式
    bool isTwoRow() const
    {
        return (TwoRowMode == pannelLayoutMode());
    }

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    //把pannel设置为扩展模式，此时会撑大水平区域
    void setExpanding(bool isExpanding = true);

    //是否是扩展模式
    bool isExpanding() const;

    //标题栏高度，仅在3行模式下生效
    int titleHeight() const;

    // optionActionButton的尺寸
    virtual QSize optionActionButtonSize() const;

    // action对应的布局index，此操作一般用于移动moveAction，其他意义不大
    int actionIndex(QAction* act) const;

    //移动action
    void moveAction(int from, int to);

    //判断是否可以自定义
    bool isCanCustomize() const;
    void setCanCustomize(bool b);

    //标题
    QString pannelName() const;
    void setPannelName(const QString& title);

    //大图标的高度
    int largeHeight() const;

    //全局的标题栏高度
    static int pannelTitleHeight();
    static void setPannelTitleHeight(int h);
signals:

    /**
     * @brief 等同于QToolBar::actionTriggered
     * @param action
     */
    void actionTriggered(QAction* action);

protected:
    //设置PannelLayoutMode，此函数设置为protect避免误调用
    void setPannelLayoutMode(PannelLayoutMode mode);
    void resetLayout(PannelLayoutMode newmode);
    void resetLargeToolButtonStyle();
    static QSize maxHightIconSize(const QSize& size, int h);
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;

    //获取布局对应的item
    const QList< SARibbonPannelItem* >& ribbonPannelItem() const;

private:
    SARibbonPannelPrivate* m_d;
};

#endif  // SARIBBONPANNEL_H
