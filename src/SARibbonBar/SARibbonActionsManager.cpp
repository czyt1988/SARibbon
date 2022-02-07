#include "SARibbonActionsManager.h"
#include <QMap>
#include <QHash>
#include <QDebug>
#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"

class SARibbonActionsManagerPrivate
{
public:
	SARibbonActionsManagerPrivate(SARibbonActionsManager *p);
	void clear();

	SARibbonActionsManager *mParent;
	QMap<int, QList<QAction *> > mTagToActions;     ///< tag : QList<QAction*>
	QMap<int, QString> mTagToName;                  ///< tag
	QHash<QString, QAction *> mKeyToAction;         ///< keyaction
	QMap<QAction *, QString> mActionToKey;          ///< actionkey
	QMap<int, SARibbonCategory *> mTagToCategory;   ///<autoRegisteActions
	int mSale;                                      ///<id，key，idmsale, SARibbonActionsManager,registeAction，id
};

SARibbonActionsManagerPrivate::SARibbonActionsManagerPrivate(SARibbonActionsManager *p)
	: mParent(p)
	, mSale(0)
{
}


void SARibbonActionsManagerPrivate::clear()
{
	mTagToActions.clear();
	mTagToName.clear();
	mKeyToAction.clear();
	mActionToKey.clear();
	mTagToCategory.clear();
	mSale = 0;
}


SARibbonActionsManager::SARibbonActionsManager(SARibbonMainWindow *p) : QObject(p)
, m_d(new SARibbonActionsManagerPrivate(this))
{
	autoRegisteActions(p);
}


SARibbonActionsManager::~SARibbonActionsManager()
{
	delete m_d;
}


/**
 * @brief tag
 * @param tag
 * @param name
 * @note
 */
void SARibbonActionsManager::setTagName(int tag, const QString& name)
{
	m_d->mTagToName[tag] = name;
}


/**
 * @brief tag
 * @param tag
 * @return
 */
QString SARibbonActionsManager::tagName(int tag) const
{
	return (m_d->mTagToName.value(tag, ""));
}


/**
 * @brief tag
 * @note
 * @param tag
 */
void SARibbonActionsManager::removeTag(int tag)
{
	QList<QAction *> oldacts = actions(tag);


	m_d->mTagToActions.remove(tag);
	m_d->mTagToName.remove(tag);
	//action
	QList<QAction *> needRemoveAct;
	QList<QAction *> total;

	for (auto i = m_d->mTagToActions.begin(); i != m_d->mTagToActions.end(); ++i)
	{
		total += i.value();
	}
	for (QAction *a : oldacts)
	{
		if (!total.contains(a)) {
			needRemoveAct.append(a);
		}
	}
	//action
	for (QAction *a : needRemoveAct)
	{
		auto i = m_d->mActionToKey.find(a);
		if (i != m_d->mActionToKey.end()) {
			m_d->mKeyToAction.remove(i.value());
			m_d->mActionToKey.erase(i);
		}
	}
}



bool SARibbonActionsManager::registeAction(QAction *act, int tag, const QString& key, bool enableEmit)
{
	if (nullptr == act) {
		return (false);
	}
	QString k = key;

	if (k.isEmpty()) {
		k = QString("id_%1_%2").arg(m_d->mSale++).arg(act->objectName());
	}
	if (m_d->mKeyToAction.contains(k)) {
		qWarning() << "key: " << k << " have been exist,you can set key in an unique value when use SARibbonActionsManager::registeAction";
		return (false);
	}
	m_d->mKeyToAction[k] = act;
	m_d->mActionToKey[act] = k;
	//tag  action
	bool isneedemit = !(m_d->mTagToActions.contains(tag));

	m_d->mTagToActions[tag].append(act);

	connect(act, &QObject::destroyed, this, &SARibbonActionsManager::onActionDestroyed);
	if (isneedemit && enableEmit) {
		//tag
		emit actionTagChanged(tag, false);
	}
	return (true);
}


void SARibbonActionsManager::unregisteAction(QAction *act, bool enableEmit)
{
	if (nullptr == act) {
		return;
	}
	disconnect(act, &QObject::destroyed, this, &SARibbonActionsManager::onActionDestroyed);
	removeAction(act, enableEmit);
}


void SARibbonActionsManager::removeAction(QAction *act, bool enableEmit)
{
	QList<int> deletedTags;                         //tag，actionTagChanged
	QMap<int, QList<QAction *> > tagToActions;      ///< tag : QList<QAction*>

	for (auto i = m_d->mTagToActions.begin(); i != m_d->mTagToActions.end(); ++i)
	{
		//tagToActions;tagToActionKeys
		auto tmpi = tagToActions.insert(i.key(), QList<QAction *>());
		int count = 0;
		for (int j = 0; j < i.value().size(); ++j)
		{
			if (i.value()[j] != act) {
				tmpi.value().append(act);
				++count;
			}
		}
		if (0 == count) {
			//tag
			tagToActions.erase(tmpi);
			deletedTags.append(i.key());
		}
	}
	//mKeyToAction
	QString key = m_d->mActionToKey.value(act);

	m_d->mActionToKey.remove(act);
	m_d->mKeyToAction.remove(key);

	m_d->mTagToActions.swap(tagToActions);
	if (enableEmit) {
		for (int tagdelete : deletedTags)
		{
			emit actionTagChanged(tagdelete, true);
		}
	}
}


