#include "SARibbonQmlActionRegistry.h"
#include <SARibbonCore/SARibbonCustomizeRecord.h>
#include "SARibbonQmlBar.h"
#include "SARibbonQmlCategory.h"
#include "SARibbonQmlPanel.h"
#include "SARibbonQmlLayoutItemHost.h"
#include "SARibbonQmlButtonRowHost.h"
#include "SARibbonQmlQuickAccessBar.h"
#include "SARibbonQmlToolButton.h"
#include <QDebug>
#include <QHash>
#include <QVariant>

namespace SARibbonQml {

// ---- RibbonActionDescriptor ----
bool RibbonActionDescriptor::isValid() const
{
    return !key.isEmpty();
}

bool RibbonActionDescriptor::hasItem() const
{
    return item != nullptr;
}

/**
 * \if ENGLISH
 * @brief Convert the descriptor into the map shape QML sees
 * @details Every key is present from construction (NOTES B60): a QML binding
 *          that reads a missing map key silently yields undefined and the V4
 *          compiler of Qt 6.7 does not recover when the key shows up later.
 *          `item` is published as a boolean, not as the pointer — QML code that
 *          needs the host goes through RibbonActionRegistry::item(key).
 * \endif
 *
 * \if CHINESE
 * @brief 把描述符转成 QML 侧看到的 map 形状
 * @details 每个 key 自构造起就存在（NOTES B60）：QML 绑定读到缺失的 map key 会
 *          静默得到 undefined，而 Qt 6.7 的 V4 编译器在该 key 后来出现时并不会
 *          恢复。`item` 以布尔发布而非指针——需要宿主的 QML 代码走
 *          RibbonActionRegistry::item(key)。
 * \endif
 */
QVariantMap RibbonActionDescriptor::toVariantMap() const
{
    QVariantMap m;
    m.insert(QStringLiteral("key"), key);
    m.insert(QStringLiteral("text"), text);
    m.insert(QStringLiteral("iconSource"), iconSource);
    m.insert(QStringLiteral("tag"), tag);
    m.insert(QStringLiteral("proportion"), int(proportion));
    m.insert(QStringLiteral("hasItem"), hasItem());
    return m;
}

// ---- RibbonActionRegistry ----
class RibbonActionRegistry::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(RibbonActionRegistry)
public:
    PrivateData(RibbonActionRegistry* p);
    void clear();

    QMap< int, QList< RibbonActionDescriptor > > mTagToActions;  ///< tag -> descriptors in registration order
    QMap< int, QString > mTagToName;                             ///< tag -> display name
    QHash< QString, RibbonLayoutItemHost* > mKeyToItem;          ///< key -> live host item (absent for templates)
    QMap< int, RibbonCategory* > mTagToCategory;                 ///< tag -> category, filled by autoRegister only
    int mSale;  ///< salt for generated keys: stable as long as the registration order is
};

RibbonActionRegistry::PrivateData::PrivateData(RibbonActionRegistry* p) : q_ptr(p), mSale(0)
{
}

void RibbonActionRegistry::PrivateData::clear()
{
    mTagToActions.clear();
    mTagToName.clear();
    mKeyToItem.clear();
    mTagToCategory.clear();
    mSale = 0;
}

RibbonActionRegistry::RibbonActionRegistry(QObject* parent)
    : QObject(parent), d_ptr(new RibbonActionRegistry::PrivateData(this))
{
}

RibbonActionRegistry::~RibbonActionRegistry()
{
}

void RibbonActionRegistry::setTagName(int tag, const QString& name)
{
    if (d_ptr->mTagToName.value(tag) == name) {
        return;
    }
    d_ptr->mTagToName[ tag ] = name;
    Q_EMIT registryChanged();
}

QString RibbonActionRegistry::tagName(int tag) const
{
    return d_ptr->mTagToName.value(tag);
}

