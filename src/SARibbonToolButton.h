#ifndef SARIBBONTOOLBUTTON_H
#define SARIBBONTOOLBUTTON_H
#include "SARibbonGlobal.h"
#include <QToolButton>
///
/// \brief Ribbon界面适用的toolButton
///
class SA_RIBBON_EXPORT SARibbonToolButton : public QToolButton
{
    Q_OBJECT
    Q_ENUMS(RibbonButtonType)
public:
    ///
    /// \brief 按钮样式
    ///
    enum RibbonButtonType{
        LargeButton
        ,SmallButton
    };
    SARibbonToolButton(QWidget *parent = Q_NULLPTR);
    RibbonButtonType buttonType() const;
    void setButtonType(const RibbonButtonType &buttonType);
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;
protected:
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    virtual void paintLargeButton(QPaintEvent *e);
    virtual void paintSmallButton(QPaintEvent *e);
    virtual bool hitButton(const QPoint &pos) const Q_DECL_OVERRIDE;

    virtual void drawIconAndLabel(QPainter& p,const QStyleOptionToolButton& opt);
private:
    RibbonButtonType m_buttonType;
    bool m_mouseOnSubControl;
    QColor m_borderColor;
    bool m_menuButtonPressed;
    QRect m_iconRect;
};

#endif // SARIBBONTOOLBUTTON_H
