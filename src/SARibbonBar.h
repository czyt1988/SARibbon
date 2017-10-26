#ifndef SARIBBONBAR_H
#define SARIBBONBAR_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QVariant>
#include "SARibbonCategory.h"
#include "SARibbonContextCategory.h"
#include <QScopedPointer>

class SARibbonElementCreateDelegate;
class SARibbonBarPrivate;
class QAbstractButton;
class SARibbonTabBar;
class SARibbonButtonGroupWidget;
///
/// \brief The SARibbonBar class
///
class SA_RIBBON_EXPORT SARibbonBar : public QWidget
{
    Q_OBJECT
public:
    SARibbonBar(QWidget* parent);
    //设置ribbon的背景
    void setRibbonBarBackground(const QBrush& brush);
    //获取applitionButton
    QAbstractButton* applitionButton();
    //设置applitionButton
    void setApplitionButton(QAbstractButton* btn);
    //获取tabbar
    SARibbonTabBar* ribbonTabBar();
    //添加一个标签
    SARibbonCategory* addCategoryPage(const QString& title);
    //添加一个上下文标签
    SARibbonContextCategory* addContextCategory(const QString& title,const QColor& color,const QVariant& id=QVariant());
    //显示一个上下文标签
    void showContextCategory(SARibbonContextCategory* context);
    //隐藏一个上下文标签
    void hideContextCategory(SARibbonContextCategory* context);
    //设置上下文标签的显示或隐藏
    void setContextCategoryVisible(SARibbonContextCategory* context,bool visible);
    //设置为隐藏模式
    void setHideMode(bool isHide);
    //当前Ribbon是否是隐藏模式
    bool isRibbonBarHideMode() const;
    //设置显示隐藏ribbon按钮
    void showHideModeButton(bool isShow = true);
    //是否显示隐藏ribbon按钮
    bool isShowHideModeButton() const;
    //ribbon tab的高度
    int tabBarHeight() const;
    //激活tabbar右边的按钮群
    SARibbonButtonGroupWidget* activeTabBarRightButtonGroup();
signals:
    void applitionButtonClicked();
    //
    void currentRibbonTabChanged(int index);

protected slots:
    void onWindowTitleChanged(const QString &title);
    void onWindowIconChanged(const QIcon &icon);
    virtual void onCurrentRibbonTabChanged(int index);
    virtual void onCurrentRibbonTabClicked(int index);
    virtual void onCurrentRibbonTabDoubleClicked(int index);
    void onContextsCategoryPageAdded(SARibbonCategory* category);

protected:
    void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;

    virtual void paintBackground(QPainter& painter);
    virtual void paintWindowTitle(QPainter& painter, const QString &title,const QPoint& contextCategoryRegion);
    virtual void paintWindowIcon(QPainter& painter, const QIcon &icon);
    virtual void paintContextCategoryTab(QPainter& painter,const QString& title, QRect contextRect, const QColor& color);
private:
    friend class SARibbonBarPrivate;
    SARibbonBarPrivate* m_d;
};



#endif // SARIBBONBAR_H
