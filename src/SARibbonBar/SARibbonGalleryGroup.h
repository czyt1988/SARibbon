#ifndef SARIBBONGALLERYGROUP_H
#define SARIBBONGALLERYGROUP_H
#include "SARibbonGlobal.h"
#include <QList>
#include "SARibbonGalleryItem.h"
#include <QListView>
#include <QStyledItemDelegate>
#include "SARibbonGalleryItem.h"

///
/// \brief SARibbonGalleryGroup对应的显示代理
///
class SA_RIBBON_EXPORT SARibbonGalleryGroupItemDelegate : public QStyledItemDelegate
{
public:
    SARibbonGalleryGroupItemDelegate(SARibbonGalleryGroup* group, QObject* parent = Q_NULLPTR);
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;

    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    virtual void paintIconOnly(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void paintIconWithText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void paintIconWithTextWordWrap(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    SARibbonGalleryGroup* m_group;
};

///
/// \brief SARibbonGalleryGroup对应的model
///
class SA_RIBBON_EXPORT SARibbonGalleryGroupModel : public QAbstractListModel
{
    Q_OBJECT
public:
    SARibbonGalleryGroupModel(QObject* parent = Q_NULLPTR);
    ~SARibbonGalleryGroupModel();
    virtual int rowCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const Q_DECL_OVERRIDE;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;
    void clear();
    SARibbonGalleryItem* at(int row) const;
    void insert(int row, SARibbonGalleryItem* item);
    SARibbonGalleryItem* take(int row);
    void append(SARibbonGalleryItem* item);

private:
    QList< SARibbonGalleryItem* > m_items;
};

class SARibbonGalleryGroupPrivate;

///
/// \brief Gallery的组
///
/// 组负责显示管理Gallery Item
///
class SA_RIBBON_EXPORT SARibbonGalleryGroup : public QListView
{
    Q_OBJECT
public:
    /**
     * @brief GalleryGroup显示的样式
     */
    enum GalleryGroupStyle
    {
        IconWithText,          ///< 图标带文字
        IconWithWordWrapText,  ///< 图标带文字,文字会换行显示，此模式只会对DisplayOneRow生效，如果不是DisplayOneRow，等同IconWithText
        IconOnly               ///< 只有图标
    };

    /**
     * @brief 定义Gallery在一个pannel下面显示的图标行数
     */
    enum DisplayRow
    {
        DisplayOneRow   = 1,  ///< 显示1行，默认
        DisplayTwoRow   = 2,
        DisplayThreeRow = 3
    };

    SARibbonGalleryGroup(QWidget* w = 0);

    virtual ~SARibbonGalleryGroup();
    //重新计算grid尺寸
    void setRecalcGridSizeBlock(bool on = true);
    bool isRecalcGridSizeBlock() const;
    void recalcGridSize();
    void recalcGridSize(int galleryHeight);
    //设置显示的样式
    void setGalleryGroupStyle(GalleryGroupStyle style);
    GalleryGroupStyle getGalleryGroupStyle() const;
    //添加一个item
    void addItem(const QString& text, const QIcon& icon);
    void addItem(SARibbonGalleryItem* item);
    //以一个aciton作为item添加
    void addActionItem(QAction* act);
    void addActionItemList(const QList< QAction* >& acts);

    //构建一个model，这个model的父类是SARibbonGalleryGroup，如果要共享model，需要手动处理model的父类
    void setupGroupModel();
    SARibbonGalleryGroupModel* groupModel();
    // 标题
    void setGroupTitle(const QString& title);
    QString getGroupTitle() const;
    void selectByIndex(int i);
    //设置显示的行数
    void setDisplayRow(DisplayRow r);
    DisplayRow getDisplayRow() const;
    //设置grid最小的宽度，默认为0（不限制），可以限定grid的宽度
    void setGridMinimumWidth(int w);
    int getGridMinimumWidth() const;
    //设置grid最大的宽度，默认为0（不限制），可以限定grid的宽度
    void setGridMaximumWidth(int w);
    int getGridMaximumWidth() const;
    //获取SARibbonGalleryGroup管理的actiongroup
    QActionGroup* getActionGroup() const;
private slots:
    void onItemClicked(const QModelIndex& index);
    void onItemEntered(const QModelIndex& index);
signals:
    void groupTitleChanged(const QString& title);
    /**
     * @brief 等同QActionGroup的triggered
     * 所有加入SARibbonGalleryGroup的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
     * @param action
     */
    void triggered(QAction* action);
    /**
     * @brief 等同QActionGroup的triggered
     * 所有加入SARibbonGalleryGroup的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
     * @note 此属性需要通过QAbstractItemView::entered(const QModelIndex &index)激活，因此要保证设置了setMouseTracking(true)
     * @param action
     */
    void hovered(QAction* action);

private:
    SARibbonGalleryGroupPrivate* m_d;
};

#endif  // SARIBBONGALLERYGROUP_H
