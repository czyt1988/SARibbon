#ifndef RIBBONACTIONREGISTRY_H
#define RIBBONACTIONREGISTRY_H
#include "SARibbonQmlGlobal.h"
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <QObject>
#include <QList>
#include <QMap>
#include <QString>
#include <QVariantMap>

namespace SARibbonQml {

class RibbonBar;
class RibbonCategory;
class RibbonLayoutItemHost;

/**
 * \if ENGLISH
 * @brief One customizable command of the QML ribbon (plan 04 WS-C2)
 * @details The QML counterpart of the QAction* a widgets SARibbonActionsManager
 *          keeps in its tables. The QML ribbon has no action abstraction
 *          (plan 04 D8 defers it), so the unit of customization is this
 *          descriptor: a stable string key addressing the command, the display
 *          data a picker list needs, and — when the command is currently placed
 *          in the ribbon — the live host item behind it. A descriptor with a
 *          null item is a command template: it can be added to a panel or to
 *          the quick access bar, and materializing it creates the host.
 * \endif
 *
 * \if CHINESE
 * @brief QML ribbon 的一条可定制命令（计划 04 WS-C2）
 * @details 对应 widgets 侧 SARibbonActionsManager 表里保存的 QAction*。QML
 *          ribbon 没有 action 抽象（计划 04 D8 已延后），因此定制的单位是本
 *          描述符：一个用于寻址的稳定字符串 key、选取列表所需的显示数据，以及
 *          ——当该命令当前确实摆在 ribbon 上时——其背后的活动宿主项。item 为空
 *          的描述符是命令模板：它可以被加进面板或快速访问栏，落地时才创建宿主。
 * \endif
 */
struct SA_RIBBON_QML_EXPORT RibbonActionDescriptor
{
    QString key;                                          ///< stable addressing key (record keyValue)
    QString text;                                         ///< display text of the command
    QString iconSource;                                   ///< icon url string
    int tag = int(SARibbon::Core::UnknowActionTag);        ///< registry tag the command belongs to
    SARibbon::Core::SARibbonRowProportion proportion = SARibbon::Core::SARibbonRowProportion::Medium;  ///< row proportion used when materialized
    RibbonLayoutItemHost* item = nullptr;                  ///< live host item, null for a command template

