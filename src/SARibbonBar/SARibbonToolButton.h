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

public:
    SARibbonToolButton(QWidget* parent = Q_NULLPTR);
    SARibbonToolButton(QAction* defaultAction, QWidget* parent = Q_NULLPTR);
    ~SARibbonToolButton();
    //标记按钮的样式，按钮的样式有不同的渲染方式
    RibbonButtonType buttonType() const;
    void setButtonType(const RibbonButtonType& buttonType);
    //是否是小按钮
    bool isSmallRibbonButton() const;
    //是否是大按钮
    bool isLargeRibbonButton() const;
    //最小尺寸提示
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    //获取间距
    int spacing() const;
    //更新尺寸
    void updateRect();

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

public:
    //在lite模式下是否允许文字换行
    static void setEnableWordWrap(bool on);
    static bool isEnableWordWrap();

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
    //事件改变 - 主要为了捕获字体的改变
    virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;

protected:
    //绘制按钮
    virtual void paintButton(QPainter& p, const QStyleOptionToolButton& opt);
    //绘制图标
    virtual void paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect);
    //绘制文本
    virtual void paintText(QPainter& p, const QStyleOptionToolButton& opt, const QRect& textDrawRect);
    //绘制Indicator
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
