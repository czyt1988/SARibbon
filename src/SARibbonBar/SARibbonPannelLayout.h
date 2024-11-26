#ifndef SARIBBONPANNELLAYOUT_H
#define SARIBBONPANNELLAYOUT_H
#include "SARibbonGlobal.h"
#include <QLayout>
#include "SARibbonPannelItem.h"
class QToolButton;
class SARibbonPannel;
class SARibbonPannelLabel;
/**
 * @brief 针对SARibbonPannel的布局
 *
 * SARibbonPannelLayout实际是一个列布局，每一列有2~3行，看窗口定占几行
 *
 * 核心函数： @ref SARibbonPannelLayout::createItem
 *
 * @note QLayout::contentsMargins 函数不会启作用,如果要设置contentsMargins，使用@sa setPannelContentsMargins
 */
class SA_RIBBON_EXPORT SARibbonPannelLayout : public QLayout
{
    Q_OBJECT
    friend class SARibbonPannel;

public:
    explicit SARibbonPannelLayout(QWidget* p = nullptr);
    ~SARibbonPannelLayout();
    // SARibbonPannelLayout additem 无效
    void addItem(QLayoutItem* item) Q_DECL_OVERRIDE;
    // QLayout 所必须的重载函数
    QLayoutItem* itemAt(int index) const Q_DECL_OVERRIDE;
    QLayoutItem* takeAt(int index) Q_DECL_OVERRIDE;
    int count() const Q_DECL_OVERRIDE;
    bool isEmpty() const Q_DECL_OVERRIDE;
    void invalidate() Q_DECL_OVERRIDE;
    Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
    void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
    QSize minimumSize() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    // 获取ribbonpannel
    SARibbonPannel* ribbonPannel() const;
    // SARibbonPannel主要通过此函数来添加action
    void insertAction(int index, QAction* act, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::None);
    // 设置OptionAction，如果要去除，传入nullptr指针即可
    void setOptionAction(QAction* action);
    bool isHaveOptionAction() const;
    // 通过action获取SARibbonPannelItem
    SARibbonPannelItem* pannelItem(QAction* action) const;
    // 获取最后一个添加的item
    SARibbonPannelItem* lastItem() const;
    // 获取最后生成的窗口
    QWidget* lastWidget() const;
    // 移动两个item
    void move(int from, int to);
    // 判断是否需要重新布局
    bool isDirty() const;
    // 更新尺寸
    void updateGeomArray();
    // 通过action获取SARibbonPannelItem的索引
    int indexByAction(QAction* action) const;
    // 标题高度
    int pannelTitleHeight() const;
    void setPannelTitleHeight(int newTitleHeight);
    // 判断是否存在标题
    bool isEnableShowPannelTitle() const;
    void setEnableShowPannelTitle(bool on);
    // 返回大按钮的高度
    int largeButtonHeight() const;
    // 标题区域和按钮的间隔
    int pannelTitleSpace() const;
    void setPannelTitleSpace(int newTitleSpace);
    // pannel 标题的label
    SARibbonPannelLabel* pannelTitleLabel() const;
    // 设置按钮的icon尺寸，large action不受此尺寸影响
    void setToolButtonIconSize(const QSize& s);
    QSize toolButtonIconSize() const;

protected:
    // 获取optionAction 按钮尺寸
    QSize optionActionButtonSize() const;
    // 布局action
    void doLayout();
    // 把action转换为item，对于纯Action，此函数会创建SARibbonToolButton,
    //  rp用于告诉Layout生成什么样的窗口，详细见SARibbonPannelItem::RowProportion
    SARibbonPannelItem* createItem(QAction* action, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::None);
    void updateGeomArray(const QRect& setrect);
    // 重新计算扩展条码，此函数必须在updateGeomArray函数之后调用
    void recalcExpandGeomArray(const QRect& setrect);
    // 返回optionActionButton的尺寸

private:
    // 根据列数，计算窗口的宽度，以及最大宽度
    void columnWidthInfo(int colindex, int& width, int& maximum) const;
    // 设置titlelabel
    void setPannelTitleLabel(SARibbonPannelLabel* newTitleLabel);

private:
    QList< SARibbonPannelItem* > mItems;
    int mColumnCount { 0 };                       ///< 记录有多少列
    bool mExpandFlag { false };                   ///< 标记是否是会扩展的
    QSize mSizeHint;                              ///< sizeHint返回的尺寸
    QSize mDefaultToolButtonIconSize { 22, 22 };  ///< 记录默认的按钮图标尺寸
    bool mDirty { true };                         ///< 用于标记是否需要刷新元素，参考QToolBarLayout源码
    int mLargeHeight { 0 };                       ///< 记录大图标的高度
    int mTitleHeight { 15 };                      ///< 标题区域高度
    int mTitleSpace { 2 };                        ///< 标题区域和按钮的间隔
    bool mEnableShowTitle { true };               ///< 是否运行显示pannel标题
    SARibbonPannelLabel* mTitleLabel { nullptr };  ///< titlelabel指针
    QRect mTitleLabelGeometry;                     ///< titlelabel的位置
    QToolButton* mOptionActionBtn { nullptr };     ///< optionAction对应的button
    QRect mOptionActionBtnGeometry;                ///< optionAction的位置
};

#endif  // SARIBBONPANNELLAYOUT_H
