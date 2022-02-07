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


SARibbonCustomizeData::ActionType SARibbonCustomizeData::actionType() const
{
	return (m_type);
}


void SARibbonCustomizeData::setActionType(SARibbonCustomizeData::ActionType a)
{
	m_type = a;
}


bool SARibbonCustomizeData::isValid() const
{
	return (actionType() != UnknowActionType);
}


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
		int toindex = currentindex + indexValue;
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
		c->movePannel(pannelIndex, pannelIndex + indexValue);
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
		pannel->moveAction(actindex, actindex + indexValue);
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
		}
		else {
			bar->hideCategory(c);
		}
		return (true);
	}

	default:
		break;
	}
	return (false);
}


SARibbonActionsManager *SARibbonCustomizeData::actionManager()
{
	return (m_actionsManagerPointer);
}


void SARibbonCustomizeData::setActionsManager(SARibbonActionsManager *mgr)
{
	m_actionsManagerPointer = mgr;
}


SARibbonCustomizeData SARibbonCustomizeData::makeAddCategoryCustomizeData(const QString& title, int index, const QString& objName)
{
	SARibbonCustomizeData d(AddCategoryActionType);

	d.indexValue = index;
	d.keyValue = title;
	d.categoryObjNameValue = objName;
	return (d);
}


SARibbonCustomizeData SARibbonCustomizeData::makeAddPannelCustomizeData(const QString& title, int index, const QString& categoryobjName, const QString& objName)
{
	SARibbonCustomizeData d(AddPannelActionType);

	d.indexValue = index;
	d.keyValue = title;
	d.pannelObjNameValue = objName;
	d.categoryObjNameValue = categoryobjName;
	return (d);
}


SARibbonCustomizeData SARibbonCustomizeData::makeAddActionCustomizeData(const QString& key, SARibbonActionsManager *mgr, SARibbonPannelItem::RowProportion rp, const QString& categoryObjName, const QString& pannelObjName)
{
	SARibbonCustomizeData d(AddActionActionType, mgr);

	d.keyValue = key;
	d.categoryObjNameValue = categoryObjName;
	d.pannelObjNameValue = pannelObjName;
	d.actionRowProportionValue = rp;

	return (d);
}


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


bool SARibbonCustomizeData::isCanCustomize(QObject *obj)
{
	QVariant v = obj->property(SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE);

	if (v.isValid()) {
		return (v.toBool());
	}
	return (false);
}


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
			res << csd[i];
		}
	}
	return (res);
}


QList<SARibbonCustomizeData> SARibbonCustomizeData::simplify(const QList<SARibbonCustomizeData>& csd)
{
	int size = csd.size();

	if (size <= 1) {
		return (csd);
	}
	QList<SARibbonCustomizeData> res;
	QList<int> willremoveIndex;

	for (int i = 1; i < size; ++i)
	{
		if ((csd[i - 1].actionType() == AddCategoryActionType) &&
			(csd[i].actionType() == RemoveCategoryActionType)) {
			if (csd[i - 1].categoryObjNameValue == csd[i].categoryObjNameValue) {
				willremoveIndex << i - 1 << i;
			}
		}
		else if ((csd[i - 1].actionType() == AddPannelActionType) &&
			(csd[i].actionType() == RemovePannelActionType)) {
			if ((csd[i - 1].pannelObjNameValue == csd[i].pannelObjNameValue) &&
				(csd[i - 1].categoryObjNameValue == csd[i].categoryObjNameValue)) {
				willremoveIndex << i - 1 << i;
			}
		}
		else if ((csd[i - 1].actionType() == AddActionActionType) &&
			(csd[i].actionType() == RemoveActionActionType)) {
			if ((csd[i - 1].keyValue == csd[i].keyValue) &&
				(csd[i - 1].pannelObjNameValue == csd[i].pannelObjNameValue) &&
				(csd[i - 1].categoryObjNameValue == csd[i].categoryObjNameValue)) {
				willremoveIndex << i - 1 << i;
			}
		}
	}
	res = remove_indexs(csd, willremoveIndex);
	willremoveIndex.clear();

	size = res.size();
	for (int i = 1; i < size; ++i)
	{
		if ((res[i - 1].actionType() == VisibleCategoryActionType) &&
			(res[i].actionType() == VisibleCategoryActionType)) {
			if (res[i - 1].categoryObjNameValue == res[i].categoryObjNameValue) {
				willremoveIndex << i - 1;
			}
		}
	}
	res = remove_indexs(res, willremoveIndex);
	willremoveIndex.clear();

	size = res.size();
	for (int i = 0; i < size; ++i)
	{
		if (res[i].actionType() == RenameCategoryActionType) {
			for (int j = i + 1; j < size; ++j)
			{
				if ((res[j].actionType() == RenameCategoryActionType) &&
					(res[i].categoryObjNameValue == res[j].categoryObjNameValue)) {
					willremoveIndex << i;
				}
			}
		}
		else if (res[i].actionType() == RenamePannelActionType) {
			for (int j = i + 1; j < size; ++j)
			{
				if ((res[j].actionType() == RenamePannelActionType) &&
					(res[i].pannelObjNameValue == res[j].pannelObjNameValue) &&
					(res[i].categoryObjNameValue == res[j].categoryObjNameValue)) {
					willremoveIndex << i;
				}
			}
		}
	}
	res = remove_indexs(res, willremoveIndex);
	willremoveIndex.clear();

	size = res.size();
	for (int i = 1; i < size; ++i)
	{
		if ((res[i - 1].actionType() == ChangeCategoryOrderActionType) &&
			(res[i].actionType() == ChangeCategoryOrderActionType) &&
			(res[i - 1].categoryObjNameValue == res[i].categoryObjNameValue)) {
			res[i].indexValue += res[i - 1].indexValue;
			willremoveIndex << i - 1;
		}
		else if ((res[i - 1].actionType() == ChangePannelOrderActionType) &&
			(res[i].actionType() == ChangePannelOrderActionType) &&
			(res[i - 1].pannelObjNameValue == res[i].pannelObjNameValue) &&
			(res[i - 1].categoryObjNameValue == res[i].categoryObjNameValue)) {
			res[i].indexValue += res[i - 1].indexValue;
			willremoveIndex << i - 1;
		}
		else if ((res[i - 1].actionType() == ChangeActionOrderActionType) &&
			(res[i].actionType() == ChangeActionOrderActionType) &&
			(res[i - 1].keyValue == res[i].keyValue) &&
			(res[i - 1].pannelObjNameValue == res[i].pannelObjNameValue) &&
			(res[i - 1].categoryObjNameValue == res[i].categoryObjNameValue)) {
			res[i].indexValue += res[i - 1].indexValue;
			willremoveIndex << i - 1;
		}
	}
	res = remove_indexs(res, willremoveIndex);
	willremoveIndex.clear();

	size = res.size();
	for (int i = 0; i < size; ++i)
	{
		if ((res[i].actionType() == ChangeCategoryOrderActionType) ||
			(res[i].actionType() == ChangePannelOrderActionType) ||
			(res[i].actionType() == ChangeActionOrderActionType)) {
			if (0 == res[i].indexValue) {
				willremoveIndex << i;
			}
		}
	}
	res = remove_indexs(res, willremoveIndex);
	willremoveIndex.clear();
	return (res);
}
