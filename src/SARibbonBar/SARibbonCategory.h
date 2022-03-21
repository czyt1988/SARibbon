#ifndef SARIBBONCATEGORY_H
#define SARIBBONCATEGORY_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include "SARibbonPannel.h"
#include <QScopedPointer>
#include <QPushButton>
#include <QWheelEvent>
class SARibbonCategoryProxyPrivate;
class SARibbonCategoryPrivate;
class QHBoxLayout;
class QWheelEvent;
class SARibbonBar;

/**
 * @brief 一项ribbon tab页
 * @note SARibbonCategory的windowTitle影响了其在SARibbonBar的标签显示，
 * 如果要改标签名字，直接调用SARibbonCategory的setWindowTitle函数
 */
class SA_RIBBON_EXPORT SARibbonCategory : public QWidget
{
    Q_OBJECT
    friend class SARibbonBar;
    friend class SARibbonContextCategory;
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(QString categoryName READ categoryName WRITE setCategoryName)
public:
    SARibbonCategory(QWidget* p = nullptr);
    ~SARibbonCategory();

    // category的名字
    QString categoryName() const;

    //设置category名字，等同setWindowTitle
    void setCategoryName(const QString& title);

    //布局模式
    SARibbonPannel::PannelLayoutMode ribbonPannelLayoutMode() const;

    //添加pannel
    SARibbonPannel* addPannel(const QString& title);

    //添加pannel
    void addPannel(SARibbonPannel* pannel);

    // qt designer专用
    Q_INVOKABLE void addPannel(QWidget* pannel);

    //插入pannel
    SARibbonPannel* insertPannel(const QString& title, int index);

    //通过名字查找pannel
    SARibbonPannel* pannelByName(const QString& title) const;

    //通过ObjectName查找pannel
    SARibbonPannel* pannelByObjectName(const QString& objname) const;

    //通过索引找到pannel，如果超过索引范围，会返回nullptr
    SARibbonPannel* pannelByIndex(int index) const;

    //查找pannel的index
    int pannelIndex(SARibbonPannel* p) const;

    //移动一个Pannel从from index到to index
    void movePannel(int from, int to);

    //把pannel从Category中移除，不会销毁，此时pannel的所有权归还操作者
    bool takePannel(SARibbonPannel* pannel);

    //移除Pannel，Category会直接回收SARibbonPannel内存
    bool removePannel(SARibbonPannel* pannel);
    bool removePannel(int index);

    //设置背景
    void setBackgroundBrush(const QBrush& brush);

    //返回所有的Pannel
    QList< SARibbonPannel* > pannelList() const;

    //
    QSize sizeHint() const Q_DECL_OVERRIDE;

    //如果是ContextCategory，此函数返回true
    bool isContextCategory() const;

    // pannel的个数
    int pannelCount() const;

    //判断是否可以自定义
    bool isCanCustomize() const;
    void setCanCustomize(bool b);

    //获取对应的ribbonbar，如果没有加入ribbonbar的管理，此值为null
    SARibbonBar* ribbonBar() const;

    //刷新category的尺寸布局
    void updateItemGeometry();
protected slots:
    void onLeftScrollButtonClicked();
    void onRightScrollButtonClicked();

protected:
    //事件处理
    bool event(QEvent* e) Q_DECL_OVERRIDE;

    //
    void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;

    //设置pannel的模式
    void setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m);
    bool eventFilter(QObject* watched, QEvent* event) Q_DECL_OVERRIDE;

    //处理滚轮事件
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;

    //标记这个是上下文标签
    void markIsContextCategory(bool isContextCategory = true);

private:
    void setRibbonBar(SARibbonBar* bar);

    SARibbonCategoryPrivate* m_d;
};

/**
 * @brief SARibbonCategory无法完全显示时，显示的调整按钮
 *
 * 重新定义是为了防止被外部的样式影响,同时可以使用SARibbonCategoryScrollButton的样式定义
 */
class SA_RIBBON_EXPORT SARibbonCategoryScrollButton : public QToolButton
{
    Q_OBJECT
public:
    SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget* p = nullptr);
};

#endif  // SARIBBONCATEGORY_H
