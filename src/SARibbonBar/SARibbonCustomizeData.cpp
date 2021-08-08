#include "SARibbonCustomizeData.h"
#include "SARibbonBar.h"
#include "SARibbonMainWindow.h"
#include <QDebug>
#include <QObject>
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
        SARibbonCustomizeData::setCanCustomize(act);
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
        return (true);
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

    if (categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize rename category,"
            "but get an empty category object name,"
            "if you want to customize SARibbon,"
            "please make sure every element has been set object name.");
    }
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

    if (pannelObjName.isEmpty() || categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize rename pannel,"
            "but get an empty category/pannel object name,"
            "if you want to customize SARibbon,"
            "please make sure every element has been set object name.");
    }
    d.keyValue = newname;
    d.pannelObjNameValue = pannelObjName;
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

    if (categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize change category order,"
            "but get an empty category object name,"
            "if you want to customize SARibbon,"
            "please make sure every element has been set object name.");
    }
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

    if (categoryobjName.isEmpty() || pannelObjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize change pannel order,"
            "but get an empty category/pannel object name,"
            "if you want to customize SARibbon,"
            "please make sure every element has been set object name.");
    }
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

    if (categoryobjName.isEmpty() || pannelObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize change action order,"
            "but get an empty category/pannel/action object name,"
            "if you want to customize SARibbon,"
            "please make sure every element has been set object name.");
    }
    d.categoryObjNameValue = categoryobjName;
    d.pannelObjNameValue = pannelObjName;
    d.keyValue = key;
    d.indexValue = moveindex;
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
 * @brief 对应RemovePannelActionType
 * @param categoryobjName pannel对应的category的obj name
 * @param pannelObjName pannel对应的 obj name
 * @return
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRemovePannelCustomizeData(const QString& categoryobjName, const QString& pannelObjName)
{
    SARibbonCustomizeData d(RemovePannelActionType);

    if (categoryobjName.isEmpty() || pannelObjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize remove pannel,"
            "but get an empty category/pannel object name,"
            "if you want to customize SARibbon,"
            "please make sure every element has been set object name.");
    }
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

    if (categoryobjName.isEmpty() || pannelObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize remove action,"
            "but get an empty category/pannel/action object name,"
            "if you want to customize SARibbon,"
            "please make sure every element has been set object name.");
    }
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

    if (categoryobjName.isEmpty()) {
        qDebug() << QObject::tr("SARibbon Warning !!! customize visible category,"
            "but get an empty category object name,"
            "if you want to customize SARibbon,"
            "please make sure every element has been set object name.");
    }
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


QList<SARibbonCustomizeData> remove_indexs(const QList<SARibbonCustomizeData>& csd, const QList<int>& willremoveIndex);

QList<SARibbonCustomizeData> remove_indexs(const QList<SARibbonCustomizeData>& csd, const QList<int>& willremoveIndex)
{
    QList<SARibbonCustomizeData> res;

    for (int i = 0; i < csd.size(); ++i)
    {
        if (!willremoveIndex.contains(i)) {
            res<<csd[i];
        }
    }
    return (res);
}


/**
 * @brief 对QList<SARibbonCustomizeData>进行简化操作
 *
 * 此函数会执行如下操作：
 * 1、针对同一个category/pannel连续出现的添加和删除操作进行移除（前一步添加，后一步删除）
 *
 * 2、针对VisibleCategoryActionType，对于连续出现的操作只保留最后一步
 *
 * 3、针对RenameCategoryActionType和RenamePannelActionType操作，只保留最后一个
 *
 * 4、针对连续的ChangeCategoryOrderActionType，ChangePannelOrderActionType，ChangeActionOrderActionType进行合并为一个动作，
 * 如果合并后原地不动，则删除
 *
 * @param csd
 * @return 返回简化的QList<SARibbonCustomizeData>
 */
