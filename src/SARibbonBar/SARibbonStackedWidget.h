#ifndef SARIBBONSTACKEDWIDGET_H
#define SARIBBONSTACKEDWIDGET_H
#include <QStackedWidget>
#include "SARibbonGlobal.h"
class QHideEvent;
class QResizeEvent;

/**
 * @class SARibbonStackedWidget
 * @brief 具有弹出式菜单功能的堆叠窗口部件
 *
 * 此类扩展了 QStackedWidget，添加了两种工作模式：
 * 1. 正常模式：作为常规的堆叠部件，嵌入在父窗口布局中
 * 2. 弹出模式：作为独立的弹出窗口，可用于实现类似菜单的显示效果
 *
 * 在弹出模式下，窗口将显示为无边框面板，并支持模态事件循环（exec()方法），
 * 这使得它可以像菜单或对话框一样使用。
 *
 * @note 切换模式时会自动处理窗口状态和事件循环
 * @sa QStackedWidget
 */
class SA_RIBBON_EXPORT SARibbonStackedWidget : public QStackedWidget
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonStackedWidget)
public:
	Q_PROPERTY(int animationWidgetHeight READ animationWidgetHeight WRITE setAnimationWidgetHeight)  // 添加自定义属性
public:
	explicit SARibbonStackedWidget(QWidget* parent);
	~SARibbonStackedWidget();
	// 弹出模式
	void setPopupMode();
	bool isPopupMode() const;

	// 正常模式 和普通的stackwidget一样
	void setNormalMode();
	bool isNormalMode() const;

	// 在弹出模式下以模态方式运行事件循环
	void exec();

	// 类似tabbar的moveTab函数，交换两个窗口的index
	void moveWidget(int from, int to);

	// 是否启用弹出动画
	void setUseAnimation(bool on);
	bool isUseAnimation() const;

	// 动画持续时间（毫秒）
	void setAnimationDuration(int duration);
	int animationDuration() const;

	// 窗口高度
	int animationWidgetHeight() const;
	void setAnimationWidgetHeight(int h);

	// 设置窗口normalGeometry，由于此窗口会有动画，防止动画过程中设置尺寸又被动画覆盖，因此此窗口的尺寸设置使用setNormalSize
	void setNormalGeometry(const QRect& normalGeometry);
	QRect normalGeometry() const;

	// 对内部窗口发送布局请求
	void layoutRequestInnerWidgets();
Q_SIGNALS:
	/**
	 * @brief 隐藏窗口信号
	 */
	void hidWindow();

protected:
	void showEvent(QShowEvent* e) override;
	void hideEvent(QHideEvent* e) override;
	virtual void resizeEvent(QResizeEvent* e) override;
	// 同步内部窗口的尺寸
	void updateInnerWidgetGeometry();
private Q_SLOTS:
	// 动画完成槽函数
	void onAnimationFinished();
	// 建立动画
	void setupAnimation();
};

#endif  // SARIBBONSTACKEDWIDGET_H
