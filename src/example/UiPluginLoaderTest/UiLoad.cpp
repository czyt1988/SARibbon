#include "UiLoad.h"
#include <QUiLoader>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
QWidget *load()
{
    QString fn = QFileDialog::getOpenFileName(nullptr, QObject::tr("choose ui file")
        , QString()
        , "ui (*.ui)"
        );

    if (fn.isEmpty()) {
        return (nullptr);
    }
    QFile file(fn);

    if (!file.open(QIODevice::ReadWrite)) {
        QMessageBox::critical(nullptr, QObject::tr("critical")
            , QObject::tr("can not open ui file:%1").arg(fn));
        return (nullptr);
    }
    QUiLoader loader;
    QWidget *res = loader.load(&file, nullptr);

    if (res == nullptr) {
        QMessageBox::critical(nullptr, QObject::tr("critical")
            , QObject::tr("can not open ui file:%1").arg(loader.errorString()));
    }
    return (res);
}
