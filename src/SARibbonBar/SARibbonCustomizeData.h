#ifndef SARIBBONCUSTOMIZEDATA_H
#define SARIBBONCUSTOMIZEDATA_H
#include "SARibbonGlobal.h"
#include "SARibbonActionsManager.h"
#include "SARibbonPanel.h"
#include <QList>
class SARibbonBar;
class SARibbonMainWindow;

/**
 * \if ENGLISH
 * @brief Data class for recording all customization operations
 * @note This data depends on @ref SARibbonActionsManager, use this class after SARibbonActionsManager
 * \endif
 *
 * \if CHINESE
 * @brief 记录所有自定义操作的数据类
 * @note 此数据依赖于@ref SARibbonActionsManager 要在SARibbonActionsManager之后使用此类
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCustomizeData
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
		VisibleCategoryActionType       ///< 对category执行隐藏/显示操作(12)
	};
	/// Default constructor
	SARibbonCustomizeData();
	/// Constructor with action type and manager
	SARibbonCustomizeData(ActionType type, SARibbonActionsManager* mgr = nullptr);
	/// Get the action type of the CustomizeData
	ActionType actionType() const;

	/// Set the action type of the CustomizeData
	void setActionType(ActionType a);

	/// Check if this is a valid CustomizeData
	bool isValid() const;

	/// Apply SARibbonCustomizeData to SARibbonBar
	bool apply(SARibbonBar* bar) const;

	/// Get the action manager pointer
	SARibbonActionsManager* actionManager();

	/// Set the ActionsManager
	void setActionsManager(SARibbonActionsManager* mgr);

	/// Create AddCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeAddCategoryCustomizeData(const QString& title, int index, const QString& objName);

	/// Create AddPanelActionType SARibbonCustomizeData
	static SARibbonCustomizeData
	makeAddPanelCustomizeData(const QString& title, int index, const QString& categoryobjName, const QString& objName);

	/// Create AddActionActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeAddActionCustomizeData(const QString& key,
															SARibbonActionsManager* mgr,
															SARibbonPanelItem::RowProportion rp,
															const QString& categoryObjName,
															const QString& panelObjName);

	/// Create RenameCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRenameCategoryCustomizeData(const QString& newname, const QString& categoryobjName);

	/// Create RenamePanelActionType SARibbonCustomizeData
	static SARibbonCustomizeData
	makeRenamePanelCustomizeData(const QString& newname, const QString& categoryobjName, const QString& panelObjName);

	/// Create RemoveCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRemoveCategoryCustomizeData(const QString& categoryobjName);

	/// Create ChangeCategoryOrderActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeChangeCategoryOrderCustomizeData(const QString& categoryobjName, int moveindex);

	/// Create ChangePanelOrderActionType SARibbonCustomizeData
	static SARibbonCustomizeData
	makeChangePanelOrderCustomizeData(const QString& categoryobjName, const QString& panelObjName, int moveindex);

	/// Create ChangeActionOrderActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeChangeActionOrderCustomizeData(const QString& categoryobjName,
																	const QString& panelObjName,
																	const QString& key,
																	SARibbonActionsManager* mgr,
																	int moveindex);

	/// Create RemovePanelActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRemovePanelCustomizeData(const QString& categoryobjName, const QString& panelObjName);

	/// Create RemoveActionActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRemoveActionCustomizeData(const QString& categoryobjName,
															   const QString& panelObjName,
															   const QString& key,
															   SARibbonActionsManager* mgr);

	/// Create VisibleCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeVisibleCategoryCustomizeData(const QString& categoryobjName, bool isShow);

	/// Check if customization is allowed for the object
	static bool isCanCustomize(QObject* obj);
	/// Set whether customization is allowed for the object
	static void setCanCustomize(QObject* obj, bool canbe = true);

	/// Simplify QList<SARibbonCustomizeData>
	static QList< SARibbonCustomizeData > simplify(const QList< SARibbonCustomizeData >& csd);

public:
	/**
	 * \if ENGLISH
	 * @brief Parameter for recording order
	 * @details When actionType==AddCategoryActionType, this parameter records the insert position of Category,
	 *          When actionType==AddPanelActionType, this parameter records the insert position of panel,
	 *          When actionType==AddActionActionType, this parameter records the insert position of action
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 记录顺序的参数
	 * @details 在actionType==AddCategoryActionType时，此参数记录Category的insert位置,
	 *          在actionType==AddPanelActionType时，此参数记录panel的insert位置,
	 *          在actionType==AddActionActionType时，此参数记录panel的insert位置
	 * \endif
	 */
	int indexValue;

	/**
	 * \if ENGLISH
	 * @brief Parameter for recording title, index, etc.
	 * @details When actionType==AddCategoryActionType, key is the category title,
	 *          When actionType==AddPanelActionType, key is the panel title,
	 *          When actionType==AddActionActionType, key is the action query basis, based on SARibbonActionsManager::action query
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 记录标题、索引等参数
	 * @details 在actionType==AddCategoryActionType时，key为category标题，
	 *          在actionType==AddPanelActionType时，key为panel标题，
	 *          在actionType==AddActionActionType时，key为action的查询依据，基于SARibbonActionsManager::action查询
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
	 * \if ENGLISH
	 * @brief Record panelObjName, SARibbon's Customize index is mostly based on objname
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 记录panelObjName，saribbon的Customize索引大部分基于objname
	 * \endif
	 */
	QString panelObjNameValue;

	SARibbonPanelItem::RowProportion actionRowProportionValue;  ///< 行的占比，ribbon中有large，media和small三种占比,见@ref RowProportion
private:
	ActionType mType;  ///< 标记这个data是category还是panel亦或是action
	SARibbonActionsManager* mActionsManagerPointer;
};
Q_DECLARE_METATYPE(SARibbonCustomizeData)

typedef QList< SARibbonCustomizeData > SARibbonCustomizeDataList;

#endif  // SARIBBONCUSTOMIZEDATA_H
