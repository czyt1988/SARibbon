#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Widget;
}
QT_END_NAMESPACE

class SARibbonBar;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    ~Widget();

private:
    void buildRibbon(SARibbonBar* bar);
    QAction* createAction(const QString& text, const QString& iconurl);

private:
    Ui::Widget* ui;
    SARibbonBar* mRibbonBar { nullptr };
};
#endif  // WIDGET_H
