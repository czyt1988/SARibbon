#ifndef RIBBONACTIONREGISTRY_H
#define RIBBONACTIONREGISTRY_H
#include "SARibbonQmlGlobal.h"
// the single QAction include face of the module (contract D5, plan-05 S1)
#include "SARibbonQmlActionCompat.h"
// full definition, not a forward declaration: RibbonBar* appears in a
// Q_INVOKABLE signature, so the moc output instantiates QMetaType::fromType
// and silently loses the QObject specialization if the type is incomplete
// where that moc file happens to be compiled (NOTES B64)
#include "SARibbonQmlBar.h"
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <QObject>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QString>
#include <QVariantMap>

namespace SARibbonQml {

class RibbonBar;
class RibbonCategory;
class RibbonLayoutItemHost;
class RibbonToolButton;

/**
 * \if ENGLISH
 * @brief One customizable command of the QML ribbon (plan 06 S1)
 * @details The unit of customization is a QAction — exactly as in the widgets
 *          SARibbonActionsManager. The addressing key is the action's
 *          objectName (contract §5 persistence identity); text/icon and every
 *          other display datum is a LIVE read of the action, never a
 *          registration-time snapshot. The live host items (buttons the
 *          action is currently bound to) are tracked as a list: the same
 *          command may sit in a panel and in the quick access bar at once,
 *          each placement an independent view (contract §6).
 * \endif
 *
 * \if CHINESE
 * @brief QML ribbon 的一条可定制命令（计划 06 S1）
 * @details 定制的单位是一个 QAction——与 widgets 的 SARibbonActionsManager
 *          完全一致。寻址 key 即 action 的 objectName（契约 §5 持久化身份）；
 *          text/图标等显示数据都是对 action 的活读取，绝非注册时快照。活动
 *          宿主项（当前绑定该 action 的按钮）以列表跟踪：同一命令可以同时摆在
 *          面板和快速访问栏，每次放置都是独立视图（契约 §6）。
 * \endif
 */
struct SA_RIBBON_QML_EXPORT RibbonActionDescriptor
{
    QPointer< QAction > action;                            ///< the command object (null when default-constructed)
    int tag = int(SARibbon::Core::UnknowActionTag);        ///< registry tag the command is filed under
    QList< RibbonLayoutItemHost* > items;                  ///< live host items bound to the action
    SARibbon::Core::SARibbonRowProportion proportion = SARibbon::Core::SARibbonRowProportion::Medium;  ///< default placement proportion when materialized

