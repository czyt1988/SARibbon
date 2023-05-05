#ifndef SARIBBONTOOLBUTTON_H
#define SARIBBONTOOLBUTTON_H
#include "SARibbonGlobal.h"
#include <QToolButton>

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
     * @brief LargeButton的显示样式，仅在LargeButton模式下，有mean的情况生效
     */
    enum LargeButtonType
    {
        Normal,  ///< icon占大按钮的一半区域,箭头在按钮最下方，文字可以换一行，对应于office的菜单按钮，下箭头在按钮最下方(默认)
        Lite  ///< icon占大按钮的60%，文字占40%，且文字不换行，对应于wps模式的ribbon菜单按钮，下箭头在文字旁边
    };
    Q_ENUM(LargeButtonType)
public:
    SARibbonToolButton(QWidget* parent = Q_NULLPTR);
    SARibbonToolButton(QAction* defaultAction, QWidget* parent = Q_NULLPTR);
    ~SARibbonToolButton();
    //标记按钮的样式，按钮的样式有不同的渲染方式
    RibbonButtonType buttonType() const;
    void setButtonType(const RibbonButtonType& buttonType);
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    void setLargeButtonType(LargeButtonType type);
    LargeButtonType largeButtonType() const;
    //获取间距
    int spacing() const;

public:
    //在lite模式下是否允许文字换行
    static void setLiteStyleEnableWordWrap(bool on);
    static bool isLiteStyleEnableWordWrap();

protected:
    virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    virtual void focusOutEvent(QFocusEvent* e) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent* e) Q_DECL_OVERRIDE;
    virtual bool hitButton(const QPoint& pos) const Q_DECL_OVERRIDE;
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
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

#endif  // SARIBBONTOOLBUTTON_H
