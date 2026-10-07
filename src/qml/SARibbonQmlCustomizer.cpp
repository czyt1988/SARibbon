#include "SARibbonQmlCustomizer.h"
#include "SARibbonQmlActionRegistry.h"
#include "SARibbonQmlBar.h"
#include "SARibbonQmlCategory.h"
#include "SARibbonQmlPanel.h"
#include "SARibbonQmlToolButton.h"
#include "SARibbonQmlLayoutItemHost.h"
#include "SARibbonQmlQuickHost.h"
#include "SARibbonQmlQuickAccessBar.h"
#include "SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCustomizeXml.h>
#include <QBuffer>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace SARibbonQml {

RibbonCustomizer::RibbonCustomizer(QObject* parent) : QObject(parent)
{
}

RibbonCustomizer::~RibbonCustomizer()
{
}

RibbonBar* RibbonCustomizer::bar() const
{
    return mBar;
}

void RibbonCustomizer::setBar(RibbonBar* b)
{
    if (mBar == b) {
        return;
    }
    mBar = b;
    Q_EMIT barChanged();
}

RibbonActionRegistry* RibbonCustomizer::registry() const
{
    return mRegistry;
}

void RibbonCustomizer::setRegistry(RibbonActionRegistry* m)
{
    if (mRegistry == m) {
        return;
    }
    mRegistry = m;
    Q_EMIT registryChanged();
}

bool RibbonCustomizer::isEnforceCanCustomize() const
{
    return mEnforceCanCustomize;
}

void RibbonCustomizer::setEnforceCanCustomize(bool on)
{
    if (mEnforceCanCustomize == on) {
        return;
    }
    mEnforceCanCustomize = on;
    Q_EMIT enforceCanCustomizeChanged();
}

// ---- record producers ----
bool RibbonCustomizer::addCategory(const QString& title, int index, const QString& objName)
{
    const QString name = objName.isEmpty() ? generateObjectName(QStringLiteral("qml_category")) : objName;
    appendRecord(Record::makeAddCategory(title, index, name));
    return true;
}

bool RibbonCustomizer::removeCategory(const QString& categoryObjName)
{
    appendRecord(Record::makeRemoveCategory(categoryObjName));
    return true;
}

bool RibbonCustomizer::addPanel(const QString& title, int index, const QString& categoryObjName, const QString& objName)
{
    const QString name = objName.isEmpty() ? generateObjectName(QStringLiteral("qml_panel")) : objName;
    appendRecord(Record::makeAddPanel(title, index, categoryObjName, name));
    return true;
}

bool RibbonCustomizer::removePanel(const QString& categoryObjName, const QString& panelObjName)
{
    appendRecord(Record::makeRemovePanel(categoryObjName, panelObjName));
    return true;
}

bool RibbonCustomizer::addAction(const QString& key, int proportion, const QString& categoryObjName, const QString& panelObjName)
{
    appendRecord(Record::makeAddAction(key,
                                       SARibbon::Core::SARibbonRowProportion(proportion),
                                       categoryObjName,
                                       panelObjName));
    return true;
}

bool RibbonCustomizer::removeAction(const QString& categoryObjName, const QString& panelObjName, const QString& key)
{
    appendRecord(Record::makeRemoveAction(categoryObjName, panelObjName, key));
    return true;
}

bool RibbonCustomizer::changeCategoryOrder(const QString& categoryObjName, int moveIndex)
{
    appendRecord(Record::makeChangeCategoryOrder(categoryObjName, moveIndex));
    return true;
}

bool RibbonCustomizer::changePanelOrder(const QString& categoryObjName, const QString& panelObjName, int moveIndex)
{
    appendRecord(Record::makeChangePanelOrder(categoryObjName, panelObjName, moveIndex));
    return true;
}

bool RibbonCustomizer::changeActionOrder(const QString& categoryObjName,
                                        const QString& panelObjName,
                                        const QString& key,
                                        int moveIndex)
{
    appendRecord(Record::makeChangeActionOrder(categoryObjName, panelObjName, key, moveIndex));
    return true;
}

