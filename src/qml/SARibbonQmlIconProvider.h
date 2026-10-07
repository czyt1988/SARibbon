#ifndef SARIBBONQMLICONPROVIDER_H
#define SARIBBONQMLICONPROVIDER_H
#include "SARibbonQmlGlobal.h"
// the single QAction include face of the module (contract D5, plan-05 S1)
#include "SARibbonQmlActionCompat.h"
#include <QQuickImageProvider>
#include <QUrl>

class QMutex;

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief image://saribbon/act/&lt;id&gt; bridge for bare-QAction icons
 * @details Plan-05 S3, the C++ backend path: a plain QAction carries a QIcon,
 *          which QML cannot consume directly. The provider renders
 *          QAction::icon pixmaps on demand and caches them per (action, size);
 *          a cache entry dies with its action (QPointer registry) and is
 *          invalidated on iconChanged. RibbonAction-backed commands never
 *          touch this path — their `iconSource` url feeds the leaf directly
 *          (the zero-overhead main route).
 *          The registry is static process-wide data; one provider instance is
 *          created per QQmlEngine (engine ownership rule of addImageProvider),
 *          all instances read the same registry.
 * \endif
 *
 * \if CHINESE
 * @brief 裸 QAction 图标的 image://saribbon/act/&lt;id&gt; 桥
 * @details 计划 05 S3 的 C++ 后端路径：裸 QAction 携带的 QIcon 无法被 QML 直接
 *          消费。provider 按需渲染 QAction::icon 的像素图并按 (action, size)
 *          缓存；缓存条目随 action 一起死亡（QPointer 登记表），iconChanged 时
 *          失效。RibbonAction 背书的命令不走此路径——其 `iconSource` url 直接
 *          喂给叶子（零开销主路径）。
 *          登记表是进程级静态数据；每个 QQmlEngine 各建一个 provider 实例
 *          （addImageProvider 的所有权规则），所有实例读同一份登记表。
 * \endif
 */
class SA_RIBBON_QML_EXPORT SAIconImageProvider : public QQuickImageProvider
{
public:
    SAIconImageProvider();
    ~SAIconImageProvider() override;

    // QImage-generating provider (the leaf Image reads pixels, no textures
    // need to be shared)
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

    // Register the action and return its stable image url
    // (image://saribbon/act/<registryIndex>). Idempotent per action.
    static QUrl registerAction(QAction* action);

    // The icon url of a registered action (empty QUrl when unregistered)
    static QUrl iconUrl(QAction* action);

    // Drop the registration and the cache entry (action going away); stale
    // entries are also reclaimed lazily inside requestImage
    static void unregisterAction(QAction* action);

private:
    struct Entry
    {
        QPointer< QAction > action;
        QHash< int, QImage > cache;  ///< requestedSize edge -> pixels
    };
    static QMutex& registryMutex();
    static QHash< QAction*, int >& actionToId();
    static QHash< int, Entry >& idToEntry();
    static int& nextId();
    static QUrl urlForId(int id);
};

}
#endif  // SARIBBONQMLICONPROVIDER_H
