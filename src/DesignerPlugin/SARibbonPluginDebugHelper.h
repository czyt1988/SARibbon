#ifndef SARIBBONPLUGINDEBUGHELPER_H
#define SARIBBONPLUGINDEBUGHELPER_H
#include <QString>
#include <QFile>
#include <QDateTime>
#include <memory>
#include <QMutex>

#ifndef SA_PLUGIN_MARK
#define SA_PLUGIN_MARK()							\
    do {									\
        QString info = QString("[%1],[%2],[%3]-(%4)")			\
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"))	\
            .arg(__FUNCTION__)						\
            .arg(__LINE__)						\
            .arg(__FILE__);						\
        SARibbonPluginDebugHelper::instance().append(info);		\
    } while (0)
#endif

#ifndef SA_PLUGIN_LOG
#define SA_PLUGIN_LOG(...)							\
    do {									\
        QString info = QString("[%1],[%2],[%3],")			\
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"))	\
            .arg(__FUNCTION__)						\
            .arg(__LINE__);						\
        info += QString::asprintf(__VA_ARGS__);				\
        SARibbonPluginDebugHelper::instance().append(info);		\
    } while (0)
#endif
class SARibbonPluginDebugHelperPrivate;

/**
 * @brief The SARibbonPluginDebugHelper class
 */
class SARibbonPluginDebugHelper
{
private:
    SARibbonPluginDebugHelper();

public:
    static SARibbonPluginDebugHelper& instance();

    //添加字符
    void append(const QString& str);

private:
    QMutex m_mutex;
    std::shared_ptr<SARibbonPluginDebugHelperPrivate> d_p;
};

#endif // SARIBBONPLUGINDEBUGHELPER_H
