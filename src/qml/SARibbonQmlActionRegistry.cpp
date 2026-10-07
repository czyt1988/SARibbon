#include "SARibbonQmlActionRegistry.h"
#include <SARibbonCore/SARibbonCustomizeRecord.h>
#include "SARibbonQmlAction.h"
#include "SARibbonQmlBar.h"
#include "SARibbonQmlCategory.h"
#include "SARibbonQmlIconProvider.h"
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
QString RibbonActionDescriptor::key() const
{
    return action ? action->objectName() : QString();
}

QString RibbonActionDescriptor::text() const
{
    return action ? action->text() : QString();
}

QString RibbonActionDescriptor::iconSource() const
{
    if (!action) {
        return QString();
    }
    if (auto* ribbonAction = qobject_cast< RibbonAction* >(action.data())) {
        return ribbonAction->iconSource().toString();
    }
    if (!action->icon().isNull()) {
        return SAIconImageProvider::iconUrl(action.data()).toString();
    }
    return QString();
}

bool RibbonActionDescriptor::isValid() const
{
    return action != nullptr && !action->objectName().isEmpty();
}

bool RibbonActionDescriptor::hasItem() const
{
    for (RibbonLayoutItemHost* it : items) {
        if (it) {
            return true;
        }
    }
    return false;
}

/**
 * \if ENGLISH
 * @brief Convert the descriptor into the map shape QML sees
 * @details Every key is present from construction (NOTES B60): a QML binding
 *          that reads a missing map key silently yields undefined and the V4
 *          compiler of Qt 6.7 does not recover when the key shows up later.
 *          The values are live reads of the action; `hasItem` tells whether
 *          the command is currently placed, QML reaches the host itself
 *          through the button's `action` property, never through the map.
 * \endif
 *
 * \if CHINESE
 * @brief 把描述符转成 QML 侧看到的 map 形状
 * @details 每个 key 自构造起就存在（NOTES B60）：QML 绑定读到缺失的 map key 会
 *          静默得到 undefined，而 Qt 6.7 的 V4 编译器在该 key 后来出现时并不会
 *          恢复。取值是对 action 的活读取；`hasItem` 标明命令当前是否已摆出，
 *          QML 需要宿主时经按钮的 `action` 属性自行到达，不经 map。
 * \endif
 */
QVariantMap RibbonActionDescriptor::toVariantMap() const
{
    QVariantMap m;
    m.insert(QStringLiteral("key"), key());
    m.insert(QStringLiteral("text"), text());
    m.insert(QStringLiteral("iconSource"), iconSource());
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
    QHash< QString, QAction* > mKeyToAction;                     ///< key (objectName) -> command object
    QMap< int, RibbonCategory* > mTagToCategory;                 ///< tag -> category, filled by autoRegister only
};

RibbonActionRegistry::PrivateData::PrivateData(RibbonActionRegistry* p) : q_ptr(p)
{
}

