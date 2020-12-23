﻿#ifndef SARIBBONCATEGORY_H
#define SARIBBONCATEGORY_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include "SARibbonPannel.h"
#include <QScopedPointer>
class SARibbonCategoryProxyPrivate;
class SARibbonCategoryProxy;
class QHBoxLayout;
#define NOT_USE_LAYOUT 1
///
/// \brief 一项ribbon页
///
class SA_RIBBON_EXPORT SARibbonCategory : public QWidget
{
    Q_OBJECT
public:
    SARibbonCategory(QWidget* parent);
    ~SARibbonCategory();
    void setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m);
    SARibbonPannel::PannelLayoutMode ribbonPannelLayoutMode() const;

    SARibbonPannel* addPannel(const QString& title);
    void addPannel(SARibbonPannel* pannel);
    void setBackgroundBrush(const QBrush& brush);
    SARibbonCategoryProxy* proxy();
    const SARibbonCategoryProxy* proxy() const;
    void setProxy(SARibbonCategoryProxy* proxy);
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    QScopedPointer<SARibbonCategoryProxy> m_proxy;
};

///
/// \brief ribbon页的代理类
/// 如果需要修改重绘SARibbonCategory，可以通过设置SARibbonCategory::setProxy
///
class SA_RIBBON_EXPORT SARibbonCategoryProxy : public QObject
{
    Q_OBJECT
public:
    SARibbonCategoryProxy(SARibbonCategory* parent);
    virtual ~SARibbonCategoryProxy();

    virtual SARibbonPannel* addPannel(const QString& title);
    virtual void addPannel(SARibbonPannel* pannel);
    virtual void setBackgroundBrush(const QBrush& brush);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    SARibbonCategory* ribbonCategory();
    void setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m);
    SARibbonPannel::PannelLayoutMode ribbonPannelLayoutMode() const;
#if NOT_USE_LAYOUT
    virtual void resizePannels(const QSize &categorySize);
protected:
    int buildReduceModePannel(SARibbonPannel* realPannel, int x, int y);
    static QPoint calcPopupPannelPosition(SARibbonCategory* category,SARibbonPannel *pannel, int x);
#endif
private:
    SARibbonCategoryProxyPrivate* m_d;
};

#endif // SARIBBONCATEGORY_H
