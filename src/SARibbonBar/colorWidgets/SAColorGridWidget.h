#ifndef SACOLORGRIDWIDGET_H
#define SACOLORGRIDWIDGET_H
#include <QWidget>
#include <functional>
#include "SAColorWidgetsGlobal.h"
class QAbstractButton;
class SAColorToolButton;
/**
 * @brief 一个grid的Color布局
 *
 * 一个形如下面的颜色grid：
 *
 * □□□□□□□□□
 *
 * □□□□□□□□□
 *
 * □□□□□□□□□
 */
class SA_COLOR_WIDGETS_API SAColorGridWidget : public QWidget
{
    Q_OBJECT
    SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorGridWidget)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
public:
    using FunColorBtn = std::function< void(SAColorToolButton*) >;

public:
    SAColorGridWidget(QWidget* par = nullptr);
    ~SAColorGridWidget();
    // 设置列数，行数量会根据列数量来匹配,如果设置-1或者0，说明不限定列数量，这样会只有一行
    void setColumnCount(int c);
    int columnCount() const;
    // 设置当前的颜色列表
    void setColorList(const QList< QColor >& cls);
    QList< QColor > getColorList() const;
    // 间隔
    int spacing() const;
    void setSpacing(int v);
    // 获取颜色的数量
    int colorCount() const;
    // 图标的尺寸
    void setColorIconSize(const QSize& s);
    QSize colorIconSize() const;
    // 设置颜色是否是checkable
    void setColorCheckable(bool on = true);
    bool isColorCheckable() const;
    // 获取当前选中的颜色
    QColor currentCheckedColor() const;
    // 获取index对于的colorbutton
    SAColorToolButton* colorButton(int index) const;
    // 垂直间距
    void setVerticalSpacing(int v);
    int verticalSpacing() const;
    // 水平间距
    void setHorizontalSpacing(int v);
    int horizontalSpacing() const;
    // 清除当前选中状态，这时没有颜色是选中的
    void clearCheckedState();
    // 对所有的colorbtn就行遍历处理，可以通过此函数进行tooltip设置等操作
    void iterationColorBtns(FunColorBtn fn);
    // 设置行最小高度
    void setRowMinimumHeight(int row, int minSize);
    // 让颜色块左对齐
    void setHorizontalSpacerToRight(bool on = true);
private Q_SLOTS:
    void onButtonClicked(QAbstractButton* btn);
    void onButtonPressed(QAbstractButton* btn);
    void onButtonReleased(QAbstractButton* btn);
    void onButtonToggled(QAbstractButton* btn, bool on);

Q_SIGNALS:
    /**
     * @brief 对于check模式，check的颜色触发的信号
     * @param c
     * @param on
     */
    void colorClicked(const QColor& c);
    void colorPressed(const QColor& c);
    void colorReleased(const QColor& c);
    void colorToggled(const QColor& c, bool on);

public:
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
};
namespace SA
{
/**
 * @brief 获取标准色列表（一共10种颜色）
 * @return
 */
SA_COLOR_WIDGETS_API QList< QColor > getStandardColorList();
}
#endif  // SACOLORGRIDWIDGET_H