bool RibbonCustomizer::renameCategory(const QString& newName, const QString& categoryObjName)
{
    appendRecord(Record::makeRenameCategory(newName, categoryObjName));
    return true;
}

bool RibbonCustomizer::renamePanel(const QString& newName, const QString& categoryObjName, const QString& panelObjName)
{
    appendRecord(Record::makeRenamePanel(newName, categoryObjName, panelObjName));
    return true;
}

bool RibbonCustomizer::visibleCategory(const QString& categoryObjName, bool isShow)
{
    appendRecord(Record::makeVisibleCategory(categoryObjName, isShow));
    return true;
}

bool RibbonCustomizer::addQuickAction(const QString& key, int index)
{
    appendRecord(Record::makeAddQuickAction(key, index));
    return true;
}

bool RibbonCustomizer::removeQuickAction(const QString& key)
{
    appendRecord(Record::makeRemoveQuickAction(key));
    return true;
}

bool RibbonCustomizer::changeQuickActionOrder(const QString& key, int moveIndex)
{
    appendRecord(Record::makeChangeQuickActionOrder(key, moveIndex));
    return true;
}

int RibbonCustomizer::recordCount() const
{
    return mRecords.size();
}

const QList< RibbonCustomizer::Record >& RibbonCustomizer::pendingRecords() const
{
    return mRecords;
}

/**
 * \if ENGLISH
 * @brief Publish one pending record as a full-key map
 * @details The record type is a plain core class with no QML registration, so the
 *          picker UI reads it through this map instead. Every key is present even
 *          for an out-of-range index (NOTES B60): a QML delegate that reads a
 *          missing map key gets undefined and Qt 6.7's V4 compiler does not
 *          recover when the key shows up on the next reset.
 * \endif
 *
 * \if CHINESE
 * @brief 把一条待应用记录发布成满 key 的 map
 * @details 记录类型是没有 QML 注册的 core 纯类，因此选取 UI 通过这个 map 读取。
 *          即使索引越界，每个 key 也都在（NOTES B60）：QML 委托读到缺失的 map
 *          key 会得到 undefined，而 Qt 6.7 的 V4 编译器在下一次 reset 后该 key
 *          出现时并不会恢复。
 * \endif
 */
QVariantMap RibbonCustomizer::recordInfo(int index) const
{
    QVariantMap m;
    m.insert(QStringLiteral("type"), int(Record::UnknowActionType));
    m.insert(QStringLiteral("index"), 0);
    m.insert(QStringLiteral("key"), QString());
    m.insert(QStringLiteral("categoryObjName"), QString());
    m.insert(QStringLiteral("panelObjName"), QString());
    m.insert(QStringLiteral("proportion"), int(SARibbon::Core::SARibbonRowProportion::Medium));
    if (index < 0 || index >= mRecords.size()) {
        return m;
    }
    const Record& r   = mRecords[ index ];
    m[ QStringLiteral("type") ]            = int(r.actionType());
    m[ QStringLiteral("index") ]           = r.indexValue;
    m[ QStringLiteral("key") ]             = r.keyValue;
    m[ QStringLiteral("categoryObjName") ] = r.categoryObjNameValue;
    m[ QStringLiteral("panelObjName") ]    = r.panelObjNameValue;
    m[ QStringLiteral("proportion") ]      = int(r.actionRowProportionValue);
    return m;
}

QVariantList RibbonCustomizer::recordInfoList() const
{
    QVariantList res;
    for (int i = 0; i < mRecords.size(); ++i) {
        res.append(recordInfo(i));
    }
    return res;
}

void RibbonCustomizer::clearRecords()
{
    if (mRecords.isEmpty()) {
        return;
    }
    mRecords.clear();
    Q_EMIT recordsChanged();
}

int RibbonCustomizer::simplifyRecords()
{
    const int before = mRecords.size();
    mRecords         = Record::simplify(mRecords);
    if (mRecords.size() != before) {
        Q_EMIT recordsChanged();
    }
    return mRecords.size();
}

