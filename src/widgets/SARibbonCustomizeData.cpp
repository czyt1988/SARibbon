#include "SARibbonCustomizeData.h"
#include <SARibbonCore/SARibbonCustomizeRecord.h>
#include "SARibbonBar.h"
#include "SARibbonQuickAccessBar.h"
#include <QDebug>
#include <QObject>
////////////////////////////////////////////////////////////////////////////////////////////////////////
// SARibbonCustomizeData
////////////////////////////////////////////////////////////////////////////////////////////////////////

SARibbonCustomizeData::SARibbonCustomizeData()
    : SARibbon::Core::SARibbonCustomizeRecord()
    , mActionsManagerPointer(nullptr)
{
}

SARibbonCustomizeData::SARibbonCustomizeData(ActionType type, SARibbonActionsManager* mgr)
    : SARibbon::Core::SARibbonCustomizeRecord(type), mActionsManagerPointer(mgr)
{
}

/**
 * \if ENGLISH
 * @brief Adopt a pure core record
 * @details The core factories (plan 04 WS-C1) return SARibbonCustomizeRecord, which
 *          carries every persisted field but no manager; this constructor lifts one
 *          into the widgets type and attaches the manager the caller supplied.
 * \endif
 *
 * \if CHINESE
 * @brief 接收一条 core 纯记录
 * @details core 工厂（计划 04 WS-C1）返回 SARibbonCustomizeRecord，它带有全部
 *          持久化字段但没有 manager；本构造函数把它提升为 widgets 类型，
 *          并挂上调用方给出的 manager。
 * \endif
 */
SARibbonCustomizeData::SARibbonCustomizeData(const SARibbon::Core::SARibbonCustomizeRecord& record,
                                             SARibbonActionsManager* mgr)
    : SARibbon::Core::SARibbonCustomizeRecord(record), mActionsManagerPointer(mgr)
{
}

/**
 * \if ENGLISH
 * @brief Apply SARibbonCustomizeData to SARibbonBar
 * @param m SARibbonBar to apply to
 * @return If application fails, returns false; if actionType==UnknowActionType, directly returns false
 * \endif
 *
 * \if CHINESE
 * @brief 应用SARibbonCustomizeData到SARibbonBar
 * @param m 要应用到的 SARibbonBar
 * @return 如果应用失败，返回false,如果actionType==UnknowActionType直接返回false
 * \endif
 */
