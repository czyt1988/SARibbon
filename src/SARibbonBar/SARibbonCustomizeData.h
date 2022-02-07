#ifndef SARIBBONCUSTOMIZEDATA_H
#define SARIBBONCUSTOMIZEDATA_H
#include "SARibbonGlobal.h"
#include "SARibbonActionsManager.h"
#include "SARibbonPannel.h"
#include <QList>

class SARibbonMainWindow;



class SA_RIBBON_EXPORT SARibbonCustomizeData {
public:
	enum ActionType {
		UnknowActionType = 0
		, AddCategoryActionType
		, AddPannelActionType
		, AddActionActionType
		, RemoveCategoryActionType
		, RemovePannelActionType
		, RemoveActionActionType
		, ChangeCategoryOrderActionType
		, ChangePannelOrderActionType
		, ChangeActionOrderActionType
		, RenameCategoryActionType
		, RenamePannelActionType
		, VisibleCategoryActionType
	};
	SARibbonCustomizeData();
	SARibbonCustomizeData(ActionType type, SARibbonActionsManager *mgr = nullptr);
	ActionType actionType() const;

	void setActionType(ActionType a);

	bool isValid() const;

	bool apply(SARibbonMainWindow *m) const;

	SARibbonActionsManager *actionManager();

	void setActionsManager(SARibbonActionsManager *mgr);

	static SARibbonCustomizeData makeAddCategoryCustomizeData(const QString& title
		, int index
		, const QString& objName);

	static SARibbonCustomizeData makeAddPannelCustomizeData(const QString& title
		, int index
		, const QString& categoryobjName
		, const QString& objName);

	static SARibbonCustomizeData makeAddActionCustomizeData(const QString& key
		, SARibbonActionsManager *mgr
		, SARibbonPannelItem::RowProportion rp
		, const QString& categoryObjName
		, const QString& pannelObjName
	);

	static SARibbonCustomizeData makeRenameCategoryCustomizeData(const QString& newname
		, const QString& categoryobjName);

	static SARibbonCustomizeData makeRenamePannelCustomizeData(const QString& newname
		, const QString& categoryobjName
		, const QString& pannelObjName);

	static SARibbonCustomizeData makeRemoveCategoryCustomizeData(const QString& categoryobjName);

	static SARibbonCustomizeData makeChangeCategoryOrderCustomizeData(const QString& categoryobjName
		, int moveindex);

	static SARibbonCustomizeData makeChangePannelOrderCustomizeData(const QString& categoryobjName
		, const QString& pannelObjName
		, int moveindex);

	static SARibbonCustomizeData makeChangeActionOrderCustomizeData(const QString& categoryobjName
		, const QString& pannelObjName
		, const QString& key
		, SARibbonActionsManager *mgr
		, int moveindex);

	static SARibbonCustomizeData makeRemovePannelCustomizeData(const QString& categoryobjName
		, const QString& pannelObjName);

	static SARibbonCustomizeData makeRemoveActionCustomizeData(const QString& categoryobjName
		, const QString& pannelObjName
		, const QString& key
		, SARibbonActionsManager *mgr);

	static SARibbonCustomizeData makeVisibleCategoryCustomizeData(const QString& categoryobjName, bool isShow);

	static bool isCanCustomize(QObject *obj);
	static void setCanCustomize(QObject *obj, bool canbe = true);

	static QList<SARibbonCustomizeData> simplify(const QList<SARibbonCustomizeData>& csd);

public:

	int indexValue;
	QString keyValue;

	QString categoryObjNameValue;

	QString pannelObjNameValue;

	SARibbonPannelItem::RowProportion actionRowProportionValue;
private:
	ActionType m_type;
	SARibbonActionsManager *m_actionsManagerPointer;
};
Q_DECLARE_METATYPE(SARibbonCustomizeData)

typedef QList<SARibbonCustomizeData> SARibbonCustomizeDataList;

#endif // SARIBBONCUSTOMIZEDATA_H
