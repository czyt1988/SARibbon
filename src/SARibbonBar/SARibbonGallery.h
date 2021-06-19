﻿#ifndef SARIBBONGALLERY_H
#define SARIBBONGALLERY_H
#include "SARibbonGlobal.h"
#include <QFrame>
#include "SARibbonGalleryGroup.h"
class QVBoxLayout;
class SARibbonGalleryPrivate;
class RibbonGalleryViewport;

///
/// \brief Gallery控件
///
class SA_RIBBON_EXPORT SARibbonGallery : public QFrame
{
    Q_OBJECT
public:
    SARibbonGallery(QWidget* parent = 0);
    virtual ~SARibbonGallery();
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    SARibbonGalleryGroup* addGalleryGroup();
    void addGalleryGroup(SARibbonGalleryGroup* group);
    SARibbonGalleryGroup* addCategoryActions(const QString& title,QList<QAction *> actions);
    void setCurrentViewGroup(SARibbonGalleryGroup* group);
    SARibbonGalleryGroup* currentViewGroup() const;
    QActionGroup* getActionGroup() const;
signals:
    /**
     * @brief 等同QActionGroup的triggered
     * 所有加入SARibbonGallery的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
     * @param action
     */
    void triggered(QAction *action);
    void hovered(QAction *action);
protected slots:
    virtual void onPageDown();
    virtual void onPageUp();
    virtual void onShowMoreDetail();
    void onItemClicked(const QModelIndex &index);
private:
    RibbonGalleryViewport* ensureGetPopupViewPort();
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    friend class SARibbonGalleryPrivate;
    SARibbonGalleryPrivate* m_d;
};

///
/// \brief SARibbonGallery的Viewport class
///
class RibbonGalleryViewport : public QWidget
{
    Q_OBJECT
public:
    RibbonGalleryViewport(QWidget* parent);
    void addWidget(QWidget* w);
private:
    QVBoxLayout* m_layout;
};

#endif // SARIBBONGALLERY_H
