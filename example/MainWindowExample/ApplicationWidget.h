#ifndef APPLICATIONWIDGET_H
#define APPLICATIONWIDGET_H

#include <QWidget>

namespace Ui {
class ApplicationWidget;
}

class ApplicationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ApplicationWidget(QWidget *parent = nullptr);
    ~ApplicationWidget();

private:
    Ui::ApplicationWidget *ui;
};

#endif // APPLICATIONWIDGET_H