/**
 * \if ENGLISH
 * @brief Remove a tag together with everything filed under it
 * @details Unlike unregisteAction this is a bulk drop: the descriptors of the
 *          tag leave the key table as well, so a later registeAction of the
 *          same item produces a fresh key. The tag name goes too — widgets
 *          removeTag keeps the name only while actions remain, and an empty
 *          tag with a lingering name would show up as a blank group header.
 * \endif
 *
 * \if CHINESE
 * @brief 移除某个 tag 连同其下全部描述符
 * @details 与 unregisteAction 不同，这是批量丢弃：该 tag 下的描述符同时离开
 *          key 表，因此之后对同一项再调 registeAction 会得到新的 key。tag 名
 *          也一并去掉——widgets 的 removeTag 只在仍有 action 时保留名字，而一
 *          个空 tag 拖着名字会显示成一个空白的分组标题。
 * \endif
 */
void RibbonActionRegistry::removeTag(int tag)
{
    if (!d_ptr->mTagToActions.contains(tag)) {
        return;
    }
    const QList< RibbonActionDescriptor > list = d_ptr->mTagToActions.value(tag);
    for (const RibbonActionDescriptor& d : list) {
        d_ptr->mKeyToItem.remove(d.key);
    }
    d_ptr->mTagToActions.remove(tag);
    d_ptr->mTagToName.remove(tag);
    d_ptr->mTagToCategory.remove(tag);
    Q_EMIT actionTagChanged(tag, true);
    Q_EMIT registryChanged();
}

/**
 * \if ENGLISH
 * @brief Register a live host item under a tag
 * @details Widgets registeAction parity, including the two behaviours that are
 *          easy to lose: an empty key falls back to the salt generator (so the
 *          same registration order always yields the same keys), and a key that
 *          is already taken is refused with a warning instead of silently
 *          shadowing the previous entry. The item's destroyed signal is hooked
 *          so a host that goes away takes its descriptor with it.
 * \endif
 *
 * \if CHINESE
 * @brief 把一个活动宿主项注册到某个 tag 下
 * @details 与 widgets registeAction 对齐，包括两个容易丢失的行为：key 为空时
 *          退回盐值生成器（因此相同的注册顺序总是得到相同的 key）；key 已被占
 *          用时带告警拒绝，而不是静默覆盖前一条。同时挂上项的 destroyed 信号，
 *          宿主销毁时其描述符一并离开注册表。
 * \endif
 */
bool RibbonActionRegistry::registeAction(RibbonLayoutItemHost* item, int tag, const QString& key, bool enableEmit)
{
    if (!item) {
        return false;
    }
    QString k = key;
    if (k.isEmpty()) {
        k = generateKey(item);
    }
    const QList< RibbonActionDescriptor > existing = d_ptr->mTagToActions.value(tag);
    if (!existing.isEmpty()) {
        for (const RibbonActionDescriptor& d : existing) {
            if (d.key == k) {
                qWarning() << "key: " << k << " have been exist,you can set key in an unique value when use "
                              "RibbonActionRegistry::registeAction";
                return false;
            }
        }
    }

    RibbonActionDescriptor d;
    d.key         = k;
    d.tag         = tag;
    d.item        = item;
    d.text        = item->property("text").toString();
    d.iconSource  = item->property("iconSource").toString();
    const QVariant rp = item->property("proportion");
    d.proportion  = rp.isValid() ? SARibbon::Core::SARibbonRowProportion(rp.toInt())
                                 : SARibbon::Core::SARibbonRowProportion::Medium;

    const bool isNewTag = !d_ptr->mTagToActions.contains(tag);
    d_ptr->mTagToActions[ tag ].append(d);
    d_ptr->mKeyToItem.insert(k, item);
    connect(item, &QObject::destroyed, this, &RibbonActionRegistry::onItemDestroyed);
    if (isNewTag && enableEmit) {
        Q_EMIT actionTagChanged(tag, false);
    }
    Q_EMIT registryChanged();
    return true;
}

