#include "SARibbonStackedWidget.h"
#include <QEventLoop>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPropertyAnimation>
#include <QLayout>
#ifndef SARIBBONSTACKEDWIDGET_DEBUG_PRINT
#define SARIBBONSTACKEDWIDGET_DEBUG_PRINT 0
#endif
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
#include <QDebug>
#endif
/**
 * @brief The SARibbonStackedWidgetPrivate class
 */
class SARibbonStackedWidget::PrivateData
{
	SA_RIBBON_DECLARE_PUBLIC(SARibbonStackedWidget)
public:
	QEventLoop* eventLoop { nullptr };
	bool useAnimation { false };                ///< 是否使用动画
	QPropertyAnimation* animation { nullptr };  ///< 动画对象
	QRect normalGeometry;                       ///< 正常状态下的几何位置
	bool isAnimating { false };  ///< 标记动画是否正在进行，这个必须要有，它比animation->state()更早标记动画是否启动
	int animationWidgetHeight { 0 };  ///< 动画时的窗口高度
public:
	PrivateData(SARibbonStackedWidget* p) : q_ptr(p)
	{
	}

	bool isAnimationRunning() const
	{
		return (isAnimating || animation->state() == QAbstractAnimation::Running);
	}
};

SARibbonStackedWidget::SARibbonStackedWidget(QWidget* parent)
    : QStackedWidget(parent), d_ptr(new SARibbonStackedWidget::PrivateData(this))
{
	setNormalMode();
	setupAnimation();
}

SARibbonStackedWidget::~SARibbonStackedWidget()
{
}

void SARibbonStackedWidget::setupAnimation()
{
	d_ptr->animation = new QPropertyAnimation(this, "animationWidgetHeight", this);
	d_ptr->animation->setEasingCurve(QEasingCurve::OutQuad);
	d_ptr->animation->setDuration(300);
	connect(d_ptr->animation, &QPropertyAnimation::finished, this, &SARibbonStackedWidget::onAnimationFinished);
}

int SARibbonStackedWidget::animationWidgetHeight() const
{
	return d_ptr->animationWidgetHeight;
}
void SARibbonStackedWidget::setAnimationWidgetHeight(int h)
{
	if (d_ptr->animationWidgetHeight == h) {
		return;
	}

	d_ptr->animationWidgetHeight = h;

	if (d_ptr->isAnimationRunning() && isPopupMode()) {
		// 更新窗口大小和位置
		setFixedSize(d_ptr->normalGeometry.width(), h);
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
		qDebug() << "setAnimationWidgetHeight setFixedSize=" << d_ptr->normalGeometry.width() << "," << h;
#endif
	}
}

/**
 * @brief 设置窗口normalGeometry，由于此窗口会有动画，防止动画过程中设置尺寸又被动画覆盖，因此此窗口的尺寸设置使用setNormalSize
 *
 * 此函数在没有动画的时候，等同于
 * @code
 * setFixedSize(normalGeometry.width(),normalGeometry.height());
 * move(normalGeometry.x(),normalGeometry.y());
 * @endcode
 * @param normalGeometry
 */
void SARibbonStackedWidget::setNormalGeometry(const QRect& normalGeometry)
{
	d_ptr->normalGeometry = normalGeometry;
	if (!d_ptr->isAnimationRunning()) {
		setFixedSize(d_ptr->normalGeometry.width(), d_ptr->normalGeometry.height());
		move(d_ptr->normalGeometry.x(), d_ptr->normalGeometry.y());
	}
}

QRect SARibbonStackedWidget::normalGeometry() const
{
    return d_ptr->normalGeometry;
}

/**
 * @brief 对内部窗口发送布局请求
 *
 * 这个方法会让子窗口布局失效同时重新计算布局
 */
