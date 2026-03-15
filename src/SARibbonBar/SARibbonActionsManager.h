#ifndef SARIBBONACTIONSMANAGER_H
#define SARIBBONACTIONSMANAGER_H
#include "SARibbonGlobal.h"
#include <QObject>
#include <QAbstractListModel>
#include <QAction>
#include <QMap>
#include <QString>
#include <QSet>
class SARibbonBar;
class SARibbonCategory;

/**
 * \if ENGLISH
 * @brief Manager for all SARibbon actions
 *
 * SARibbonActionsManager maintains two tables: one for tag-to-action list mapping,
 * and another for all actions under management.
 * \endif
 *
 * \if CHINESE
 * @brief SARibbon动作管理器
 *
 * SARibbonActionsManager维护两个表：一个用于标签到动作列表的映射，
 * 另一个用于管理下的所有动作。
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonActionsManager : public QObject
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonActionsManager)
	friend class SARibbonActionsManagerModel;

public:
	/// Action tag definition
	enum ActionTag
	{
		UnknowActionTag              = 0,     ///< Unknown tag
		CommonlyUsedActionTag        = 0x01,  ///< Preset tag - commonly used commands
		NotInFunctionalAreaActionTag = 0x02,  ///< Preset tag - commands not in functional area
		AutoCategoryDistinguishBeginTag = 0x1000,  ///< Auto category distinguish begin tag
		AutoCategoryDistinguishEndTag = 0x2000,  ///< Auto category distinguish end tag
		NotInRibbonCategoryTag = 0x2001,  ///< Tag for actions not in ribbon category
		UserDefineActionTag = 0x8000  ///< User defined tag, all custom tags should be greater than this
	};
	/// Constructor
	explicit SARibbonActionsManager(SARibbonBar* bar);
	/// Destructor
	~SARibbonActionsManager();
	/// Set tag name
	void setTagName(int tag, const QString& name);

	/// Get tag name
	QString tagName(int tag) const;

	/// Remove tag, note this function is time-consuming
	void removeTag(int tag);

	/// Register action
	bool registeAction(QAction* act, int tag, const QString& key = QString(), bool enableEmit = true);

	/// Unregister action
	void unregisteAction(QAction* act, bool enableEmit = true);

	/// Filter actions by tag, returns a reference
	QList< QAction* >& filter(int tag);

	/// Get actions by tag
	QList< QAction* >& actions(int tag);
	/// Get actions by tag (const version)
	const QList< QAction* > actions(int tag) const;

	/// Get all tags
	QList< int > actionTags() const;

	/// Get action by key
	QAction* action(const QString& key) const;

	/// Get key by action
	QString key(QAction* act) const;

	/// Get count of all managed actions
	int count() const;

	/// Get all managed actions
	QList< QAction* > allActions() const;

	/// Auto register actions from SARibbonBar, returns tag-to-category mapping
	QMap< int, SARibbonCategory* > autoRegisteActions(SARibbonBar* bar);

	/// Auto register widget actions
	QSet< QAction* > autoRegisteWidgetActions(QWidget* w, int tag, bool enableEmit = false);

	/// Search actions by text
	QList< QAction* > search(const QString& text);

	/// Clear all
	void clear();

Q_SIGNALS:

	/**
     * \if ENGLISH
     * @brief Signal emitted when action tag changed
     * @param tag Changed tag
     * @param isdelete Whether the tag is deleted (true) or added (false)
     * \endif
     *
     * \if CHINESE
     * @brief 当action标签发生变化时发射的信号
     * @param tag 发生变化的标签
     * @param isdelete 标签是否被删除（true表示删除，false表示添加）
     * \endif
     */
    void actionTagChanged(int tag, bool isdelete);

private Q_SLOTS:
	void onActionDestroyed(QObject* o);
	void onCategoryTitleChanged(const QString& title);

private:
	void removeAction(QAction* act, bool enableEmit = true);
};

/**
 * \if ENGLISH
 * @brief Model for SARibbonActionsManager
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonActionsManager的模型类
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonActionsManagerModel : public QAbstractListModel
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonActionsManagerModel)
public:
	/// Constructor
	explicit SARibbonActionsManagerModel(QObject* p = nullptr);
	/// Constructor with SARibbonActionsManager
	explicit SARibbonActionsManagerModel(SARibbonActionsManager* m, QObject* p = nullptr);
	/// Destructor
	~SARibbonActionsManagerModel();
	/// Get row count
	virtual int rowCount(const QModelIndex& parent) const override;
	/// Get header data
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	/// Get item flags
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
	/// Get data
	virtual QVariant data(const QModelIndex& index, int role) const override;
	/// Set filter tag
	void setFilter(int tag);
	/// Update model
	void update();
	/// Setup actions manager
	void setupActionsManager(SARibbonActionsManager* m);
	/// Uninstall actions manager
	void uninstallActionsManager();
	/// Get action from model index
	QAction* indexToAction(QModelIndex index) const;
	/// Search actions
	void search(const QString& text);

private Q_SLOTS:
	void onActionTagChanged(int tag, bool isdelete);
};

#endif  // SARIBBONACTIONSMANAGER_H
