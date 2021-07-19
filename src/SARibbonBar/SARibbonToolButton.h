#ifndef SARIBBONTOOLBUTTON_H
#define SARIBBONTOOLBUTTON_H
#include "SARibbonGlobal.h"
#include <QToolButton>


/**
 * @brief Ribbon界面适用的toolButton
 *
 * 相对于普通toolbutton，主要多了两个类型设置，@ref setButtonType 和 @ref setLargeButtonType
 */
class SA_RIBBON_EXPORT SARibbonToolButton : public QToolButton
{
    Q_OBJECT
    Q_ENUMS(RibbonButtonType)
public:

    /**
     * @brief 按钮样式
     */
    enum RibbonButtonType {
        LargeButton
        , SmallButton
    };

    /**
     * @brief LargeButton的显示样式，仅在LargeButton模式下，有mean的情况生效
     */
    enum LargeButtonType {
        Normal  ///< icon占大按钮的一半区域,箭头在按钮最下方，文字可以换一行，对应于office的菜单按钮，下箭头在按钮最下方(默认)
        , Lite  ///< icon占大按钮的60%，文字占40%，且文字不换行，对应于wps模式的ribbon菜单按钮，下箭头在文字旁边
    };

    SARibbonToolButton(QWidget *parent = Q_NULLPTR);
    SARibbonToolButton(QAction *defaultAction, QWidget *parent = Q_NULLPTR);
    RibbonButtonType buttonType() const;
    void setButtonType(const RibbonButtonType& buttonType);
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    void setLargeButtonType(LargeButtonType type);
    LargeButtonType largeButtonType() const;

    //lite模式下的分割线
    virtual int liteLargeButtonIconHeight(int buttonHeight) const;

protected:
    void calcIconRect(const QStyleOptionToolButton& opt);
    QRect calcTextRect(const QStyleOptionToolButton& opt) const;
    QRect calcTextRect(const QRect& buttonRect, bool hasMenu = false) const;
    QRect calcIndicatorArrowDownRect(const QStyleOptionToolButton& opt);
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt);
    bool event(QEvent *e) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    virtual void focusOutEvent(QFocusEvent *e) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    virtual void paintLargeButton(QPaintEvent *e);
    virtual void paintSmallButton(QPaintEvent *e);
    virtual bool hitButton(const QPoint& pos) const Q_DECL_OVERRIDE;
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual void drawIconAndLabel(QPainter& p, QStyleOptionToolButton& opt);

private:
    static void drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton,
        const QRect& rect, QPainter *painter, const QWidget *widget = 0);

private:
    RibbonButtonType m_buttonType;
    LargeButtonType m_largeButtonType;
    bool m_mouseOnSubControl; ///< 这个用于标记MenuButtonPopup模式下，鼠标在文本区域
    bool m_menuButtonPressed;
    QRect m_iconRect;
    bool m_isWordWrap;///< 标记是否文字换行 @default false
};

#endif // SARIBBONTOOLBUTTON_H