QList<QAction *>& SARibbonActionsManager::filter(int tag)
{
	return (actions(tag));
}


QList<QAction *>& SARibbonActionsManager::actions(int tag)
{
	return (m_d->mTagToActions[tag]);
}


const QList<QAction *> SARibbonActionsManager::actions(int tag) const
{
	return (m_d->mTagToActions[tag]);
}


QList<int> SARibbonActionsManager::actionTags() const
{
	return (m_d->mTagToActions.keys());
}


QAction *SARibbonActionsManager::action(const QString& key) const
{
	return (m_d->mKeyToAction.value(key, nullptr));
}


QString SARibbonActionsManager::key(QAction *act) const
{
	return (m_d->mActionToKey.value(act, QString()));
}


int SARibbonActionsManager::count() const
{
	return (m_d->mKeyToAction.size());
}

QList<QAction *> SARibbonActionsManager::allActions() const
{
	return (m_d->mKeyToAction.values());
}


QMap<int, SARibbonCategory *> SARibbonActionsManager::autoRegisteActions(SARibbonMainWindow *w)
{
	QMap<int, SARibbonCategory *> res;
	//SARibbonMainWindow，action
	QSet<QAction *> mainwindowActions;

	for (QObject *o : w->children())
	{
		if (QAction *a = qobject_cast<QAction *>(o)) {
			//action
			if (!a->objectName().isEmpty()) {
				mainwindowActions.insert(a);
			}
		}
	}
	//category，action
	SARibbonBar *bar = w->ribbonBar();

	if (nullptr == bar) {
		//ribbon，
		return (res);
	}
	QSet<QAction *> categoryActions;
	QList<SARibbonCategory *> categorys = bar->categoryPages();
	int tag = AutoCategoryDistinguishBeginTag;

	for (SARibbonCategory *c : categorys)
	{
		QList<SARibbonPannel *> pannels = c->pannelList();
		for (SARibbonPannel *p : pannels)
		{
			categoryActions += autoRegisteWidgetActions(p, tag, false);
		}
		setTagName(tag, c->windowTitle());
		res[tag] = c;
		++tag;
	}
	//actions
	QSet<QAction *> notincategory = mainwindowActions - categoryActions;

	for (QAction *a : notincategory)
	{
		if (!a->objectName().isEmpty()) {
			registeAction(a, NotInRibbonCategoryTag, a->objectName(), false);
		}
	}
	if (notincategory.size() > 0) {
		setTagName(NotInRibbonCategoryTag, tr("not in ribbon"));
	}
	for (auto i = res.begin(); i != res.end(); ++i)
	{
		connect(i.value(), &SARibbonCategory::windowTitleChanged
			, this, &SARibbonActionsManager::onCategoryTitleChanged);
	}
	m_d->mTagToCategory = res;
	return (res);
}


QSet<QAction *> SARibbonActionsManager::autoRegisteWidgetActions(QWidget *w, int tag, bool enableEmit)
{
	QSet<QAction *> res;
	QList<QAction *> was = w->actions();

	for (QAction *a : was)
	{
		if (res.contains(a) || a->objectName().isEmpty()) {
			continue;
		}
		if (registeAction(a, tag, a->objectName(), enableEmit)) {
			res.insert(a);
		}
	}
	return (res);
}


QList<QAction *> SARibbonActionsManager::search(const QString& text)
{
	QList<QAction *> res;

	if (text.isEmpty()) {
		return (res);
	}
	QStringList kws = text.split(" ");

	if (kws.isEmpty()) {
		kws.append(text);
	}
	QList<QAction *> acts = m_d->mActionToKey.keys();

	for (const QString& k : kws)
	{
		for (auto i = m_d->mActionToKey.begin(); i != m_d->mActionToKey.end(); ++i)
		{
			if (i.key()->text().contains(k, Qt::CaseInsensitive)) {
				res.append(i.key());
			}
		}
	}
	return (res);
}


void SARibbonActionsManager::clear()
{
	m_d->clear();
}


SARibbonMainWindow *SARibbonActionsManager::ribbonWindow() const
{
	return (qobject_cast<SARibbonMainWindow *>(parent()));
}


void SARibbonActionsManager::onActionDestroyed(QObject *o)
{
	QAction *act = static_cast<QAction *>(o);

	removeAction(act, false);
}