/**
 * \if ENGLISH
 * @brief Apply the pending records to the bar
 * @details The list goes through the core simplify pass first, so an
 *          add-immediately-followed-by-remove pair never touches the tree — the
 *          same optimization the widgets side runs before applying. Records that
 *          apply move into the applied list (undo and persistence operate on
 *          that one); records that fail stay pending and report through
 *          applyFailed, so a picker UI can surface the reason and let the user
 *          retry after fixing the ribbon.
 * \endif
 *
 * \if CHINESE
 * @brief 把待应用记录应用到 bar 上
 * \details 列表先过一遍 core 的 simplify，因此"添加紧接着删除"的一对记录根本不会
 *          碰到树——与 widgets 侧应用前的优化一致。应用成功的记录进入已应用列表
 *          （撤销与持久化都针对它）；失败的记录留在待应用列表并通过 applyFailed
 *          报告，选取 UI 因此可以给出原因，让用户修好 ribbon 后重试。
 * \endif
 */
bool RibbonCustomizer::apply()
{
    if (!mBar) {
        return false;
    }
    if (mRecords.isEmpty()) {
        return true;
    }
    const QList< Record > list = Record::simplify(mRecords);
    QList< Record > done;
    QList< Record > failed;
    for (int i = 0; i < list.size(); ++i) {
        QString reason;
        if (applyRecord(list[ i ], &reason)) {
            done.append(list[ i ]);
        } else {
            failed.append(list[ i ]);
            Q_EMIT applyFailed(i, int(list[ i ].actionType()), reason);
        }
    }
    mApplied += done;
    mRecords  = failed;
    Q_EMIT recordsChanged();
    Q_EMIT appliedChanged();
    return failed.isEmpty();
}

/**
 * \if ENGLISH
 * @brief Undo every applied record, last first
 * @details Widgets sa_customize_datas_reverse parity, including its asymmetry:
 *          the record types that have an inverse are Add{Category,Panel,Action},
 *          the three ChangeOrder types, VisibleCategory and the three quick
 *          access types. Remove{Category,Panel,Action} and the two Rename types
 *          have none — widgets skips them with `default: continue`, and so does
 *          this. A reverse pass therefore restores structure and order but not
 *          deleted content or old titles.
 * \endif
 *
 * \if CHINESE
 * @brief 撤销全部已应用记录，从最后一条开始
 * @details 与 widgets sa_customize_datas_reverse 对齐，包括它的不对称性：有逆操作
 *          的记录类型是 Add{Category,Panel,Action}、三个 ChangeOrder 类型、
 *          VisibleCategory 以及三个快速访问栏类型。Remove{Category,Panel,Action}
 *          与两个 Rename 类型没有逆操作——widgets 用 `default: continue` 跳过，
 *          这里同样跳过。因此一次撤销能恢复结构与顺序，但恢复不了被删除的内容
 *          或旧标题。
 * \endif
 */
bool RibbonCustomizer::reverse()
{
    if (!mBar) {
        return false;
    }
    bool allOk = true;
    for (int i = mApplied.size() - 1; i >= 0; --i) {
        const Record r = reverseRecord(mApplied[ i ]);
        if (r.actionType() == Record::UnknowActionType) {
            continue;  // no inverse for this record type (widgets parity)
        }
        QString reason;
        if (!applyRecord(r, &reason)) {
            allOk = false;
            Q_EMIT applyFailed(i, int(r.actionType()), reason);
        }
    }
    if (!mApplied.isEmpty()) {
        mApplied.clear();
        Q_EMIT appliedChanged();
    }
    return allOk;
}

int RibbonCustomizer::appliedCount() const
{
    return mApplied.size();
}

QVariantList RibbonCustomizer::appliedInfoList() const
{
    QVariantList res;
    for (int i = 0; i < mApplied.size(); ++i) {
        const Record& r = mApplied[ i ];
        QVariantMap m;
        m.insert(QStringLiteral("type"), int(r.actionType()));
        m.insert(QStringLiteral("index"), r.indexValue);
        m.insert(QStringLiteral("key"), r.keyValue);
        m.insert(QStringLiteral("categoryObjName"), r.categoryObjNameValue);
        m.insert(QStringLiteral("panelObjName"), r.panelObjNameValue);
        m.insert(QStringLiteral("proportion"), int(r.actionRowProportionValue));
        res.append(m);
    }
    return res;
}

