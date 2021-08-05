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
//SARibbonActionsModel 特有
class SARibbonActionsModelPrivete;

/**
 * @brief 用于管理SARibbon的所有Action
 *
 * SARibbonActionsManager维护着两个表，一个是tag（标签）对应的Action list，
 * 一个是所有接受SARibbonActionsManager管理的action list。
 *
 * SARibbonActionsManager的标签对应一组actions，每个标签对应的action可以重复出现，
 * 但SARibbonActionsManager维护的action list里只有一份action，不会重复出现。
 *
 * tag用于对action list分组，每个tag的实体名字通过@ref setTagName 进行设置，在语言变化时需要及时调用
 * setTagName设置新的标签对应的文本。
 *
 * SARibbonActionsManager默认预设了6个常用标签见@ref SARibbonActionsManager::ActionTag ，用户自定义标签需要在
 * SARibbonActionsManager::UserDefineActionTag值的基础上进行累加。
 *
 * @ref filter （等同@ref actions ）函数用于提取标签管理的action list，@ref allActions 函数返回SARibbonActionsManager
 * 管理的所有标签。
 *
 * 通过@ref autoRegisteActions 函数可以快速的建立action的管理，此函数会遍历@ref SARibbonMainWindow 下的所有子object，
 * 同时遍历SARibbonMainWindow下所有@ref SARibbonPannel 添加的action,并给予Category建立tag，正常使用用户仅需关注此autoRegisteActions函数即可
 *
 *
 */
class SA_RIBBON_EXPORT SARibbonActionsManager : public QObject
{
    Q_OBJECT
    friend class SARibbonActionsManagerModel;
public:

    /**
     * @brief 定义action的标签
     */
    enum ActionTag {
        UnknowActionTag				= 0             ///< 未知的tag
        , CommonlyUsedActionTag			= 0x01          ///< 预设tag-常用命令
        , NotInFunctionalAreaActionTag		= 0x02          ///< 预设tag-不在功能区命令
        , AutoCategoryDistinguishBeginTag	= 0x1000        ///< 自动按Category划分的标签起始，在@ref autoRegisteActions 函数会用到
        , AutoCategoryDistinguishEndTag		= 0x2000        ///< 自动按Category划分的标签结束，在@ref autoRegisteActions 函数会用到
        , NotInRibbonCategoryTag		= 0x2001        ///< 不在功能区的标签@ref autoRegisteActions 函数会遍历所有category的action以及SARibbonMainWindow下的action，如果两个
        , UserDefineActionTag			= 0x8000        ///< 自定义标签，所有用户自定义tag要大于此tag
    };
    SARibbonActionsManager(SARibbonMainWindow *p);
    ~SARibbonActionsManager();
    //设置tag对应的名字
    void setTagName(int tag, const QString& name);

    //获取tag对应的名字
    QString tagName(int tag) const;

    //移除tag，注意，这个函数非常耗时
    void removeTag(int tag);

    //注册action
    bool registeAction(QAction *act, int tag, const QString& key = QString(), bool enableEmit = true);

    //取消action的注册
    void unregisteAction(QAction *act, bool enableEmit = true);

    //过滤得到actions对应的引用，实际是一个迭代器
    QList<QAction *>& filter(int tag);

    //通过tag筛选出系列action
    QList<QAction *>& actions(int tag);
    const QList<QAction *> actions(int tag) const;

    //获取所有的标签
    QList<int> actionTags() const;

    //通过key获取action
    QAction *action(const QString& key) const;

    //通过action找到key
    QString key(QAction *act) const;

    //返回所有管理的action数
    int count() const;

    //返回所有管理的actions
    QList<QAction *> allActions() const;

    //自动加载action,返回tag对应的Category指针
    QMap<int, SARibbonCategory *> autoRegisteActions(SARibbonMainWindow *w);

    //自动加载widget下的actions函数返回的action,返回加载的数量，这些
    QSet<QAction *> autoRegisteWidgetActions(QWidget *w, int tag, bool enableEmit = false);

    //根据标题查找action
    QList<QAction *> search(const QString& text);

    //清除
    void clear();

    //获取ribbonwindow
    SARibbonMainWindow *ribbonWindow() const;

signals:

    /**
     * @brief 标签变化触发的信号，变化包括新增和删除
     */
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
 * @brief SARibbonActionsManager 对应的model
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