/**
 * \if ENGLISH
 * @brief Register a command template — a command with no live host item
 * @details This is the QML substitute for the widgets capability of collecting
 *          the actions that sit on the main window but nowhere in the ribbon:
 *          QML has no enumerable action owner to walk, so the application
 *          declares those commands explicitly. The descriptor keeps a null
 *          item; RibbonCustomizer materializes a host when the command is
 *          placed into a panel or into the quick access bar.
 * \endif
 *
 * \if CHINESE
 * @brief 注册命令模板——没有活动宿主项的命令
 * @details 这是 QML 对 widgets "收集挂在主窗口上但不在 ribbon 任何位置的
 *          action" 能力的替代：QML 没有可枚举的 action 宿主可遍历，因此由应用
 *          显式声明这些命令。描述符的 item 为空；命令被放进面板或快速访问栏
 *          时由 RibbonCustomizer 落地创建宿主。
 * \endif
 */
bool RibbonActionRegistry::registeCommand(int tag,
                                         const QString& key,
                                         const QString& text,
                                         const QString& iconSource,
                                         SARibbon::Core::SARibbonRowProportion proportion,
                                         bool enableEmit)
{
    if (key.isEmpty()) {
        qWarning() << "RibbonActionRegistry::registeCommand needs a non-empty key (a template has no item to derive one from)";
        return false;
    }
    if (d_ptr->mKeyToItem.contains(key) || descriptor(key).isValid()) {
        qWarning() << "key: " << key << " have been exist,you can set key in an unique value when use "
                      "RibbonActionRegistry::registeCommand";
        return false;
    }

    RibbonActionDescriptor d;
    d.key        = key;
    d.tag        = tag;
    d.text       = text;
    d.iconSource = iconSource;
    d.proportion = proportion;
    d.item       = nullptr;

    const bool isNewTag = !d_ptr->mTagToActions.contains(tag);
    d_ptr->mTagToActions[ tag ].append(d);
    if (isNewTag && enableEmit) {
        Q_EMIT actionTagChanged(tag, false);
    }
    Q_EMIT registryChanged();
    return true;
}

void RibbonActionRegistry::unregisteAction(RibbonLayoutItemHost* item, bool enableEmit)
{
    if (!item) {
        return;
    }
    unregisteKey(key(item), enableEmit);
}

void RibbonActionRegistry::unregisteKey(const QString& key, bool enableEmit)
{
    if (key.isEmpty()) {
        return;
    }
    removeDescriptor(key, enableEmit);
}

/**
 * \if ENGLISH
 * @brief Point an existing key at a live host item
 * @details The customizer calls this after it materialized a command template:
 *          the descriptor keeps its key (records already written against that
 *          key stay valid) and gains the item, so later records address the new
 *          host through the very same key. A key that is not registered is
 *          ignored — bindItem repairs a descriptor, it never creates one.
 * \endif
 *
 * \if CHINESE
 * @brief 让一个已存在的 key 指向活动宿主项
 * \details 定制器在把命令模板落地后调用本函数：描述符保留其 key（已针对该 key
 *          写下的记录继续有效）并获得 item，因此后续记录可以通过同一个 key 寻址
 *          新宿主。未注册的 key 直接忽略——bindItem 修补描述符，绝不新建。
 * \endif
 */
void RibbonActionRegistry::bindItem(const QString& key, RibbonLayoutItemHost* item)
{
    if (key.isEmpty() || !item) {
        return;
    }
    for (auto it = d_ptr->mTagToActions.begin(); it != d_ptr->mTagToActions.end(); ++it) {
        for (RibbonActionDescriptor& d : it.value()) {
            if (d.key == key) {
                d.item = item;
                d_ptr->mKeyToItem.insert(key, item);
                connect(item, &QObject::destroyed, this, &RibbonActionRegistry::onItemDestroyed);
                Q_EMIT registryChanged();
                return;
            }
        }
    }
}

