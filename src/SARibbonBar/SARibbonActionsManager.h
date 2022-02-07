#ifndef SARIBBONACTIONSMANAGER_H
#define SARIBBONACTIONSMANAGER_H
#include "SARibbonGlobal.h"
#include <QObject>
#include <QAbstractListModel>
#include <QAction>
#include <QMap>
#include <QString>
#include <QSet>
class SARibbonMainWindow;
class SARibbonCategory;
class SARibbonActionsManagerPrivate;
//SARibbonActionsModel 
class SARibbonActionsModelPrivete;

class SA_RIBBON_EXPORT SARibbonActionsManager : public QObject
{
	Q_OBJECT
		friend class SARibbonActionsManagerModel;
public:
	enum ActionTag {
		UnknowActionTag = 0
		, CommonlyUsedActionTag = 0x01
		, NotInFunctionalAreaActionTag = 0x02
		, AutoCategoryDistinguishBeginTag = 0x1000  // autoRegisteActions 
		, AutoCategoryDistinguishEndTag = 0x2000  // autoRegisteActions 
		, NotInRibbonCategoryTag = 0x2001
		, UserDefineActionTag = 0x8000
	};
	SARibbonActionsManager(SARibbonMainWindow *p);
	~SARibbonActionsManager();
	void setTagName(int tag, const QString& name);

	QString tagName(int tag) const;

	void removeTag(int tag);

	bool registeAction(QAction *act, int tag, const QString& key = QString(), bool enableEmit = true);

	void unregisteAction(QAction *act, bool enableEmit = true);

	QList<QAction *>& filter(int tag);

	QList<QAction *>& actions(int tag);
	const QList<QAction *> actions(int tag) const;

	QList<int> actionTags() const;

	QAction *action(const QString& key) const;

	QString key(QAction *act) const;

	int count() const;

	QList<QAction *> allActions() const;

	QMap<int, SARibbonCategory *> autoRegisteActions(SARibbonMainWindow *w);

	QSet<QAction *> autoRegisteWidgetActions(QWidget *w, int tag, bool enableEmit = false);

	QList<QAction *> search(const QString& text);

	void clear();

	SARibbonMainWindow *ribbonWindow() const;

signals:

	void actionTagChanged(int tag, bool isdelete);

private slots:
	void onActionDestroyed(QObject *o);
	void onCategoryTitleChanged(const QString& title);

private:
	void removeAction(QAction *act, bool enableEmit = true);

private:
	SARibbonActionsManagerPrivate *m_d;
};



/**
 * @brief SARibbonActionsManagermodel
 */
class SA_RIBBON_EXPORT SARibbonActionsManagerModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit SARibbonActionsManagerModel(QObject *p = nullptr);
	explicit SARibbonActionsManagerModel(SARibbonActionsManager *m, QObject *p = nullptr);
	~SARibbonActionsManagerModel();
	virtual int rowCount(const QModelIndex& parent) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
	virtual QVariant data(const QModelIndex& index, int role) const override;
	void setFilter(int tag);
	void update();
	void setupActionsManager(SARibbonActionsManager *m);
	void uninstallActionsManager();
	QAction *indexToAction(QModelIndex index) const;
	void search(const QString& text);

private slots:
	void onActionTagChanged(int tag, bool isdelete);

private:
	SARibbonActionsModelPrivete *m_d;
};

#endif // SARIBBONACTIONSMANAGER_H