bool SARibbonCustomizeData::apply(SARibbonBar* bar) const
{
    if (nullptr == bar) {
        return (false);
    }
    switch (actionType()) {
    case UnknowActionType:
        return (false);

    case AddCategoryActionType: {
        // 添加标签
        SARibbonCategory* c = bar->insertCategoryPage(keyValue, indexValue);
        if (nullptr == c) {
            return (false);
        }
        c->setObjectName(categoryObjNameValue);
        SARibbonCustomizeData::setCanCustomize(c);
        return (true);
    }

    case AddPanelActionType: {
        // 添加panel
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPanel* p = c->insertPanel(keyValue, indexValue);
        p->setObjectName(panelObjNameValue);
        SARibbonCustomizeData::setCanCustomize(p);
        return (true);
    }

    case AddActionActionType: {
        if (nullptr == mActionsManagerPointer) {
            return (false);
        }
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPanel* panel = c->panelByObjectName(panelObjNameValue);
        if (nullptr == panel) {
            return (false);
        }
        QAction* act = mActionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        // plan-07 S3: 命令级可定制标记由 ActionsManager 持有（原 _sa_isCanCustomize 动态属性通道废除）
        mActionsManagerPointer->setCanCustomize(act);
        panel->addAction(act, actionRowProportionValue);
        return (true);
    }

    case RemoveCategoryActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        bar->removeCategory(c);
        return (true);
    }

    case RemovePanelActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPanel* panel = c->panelByObjectName(panelObjNameValue);
        if (nullptr == panel) {
            return (false);
        }
        c->removePanel(panel);
        return (true);
    }

    case RemoveActionActionType: {
        if (nullptr == mActionsManagerPointer) {
            return (false);
        }
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPanel* panel = c->panelByObjectName(panelObjNameValue);
        if (nullptr == panel) {
            return (false);
        }
        QAction* act = mActionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        panel->removeAction(act);
        return (true);
    }

    case ChangeCategoryOrderActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        int currentindex = bar->categoryIndex(c);
        if (-1 == currentindex) {
            return (false);
        }
        int toindex = currentindex + indexValue;
        bar->moveCategory(currentindex, toindex);
        return (true);
    }

    case ChangePanelOrderActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPanel* panel = c->panelByObjectName(panelObjNameValue);
        if (nullptr == panel) {
            return (false);
        }
        int panelIndex = c->panelIndex(panel);
        if (-1 == panelIndex) {
            return (false);
        }
        c->movePanel(panelIndex, panelIndex + indexValue);
        return (true);
    }

    case ChangeActionOrderActionType: {
        if (nullptr == mActionsManagerPointer) {
            return (false);
        }
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPanel* panel = c->panelByObjectName(panelObjNameValue);
        if (nullptr == panel) {
            return (false);
        }
        QAction* act = mActionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        int actindex = panel->actionIndex(act);
        if (actindex <= -1) {
            return (false);
        }
        panel->moveAction(actindex, actindex + indexValue);
        return (true);
    }

    case RenameCategoryActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        c->setCategoryName(keyValue);
        return (true);
    }

    case RenamePanelActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPanel* panel = c->panelByObjectName(panelObjNameValue);
        if (nullptr == panel) {
            return (false);
        }
        panel->setPanelName(keyValue);
        return (true);
    }

    case VisibleCategoryActionType: {
        SARibbonCategory* c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        if (1 == indexValue) {
            bar->showCategory(c);
        } else {
            bar->hideCategory(c);
        }
        return (true);
    }

    case AddQuickActionActionType: {
        // 添加action到快速访问栏（issue #67）
        if (nullptr == mActionsManagerPointer) {
            return (false);
        }
        SARibbonQuickAccessBar* quickBar = bar->quickAccessBar();
        if (nullptr == quickBar) {
            return (false);
        }
        QAction* act = mActionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        // plan-07 S3: 命令级可定制标记由 ActionsManager 持有
        mActionsManagerPointer->setCanCustomize(act);
        if (indexValue >= 0) {
            // 插到指定位置：取当前该位置的 action 作为 before 锚点
            const QList< QAction* > acts = quickBar->actions();
            if (indexValue < acts.size()) {
                quickBar->insertAction(acts.at(indexValue), act);
            } else {
                quickBar->addAction(act);
            }
        } else {
            quickBar->addAction(act);
        }
        return (true);
    }

    case RemoveQuickActionActionType: {
        // 从快速访问栏移除action（issue #67）
        if (nullptr == mActionsManagerPointer) {
            return (false);
        }
        SARibbonQuickAccessBar* quickBar = bar->quickAccessBar();
        if (nullptr == quickBar) {
            return (false);
        }
        QAction* act = mActionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        quickBar->removeAction(act);
        return (true);
    }

    case ChangeQuickActionOrderActionType: {
        // 改变快速访问栏action顺序（issue #67）
        if (nullptr == mActionsManagerPointer) {
            return (false);
        }
        SARibbonQuickAccessBar* quickBar = bar->quickAccessBar();
        if (nullptr == quickBar) {
            return (false);
        }
        QAction* act = mActionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        const QList< QAction* > acts = quickBar->actions();
        const int currentIndex       = acts.indexOf(act);
        if (currentIndex < 0) {
            return (false);
        }
        const int toIndex = currentIndex + indexValue;
        if (toIndex < 0 || toIndex >= acts.size()) {
            return (false);
        }
        // QToolBar 的移动 = 移除后插入到目标位置之前
        quickBar->removeAction(act);
        const QList< QAction* > actsAfterRemove = quickBar->actions();
        if (toIndex < actsAfterRemove.size()) {
            quickBar->insertAction(actsAfterRemove.at(toIndex), act);
        } else {
            quickBar->addAction(act);
        }
        return (true);
    }

    default:
        break;
    }
    return (false);
}