void RibbonCustomizer::clearApplied()
{
    if (mApplied.isEmpty()) {
        return;
    }
    mApplied.clear();
    Q_EMIT appliedChanged();
}

bool RibbonCustomizer::isApplied() const
{
    return !mApplied.isEmpty();
}

QByteArray RibbonCustomizer::appliedToXml() const
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    // the writer is built on a device, not on a QString: QXmlStreamWriter cannot
    // pick an encoding for a string target and customize data carries non-ASCII
    // titles (core SARibbonCustomizeXml note)
    QXmlStreamWriter xml(&buffer);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)  // QXmlStreamWriter always encodes XML in UTF-8.
    xml.setCodec("utf-8");                  // 在writeStartDocument之前指定编码
#endif
    xml.writeStartDocument();
    SARibbon::Core::recordsToXml(&xml, mApplied);
    xml.writeEndDocument();
    return data;
}

bool RibbonCustomizer::saveAppliedToFile(const QString& filePath) const
{
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    QXmlStreamWriter xml(&f);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)  // QXmlStreamWriter always encodes XML in UTF-8.
    xml.setCodec("utf-8");                  // 在writeStartDocument之前指定编码
#endif
    xml.writeStartDocument();
    const bool isOk = SARibbon::Core::recordsToXml(&xml, mApplied);
    xml.writeEndDocument();
    f.close();
    return isOk;
}

bool RibbonCustomizer::applyFromXml(const QByteArray& xmlData)
{
    QXmlStreamReader xml(xmlData);
    const QList< Record > records = SARibbon::Core::recordsFromXml< Record >(&xml);
    if (xml.hasError()) {
        return false;
    }
    if (records.isEmpty()) {
        return false;
    }
    if (!mBar) {
        return false;
    }
    bool allOk = true;
    for (int i = 0; i < records.size(); ++i) {
        QString reason;
        if (applyRecord(records[ i ], &reason)) {
            mApplied.append(records[ i ]);
        } else {
            allOk = false;
            Q_EMIT applyFailed(i, int(records[ i ].actionType()), reason);
        }
    }
    Q_EMIT appliedChanged();
    return allOk;
}

bool RibbonCustomizer::applyFromFile(const QString& filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    const QByteArray data = f.readAll();
    f.close();
    return applyFromXml(data);
}

void RibbonCustomizer::setCanCustomize(QObject* obj, bool canbe)
{
    SARibbon::Core::setCanCustomize(obj, canbe);
}

void RibbonCustomizer::appendRecord(const Record& r)
{
    mRecords.append(r);
    Q_EMIT recordsChanged();
}

/**
 * \if ENGLISH
 * @brief Run one record against the host tree
 * @details The QML mirror of SARibbonCustomizeData::apply. Object resolution
 *          goes by objectName for categories and panels (same as widgets) and by
 *          registry key for commands, which is where the two front ends part
 *          ways: widgets asks its actions manager for a QAction pointer, this
 *          asks RibbonActionRegistry for a descriptor and materializes a host when
 *          the descriptor is only a template. Order records carry a RELATIVE move
 *          (widgets parity), so the current index is read first and the delta
 *          added to it.
 * @note AddAction and RemoveAction state an end state, not a transition: an item
 *       already attached, or already detached, makes the record a no-op success.
 *       Without that, undoing an add+remove pair fails on the replayed inverse,
 *       because the RemoveAction record has already taken the host out of the panel.
 * \endif
 *
 * \if CHINESE
 * @brief 把一条记录作用到宿主树上
 * @details SARibbonCustomizeData::apply 的 QML 镜像。对象解析对 category 与 panel
 *          走 objectName（与 widgets 相同），对命令走注册表 key，这正是两个前端
 *          分道的地方：widgets 向 actions manager 要一个 QAction 指针，这里向
 *          RibbonActionRegistry 要一条描述符，且当描述符只是模板时落地创建宿主。
 *          顺序类记录携带的是**相对**位移（与 widgets 一致），因此先读当前索引再
 *          加增量。
 * @note AddAction 与 RemoveAction 表达的是终态而非转变：项已经挂上、或已经摘下，
 *       都使该记录成为一次空操作的成功。没有这一点，撤销一对"增+删"记录会在重放
 *       的逆记录上失败，因为 RemoveAction 那条记录早已把宿主从面板里摘了出去。
 * \endif
 */
