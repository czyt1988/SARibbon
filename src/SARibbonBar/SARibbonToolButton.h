#ifndef SARIBBONTOOLBUTTON_H
#define SARIBBONTOOLBUTTON_H
#include "SARibbonGlobal.h"
#include <QToolButton>
#include <QDebug>
/**
 * @brief Ribbon界面适用的toolButton
 *
 * 相对于普通toolbutton，主要多了两个类型设置，@ref setButtonType 和 @ref setLargeButtonType
 *
 * @note @sa setIconSize 函数不在起作用，iconsize是根据当前尺寸动态调整的
 */
class SA_RIBBON_EXPORT SARibbonToolButton : public QToolButton
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonToolButton)
public:
	/**
	 * @brief 按钮样式
	 */
	enum RibbonButtonType
	{
		LargeButton,
		SmallButton
	};
	Q_ENUM(RibbonButtonType)

    /**
     * @brief 布局系数
     */
    struct LayoutFactor
    {
        /**
         * @brief  大按钮模式下，两行文本高度系数，这个系数决定了文字换行时2行文本的矩形高度
         *
         * 两行文本区域高度 = fontMetrics.lineSpacing*系数
         * @note 此值应该大于2
         * @default 2.05
         */
        qreal twoLineHeightFactor { 2.05 };

        /**
         * @brief 大按钮模式下，单行文本高度系数，这个系数决定了单行文本的行高度
         *
         * 单行文本区域高度 = fontMetrics.lineSpacing*系数
         * @note 此值应该大于1
         * @default 1.2
         */
        qreal oneLineHeightFactor { 1.2 };

        /**
         * @brief 按钮最大宽高比，这个系数决定按钮的最大宽度
         *
         * 按钮的最大宽度为按钮高度*此系数，例如按钮高度为h，那么按钮最大宽度maxw=h*buttonMaximumAspectRatio
         * 如果在此宽度下文字还无法完全显示，那么按钮将不会继续横向扩展，将使用...替代未完全显示的文字
         */
        qreal buttonMaximumAspectRatio { 1.4 };
    };

public:
	explicit SARibbonToolButton(QWidget* parent = nullptr);
	explicit SARibbonToolButton(QAction* defaultAction, QWidget* parent = nullptr);
	~SARibbonToolButton();
	// 标记按钮的样式，按钮的样式有不同的渲染方式
	RibbonButtonType buttonType() const;
	void setButtonType(const RibbonButtonType& buttonType);
	// 是否是小按钮
	bool isSmallRibbonButton() const;
	// 是否是大按钮
	bool isLargeRibbonButton() const;
	// 最小尺寸提示
	virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

	// 获取间距
	int spacing() const;
	// 更新尺寸
	void updateRect();

	virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    // 布局系数
    void setLayoutFactor(const LayoutFactor& fac);
    const LayoutFactor& layoutFactor() const;
    LayoutFactor& layoutFactor();

public:
	// 是否允许文字换行
    void setEnableWordWrap(bool on);
    bool isEnableWordWrap();
    // 按钮的最大宽高比，这个系数决定按钮的最大宽度
    void setButtonMaximumAspectRatio(qreal v = 1.4);
    qreal buttonMaximumAspectRatio() const;

protected:
	virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
	virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
	virtual void mousePressEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
	virtual void focusOutEvent(QFocusEvent* e) Q_DECL_OVERRIDE;
	virtual void leaveEvent(QEvent* e) Q_DECL_OVERRIDE;
	virtual bool hitButton(const QPoint& pos) const Q_DECL_OVERRIDE;
	virtual bool event(QEvent* e) Q_DECL_OVERRIDE;
	// 事件改变 - 主要为了捕获字体的改变
	virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;
	virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;

protected:
	// 绘制按钮
	virtual void paintButton(QPainter& p, const QStyleOptionToolButton& opt);
	// 绘制图标
	virtual void paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect);
	// 绘制文本
	virtual void paintText(QPainter& p, const QStyleOptionToolButton& opt, const QRect& textDrawRect);
	// 绘制Indicator
	virtual void paintIndicator(QPainter& p, const QStyleOptionToolButton& opt, const QRect& indicatorDrawRect);

private:
	static void drawArrow(const QStyle* style,
                          const QStyleOptionToolButton* toolbutton,
                          const QRect& rect,
                          QPainter* painter,
                          const QWidget* widget = 0);

protected:
};

namespace SA
{
QDebug operator<<(QDebug debug, const QStyleOptionToolButton& opt);
}
#endif  // SARIBBONTOOLBUTTON_H
