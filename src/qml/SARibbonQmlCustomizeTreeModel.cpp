#include "SARibbonQmlCustomizeTreeModel.h"
#include "SARibbonQmlActionRegistry.h"
#include "SARibbonQmlCustomizer.h"
#include "SARibbonQmlTypes.h"
#include "SARibbonQmlBar.h"
#include "SARibbonQmlCategory.h"
#include "SARibbonQmlPanel.h"
#include "SARibbonQmlLayoutItemHost.h"
#include "SARibbonQmlButtonRowHost.h"
#include "SARibbonQmlToolButton.h"
#include "SARibbonQmlQuickAccessBar.h"
#include <SARibbonCore/SARibbonCustomizeRecord.h>
#include <SARibbonCore/SARibbonEnums.h>

namespace SARibbonQml {

namespace {

// One command of the shadow tree (a panel child or a quick access button)
struct ShadowItem
{
    QString key;
    QString text;
    QString iconSource;
    int tag        = int(SARibbon::Core::UnknowActionTag);
    int proportion = int(SARibbon::Core::SARibbonRowProportion::Medium);
    bool canCustomize = false;
    bool pending      = false;  ///< created by a pending record, not by the ribbon
};

// One panel of the shadow tree
struct ShadowPanel
{
    QString objName;
    QString title;
    bool canCustomize = false;
    bool pending      = false;
    QVector< ShadowItem > items;
};

// One category page of the shadow tree
struct ShadowCategory
{
    QString objName;
    QString title;
    bool visible      = true;
    bool isContext    = false;
    bool canCustomize = false;
    bool pending      = false;
    QVector< ShadowPanel > panels;
};

// Insert honoring the host insert index rule (negative or past-the-end appends)
template< typename T >
void insertShadow(QVector< T >& v, const T& val, int index)
{
    const int at = (index < 0 || index > v.size()) ? v.size() : index;
    v.insert(at, val);
}

// Relative move, refused out of range exactly as the host move* functions do
template< typename T >
bool moveShadow(QVector< T >& v, int from, int to)
{
    if (from < 0 || from >= v.size() || to < 0 || to >= v.size() || from == to) {
        return false;
    }
    v.move(from, to);
    return true;
}

ShadowCategory* findShadowCategory(QVector< ShadowCategory >& cats, const QString& objName)
{
    if (objName.isEmpty()) {
        return nullptr;
    }
    for (int i = 0; i < cats.size(); ++i) {
        if (cats[ i ].objName == objName) {
            return &cats[ i ];
        }
    }
    return nullptr;
}

ShadowPanel* findShadowPanel(ShadowCategory& c, const QString& objName)
{
    if (objName.isEmpty()) {
        return nullptr;
    }
    for (int i = 0; i < c.panels.size(); ++i) {
        if (c.panels[ i ].objName == objName) {
            return &c.panels[ i ];
        }
    }
    return nullptr;
}

int findShadowItem(const ShadowPanel& p, const QString& key)
{
    if (key.isEmpty()) {
        return -1;
    }
    for (int i = 0; i < p.items.size(); ++i) {
        if (p.items[ i ].key == key) {
            return i;
        }
    }
    return -1;
}

int findShadowQuickItem(const QVector< ShadowItem >& items, const QString& key)
{
    if (key.isEmpty()) {
        return -1;
    }
    for (int i = 0; i < items.size(); ++i) {
        if (items[ i ].key == key) {
            return i;
        }
    }
    return -1;
}

}

/**
 * \if ENGLISH
 * @brief Replay one pending record against the shadow tree
 * @details Mirrors RibbonCustomizer::applyRecord record for record, with the host
 *          calls replaced by shadow edits. A record the shadow cannot honor
 *          (unknown objectName, unresolvable key) is skipped rather than
 *          reported: this is a preview, and the authoritative failure diagnostic
 *          belongs to apply(), which is the only place a record really runs.
 *          AddAction and RemoveAction keep the end-state reading the customizer
 *          gives them, so an add+remove pair cancels out here too.
 * \endif
 *
 * \if CHINESE
 * @brief 把一条待应用记录重放到影子树上
 * @details 逐条镜像 RibbonCustomizer::applyRecord，只是把宿主调用换成影子编辑。
 *          影子无法满足的记录（objectName 未知、key 无法解析）被跳过而不报错：
 *          这里只是预览，权威的失败诊断属于 apply()——那才是记录真正执行的地方。
 *          AddAction 与 RemoveAction 保持定制器给它们的"终态"读法，因此一对增删
 *          在这里同样互相抵消。
 * \endif
 */
static void replayRecord(const SARibbon::Core::SARibbonCustomizeRecord& r,
                         QVector< ShadowCategory >& cats,
                         QVector< ShadowItem >& quick,
                         RibbonActionRegistry* reg)
{
    using Record = SARibbon::Core::SARibbonCustomizeRecord;
    switch (r.actionType()) {
    case Record::UnknowActionType:
        break;

    case Record::AddCategoryActionType: {
        ShadowCategory c;
        c.objName      = r.categoryObjNameValue;
        c.title        = r.keyValue;
        c.canCustomize = true;
        c.pending      = true;
        insertShadow(cats, c, r.indexValue);
        break;
    }

    case Record::RemoveCategoryActionType: {
        ShadowCategory* c = findShadowCategory(cats, r.categoryObjNameValue);
        if (c) {
            cats.removeAt(int(c - cats.data()));
        }
        break;
    }

    case Record::AddPanelActionType: {
        ShadowCategory* c = findShadowCategory(cats, r.categoryObjNameValue);
        if (!c) {
            break;
        }
        ShadowPanel p;
        p.objName      = r.panelObjNameValue;
        p.title        = r.keyValue;
        p.canCustomize = true;
        p.pending      = true;
        insertShadow(c->panels, p, r.indexValue);
        break;
    }

    case Record::RemovePanelActionType: {
        ShadowCategory* c = findShadowCategory(cats, r.categoryObjNameValue);
        ShadowPanel* p    = c ? findShadowPanel(*c, r.panelObjNameValue) : nullptr;
        if (p) {
            c->panels.removeAt(int(p - c->panels.data()));
        }
        break;
    }

    case Record::AddActionActionType: {
        ShadowCategory* c = findShadowCategory(cats, r.categoryObjNameValue);
        ShadowPanel* p    = c ? findShadowPanel(*c, r.panelObjNameValue) : nullptr;
        if (!p || findShadowItem(*p, r.keyValue) >= 0) {
            break;  // no panel, or already attached: the end state holds
        }
        ShadowItem si;
        si.key         = r.keyValue;
        si.proportion  = int(r.actionRowProportionValue);
        si.canCustomize = true;
        si.pending     = true;
        if (reg) {
            const RibbonActionDescriptor d = reg->descriptor(r.keyValue);
            si.text       = d.text;
            si.iconSource = d.iconSource;
            si.tag        = d.tag;
        }
        p->items.append(si);
        break;
    }

    case Record::RemoveActionActionType: {
        ShadowCategory* c = findShadowCategory(cats, r.categoryObjNameValue);
        ShadowPanel* p    = c ? findShadowPanel(*c, r.panelObjNameValue) : nullptr;
        if (!p) {
            break;
        }
        const int idx = findShadowItem(*p, r.keyValue);
        if (idx >= 0) {
            p->items.removeAt(idx);
        }
        break;
    }

    case Record::ChangeCategoryOrderActionType: {
        ShadowCategory* c = findShadowCategory(cats, r.categoryObjNameValue);
        if (c) {
            const int cur = int(c - cats.data());
            moveShadow(cats, cur, cur + r.indexValue);
        }
        break;
    }

    case Record::ChangePanelOrderActionType: {
        ShadowCategory* c = findShadowCategory(cats, r.categoryObjNameValue);
        ShadowPanel* p    = c ? findShadowPanel(*c, r.panelObjNameValue) : nullptr;
        if (p) {
            const int cur = int(p - c->panels.data());
            moveShadow(c->panels, cur, cur + r.indexValue);
        }
        break;
    }

    case Record::ChangeActionOrderActionType: {
        ShadowCategory* c = findShadowCategory(cats, r.categoryObjNameValue);
        ShadowPanel* p    = c ? findShadowPanel(*c, r.panelObjNameValue) : nullptr;
        if (!p) {
            break;
        }
        const int cur = findShadowItem(*p, r.keyValue);
        if (cur >= 0) {
            moveShadow(p->items, cur, cur + r.indexValue);
        }
        break;
    }

    case Record::RenameCategoryActionType: {
        ShadowCategory* c = findShadowCategory(cats, r.categoryObjNameValue);
        if (c) {
            c->title = r.keyValue;
        }
        break;
    }

    case Record::RenamePanelActionType: {
        ShadowCategory* c = findShadowCategory(cats, r.categoryObjNameValue);
        ShadowPanel* p    = c ? findShadowPanel(*c, r.panelObjNameValue) : nullptr;
        if (p) {
            p->title = r.keyValue;
        }
        break;
    }

    case Record::VisibleCategoryActionType: {
        ShadowCategory* c = findShadowCategory(cats, r.categoryObjNameValue);
        if (c) {
            c->visible = (1 == r.indexValue);
        }
        break;
    }

    case Record::AddQuickActionActionType: {
        if (findShadowQuickItem(quick, r.keyValue) >= 0) {
            break;
        }
        ShadowItem si;
        si.key         = r.keyValue;
        si.tag         = RibbonActionRegistry::QuickAccessActionTag;
        si.canCustomize = true;
        si.pending     = true;
        if (reg) {
            const RibbonActionDescriptor d = reg->descriptor(r.keyValue);
            si.text       = d.text;
            si.iconSource = d.iconSource;
            si.proportion = int(d.proportion);
        }
        insertShadow(quick, si, r.indexValue);
        break;
    }

    case Record::RemoveQuickActionActionType: {
        const int idx = findShadowQuickItem(quick, r.keyValue);
        if (idx >= 0) {
            quick.removeAt(idx);
        }
        break;
    }

    case Record::ChangeQuickActionOrderActionType: {
        const int cur = findShadowQuickItem(quick, r.keyValue);
        if (cur >= 0) {
            moveShadow(quick, cur, cur + r.indexValue);
        }
        break;
    }
    }
}

RibbonCustomizeTreeModel::RibbonCustomizeTreeModel(QObject* p) : QAbstractListModel(p)
{
    mShowType = int(RibbonEnums::ShowAllCategory);
}

RibbonCustomizeTreeModel::~RibbonCustomizeTreeModel()
{
}

int RibbonCustomizeTreeModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : mRows.size();
}