bool RibbonCustomizer::applyRecord(const Record& r, QString* reason)
{
    auto fail = [reason](const QString& why) {
        if (reason) {
            *reason = why;
        }
        return false;
    };
    if (!mBar) {
        return fail(QStringLiteral("no ribbon bar bound"));
    }

    switch (r.actionType()) {
    case Record::UnknowActionType:
        return fail(QStringLiteral("unknown record type"));

    case Record::AddCategoryActionType: {
        RibbonCategory* c = mBar->insertCategory(r.keyValue, r.indexValue);
        if (!c) {
            return fail(QStringLiteral("insertCategory failed"));
        }
        c->setObjectName(r.categoryObjNameValue);
        SARibbon::Core::setCanCustomize(c, true);
        c->ensureQmlLeaf();
        return true;
    }

    case Record::AddPanelActionType: {
        RibbonCategory* c = findCategory(r.categoryObjNameValue, reason);
        if (!c) {
            return false;
        }
        if (!canCustomize(c, reason)) {
            return false;
        }
        RibbonPanel* p = c->insertPanel(r.keyValue, r.indexValue);
        if (!p) {
            return fail(QStringLiteral("insertPanel failed"));
        }
        p->setObjectName(r.panelObjNameValue);
        SARibbon::Core::setCanCustomize(p, true);
        return true;
    }

    case Record::AddActionActionType: {
        RibbonPanel* p = findPanel(r.categoryObjNameValue, r.panelObjNameValue, reason);
        if (!p) {
            return false;
        }
        if (!canCustomize(p, reason)) {
            return false;
        }
        RibbonLayoutItemHost* item = resolveItem(r.keyValue);
        if (!item) {
            return fail(QStringLiteral("key not in registry: ") + r.keyValue);
        }
        if (p->childItemIndex(item) >= 0) {
            // already attached: the record's end state holds, so this is a no-op
            // success rather than an error. An undo of an add+remove pair replays
            // AddAction against a host a later record already put back
            SARibbon::Core::setCanCustomize(mRegistry->action(r.keyValue), true);
            return true;
        }
        if (!p->attachChildItem(item, -1)) {
            return fail(QStringLiteral("attachChildItem failed"));
        }
        item->ensureQmlLeaf();
        SARibbon::Core::setCanCustomize(mRegistry->action(r.keyValue), true);
        return true;
    }

    case Record::RemoveCategoryActionType: {
        RibbonCategory* c = findCategory(r.categoryObjNameValue, reason);
        if (!c) {
            return false;
        }
        if (!canCustomize(c, reason)) {
            return false;
        }
        return mBar->removeCategory(c) ? true : fail(QStringLiteral("removeCategory failed"));
    }

    case Record::RemovePanelActionType: {
        RibbonPanel* p = findPanel(r.categoryObjNameValue, r.panelObjNameValue, reason);
        if (!p) {
            return false;
        }
        RibbonCategory* c = findCategory(r.categoryObjNameValue, reason);
        if (!c || !canCustomize(p, reason)) {
            return false;
        }
        return c->removePanel(p) ? true : fail(QStringLiteral("removePanel failed"));
    }

    case Record::RemoveActionActionType: {
        RibbonPanel* p = findPanel(r.categoryObjNameValue, r.panelObjNameValue, reason);
        if (!p) {
            return false;
        }
        QAction* act = mRegistry ? mRegistry->action(r.keyValue) : nullptr;
        if (!act) {
            return fail(QStringLiteral("key not in registry: ") + r.keyValue);
        }
        RibbonLayoutItemHost* item = mRegistry->firstItem(r.keyValue);
        if (!item) {
            // no live view: nothing to detach (the record states an end state)
            return true;
        }
        if (!canCustomize(act, reason)) {
            return false;
        }
        if (p->childItemIndex(item) < 0) {
            // already detached: the record states an end state, not a transition,
            // so the undo of an add+remove pair succeeds instead of failing on an
            // item a later record already took out
            return true;
        }
        // detach, never destroy: the host stays owned by this customizer so the
        // inverse AddAction record can put the very same host back
        return p->detachChildItem(item) ? true : fail(QStringLiteral("detachChildItem failed"));
    }

    case Record::ChangeCategoryOrderActionType: {
        RibbonCategory* c = findCategory(r.categoryObjNameValue, reason);
        if (!c) {
            return false;
        }
        const int cur = mBar->categoryIndex(c);
        if (cur < 0) {
            return fail(QStringLiteral("category not in row"));
        }
        return mBar->moveCategory(cur, cur + r.indexValue) ? true : fail(QStringLiteral("moveCategory refused"));
    }

    case Record::ChangePanelOrderActionType: {
        RibbonCategory* c = findCategory(r.categoryObjNameValue, reason);
        RibbonPanel* p    = findPanel(r.categoryObjNameValue, r.panelObjNameValue, reason);
        if (!c || !p) {
            return false;
        }
        const int cur = c->panelIndex(p);
        if (cur < 0) {
            return fail(QStringLiteral("panel not in category"));
        }
        return c->movePanel(cur, cur + r.indexValue) ? true : fail(QStringLiteral("movePanel refused"));
    }

    case Record::ChangeActionOrderActionType: {
        RibbonPanel* p = findPanel(r.categoryObjNameValue, r.panelObjNameValue, reason);
        if (!p) {
            return false;
        }
        RibbonLayoutItemHost* item = mRegistry ? mRegistry->firstItem(r.keyValue) : nullptr;
        if (!item) {
            return fail(QStringLiteral("key has no live item: ") + r.keyValue);
        }
        const int cur = p->childItemIndex(item);
        if (cur < 0) {
            return fail(QStringLiteral("item not in panel"));
        }
        return p->moveChildItem(cur, cur + r.indexValue) ? true : fail(QStringLiteral("moveChildItem refused"));
    }

    case Record::RenameCategoryActionType: {
        RibbonCategory* c = findCategory(r.categoryObjNameValue, reason);
        if (!c) {
            return false;
        }
        if (!canCustomize(c, reason)) {
            return false;
        }
        c->setTitle(r.keyValue);
        return true;
    }

    case Record::RenamePanelActionType: {
        RibbonPanel* p = findPanel(r.categoryObjNameValue, r.panelObjNameValue, reason);
        if (!p) {
            return false;
        }
        if (!canCustomize(p, reason)) {
            return false;
        }
        p->setPanelTitle(r.keyValue);
        return true;
    }

    case Record::VisibleCategoryActionType: {
        RibbonCategory* c = findCategory(r.categoryObjNameValue, reason);
        if (!c) {
            return false;
        }
        if (1 == r.indexValue) {
            mBar->showCategory(c);
        } else {
            mBar->hideCategory(c);
        }
        return true;
    }

    case Record::AddQuickActionActionType: {
        RibbonQuickAccessBar* qab = mBar->quickAccessBar();
        if (!qab) {
            return fail(QStringLiteral("bar has no quick access bar"));
        }
        RibbonToolButton* btn = qobject_cast< RibbonToolButton* >(resolveItem(r.keyValue));
        if (!btn) {
            return fail(QStringLiteral("key does not resolve to a button: ") + r.keyValue);
        }
        if (!qab->attachButton(btn, r.indexValue)) {
            return fail(QStringLiteral("attachButton failed"));
        }
        btn->ensureQmlLeaf();
        SARibbon::Core::setCanCustomize(mRegistry->action(r.keyValue), true);
        return true;
    }

    case Record::RemoveQuickActionActionType: {
        RibbonQuickAccessBar* qab = mBar->quickAccessBar();
        if (!qab) {
            return fail(QStringLiteral("bar has no quick access bar"));
        }
        RibbonToolButton* btn = mRegistry ? qobject_cast< RibbonToolButton* >(mRegistry->firstItem(r.keyValue)) : nullptr;
        if (!btn) {
            return fail(QStringLiteral("key has no live button: ") + r.keyValue);
        }
        return qab->detachButton(btn) ? true : fail(QStringLiteral("detachButton failed"));
    }

    case Record::ChangeQuickActionOrderActionType: {
        RibbonQuickAccessBar* qab = mBar->quickAccessBar();
        if (!qab) {
            return fail(QStringLiteral("bar has no quick access bar"));
        }
        RibbonToolButton* btn = mRegistry ? qobject_cast< RibbonToolButton* >(mRegistry->firstItem(r.keyValue)) : nullptr;
        if (!btn) {
            return fail(QStringLiteral("key has no live button: ") + r.keyValue);
        }
        const int cur = qab->buttonIndex(btn);
        if (cur < 0) {
            return fail(QStringLiteral("button not in quick access bar"));
        }
        return qab->moveButton(cur, cur + r.indexValue) ? true : fail(QStringLiteral("moveButton refused"));
    }

    default:
        break;
    }
    return fail(QStringLiteral("unhandled record type"));
}

