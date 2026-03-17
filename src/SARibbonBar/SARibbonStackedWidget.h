#ifndef SARIBBONSTACKEDWIDGET_H
#define SARIBBONSTACKEDWIDGET_H
#include <QStackedWidget>
#include "SARibbonGlobal.h"
class QHideEvent;
class QResizeEvent;

/**
 * \if ENGLISH
 * @class SARibbonStackedWidget
 * @brief Stacked widget with popup menu functionality
 * @details This class extends QStackedWidget and adds two working modes:
 * @details 1. Normal mode: As a regular stacked widget, embedded in the parent window layout
 * @details 2. Popup mode: As an independent popup window, can be used to implement menu-like display effects
 * @details In popup mode, the window will be displayed as a frameless panel and supports modal event loop (exec() method),
 * @details which makes it usable like a menu or dialog.
 * @note Switching modes will automatically handle window state and event loop
 * @sa QStackedWidget
 * \endif
 *
 * \if CHINESE
 * @class SARibbonStackedWidget
 * @brief 具有弹出式菜单功能的堆叠窗口部件
 * @details 此类扩展了 QStackedWidget，添加了两种工作模式：
 * @details 1. 正常模式：作为常规的堆叠部件，嵌入在父窗口布局中
 * @details 2. 弹出模式：作为独立的弹出窗口，可用于实现类似菜单的显示效果
 * @details 在弹出模式下，窗口将显示为无边框面板，并支持模态事件循环（exec()方法），
 * @details 这使得它可以像菜单或对话框一样使用。
 * @note 切换模式时会自动处理窗口状态和事件循环
 * @sa QStackedWidget
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonStackedWidget : public QStackedWidget
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonStackedWidget)
public:
	Q_PROPERTY(int animationWidgetHeight READ animationWidgetHeight WRITE setAnimationWidgetHeight)  // Add custom property
public:
	/// Constructor for SARibbonStackedWidget
	explicit SARibbonStackedWidget(QWidget* parent);

	/// Destructor for SARibbonStackedWidget
	~SARibbonStackedWidget();

	/// Set popup mode
	void setPopupMode();

	/// Check if currently in popup mode
	bool isPopupMode() const;

	/// Set normal mode, same as normal stackwidget
	void setNormalMode();

	/// Check if currently in normal mode
	bool isNormalMode() const;

	/// Run event loop modally in popup mode
	void exec();

	/// Similar to tabbar's moveTab function, swap the index of two widgets
	void moveWidget(int from, int to);

	/// Set whether to enable popup animation
	void setUseAnimation(bool on);

	/// Check if animation is enabled
	bool isUseAnimation() const;

	/// Set animation duration (milliseconds)
	void setAnimationDuration(int duration);

	/// Get animation duration
	int animationDuration() const;

	/// Get animation widget height
	int animationWidgetHeight() const;

	/// Set animation widget height
	void setAnimationWidgetHeight(int h);

	/// Set window normalGeometry, since this window has animation, to prevent size setting from being overwritten by animation during animation process
	void setNormalGeometry(const QRect& normalGeometry);

	/// Get normal geometry
	QRect normalGeometry() const;

	/// Send layout request to inner widgets
	void layoutRequestInnerWidgets();
Q_SIGNALS:
	/**
	 * \if ENGLISH
	 * @brief Hide window signal
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 隐藏窗口信号
	 * \endif
	 */
	void hidWindow();

protected:
	/// Handle show event
	void showEvent(QShowEvent* e) override;

	/// Handle hide event
	void hideEvent(QHideEvent* e) override;

	/// Handle resize event
	virtual void resizeEvent(QResizeEvent* e) override;

	/// Update inner widget geometry
	void updateInnerWidgetGeometry();
private Q_SLOTS:
	/// Animation finished slot function
	void onAnimationFinished();

	/// Setup animation
	void setupAnimation();
};

#endif  // SARIBBONSTACKEDWIDGET_H