/**
 * \if ENGLISH
 * @brief Mark every placed command customizable
 * @details Writes the core SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE dynamic property on
 *          each descriptor's live item. This is the QML stand-in for the widgets
 *          flow, where the application marks its own QAction objects: declarative
 *          QML hosts have no natural marking point, so the registry — which
 *          already walked the whole tree — offers to do it in one call. Only
 *          matters while RibbonCustomizer::enforceCanCustomize is on.
 * \endif
 *
 * \if CHINESE
 * @brief 把每个已摆出的命令标记为可定制
 * \details 对每条描述符的活动项写入 core 的 SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
 *          动态属性。这是 QML 对 widgets 流程的替代——widgets 侧由应用自行标记
 *          其 QAction 对象，而声明式 QML 宿主没有天然的标记点，因此由已经遍历过
 *          整棵树的注册表一次调用完成。仅在 RibbonCustomizer::enforceCanCustomize
 *          打开时才有意义。
 * \endif
 */
int RibbonActionRegistry::markCustomizable(bool canbe)
{
    int n = 0;
    for (auto it = d_ptr->mTagToActions.constBegin(); it != d_ptr->mTagToActions.constEnd(); ++it) {
        for (const RibbonActionDescriptor& d : it.value()) {
            if (d.item) {
                SARibbon::Core::setCanCustomize(d.item, canbe);
                ++n;
            }
        }
    }
    return n;
}

QList< RibbonActionDescriptor > RibbonActionRegistry::filter(int tag) const{
    return d_ptr->mTagToActions.value(tag);
}

QList< RibbonActionDescriptor > RibbonActionRegistry::actions(int tag) const
{
    return d_ptr->mTagToActions.value(tag);
}

QList< int > RibbonActionRegistry::actionTags() const
{
    return d_ptr->mTagToActions.keys();
}

/**
 * \if ENGLISH
 * @brief Look a descriptor up by key
 * @details The key table only stores live items (a template has none), so the
 *          search walks the tag lists. That is O(n) per lookup but n is the
 *          number of ribbon commands — a few hundred at most — and it keeps one
 *          single source of truth for the descriptor contents instead of a
 *          second hash that could drift.
 * \endif
 *
 * \if CHINESE
 * @brief 按 key 查找描述符
 * @details key 表只存活动项（模板没有），因此查找遍历 tag 列表。这是 O(n)
 *          的，但 n 是 ribbon 命令数——最多几百条——并且它让描述符内容只有
 *          一个真相来源，避免第二张可能漂移的哈希表。
 * \endif
 */
RibbonActionDescriptor RibbonActionRegistry::descriptor(const QString& key) const
{
    if (key.isEmpty()) {
        return RibbonActionDescriptor();
    }
    for (auto it = d_ptr->mTagToActions.constBegin(); it != d_ptr->mTagToActions.constEnd(); ++it) {
        for (const RibbonActionDescriptor& d : it.value()) {
            if (d.key == key) {
                return d;
            }
        }
    }
    return RibbonActionDescriptor();
}

RibbonLayoutItemHost* RibbonActionRegistry::item(const QString& key) const
{
    return d_ptr->mKeyToItem.value(key, nullptr);
}

QString RibbonActionRegistry::key(RibbonLayoutItemHost* item) const
{
    if (!item) {
        return QString();
    }
    for (auto it = d_ptr->mKeyToItem.constBegin(); it != d_ptr->mKeyToItem.constEnd(); ++it) {
        if (it.value() == item) {
            return it.key();
        }
    }
    return QString();
}

int RibbonActionRegistry::tagOf(RibbonLayoutItemHost* item) const
{
    const QString k = key(item);
    if (k.isEmpty()) {
        return int(SARibbon::Core::UnknowActionTag);
    }
    return descriptor(k).tag;
}

int RibbonActionRegistry::count() const
{
    int n = 0;
    for (auto it = d_ptr->mTagToActions.constBegin(); it != d_ptr->mTagToActions.constEnd(); ++it) {
        n += it.value().size();
    }
    return n;
}