/**
 * \if ENGLISH
 * @brief Build the inverse of a record
 * @details Table-for-table the switch of widgets sa_customize_datas_reverse. A
 *          record type without an inverse yields an UnknowActionType record,
 *          which the caller skips — that is how the widgets `default: continue`
 *          branch is expressed here without silently pretending success.
 * \endif
 *
 * \if CHINESE
 * @brief 构建一条记录的逆记录
 * @details 逐条对应 widgets sa_customize_datas_reverse 的 switch。没有逆操作的
 *          记录类型返回一条 UnknowActionType 记录，由调用方跳过——这就是 widgets
 *          `default: continue` 分支在这里的表达方式，不会假装成功。
 * \endif
 */
RibbonCustomizer::Record RibbonCustomizer::reverseRecord(const Record& r)
{
    switch (r.actionType()) {
    case Record::AddCategoryActionType:
        return Record::makeRemoveCategory(r.categoryObjNameValue);
    case Record::AddPanelActionType:
        return Record::makeRemovePanel(r.categoryObjNameValue, r.panelObjNameValue);
    case Record::AddActionActionType:
        return Record::makeRemoveAction(r.categoryObjNameValue, r.panelObjNameValue, r.keyValue);
    case Record::ChangeCategoryOrderActionType:
        return Record::makeChangeCategoryOrder(r.categoryObjNameValue, -r.indexValue);
    case Record::ChangePanelOrderActionType:
        return Record::makeChangePanelOrder(r.categoryObjNameValue, r.panelObjNameValue, -r.indexValue);
    case Record::ChangeActionOrderActionType:
        return Record::makeChangeActionOrder(r.categoryObjNameValue, r.panelObjNameValue, r.keyValue, -r.indexValue);
    case Record::VisibleCategoryActionType:
        return Record::makeVisibleCategory(r.categoryObjNameValue, 1 != r.indexValue);
    case Record::AddQuickActionActionType:
        return Record::makeRemoveQuickAction(r.keyValue);
    case Record::RemoveQuickActionActionType:
        return Record::makeAddQuickAction(r.keyValue);
    case Record::ChangeQuickActionOrderActionType:
        return Record::makeChangeQuickActionOrder(r.keyValue, -r.indexValue);
    default:
        break;
    }
    return Record(Record::UnknowActionType);
}

