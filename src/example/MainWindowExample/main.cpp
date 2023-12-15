#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>

// 重定向qdebug的打印
void log_out_put(QtMsgType type, const QMessageLogContext& context, const QString& msg);

/**
 * @brief 重定向qdebug的打印
 * @param type
 * @param context
 * @param msg
 */
void log_out_put(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type) {
    case QtDebugMsg:
        fprintf(stdout, "[Debug] %s \n-------------->(%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;

        //    case QtInfoMsg:
        //        fprintf(stdout, "[Info] %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line,
        //        context.function); break;

    case QtWarningMsg:
        fprintf(stdout, "[Warning] %s \n-------------->(%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;

    case QtCriticalMsg:
        fprintf(stdout, "[Critical] %s \n-------------->(%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;

    case QtFatalMsg:
        fprintf(stdout, "Fatal: %s \n-------------->(%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
        break;

    default:
        fprintf(stdout, "[Debug] %s \n-------------->(%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    }
#ifndef QT_NO_DEBUG_OUTPUT
    fflush(stdout);
#endif
}

int main(int argc, char* argv[])
{
    // 以下是针对高分屏的设置，有高分屏需求都需要按照下面进行设置
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
    QApplication a(argc, argv);
    qInstallMessageHandler(log_out_put);
    QFont f = a.font();
    f.setFamily(u8"微软雅黑");
    a.setFont(f);
    QElapsedTimer cost;

    cost.start();
    MainWindow w;
    qDebug() << "window build cost:" << cost.elapsed() << " ms";
    w.show();

    return (a.exec());
}