void SARibbonStackedWidget::layoutRequestInnerWidgets()
{
	// 确保所有子部件都填满整个区域
	for (int i = 0; i < count(); ++i) {
		QWidget* innerWidget = widget(i);
		if (!innerWidget) {
			continue;
		}
		// 方法1 update（不生效）
		//  innerWidget->update();

		// 方法2 postEvent（不生效）
		//  QApplication::postEvent(innerWidget, new QEvent(QEvent::LayoutRequest));

		// 方法3 invalidate+activate（生效）
		if (auto lay = innerWidget->layout()) {
			lay->invalidate();
			lay->activate();
		}
	}
}

/**
 * @brief 设置弹出模式
 *
 * 在此模式下，窗口将：
 * - 设置为 Qt::Popup 标志
 * - 无边框窗口
 * - 显示为面板样式
 *
 * @note 切换模式时会自动处理窗口隐藏和显示
 */
void SARibbonStackedWidget::setPopupMode()
{
	if (isPopupMode()) {
		return;
	}
	setMouseTracking(true);
	setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
	setFrameShape(QFrame::Panel);
}

/**
 * @brief 检查当前是否处于弹出模式
 * @return 如果是弹出模式返回 true，否则返回 false
 */
bool SARibbonStackedWidget::isPopupMode() const
{
    return (windowFlags() & Qt::Popup);
}

/**
 * @brief 设置正常模式 和普通的stackwidget一样
 *
 * 在此模式下，窗口将：
 * - 设置为常规部件模式 (Qt::Widget)
 * - 无边框窗口
 * - 不显示框架
 *
 * @note 如果存在模态事件循环，将退出该循环
 */
void SARibbonStackedWidget::setNormalMode()
{
	if (isNormalMode()) {
		return;
	}
	if (d_ptr->eventLoop) {
		d_ptr->eventLoop->exit();
		d_ptr->eventLoop = nullptr;
	}
	// 停止动画并恢复最终位置
	if (d_ptr->isAnimationRunning()) {
		d_ptr->animation->stop();
		d_ptr->isAnimating = false;  // 停止后，一定要加上标记
		setFixedHeight(d_ptr->normalGeometry.height());
	}
	setMouseTracking(false);
	setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
	setFrameShape(QFrame::NoFrame);
}

/**
 * @brief 检查当前是否处于正常模式
 * @return 如果是正常模式返回 true，否则返回 false
 */
bool SARibbonStackedWidget::isNormalMode() const
{
    return (!isPopupMode());
}

/**
 * @brief 在弹出模式下以模态方式运行事件循环
 *
 * 此方法仅在弹出模式下有效：
 * 1. 显示窗口
 * 2. 启动局部事件循环
 * 3. 窗口隐藏时退出事件循环
 *
 * @note 在正常模式下调用此方法仅显示窗口
 * @warning 不要在事件循环中再次调用此方法
 */
void SARibbonStackedWidget::exec()
{
	if (!isPopupMode()) {
		show();
		return;
	}
	show();
	QEventLoop event;
	d_ptr->eventLoop = &event;
	event.exec();
	d_ptr->eventLoop = nullptr;  // 确保重置
}

/**
 * @brief 类似tabbar的moveTab函数，交换两个窗口的index
 * @param from
 * @param to
 * @note 此操作会触发widgetRemoved(int index)信号
 */
void SARibbonStackedWidget::moveWidget(int from, int to)
{
	QWidget* w = widget(from);

	removeWidget(w);
	insertWidget(to, w);
}

/**
 * @brief 设置是否启用弹出动画
 * @param on
 */
void SARibbonStackedWidget::setUseAnimation(bool on)
{
    d_ptr->useAnimation = on;
}

/**
 * @brief 获取动画启用状态
 * @return
 */
bool SARibbonStackedWidget::isUseAnimation() const
{
    return d_ptr->useAnimation;
}
/**
 * @brief 设置动画持续时间（毫秒）
 * @param duration
 */
void SARibbonStackedWidget::setAnimationDuration(int duration)
{
    d_ptr->animation->setDuration(duration);
}

/**
 * @brief 获取动画持续时间
 * @return
 */
int SARibbonStackedWidget::animationDuration() const
{
    return d_ptr->animation->duration();
}

/**
 * @brief 动画完成槽函数
 */
