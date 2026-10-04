#include "SARibbonCustomizeRecord.h"
#include <SARibbonCore/SARibbonEnums.h>
#include <QDebug>
#include <QObject>
#include <QVariant>

namespace SARibbon
{
namespace Core
{

/**
 * \if ENGLISH
 * @brief Warn about the empty object names that make customize addressing impossible
 * @details Every record locates its target through object names, so an empty one is
 *          always a caller mistake. The wording is the 2.x SARibbonCustomizeData one,
 *          kept verbatim while the factories moved here (plan 04 WS-C1).
 * \endif
 *
 * \if CHINESE
 * @brief 对使定制寻址失效的空对象名发出告警
 * @details 记录一律通过对象名定位目标，空名必然是调用方的疏漏。文案沿用 2.x
 *          SARibbonCustomizeData 的原文，随工厂一起搬到 core（计划 04 WS-C1）。
 * \endif
 */
static void sa_warn_empty_customize_name(const char* op, const char* what)
{
    qDebug() << QObject::tr("SARibbon Warning !!! customize %1,"
                            "but get an empty %2 object name,"
                            "if you want to customize SARibbon,"
                            "please make sure every element has been set object name.")
                    .arg(QLatin1String(op), QLatin1String(what));
}

SARibbonCustomizeRecord::SARibbonCustomizeRecord()
    : indexValue(-1)
    , actionRowProportionValue(SARibbonRowProportion::Large)
    , mType(UnknowActionType)
{
}

SARibbonCustomizeRecord::SARibbonCustomizeRecord(ActionType type)
    : indexValue(-1)
    , actionRowProportionValue(SARibbonRowProportion::Large)
    , mType(type)
{
}

SARibbonCustomizeRecord::~SARibbonCustomizeRecord()
{
}

SARibbonCustomizeRecord::ActionType SARibbonCustomizeRecord::actionType() const
{
    return (mType);
}

void SARibbonCustomizeRecord::setActionType(SARibbonCustomizeRecord::ActionType a)
{
    mType = a;
}

bool SARibbonCustomizeRecord::isValid() const
{
    return (actionType() != UnknowActionType);
}

/**
 * \if ENGLISH
 * @brief Make an AddCategoryActionType record
 * @param title Category title
 * @param index Position to insert the category at
 * @param objName Object name the new category will carry
 * @return Record of AddCategoryActionType
 * \endif
 *
 * \if CHINESE
 * @brief 生成 AddCategoryActionType 记录
 * @param title category 的标题
 * @param index category 要插入的位置
 * @param objName 新 category 的 object name
 * @return AddCategoryActionType 的记录
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeAddCategory(const QString& title, int index, const QString& objName)
{
    SARibbonCustomizeRecord d(AddCategoryActionType);

    d.indexValue           = index;
    d.keyValue             = title;
    d.categoryObjNameValue = objName;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make an AddPanelActionType record
 * @param title Panel title
 * @param index Position to insert the panel at
 * @param categoryObjName Object name of the owning category
 * @param objName Object name the new panel will carry
 * @return Record of AddPanelActionType
 * \endif
 *
 * \if CHINESE
 * @brief 生成 AddPanelActionType 记录
 * @param title panel 的标题
 * @param index panel 要插入的位置
 * @param categoryObjName 所属 category 的 object name
 * @param objName 新 panel 的 object name
 * @return AddPanelActionType 的记录
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeAddPanel(const QString& title,
                                                             int index,
                                                             const QString& categoryObjName,
                                                             const QString& objName)
{
    SARibbonCustomizeRecord d(AddPanelActionType);

    d.indexValue           = index;
    d.keyValue             = title;
    d.panelObjNameValue    = objName;
    d.categoryObjNameValue = categoryObjName;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make an AddActionActionType record
 * @param key Registry key of the item to add
 * @param rp Row proportion the item occupies
 * @param categoryObjName Object name of the owning category
 * @param panelObjName Object name of the owning panel
 * @return Record of AddActionActionType
 * \endif
 *
 * \if CHINESE
 * @brief 生成 AddActionActionType 记录
 * @param key 待添加项在注册表中的 key
 * @param rp 该项占据的行占比
 * @param categoryObjName 所属 category 的 object name
 * @param panelObjName 所属 panel 的 object name
 * @return AddActionActionType 的记录
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeAddAction(const QString& key,
                                                              SARibbonRowProportion rp,
                                                              const QString& categoryObjName,
                                                              const QString& panelObjName)
{
    SARibbonCustomizeRecord d(AddActionActionType);

    d.keyValue                 = key;
    d.categoryObjNameValue     = categoryObjName;
    d.panelObjNameValue        = panelObjName;
    d.actionRowProportionValue = rp;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make a RemoveCategoryActionType record
 * \endif
 *
 * \if CHINESE
 * @brief 生成 RemoveCategoryActionType 记录
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeRemoveCategory(const QString& categoryObjName)
{
    SARibbonCustomizeRecord d(RemoveCategoryActionType);

    if (categoryObjName.isEmpty()) {
        sa_warn_empty_customize_name("remove category", "category");
    }
    d.categoryObjNameValue = categoryObjName;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make a RemovePanelActionType record
 * \endif
 *
 * \if CHINESE
 * @brief 生成 RemovePanelActionType 记录
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeRemovePanel(const QString& categoryObjName,
                                                                const QString& panelObjName)
{
    SARibbonCustomizeRecord d(RemovePanelActionType);

    if (categoryObjName.isEmpty() || panelObjName.isEmpty()) {
        sa_warn_empty_customize_name("remove panel", "category/panel");
    }
    d.categoryObjNameValue = categoryObjName;
    d.panelObjNameValue    = panelObjName;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make a RemoveActionActionType record
 * \endif
 *
 * \if CHINESE
 * @brief 生成 RemoveActionActionType 记录
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeRemoveAction(const QString& categoryObjName,
                                                                 const QString& panelObjName,
                                                                 const QString& key)
{
    SARibbonCustomizeRecord d(RemoveActionActionType);

    if (categoryObjName.isEmpty() || panelObjName.isEmpty() || key.isEmpty()) {
        sa_warn_empty_customize_name("remove action", "category/panel/action");
    }
    d.categoryObjNameValue = categoryObjName;
    d.panelObjNameValue    = panelObjName;
    d.keyValue             = key;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make a ChangeCategoryOrderActionType record
 * @param moveIndex -1 moves one position left, 1 moves one position right
 * \endif
 *
 * \if CHINESE
 * @brief 生成 ChangeCategoryOrderActionType 记录
 * @param moveIndex -1 向左（上）移动一个位置，1 向右（下）移动一个位置
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeChangeCategoryOrder(const QString& categoryObjName, int moveIndex)
{
    SARibbonCustomizeRecord d(ChangeCategoryOrderActionType);

    if (categoryObjName.isEmpty()) {
        sa_warn_empty_customize_name("change category order", "category");
    }
    d.categoryObjNameValue = categoryObjName;
    d.indexValue           = moveIndex;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make a ChangePanelOrderActionType record
 * @param moveIndex -1 moves one position left, 1 moves one position right
 * \endif
 *
 * \if CHINESE
 * @brief 生成 ChangePanelOrderActionType 记录
 * @param moveIndex -1 向左（上）移动一个位置，1 向右（下）移动一个位置
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeChangePanelOrder(const QString& categoryObjName,
                                                                     const QString& panelObjName,
                                                                     int moveIndex)
{
    SARibbonCustomizeRecord d(ChangePanelOrderActionType);

    if (categoryObjName.isEmpty() || panelObjName.isEmpty()) {
        sa_warn_empty_customize_name("change panel order", "category/panel");
    }
    d.categoryObjNameValue = categoryObjName;
    d.panelObjNameValue    = panelObjName;
    d.indexValue           = moveIndex;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make a ChangeActionOrderActionType record
 * @param moveIndex -1 moves one position left, 1 moves one position right
 * \endif
 *
 * \if CHINESE
 * @brief 生成 ChangeActionOrderActionType 记录
 * @param moveIndex -1 向左（上）移动一个位置，1 向右（下）移动一个位置
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeChangeActionOrder(const QString& categoryObjName,
                                                                      const QString& panelObjName,
                                                                      const QString& key,
                                                                      int moveIndex)
{
    SARibbonCustomizeRecord d(ChangeActionOrderActionType);

    if (categoryObjName.isEmpty() || panelObjName.isEmpty() || key.isEmpty()) {
        sa_warn_empty_customize_name("change action order", "category/panel/action");
    }
    d.categoryObjNameValue = categoryObjName;
    d.panelObjNameValue    = panelObjName;
    d.keyValue             = key;
    d.indexValue           = moveIndex;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make a RenameCategoryActionType record
 * \endif
 *
 * \if CHINESE
 * @brief 生成 RenameCategoryActionType 记录
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeRenameCategory(const QString& newName,
                                                                   const QString& categoryObjName)
{
    SARibbonCustomizeRecord d(RenameCategoryActionType);

    if (categoryObjName.isEmpty()) {
        sa_warn_empty_customize_name("rename category", "category");
    }
    d.keyValue             = newName;
    d.categoryObjNameValue = categoryObjName;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make a RenamePanelActionType record
 * \endif
 *
 * \if CHINESE
 * @brief 生成 RenamePanelActionType 记录
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeRenamePanel(const QString& newName,
                                                                const QString& categoryObjName,
                                                                const QString& panelObjName)
{
    SARibbonCustomizeRecord d(RenamePanelActionType);

    if (categoryObjName.isEmpty() || panelObjName.isEmpty()) {
        sa_warn_empty_customize_name("rename panel", "category/panel");
    }
    d.keyValue             = newName;
    d.panelObjNameValue    = panelObjName;
    d.categoryObjNameValue = categoryObjName;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make a VisibleCategoryActionType record
 * @param isShow true shows the category, false hides it
 * \endif
 *
 * \if CHINESE
 * @brief 生成 VisibleCategoryActionType 记录
 * @param isShow true 显示该 category，false 隐藏
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeVisibleCategory(const QString& categoryObjName, bool isShow)
{
    SARibbonCustomizeRecord d(VisibleCategoryActionType);

    if (categoryObjName.isEmpty()) {
        sa_warn_empty_customize_name("visible category", "category");
    }
    d.categoryObjNameValue = categoryObjName;
    d.indexValue           = isShow ? 1 : 0;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make an AddQuickActionActionType record
 * @param index Insert position in the quick access bar, a negative value appends
 * \endif
 *
 * \if CHINESE
 * @brief 生成 AddQuickActionActionType 记录
 * @param index 在快速访问栏中的插入位置，负值表示追加到末尾
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeAddQuickAction(const QString& key, int index)
{
    SARibbonCustomizeRecord d(AddQuickActionActionType);

    d.keyValue   = key;
    d.indexValue = index;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make a RemoveQuickActionActionType record
 * \endif
 *
 * \if CHINESE
 * @brief 生成 RemoveQuickActionActionType 记录
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeRemoveQuickAction(const QString& key)
{
    SARibbonCustomizeRecord d(RemoveQuickActionActionType);

    d.keyValue = key;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Make a ChangeQuickActionOrderActionType record
 * @param moveIndex -1 moves one position left, 1 moves one position right
 * \endif
 *
 * \if CHINESE
 * @brief 生成 ChangeQuickActionOrderActionType 记录
 * @param moveIndex -1 向左移动一个位置，1 向右移动一个位置
 * \endif
 */
SARibbonCustomizeRecord SARibbonCustomizeRecord::makeChangeQuickActionOrder(const QString& key, int moveIndex)
{
    SARibbonCustomizeRecord d(ChangeQuickActionOrderActionType);

    d.keyValue   = key;
    d.indexValue = moveIndex;
    return (d);
}

/**
 * \if ENGLISH
 * @brief Read the dynamic property that marks an object customizable
 * @details Shared by both front ends: the widgets SARibbonCustomizeData forwards
 *          here and the QML hosts expose the same property (plan 04 WS-C1).
 *          The property is absent by default, and only "present and true" counts.
 * \endif
 *
 * \if CHINESE
 * @brief 读取标记对象可定制的动态属性
 * @details 两个前端共用：widgets 的 SARibbonCustomizeData 转发到这里，
 *          QML host 暴露同名属性（计划 04 WS-C1）。该属性默认不存在，
 *          只有"存在且为 true"才算可定制。
 * \endif
 */
bool isCanCustomize(QObject* obj)
{
    if (nullptr == obj) {
        return (false);
    }
    const QVariant v = obj->property(SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE);

    if (v.isValid()) {
        return (v.toBool());
    }
    return (false);
}

/**
 * \if ENGLISH
 * @brief Write the dynamic property that marks an object customizable
 * \endif
 *
 * \if CHINESE
 * @brief 写入标记对象可定制的动态属性
 * \endif
 */
void setCanCustomize(QObject* obj, bool canbe)
{
    if (nullptr == obj) {
        return;
    }
    obj->setProperty(SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE, canbe);
}

}
}
