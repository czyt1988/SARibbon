#ifndef SARIBBONPANNEL_H
#define SARIBBONPANNEL_H
#include "SARibbonGlobal.h"
#include "SARibbonPannelItem.h"
#include "SARibbonPannelLayout.h"
#include "SARibbonToolButton.h"
#include <QLayout>
#include <QWidget>
#include <QLabel>
class SARibbonMenu;
class SARibbonGallery;
class QGridLayout;
class SARibbonPannelOptionButton;
class SARibbonPannelLayout;
class SARibbonCategory;
class SARibbonBar;

/**
 * @brief SARibbonPannel的标题label，此类用于qss
 */
class SA_RIBBON_EXPORT SARibbonPannelLabel : public QLabel
{
    Q_OBJECT
public:
    SARibbonPannelLabel(QWidget* parent = nullptr);
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
 * pannel的布局通过@ref SARibbonPannelLayout 来实现，如果有其他布局，可以通过继承@ref
 * SARibbonElementCreateDelegate::createRibbonPannel 函数返回带有自己布局的pannel，但你必须继承对应的虚函数
 */
class SA_RIBBON_EXPORT SARibbonPannel : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonPannel)
    friend class SARibbonCategory;
    friend class SARibbonCategoryPrivate;
    friend class SARibbonCustomizeWidgetPrivate;
    friend class SARibbonPannelLayout;
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(bool isExpanding READ isExpanding WRITE setExpanding)
    Q_PROPERTY(QString pannelName READ pannelName WRITE setPannelName)
public:
    enum PannelLayoutMode
    {
        ThreeRowMode,  ///< 三行布局模式，office就是三行布局模式，pannel能布置3行小toolbutton，默认模式
        TwoRowMode  ///< 两行布局模式，wps的后续布局模式就是两行布局模式，pannel能布置2行小toolbutton
    };
    Q_ENUM(PannelLayoutMode)
