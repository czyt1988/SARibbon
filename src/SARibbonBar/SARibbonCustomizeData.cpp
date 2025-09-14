#include "SARibbonCustomizeData.h"
#include "SARibbonBar.h"
#include <QDebug>
#include <QObject>
////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SARibbonCustomizeData
////////////////////////////////////////////////////////////////////////////////////////////////////////

SARibbonCustomizeData::SARibbonCustomizeData()
    : indexValue(-1)
    , actionRowProportionValue(SARibbonPanelItem::Large)
    , mType(UnknowActionType)
    , mActionsManagerPointer(nullptr)
{
}

SARibbonCustomizeData::SARibbonCustomizeData(ActionType type, SARibbonActionsManager* mgr)
    : indexValue(-1), actionRowProportionValue(SARibbonPanelItem::Large), mType(type), mActionsManagerPointer(mgr)
{
}

/**
 * @brief 获取CustomizeData的action type
 * @return
 */
SARibbonCustomizeData::ActionType SARibbonCustomizeData::actionType() const
{
    return (mType);
}

/**
 * @brief 设置CustomizeData的action type
 * @param a
 */
void SARibbonCustomizeData::setActionType(SARibbonCustomizeData::ActionType a)
{
    mType = a;
}

/**
 * @brief 判断是否是一个正常的CustomizeData
 *
 * 实际逻辑actionType() != UnknowActionType
 * @return 有用的CustomizeData返回true
 */
bool SARibbonCustomizeData::isValid() const
{
    return (actionType() != UnknowActionType);
}

/**
 * @brief 应用SARibbonCustomizeData到SARibbonBar
 * @param m
 * @return 如果应用失败，返回false,如果actionType==UnknowActionType直接返回false
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

    default:
        break;
    }
    return (false);
}

/**
 * @brief 获取actionmanager指针
 * @return
 */
SARibbonActionsManager* SARibbonCustomizeData::actionManager()
{
    return (mActionsManagerPointer);
}

/**
 * @brief 设置ActionsManager
 * @param mgr
 */
void SARibbonCustomizeData::setActionsManager(SARibbonActionsManager* mgr)
{
    mActionsManagerPointer = mgr;
}

/**
 * @brief 创建一个AddCategoryActionType的SARibbonCustomizeData
 * @param title category 的标题
 * @param index category要插入的位置
 * @param objName category的object name
 * @return 返回AddCategoryActionType的SARibbonCustomizeData
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
 * @brief 创建一个AddPanelActionType的SARibbonCustomizeData
 * @param title panel的标题
 * @param index panel的index
 * @param categoryobjName panel的category的objectname
 * @param objName panel的objname
 * @return 返回AddPanelActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddPanelCustomizeData(const QString& title,
                                                                        int index,
                                                                        const QString& categoryobjName,
                                                                        const QString& objName)
{
    SARibbonCustomizeData d(AddPanelActionType);

    d.indexValue           = index;
    d.keyValue             = title;
    d.panelObjNameValue   = objName;
    d.categoryObjNameValue = categoryobjName;
    return (d);
}

/**
 * @brief 添加action
 * @param key action的索引
 * @param mgr action管理器
 * @param rp 定义action的占位情况
 * @param categoryObjName action添加到的category的objname
 * @param panelObjName action添加到的category下的panel的objname
 * @param index action添加到的panel的索引
 * @return
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
    d.panelObjNameValue       = panelObjName;
    d.actionRowProportionValue = rp;

    return (d);
}

/**
 * @brief 创建一个RenameCategoryActionType的SARibbonCustomizeData
 * @param newname 新名字
 * @param index category的索引
 * @return 返回RenameCategoryActionType的SARibbonCustomizeData
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
 * @brief 创建一个RenamePanelActionType的SARibbonCustomizeData
 * @param newname panel的名字
 * @param indexValue panel的索引
 * @param categoryobjName panel对应的category的object name
 * @return 返回RenamePanelActionType的SARibbonCustomizeData
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
    d.panelObjNameValue   = panelObjName;
    d.categoryObjNameValue = categoryobjName;
    return (d);
}

/**
 * @brief 对应ChangeCategoryOrderActionType
 * @param categoryobjName 需要移动的categoryobjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
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
 * @brief 对应ChangePanelOrderActionType
 * @param categoryobjName 需要移动的panel对应的categoryobjName
 * @param panelObjName 需要移动的panelObjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
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
    d.panelObjNameValue   = panelObjName;
    d.indexValue           = moveindex;
    return (d);
}

/**
 * @brief 对应ChangeActionOrderActionType
 * @param categoryobjName 需要移动的panel对应的categoryobjName
 * @param panelObjName 需要移动的panelObjName
 * @param key SARibbonActionsManager管理的key名
 * @param mgr SARibbonActionsManager指针
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
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
    d.panelObjNameValue   = panelObjName;
    d.keyValue             = key;
    d.indexValue           = moveindex;
    return (d);
}

/**
 * @brief 对应RemoveCategoryActionType
 * @param categoryobjName 需要移除的objname
 * @return
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
 * @brief 对应RemovePanelActionType
 * @param categoryobjName panel对应的category的obj name
 * @param panelObjName panel对应的 obj name
 * @return
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
    d.panelObjNameValue   = panelObjName;
    return (d);
}

/**
 * @brief 对应RemoveActionActionType
 * @param categoryobjName panel对应的category的obj name
 * @param panelObjName panel对应的 obj name
 * @param key SARibbonActionsManager管理的key名
 * @param mgr SARibbonActionsManager指针
 * @return
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
    d.panelObjNameValue   = panelObjName;
    d.keyValue             = key;
    return (d);
}

/**
 * @brief SARibbonCustomizeData::makeVisibleCategoryCustomizeData
 * @param categoryobjName
 * @param isShow
 * @return
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
 * @brief 判断外置属性，是否允许自定义
 * @param obj
 * @return
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
 * @brief 设置外置属性允许自定义
 * @param obj
 * @param canbe
 */
