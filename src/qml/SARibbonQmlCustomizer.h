#ifndef RIBBONCUSTOMIZER_H
#define RIBBONCUSTOMIZER_H
#include "SARibbonQmlGlobal.h"
// full definitions, not forward declarations: the pointer Q_PROPERTYs below go
// through Qt 6's compile-time metatype check inside the moc output, which is
// assembled per target and cannot rely on another moc file happening to define
// the type first (NOTES B64)
#include "SARibbonQmlActionRegistry.h"
#include "SARibbonQmlBar.h"
#include <SARibbonCore/SARibbonCustomizeRecord.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <QObject>
#include <QList>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

namespace SARibbonQml {

class RibbonBar;
class RibbonCategory;
class RibbonPanel;
class RibbonActionRegistry;
class RibbonLayoutItemHost;
class RibbonToolButton;

/**
 * \if ENGLISH
 * @brief Produces, applies, undoes and persists QML ribbon customizations (plan 04 WS-C2)
 * @details The QML counterpart of the widgets SARibbonCustomizeWidget — but not
 *          a translation of it. The record model is shared: every operation
 *          becomes a Core::SARibbonCustomizeRecord built by the same core make*
 *          factories the widgets front end forwards to, and persistence goes
 *          through the core recordsToXml/recordsFromXml, so a file written by
 *          one front end loads in the other. What differs is the addressing
 *          layer: widgets resolves QAction pointers through
 *          SARibbonActionsManager, this customizer resolves stable string keys
 *          through RibbonActionRegistry, and mutations run against the host tree
 *          query/mutation API instead of direct widget-toolbox calls.
 *          Two lists are kept, exactly as on the widgets side: the pending
 *          records a picker UI accumulates, and the applied records that undo
 *          and persistence operate on.
 * @note AddAction against a command template materializes a RibbonToolButton
 *       owned by this customizer, so a later undo can detach it and a later redo
 *       re-attach the same host. Declarative items are detached, never
 *       destroyed, which is the counterpart of widgets removeAction keeping the
 *       QAction alive inside the manager.
 * \endif
 *
 * \if CHINESE
 * @brief 产出、应用、撤销并持久化 QML ribbon 定制（计划 04 WS-C2）
 * @details 对应 widgets 侧 SARibbonCustomizeWidget——但不是它的直译。记录模型是
 *          共享的：每个操作都变成一条 Core::SARibbonCustomizeRecord，由 widgets
 *          前端同样转发过去的那批 core make* 工厂构建；持久化走 core 的
 *          recordsToXml/recordsFromXml，因此一个前端写出的文件另一个前端能读。
 *          不同的是寻址层：widgets 通过 SARibbonActionsManager 解析 QAction
 *          指针，本定制器通过 RibbonActionRegistry 解析稳定字符串 key，变更操作
 *          落在宿主树的查询/修改 API 上而非直接的工具栏/布局调用。
 *          与 widgets 一致地维护两张列表：选取 UI 累积的待应用记录，以及撤销与
 *          持久化所操作的已应用记录。
 * @note 对命令模板执行 AddAction 会落地创建一个归本定制器所有的
 *       RibbonToolButton，因此之后的撤销可以把它摘下、再次的重做可以把同一个
 *       宿主挂回去。声明式项只摘不销毁，对应 widgets removeAction 把 QAction
 *       留在 manager 里的做法。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonCustomizer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SARibbonQml::RibbonBar* bar READ bar WRITE setBar NOTIFY barChanged)
    Q_PROPERTY(SARibbonQml::RibbonActionRegistry* registry READ registry WRITE setRegistry NOTIFY registryChanged)
    Q_PROPERTY(int recordCount READ recordCount NOTIFY recordsChanged)
    Q_PROPERTY(int appliedCount READ appliedCount NOTIFY appliedChanged)
    Q_PROPERTY(bool enforceCanCustomize READ isEnforceCanCustomize WRITE setEnforceCanCustomize NOTIFY enforceCanCustomizeChanged)
public:
    using Record = SARibbon::Core::SARibbonCustomizeRecord;

    explicit RibbonCustomizer(QObject* parent = nullptr);
    ~RibbonCustomizer() override;

    RibbonBar* bar() const;
    void setBar(RibbonBar* b);
    RibbonActionRegistry* registry() const;
    void setRegistry(RibbonActionRegistry* m);

    // When on, a record touching an object not marked through the core
    // SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE property is refused. Off by default:
    // declarative QML hosts have no natural marking point, so requiring it would
    // leave a freshly loaded ribbon entirely uncustomizable
    bool isEnforceCanCustomize() const;
    void setEnforceCanCustomize(bool on);

    // ---- record producers (one per Core ActionType, widgets make* parity) ----
    Q_INVOKABLE bool addCategory(const QString& title, int index, const QString& objName = QString());
    Q_INVOKABLE bool removeCategory(const QString& categoryObjName);
    Q_INVOKABLE bool addPanel(const QString& title, int index, const QString& categoryObjName, const QString& objName = QString());
    Q_INVOKABLE bool removePanel(const QString& categoryObjName, const QString& panelObjName);
    Q_INVOKABLE bool addAction(const QString& key,
                               int proportion,
                               const QString& categoryObjName,
                               const QString& panelObjName);
    Q_INVOKABLE bool removeAction(const QString& categoryObjName, const QString& panelObjName, const QString& key);
    Q_INVOKABLE bool changeCategoryOrder(const QString& categoryObjName, int moveIndex);
    Q_INVOKABLE bool changePanelOrder(const QString& categoryObjName, const QString& panelObjName, int moveIndex);
    Q_INVOKABLE bool changeActionOrder(const QString& categoryObjName, const QString& panelObjName, const QString& key, int moveIndex);
    Q_INVOKABLE bool renameCategory(const QString& newName, const QString& categoryObjName);
    Q_INVOKABLE bool renamePanel(const QString& newName, const QString& categoryObjName, const QString& panelObjName);
    Q_INVOKABLE bool visibleCategory(const QString& categoryObjName, bool isShow);
    Q_INVOKABLE bool addQuickAction(const QString& key, int index = -1);
    Q_INVOKABLE bool removeQuickAction(const QString& key);
    Q_INVOKABLE bool changeQuickActionOrder(const QString& key, int moveIndex);

    // Pending record list access
    int recordCount() const;
    // Pending records in C++ shape: the customize tree model replays them into
    // its preview, which a QVariant round trip would only slow down
    const QList< Record >& pendingRecords() const;
    Q_INVOKABLE QVariantMap recordInfo(int index) const;
    Q_INVOKABLE QVariantList recordInfoList() const;
    // Drop the pending records (applied ones are untouched)
    Q_INVOKABLE void clearRecords();
    // Merge the pending records through the core simplify algorithm
    Q_INVOKABLE int simplifyRecords();

    // ---- apply / undo ----
    Q_INVOKABLE bool apply();
    Q_INVOKABLE bool reverse();
    int appliedCount() const;
    Q_INVOKABLE QVariantList appliedInfoList() const;
    Q_INVOKABLE void clearApplied();
    Q_INVOKABLE bool isApplied() const;

    // ---- persistence (core recordsToXml / recordsFromXml, byte compatible) ----
    Q_INVOKABLE QByteArray appliedToXml() const;
    Q_INVOKABLE bool saveAppliedToFile(const QString& filePath) const;
    // Read a record list and apply it (widgets sa_apply_customize_from_xml_file parity)
    Q_INVOKABLE bool applyFromXml(const QByteArray& xmlData);
    Q_INVOKABLE bool applyFromFile(const QString& filePath);

    // Mark an object customizable (core setCanCustomize passthrough)
    Q_INVOKABLE void setCanCustomize(QObject* obj, bool canbe = true);

Q_SIGNALS:
    void barChanged();
    void registryChanged();
    void recordsChanged();
    void appliedChanged();
    void enforceCanCustomizeChanged();
    /**
     * \if ENGLISH
     * @brief A record was refused or failed while applying
     * @param index position in the list handed to apply
     * @param type the Core ActionType of the record
     * @param reason short diagnostic (missing object, unmarked, unresolvable key)
     * \endif
     *
     * \if CHINESE
     * @brief 某条记录在应用时被拒绝或失败
     * @param index 在交给 apply 的列表中的位置
     * @param type 记录的 Core ActionType
     * @param reason 简短诊断（对象缺失、未标记、key 无法解析）
     * \endif
     */
    void applyFailed(int index, int type, const QString& reason);

private:
    // Append a record to the pending list
    void appendRecord(const Record& r);
    // Apply one record to the bar; reason receives the diagnostic on failure
    bool applyRecord(const Record& r, QString* reason);
    // Build the inverse of one record (widgets sa_customize_datas_reverse parity)
    static Record reverseRecord(const Record& r);
    // Resolve the live host item behind a key, materializing a template if needed
    RibbonLayoutItemHost* resolveItem(const QString& key);
    // Create an action-bound host for a command template (plan-06 S2: the
    // QAction is the command; the button derives everything from it)
    RibbonToolButton* materialize(const QString& key);
    // Locate a category / panel by objectName (nullptr with a diagnostic)
    RibbonCategory* findCategory(const QString& objName, QString* reason) const;
    RibbonPanel* findPanel(const QString& categoryObjName, const QString& panelObjName, QString* reason) const;
    // customizable gate: passes unless enforceCanCustomize is on and unmarked
    bool canCustomize(QObject* obj, QString* reason) const;
    // Generate a unique objectName for a host this customizer creates
    QString generateObjectName(const QString& prefix);

    RibbonBar* mBar                    = nullptr;
    RibbonActionRegistry* mRegistry    = nullptr;
    QList< Record > mRecords;          ///< pending records
    QList< Record > mApplied;          ///< records already applied (undo + persistence)
    bool mEnforceCanCustomize          = false;
    int mSerial                        = 0;
};

}

#endif  // RIBBONCUSTOMIZER_H
