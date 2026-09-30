#ifndef SARIBBONCUSTOMIZEDATA_H
#define SARIBBONCUSTOMIZEDATA_H
#include "SARibbonGlobal.h"
#include "SARibbonActionsManager.h"
#include "SARibbonPanel.h"
#include <SARibbonCore/SARibbonCustomizeRecord.h>
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
class SA_RIBBON_EXPORT SARibbonCustomizeData : public SARibbon::Core::SARibbonCustomizeRecord
{
public:
    // 计划 02 S4.2：ActionType 枚举、五个公有数据字段、mType、actionType()/setActionType()/isValid()
    // 与 simplify() 算法均下沉 core 的 SARibbonCustomizeRecord（公有继承，存量字段直接访问零改动）
	// Default constructor
	SARibbonCustomizeData();
	// Constructor with action type and manager
	SARibbonCustomizeData(ActionType type, SARibbonActionsManager* mgr = nullptr);
	// Apply SARibbonCustomizeData to SARibbonBar
	bool apply(SARibbonBar* bar) const;

	// Get the action manager pointer
	SARibbonActionsManager* actionManager();

	// Set the ActionsManager
	void setActionsManager(SARibbonActionsManager* mgr);

	// Create AddCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeAddCategoryCustomizeData(const QString& title, int index, const QString& objName);

	// Create AddPanelActionType SARibbonCustomizeData
	static SARibbonCustomizeData
	makeAddPanelCustomizeData(const QString& title, int index, const QString& categoryobjName, const QString& objName);

	// Create AddActionActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeAddActionCustomizeData(const QString& key,
															SARibbonActionsManager* mgr,
															SARibbonPanelItem::RowProportion rp,
															const QString& categoryObjName,
															const QString& panelObjName);

	// Create RenameCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRenameCategoryCustomizeData(const QString& newname, const QString& categoryobjName);

	// Create RenamePanelActionType SARibbonCustomizeData
	static SARibbonCustomizeData
	makeRenamePanelCustomizeData(const QString& newname, const QString& categoryobjName, const QString& panelObjName);

	// Create RemoveCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRemoveCategoryCustomizeData(const QString& categoryobjName);

	// Create ChangeCategoryOrderActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeChangeCategoryOrderCustomizeData(const QString& categoryobjName, int moveindex);

	// Create ChangePanelOrderActionType SARibbonCustomizeData
	static SARibbonCustomizeData
	makeChangePanelOrderCustomizeData(const QString& categoryobjName, const QString& panelObjName, int moveindex);

	// Create ChangeActionOrderActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeChangeActionOrderCustomizeData(const QString& categoryobjName,
																	const QString& panelObjName,
																	const QString& key,
																	SARibbonActionsManager* mgr,
																	int moveindex);

	// Create RemovePanelActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRemovePanelCustomizeData(const QString& categoryobjName, const QString& panelObjName);

	// Create RemoveActionActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRemoveActionCustomizeData(const QString& categoryobjName,
															   const QString& panelObjName,
															   const QString& key,
															   SARibbonActionsManager* mgr);

	// Create VisibleCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeVisibleCategoryCustomizeData(const QString& categoryobjName, bool isShow);

	// Create AddQuickActionActionType SARibbonCustomizeData (add action to quick access bar)
	static SARibbonCustomizeData makeAddQuickActionCustomizeData(const QString& key, SARibbonActionsManager* mgr);

	// Create RemoveQuickActionActionType SARibbonCustomizeData (remove action from quick access bar)
	static SARibbonCustomizeData makeRemoveQuickActionCustomizeData(const QString& key, SARibbonActionsManager* mgr);

	// Create ChangeQuickActionOrderActionType SARibbonCustomizeData (change action order in quick access bar)
	static SARibbonCustomizeData makeChangeQuickActionOrderCustomizeData(const QString& key,
																		SARibbonActionsManager* mgr,
																		int moveindex);

	// Check if customization is allowed for the object
	static bool isCanCustomize(QObject* obj);
	// Set whether customization is allowed for the object
	static void setCanCustomize(QObject* obj, bool canbe = true);

	// Simplify QList<SARibbonCustomizeData>
	static QList< SARibbonCustomizeData > simplify(const QList< SARibbonCustomizeData >& csd);

public:




private:
	SARibbonActionsManager* mActionsManagerPointer;
};
Q_DECLARE_METATYPE(SARibbonCustomizeData)

typedef QList< SARibbonCustomizeData > SARibbonCustomizeDataList;

#endif  // SARIBBONCUSTOMIZEDATA_H