void SARibbonCustomizeData::setCanCustomize(QObject* obj, bool canbe)
{
    obj->setProperty(SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE, canbe);
}

QList< SARibbonCustomizeData > remove_indexs(const QList< SARibbonCustomizeData >& csd, const QList< int >& willremoveIndex);

QList< SARibbonCustomizeData > remove_indexs(const QList< SARibbonCustomizeData >& csd, const QList< int >& willremoveIndex)
{
    QList< SARibbonCustomizeData > res;

    for (int i = 0; i < csd.size(); ++i) {
        if (!willremoveIndex.contains(i)) {
            res << csd[ i ];
        }
    }
    return (res);
}

/**
 * @brief 对QList<SARibbonCustomizeData>进行简化操作
 *
 * 此函数会执行如下操作：
 * 1、针对同一个category/panel连续出现的添加和删除操作进行移除（前一步添加，后一步删除）
 *
 * 2、针对VisibleCategoryActionType，对于连续出现的操作只保留最后一步
 *
 * 3、针对RenameCategoryActionType和RenamePanelActionType操作，只保留最后一个
 *
 * 4、针对连续的ChangeCategoryOrderActionType，ChangePanelOrderActionType，ChangeActionOrderActionType进行合并为一个动作，
 * 如果合并后原地不动，则删除
 *
 * @param csd
 * @return 返回简化的QList<SARibbonCustomizeData>
 */