const RibbonCustomizeTreeModel::Row* RibbonCustomizeTreeModel::rowAt(int row) const
{
    return (row >= 0 && row < mRows.size()) ? &mRows[ row ] : nullptr;
}

QVariant RibbonCustomizeTreeModel::data(const QModelIndex& index, int role) const
{
    const Row* r = rowAt(index.row());
    if (!index.isValid() || !r) {
        return QVariant();
    }
    switch (role) {
    case Qt::DisplayRole:
    case TitleRole:
        return r->title;
    case NodeTypeRole:
        return r->nodeType;
    case DepthRole:
        return r->depth;
    case CategoryObjNameRole:
        return r->categoryObjName;
    case PanelObjNameRole:
        return r->panelObjName;
    case KeyRole:
        return r->key;
    case IconSourceRole:
        return r->iconSource;
    case TagRole:
        return r->tag;
    case ProportionRole:
        return r->proportion;
    case NodeVisibleRole:
        return r->visible;
    case ContextCategoryRole:
        return r->isContext;
    case CanCustomizeRole:
        return r->canCustomize;
    case PendingRole:
        return r->pending;
    case IndexInParentRole:
        return r->indexInParent;
    case SiblingCountRole:
        return r->siblingCount;
    default:
        break;
    }
    return QVariant();
}