/**
 * \if ENGLISH
 * @brief Get the action manager pointer
 * @return SARibbonActionsManager pointer
 * \endif
 *
 * \if CHINESE
 * @brief 获取actionmanager指针
 * @return SARibbonActionsManager 指针
 * \endif
 */
SARibbonActionsManager* SARibbonCustomizeData::actionManager()
{
    return (mActionsManagerPointer);
}

/**
 * \if ENGLISH
 * @brief Set the ActionsManager
 * @param mgr SARibbonActionsManager pointer to set
 * \endif
 *
 * \if CHINESE
 * @brief 设置ActionsManager
 * @param mgr 要设置的 SARibbonActionsManager 指针
 * \endif
 */
void SARibbonCustomizeData::setActionsManager(SARibbonActionsManager* mgr)
{
    mActionsManagerPointer = mgr;
}

/**
 * \if ENGLISH
 * @brief Create an AddCategoryActionType SARibbonCustomizeData
 * @param title Category title
 * @param index Position to insert the category
 * @param objName Object name of the category
 * @return SARibbonCustomizeData with AddCategoryActionType
 * \endif
 *
 * \if CHINESE
 * @brief 创建一个AddCategoryActionType的SARibbonCustomizeData
 * @param title category 的标题
 * @param index category要插入的位置
 * @param objName category的object name
 * @return 返回AddCategoryActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddCategoryCustomizeData(const QString& title, int index, const QString& objName)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数保持原签名与原语义
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeAddCategory(title, index, objName)));
}

/**
 * \if ENGLISH
 * @brief Create an AddPanelActionType SARibbonCustomizeData
 * @param title Panel title
 * @param index Panel index
 * @param categoryobjName Object name of the panel's category
 * @param objName Object name of the panel
 * @return SARibbonCustomizeData with AddPanelActionType
 * \endif
 *
 * \if CHINESE
 * @brief 创建一个AddPanelActionType的SARibbonCustomizeData
 * @param title panel的标题
 * @param index panel的index
 * @param categoryobjName panel的category的objectname
 * @param objName panel的objname
 * @return 返回AddPanelActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddPanelCustomizeData(const QString& title,
                                                                       int index,
                                                                       const QString& categoryobjName,
                                                                       const QString& objName)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数保持原签名与原语义
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeAddPanel(title, index, categoryobjName, objName)));
}

/**
 * \if ENGLISH
 * @brief Add action
 * @param key Action index key
 * @param mgr Action manager
 * @param rp Define the action's row proportion
 * @param categoryObjName Object name of the category to add action to
 * @param panelObjName Object name of the panel under the category to add action to
 * @return SARibbonCustomizeData with AddActionActionType
 * \endif
 *
 * \if CHINESE
 * @brief 添加action
 * @param key action的索引
 * @param mgr action管理器
 * @param rp 定义action的占位情况
 * @param categoryObjName action添加到的category的objname
 * @param panelObjName action添加到的category下的panel的objname
 * @return 返回AddActionActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddActionCustomizeData(const QString& key,
                                                                        SARibbonActionsManager* mgr,
                                                                        SARibbonPanelItem::RowProportion rp,
                                                                        const QString& categoryObjName,
                                                                        const QString& panelObjName)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数只补 manager 指针
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeAddAction(key, rp, categoryObjName, panelObjName), mgr));
}

/**
 * \if ENGLISH
 * @brief Create a RenameCategoryActionType SARibbonCustomizeData
 * @param newname New name for the category
 * @param categoryobjName Object name of the category
 * @return SARibbonCustomizeData with RenameCategoryActionType
 * \endif
 *
 * \if CHINESE
 * @brief 创建一个RenameCategoryActionType的SARibbonCustomizeData
 * @param newname 新名字
 * @param categoryobjName category的object name
 * @return 返回RenameCategoryActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRenameCategoryCustomizeData(const QString& newname,
                                                                             const QString& categoryobjName)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数保持原签名与原语义
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeRenameCategory(newname, categoryobjName)));
}

/**
 * \if ENGLISH
 * @brief Create a RenamePanelActionType SARibbonCustomizeData
 * @param newname New name for the panel
 * @param categoryobjName Object name of the category the panel belongs to
 * @param panelObjName Object name of the panel
 * @return SARibbonCustomizeData with RenamePanelActionType
 * \endif
 *
 * \if CHINESE
 * @brief 创建一个RenamePanelActionType的SARibbonCustomizeData
 * @param newname panel的名字
 * @param categoryobjName panel对应的category的object name
 * @param panelObjName panel的object name
 * @return 返回RenamePanelActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRenamePanelCustomizeData(const QString& newname,
                                                                          const QString& categoryobjName,
                                                                          const QString& panelObjName)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数保持原签名与原语义
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeRenamePanel(newname, categoryobjName, panelObjName)));
}

/**
 * \if ENGLISH
 * @brief Create a ChangeCategoryOrderActionType SARibbonCustomizeData
 * @param categoryobjName Object name of the category to move
 * @param moveindex Move position, -1 means move up (left) one position, 1 means move down (right) one position
 * @return SARibbonCustomizeData with ChangeCategoryOrderActionType
 * \endif
 *
 * \if CHINESE
 * @brief 对应ChangeCategoryOrderActionType
 * @param categoryobjName 需要移动的categoryobjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return 返回ChangeCategoryOrderActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(const QString& categoryobjName,
                                                                                  int moveindex)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数保持原签名与原语义
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeChangeCategoryOrder(categoryobjName, moveindex)));
}

/**
 * \if ENGLISH
 * @brief Create a ChangePanelOrderActionType SARibbonCustomizeData
 * @param categoryobjName Object name of the category the panel belongs to
 * @param panelObjName Object name of the panel to move
 * @param moveindex Move position, -1 means move up (left) one position, 1 means move down (right) one position
 * @return SARibbonCustomizeData with ChangePanelOrderActionType
 * \endif
 *
 * \if CHINESE
 * @brief 对应ChangePanelOrderActionType
 * @param categoryobjName 需要移动的panel对应的categoryobjName
 * @param panelObjName 需要移动的panelObjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return 返回ChangePanelOrderActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeChangePanelOrderCustomizeData(const QString& categoryobjName,
                                                                               const QString& panelObjName,
                                                                               int moveindex)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数保持原签名与原语义
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeChangePanelOrder(categoryobjName, panelObjName, moveindex)));
}

/**
 * \if ENGLISH
 * @brief Create a ChangeActionOrderActionType SARibbonCustomizeData
 * @param categoryobjName Object name of the category the panel belongs to
 * @param panelObjName Object name of the panel the action belongs to
 * @param key Key name managed by SARibbonActionsManager
 * @param mgr SARibbonActionsManager pointer
 * @param moveindex Move position, -1 means move up (left) one position, 1 means move down (right) one position
 * @return SARibbonCustomizeData with ChangeActionOrderActionType
 * \endif
 *
 * \if CHINESE
 * @brief 对应ChangeActionOrderActionType
 * @param categoryobjName 需要移动的panel对应的categoryobjName
 * @param panelObjName 需要移动的panelObjName
 * @param key SARibbonActionsManager管理的key名
 * @param mgr SARibbonActionsManager指针
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return 返回ChangeActionOrderActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeChangeActionOrderCustomizeData(const QString& categoryobjName,
                                                                                const QString& panelObjName,
                                                                                const QString& key,
                                                                                SARibbonActionsManager* mgr,
                                                                                int moveindex)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数只补 manager 指针
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeChangeActionOrder(categoryobjName, panelObjName, key, moveindex), mgr));
}

/**
 * \if ENGLISH
 * @brief Create a RemoveCategoryActionType SARibbonCustomizeData
 * @param categoryobjName Object name of the category to remove
 * @return SARibbonCustomizeData with RemoveCategoryActionType
 * \endif
 *
 * \if CHINESE
 * @brief 对应RemoveCategoryActionType
 * @param categoryobjName 需要移除的objname
 * @return 返回RemoveCategoryActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRemoveCategoryCustomizeData(const QString& categoryobjName)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数保持原签名与原语义
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeRemoveCategory(categoryobjName)));
}

/**
 * \if ENGLISH
 * @brief Create a RemovePanelActionType SARibbonCustomizeData
 * @param categoryobjName Object name of the category the panel belongs to
 * @param panelObjName Object name of the panel to remove
 * @return SARibbonCustomizeData with RemovePanelActionType
 * \endif
 *
 * \if CHINESE
 * @brief 对应RemovePanelActionType
 * @param categoryobjName panel对应的category的obj name
 * @param panelObjName panel对应的 obj name
 * @return 返回RemovePanelActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRemovePanelCustomizeData(const QString& categoryobjName,
                                                                          const QString& panelObjName)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数保持原签名与原语义
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeRemovePanel(categoryobjName, panelObjName)));
}

/**
 * \if ENGLISH
 * @brief Create a RemoveActionActionType SARibbonCustomizeData
 * @param categoryobjName Object name of the category the panel belongs to
 * @param panelObjName Object name of the panel the action belongs to
 * @param key Key name managed by SARibbonActionsManager
 * @param mgr SARibbonActionsManager pointer
 * @return SARibbonCustomizeData with RemoveActionActionType
 * \endif
 *
 * \if CHINESE
 * @brief 对应RemoveActionActionType
 * @param categoryobjName panel对应的category的obj name
 * @param panelObjName panel对应的 obj name
 * @param key SARibbonActionsManager管理的key名
 * @param mgr SARibbonActionsManager指针
 * @return 返回RemoveActionActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRemoveActionCustomizeData(const QString& categoryobjName,
                                                                           const QString& panelObjName,
                                                                           const QString& key,
                                                                           SARibbonActionsManager* mgr)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数只补 manager 指针
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeRemoveAction(categoryobjName, panelObjName, key), mgr));
}

/**
 * \if ENGLISH
 * @brief Create a VisibleCategoryActionType SARibbonCustomizeData
 * @param categoryobjName Object name of the category
 * @param isShow Whether to show the category
 * @return SARibbonCustomizeData with VisibleCategoryActionType
 * \endif
 *
 * \if CHINESE
 * @brief 创建一个VisibleCategoryActionType的SARibbonCustomizeData
 * @param categoryobjName category的object name
 * @param isShow 是否显示
 * @return 返回VisibleCategoryActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeVisibleCategoryCustomizeData(const QString& categoryobjName, bool isShow)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数保持原签名与原语义
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeVisibleCategory(categoryobjName, isShow)));
}

/**
 * \if ENGLISH
 * @brief Create an AddQuickActionActionType SARibbonCustomizeData (add action to quick access bar)
 * @param key Key name managed by SARibbonActionsManager
 * @param mgr SARibbonActionsManager pointer
 * @return SARibbonCustomizeData with AddQuickActionActionType
 * \endif
 *
 * \if CHINESE
 * @brief 创建一个添加action到快速访问栏的SARibbonCustomizeData
 * @param key SARibbonActionsManager管理的key名
 * @param mgr SARibbonActionsManager指针
 * @return 返回AddQuickActionActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddQuickActionCustomizeData(const QString& key,
                                                                             SARibbonActionsManager* mgr)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数只补 manager 指针
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeAddQuickAction(key), mgr));
}

/**
 * \if ENGLISH
 * @brief Create a RemoveQuickActionActionType SARibbonCustomizeData (remove action from quick access bar)
 * @param key Key name managed by SARibbonActionsManager
 * @param mgr SARibbonActionsManager pointer
 * @return SARibbonCustomizeData with RemoveQuickActionActionType
 * \endif
 *
 * \if CHINESE
 * @brief 创建一个从快速访问栏移除action的SARibbonCustomizeData
 * @param key SARibbonActionsManager管理的key名
 * @param mgr SARibbonActionsManager指针
 * @return 返回RemoveQuickActionActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRemoveQuickActionCustomizeData(const QString& key,
                                                                                SARibbonActionsManager* mgr)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数只补 manager 指针
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeRemoveQuickAction(key), mgr));
}

/**
 * \if ENGLISH
 * @brief Create a ChangeQuickActionOrderActionType SARibbonCustomizeData (change action order in quick access bar)
 * @param key Key name managed by SARibbonActionsManager
 * @param mgr SARibbonActionsManager pointer
 * @param moveindex Move position, -1 means move left one position, 1 means move right one position
 * @return SARibbonCustomizeData with ChangeQuickActionOrderActionType
 * \endif
 *
 * \if CHINESE
 * @brief 创建一个改变快速访问栏action顺序的SARibbonCustomizeData
 * @param key SARibbonActionsManager管理的key名
 * @param mgr SARibbonActionsManager指针
 * @param moveindex 移动位置，-1代表向左移动一个位置，1代表向右移动一个位置
 * @return 返回ChangeQuickActionOrderActionType的SARibbonCustomizeData
 * \endif
 */