QList< RibbonActionDescriptor > RibbonActionRegistry::allActions() const
{
    QList< RibbonActionDescriptor > res;
    for (auto it = d_ptr->mTagToActions.constBegin(); it != d_ptr->mTagToActions.constEnd(); ++it) {
        res += it.value();
    }
    return res;
}

QList< RibbonActionDescriptor > RibbonActionRegistry::search(const QString& text) const
{
    QList< RibbonActionDescriptor > res;
    if (text.isEmpty()) {
        return res;
    }
    const QList< RibbonActionDescriptor > all = allActions();
    for (const RibbonActionDescriptor& d : all) {
        if (d.text.contains(text, Qt::CaseInsensitive)) {
            res.append(d);
        }
    }
    return res;
}

void RibbonActionRegistry::clear()
{
    const QList< int > tags = d_ptr->mTagToActions.keys();
    d_ptr->clear();
    for (int tag : tags) {
        Q_EMIT actionTagChanged(tag, true);
    }
    Q_EMIT registryChanged();
}

/**
 * \if ENGLISH
 * @brief Walk a bar host tree and register everything customizable in it
 * @details Widgets autoRegisteActions parity in the part that matters for the
 *          customize dialog: one tag per category, taken from
 *          AutoCategoryDistinguishBeginTag upwards in category row order, the
 *          tag name set to the category title and kept in sync with later
 *          renames, and every panel child filed under the tag of the category
 *          that owns its panel. The returned map is the tag -> category table.
 *          Quick access bar buttons are filed under QuickAccessActionTag: on the
 *          widgets side the very same QAction objects are already reachable
 *          through their category tag, in QML they are hosts of their own.
 * @note Call this after the categories carry their titles, since the title is
 *       what names the tag. Calling it again on the same bar is safe: items
 *       that already hold a key keep it, so the tag table refreshes without
 *       duplicating the catalogue.
 * \endif
 *
 * \if CHINESE
 * @brief 遍历 bar 宿主树，把其中所有可定制内容注册进来
 * @details 在定制对话框真正需要的部分上与 widgets autoRegisteActions 对齐：
 *          每个 category 一个 tag，自 AutoCategoryDistinguishBeginTag 起按
 *          category 行序递增，tag 名取 category 标题并跟随后续改名同步，每个
 *          面板子项归入其面板所属 category 的 tag。返回的 map 即 tag ->
 *          category 表。快速访问栏按钮归入 QuickAccessActionTag：widgets 侧
 *          同一批 QAction 对象已可通过其 category tag 到达，QML 侧它们则是
 *          独立的宿主。
 * @note 请在 category 设置标题之后调用，因为标题就是 tag 名。对同一个 bar 重复
 *       调用是安全的：已持有 key 的项保持原 key，因此 tag 表会刷新而命令目录不
 *       会重复。
 * \endif
 */