void SARibbonStackedWidget::onAnimationFinished()
{
	d_ptr->isAnimating = false;
	if (isPopupMode()) {
		// 完成显示后，把其它窗口的尺寸移动到位置
		if (height() != d_ptr->normalGeometry.height()) {
			// 恢复窗口到正常位置
			setFixedHeight(d_ptr->normalGeometry.height());
		} else {
			updateInnerWidgetGeometry();
		}
	}
}

void SARibbonStackedWidget::showEvent(QShowEvent* e)
{
	if (isPopupMode() && d_ptr->useAnimation && !d_ptr->isAnimationRunning()) {
		// 这个必须放在最前面，否则setFixedHeight(0);就会把子窗口的尺寸改变
		d_ptr->isAnimating = true;
		// 标记为显示动画

		// 设置动画参数
		d_ptr->animation->setStartValue(0);
		d_ptr->animation->setEndValue(d_ptr->normalGeometry.height());

		// 启动动画
		d_ptr->animation->start();
		// 设置起始位置
		setFixedHeight(0);
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
		qDebug() << "Starting show animation. Current state:" << d_ptr->animation->state()
				 << "Start value:" << d_ptr->animation->startValue().toInt()
				 << "End value:" << d_ptr->animation->endValue().toInt();
#endif

		// 确保动画已启动
		QCoreApplication::processEvents();
	} else {
		QStackedWidget::showEvent(e);
	}
}
void SARibbonStackedWidget::hideEvent(QHideEvent* e)
{
	if (!isPopupMode()) {
		// 非弹出模式，正常的隐藏
		QStackedWidget::hideEvent(e);
		return;
	}

	if (d_ptr->eventLoop) {
		d_ptr->eventLoop->exit();
		d_ptr->eventLoop = nullptr;
	}
	Q_EMIT hidWindow();
	QStackedWidget::hideEvent(e);
}

void SARibbonStackedWidget::updateInnerWidgetGeometry()
{
	// 确保所有子部件都填满整个区域
	const QSize newSize = size();
	for (int i = 0; i < count(); ++i) {
		QWidget* innerWidget = widget(i);
		if (!innerWidget)
			continue;

		if (i == currentIndex()) {
			// 确保当前部件也填满整个区域
			if (innerWidget->size() != newSize) {
				innerWidget->move(0, 0);
				innerWidget->setFixedSize(newSize);
			}
			continue;
		}

		// 设置非活动部件的位置和大小
		innerWidget->move(0, 0);
		innerWidget->setFixedSize(newSize);
		// innerWidget->setGeometry(0, 0, newSize.width(), newSize.height());
		// 通知部件布局可能需要更新
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
		qDebug() << "SARibbonStackedWidget::resizeEvent [not Animation]innerWidget move to:" << innerWidget->geometry();
#endif
		QApplication::postEvent(innerWidget, new QEvent(QEvent::LayoutRequest));
	}
}

void SARibbonStackedWidget::resizeEvent(QResizeEvent* e)
{
	if (d_ptr->isAnimationRunning() && d_ptr->useAnimation) {
		// 正在动画过程中，移动当前窗口的位置
		QWidget* innerWidget = currentWidget();
		if (!innerWidget) {
			return;
		}
		int x = innerWidget->x();
		if (innerWidget->size() != d_ptr->normalGeometry.size()) {
			innerWidget->setFixedSize(d_ptr->normalGeometry.size());
		}
#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
		qDebug() << "SARibbonStackedWidget::resizeEvent innerWidget geo:" << innerWidget->geometry();
#endif
		innerWidget->move(x, e->size().height() - d_ptr->normalGeometry.height());

#if SARIBBONSTACKEDWIDGET_DEBUG_PRINT
		qDebug() << "SARibbonStackedWidget::resizeEvent innerWidget move to:" << innerWidget->geometry();
#endif

	} else {
		// 没有动画，尺寸的改变记录下来
		d_ptr->normalGeometry = geometry();
		updateInnerWidgetGeometry();
	}
	// 调用基类处理
	QStackedWidget::resizeEvent(e);
}