QHash< int, QByteArray > RibbonCustomizeTreeModel::roleNames() const
{
    QHash< int, QByteArray > roles;
    roles[ NodeTypeRole ]        = "nodeType";
    roles[ DepthRole ]           = "depth";
    roles[ TitleRole ]           = "title";
    roles[ CategoryObjNameRole ] = "categoryObjName";
    roles[ PanelObjNameRole ]    = "panelObjName";
    roles[ KeyRole ]             = "key";
    roles[ IconSourceRole ]      = "iconSource";
    roles[ TagRole ]             = "tag";
    roles[ ProportionRole ]      = "proportion";
    roles[ NodeVisibleRole ]     = "nodeVisible";
    roles[ ContextCategoryRole ] = "contextCategory";
    roles[ CanCustomizeRole ]    = "canCustomize";
    roles[ PendingRole ]         = "pending";
    roles[ IndexInParentRole ]   = "indexInParent";
    roles[ SiblingCountRole ]    = "siblingCount";
    return roles;
}

RibbonBar* RibbonCustomizeTreeModel::bar() const
{
    return mBar;
}

void RibbonCustomizeTreeModel::setBar(RibbonBar* b)
{
    if (mBar == b) {
        return;
    }
    mBar = b;
    Q_EMIT barChanged();
    rebuild();
}