QMap< int, RibbonCategory* > RibbonActionRegistry::autoRegister(RibbonBar* bar, bool enableEmit)
{
    QMap< int, RibbonCategory* > res;
    if (!bar) {
        return res;
    }
    int tag = int(SARibbon::Core::AutoCategoryDistinguishBeginTag);
    for (int i = 0; i < bar->categoryCount(); ++i) {
        RibbonCategory* c = bar->categoryAt(i);
        if (!c) {
            continue;
        }
        for (int p = 0; p < c->panelCount(); ++p) {
            RibbonPanel* panel = c->panelAt(p);
            if (!panel) {
                continue;
            }
            for (int k = 0; k < panel->childItemCount(); ++k) {
                RibbonLayoutItemHost* item = panel->childItemAt(k);
                // re-runnable (WS-C3): the customize dialog re-registers every
                // time it opens, and generateKey is serial based, so an item
                // that already has a key would be filed a second time under a
                // different key instead of being rejected as a duplicate
                if (!item || !key(item).isEmpty()) {
                    continue;
                }
                registeAction(item, tag, QString(), false);
            }
        }
        setTagName(tag, c->title());
        res[ tag ] = c;
        connect(c, &RibbonCategory::titleChanged, this, &RibbonActionRegistry::onCategoryTitleChanged,
                Qt::UniqueConnection);
        ++tag;
    }

    if (RibbonButtonRowHost* qab = bar->quickAccessBar()) {
        for (int i = 0; i < qab->buttonCount(); ++i) {
            if (RibbonLayoutItemHost* item = qobject_cast< RibbonLayoutItemHost* >(qab->buttonAt(i))) {
                if (key(item).isEmpty()) {
                    registeAction(item, QuickAccessActionTag, QString(), false);
                }
            }
        }
        if (d_ptr->mTagToActions.contains(QuickAccessActionTag)) {
            setTagName(QuickAccessActionTag, tr("quick access bar"));
        }
    }

    d_ptr->mTagToCategory = res;
    if (enableEmit) {
        for (auto it = res.constBegin(); it != res.constEnd(); ++it) {
            Q_EMIT actionTagChanged(it.key(), false);
        }
        if (d_ptr->mTagToActions.contains(QuickAccessActionTag)) {
            Q_EMIT actionTagChanged(QuickAccessActionTag, false);
        }
        Q_EMIT registryChanged();
    }
    return res;
}

/**
 * \if ENGLISH
 * @brief QML entry point of autoRegister
 * @details autoRegister returns a tag -> category map of C++ pointers, which
 *          QML has no way to hold, so a picker written in QML could not call it
 *          at all. This wrapper runs the same walk and reports the number of
 *          descriptors the catalogue holds afterwards, which is the only figure
 *          a QML caller can act on.
 * \endif
 *
 * \if CHINESE
 * @brief autoRegister 的 QML 入口
 * @details autoRegister 返回的是 tag -> category 的 C++ 指针 map，QML 无法持有，
 *          因此用 QML 写的选取器根本调不到它。本包装跑同一套遍历，改为返回遍历
 *          之后命令目录里的描述符数量——这是 QML 调用方唯一能用得上的数字。
 * \endif
 */
int RibbonActionRegistry::autoRegisterBar(RibbonBar* bar)
{
    autoRegister(bar);
    return count();
}

QVariantMap RibbonActionRegistry::actionInfo(const QString& key) const
{
    const RibbonActionDescriptor d = descriptor(key);
    if (!d.isValid()) {
        // still full-key (NOTES B60): an empty descriptor converts to a map
        // with every key present rather than an empty map
        return RibbonActionDescriptor().toVariantMap();
    }
    return d.toVariantMap();
}

QVariantList RibbonActionRegistry::actionInfoList(int tag) const
{
    QVariantList res;
    const QList< RibbonActionDescriptor > list = d_ptr->mTagToActions.value(tag);
    for (const RibbonActionDescriptor& d : list) {
        res.append(d.toVariantMap());
    }
    return res;
}

QVariantList RibbonActionRegistry::tagInfoList() const
{
    QVariantList res;
    for (auto it = d_ptr->mTagToActions.constBegin(); it != d_ptr->mTagToActions.constEnd(); ++it) {
        QVariantMap m;
        m.insert(QStringLiteral("tag"), it.key());
        m.insert(QStringLiteral("name"), d_ptr->mTagToName.value(it.key()));
        m.insert(QStringLiteral("count"), it.value().size());
        res.append(m);
    }
    return res;
}

QVariantList RibbonActionRegistry::searchInfo(const QString& text) const
{
    QVariantList res;
    const QList< RibbonActionDescriptor > list = search(text);
    for (const RibbonActionDescriptor& d : list) {
        res.append(d.toVariantMap());
    }
    return res;
}