SARibbonCustomizeData SARibbonCustomizeData::makeChangeQuickActionOrderCustomizeData(const QString& key,
                                                                                     SARibbonActionsManager* mgr,
                                                                                     int moveindex)
{
    // 计划 04 WS-C1：记录构造下沉 core，本函数只补 manager 指针
    return (SARibbonCustomizeData(SARibbon::Core::SARibbonCustomizeRecord::makeChangeQuickActionOrder(key, moveindex), mgr));
}

/**
 * \if ENGLISH
 * @brief Check the external property whether customization is allowed
 * @param obj Object to check
 * @return true if customization is allowed
 * \endif
 *
 * \if CHINESE
 * @brief 判断外置属性，是否允许自定义
 * @param obj 要检查的对象
 * @return 如果允许自定义返回true
 * \endif
 */
bool SARibbonCustomizeData::isCanCustomize(QObject* obj)
{
    // 计划 04 WS-C1：与 QML 前端共用 core 的动态属性实现
    return (SARibbon::Core::isCanCustomize(obj));
}

/**
 * \if ENGLISH
 * @brief Set the external property to allow customization
 * @param obj Object to set
 * @param canbe Whether to allow customization
 * \endif
 *
 * \if CHINESE
 * @brief 设置外置属性允许自定义
 * @param obj 要设置的对象
 * @param canbe 是否允许自定义
 * \endif
 */
void SARibbonCustomizeData::setCanCustomize(QObject* obj, bool canbe)
{
    // 计划 04 WS-C1：与 QML 前端共用 core 的动态属性实现
    SARibbon::Core::setCanCustomize(obj, canbe);
}

QList< SARibbonCustomizeData > SARibbonCustomizeData::simplify(const QList< SARibbonCustomizeData >& csd)
{
    // plan-02 S4.2: algorithm moved verbatim to the core template (SARibbonCustomizeRecord::simplify)
    return SARibbon::Core::SARibbonCustomizeRecord::simplify(csd);
}