RibbonActionRegistry* RibbonCustomizeTreeModel::registry() const
{
    return mRegistry;
}

void RibbonCustomizeTreeModel::setRegistry(RibbonActionRegistry* m)
{
    if (mRegistry == m) {
        return;
    }
    mRegistry = m;
    Q_EMIT registryChanged();
    rebuild();
}

RibbonCustomizer* RibbonCustomizeTreeModel::customizer() const
{
    return mCustomizer;
}

/**
 * \if ENGLISH
 * @brief Bind the customizer whose pending records the preview replays
 * @details The model listens to recordsChanged so a picker UI appending records
 *          sees its own edits without calling update() after every button, and
 *          to appliedChanged because apply(), reverse() and applyFromXml() move
 *          the live host tree underneath the preview. The connections are
 *          dropped again when the customizer is replaced, through the same
 *          disconnect-everything call RibbonActionRegistryModel uses.
 * \endif
 *
 * \if CHINESE
 * @brief 绑定其待应用记录将被预览重放的定制器
 * @details 模型监听 recordsChanged，因此追加记录的选取 UI 无需在每次点击后再调
 *          update() 就能看到自己的编辑；也监听 appliedChanged，因为 apply()、
 *          reverse() 与 applyFromXml() 会在预览底下改动活动宿主树。替换定制器时
 *          这些连接会被断开，用的是 RibbonActionRegistryModel 同一套"断开全部"
 *          的写法。
 * \endif
 */
void RibbonCustomizeTreeModel::setCustomizer(RibbonCustomizer* c)
{
    if (mCustomizer == c) {
        return;
    }
    if (mCustomizer) {
        disconnect(mCustomizer, nullptr, this, nullptr);
    }
    mCustomizer = c;
    if (mCustomizer) {
        connect(mCustomizer, &RibbonCustomizer::recordsChanged, this, [this]() { rebuild(); });
        // reverse() and applyFromXml() touch the live host tree without ever
        // going through the pending list, so the preview has to follow them too
        connect(mCustomizer, &RibbonCustomizer::appliedChanged, this, [this]() { rebuild(); });
    }
    Q_EMIT customizerChanged();
    rebuild();
}

int RibbonCustomizeTreeModel::showType() const
{
    return mShowType;
}

void RibbonCustomizeTreeModel::setShowType(int t)
{
    if (mShowType == t) {
        return;
    }
    mShowType = t;
    Q_EMIT showTypeChanged();
    rebuild();
}

int RibbonCustomizeTreeModel::revision() const
{
    return mRevision;
}

void RibbonCustomizeTreeModel::update()
{
    rebuild();
}

QVariantMap RibbonCustomizeTreeModel::rowToMap(const Row& r)
{
    // full-key map (NOTES B60): every role is present, so a QML reader never
    // has to test for a key before using it
    QVariantMap m;
    m[ QStringLiteral("nodeType") ]        = r.nodeType;
    m[ QStringLiteral("depth") ]           = r.depth;
    m[ QStringLiteral("title") ]           = r.title;
    m[ QStringLiteral("categoryObjName") ] = r.categoryObjName;
    m[ QStringLiteral("panelObjName") ]    = r.panelObjName;
    m[ QStringLiteral("key") ]             = r.key;
    m[ QStringLiteral("iconSource") ]      = r.iconSource;
    m[ QStringLiteral("tag") ]             = r.tag;
    m[ QStringLiteral("proportion") ]      = r.proportion;
    m[ QStringLiteral("nodeVisible") ]     = r.visible;
    m[ QStringLiteral("contextCategory") ] = r.isContext;
    m[ QStringLiteral("canCustomize") ]    = r.canCustomize;
    m[ QStringLiteral("pending") ]         = r.pending;
    m[ QStringLiteral("indexInParent") ]   = r.indexInParent;
    m[ QStringLiteral("siblingCount") ]    = r.siblingCount;
    return m;
}