void RibbonActionRegistry::onItemDestroyed(QObject* o)
{
    // the item is gone: drop its descriptor but keep the tag alive if other
    // descriptors remain (removeDescriptor handles the emptied-tag case).
    // static_cast, not qobject_cast: destroyed fires from ~QObject, by which
    // time the derived metaobject is already gone (widgets onActionDestroyed
    // does the same)
    RibbonLayoutItemHost* item = static_cast< RibbonLayoutItemHost* >(o);
    const QList< QString > keys = d_ptr->mKeyToItem.keys(item);
    for (const QString& k : keys) {
        removeDescriptor(k, true);
    }
}

void RibbonActionRegistry::onCategoryTitleChanged()
{
    RibbonCategory* c = qobject_cast< RibbonCategory* >(sender());
    if (!c) {
        return;
    }
    for (auto it = d_ptr->mTagToCategory.constBegin(); it != d_ptr->mTagToCategory.constEnd(); ++it) {
        if (it.value() == c) {
            setTagName(it.key(), c->title());
            return;
        }
    }
}

/**
 * \if ENGLISH
 * @brief Drop one descriptor from every table
 * @details A descriptor is filed under exactly one tag, but the removal still
 *          walks all tags: that is what makes the function correct for entries
 *          whose tag was changed behind the registry's back, and it costs one
 *          pass over a handful of lists. When the tag loses its last descriptor
 *          the tag itself goes, which is the widgets removeAction contract the
 *          customize dialog's group list relies on.
 * \endif
 *
 * \if CHINESE
 * @brief 从所有表中删除一条描述符
 * @details 一条描述符只归在一个 tag 下，但删除仍然遍历全部 tag：这使得即使某
 *          条目的 tag 在注册表之外被改动，本函数依然正确，代价只是对几条列表
 *          走一遍。当某个 tag 失去最后一条描述符时该 tag 一并消失——这正是
 *          定制对话框分组列表所依赖的 widgets removeAction 契约。
 * \endif
 */
void RibbonActionRegistry::removeDescriptor(const QString& key, bool enableEmit)
{
    if (key.isEmpty()) {
        return;
    }
    d_ptr->mKeyToItem.remove(key);
    QList< int > emptiedTags;
    for (auto it = d_ptr->mTagToActions.begin(); it != d_ptr->mTagToActions.end();) {
        QList< RibbonActionDescriptor >& list = it.value();
        for (int i = list.size() - 1; i >= 0; --i) {
            if (list[ i ].key == key) {
                list.removeAt(i);
            }
        }
        if (list.isEmpty()) {
            emptiedTags.append(it.key());
            d_ptr->mTagToCategory.remove(it.key());
            it = d_ptr->mTagToActions.erase(it);
        } else {
            ++it;
        }
    }
    if (enableEmit) {
        for (int tag : emptiedTags) {
            Q_EMIT actionTagChanged(tag, true);
        }
        Q_EMIT registryChanged();
    }
}

/**
 * \if ENGLISH
 * @brief Generate the fallback key of an item that carries no objectName
 * @details Widgets registeAction parity: `id_<salt>_<objectName>`. The salt is
 *          a plain counter, so the generated keys are reproducible for as long
 *          as the registration order does not change — which is exactly the
 *          guarantee the widgets manager documents. Declarative items that must
 *          stay addressable across sessions should set objectName; the
 *          generated key is a fallback, not a stable identity.
 * \endif
 *
 * \if CHINESE
 * @brief 为没有 objectName 的项生成兜底 key
 * @details 与 widgets registeAction 对齐：`id_<盐值>_<objectName>`。盐值是普通
 *          计数器，因此只要注册顺序不变，生成的 key 就可复现——这正是 widgets
 *          管理器所承诺的保证。需要跨会话保持可寻址的声明式项应当设置
 *          objectName；生成的 key 是兜底，不是稳定身份。
 * \endif
 */
QString RibbonActionRegistry::generateKey(RibbonLayoutItemHost* item)
{
    const QString objName = item ? item->objectName() : QString();
    return QStringLiteral("id_%1_%2").arg(d_ptr->mSale++).arg(objName);
}

}
