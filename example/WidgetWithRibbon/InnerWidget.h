#ifndef INNERWIDGET_H
#define INNERWIDGET_H

#include <QWidget>

namespace Ui
{
class InnerWidget;
}

class InnerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InnerWidget(QWidget* parent = nullptr);
    ~InnerWidget();
public slots:
    void appendText(const QString& t);

private:
    Ui::InnerWidget* ui;
};

#endif  // INNERWIDGET_H