QVariantMap RibbonCustomizeTreeModel::infoAt(int row) const
{
    const Row* r = rowAt(row);
    return r ? rowToMap(*r) : rowToMap(Row());
}

int RibbonCustomizeTreeModel::rowOfCategory(const QString& categoryObjName) const
{
    for (int i = 0; i < mRows.size(); ++i) {
        if (mRows[ i ].nodeType == int(RibbonEnums::CategoryNode) && mRows[ i ].categoryObjName == categoryObjName) {
            return i;
        }
    }
    return -1;
}

int RibbonCustomizeTreeModel::rowOfPanel(const QString& categoryObjName, const QString& panelObjName) const
{
    for (int i = 0; i < mRows.size(); ++i) {
        const Row& r = mRows[ i ];
        if (r.nodeType == int(RibbonEnums::PanelNode) && r.categoryObjName == categoryObjName && r.panelObjName == panelObjName) {
            return i;
        }
    }
    return -1;
}

int RibbonCustomizeTreeModel::rowOfAction(const QString& categoryObjName, const QString& panelObjName, const QString& key) const
{
    for (int i = 0; i < mRows.size(); ++i) {
        const Row& r = mRows[ i ];
        if (r.nodeType == int(RibbonEnums::ActionNode) && r.categoryObjName == categoryObjName && r.panelObjName == panelObjName
            && r.key == key) {
            return i;
        }
    }
    return -1;
}

int RibbonCustomizeTreeModel::rowOfQuickAction(const QString& key) const
{
    for (int i = 0; i < mRows.size(); ++i) {
        const Row& r = mRows[ i ];
        if (r.nodeType == int(RibbonEnums::QuickAccessActionNode) && r.key == key) {
            return i;
        }
    }
    return -1;
}

/**
 * \if ENGLISH
 * @brief Read the host tree, replay the pending records, flatten into rows
 * @details Three passes. The first reads the live hosts into the shadow
 *          structures; a command is addressed through the registry key, so an
 *          item the registry does not know gets an empty key and is therefore
 *          not customizable — the record vocabulary has nothing to name it with.
 *          The second replays every pending record of the bound customizer. The
 *          third flattens under showType: ShowQuickAccessBar emits the quick
 *          access root and its buttons only, ShowMainCategory emits the declared
 *          main categories, ShowAllCategory appends the context pages after them
 *          (widgets categoryPages order) with bracketed titles.
 * @note Context pages are listed read-only. QML addressing runs through
 *       RibbonBar::categoryByObjectName, which walks the declared main row only,
 *       so a record naming a context page could never be applied; offering the
 *       edit would produce a guaranteed applyFailed. The widgets front end can
 *       rename a context page because its lookup covers every page.
 * \endif
 *
 * \if CHINESE
 * @brief 读取宿主树、重放待应用记录、拍平成行
 * @details 三趟。第一趟把活动宿主读进影子结构；命令通过注册表 key 寻址，因此
 *          注册表不认识的项得到空 key，也就不可定制——记录词汇表里没有可以称呼
 *          它的东西。第二趟重放绑定定制器的每条待应用记录。第三趟按 showType
 *          拍平：ShowQuickAccessBar 只发出快速访问根节点及其按钮，
 *          ShowMainCategory 发出声明的主类别，ShowAllCategory 在其后追加上下文页
 *          （widgets categoryPages 的顺序）并给标题加方括号。
 * @note 上下文页以只读方式列出。QML 的寻址走 RibbonBar::categoryByObjectName，
 *       它只遍历声明的主类别行，因此指名上下文页的记录永远无法应用；提供编辑入口
 *       只会换来一次必然的 applyFailed。widgets 前端能重命名上下文页，是因为它的
 *       查找覆盖所有页。
 * \endif
 */