    // True when the descriptor names a command (an empty key addresses nothing)
    bool isValid() const;
    // True when the command is currently placed in the ribbon
    bool hasItem() const;
    // Convert to the map shape published to QML (full keys, NOTES B60)
    QVariantMap toVariantMap() const;
};

/**
 * \if ENGLISH
 * @brief Registry of every customizable command of a QML ribbon (plan 04 WS-C2)
 * @details The QML counterpart of the widgets SARibbonActionsManager: two
 *          tables (tag -> descriptor list, key -> descriptor) plus the salt
 *          based key generator, and the same API vocabulary
 *          (registeAction / unregisteAction / actions / actionTags / filter /
 *          search / clear / setTagName / tagName / removeTag, signal
 *          actionTagChanged). Tag values come from the core SARibbonActionTag
 *          enum sunk in WS-C1, so both front ends speak one tag language.
 *          autoRegister walks a RibbonBar host tree and files every panel child
 *          under the tag of its category, exactly as the widgets
 *          autoRegisteActions files every panel action under its category tag.
 * @note The one capability widgets has and this registry cannot reproduce is
 *       collecting the commands that live on the main window but nowhere in the
 *       ribbon (widgets autoRegisteWidgetActions over the bar's parent widget):
 *       QML has no equivalent enumerable action owner. registeCommand is the
 *       explicit substitute — the application declares those commands itself.
 * \endif
 *
 * \if CHINESE
 * @brief QML ribbon 全部可定制命令的注册表（计划 04 WS-C2）
 * @details 对应 widgets 侧 SARibbonActionsManager：两张表（tag -> 描述符列表、
 *          key -> 描述符）加上基于盐值的 key 生成器，API 词汇也保持一致
 *          （registeAction / unregisteAction / actions / actionTags / filter /
 *          search / clear / setTagName / tagName / removeTag，信号
 *          actionTagChanged）。tag 取值来自 WS-C1 下沉到 core 的
 *          SARibbonActionTag，两个前端共用同一套 tag 语言。autoRegister 遍历
 *          RibbonBar 宿主树，把每个面板子项归入其所属 category 的 tag，与
 *          widgets autoRegisteActions 把每个面板 action 归入 category tag 一致。
 * @note widgets 有而本注册表无法复刻的唯一能力，是收集"挂在主窗口上但不在
 *       ribbon 任何位置"的命令（widgets 对 bar 的父窗口调
 *       autoRegisteWidgetActions）：QML 没有可枚举的 action 宿主。替代方案是
 *       显式的 registeCommand——由应用自行声明这些命令。
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
    // under their category tag); QML quick access buttons are hosts of their
    // own, so they get a dedicated user-range tag
    static constexpr int QuickAccessActionTag = int(SARibbon::Core::UserDefineActionTag) + 1;

    explicit RibbonActionRegistry(QObject* parent = nullptr);
    ~RibbonActionRegistry() override;

    // Set the display name of a tag (multi-language: reset on language change)
    void setTagName(int tag, const QString& name);
    // Get the display name of a tag
    QString tagName(int tag) const;
    // Remove a tag together with the descriptors filed under it
    void removeTag(int tag);

    // Register a live host item; an empty key falls back to the salt generator
    bool registeAction(RibbonLayoutItemHost* item, int tag, const QString& key = QString(), bool enableEmit = true);
    // Register a command template (no live item) the customizer can place later
    bool registeCommand(int tag,
                        const QString& key,
                        const QString& text,
                        const QString& iconSource = QString(),
                        SARibbon::Core::SARibbonRowProportion proportion = SARibbon::Core::SARibbonRowProportion::Medium,
                        bool enableEmit = true);
    // Unregister by item (removes it from every tag it was filed under)
    void unregisteAction(RibbonLayoutItemHost* item, bool enableEmit = true);
    // Unregister by key
    void unregisteKey(const QString& key, bool enableEmit = true);
    // Point an existing key at a live host item (a materialized command template)
    void bindItem(const QString& key, RibbonLayoutItemHost* item);
    // Mark every descriptor with a live item customizable; returns the marked count
    Q_INVOKABLE int markCustomizable(bool canbe = true);

    // Descriptors filed under a tag (widgets filter/actions parity)
    QList< SARibbonQml::RibbonActionDescriptor > filter(int tag) const;
    // Same list, alias kept for widgets API parity
    QList< SARibbonQml::RibbonActionDescriptor > actions(int tag) const;
    // Every tag currently holding at least one descriptor
    QList< int > actionTags() const;
    // Descriptor by key (invalid when unknown)
    SARibbonQml::RibbonActionDescriptor descriptor(const QString& key) const;
    // Live host item behind a key, nullptr for a template or an unknown key
    RibbonLayoutItemHost* item(const QString& key) const;
    // Key of a registered item, empty when it is not registered
    QString key(RibbonLayoutItemHost* item) const;
    // Tag an item is filed under (UnknowActionTag when not registered)
    int tagOf(RibbonLayoutItemHost* item) const;
    // Number of registered descriptors
    int count() const;
    // Every registered descriptor, ordered by tag then registration
    QList< SARibbonQml::RibbonActionDescriptor > allActions() const;
    // Descriptors whose text contains `text` (case insensitive, widgets search parity)
    QList< SARibbonQml::RibbonActionDescriptor > search(const QString& text) const;
    // Drop every descriptor and tag name and reset the key salt
    void clear();

    // Walk a bar host tree and register everything customizable in it
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
     * \endif
     *
     * \if CHINESE
     * @brief 某个 tag 新增或被移除
     * @param tag 发生变化的 tag
     * @param isdelete 该 tag 失去最后一个描述符时为 true
     * \endif
     */
    void actionTagChanged(int tag, bool isdelete);

    /**
     * \if ENGLISH
     * @brief The descriptor tables changed in any way
     * @details Emitted on every registration, removal, tag rename and clear, so
     *          QML models and repeaters bound to the registry refresh without
     *          having to track the tag level themselves.
     * \endif
     *
     * \if CHINESE
     * @brief 描述符表发生任何变化
     * @details 每次注册、移除、tag 改名与清空都会发射，使绑定注册表的 QML
     *          model 与 repeater 无需自行跟踪 tag 层级即可刷新。
     * \endif
     */
    void registryChanged();

private Q_SLOTS:
    void onItemDestroyed(QObject* o);
    void onCategoryTitleChanged();

private:
    // Drop a descriptor from every table; removes emptied tags
    void removeDescriptor(const QString& key, bool enableEmit);
    // Generate the salt based fallback key (widgets registeAction parity)
    QString generateKey(RibbonLayoutItemHost* item);
};

}

#endif  // RIBBONACTIONREGISTRY_H
