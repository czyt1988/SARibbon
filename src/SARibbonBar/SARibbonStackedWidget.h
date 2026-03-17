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
	/**
	 * \if ENGLISH
	 * @brief Constructor for SARibbonStackedWidget
	 * @param parent Parent widget
	 * \endif
	 *
	 * \if CHINESE
	 * @brief SARibbonStackedWidget 构造函数
	 * @param parent 父窗口
	 * \endif
	 */
	explicit SARibbonStackedWidget(QWidget* parent);

	/**
	 * \if ENGLISH
	 * @brief Destructor for SARibbonStackedWidget
	 * \endif
	 *
	 * \if CHINESE
	 * @brief SARibbonStackedWidget 析构函数
	 * \endif
	 */
	~SARibbonStackedWidget();

	/**
	 * \if ENGLISH
	 * @brief Set popup mode
	 * @details In this mode, the window will:
	 * @details - Set Qt::Popup flag
	 * @details - Frameless window
	 * @details - Display as panel style
	 * @note Switching modes will automatically handle window hiding and showing
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 设置弹出模式
	 * @details 在此模式下，窗口将：
	 * @details - 设置为 Qt::Popup 标志
	 * @details - 无边框窗口
	 * @details - 显示为面板样式
	 * @note 切换模式时会自动处理窗口隐藏和显示
	 * \endif
	 */
	void setPopupMode();

	/**
	 * \if ENGLISH
	 * @brief Check if currently in popup mode
	 * @return true if in popup mode, false otherwise
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 检查当前是否处于弹出模式
	 * @return 如果是弹出模式返回 true，否则返回 false
	 * \endif
	 */
	bool isPopupMode() const;

	/**
	 * \if ENGLISH
	 * @brief Set normal mode, same as normal stackwidget
	 * @details In this mode, the window will:
	 * @details - Set to regular widget mode (Qt::Widget)
	 * @details - Frameless window
	 * @details - No frame display
	 * @note If there is a modal event loop, it will exit that loop
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 设置正常模式 和普通的stackwidget一样
	 * @details 在此模式下，窗口将：
	 * @details - 设置为常规部件模式 (Qt::Widget)
	 * @details - 无边框窗口
	 * @details - 不显示框架
	 * @note 如果存在模态事件循环，将退出该循环
	 * \endif
	 */
	void setNormalMode();

	/**
	 * \if ENGLISH
	 * @brief Check if currently in normal mode
	 * @return true if in normal mode, false otherwise
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 检查当前是否处于正常模式
	 * @return 如果是正常模式返回 true，否则返回 false
	 * \endif
	 */
	bool isNormalMode() const;

	/**
	 * \if ENGLISH
	 * @brief Run event loop modally in popup mode
	 * @details This method is only valid in popup mode:
	 * @details 1. Show window
	 * @details 2. Start local event loop
	 * @details 3. Exit event loop when window is hidden
	 * @note Only show window in normal mode
	 * @warning Do not call this method again in event loop
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 在弹出模式下以模态方式运行事件循环
	 * @details 此方法仅在弹出模式下有效：
	 * @details 1. 显示窗口
	 * @details 2. 启动局部事件循环
	 * @details 3. 窗口隐藏时退出事件循环
	 * @note 在正常模式下调用此方法仅显示窗口
	 * @warning 不要在事件循环中再次调用此方法
	 * \endif
	 */
	void exec();

	/**
	 * \if ENGLISH
	 * @brief Similar to tabbar's moveTab function, swap the index of two widgets
	 * @param from Source index
	 * @param to Target index
	 * @note This operation will trigger widgetRemoved(int index) signal
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 类似tabbar的moveTab函数，交换两个窗口的index
	 * @param from 源索引
	 * @param to 目标索引
	 * @note 此操作会触发widgetRemoved(int index)信号
	 * \endif
	 */
	void moveWidget(int from, int to);

	/**
	 * \if ENGLISH
	 * @brief Set whether to enable popup animation
	 * @param on Enable or disable
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 设置是否启用弹出动画
	 * @param on 是否启用
	 * \endif
	 */
	void setUseAnimation(bool on);

	/**
	 * \if ENGLISH
	 * @brief Get animation enabled state
	 * @return Animation enabled state
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 获取动画启用状态
	 * @return 动画启用状态
	 * \endif
	 */
	bool isUseAnimation() const;

	/**
	 * \if ENGLISH
	 * @brief Set animation duration (milliseconds)
	 * @param duration Duration in milliseconds
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 设置动画持续时间（毫秒）
	 * @param duration 持续时间（毫秒）
	 * \endif
	 */
	void setAnimationDuration(int duration);

	/**
	 * \if ENGLISH
	 * @brief Get animation duration
	 * @return Animation duration in milliseconds
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 获取动画持续时间
	 * @return 动画持续时间（毫秒）
	 * \endif
	 */
	int animationDuration() const;

	/**
	 * \if ENGLISH
	 * @brief Get animation widget height
	 * @return Animation widget height
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 获取动画窗口高度
	 * @return 动画窗口高度
	 * \endif
	 */
	int animationWidgetHeight() const;

	/**
	 * \if ENGLISH
	 * @brief Set animation widget height
	 * @param h Height value
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 设置动画窗口高度
	 * @param h 高度值
	 * \endif
	 */
	void setAnimationWidgetHeight(int h);

	/**
	 * \if ENGLISH
	 * @brief Set window normalGeometry, since this window has animation, to prevent size setting from being overwritten by animation during animation process, this window's size setting uses setNormalSize
	 * @details This function is equivalent to the following when there is no animation:
	 * @details @code
	 * @details setFixedSize(normalGeometry.width(),normalGeometry.height());
	 * @details move(normalGeometry.x(),normalGeometry.y());
	 * @details @endcode
	 * @param normalGeometry
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 设置窗口normalGeometry，由于此窗口会有动画，防止动画过程中设置尺寸又被动画覆盖，因此此窗口的尺寸设置使用setNormalSize
	 * @details 此函数在没有动画的时候，等同于
	 * @details @code
	 * @details setFixedSize(normalGeometry.width(),normalGeometry.height());
	 * @details move(normalGeometry.x(),normalGeometry.y());
	 * @details @endcode
	 * @param normalGeometry
	 * \endif
	 */
	void setNormalGeometry(const QRect& normalGeometry);

	/**
	 * \if ENGLISH
	 * @brief Get normal geometry
	 * @return Normal geometry
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 获取正常几何位置
	 * @return 正常几何位置
	 * \endif
	 */
	QRect normalGeometry() const;

	/**
	 * \if ENGLISH
	 * @brief Send layout request to inner widgets
	 * @details This method will invalidate and recalculate the layout of child widgets
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 对内部窗口发送布局请求
	 * @details 这个方法会让子窗口布局失效同时重新计算布局
	 * \endif
	 */
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
	/**
	 * \if ENGLISH
	 * @brief Handle show event
	 * @param e Show event
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 处理显示事件
	 * @param e 显示事件
	 * \endif
	 */
	void showEvent(QShowEvent* e) override;

	/**
	 * \if ENGLISH
	 * @brief Handle hide event
	 * @param e Hide event
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 处理隐藏事件
	 * @param e 隐藏事件
	 * \endif
	 */
	void hideEvent(QHideEvent* e) override;

	/**
	 * \if ENGLISH
	 * @brief Handle resize event
	 * @param e Resize event
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 处理尺寸改变事件
	 * @param e 尺寸改变事件
	 * \endif
	 */
	virtual void resizeEvent(QResizeEvent* e) override;

	/**
	 * \if ENGLISH
	 * @brief Update inner widget geometry
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 更新内部窗口几何位置
	 * \endif
	 */
	void updateInnerWidgetGeometry();
private Q_SLOTS:
	/**
	 * \if ENGLISH
	 * @brief Animation finished slot function
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 动画完成槽函数
	 * \endif
	 */
	void onAnimationFinished();

	/**
	 * \if ENGLISH
	 * @brief Setup animation
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 设置动画
	 * \endif
	 */
	void setupAnimation();
};

#endif  // SARIBBONSTACKEDWIDGET_H
