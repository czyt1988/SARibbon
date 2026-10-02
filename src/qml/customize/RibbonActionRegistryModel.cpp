#include "RibbonActionRegistryModel.h"
#include <QHash>

namespace SARibbonQml {

RibbonActionRegistryModel::RibbonActionRegistryModel(QObject* p) : QAbstractListModel(p)
{
}

RibbonActionRegistryModel::RibbonActionRegistryModel(RibbonActionRegistry* m, QObject* p) : QAbstractListModel(p)
{
    setupRegistry(m);
}

RibbonActionRegistryModel::~RibbonActionRegistryModel()
{
}

int RibbonActionRegistryModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mRows.size();
}

QVariant RibbonActionRegistryModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mRows.size()) {
        return QVariant();
    }
    const RibbonActionDescriptor& d = mRows[ index.row() ];
    switch (role) {
    case Qt::DisplayRole:
    case TextRole:
        return d.text;
    case KeyRole:
        return d.key;
    case IconSourceRole:
        return d.iconSource;
    case TagRole:
        return d.tag;
    case TagNameRole:
        return mRegistry ? mRegistry->tagName(d.tag) : QString();
    case ProportionRole:
        return int(d.proportion);
    case HasItemRole:
        return d.hasItem();
    default:
        break;
    }
    return QVariant();
}

QHash< int, QByteArray > RibbonActionRegistryModel::roleNames() const
{
    QHash< int, QByteArray > roles;
    roles[ Qt::DisplayRole ] = QByteArrayLiteral("display");
    roles[ KeyRole ]         = QByteArrayLiteral("key");
    roles[ TextRole ]        = QByteArrayLiteral("text");
    roles[ IconSourceRole ]  = QByteArrayLiteral("iconSource");
    roles[ TagRole ]         = QByteArrayLiteral("tag");
    roles[ TagNameRole ]     = QByteArrayLiteral("tagName");
    roles[ ProportionRole ]  = QByteArrayLiteral("proportion");
    roles[ HasItemRole ]     = QByteArrayLiteral("hasItem");
    return roles;
}

RibbonActionRegistry* RibbonActionRegistryModel::registry() const
{
    return mRegistry;
}

void RibbonActionRegistryModel::setRegistry(RibbonActionRegistry* m)
{
    if (mRegistry == m) {
        return;
    }
    // disconnect inline rather than through uninstallRegistry: that one also
    // resets the rows, and swapping the source needs a single reset, not two
    if (mRegistry) {
        disconnect(mRegistry, nullptr, this, nullptr);
        mRegistry = nullptr;
    }
    mRegistry = m;
    if (mRegistry) {
        // both signals are hooked: the tag level one keeps the group list of a
        // picker in step, the content level one keeps the rows themselves right
        connect(mRegistry, &RibbonActionRegistry::actionTagChanged, this, &RibbonActionRegistryModel::onActionTagChanged);
        connect(mRegistry, &RibbonActionRegistry::registryChanged, this, &RibbonActionRegistryModel::onRegistryChanged);
    }
    Q_EMIT registryChanged();
    rebuild();
}

void RibbonActionRegistryModel::setupRegistry(RibbonActionRegistry* m)
{
    setRegistry(m);
}

/**
 * \if ENGLISH
 * @brief Drop the registry and the rows that mirror it
 * @details Disconnecting alone would leave a stale view: the rows are value
 *          copies of descriptors that carry live host pointers, so a model that
 *          no longer tracks the registry must stop publishing them. This is also
 *          the widgets behaviour, where uninstallActionsManager routes through
 *          setActionsManager(nullptr) and the row list is rebuilt empty.
 * \endif
 *
 * \if CHINESE
 * @brief 断开注册表，并丢弃映射自它的行
 * @details 只断连接会留下过期视图：行是描述符的值拷贝，其中带着活动的宿主指针，
 *          因此不再跟踪注册表的模型必须停止发布这些行。这也与 widgets 一致——
 *          uninstallActionsManager 走 setActionsManager(nullptr)，行列表被重建为空。
 * \endif
 */