void RibbonCustomizeTreeModel::rebuild()
{
    beginResetModel();
    mRows.clear();

    QVector< ShadowCategory > cats;
    QVector< ShadowCategory > ctxCats;
    QVector< ShadowItem > quick;
    bool hasQuickBar = false;

    // the customize gate only bites when the customizer asks for it; without a
    // bound customizer an addressable node is an editable node
    const bool gate = mCustomizer ? mCustomizer->isEnforceCanCustomize() : false;

    if (mBar) {
        for (int i = 0; i < mBar->categoryCount(); ++i) {
            RibbonCategory* c = mBar->categoryAt(i);
            if (!c) {
                continue;
            }
            ShadowCategory sc;
            sc.objName      = c->objectName();
            sc.title        = c->title();
            sc.visible      = !mBar->isCategoryHidden(c);
            sc.canCustomize = !sc.objName.isEmpty() && (!gate || SARibbon::Core::isCanCustomize(c));
            for (int p = 0; p < c->panelCount(); ++p) {
                RibbonPanel* panel = c->panelAt(p);
                if (!panel) {
                    continue;
                }
                ShadowPanel sp;
                sp.objName      = panel->objectName();
                sp.title        = panel->panelTitle();
                sp.canCustomize = !sp.objName.isEmpty() && (!gate || SARibbon::Core::isCanCustomize(panel));
                for (int k = 0; k < panel->childItemCount(); ++k) {
                    RibbonLayoutItemHost* item = panel->childItemAt(k);
                    if (!item) {
                        continue;
                    }
                    ShadowItem si;
                    si.key         = mRegistry ? mRegistry->key(item) : QString();
                    si.text        = item->property("text").toString();
                    si.iconSource  = item->property("iconSource").toString();
                    si.tag         = mRegistry ? mRegistry->tagOf(item) : int(SARibbon::Core::UnknowActionTag);
                    si.proportion  = item->property("proportion").toInt();
                    si.canCustomize = !si.key.isEmpty() && (!gate || SARibbon::Core::isCanCustomize(item));
                    sp.items.append(si);
                }
                sc.panels.append(sp);
            }
            cats.append(sc);
        }

        // context pages: same shape, but read-only (see the note above)
        const QVector< RibbonCategory* > ctxList = mBar->contextCategories();
        for (RibbonCategory* c : ctxList) {
            if (!c) {
                continue;
            }
            ShadowCategory sc;
            sc.objName      = c->objectName();
            sc.title        = c->title();
            sc.isContext    = true;
            sc.canCustomize = false;
            for (int p = 0; p < c->panelCount(); ++p) {
                RibbonPanel* panel = c->panelAt(p);
                if (!panel) {
                    continue;
                }
                ShadowPanel sp;
                sp.objName      = panel->objectName();
                sp.title        = panel->panelTitle();
                sp.canCustomize = false;
                for (int k = 0; k < panel->childItemCount(); ++k) {
                    RibbonLayoutItemHost* item = panel->childItemAt(k);
                    if (!item) {
                        continue;
                    }
                    ShadowItem si;
                    si.key        = mRegistry ? mRegistry->key(item) : QString();
                    si.text       = item->property("text").toString();
                    si.iconSource = item->property("iconSource").toString();
                    si.tag        = mRegistry ? mRegistry->tagOf(item) : int(SARibbon::Core::UnknowActionTag);
                    si.proportion = item->property("proportion").toInt();
                    sp.items.append(si);
                }
                sc.panels.append(sp);
            }
            ctxCats.append(sc);
        }

        if (RibbonButtonRowHost* qab = mBar->quickAccessBar()) {
            hasQuickBar = true;
            for (int i = 0; i < qab->buttonCount(); ++i) {
                RibbonLayoutItemHost* item = qobject_cast< RibbonLayoutItemHost* >(qab->buttonAt(i));
                if (!item) {
                    continue;
                }
                ShadowItem si;
                si.key         = mRegistry ? mRegistry->key(item) : QString();
                si.text        = item->property("text").toString();
                si.iconSource  = item->property("iconSource").toString();
                si.tag         = RibbonActionRegistry::QuickAccessActionTag;
                si.proportion  = item->property("proportion").toInt();
                si.canCustomize = !si.key.isEmpty() && (!gate || SARibbon::Core::isCanCustomize(item));
                quick.append(si);
            }
        }
    }

    if (mCustomizer) {
        const QList< SARibbon::Core::SARibbonCustomizeRecord >& pending = mCustomizer->pendingRecords();
        for (const SARibbon::Core::SARibbonCustomizeRecord& r : pending) {
            replayRecord(r, cats, quick, mRegistry);
        }
    }

    // ---- flatten ----
    if (mShowType == int(RibbonEnums::ShowQuickAccessBar)) {
        if (hasQuickBar || !quick.isEmpty()) {
            Row root;
            root.nodeType      = int(RibbonEnums::QuickAccessNode);
            root.depth         = 0;
            root.title         = tr("Quick Access Bar");
            root.canCustomize  = true;
            root.indexInParent = 0;
            root.siblingCount  = 1;
            mRows.append(root);
            for (int i = 0; i < quick.size(); ++i) {
                Row r;
                r.nodeType       = int(RibbonEnums::QuickAccessActionNode);
                r.depth          = 1;
                r.title          = quick[ i ].text;
                r.key            = quick[ i ].key;
                r.iconSource     = quick[ i ].iconSource;
                r.tag            = quick[ i ].tag;
                r.proportion     = quick[ i ].proportion;
                r.canCustomize   = quick[ i ].canCustomize;
                r.pending        = quick[ i ].pending;
                r.indexInParent  = i;
                r.siblingCount   = quick.size();
                mRows.append(r);
            }
        }
        endResetModel();
        // published after the reset, never inside it: a reader that reacts to
        // revisionChanged calls infoAt(), which must see a consistent model
        ++mRevision;
        Q_EMIT revisionChanged();
        return;
    }

    QVector< ShadowCategory > listed = cats;
    if (mShowType == int(RibbonEnums::ShowAllCategory)) {
        listed += ctxCats;
    }
    for (int i = 0; i < listed.size(); ++i) {
        const ShadowCategory& sc = listed[ i ];
        Row cr;
        cr.nodeType        = int(RibbonEnums::CategoryNode);
        cr.depth           = 0;
        // widgets brackets a context page title so the two kinds stay apart in
        // a flat list, where indentation alone cannot carry that distinction
        cr.title           = sc.isContext ? QStringLiteral("[%1]").arg(sc.title) : sc.title;
        cr.categoryObjName = sc.objName;
        cr.visible         = sc.visible;
        cr.isContext       = sc.isContext;
        cr.canCustomize    = sc.canCustomize;
        cr.pending         = sc.pending;
        cr.indexInParent   = i;
        cr.siblingCount    = listed.size();
        mRows.append(cr);

        for (int p = 0; p < sc.panels.size(); ++p) {
            const ShadowPanel& sp = sc.panels[ p ];
            Row pr;
            pr.nodeType        = int(RibbonEnums::PanelNode);
            pr.depth           = 1;
            pr.title           = sp.title;
            pr.categoryObjName = sc.objName;
            pr.panelObjName    = sp.objName;
            pr.canCustomize    = sp.canCustomize;
            pr.pending         = sp.pending;
            pr.indexInParent   = p;
            pr.siblingCount    = sc.panels.size();
            mRows.append(pr);

            for (int k = 0; k < sp.items.size(); ++k) {
                const ShadowItem& si = sp.items[ k ];
                Row ar;
                ar.nodeType        = int(RibbonEnums::ActionNode);
                ar.depth           = 2;
                ar.title           = si.text;
                ar.categoryObjName = sc.objName;
                ar.panelObjName    = sp.objName;
                ar.key             = si.key;
                ar.iconSource      = si.iconSource;
                ar.tag             = si.tag;
                ar.proportion      = si.proportion;
                ar.canCustomize    = si.canCustomize;
                ar.pending         = si.pending;
                ar.indexInParent   = k;
                ar.siblingCount    = sp.items.size();
                mRows.append(ar);
            }
        }
    }
    endResetModel();
    ++mRevision;
    Q_EMIT revisionChanged();
}

}
