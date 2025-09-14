#ifndef SARIBBONCATEGORYLAYOUT_H
#define SARIBBONCATEGORYLAYOUT_H
#include "SARibbonGlobal.h"
#include <QLayout>
#include <QList>
#include <QMap>
#include "SARibbonCategory.h"

class SARibbonPanel;
class SARibbonCategoryLayoutItem;
class SARibbonSeparatorWidget;

/**
 * @brief The SARibbonCategoryLayout class
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayout : public QLayout
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonCategoryLayout)
public:
	Q_PROPERTY(int scrollPosition READ scrollPosition WRITE setScrollPosition)
public:
	explicit SARibbonCategoryLayout(SARibbonCategory* parent);
	~SARibbonCategoryLayout();

	SARibbonCategory* ribbonCategory() const;

	virtual void addItem(QLayoutItem* item) override;
	virtual QLayoutItem* itemAt(int index) const override;
	virtual QLayoutItem* takeAt(int index) override;
	SARibbonCategoryLayoutItem* takePanelItem(int index);
	SARibbonCategoryLayoutItem* takePanelItem(SARibbonPanel* panel);
	bool takePanel(SARibbonPanel* panel);
	virtual int count() const override;
	void setGeometry(const QRect& rect) override;
	QSize sizeHint() const override;
	QSize minimumSize() const override;
	Qt::Orientations expandingDirections() const override;
	void invalidate() override;
	// 追加一个panel
	void addPanel(SARibbonPanel* panel);
	// 插入一个panel
	void insertPanel(int index, SARibbonPanel* panel);
	//
	QSize categoryContentSize() const;
	// 更新尺寸
	void updateGeometryArr();

	// 执行位置调整
	void doLayout();

	// 返回所有panels
	QList< SARibbonPanel* > panels() const;

	// 通过obj name获取panel
	SARibbonPanel* panelByObjectName(const QString& objname) const;
	// 通过panel name获取panel
	SARibbonPanel* panelByName(const QString& panelname) const;
	// 通过索引找到panel，如果超过索引范围，会返回nullptr
	SARibbonPanel* panelByIndex(int i) const;
	// 移动panel
	void movePanel(int from, int to);
	// panel的数量
	int panelCount() const;
	// panel的索引
	int panelIndex(SARibbonPanel* p) const;
	// 获取所有的panel
	QList< SARibbonPanel* > panelList() const;
	// 执行滚轮事件
	void scroll(int px);
	void scrollTo(int targetX);
	// 带动画的滚动
	void scrollByAnimate(int px);
	void scrollToByAnimate(int targetX);
	// 滚动后的位置
	int scrollPosition() const;
	// 设置滚动位置
	void setScrollPosition(int pos);
	// 是否在滚动动画中
	bool isAnimatingScroll() const;
	// 判断是否有滚动过
	bool isScrolled() const;
	// category的总宽度
	int categoryTotalWidth() const;
	// 设置Category的对齐方式
	void setCategoryAlignment(SARibbonAlignment al);
	SARibbonAlignment categoryAlignment() const;
	// 设置动画持续时间
	void setAnimationDuration(int duration);
	int animationDuration() const;
private Q_SLOTS:
	void onLeftScrollButtonClicked();
	void onRightScrollButtonClicked();

private:
	void setupAnimateScroll();
};

/**
 * @brief SARibbonCategoryLayoutItem，用于标识SARibbonCategoryLayout的item
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayoutItem : public QWidgetItem
{
public:
	explicit SARibbonCategoryLayoutItem(SARibbonPanel* w);
	~SARibbonCategoryLayoutItem();
	SARibbonSeparatorWidget* separatorWidget;
	// 把内部的widget转换为panel
	SARibbonPanel* toPanelWidget();
	QRect mWillSetGeometry;           ///< panel将要设置的Geometry
	QRect mWillSetSeparatorGeometry;  ///< panel将要设置的Separator的Geometry
};
#endif  // SARIBBONCATEGORYLAYOUT_H