    // The addressing key (the action's objectName; empty when invalid)
    QString key() const;
    // Display text of the command (live read)
    QString text() const;
    // Icon url string of the command (live read: RibbonAction url or the image-provider bridge)
    QString iconSource() const;
    // True when the descriptor names a command
    bool isValid() const;
    // True when the command is currently placed (at least one live item)
    bool hasItem() const;
    // Convert to the map shape published to QML (full keys, NOTES B60)
    QVariantMap toVariantMap() const;
};

/**
 * \if ENGLISH
 * @brief Registry of every customizable command of a QML ribbon
 * @details The QML counterpart of the widgets SARibbonActionsManager over the
 *          same QAction units: a tag -> descriptor table plus a key -> action
 *          hash, the same API vocabulary (registeAction / unregisteAction /
 *          actions / actionTags / filter / search / clear / setTagName /
 *          tagName / removeTag, signal actionTagChanged). Tag values come from
 *          the core SARibbonActionTag enum, so both front ends speak one tag
 *          language. The addressing key is always the action's objectName;
 *          an action without one is refused with a warning (contract §5) —
 *          the identity of a command is its objectName, exactly as in widgets.
 *          autoRegister walks a RibbonBar host tree and files every
 *          action-bound button under the tag of its category; plain-declarative
 *          buttons (no action) are skipped — the two-level semantics of
 *          contract D6, the widgets customizer equally ignores addWidget
 *          content.
 * \endif
 *
 * \if CHINESE
 * @brief QML ribbon 全部可定制命令的注册表
 * @details widgets SARibbonActionsManager 在同一 QAction 单位上的 QML 对应物：
 *          一张 tag -> 描述符表加一张 key -> action 哈希，API 词汇保持一致
 *          （registeAction / unregisteAction / actions / actionTags / filter /
 *          search / clear / setTagName / tagName / removeTag，信号
 *          actionTagChanged）。tag 取值来自 core 的 SARibbonActionTag，两个前端
 *          共用同一套 tag 语言。寻址 key 恒为 action 的 objectName；没有
 *          objectName 的 action 会被带告警拒绝（契约 §5）——命令的身份就是其
 *          objectName，与 widgets 一致。autoRegister 遍历 RibbonBar 宿主树，把
 *          每个 action 绑定按钮归入其所属 category 的 tag；纯声明按钮（无
 *          action）跳过——契约 D6 的两级语义，widgets 定制器同样不管 addWidget
 *          塞进去的内容。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonActionRegistry : public QObject
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(RibbonActionRegistry)
    Q_PROPERTY(int count READ count NOTIFY registryChanged)
    Q_PROPERTY(QList< int > actionTags READ actionTags NOTIFY registryChanged)
public:
    // QML-side tag for the quick access bar entries. widgets does not need one
    // (its quick access bar reuses the very same QAction objects already filed
    // under their category tag); a QML quick access button created by the
    // customizer is a host of its own, so the placement is filed under a
    // dedicated user-range tag while the command stays reachable through its
    // own key
    static constexpr int QuickAccessActionTag = int(SARibbon::Core::UserDefineActionTag) + 1;

    explicit RibbonActionRegistry(QObject* parent = nullptr);
    ~RibbonActionRegistry() override;

    // Set the display name of a tag (multi-language: reset on language change)
    void setTagName(int tag, const QString& name);
    // Get the display name of a tag
    QString tagName(int tag) const;
    // Remove a tag together with the descriptors filed under it
    void removeTag(int tag);

    // Register a command. The key IS the action's objectName (an explicit key
    // argument must equal it); an action without one is refused (contract §5)
    bool registeAction(QAction* act, int tag, const QString& key = QString(), bool enableEmit = true);
    // Declare a command template: a bare QAction created by the registry from
    // the given identity, placeable by the customizer later (the QML
    // substitute for widgets "actions that live outside the ribbon")
    bool registeCommand(int tag,
                        const QString& key,
                        const QString& text,
                        const QString& iconSource = QString(),
                        SARibbon::Core::SARibbonRowProportion proportion = SARibbon::Core::SARibbonRowProportion::Medium,
                        bool enableEmit = true);
    // Unregister by action (removes it from every tag it was filed under)
    void unregisteAction(QAction* act, bool enableEmit = true);
    // Unregister by key
    void unregisteKey(const QString& key, bool enableEmit = true);
    // Track a live host item as a placement of the key's command (the
    // customizer/autoRegister route; the binding itself is expressed by the
    // button's `action` property — this only keeps the placement list current)
    void attachItem(const QString& key, RibbonLayoutItemHost* item);
    void detachItem(RibbonLayoutItemHost* item);
    // Mark every registered command customizable through the core marker;
    // returns the marked count (the command-level flag lives on the QAction,
    // plan-07 S3 semantics)
    Q_INVOKABLE int markCustomizable(bool canbe = true);

    // Descriptors filed under a tag (widgets filter/actions parity)
    QList< SARibbonQml::RibbonActionDescriptor > filter(int tag) const;
    // Same list, alias kept for widgets API parity
    QList< SARibbonQml::RibbonActionDescriptor > actions(int tag) const;
    // Every tag currently holding at least one descriptor
    QList< int > actionTags() const;
    // Descriptor by key (invalid when unknown)
    SARibbonQml::RibbonActionDescriptor descriptor(const QString& key) const;
    // The command object behind a key, nullptr for an unknown key
    QAction* action(const QString& key) const;
    // Key of a registered action, empty when it is not registered
    QString key(QAction* act) const;
    // Tag a command is filed under (UnknowActionTag when not registered)
    int tagOf(QAction* act) const;
    // The first live host item bound to the key's command (nullptr for a
    // template or an unknown key); the tree model and customizer row lookups
    // go through this
    RibbonLayoutItemHost* firstItem(const QString& key) const;
    // Number of registered descriptors
    int count() const;
    // Every registered descriptor, ordered by tag then registration
    QList< SARibbonQml::RibbonActionDescriptor > allActions() const;
    // Descriptors whose text contains `text` (case insensitive, widgets search parity)
    QList< SARibbonQml::RibbonActionDescriptor > search(const QString& text) const;
    // Drop every descriptor and tag name
    void clear();

    // Walk a bar host tree and register every action-bound button in it
    QMap< int, SARibbonQml::RibbonCategory* > autoRegister(RibbonBar* bar, bool enableEmit = true);
    // QML wrapper of autoRegister: the C++ return type is a map of category
    // pointers that QML cannot consume, so this reports count() instead
    Q_INVOKABLE int autoRegisterBar(RibbonBar* bar);

    // QML-facing views of the tables (full-key maps, NOTES B60)
    Q_INVOKABLE QVariantMap actionInfo(const QString& key) const;
    Q_INVOKABLE QVariantList actionInfoList(int tag) const;
    Q_INVOKABLE QVariantList tagInfoList() const;
    Q_INVOKABLE QVariantList searchInfo(const QString& text) const;

Q_SIGNALS:
    /**
     * \if ENGLISH
     * @brief A tag appeared or disappeared
     * @param tag the changed tag
     * @param isdelete true when the tag lost its last descriptor
     \endif
     *
     * \if CHINESE
     * @brief 某个 tag 新增或被移除
     * @param tag 发生变化的 tag
     * @param isdelete 该 tag 失去最后一个描述符时为 true
     \endif
     */
    void actionTagChanged(int tag, bool isdelete);

    /**
     * \if ENGLISH
     * @brief The descriptor tables changed in any way
     * @details Emitted on every registration, removal, tag rename and clear, so
     *          QML models and repeaters bound to the registry refresh without
     *          having to track the tag level themselves.
     \endif
     *
     * \if CHINESE
     * @brief 描述符表发生任何变化
     * @details 每次注册、移除、tag 改名与清空都会发射，使绑定注册表的 QML
     *          model 与 repeater 无需自行跟踪 tag 层级即可刷新。
     \endif
     */
    void registryChanged();

private Q_SLOTS:
    void onActionDestroyed(QObject* o);
    void onCategoryTitleChanged();

private:
    // Drop a descriptor from every table; removes emptied tags
    void removeDescriptor(const QString& key, bool enableEmit);
};

}
#endif  // RIBBONACTIONREGISTRY_H
