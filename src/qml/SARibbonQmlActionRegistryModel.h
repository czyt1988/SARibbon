#ifndef RIBBONACTIONREGISTRYMODEL_H
#define RIBBONACTIONREGISTRYMODEL_H
#include "SARibbonQmlGlobal.h"
#include "SARibbonQmlActionRegistry.h"
#include <QAbstractListModel>
#include <QList>
#include <QString>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief List model over a RibbonActionRegistry for QML pickers (plan 04 WS-C2)
 * @details The QML counterpart of the widgets SARibbonActionsManagerModel: it
 *          mirrors the registry into a flat row list a ListView can bind to,
 *          narrowed by filterTag and searchText, and refreshes whenever the
 *          registry reports a tag or content change. Rows expose the descriptor
 *          fields as named roles, so a delegate binds `text`/`iconSource`
 *          directly instead of unpacking a map. filterTag set to
 *          Ribbon.UnknowActionTag means "every tag", which is what a search
 *          across the whole command catalogue needs.
 * @note The refresh is a full model reset, not a row level insert/remove: the
 *       registry reports tag granularity only, and a picker list of a few
 *       hundred commands gains nothing from partial updates.
 * \endif
 *
 * \if CHINESE
 * @brief 面向 QML 选取列表的 RibbonActionRegistry 列表模型（计划 04 WS-C2）
 * @details 对应 widgets 侧 SARibbonActionsManagerModel：把注册表映射成一个可供
 *          ListView 绑定的扁平行列表，按 filterTag 与 searchText 收窄，并在注册
 *          表报告 tag 或内容变化时刷新。行以具名 role 暴露描述符字段，委托因此
 *          可直接绑定 `text`/`iconSource`，无需拆解 map。filterTag 设为
 *          Ribbon.UnknowActionTag 表示"全部 tag"，这正是跨整个命令目录搜索所
 *          需要的。
 * @note 刷新是整模型 reset 而非行级 insert/remove：注册表只报告到 tag 粒度，
 *       而几百条命令的选取列表从局部更新中得不到任何好处。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonActionRegistryModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(SARibbonQml::RibbonActionRegistry* registry READ registry WRITE setRegistry NOTIFY registryChanged)
    Q_PROPERTY(int filterTag READ filterTag WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
public:
    // Named roles of a row (Qt::UserRole based, the QML delegate vocabulary)
    enum Roles {
        KeyRole = Qt::UserRole + 1,
        TextRole,
        IconSourceRole,
        TagRole,
        TagNameRole,
        ProportionRole,
        HasItemRole
    };

    explicit RibbonActionRegistryModel(QObject* p = nullptr);
    explicit RibbonActionRegistryModel(RibbonActionRegistry* m, QObject* p = nullptr);
    ~RibbonActionRegistryModel() override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash< int, QByteArray > roleNames() const override;

    RibbonActionRegistry* registry() const;
    void setRegistry(RibbonActionRegistry* m);
    // widgets setupActionsManager / uninstallActionsManager parity aliases
    void setupRegistry(RibbonActionRegistry* m);
    void uninstallRegistry();

    int filterTag() const;
    void setFilter(int tag);
    // Re-read the registry (widgets update parity)
    Q_INVOKABLE void update();

    QString searchText() const;
    void setSearchText(const QString& text);

    // Descriptor of a row (invalid when out of range)
    SARibbonQml::RibbonActionDescriptor descriptorAt(int row) const;
    // Row of a key, -1 when the key is not in the current view
    Q_INVOKABLE int rowOfKey(const QString& key) const;
    // QML-facing row accessors
    Q_INVOKABLE QString keyAt(int row) const;
    Q_INVOKABLE QVariantMap infoAt(int row) const;

Q_SIGNALS:
    void registryChanged();
    void filterChanged();
    void searchTextChanged();

private Q_SLOTS:
    void onActionTagChanged(int tag, bool isdelete);
    void onRegistryChanged();

private:
    // Rebuild the row list from the registry under the current filter/search
    void rebuild();

    RibbonActionRegistry* mRegistry = nullptr;
    QList< SARibbonQml::RibbonActionDescriptor > mRows;
    int mFilterTag = int(SARibbon::Core::UnknowActionTag);
    QString mSearchText;
};

}

#endif  // RIBBONACTIONREGISTRYMODEL_H