/**
 * \if ENGLISH
 * @brief Resolve the host item behind a registry key
 * @details A descriptor that already carries a live item returns it — that is the
 *          re-attach path an undo of RemoveAction takes. A template descriptor
 *          gets materialized first. Anything else (unknown key, no registry) is a
 *          hard failure the caller reports.
 * \endif
 *
 * \if CHINESE
 * @brief 解析注册表 key 背后的宿主项
 * @details 已带活动项的描述符直接返回该项——这正是撤销 RemoveAction 时走的重新
 *          挂回路径。模板描述符先落地创建。其余情况（key 未知、没有注册表）都是
 *          由调用方报告的硬失败。
 * \endif
 */
RibbonLayoutItemHost* RibbonCustomizer::resolveItem(const QString& key)
{
    if (!mRegistry || key.isEmpty()) {
        return nullptr;
    }
    if (RibbonLayoutItemHost* live = mRegistry->firstItem(key)) {
        return live;
    }
    return materialize(key);
}

/**
 * \if ENGLISH
 * @brief Create the host of a command template
 * @details The new button is QObject-parented to this customizer, not to the
 *          panel it lands in. That is deliberate: a materialized command can be
 *          detached by an undo and re-attached by a redo, and can be moved
 *          between panels and into the quick access bar, so its lifetime must not
 *          be tied to any one container. The registry placement bookkeeping is
 *          repaired through attachItem; the key (the QAction objectName) never
 *          moves — records already written against it stay valid.
 * \endif
 *
 * \if CHINESE
 * @brief 为命令模板创建宿主
 * @details 新按钮的 QObject 父级是本定制器，而不是它落进去的面板。这是有意为之：
 *          落地的命令可以被撤销摘下、被重做重新挂上，还可以在面板之间以及快速
 *          访问栏之间移动，因此其生命周期不能绑死在任何一个容器上。注册表的放置
 *          簿记经 attachItem 修补；key（QAction 的 objectName）永不移动——已针对
 *          该 key 写下的记录继续有效。
 * \endif
 */
