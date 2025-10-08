#ifndef SAFRAMELESSHELPER_H
#define SAFRAMELESSHELPER_H

#include "SARibbonGlobal.h"
#include <QObject>

class QWidget;

/**
 * @class SAFramelessHelper
 * @brief 该类用于帮助实现无边框窗体的移动和缩放功能。
 *
 * SAFramelessHelper 提供了一系列方法来激活、移除无边框窗体，设置窗体的移动、缩放属性，
 * 以及橡皮筋效果等。通过事件过滤机制，它能够处理鼠标事件，从而实现窗体的移动和缩放操作。
 *
 * 示例：
 *
 * @code
 * SAFramelessHelper* helper = new SAFramelessHelper(this);
 * helper->activateOn(myWidget);
 * helper->setWidgetMovable(true);
 * helper->setWidgetResizable(true);
 * @endcode
 */
class SA_RIBBON_EXPORT SAFramelessHelper : public QObject
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SAFramelessHelper)
	friend class SAPrivateFramelessWidgetData;

public:
	explicit SAFramelessHelper(QObject* parent);
	~SAFramelessHelper();
	// 激活窗体
	void activateOn(QWidget* topLevelWidget);

	// 移除窗体
	void removeFrom(QWidget* topLevelWidget);

	// 设置窗体移动
	void setWidgetMovable(bool movable);

	// 设置窗体缩放
	void setWidgetResizable(bool resizable);

	// 设置橡皮筋移动
	void setRubberBandOnMove(bool movable);

	// 设置橡皮筋缩放
	void setRubberBandOnResize(bool resizable);

	// 设置边框的宽度
	void setBorderWidth(int width);

	// 设置标题栏高度
	void setTitleHeight(int height);
	bool widgetResizable();
	bool widgetMovable();
	bool rubberBandOnMove();
	bool rubberBandOnResisze();
	uint borderWidth();
	uint titleHeight();
	// 获取当前屏幕的 DPI 缩放因子
	static qreal getScreenDpiScale(const QWidget* widget);

protected:
	// 事件过滤，进行移动、缩放等
	virtual bool eventFilter(QObject* obj, QEvent* event);
};

#endif  // FRAMELESSHELPER_H
