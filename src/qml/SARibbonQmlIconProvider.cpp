#include "SARibbonQmlIconProvider.h"
#include <QHash>
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QPointer>

namespace SARibbonQml {

QMutex& SAIconImageProvider::registryMutex()
{
    static QMutex m;
    return m;
}

QHash< QAction*, int >& SAIconImageProvider::actionToId()
{
    static QHash< QAction*, int > m;
    return m;
}

QHash< int, SAIconImageProvider::Entry >& SAIconImageProvider::idToEntry()
{
    static QHash< int, Entry > m;
    return m;
}

int& SAIconImageProvider::nextId()
{
    static int id = 1;
    return id;
}

SAIconImageProvider::SAIconImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{
}

SAIconImageProvider::~SAIconImageProvider()
{
}

QUrl SAIconImageProvider::urlForId(int id)
{
    QUrl url;
    url.setScheme(QStringLiteral("image"));
    url.setHost(QStringLiteral("saribbon"));
    url.setPath(QStringLiteral("/act/") + QString::number(id));
    return url;
}

QUrl SAIconImageProvider::registerAction(QAction* action)
{
    if (nullptr == action) {
        return QUrl();
    }
    QMutexLocker lock(&registryMutex());
    int id = actionToId().value(action, 0);
    if (0 == id) {
        id = nextId()++;
        actionToId().insert(action, id);
        Entry e;
        e.action = action;
        idToEntry().insert(id, e);
        // cache invalidation: only an icon change flushes this entry (plan-05
        // S3: no high-frequency refresh). The connection dies with the action
        QObject::connect(action, &QAction::changed, action, [action]() {
            QMutexLocker innerLock(&registryMutex());
            const int cid = actionToId().value(action, 0);
            if (cid != 0) {
                auto it = idToEntry().find(cid);
                if (it != idToEntry().end()) {
                    it.value().cache.clear();
                }
            }
        });
    }
    return urlForId(id);
}

QUrl SAIconImageProvider::iconUrl(QAction* action)
{
    if (nullptr == action) {
        return QUrl();
    }
    QMutexLocker lock(&registryMutex());
    const int id = actionToId().value(action, 0);
    return (0 == id) ? QUrl() : urlForId(id);
}

void SAIconImageProvider::unregisterAction(QAction* action)
{
    if (nullptr == action) {
        return;
    }
    QMutexLocker lock(&registryMutex());
    const int id = actionToId().value(action, 0);
    if (0 != id) {
        actionToId().remove(action);
        idToEntry().remove(id);
    }
}

QImage SAIconImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    // id form: "act/<n>"
    if (!id.startsWith(QStringLiteral("act/"))) {
        return QImage();
    }
    bool ok       = false;
    const int aid = id.mid(4).toInt(&ok);
    if (!ok) {
        return QImage();
    }
    QMutexLocker lock(&registryMutex());
    auto it = idToEntry().find(aid);
    if (it == idToEntry().end()) {
        return QImage();
    }
    if (it.value().action.isNull()) {
        // the action is gone; reclaim the dead entry as well
        idToEntry().erase(it);
        return QImage();
    }
    QAction* action = it.value().action.data();
    const int edge  = requestedSize.isValid() ? qMax(2, qMin(requestedSize.width(), requestedSize.height())) : 32;
    auto cacheIt    = it.value().cache.find(edge);
    if (cacheIt != it.value().cache.end() && !cacheIt.value().isNull()) {
        if (size) {
            *size = cacheIt.value().size();
        }
        return cacheIt.value();
    }
    const QIcon icon = action->icon();
    if (icon.isNull()) {
        if (size) {
            *size = QSize(0, 0);
        }
        return QImage();
    }
    QImage img = icon.pixmap(edge, edge).toImage();
    it.value().cache.insert(edge, img);
    if (size) {
        *size = img.size();
    }
    return img;
}

}  // namespace SARibbonQml