void SARibbonActionsManager::onCategoryTitleChanged(const QString& title)
{
	SARibbonCategory *c = qobject_cast<SARibbonCategory *>(sender());

	if (nullptr == c) {
		return;
	}
	int tag = m_d->mTagToCategory.key(c, -1);

	if (tag == -1) {
		return;
	}
	setTagName(tag, title);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SARibbonActionsModel
////////////////////////////////////////////////////////////////////////////////////////////////////////
class SARibbonActionsModelPrivete
{
public:
	SARibbonActionsManagerModel *mParent;
	SARibbonActionsManager *mMgr;
	int mTag;
	QString mSeatchText;
	QList<QAction *> mActions;
	SARibbonActionsModelPrivete(SARibbonActionsManagerModel *m);
	void updateRef();
	int count() const;
	QAction *at(int index);
	bool isNull() const;
};

SARibbonActionsModelPrivete::SARibbonActionsModelPrivete(SARibbonActionsManagerModel *m)
	: mParent(m)
	, mMgr(nullptr)
	, mTag(SARibbonActionsManager::CommonlyUsedActionTag)
	, mSeatchText(QString())
{
}


void SARibbonActionsModelPrivete::updateRef()
{
	if (isNull()) {
		return;
	}
	if (!mSeatchText.isEmpty()) {
		mActions = mMgr->search(mSeatchText);
	}
	else {
		mActions = mMgr->actions(mTag);
	}
}


int SARibbonActionsModelPrivete::count() const
{
	if (isNull()) {
		return (0);
	}
	return (mActions.size());
}


QAction *SARibbonActionsModelPrivete::at(int index)
{
	if (isNull()) {
		return (nullptr);
	}
	if (index >= count()) {
		return (nullptr);
	}
	return (mActions.at(index));
}


bool SARibbonActionsModelPrivete::isNull() const
{
	return (mMgr == nullptr);
}


SARibbonActionsManagerModel::SARibbonActionsManagerModel(QObject *p) : QAbstractListModel(p)
, m_d(new SARibbonActionsModelPrivete(this))
{
}


SARibbonActionsManagerModel::SARibbonActionsManagerModel(SARibbonActionsManager *m, QObject *p) : QAbstractListModel(p)
, m_d(new SARibbonActionsModelPrivete(this))
{
	setupActionsManager(m);
}


SARibbonActionsManagerModel::~SARibbonActionsManagerModel()
{
	delete m_d;
}


int SARibbonActionsManagerModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid()) {
		return (0);
	}
	return (m_d->count());
}


QVariant SARibbonActionsManagerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	if (role != Qt::DisplayRole) {
		return (QVariant());
	}
	if (Qt::Horizontal == orientation) {
		return (tr("action name"));
	}
	return (QVariant());
}


Qt::ItemFlags SARibbonActionsManagerModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return (Qt::NoItemFlags);
	}
	return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}


QVariant SARibbonActionsManagerModel::data(const QModelIndex& index, int role) const
{
	QAction *act = indexToAction(index);

	if (nullptr == act) {
		return (QVariant());
	}

	switch (role)
	{
	case Qt::DisplayRole:
		return (act->text());

	case Qt::DecorationRole:
		return (act->icon());

	default:
		break;
	}
	return (QVariant());
}


void SARibbonActionsManagerModel::setFilter(int tag)
{
	m_d->mTag = tag;
	update();
}


void SARibbonActionsManagerModel::update()
{
	beginResetModel();
	m_d->updateRef();
	endResetModel();
}


void SARibbonActionsManagerModel::setupActionsManager(SARibbonActionsManager *m)
{
	m_d->mMgr = m;
	m_d->mTag = SARibbonActionsManager::CommonlyUsedActionTag;
	m_d->mActions = m->filter(m_d->mTag);
	connect(m, &SARibbonActionsManager::actionTagChanged
		, this, &SARibbonActionsManagerModel::onActionTagChanged);
	update();
}


void SARibbonActionsManagerModel::uninstallActionsManager()
{
	if (!m_d->isNull()) {
		disconnect(m_d->mMgr, &SARibbonActionsManager::actionTagChanged
			, this, &SARibbonActionsManagerModel::onActionTagChanged);
		m_d->mMgr = nullptr;
		m_d->mTag = SARibbonActionsManager::CommonlyUsedActionTag;
	}
	update();
}


QAction *SARibbonActionsManagerModel::indexToAction(QModelIndex index) const
{
	if (!index.isValid()) {
		return (nullptr);
	}
	if (index.row() >= m_d->count()) {
		return (nullptr);
	}
	return (m_d->at(index.row()));
}


void SARibbonActionsManagerModel::search(const QString& text)
{
	m_d->mSeatchText = text;
	update();
}


void SARibbonActionsManagerModel::onActionTagChanged(int tag, bool isdelete)
{
	if (isdelete && (tag == m_d->mTag)) {
		m_d->mTag = SARibbonActionsManager::UnknowActionTag;
		update();
	}
	else {
		if (tag == m_d->mTag) {
			update();
		}
	}
}
