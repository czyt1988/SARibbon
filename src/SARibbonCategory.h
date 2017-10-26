#ifndef SARIBBONCATEGORY_H
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
    SARibbonPannel* addPannel(const QString& title);
    void setBackgroundBrush(const QBrush& brush);
    SARibbonCategoryProxy* proxy();
    void setProxy(SARibbonCategoryProxy* proxy);
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    QScopedPointer<SARibbonCategoryProxy> m_proxy;
    QHBoxLayout* m_pannelLayout;
};

///
/// \brief ribbon页的代理类
/// 如果需要修改重绘SARibbonCategory，可以通过设置SARibbonCategory::setProxy
///
class SA_RIBBON_EXPORT SARibbonCategoryProxy
{
public:
    SARibbonCategoryProxy(SARibbonCategory* parent);
    virtual ~SARibbonCategoryProxy();

    virtual SARibbonPannel* addPannel(const QString& title);
    virtual void setBackgroundBrush(const QBrush& brush);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    SARibbonCategory* ribbonCategory();
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
