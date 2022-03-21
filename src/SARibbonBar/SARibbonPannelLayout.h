#ifndef SARIBBONPANNELLAYOUT_H
#define SARIBBONPANNELLAYOUT_H
#include "SARibbonGlobal.h"
#include <QLayout>
#include "SARibbonPannelItem.h"
class SARibbonPannel;

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
    SARibbonPannelLayout(QWidget* p = 0);
    ~SARibbonPannelLayout();
    virtual int indexOf(QAction* action) const;

    // SARibbonPannelLayout additem 无效
    void addItem(QLayoutItem* item) Q_DECL_OVERRIDE;

    // SARibbonPannel主要通过此函数来添加action
    void insertAction(int index, QAction* act, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::None);

    //
    QLayoutItem* itemAt(int index) const Q_DECL_OVERRIDE;
    QLayoutItem* takeAt(int index) Q_DECL_OVERRIDE;
    int count() const Q_DECL_OVERRIDE;

    bool isEmpty() const Q_DECL_OVERRIDE;
    void invalidate() Q_DECL_OVERRIDE;
    Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;

    void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
    QSize minimumSize() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    //通过action获取SARibbonPannelItem
    SARibbonPannelItem* pannelItem(QAction* action) const;

    //获取最后一个添加的item
    SARibbonPannelItem* lastItem() const;

    //获取最后生成的窗口
    QWidget* lastWidget() const;

    //移动两个item
    void move(int from, int to);

    //判断是否需要重新布局
    bool isDirty() const;

    //计算大图标的高度
    static int calcLargeHeight(const QRect& setrect, const SARibbonPannel* pannel);

public:
    //全局的contentsMargins
    static const QMargins& pannelContentsMargins();
    static void setPannelContentsMargins(const QMargins& m);

protected:
    //布局action
    void layoutActions();

    //把action转换为item，对于纯Action，此函数会创建SARibbonToolButton,
    // rp用于告诉Layout生成什么样的窗口，详细见SARibbonPannelItem::RowProportion
    SARibbonPannelItem* createItem(QAction* action, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::None);
    void updateGeomArray(const QRect& setrect);

    //重新计算扩展条码，此函数必须在updateGeomArray函数之后调用
    void recalcExpandGeomArray(const QRect& setrect);

private:
    //返回所有列的区域
    // QMap<int,QRect> columnsGeometry() const;
    //根据列数，计算窗口的宽度，以及最大宽度
    void columnWidthInfo(int colindex, int& width, int& maximum) const;

private:
    QList< SARibbonPannelItem* > m_items;
    int m_columnCount;  ///< 记录有多少列
    bool m_expandFlag;  ///< 标记是否是会扩展的
    QSize m_sizeHint;   ///< sizeHint返回的尺寸
    bool m_dirty;       ///< 用于标记是否需要刷新元素，参考QToolBarLayout源码
    int m_largeHeight;  ///< 记录大图标的高度
    /**
     * @brief 统一的contentsMargins，此参数作为静态变量，可以进行全局设置
     */
    static QMargins s_contentsMargins;
};

#endif  // SARIBBONPANNELLAYOUT_H
