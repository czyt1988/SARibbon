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
    SARibbonGalleryGroupItemDelegate(SARibbonGalleryGroup* group,QObject *parent = Q_NULLPTR);
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
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
    SARibbonGalleryGroupModel(QObject *parent = Q_NULLPTR);
    ~SARibbonGalleryGroupModel();
    virtual int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE;
    void clear();
    SARibbonGalleryItem * at(int row) const;
    void insert(int row, SARibbonGalleryItem *item);
    SARibbonGalleryItem * take(int row);
    void append(SARibbonGalleryItem *item);
private:
    QList<SARibbonGalleryItem*> m_items;
};
///
/// \brief Gallery的组
///
/// 组负责显示管理Gallery Item，
///

class SA_RIBBON_EXPORT SARibbonGalleryGroup : public QListView
{
    Q_OBJECT
public:
    SARibbonGalleryGroup(QWidget* w = 0);

    virtual ~SARibbonGalleryGroup();

    void addItem(const QIcon& icon);
    void addItem(SARibbonGalleryItem *item);
    //构建一个model，这个model的父类是SARibbonGalleryGroup，如果要共享model，需要手动处理model的父类
    void setupGroupModel();
    SARibbonGalleryGroupModel* groupModel();
};


#endif // SARIBBONGALLERYGROUP_H
