#ifndef SARIBBONCUSTOMIZEWIDGET_H
#define SARIBBONCUSTOMIZEWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QAbstractListModel>
#include "SARibbonMainWindow.h"
//SARibbonActionsManager 特有
class SARibbonActionsManagerPrivate;
//SARibbonActionsModel 特有
class SARibbonActionsModelPrivete;
//SARibbonCustomizeWidget 特有
class SARibbonCustomizeWidgetUi;
class SARibbonCustomizeWidgetPrivate;

/**
 * @brief 用于管理SARibbon的所有Action，
 */
class SA_RIBBON_EXPORT SARibbonActionsManager : public QObject
{
    Q_OBJECT
    friend class SARibbonActionsModel;
public:

    /**
     * @brief 定义action的标签
     */
    enum ActionTag {
        InvalidActionTag		= 0             ///< 无效的tag
        , CommonlyUsedActionTag		= 0x01          ///< 常用命令
        , NotInFunctionalAreaActionTag	= 0x02          ///< 不在功能区命令
        , UserDefineActionTag		= 0x8000        ///< 自定义标签
    };
    typedef QMap<int, QList<QAction *> >::iterator ActionRef;
    SARibbonActionsManager(QObject *p = nullptr);
    ~SARibbonActionsManager();
    //设置tag对应的名字
    void setTagName(int tag, const QString& name);

    //获取tag对应的名字
    QString tagName(int tag) const;

    //注册action
    void registeAction(QAction *act, int tag);

    //取消action的注册
    void unregisteAction(QAction *act);

    //过滤得到actions对应的引用，实际是一个迭代器
    ActionRef filter(int tag);

    //判断返回的ActionRefs是否有效，如果没有tag，返回的ActionRefs是无效的
    bool isActionRefsValid(ActionRef r) const;

    //直接得到一个无效的ActionRefs
    ActionRef invalidActionRefs() const;

    //获取所有的标签
    QList<int> actionTags() const;

signals:

    /**
     * @brief 标签变化触发的信号，变化包括新增和删除
     */
    void actionTagChanged(int tag, bool isdelete);

private slots:
    void onActionDestroyed(QObject *o);

private:
    SARibbonActionsManagerPrivate *m_d;
};

/**
 * @brief SARibbonActionsManager 对应的model
 */
class SA_RIBBON_EXPORT SARibbonActionsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SARibbonActionsModel(QObject *p = nullptr);
    explicit SARibbonActionsModel(SARibbonActionsManager *m, QObject *p = nullptr);
    ~SARibbonActionsModel();
    virtual int rowCount(const QModelIndex& parent) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    void setFilter(int tag);
    void update();
    void setupActionsManager(SARibbonActionsManager *m);
    void uninstallActionsManager();

private slots:
    void onActionTagChanged(int tag, bool isdelete);

private:
    SARibbonActionsModelPrivete *m_d;
};


/**
 * @brief 自定义界面窗口
 */
class SA_RIBBON_EXPORT SARibbonCustomizeWidget : public QWidget
{
    Q_OBJECT
public:
    SARibbonCustomizeWidget(SARibbonMainWindow *ribbonWindow, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~SARibbonCustomizeWidget();
    //设置action管理器
    void setupActionsManager(SARibbonActionsManager *mgr);

    //判断用户是否有改动内容
    bool isChanged() const;

    //把定义的内容转换为xml
    virtual QString toXml() const;

private slots:
    void onComboBoxActionIndexCurrentIndexChanged(int index);

private:
    void initConnection();

private:
    SARibbonCustomizeWidgetUi *ui;
    SARibbonCustomizeWidgetPrivate *m_d;
};



#endif // SARIBBONCUSTOMIZEWIDGET_H
