#ifndef RIBBONQUICKHOST_H
#define RIBBONQUICKHOST_H
#include "SARibbonQmlGlobal.h"
#include <QQuickItem>
#include <QUrl>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Common base of every structural host: owns the visual leaf lifecycle
 * @details The leaf-creation trilogy (QQmlComponent create -> handshake
 *          injection -> reparent) and the safe teardown (unparent +
 *          deleteLater, never a direct delete) live here once instead of being
 *          copied into each host. The uniform handshake contract: every leaf
 *          root declares `property QtObject cppHost` and assigns itself back
 *          into the inherited `qmlLeaf` property, so hosts, leaves and tests
 *          share one vocabulary.
 * \endif
 *
 * \if CHINESE
 * @brief 所有结构宿主的公共基类：持有视觉叶子的生命周期
 * @details 叶子创建三部曲（QQmlComponent 创建 -> 握手注入 -> 重挂父子）与
 *          安全拆除（解除父子 + deleteLater，绝不直接 delete）在此实现一次，
 *          不再在每个宿主里复制。统一握手契约：每个叶子根声明
 *          `property QtObject cppHost` 并把自己赋回继承而来的 `qmlLeaf`
 *          属性，宿主、叶子与测试共享同一套词汇。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonQuickHost : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem* qmlLeaf READ qmlLeaf WRITE setQmlLeaf NOTIFY qmlLeafChanged)
public:
    explicit RibbonQuickHost(QQuickItem* parent = nullptr);
    ~RibbonQuickHost() override;

    // Handshake property: the visual leaf assigns itself back (doubles as the
    // test reachability entry into the leaf tree)
    QQuickItem* qmlLeaf() const;
    void setQmlLeaf(QQuickItem* item);

    // Create the visual leaf if not yet present (idempotent). Public because
    // C++-created hosts (e.g. the bar's auto tabs) never run componentComplete
    void ensureQmlLeaf();

Q_SIGNALS:
    void qmlLeafChanged();

protected:
    // The qrc URL of this host's default visual leaf (table in SARibbonQmlTypes.h)
    virtual QUrl leafUrl() const = 0;

private:
    QQuickItem* mQmlLeaf = nullptr;
};

}

#endif  // RIBBONQUICKHOST_H