public:
    SARibbonPannel(QWidget* parent = nullptr);
    SARibbonPannel(const QString& name, QWidget* parent = nullptr);
    ~SARibbonPannel() Q_DECL_OVERRIDE;
    using QWidget::addAction;

    // 把action加入到pannel
    void addAction(QAction* action, SARibbonPannelItem::RowProportion rp);
    // 生成并添加一个action
    void addAction(QAction* act,
                   QToolButton::ToolButtonPopupMode popMode,
                   SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::Large);
    // 把action加入到pannel，并以大图标显示
    void addLargeAction(QAction* action);
    // 把action加入到pannel，在三行模式下会以中图标显示
    void addMediumAction(QAction* action);
    // 把action加入到pannel，并以小图标显示
    void addSmallAction(QAction* action);

    // 把action加入到pannel，并以小图标显示
    void addSmallAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
    // 把action加入到pannel，并以大图标显示
    void addLargeAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
    // 把action加入到pannel，在三行模式下会以中图标显示
    void addMediumAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);

    QAction* addAction(const QString& text,
                       const QIcon& icon,
                       QToolButton::ToolButtonPopupMode popMode,
                       SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::Large);

    // 添加menu
    void addMenu(QMenu* menu,
                 SARibbonPannelItem::RowProportion rp,
                 QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    // 添加普通大菜单
    void addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    // 添加普通小按钮菜单
    void addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    // 添加窗口
    QAction* addWidget(QWidget* w, SARibbonPannelItem::RowProportion rp);

    // 添加窗口,占用ribbon的一行
    QAction* addSmallWidget(QWidget* w);

    // 添加窗口,占用ribbon的一行
    QAction* addMediumWidget(QWidget* w);

    // 添加窗口，占用所有行
    QAction* addLargeWidget(QWidget* w);

    // 添加一个Gallery
    SARibbonGallery* addGallery(bool expanding = true);

    // 添加分割线
    QAction* addSeparator();

    // 从pannel中把action对应的button提取出来，如果action没有对应的button，就返回nullptr
    SARibbonToolButton* actionToRibbonToolButton(QAction* action);

    // 设置操作action，如果要去除，传入nullptr指针即可，SARibbonPannel不会对QAction的所有权进行管理
    // OptionAction也会触发actionTriggered信号
    void setOptionAction(QAction* action);

    // 判断是否存在OptionAction
    bool isHaveOptionAction() const;

    // 获取所有的buttons
    QList< SARibbonToolButton* > ribbonToolButtons() const;

    // 获取PannelLayoutMode
    PannelLayoutMode pannelLayoutMode() const;
    void setPannelLayoutMode(PannelLayoutMode mode);
    // 更新按钮的尺寸，这个函数在pannel的布局状态变换后需要调用刷新
    void resetToolButtonSize();

    // 判断是否为2行模式
    bool isTwoRow() const
    {
        return (TwoRowMode == pannelLayoutMode());
    }

    // 把pannel设置为扩展模式，此时会撑大水平区域
    void setExpanding(bool isExpanding = true);

    // 是否是扩展模式
    bool isExpanding() const;

    // 标题栏高度
    void setTitleHeight(int h);
    int titleHeight() const;

    // 是否显示标题，显示标题后，标题的高度需要设置，默认高度为15
    bool isEnableShowTitle() const;
    void setEnableShowTitle(bool on);

    // action对应的布局index，此操作一般用于移动moveAction，其他意义不大
    int actionIndex(QAction* act) const;

    // 移动action
    void moveAction(int from, int to);

    // 判断是否可以自定义
    bool isCanCustomize() const;
    void setCanCustomize(bool b);

    // 标题
    QString pannelName() const;
    void setPannelName(const QString& title);

    // 大图标的高度
    int largeButtonHeight() const;
    // 获取布局对应的item,此函数目前仅仅在自定义过程中用到
    const QList< SARibbonPannelItem* >& ribbonPannelItem() const;
    // 获取pannel layout
    SARibbonPannelLayout* pannelLayout() const;
    // 更新布局
    void updateItemGeometry();
    // 获取category指针，如果没有parent，或者不在category管理，返回nullptr
    SARibbonCategory* category() const;
    // 获取ribbonBar指针，如果没有返回nullptr
    SARibbonBar* ribbonBar() const;
    //
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;
signals:

    /**
     * @brief 等同于QToolBar::actionTriggered
     * @param action
     */
    void actionTriggered(QAction* action);

    /**
     * @brief pannel的标题发生了改变
     * @param n
     */
    void pannelNameChanged(const QString& n);

public:
    // pannel高度推荐
    static int pannelHeightHint(const QFontMetrics& fm, PannelLayoutMode layMode, int pannelTitleHeight);
    // 把action的行属性设置进action中，action自身携带了行属性
    static void setActionRowProportionProperty(QAction* action, SARibbonPannelItem::RowProportion rp);
    // 获取action的行属性
    static SARibbonPannelItem::RowProportion getActionRowProportionProperty(QAction* action);
    // 把action的PopupMode属性设置进action中，action自身携带了PopupMode属性
    static void setActionToolButtonPopupModeProperty(QAction* action, QToolButton::ToolButtonPopupMode popMode);
    // 获取action的PopupMode属性
    static QToolButton::ToolButtonPopupMode getActionToolButtonPopupModeProperty(QAction* action);
    // 把action的ToolButtonStyle属性设置进action中，action自身携带了ToolButtonStyle属性
    static void setActionToolButtonStyleProperty(QAction* action, Qt::ToolButtonStyle buttonStyle);
    // 获取action的ToolButtonStyle属性
    static Qt::ToolButtonStyle getActionToolButtonStyleProperty(QAction* action);

protected:
    virtual bool event(QEvent* e) Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;
    virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;
};

#endif  // SARIBBONPANNEL_H