QList<SARibbonCustomizeData> SARibbonCustomizeData::simplify(const QList<SARibbonCustomizeData>& csd)
{
    int size = csd.size();

    if (size <= 1) {
        return (csd);
    }
    QList<SARibbonCustomizeData> res;
    QList<int> willremoveIndex;//记录要删除的index

    //! 首先针对连续出现的添加和删除操作进行优化
    for (int i = 1; i < size; ++i)
    {
        if ((csd[i-1].actionType() == AddCategoryActionType) &&
            (csd[i].actionType() == RemoveCategoryActionType)) {
            if (csd[i-1].categoryObjNameValue == csd[i].categoryObjNameValue) {
                willremoveIndex << i-1 << i;
            }
        }else if ((csd[i-1].actionType() == AddPannelActionType) &&
            (csd[i].actionType() == RemovePannelActionType)) {
            if ((csd[i-1].pannelObjNameValue == csd[i].pannelObjNameValue) &&
                (csd[i-1].categoryObjNameValue == csd[i].categoryObjNameValue)) {
                willremoveIndex << i-1 << i;
            }
        }else if ((csd[i-1].actionType() == AddActionActionType) &&
            (csd[i].actionType() == RemoveActionActionType)) {
            if ((csd[i-1].keyValue == csd[i].keyValue) &&
                (csd[i-1].pannelObjNameValue == csd[i].pannelObjNameValue) &&
                (csd[i-1].categoryObjNameValue == csd[i].categoryObjNameValue)) {
                willremoveIndex << i-1 << i;
            }
        }
    }
    res = remove_indexs(csd, willremoveIndex);
    willremoveIndex.clear();

    //! 筛选VisibleCategoryActionType，对于连续出现的操作只保留最后一步
    size = res.size();
    for (int i = 1; i < size; ++i)
    {
        if ((res[i-1].actionType() == VisibleCategoryActionType) &&
            (res[i].actionType() == VisibleCategoryActionType)) {
            if (res[i-1].categoryObjNameValue == res[i].categoryObjNameValue) {
                //要保证操作的是同一个内容
                willremoveIndex << i-1;//删除前一个只保留最后一个
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 针对RenameCategoryActionType和RenamePannelActionType操作，只需保留最后一个
    size = res.size();
    for (int i = 0; i < size; ++i)
    {
        if (res[i].actionType() == RenameCategoryActionType) {
            //向后查询，如果查询到有同一个Category改名，把这个索引加入删除队列
            for (int j = i+1; j < size; ++j)
            {
                if ((res[j].actionType() == RenameCategoryActionType) &&
                    (res[i].categoryObjNameValue == res[j].categoryObjNameValue)) {
                    willremoveIndex<<i;
                }
            }
        }else if (res[i].actionType() == RenamePannelActionType) {
            //向后查询，如果查询到有同一个pannel改名，把这个索引加入删除队列
            for (int j = i+1; j < size; ++j)
            {
                if ((res[j].actionType() == RenamePannelActionType) &&
                    (res[i].pannelObjNameValue == res[j].pannelObjNameValue) &&
                    (res[i].categoryObjNameValue == res[j].categoryObjNameValue)) {
                    willremoveIndex<<i;
                }
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 针对连续的ChangeCategoryOrderActionType，ChangePannelOrderActionType，ChangeActionOrderActionType进行合并
    size = res.size();
    for (int i = 1; i < size; ++i)
    {
        if ((res[i-1].actionType() == ChangeCategoryOrderActionType) &&
            (res[i].actionType() == ChangeCategoryOrderActionType) &&
            (res[i-1].categoryObjNameValue == res[i].categoryObjNameValue)) {
            //说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[i].indexValue += res[i-1].indexValue;
            willremoveIndex<<i-1;
        }else if ((res[i-1].actionType() == ChangePannelOrderActionType) &&
            (res[i].actionType() == ChangePannelOrderActionType) &&
            (res[i-1].pannelObjNameValue == res[i].pannelObjNameValue) &&
            (res[i-1].categoryObjNameValue == res[i].categoryObjNameValue)) {
            //说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[i].indexValue += res[i-1].indexValue;
            willremoveIndex<<i-1;
        }else if ((res[i-1].actionType() == ChangeActionOrderActionType) &&
            (res[i].actionType() == ChangeActionOrderActionType) &&
            (res[i-1].keyValue == res[i].keyValue) &&
            (res[i-1].pannelObjNameValue == res[i].pannelObjNameValue) &&
            (res[i-1].categoryObjNameValue == res[i].categoryObjNameValue)) {
            //说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[i].indexValue += res[i-1].indexValue;
            willremoveIndex<<i-1;
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();

    //! 上一步操作可能会产生indexvalue为0的情况，此操作把indexvalue为0的删除
    size = res.size();
    for (int i = 0; i < size; ++i)
    {
        if ((res[i].actionType() == ChangeCategoryOrderActionType) ||
            (res[i].actionType() == ChangePannelOrderActionType) ||
            (res[i].actionType() == ChangeActionOrderActionType)) {
            if (0 == res[i].indexValue) {
                willremoveIndex<<i;
            }
        }
    }
    res = remove_indexs(res, willremoveIndex);
    willremoveIndex.clear();
    return (res);
}
