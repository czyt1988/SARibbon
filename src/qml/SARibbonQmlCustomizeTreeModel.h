#ifndef RIBBONCUSTOMIZETREEMODEL_H
#define RIBBONCUSTOMIZETREEMODEL_H
#include "SARibbonQmlGlobal.h"
// full definitions, not forward declarations: the pointer Q_PROPERTYs below go
// through Qt 6's compile-time metatype check inside the moc output, which is
// assembled per target and cannot rely on another moc file happening to define
// the type first
#include "SARibbonQmlActionRegistry.h"
#include "SARibbonQmlCustomizer.h"
#include "SARibbonQmlBar.h"
#include <QAbstractListModel>
#include <QHash>
#include <QString>
#include <QVector>
#include <QVariantMap>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Flat model of the ribbon tree for the QML customize picker (plan 04 WS-C3)
 * @details The QML counterpart of the widgets SARibbonCustomizeWidget preview
 *          model (its QStandardItemModel over category / panel / action levels),
 *          flattened into a row list because the module targets Qt 5.12, where
 *          QtQuick.Controls has no TreeView. Every row carries its full address
 *          (category objectName, panel objectName, command key) plus its depth,
 *          so a delegate can indent it and an editor can build a record from it
 *          without walking parents.
 *          The rows are a PREVIEW: the live host tree is read once, then the
 *          pending records of the bound RibbonCustomizer are replayed against an
 *          in-memory shadow of that tree. Editing therefore never touches the
 *          ribbon until apply() runs, exactly as on the widgets side, and rows a
 *          pending record created are marked so the dialog can flag them.
 * @note Node type values are the widgets LevelRole numbers (category 0, panel 1,
 *       action 2, quick access bar root 3, quick access button 4), so the two
 *       front ends label a row identically. showType mirrors the widgets
 *       RibbonTreeShowType: ShowAllCategory lists context pages with bracketed
 *       titles, ShowMainCategory skips them, ShowQuickAccessBar lists the title
 *       bar row only.
 * \endif
 *
 * \if CHINESE
 * @brief 供 QML 定制选取器使用的 ribbon 树扁平模型（计划 04 WS-C3）
 * @details 对应 widgets 侧 SARibbonCustomizeWidget 的预览模型（其覆盖
 *          category/panel/action 三层的 QStandardItemModel），之所以拍平成行列表，
 *          是因为本模块面向 Qt 5.12，而该版本的 QtQuick.Controls 没有 TreeView。
 *          每行都携带完整地址（category 的 objectName、panel 的 objectName、命令
 *          key）以及深度，因此委托可以直接缩进，编辑器可以直接据其构造记录，无需
 *          回溯父节点。
 *          行是**预览**：先读一次活动宿主树，再把绑定的 RibbonCustomizer 的待应用
 *          记录重放到该树的一份内存影子上。因此编辑在 apply() 之前绝不触碰
 *          ribbon，与 widgets 侧一致；由待应用记录创建出来的行会被标记，供对话框
 *          加以区分。
 * @note 节点类型取值就是 widgets 的 LevelRole 数字（category 0、panel 1、
 *       action 2、快速访问栏根 3、快速访问按钮 4），两个前端因此对同一行给出相同
 *       标签。showType 对应 widgets 的 RibbonTreeShowType：ShowAllCategory 列出
 *       上下文页并给标题加方括号，ShowMainCategory 跳过它们，ShowQuickAccessBar
 *       只列标题栏那一排。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonCustomizeTreeModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(SARibbonQml::RibbonBar* bar READ bar WRITE setBar NOTIFY barChanged)
    Q_PROPERTY(SARibbonQml::RibbonActionRegistry* registry READ registry WRITE setRegistry NOTIFY registryChanged)
    Q_PROPERTY(SARibbonQml::RibbonCustomizer* customizer READ customizer WRITE setCustomizer NOTIFY customizerChanged)
    Q_PROPERTY(int showType READ showType WRITE setShowType NOTIFY showTypeChanged)
    Q_PROPERTY(int revision READ revision NOTIFY revisionChanged)
public:
    // Named roles of a row (Qt::UserRole based, the QML delegate vocabulary)
    enum Roles {
        NodeTypeRole = Qt::UserRole + 1,
        DepthRole,
        TitleRole,
        CategoryObjNameRole,
        PanelObjNameRole,
        KeyRole,
        IconSourceRole,
        TagRole,
        ProportionRole,
        NodeVisibleRole,
        ContextCategoryRole,
        CanCustomizeRole,
        PendingRole,
        IndexInParentRole,
        SiblingCountRole
    };

    explicit RibbonCustomizeTreeModel(QObject* p = nullptr);
    ~RibbonCustomizeTreeModel() override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash< int, QByteArray > roleNames() const override;

    RibbonBar* bar() const;
    void setBar(RibbonBar* b);
    RibbonActionRegistry* registry() const;
    void setRegistry(RibbonActionRegistry* m);
    // Source of the pending records replayed into the preview (may be null:
    // the model then shows the live tree unmodified)
    RibbonCustomizer* customizer() const;
    void setCustomizer(RibbonCustomizer* c);

    // Ribbon.ShowAllCategory / ShowMainCategory / ShowQuickAccessBar
    int showType() const;
    void setShowType(int t);

    // Bumped on every rebuild: a QML binding that reads row data through
    // infoAt() has no other way to learn that a reset kept the row count
    int revision() const;

    // Re-read the host tree and replay the pending records
    Q_INVOKABLE void update();

    // Full-key map of a row (invalid row -> every key present with defaults)
    Q_INVOKABLE QVariantMap infoAt(int row) const;
    // Row of a tree address, -1 when it is not in the current view
    Q_INVOKABLE int rowOfCategory(const QString& categoryObjName) const;
    Q_INVOKABLE int rowOfPanel(const QString& categoryObjName, const QString& panelObjName) const;
    Q_INVOKABLE int rowOfAction(const QString& categoryObjName, const QString& panelObjName, const QString& key) const;
    Q_INVOKABLE int rowOfQuickAction(const QString& key) const;

Q_SIGNALS:
    void barChanged();
    void registryChanged();
    void customizerChanged();
    void showTypeChanged();
    void revisionChanged();

private:
    // One flattened tree row; the shadow tree the pending records are replayed
    // against stays in the .cpp, only its flattened result is kept here
    struct Row
    {
        int nodeType      = 0;
        int depth         = 0;
        QString title;
        QString categoryObjName;
        QString panelObjName;
        QString key;
        QString iconSource;
        int tag           = 0;
        int proportion    = 0;
        bool visible      = true;
        bool isContext    = false;
        bool canCustomize = false;
        bool pending      = false;
        int indexInParent = 0;
        int siblingCount  = 0;
    };

    // Read the host tree, replay the pending records, flatten under showType
    void rebuild();
    // Row accessors shared by data() and the QML-facing queries
    const Row* rowAt(int row) const;
    static QVariantMap rowToMap(const Row& r);

    RibbonBar* mBar                 = nullptr;
    RibbonActionRegistry* mRegistry = nullptr;
    RibbonCustomizer* mCustomizer   = nullptr;
    int mShowType                   = 0;
    int mRevision                   = 0;
    QVector< Row > mRows;
};

}

#endif  // RIBBONCUSTOMIZETREEMODEL_H
