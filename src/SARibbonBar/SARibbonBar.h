#ifndef SARIBBONBAR_H
#define SARIBBONBAR_H
#include "SARibbonGlobal.h"
#include <QMenuBar>
#include <QVariant>
#include "SARibbonCategory.h"
#include "SARibbonContextCategory.h"
#include <QScopedPointer>

class QAbstractButton;
class SARibbonElementCreateDelegate;
class SARibbonBarPrivate;
class SARibbonTabBar;
class SARibbonButtonGroupWidget;
class SARibbonQuickAccessBar;
///
/// \brief The SARibbonBar class
///
class SA_RIBBON_EXPORT SARibbonBar : public QMenuBar
{
    Q_OBJECT
public:
    /**
     * @brief 定义ribbon的风格
     */
    enum RibbonStyle {
        OfficeStyle = 0x00         ///< 类似office 的ribbon风格
        , WpsLiteStyle = 0x01      ///< 类似wps的紧凑风格
        ,OfficeStyleTwoRow = 0x10  ///< 类似office 的ribbon风格 2行工具栏 三行布局模式，office就是三行布局模式，pannel能布置3行小toolbutton，默认模式
        ,WpsLiteStyleTwoRow = 0x11 ///< 类似wps的紧凑风格  2行工具栏
    };
    Q_FLAG(RibbonStyle)
    /**
     * @brief 定义当前ribbon 的状态
     */
    enum RibbonState {
        MinimumRibbonMode       ///< 缩小模式
        , NormalRibbonMode      ///< 正常模式
    };

    SARibbonBar(QWidget *parent);

    //获取applitionButton
    QAbstractButton *applitionButton();

    //设置applitionButton
    void setApplitionButton(QAbstractButton *btn);

    //获取tabbar
    SARibbonTabBar *ribbonTabBar();

    //添加一个标签
    SARibbonCategory *addCategoryPage(const QString& title);

    //添加一个上下文标签
    SARibbonContextCategory *addContextCategory(const QString& title, const QColor& color, const QVariant& id = QVariant());

    //显示一个上下文标签
    void showContextCategory(SARibbonContextCategory *context);

    //隐藏一个上下文标签
    void hideContextCategory(SARibbonContextCategory *context);

    //判断上下文是否是在显示状态
    bool isContextCategoryVisible(SARibbonContextCategory *context);

    //设置上下文标签的显示或隐藏
    void setContextCategoryVisible(SARibbonContextCategory *context, bool visible);

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

    //标题栏的高度
    int titleBarHeight() const;

    //激活tabbar右边的按钮群
    SARibbonButtonGroupWidget *activeTabBarRightButtonGroup();

    //快速响应栏
    SARibbonQuickAccessBar *quickAccessBar();

    //设置ribbon的风格
    void setRibbonStyle(RibbonStyle v);
    RibbonStyle currentRibbonStyle() const;

    //当前的模式
    RibbonState currentRibbonState() const;

    //获取右边不可用区域，只有在wps模式下有用
    int unusableTitleRegion() const;
    void setUnusableTitleRegion(int v);

	//设置当前ribbon的index
	void setCurrentIndex(int index);
	
	//确保标签显示出来
	void raiseCategory(SARibbonCategory* category);
signals:

    /**
     * @brief 应用按钮点击响应 - 左上角的按钮，通过关联此信号触发应用按钮点击的效果
     */
    void applitionButtonClicked();

    /**
     * @brief 标签页变化触发的信号
     * @param index
     */
    void currentRibbonTabChanged(int index);

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;

    //根据情况重置tabbar的宽度，主要针对wps模式
    int calcMinTabBarWidth() const;

protected slots:
    void onWindowTitleChanged(const QString& title);
    void onWindowIconChanged(const QIcon& icon);
    void onCategoryWindowTitleChanged(const QString& title);
    void onStackWidgetHided();
    virtual void onCurrentRibbonTabChanged(int index);
    virtual void onCurrentRibbonTabClicked(int index);
    virtual void onCurrentRibbonTabDoubleClicked(int index);
    void onContextsCategoryPageAdded(SARibbonCategory *category);

private:
    void updateRibbonElementGeometry();
    void updateRibbonElementGeometry(RibbonStyle style);
    void resizeInNormalStyle();
    void resizeInWpsLiteStyle();
    void paintInNormalStyle();
    void paintInWpsLiteStyle();

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

    virtual void paintBackground(QPainter& painter);
    virtual void paintWindowTitle(QPainter& painter, const QString& title, const QRect& titleRegion);
    virtual void paintWindowIcon(QPainter& painter, const QIcon& icon);
    virtual void paintContextCategoryTab(QPainter& painter, const QString& title, QRect contextRect, const QColor& color);

private:
    friend class SARibbonBarPrivate;
    SARibbonBarPrivate *m_d;
};



#endif // SARIBBONBAR_H
