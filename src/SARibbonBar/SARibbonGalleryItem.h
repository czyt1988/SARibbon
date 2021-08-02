﻿#ifndef SARIBBONGALLERYITEM_H
#define SARIBBONGALLERYITEM_H
#include "SARibbonGlobal.h"
#include <QIcon>
#include <QVariant>
#include <QMap>
#include <QAction>
class SARibbonGalleryGroup;

///
/// \brief 类似QStandardItem的GalleryItem
///
class SA_RIBBON_EXPORT SARibbonGalleryItem
{
public:
    SARibbonGalleryItem();
    SARibbonGalleryItem(const QString& text, const QIcon& icon);
    SARibbonGalleryItem(QAction *act);
    virtual ~SARibbonGalleryItem();
    //设置角色
    void setData(int role, const QVariant& data);
    virtual QVariant data(int role) const;

    //设置文字描述
    void setText(const QString& text);
    QString text() const;

    //设置tooltip
    void setToolTip(const QString& text);
    QString toolTip() const;

    //设置图标
    void setIcon(const QIcon& ico);
    QIcon icon() const;

    //设置是否可见
    bool isSelectable() const;
    void setSelectable(bool isSelectable);

    //设置是否可选
    bool isEnable() const;
    void setEnable(bool isEnable);

    //设置item的flag
    void setFlags(Qt::ItemFlags flag);
    virtual Qt::ItemFlags flags() const;

    //设置action
    void setAction(QAction *act);
    QAction *action();

private:
    friend class SARibbonGalleryGroupModel;
    QMap<int, QVariant> m_datas;
    Qt::ItemFlags m_flags;
    QAction *m_action;
};

#endif // SARIBBONGALLERYITEM_H
