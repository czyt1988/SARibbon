#ifndef SACOLORTOOLBUTTON_H
#define SACOLORTOOLBUTTON_H
#include <QToolButton>
#include "SAColorWidgetsGlobal.h"
class QPaintEvent;
/**
 * @brief 这是一个只显示颜色的toolbutton
 *
 * 普通显示完整的颜色
 * ┌─────┐
 * │color│
 * └─────┘
 * 如果有图标，图标在上面显示，下面显示颜色
 * ┌─────┐
 * │icon │
 * │color│
 * └─────┘
 */
class SAColorToolButton : public QToolButton
{
    Q_OBJECT
    SA_DECLARE_PRIVATE(SAColorToolButton)
public:
    explicit SAColorToolButton(QWidget* parent = nullptr);
    //获取颜色
    QColor getColor() const;
public slots:
    //设置颜色
    void setColor(const QColor& c);

protected:
    virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
};

#endif  // SACOLORTOOLBUTTON_H
