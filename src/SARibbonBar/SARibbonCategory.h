#ifndef SARIBBONCATEGORY_H
#define SARIBBONCATEGORY_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include "SARibbonPannel.h"
#include <QScopedPointer>
class SARibbonCategoryProxyPrivate;
class SARibbonCategoryProxy;
class QHBoxLayout;


/**
 * @brief 一项ribbon tab页
 */
class SA_RIBBON_EXPORT SARibbonCategory : public QWidget
{
    Q_OBJECT
    friend class SARibbonBar;
public:
    SARibbonCategory(QWidget *parent);
    ~SARibbonCategory();

    SARibbonPannel::PannelLayoutMode ribbonPannelLayoutMode() const;

    //添加pannel
    SARibbonPannel *addPannel(const QString& title);

    //添加pannel
    void addPannel(SARibbonPannel *pannel);

    //设置背景
    void setBackgroundBrush(const QBrush& brush);

    //返回SARibbonCategory的代理
    SARibbonCategoryProxy *proxy();
    const SARibbonCategoryProxy *proxy() const;

    //设置SARibbonCategory的代理
    void setProxy(SARibbonCategoryProxy *proxy);

    //返回所有的Pannel
    const QList<SARibbonPannel *>& pannelList() const;
    QList<SARibbonPannel *>& pannelList();

protected:
    //设置pannel的模式
    void setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m);
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
    SARibbonCategoryProxy(SARibbonCategory *parent);
    virtual ~SARibbonCategoryProxy();

    virtual SARibbonPannel *addPannel(const QString& title);
    virtual void addPannel(SARibbonPannel *pannel);
    virtual void setBackgroundBrush(const QBrush& brush);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    SARibbonCategory *ribbonCategory();
    void setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m);
    SARibbonPannel::PannelLayoutMode ribbonPannelLayoutMode() const;
    virtual void resizePannels(const QSize& categorySize);

    //返回所有的Pannel
    virtual const QList<SARibbonPannel *>& pannelList() const;
    virtual QList<SARibbonPannel *>& pannelList();

protected:
    int buildReduceModePannel(SARibbonPannel *realPannel, int x, int y);
    static QPoint calcPopupPannelPosition(SARibbonCategory *category, SARibbonPannel *pannel, int x);

private:
    SARibbonCategoryProxyPrivate *m_d;
};

#endif // SARIBBONCATEGORY_H