void RibbonActionRegistry::PrivateData::clear()
{
    mTagToActions.clear();
    mTagToName.clear();
    mKeyToAction.clear();
    mTagToCategory.clear();
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
 *          same command produces a fresh entry. The tag name goes too — an
 *          empty tag with a lingering name would show up as a blank group
 *          header.
 * \endif
 *
 * \if CHINESE
 * @brief 移除某个 tag 连同其下全部描述符
 * @details 与 unregisteAction 不同，这是批量丢弃：该 tag 下的描述符同时离开
 *          key 表，因此之后对同一命令再调 registeAction 会得到新条目。tag 名
 *          也一并去掉——一个空 tag 拖着名字会显示成一个空白的分组标题。
 * \endif
 */
void RibbonActionRegistry::removeTag(int tag)
{
    if (!d_ptr->mTagToActions.contains(tag)) {
        return;
    }
    const QList< RibbonActionDescriptor > list = d_ptr->mTagToActions.value(tag);
    for (const RibbonActionDescriptor& d : list) {
        d_ptr->mKeyToAction.remove(d.key());
    }
    d_ptr->mTagToActions.remove(tag);
    d_ptr->mTagToName.remove(tag);
    d_ptr->mTagToCategory.remove(tag);
    Q_EMIT actionTagChanged(tag, true);
    Q_EMIT registryChanged();
}

/**
 * \if ENGLISH
 * @brief Register a command under a tag
 * @details The key is the action's objectName (contract §5); an explicit key
 *          argument must equal it, an action without one is refused with a
 *          warning — a command without an identity cannot be addressed by a
 *          customize record. The action's destroyed signal is hooked so a
 *          command that goes away takes its descriptor with it.
 * \endif
 *
 * \if CHINESE
 * @brief 把一条命令注册到某个 tag 下
 * @details key 即 action 的 objectName（契约 §5）；显式传入的 key 必须与之相等，
 *          没有 objectName 的 action 会被带告警拒绝——没有身份的命令无法被
 *          定制记录寻址。同时挂上 action 的 destroyed 信号，命令销毁时其描述符
 *          一并离开注册表。
 * \endif
 */
bool RibbonActionRegistry::registeAction(QAction* act, int tag, const QString& key, bool enableEmit)
{
    if (!act) {
        return false;
    }
    const QString k = act->objectName();
    if (k.isEmpty()) {
        qWarning() << "RibbonActionRegistry::registeAction refuses an action without objectName — the objectName "
                      "is the persistent command identity (contract §5)";
        return false;
    }
    if (!key.isEmpty() && key != k) {
        qWarning() << "RibbonActionRegistry::registeAction: key" << key << "differs from the action objectName"
                   << k << "— the objectName is the registry key (contract §5)";
        return false;
    }
    if (d_ptr->mKeyToAction.contains(k)) {
        qWarning() << "key: " << k << " have been exist,you can set key in an unique value when use "
                      "RibbonActionRegistry::registeAction";
        return false;
    }

    RibbonActionDescriptor d;
    d.action     = act;
    d.tag        = tag;
    const QVariant rp = act->property("saRibbonDefaultProportion");
    d.proportion = rp.isValid() ? SARibbon::Core::SARibbonRowProportion(rp.toInt())
                                 : SARibbon::Core::SARibbonRowProportion::Medium;

    const bool isNewTag = !d_ptr->mTagToActions.contains(tag);
    d_ptr->mTagToActions[ tag ].append(d);
    d_ptr->mKeyToAction.insert(k, act);
    connect(act, &QObject::destroyed, this, &RibbonActionRegistry::onActionDestroyed);
    if (isNewTag && enableEmit) {
        Q_EMIT actionTagChanged(tag, false);
    }
    Q_EMIT registryChanged();
    return true;
}

/**
 * \if ENGLISH
 * @brief Declare a command template — a command not placed anywhere yet
 * @details This is the QML substitute for the widgets capability of collecting
 *          the actions that sit on the main window but nowhere in the ribbon:
 *          QML has no enumerable action owner to walk, so the application
 *          declares those commands explicitly. A bare QAction is created here
 *          (owned by the registry) carrying the identity and display data; the
 *          customizer binds views to it when the command is placed.
 * \endif
 *
 * \if CHINESE
 * @brief 声明命令模板——尚未摆到任何位置的命令
 * @details 这是 QML 对 widgets "收集挂在主窗口上但不在 ribbon 任何位置的
 *          action" 能力的替代：QML 没有可枚举的 action 宿主可遍历，因此由应用
 *          显式声明这些命令。这里创建一个裸 QAction（归注册表所有）承载身份与
 *          显示数据；命令被放置时由定制器把视图绑定上去。
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
        qWarning() << "RibbonActionRegistry::registeCommand needs a non-empty key (the command identity)";
        return false;
    }
    if (d_ptr->mKeyToAction.contains(key)) {
        qWarning() << "key: " << key << " have been exist,you can set key in an unique value when use "
                      "RibbonActionRegistry::registeCommand";
        return false;
    }

    QAction* act = new QAction(this);
    act->setObjectName(key);
    act->setText(text);
    act->setProperty("saRibbonDefaultProportion", int(proportion));
    if (!iconSource.isEmpty()) {
        // mirror the url into the QIcon so the widgets world sees the same icon
        QUrl url(iconSource);
        QString path;
        if (url.scheme() == QLatin1String("qrc")) {
            path = QLatin1Char(':') + url.path();
        } else if (url.isLocalFile()) {
            path = url.toLocalFile();
        } else {
            path = iconSource;
        }
        act->setIcon(QIcon(path));
    }

    RibbonActionDescriptor d;
    d.action     = act;
    d.tag        = tag;
    d.proportion = proportion;

    const bool isNewTag = !d_ptr->mTagToActions.contains(tag);
    d_ptr->mTagToActions[ tag ].append(d);
    d_ptr->mKeyToAction.insert(key, act);
    connect(act, &QObject::destroyed, this, &RibbonActionRegistry::onActionDestroyed);
    if (isNewTag && enableEmit) {
        Q_EMIT actionTagChanged(tag, false);
    }
    Q_EMIT registryChanged();
    return true;
}

void RibbonActionRegistry::unregisteAction(QAction* act, bool enableEmit)
{
    if (!act) {
        return;
    }
    unregisteKey(act->objectName(), enableEmit);
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
 * @brief Track a live host item as a placement of the key's command
 * @details The binding itself is expressed by the button's `action` property
 *          (plan-05 S4) — this bookkeeping only keeps the descriptor's
 *          placement list current, so hasItem() and the QML views can answer
 *          "is this command placed". The item's destroyed signal detaches it
 *          from the list.
 * \endif
 *
 * \if CHINESE
 * @brief 把一个活动宿主项登记为某命令的一次放置
 * @details 绑定关系本身已由按钮的 `action` 属性表达（计划 05 S4）——这里的
 *          簿记只是让描述符的放置列表保持新鲜，使 hasItem() 与 QML 视图能回答
 *          "该命令当前是否已摆出"。项销毁时自动离开列表。
 * \endif
 */
void RibbonActionRegistry::attachItem(const QString& key, RibbonLayoutItemHost* item)
{
    if (key.isEmpty() || !item) {
        return;
    }
    for (auto it = d_ptr->mTagToActions.begin(); it != d_ptr->mTagToActions.end(); ++it) {
        for (RibbonActionDescriptor& d : it.value()) {
            if (d.key() == key) {
                if (!d.items.contains(item)) {
                    d.items.append(item);
                    connect(item, &QObject::destroyed, this, [this, item](QObject*) {
                        detachItem(item);
                    });
                }
                Q_EMIT registryChanged();
                return;
            }
        }
    }
}

void RibbonActionRegistry::detachItem(RibbonLayoutItemHost* item)
{
    if (!item) {
        return;
    }
    for (auto it = d_ptr->mTagToActions.begin(); it != d_ptr->mTagToActions.end(); ++it) {
        for (RibbonActionDescriptor& d : it.value()) {
            d.items.removeAll(item);
        }
    }
}

/**
 * \if ENGLISH
 * @brief Mark every registered command customizable
 * @details Writes the core SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE marker on each
          command object. The command-level flag belongs to the QAction — on
          the widgets side the flag lives in the SARibbonActionsManager table
          since plan-07 S3, here it rides the core dynamic property on the
          same kind of object; every placement inherits it for free. Only
          matters while RibbonCustomizer::enforceCanCustomize is on.
 * \endif
 *
 * \if CHINESE
 * @brief 把每条已注册命令标记为可定制
 * @details 对每个命令对象写入 core 的 SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE 标记。
 \*          命令级标记属于 QAction——widgets 侧自计划 07 S3 起该标记保存在
 \*          SARibbonActionsManager 的表里，这里则以 core 动态属性落在同类对象上；
 \*          每次放置都免费继承它。仅在 RibbonCustomizer::enforceCanCustomize
 \*          打开时才有意义。
 * \endif
 */
int RibbonActionRegistry::markCustomizable(bool canbe)
{
    int n = 0;
    for (auto it = d_ptr->mTagToActions.constBegin(); it != d_ptr->mTagToActions.constEnd(); ++it) {
        for (const RibbonActionDescriptor& d : it.value()) {
            if (d.action) {
                SARibbon::Core::setCanCustomize(d.action.data(), canbe);
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

RibbonActionDescriptor RibbonActionRegistry::descriptor(const QString& key) const
{
    if (key.isEmpty()) {
        return RibbonActionDescriptor();
    }
    for (auto it = d_ptr->mTagToActions.constBegin(); it != d_ptr->mTagToActions.constEnd(); ++it) {
        for (const RibbonActionDescriptor& d : it.value()) {
            if (d.key() == key) {
                return d;
            }
        }
    }
    return RibbonActionDescriptor();
}

QAction* RibbonActionRegistry::action(const QString& key) const
{
    return d_ptr->mKeyToAction.value(key, nullptr);
}

QString RibbonActionRegistry::key(QAction* act) const
{
    if (!act) {
        return QString();
    }
    return d_ptr->mKeyToAction.key(act, QString());
}

int RibbonActionRegistry::tagOf(QAction* act) const
{
    const QString k = key(act);
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
        if (d.text().contains(text, Qt::CaseInsensitive)) {
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
 * @brief Walk a bar host tree and register every action-bound button in it
 * @details One tag per category, taken from AutoCategoryDistinguishBeginTag
 *          upwards in category row order, the tag name set to the category
 *          title and kept in sync with later renames. Only buttons carrying
 *          an `action` are filed (contract D6 two-level semantics: plain
 *          declarative buttons are not commands, the widgets customizer
 *          equally ignores addWidget content). The placement bookkeeping of
 *          every filed button is registered along with the command itself.
 * @note Call this after the categories carry their titles. Calling it again
 *       on the same bar is safe: commands that already hold a key keep it,
 *       so the tag table refreshes without duplicating the catalogue.
 * \endif
 *
 * \if CHINESE
 * @brief 遍历 bar 宿主树，把其中所有 action 绑定按钮注册进来
 * @details 每个 category 一个 tag，自 AutoCategoryDistinguishBeginTag 起按
 *          category 行序递增，tag 名取 category 标题并跟随后续改名同步。只
 *          归档携带 `action` 的按钮（契约 D6 两级语义：纯声明按钮不是命令，
 *          widgets 定制器同样不管 addWidget 内容）。已归档命令的放置簿记随
 *          命令本身一并登记。
 * @note 请在 category 设置标题之后调用。对同一个 bar 重复调用是安全的：已持有
 *       key 的命令保持原 key，因此 tag 表会刷新而命令目录不会重复。
 * \endif
 */
QMap< int, RibbonCategory* > RibbonActionRegistry::autoRegister(RibbonBar* bar, bool enableEmit)
{
    QMap< int, RibbonCategory* > res;
    if (!bar) {
        return res;
    }
    auto fileItem = [this](RibbonLayoutItemHost* item, int tag) {
        if (auto* btn = qobject_cast< RibbonToolButton* >(item)) {
            QAction* act = btn->action();
            if (!act) {
                return;  // plain-declarative button: not a command (contract D6)
            }
            const QString k = act->objectName();
            if (k.isEmpty()) {
                qWarning() << "autoRegister skips action without objectName, text:"
                           << act->text();
                return;
            }
            if (key(act).isEmpty()) {
                registeAction(act, tag, QString(), false);
            }
            attachItem(k, item);
        }
    };
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
                fileItem(panel->childItemAt(k), tag);
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
                fileItem(item, QuickAccessActionTag);
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

void RibbonActionRegistry::onActionDestroyed(QObject* o)
{
    // the command object is gone: drop its descriptor but keep the tag alive
    // if other descriptors remain. static_cast, not qobject_cast: destroyed
    // fires from ~QObject, by which time the derived metaobject is already
    // gone (widgets onActionDestroyed does the same)
    QAction* act = static_cast< QAction* >(o);
    const QString k = act->objectName();
    if (!k.isEmpty()) {
        d_ptr->mKeyToAction.remove(k);
    }
    removeDescriptor(k, true);
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

void RibbonActionRegistry::removeDescriptor(const QString& key, bool enableEmit)
{
    if (key.isEmpty()) {
        return;
    }
    d_ptr->mKeyToAction.remove(key);
    QList< int > emptiedTags;
    for (auto it = d_ptr->mTagToActions.begin(); it != d_ptr->mTagToActions.end();) {
        QList< RibbonActionDescriptor >& list = it.value();
        for (int i = list.size() - 1; i >= 0; --i) {
            if (list[ i ].key() == key) {
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

RibbonLayoutItemHost* RibbonActionRegistry::firstItem(const QString& key) const
{
    const RibbonActionDescriptor d = descriptor(key);
    for (RibbonLayoutItemHost* it : d.items) {
        if (it) {
            return it;
        }
    }
    return nullptr;
}

}