void RibbonActionRegistryModel::uninstallRegistry()
{
    if (!mRegistry) {
        return;
    }
    disconnect(mRegistry, nullptr, this, nullptr);
    mRegistry = nullptr;
    Q_EMIT registryChanged();
    rebuild();
}

int RibbonActionRegistryModel::filterTag() const
{
    return mFilterTag;
}

void RibbonActionRegistryModel::setFilter(int tag)
{
    if (mFilterTag == tag) {
        return;
    }
    mFilterTag = tag;
    Q_EMIT filterChanged();
    rebuild();
}

void RibbonActionRegistryModel::update()
{
    rebuild();
}

QString RibbonActionRegistryModel::searchText() const
{
    return mSearchText;
}

void RibbonActionRegistryModel::setSearchText(const QString& text)
{
    if (mSearchText == text) {
        return;
    }
    mSearchText = text;
    Q_EMIT searchTextChanged();
    rebuild();
}

RibbonActionDescriptor RibbonActionRegistryModel::descriptorAt(int row) const
{
    if (row < 0 || row >= mRows.size()) {
        return RibbonActionDescriptor();
    }
    return mRows[ row ];
}

int RibbonActionRegistryModel::rowOfKey(const QString& key) const
{
    if (key.isEmpty()) {
        return -1;
    }
    for (int i = 0; i < mRows.size(); ++i) {
        if (mRows[ i ].key == key) {
            return i;
        }
    }
    return -1;
}

QString RibbonActionRegistryModel::keyAt(int row) const
{
    return descriptorAt(row).key;
}

QVariantMap RibbonActionRegistryModel::infoAt(int row) const
{
    const RibbonActionDescriptor d = descriptorAt(row);
    if (!d.isValid()) {
        return RibbonActionDescriptor().toVariantMap();
    }
    return d.toVariantMap();
}

void RibbonActionRegistryModel::onActionTagChanged(int tag, bool isdelete)
{
    Q_UNUSED(tag);
    Q_UNUSED(isdelete);
    // a tag appearing or disappearing can change whether the current filter has
    // any rows at all, so the view is rebuilt rather than patched
    rebuild();
}

void RibbonActionRegistryModel::onRegistryChanged()
{
    rebuild();
}

/**
 * \if ENGLISH
 * @brief Rebuild the row list from the registry under the current filter
 * @details Search first, tag second: a search is meant to reach across the whole
 *          catalogue, so narrowing by text before the tag keeps a match in an
 *          unfiltered tag visible while filterTag is UnknowActionTag, and a
 *          concrete filterTag still wins when one is set. Rows of a tag keep the
 *          registry's registration order, which is the ribbon's own left-to-right
 *          order — the order a picker list should show.
 * \endif
 *
 * \if CHINESE
 * @brief 按当前过滤条件从注册表重建行列表
 * @details 先搜索、后按 tag：搜索本意是跨整个目录，因此先按文本收窄再按 tag
 *          收窄，能在 filterTag 为 UnknowActionTag 时保留未过滤 tag 中的命中；
 *          而一旦设定了具体 filterTag，它仍然生效。同一 tag 内的行保持注册表的
 *          注册顺序，也就是 ribbon 自身的从左到右顺序——选取列表应当展示的
 *          顺序。
 * \endif
 */
void RibbonActionRegistryModel::rebuild()
{
    beginResetModel();
    mRows.clear();
    if (mRegistry) {
        QList< RibbonActionDescriptor > source;
        if (mSearchText.isEmpty()) {
            source = mRegistry->allActions();
        } else {
            source = mRegistry->search(mSearchText);
        }
        for (const RibbonActionDescriptor& d : static_cast< const QList< RibbonActionDescriptor >& >(source)) {
            if (mFilterTag == int(SARibbon::Core::UnknowActionTag) || d.tag == mFilterTag) {
                mRows.append(d);
            }
        }
    }
    endResetModel();
}

}
