#include "SARibbonPluginDebugHelper.h"
#include <QMutexLocker>
#include <QTextStream>
#include <QQueue>
class SARibbonPluginDebugHelperPrivate {
public:
    SARibbonPluginDebugHelperPrivate(SARibbonPluginDebugHelper *p);
    ~SARibbonPluginDebugHelperPrivate();
    void enqueue(const QString& str);

    SARibbonPluginDebugHelper *q_d;
private:
    QString dequeue();

    QQueue<QString> mQueue;
    QMutex mMutex;
    std::shared_ptr<QFile> mFile;
};


SARibbonPluginDebugHelperPrivate::SARibbonPluginDebugHelperPrivate(SARibbonPluginDebugHelper *p)
    : q_d(p)
    , mFile(new QFile("sa-ribbon-plugn.log"))
{
    if (!mFile->open(QIODevice::WriteOnly | QIODevice::Text)) { \
        mFile.reset();					    \
    }
}


SARibbonPluginDebugHelperPrivate::~SARibbonPluginDebugHelperPrivate()
{
}


void SARibbonPluginDebugHelperPrivate::enqueue(const QString& str)
{
    QMutexLocker lock(&mMutex);

    mQueue.enqueue(str);
    if (mFile) {
        QTextStream ss(mFile.get()); \
        ss << str << endl;	     \
        ss.flush();
        mFile->flush();
    }
}


QString SARibbonPluginDebugHelperPrivate::dequeue()
{
    QMutexLocker lock(&mMutex);

    return (mQueue.dequeue());
}


SARibbonPluginDebugHelper::SARibbonPluginDebugHelper()
    : d_p(new SARibbonPluginDebugHelperPrivate(this))
{
}


SARibbonPluginDebugHelper& SARibbonPluginDebugHelper::instance()
{
    static SARibbonPluginDebugHelper sa_debug;

    return (sa_debug);
}


void SARibbonPluginDebugHelper::append(const QString& str)
{
    QMutexLocker lock(&m_mutex);

    d_p->enqueue(str);
}