RibbonToolButton* RibbonCustomizer::materialize(const QString& key)
{
    const RibbonActionDescriptor d = mRegistry ? mRegistry->descriptor(key) : RibbonActionDescriptor();
    if (!d.isValid() || d.hasItem()) {
        return nullptr;
    }
    // plan-06 S2: the template's QAction IS the command — the created button
    // binds to it, so text/icon/tooltip/checked/enabled derive and stay in
    // sync for free (the 2.x snapshot lost the menu/popup/toolTip data on
    // materialize; that loss is structurally impossible now)
    RibbonToolButton* btn = new RibbonToolButton();
    btn->setParent(this);
    btn->setObjectName(QStringLiteral("button.") + key);
    btn->setAction(d.action.data());
    btn->setProportion(RibbonEnums::RowProportion(int(d.proportion)));
    mRegistry->attachItem(key, btn);
    return btn;
}

RibbonCategory* RibbonCustomizer::findCategory(const QString& objName, QString* reason) const
{
    RibbonCategory* c = mBar ? mBar->categoryByObjectName(objName) : nullptr;
    if (!c && reason) {
        *reason = QStringLiteral("no category named: ") + objName;
    }
    return c;
}

RibbonPanel* RibbonCustomizer::findPanel(const QString& categoryObjName, const QString& panelObjName, QString* reason) const
{
    RibbonCategory* c = findCategory(categoryObjName, reason);
    if (!c) {
        return nullptr;
    }
    RibbonPanel* p = c->panelByObjectName(panelObjName);
    if (!p && reason) {
        *reason = QStringLiteral("no panel named: ") + panelObjName;
    }
    return p;
}

/**
 * \if ENGLISH
 * @brief The customizable gate
 * @details The core marker defaults to false, so an unconditional check would
 *          refuse every operation on a freshly loaded declarative ribbon — the
 *          marker exists for applications that want to lock parts of their ribbon
 *          down, not as a barrier to entry. Hence the opt-in flag; when it is on
 *          the semantics are exactly the widgets ones.
 * \endif
 *
 * \if CHINESE
 * @brief 可定制闸门
 * @details core 的标记默认为 false，因此无条件检查会拒绝对刚加载的声明式 ribbon
 *          的一切操作——该标记是给想锁住 ribbon 某些部分的应用用的，不是入门
 *          门槛。所以做成可选开关；开关打开时语义与 widgets 完全一致。
 * \endif
 */
bool RibbonCustomizer::canCustomize(QObject* obj, QString* reason) const
{
    if (!mEnforceCanCustomize) {
        return true;
    }
    if (SARibbon::Core::isCanCustomize(obj)) {
        return true;
    }
    if (reason) {
        *reason = QStringLiteral("object is not marked customizable");
    }
    return false;
}

QString RibbonCustomizer::generateObjectName(const QString& prefix)
{
    return QStringLiteral("%1_%2").arg(prefix).arg(++mSerial);
}

}
