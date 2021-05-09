#ifndef SARIBBONPLUGINDEBUGHELPER_H
#define SARIBBONPLUGINDEBUGHELPER_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>


static QFile *s_file = nullptr;

#ifndef SA_PLUGIN_MARK
#define SA_PLUGIN_MARK()																		\
    do{																				\
        if (nullptr == s_file) {																\
            s_file = new QFile("sa-ribbon-plugn-debug.log");												\
            if (!s_file->open(QIODevice::WriteOnly | QIODevice::Text)) {											\
                delete s_file;																\
                s_file = nullptr;															\
            }																		\
        }																			\
        if (nullptr == s_file) {																\
            break;																		\
        }																			\
        QString info = QString("[%1],[%2],[%3],(%4)").arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz")).arg(__FILE__).arg(__FUNCTION__).arg(__LINE__);	\
        QTextStream ss(s_file);																	\
        ss << info << endl;																	\
        ss.flush();																		\
    }while(0)
#endif

#ifndef SA_PLUGIN_LOG
#define SA_PLUGIN_LOG(...)																		\
    do{																				\
        if (nullptr == s_file) {																\
            s_file = new QFile("sa-ribbon-plugn-debug.log");												\
            if (!s_file->open(QIODevice::WriteOnly | QIODevice::Text)) {											\
                delete s_file;																\
                s_file = nullptr;															\
            }																		\
        }																			\
        if (nullptr == s_file) {																\
            break;																		\
        }																			\
        QString info = QString("[%1],[%2],[%3],(%4)").arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz")).arg(__FILE__).arg(__FUNCTION__).arg(__LINE__);	\
        info += QString::asprintf(__VA_ARGS__);															\
        QTextStream ss(s_file);																	\
        ss << info << endl;																	\
        ss.flush();																		\
    }while(0)
#endif


#endif // SARIBBONPLUGINDEBUGHELPER_H
