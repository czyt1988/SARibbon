#include "SARibbonCustomizeData.h"
#include "SARibbonBar.h"
#include "SARibbonMainWindow.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SARibbonCustomizeData
////////////////////////////////////////////////////////////////////////////////////////////////////////


SARibbonCustomizeData::SARibbonCustomizeData()
    : indexValue(-1)
    , actionRowProportionValue(SARibbonPannelItem::Large)
    , m_type(UnknowActionType)
    , m_actionsManagerPointer(nullptr)
{
}


SARibbonCustomizeData::SARibbonCustomizeData(ActionType type, SARibbonActionsManager *mgr)
    : indexValue(-1)
    , actionRowProportionValue(SARibbonPannelItem::Large)
    , m_type(type)
    , m_actionsManagerPointer(mgr)
{
}


/**
 * @brief 获取CustomizeData的action type
 * @return
 */
SARibbonCustomizeData::ActionType SARibbonCustomizeData::actionType() const
{
    return (m_type);
}


/**
 * @brief 设置CustomizeData的action type
 * @param a
 */
void SARibbonCustomizeData::setActionType(SARibbonCustomizeData::ActionType a)
{
    m_type = a;
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
 * @brief 应用SARibbonCustomizeData到SARibbonMainWindow
 * @param m
 * @return 如果应用失败，返回false,如果actionType==UnknowActionType直接返回false
 */
bool SARibbonCustomizeData::apply(SARibbonMainWindow *m) const
{
    SARibbonBar *bar = m->ribbonBar();

    if (nullptr == bar) {
        return (false);
    }
    switch (actionType())
    {
    case UnknowActionType:
        return (false);

    case AddCategoryActionType:
    {
        //添加标签
        SARibbonCategory *c = bar->insertCategoryPage(keyValue, indexValue);
        if (nullptr == c) {
            return (false);
        }
        c->setObjectName(categoryObjNameValue);
        SARibbonCustomizeData::setCanCustomize(c);
        return (true);
    }

    case AddPannelActionType:
    {
        //添加pannel
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel *p = c->insertPannel(keyValue, indexValue);
        p->setObjectName(pannelObjNameValue);
        SARibbonCustomizeData::setCanCustomize(p);
        return (true);
    }

    case AddActionActionType:
    {
        if (nullptr == m_actionsManagerPointer) {
            return (false);
        }
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel *pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        QAction *act = m_actionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        pannel->addAction(act, actionRowProportionValue);
        return (true);
    }

    case RemoveCategoryActionType:
    {
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        bar->removeCategory(c);
        return (true);
    }

    case RemovePannelActionType:
    {
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel *pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        c->removePannel(pannel);
        return (true);
    }

    case RemoveActionActionType:
    {
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel *pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        QAction *act = m_actionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        pannel->removeAction(act);
    }

    case ChangeCategoryOrderActionType:
    {
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        int currentindex = bar->categoryIndex(c);
        if (-1 == currentindex) {
            return (false);
        }
        int toindex = currentindex+indexValue;
        bar->moveCategory(currentindex, toindex);
        return (true);
    }

    case ChangePannelOrderActionType:
    {
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel *pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        int pannelIndex = c->pannelIndex(pannel);
        if (-1 == pannelIndex) {
            return (false);
        }
        c->movePannel(pannelIndex, pannelIndex+indexValue);
        return (true);
    }

    case ChangeActionOrderActionType:
    {
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel *pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        QAction *act = m_actionsManagerPointer->action(keyValue);
        if (nullptr == act) {
            return (false);
        }
        int actindex = pannel->actionIndex(act);
        if (actindex <= -1) {
            return (false);
        }
        pannel->moveAction(actindex, actindex+indexValue);
        return (true);
    }

    case RenameCategoryActionType:
    {
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        c->setWindowTitle(keyValue);
        return (true);
    }

    case RenamePannelActionType:
    {
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel *pannel = c->pannelByObjectName(pannelObjNameValue);
        if (nullptr == pannel) {
            return (false);
        }
        pannel->setWindowTitle(keyValue);
        return (true);
    }

    case VisibleCategoryActionType:
    {
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        if (1 == indexValue) {
            bar->showCategory(c);
        }else{
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
SARibbonActionsManager *SARibbonCustomizeData::actionManager()
{
    return (m_actionsManagerPointer);
}


/**
 * @brief 设置ActionsManager
 * @param mgr
 */
void SARibbonCustomizeData::setActionsManager(SARibbonActionsManager *mgr)
{
    m_actionsManagerPointer = mgr;
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

    d.indexValue = index;
    d.keyValue = title;
    d.categoryObjNameValue = objName;
    return (d);
}


/**
 * @brief 创建一个AddPannelActionType的SARibbonCustomizeData
 * @param title pannel的标题
 * @param index pannel的index
 * @param categoryobjName pannel的category的objectname
 * @param objName pannel的objname
 * @return 返回AddPannelActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddPannelCustomizeData(const QString& title, int index, const QString& categoryobjName, const QString& objName)
{
    SARibbonCustomizeData d(AddPannelActionType);

    d.indexValue = index;
    d.keyValue = title;
    d.pannelObjNameValue = objName;
    d.categoryObjNameValue = categoryobjName;
    return (d);
}


/**
 * @brief 添加action
 * @param key action的索引
 * @param mgr action管理器
 * @param rp 定义action的占位情况
 * @param categoryObjName action添加到的category的objname
 * @param pannelObjName action添加到的category下的pannel的objname
 * @param index action添加到的pannel的索引
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddActionCustomizeData(const QString& key, SARibbonActionsManager *mgr, SARibbonPannelItem::RowProportion rp, const QString& categoryObjName, const QString& pannelObjName)
{
    SARibbonCustomizeData d(AddActionActionType, mgr);

    d.keyValue = key;
    d.categoryObjNameValue = categoryObjName;
    d.pannelObjNameValue = pannelObjName;
    d.actionRowProportionValue = rp;

    return (d);
}


/**
 * @brief 创建一个RenameCategoryActionType的SARibbonCustomizeData
 * @param newname 新名字
 * @param index category的索引
 * @return 返回RenameCategoryActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRenameCategoryCustomizeData(const QString& newname, const QString& categoryobjName)
{
    SARibbonCustomizeData d(RenameCategoryActionType);

    d.keyValue = newname;
    d.categoryObjNameValue = categoryobjName;
    return (d);
}


/**
 * @brief 创建一个RenamePannelActionType的SARibbonCustomizeData
 * @param newname pannel的名字
 * @param indexValue pannel的索引
 * @param categoryobjName pannel对应的category的object name
 * @return 返回RenamePannelActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRenamePannelCustomizeData(const QString& newname, const QString& categoryobjName, const QString& pannelObjName)
{
    SARibbonCustomizeData d(RenamePannelActionType);

    d.keyValue = newname;
    d.pannelObjNameValue = pannelObjName;
    d.categoryObjNameValue = categoryobjName;
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

    d.categoryObjNameValue = categoryobjName;
    return (d);
}


/**
 * @brief 对应ChangeCategoryOrderActionType
 * @param categoryobjName 需要移动的categoryobjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(const QString& categoryobjName, int moveindex)
{
    SARibbonCustomizeData d(ChangeCategoryOrderActionType);

    d.categoryObjNameValue = categoryobjName;
    d.indexValue = moveindex;
    return (d);
}


/**
 * @brief 对应ChangePannelOrderActionType
 * @param categoryobjName 需要移动的pannel对应的categoryobjName
 * @param pannelObjName 需要移动的pannelObjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeChangePannelOrderCustomizeData(const QString& categoryobjName, const QString& pannelObjName, int moveindex)
{
    SARibbonCustomizeData d(ChangePannelOrderActionType);

    d.categoryObjNameValue = categoryobjName;
    d.pannelObjNameValue = pannelObjName;
    d.indexValue = moveindex;
    return (d);
}


/**
 * @brief 对应ChangeActionOrderActionType
 * @param categoryobjName 需要移动的pannel对应的categoryobjName
 * @param pannelObjName 需要移动的pannelObjName
 * @param key SARibbonActionsManager管理的key名
 * @param mgr SARibbonActionsManager指针
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeChangeActionOrderCustomizeData(const QString& categoryobjName, const QString& pannelObjName, const QString& key, SARibbonActionsManager *mgr, int moveindex)
{
    SARibbonCustomizeData d(ChangeActionOrderActionType, mgr);

    d.categoryObjNameValue = categoryobjName;
    d.pannelObjNameValue = pannelObjName;
    d.keyValue = key;
    d.indexValue = moveindex;
    return (d);
}


/**
 * @brief 对应RemovePannelActionType
 * @param categoryobjName pannel对应的category的obj name
 * @param pannelObjName pannel对应的 obj name
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRemovePannelCustomizeData(const QString& categoryobjName, const QString& pannelObjName)
{
    SARibbonCustomizeData d(RemovePannelActionType);

    d.categoryObjNameValue = categoryobjName;
    d.pannelObjNameValue = pannelObjName;
    return (d);
}


/**
 * @brief 对应RemoveActionActionType
 * @param categoryobjName pannel对应的category的obj name
 * @param pannelObjName pannel对应的 obj name
 * @param key SARibbonActionsManager管理的key名
 * @param mgr SARibbonActionsManager指针
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRemoveActionCustomizeData(const QString& categoryobjName, const QString& pannelObjName, const QString& key, SARibbonActionsManager *mgr)
{
    SARibbonCustomizeData d(RemoveActionActionType, mgr);

    d.categoryObjNameValue = categoryobjName;
    d.pannelObjNameValue = pannelObjName;
    d.keyValue = key;
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

    d.categoryObjNameValue = categoryobjName;
    d.indexValue = isShow ? 1 : 0;
    return (d);
}


/**
 * @brief 判断外置属性，是否允许自定义
 * @param obj
 * @return
 */
bool SARibbonCustomizeData::isCanCustomize(QObject *obj)
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
void SARibbonCustomizeData::setCanCustomize(QObject *obj, bool canbe)
{
    obj->setProperty(SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE, canbe);
}
