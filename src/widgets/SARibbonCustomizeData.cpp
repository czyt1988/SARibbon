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
        SARibbonCustomizeData::setCanCustomize(act);
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
        SARibbonCustomizeData::setCanCustomize(act);
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
    SARibbonCustomizeData d(AddCategoryActionType);

    d.indexValue           = index;
    d.keyValue             = title;
    d.categoryObjNameValue = objName;
    return (d);
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
    SARibbonCustomizeData d(AddPanelActionType);

    d.indexValue           = index;
    d.keyValue             = title;
    d.panelObjNameValue    = objName;
    d.categoryObjNameValue = categoryobjName;
    return (d);
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
    SARibbonCustomizeData d(AddActionActionType, mgr);

    d.keyValue                 = key;
    d.categoryObjNameValue     = categoryObjName;
    d.panelObjNameValue        = panelObjName;
    d.actionRowProportionValue = rp;

    return (d);
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
    SARibbonCustomizeData d(RenameCategoryActionType);

    if (categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize rename category,"
                                "but get an empty category object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.keyValue             = newname;
    d.categoryObjNameValue = categoryobjName;
    return (d);
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
    SARibbonCustomizeData d(RenamePanelActionType);

    if (panelObjName.isEmpty() || categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize rename panel,"
                                "but get an empty category/panel object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.keyValue             = newname;
    d.panelObjNameValue    = panelObjName;
    d.categoryObjNameValue = categoryobjName;
    return (d);
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
    SARibbonCustomizeData d(ChangeCategoryOrderActionType);

    if (categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize change category order,"
                                "but get an empty category object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.indexValue           = moveindex;
    return (d);
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
    SARibbonCustomizeData d(ChangePanelOrderActionType);

    if (categoryobjName.isEmpty() || panelObjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize change panel order,"
                                "but get an empty category/panel object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.panelObjNameValue    = panelObjName;
    d.indexValue           = moveindex;
    return (d);
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
    SARibbonCustomizeData d(ChangeActionOrderActionType, mgr);

    if (categoryobjName.isEmpty() || panelObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize change action order,"
                                "but get an empty category/panel/action object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.panelObjNameValue    = panelObjName;
    d.keyValue             = key;
    d.indexValue           = moveindex;
    return (d);
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
    SARibbonCustomizeData d(RemoveCategoryActionType);

    if (categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize remove category,"
                                "but get an empty category object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    return (d);
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
    SARibbonCustomizeData d(RemovePanelActionType);

    if (categoryobjName.isEmpty() || panelObjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize remove panel,"
                                "but get an empty category/panel object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.panelObjNameValue    = panelObjName;
    return (d);
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
    SARibbonCustomizeData d(RemoveActionActionType, mgr);

    if (categoryobjName.isEmpty() || panelObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize remove action,"
                                "but get an empty category/panel/action object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.panelObjNameValue    = panelObjName;
    d.keyValue             = key;
    return (d);
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
    SARibbonCustomizeData d(VisibleCategoryActionType);

    if (categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize visible category,"
                                "but get an empty category object name,"
                                "if you want to customize SARibbon,"
                                "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.indexValue           = isShow ? 1 : 0;
    return (d);
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
    SARibbonCustomizeData d(AddQuickActionActionType, mgr);

    d.keyValue   = key;
    d.indexValue = -1;  // 默认追加到末尾
    return (d);
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
    SARibbonCustomizeData d(RemoveQuickActionActionType, mgr);

    d.keyValue = key;
    return (d);
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
    SARibbonCustomizeData d(ChangeQuickActionOrderActionType, mgr);

    d.keyValue   = key;
    d.indexValue = moveindex;
    return (d);
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
    QVariant v = obj->property(SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE);

    if (v.isValid()) {
        return (v.toBool());
    }
    return (false);
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
    obj->setProperty(SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE, canbe);
}

QList< SARibbonCustomizeData > SARibbonCustomizeData::simplify(const QList< SARibbonCustomizeData >& csd)
{
    // plan-02 S4.2: algorithm moved verbatim to the core template (SARibbonCustomizeRecord::simplify)
    return SARibbon::Core::SARibbonCustomizeRecord::simplify(csd);
}
