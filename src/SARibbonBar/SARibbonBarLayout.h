#ifndef SARIBBONBARLAYOUT_H
#define SARIBBONBARLAYOUT_H

#include "SARibbonGlobal.h"
#include "SARibbonBar.h"
#include <QLayout>
#include <QPointer>

class SARibbonTabBar;
class SARibbonStackedWidget;
class SARibbonQuickAccessBar;
class SARibbonButtonGroupWidget;
class SARibbonContextCategory;

class SA_RIBBON_EXPORT SARibbonBarLayout : public QLayout
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonBarLayout)
public:
	explicit SARibbonBarLayout(SARibbonBar* parent);
	virtual ~SARibbonBarLayout();

	// QLayout 必须实现的接口
	void addItem(QLayoutItem* item) override;
	QLayoutItem* itemAt(int index) const override;
	QLayoutItem* takeAt(int index) override;
	int count() const override;
	QSize sizeHint() const override;
	QSize minimumSize() const override;
	void setGeometry(const QRect& rect) override;

	// 执行布局
	virtual void doLayout();

	// 样式相关
	bool isLooseStyle() const;
	bool isCompactStyle() const;

	// 标题区域
	const QRect& titleRect() const;

	// 计算最小TabBar宽度
	int calcMinTabBarWidth() const;

	// 重置尺寸，这个是在ribbon模式发生改变的时候调用，例如setMinimumMode或者setNormalMode,此函数会触发重新布局
	void resetSize();

	// 更新标题区域
	void layoutTitleRect();

	// 调整StackedContainerWidget的位置
	void layoutStackedContainerWidget();

	// 让category重新布局，这个函数在调整category的对其方式的时候调用，由于对其方式改变StackedContainerWidget的尺寸没有改变，但category要重新布局
	void layoutCategory();

	// 设置系统按钮大小
	void setSystemButtonSize(const QSize& size);

	// 设置tab在title上面，这样可以省略title区域
	void setTabOnTitle(bool on);
	bool isTabOnTitle() const;

	// 最小模式下的MainBar高度
	int minimumModeMainBarHeight() const;

	// 获取正常模式下的mainBar的高度
	int normalModeMainBarHeight() const;

	// ribbon tab的高度
	int tabBarHeight() const;
	void setTabBarHeight(int h);

	// 标题栏的高度
	int titleBarHeight() const;
	void setTitleBarHeight(int h);

	// category的高度
	int categoryHeight() const;
	void setCategoryHeight(int h);

	// 设置panel的标题栏高度
	int panelTitleHeight() const;
	void setPanelTitleHeight(int h);

    // 设置图标
    void setWindowIcon(const QIcon& icon);
    QIcon windowIcon() const;

	// 获取元素
	SARibbonBar* ribbonBar() const;
	SARibbonTabBar* ribbonTabBar() const;
	SARibbonStackedWidget* stackedContainerWidget() const;
	SARibbonQuickAccessBar* quickAccessBar() const;
	SARibbonButtonGroupWidget* rightButtonGroup() const;
	QAbstractButton* applicationButton() const;
    SARibbonTitleIconWidget* titleIconWidget() const;

protected:
	void resizeInLooseStyle();
	void resizeInCompactStyle();

private:
	void init();
};

#endif  // SARIBBONBARLAYOUT_H
