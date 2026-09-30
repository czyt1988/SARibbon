#ifndef SARIBBONCUSTOMIZERECORD_H
#define SARIBBONCUSTOMIZERECORD_H
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <QString>
#include <QList>

namespace SARibbon
{
namespace Core
{

/**
 * \if ENGLISH
 * @brief Pure-data record base of SARibbonCustomizeData (plan 02 S4.2)
 * @details Holds the ActionType enum, the five public data fields, the private
 * type field and the pure functions isValid()/simplify() — everything from the
 * 2.x class that has no manager/widget dependency. The widgets-side
 * SARibbonCustomizeData publicly inherits this record (161 direct field accesses
 * keep compiling) and keeps the manager pointer, apply() and the make* factories.
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonCustomizeData 的纯数据记录基类（计划 02 S4.2）
 * @details 持有 ActionType 枚举、五个公有数据字段、私有类型字段与纯函数
 * isValid()/simplify()——2.x 类中无 manager/widget 依赖的全部内容。
 * widgets 侧 SARibbonCustomizeData 公有继承本记录（161 处直接字段访问
 * 保持可编译），manager 指针、apply() 与 make* 工厂留在派生类。
 * \endif
 */
class SA_RIBBON_CORE_EXPORT SARibbonCustomizeRecord
{
public:
	/**
	 * \if ENGLISH
	 * @brief Action type enumeration
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 操作类型枚举
	 * \endif
	 */
	enum ActionType
	{
		UnknowActionType = 0,           ///< 未知操作
		AddCategoryActionType,          ///< 添加category操作(1)
		AddPanelActionType,             ///< 添加panel操作(2)
		AddActionActionType,            ///< 添加action操作(3)
		RemoveCategoryActionType,       ///< 删除category操作(4)
		RemovePanelActionType,          ///< 删除panel操作(5)
		RemoveActionActionType,         ///< 删除action操作(6)
		ChangeCategoryOrderActionType,  ///< 改变category顺序的操作(7)
		ChangePanelOrderActionType,     ///< 改变panel顺序的操作(8)
		ChangeActionOrderActionType,    ///< 改变action顺序的操作(9)
		RenameCategoryActionType,       ///< 对category更名操作(10)
		RenamePanelActionType,          ///< 对Panel更名操作(11)
		VisibleCategoryActionType,      ///< 对category执行隐藏/显示操作(12)
		AddQuickActionActionType,       ///< 添加action到快速访问栏操作(13)
		RemoveQuickActionActionType,    ///< 从快速访问栏移除action操作(14)
		ChangeQuickActionOrderActionType  ///< 改变快速访问栏action顺序的操作(15)
	};
	// Default constructor
	SARibbonCustomizeRecord();
	// Constructor with action type
	explicit SARibbonCustomizeRecord(ActionType type);
	// Destructor
	virtual ~SARibbonCustomizeRecord();
	// Get the action type of the record
	ActionType actionType() const;

	// Set the action type
	void setActionType(ActionType a);

	// Check if this is a valid record
	bool isValid() const;

	// Simplify QList (pure algorithm; see the simplify template below the class)
	template< typename CustomizeDataT >
	static QList< CustomizeDataT > simplify(const QList< CustomizeDataT >& csd);

	/**
	 * \if ENGLISH
	 * @brief Parameter for recording order
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 记录顺序的参数
	 * \endif
	 */
	int indexValue;

	/**
	 * \if ENGLISH
	 * @brief Parameter for recording title, index, etc.
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 记录标题、索引等参数
	 * \endif
	 */
	QString keyValue;

	/**
	 * \if ENGLISH
	 * @brief Record categoryObjName for locating Category
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 记录categoryObjName，用于定位Category
	 * \endif
	 */
	QString categoryObjNameValue;

	/**
	 * \if CHINESE
	 * @brief 记录panelObjName，saribbon的Customize索引大部分基于objname
	 * \endif
	 */
	QString panelObjNameValue;

	SARibbonRowProportion actionRowProportionValue;  ///< 行的占比（原 SARibbonPanelItem::RowProportion，core 提升枚举）

private:
	ActionType mType;  ///< 标记这个data是category还是panel亦或是action
};

// Simplify QList (template: single core implementation, derived extras preserved)
template< typename CustomizeDataT >
QList< CustomizeDataT > remove_indexs(const QList< CustomizeDataT >& csd, const QList< int >& willremoveIndex)
{
    QList< CustomizeDataT > res;

    for (int i = 0; i < csd.size(); ++i) {
        if (!willremoveIndex.contains(i)) {
            res << csd[ i ];
        }
    }
    return (res);
}

template< typename CustomizeDataT >
QList< CustomizeDataT > SARibbonCustomizeRecord::simplify(const QList< CustomizeDataT >& csd)
{
    int size = csd.size();

    if (size <= 1) {
        return (csd);
    }
    QList< CustomizeDataT > res;
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
            if ((csd[ i - 1 ].keyValue == csd[ i ].keyValue) && (csd[ i - 1 ].panelObjNameValue == csd[ i ].panelObjNameValue)
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

}
}

#endif  // SARIBBONCUSTOMIZERECORD_H