QList< SARibbonCustomizeData > SARibbonCustomizeData::simplify(const QList< SARibbonCustomizeData >& csd)
{
    int size = csd.size();

    if (size <= 1) {
        return (csd);
    }
    QList< SARibbonCustomizeData > res;
    QList< int > willremoveIndex;  // 记录要删除的index

    //! 首先针对连续出现的添加和删除操作进行优化
    for (int i = 1; i < size; ++i) {
        if ((csd[ i - 1 ].actionType() == AddCategoryActionType) && (csd[ i ].actionType() == RemoveCategoryActionType)) {
            if (csd[ i - 1 ].categoryObjNameValue == csd[ i ].categoryObjNameValue) {
                willremoveIndex << i - 1 << i;
            }
        } else if ((csd[ i - 1 ].actionType() == AddPanelActionType) && (csd[ i ].actionType() == RemovePanelActionType)) {
            if ((csd[ i - 1 ].panelObjNameValue == csd[ i ].panelObjNameValue)
                && (csd[ i - 1 ].categoryObjNameValue == csd[ i ].categoryObjNameValue)) {
                willremoveIndex << i - 1 << i;
            }
        } else if ((csd[ i - 1 ].actionType() == AddActionActionType) && (csd[ i ].actionType() == RemoveActionActionType)) {
            if ((csd[ i - 1 ].keyValue == csd[ i ].keyValue)
                && (csd[ i - 1 ].panelObjNameValue == csd[ i ].panelObjNameValue)
                && (csd[ i - 1 ].categoryObjNameValue == csd[ i ].categoryObjNameValue)) {
                willremoveIndex << i - 1 << i;
            }
        }
    }
    res = remove_indexs(csd, willremoveIndex);
    willremoveIndex.clear();

    //! 筛选VisibleCategoryActionType，对于连续出现的操作只保留最后一步
    size = res.size();
    for (int i = 1; i < size; ++i) {
        if ((res[ i - 1 ].actionType() == VisibleCategoryActionType)
            && (res[ i ].actionType() == VisibleCategoryActionType)) {
            if (res[ i - 1 ].categoryObjNameValue == res[ i ].categoryObjNameValue) {
                // 要保证操作的是同一个内容
                willremoveIndex << i - 1;  // 删除前一个只保留最后一个
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 针对RenameCategoryActionType和RenamePanelActionType操作，只需保留最后一个
    size = res.size();
    for (int i = 0; i < size; ++i) {
        if (res[ i ].actionType() == RenameCategoryActionType) {
            // 向后查询，如果查询到有同一个Category改名，把这个索引加入删除队列
            for (int j = i + 1; j < size; ++j) {
                if ((res[ j ].actionType() == RenameCategoryActionType)
                    && (res[ i ].categoryObjNameValue == res[ j ].categoryObjNameValue)) {
                    willremoveIndex << i;
                }
            }
        } else if (res[ i ].actionType() == RenamePanelActionType) {
            // 向后查询，如果查询到有同一个panel改名，把这个索引加入删除队列
            for (int j = i + 1; j < size; ++j) {
                if ((res[ j ].actionType() == RenamePanelActionType)
                    && (res[ i ].panelObjNameValue == res[ j ].panelObjNameValue)
                    && (res[ i ].categoryObjNameValue == res[ j ].categoryObjNameValue)) {
                    willremoveIndex << i;
                }
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 针对连续的ChangeCategoryOrderActionType，ChangePanelOrderActionType，ChangeActionOrderActionType进行合并
    size = res.size();
    for (int i = 1; i < size; ++i) {
        if ((res[ i - 1 ].actionType() == ChangeCategoryOrderActionType)
            && (res[ i ].actionType() == ChangeCategoryOrderActionType)
            && (res[ i - 1 ].categoryObjNameValue == res[ i ].categoryObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[ i ].indexValue += res[ i - 1 ].indexValue;
            willremoveIndex << i - 1;
        } else if ((res[ i - 1 ].actionType() == ChangePanelOrderActionType)
                   && (res[ i ].actionType() == ChangePanelOrderActionType)
                   && (res[ i - 1 ].panelObjNameValue == res[ i ].panelObjNameValue)
                   && (res[ i - 1 ].categoryObjNameValue == res[ i ].categoryObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[ i ].indexValue += res[ i - 1 ].indexValue;
            willremoveIndex << i - 1;
        } else if ((res[ i - 1 ].actionType() == ChangeActionOrderActionType)
                   && (res[ i ].actionType() == ChangeActionOrderActionType) && (res[ i - 1 ].keyValue == res[ i ].keyValue)
                   && (res[ i - 1 ].panelObjNameValue == res[ i ].panelObjNameValue)
                   && (res[ i - 1 ].categoryObjNameValue == res[ i ].categoryObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[ i ].indexValue += res[ i - 1 ].indexValue;
            willremoveIndex << i - 1;
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 上一步操作可能会产生indexvalue为0的情况，此操作把indexvalue为0的删除
    size = res.size();
    for (int i = 0; i < size; ++i) {
        if ((res[ i ].actionType() == ChangeCategoryOrderActionType) || (res[ i ].actionType() == ChangePanelOrderActionType)
            || (res[ i ].actionType() == ChangeActionOrderActionType)) {
            if (0 == res[ i ].indexValue) {
                willremoveIndex << i;
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();
    return (res);
}
