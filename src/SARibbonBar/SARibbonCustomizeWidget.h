#ifndef SARIBBONCUSTOMIZEWIDGET_H
#define SARIBBONCUSTOMIZEWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QAbstractListModel>
#include "SARibbonBar.h"

//SARibbonActionsManager 特有
class SARibbonActionsManagerPrivate;
//SARibbonActionsModel 特有
class SARibbonActionsModelPrivete;
//SARibbonBarModel 特有
class SARibbonBarModelPrivate;
//SARibbonCustomizeData 特有
class SARibbonMainWindow;
//SARibbonCustomizeWidget 特有
class SARibbonCustomizeWidgetUi;
class SARibbonCustomizeWidgetPrivate;
class SARibbonMainWindow;
class QStandardItemModel;
class QStandardItem;



/**
 * @brief 用于管理SARibbon的所有Action，
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
    bool registeAction(QAction *act, int tag, const QString& key = QString());

    //取消action的注册
    void unregisteAction(QAction *act, bool enableEmit = true);

    //过滤得到actions对应的引用，实际是一个迭代器
    ActionRef filter(int tag);

    //判断返回的ActionRefs是否有效，如果没有tag，返回的ActionRefs是无效的
    bool isActionRefsValid(ActionRef r) const;

    //直接得到一个无效的ActionRefs
    ActionRef invalidActionRefs() const;

    //获取所有的标签
    QList<int> actionTags() const;

    //通过key获取action
    QAction *action(const QString& key) const;

    //通过action找到key
    QString key(QAction *act) const;

    //返回所有管理的action数
    int count() const;

signals:

    /**
     * @brief 标签变化触发的信号，变化包括新增和删除
     */
    void actionTagChanged(int tag, bool isdelete);

private slots:
    void onActionDestroyed(QObject *o);

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

private slots:
    void onActionTagChanged(int tag, bool isdelete);

private:
    SARibbonActionsModelPrivete *m_d;
};

/**
 * @brief 记录所有自定义操作的数据类
 * @note 此数据依赖于@ref SARibbonActionsManager 要在SARibbonActionsManager之后使用此类
 */
class SA_RIBBON_EXPORT SARibbonCustomizeData {
public:
    enum ActionType {
        UnknowActionType = 0            ///< 未知操作
        , AddCategoryActionType         ///< 添加category操作
        , AddPannelActionType           ///< 添加pannel操作
        , AddActionActionType           ///< 添加action操作
        , RemoveCategoryActionType      ///< 删除category操作
        , RemovePannelActionType        ///< 删除pannel操作
        , RemoveActionActionType        ///< 删除action操作
        , ChangeCategoryOrderActionType ///< 改变category顺序的操作
        , ChangePannelOrderActionType   ///< 改变pannel顺序的操作
        , ChangeActionOrderActionType   ///< 改变action顺序的操作
        , RenameCategoryActionType      ///< 对category更名操作
        , RenamePannelActionType        ///< 对Pannel更名操作
    };
    SARibbonCustomizeData();
    SARibbonCustomizeData(ActionType type, SARibbonActionsManager *mgr = nullptr);
    //获取CustomizeData的action type
    ActionType actionType() const;

    //判断是否是一个正常的CustomizeData
    bool isValid() const;

    //应用SARibbonCustomizeData
    bool apply(SARibbonMainWindow *m) const;


    //对应AddCategoryActionType
    static SARibbonCustomizeData makeAddCategoryCustomizeData(const QString& title
        , int index
        , const QString& objName);

    //对应AddPannelActionType
    static SARibbonCustomizeData makeAddPannelCustomizeData(const QString& title
        , int index
        , const QString& categoryobjName
        , const QString& objName);

    //对应AddActionActionType
    static SARibbonCustomizeData makeAddActionCustomizeData(const QString& key
        , SARibbonActionsManager *mgr
        , SARibbonPannelItem::RowProportion rp
        , const QString& categoryObjName
        , const QString& pannelObjName
        );

    //对应RenameCategoryActionType
    static SARibbonCustomizeData makeRenameCategoryCustomizeData(const QString& newname
        , const QString& categoryobjName);

    //对应RenamePannelActionType
    static SARibbonCustomizeData makeRenamePannelCustomizeData(const QString& newname
        , const QString& categoryobjName
        , const QString& pannelObjName);

    //对应RemoveCategoryActionType
    static SARibbonCustomizeData makeRemoveCategoryCustomizeData(const QString& categoryobjName);

    //对应ChangeCategoryOrderActionType
    static SARibbonCustomizeData makeChangeCategoryOrderCustomizeData(const QString& categoryobjName
        , int moveindex);

    //对应ChangePannelOrderActionType
    static SARibbonCustomizeData makeChangePannelOrderCustomizeData(const QString& categoryobjName
        , const QString& pannelObjName
        , int moveindex);

    //对应ChangeActionOrderActionType
    static SARibbonCustomizeData makeChangeActionOrderCustomizeData(const QString& categoryobjName
        , const QString& pannelObjName
        , const QString& key
        , SARibbonActionsManager *mgr
        , int moveindex);

    //对应RemovePannelActionType
    static SARibbonCustomizeData makeRemovePannelCustomizeData(const QString& categoryobjName
        , const QString& pannelObjName);

    //对应RemoveActionActionType
    static SARibbonCustomizeData makeRemoveActionCustomizeData(const QString& categoryobjName
        , const QString& pannelObjName
        , const QString& key
        , SARibbonActionsManager *mgr);

    //判断是否可以自定义
    static bool isCanCustomize(QObject *obj);
    static void setCanCustomize(QObject *obj, bool canbe = true);

public:

    /**
     * @brief 记录顺序的参数
     *
     * 在actionType==AddCategoryActionType时，此参数记录Category的insert位置,
     * 在actionType==AddPannelActionType时，此参数记录pannel的insert位置,
     * 在actionType==AddActionActionType时，此参数记录pannel的insert位置
     */
    int indexValue;

    /**
     * @brief 记录标题、索引等参数
     *
     * 在actionType==AddCategoryActionType时，key为category标题，
     * 在actionType==AddPannelActionType时，key为pannel标题，
     * 在actionType==AddActionActionType时，key为action的查询依据，基于SARibbonActionsManager::action查询
     */
    QString keyValue;

    /**
     * @brief 记录categoryObjName，用于定位Category
     */
    QString categoryObjNameValue;

    /**
     * @brief 记录pannelObjName，saribbon的Customize索引大部分基于objname
     */
    QString pannelObjNameValue;

    SARibbonPannelItem::RowProportion actionRowProportionValue;     ///< 行的占比，ribbon中有large，media和small三种占比,见@ref RowProportion
private:
    ActionType m_type;                                              ///< 标记这个data是category还是pannel亦或是action
    SARibbonActionsManager *m_actionsManagerPointer;
};
Q_DECLARE_METATYPE(SARibbonCustomizeData)

/**
 * @brief 自定义界面窗口
 */
class SA_RIBBON_EXPORT SARibbonCustomizeWidget : public QWidget
{
    Q_OBJECT
public:
    SARibbonCustomizeWidget(SARibbonMainWindow *ribbonWindow, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~SARibbonCustomizeWidget();

    /**
     * @brief 定义ribbon树的显示类型
     */
    enum RibbonTreeShowType {
        ShowAllCategory         ///< 显示所有Category，包括contextcategory
        , ShowMainCategory      ///< 显示主要的category，不包含上下文
    };

    /**
     * @brief QStandardItem对应的role
     */
    enum ItemRole {
        LevelRole		= Qt::UserRole + 1      ///< 代表这是层级，有0：category 1：pannel 2：item
        , PointerRole		= Qt::UserRole + 2      ///< 代表这是存放指针。根据LevelRole来进行转
        , CanCustomizeRole	= Qt::UserRole + 3      ///< 代表个item是可以自定义的.bool
        , CustomizeRole		= Qt::UserRole + 4      ///< 代表这个是自定义的item,bool,主要用于那些自己添加的标签和pannel，有此角色必有CanCustomizeRole
        , CustomizeObjNameRole	= Qt::UserRole + 5      ///< 记录了临时的自定义内容的obj名 QString
    };



    //设置action管理器
    void setupActionsManager(SARibbonActionsManager *mgr);

    //判断用户是否有改动内容
    bool isChanged() const;

    //获取model
    const QStandardItemModel *model() const;

    //根据当前的radiobutton选项来更新model
    void updateModel();

    //更新model
    void updateModel(RibbonTreeShowType type);

    //应用所有的设定
    bool applys();

protected:
    SARibbonPannelItem::RowProportion selectedRowProportion() const;

    QAction *selectedAction() const;
    QAction *itemToAction(QStandardItem *item) const;

    QStandardItem *selectedItem() const;


    //获取选中的ribbon tree 的level
    int selectedRibbonLevel() const;

    //根据选中的item判断
    int itemLevel(QStandardItem *item) const;

    //设置某个item被选中
    void setSelectItem(QStandardItem *item, bool ensureVisible = true);

    //判断itemn能否改动，可以改动返回true
    bool isItemCanCustomize(QStandardItem *item) const;
    bool isSelectedItemCanCustomize() const;

    //判断item是否是自定义的item
    bool isCustomizeItem(QStandardItem *item) const;
    bool isSelectedItemIsCustomize() const;

    //删除一个item
    void removeItem(QStandardItem *item);

private slots:
    void onComboBoxActionIndexCurrentIndexChanged(int index);
    void onRadioButtonGroupButtonClicked(QAbstractButton *b);
    void onPushButtonNewCategoryClicked();
    void onPushButtonNewPannelClicked();
    void onPushButtonRenameClicked();
    void onPushButtonAddClicked();
    void onPushButtonDeleteClicked();
    void onListViewSelectClicked(const QModelIndex& index);
    void onTreeViewResultClicked(const QModelIndex& index);
    void onToolButtonUpClicked();
    void onToolButtonDownClicked();

private:
    void initConnection();

private:
    SARibbonCustomizeWidgetUi *ui;
    SARibbonCustomizeWidgetPrivate *m_d;
};



#endif // SARIBBONCUSTOMIZEWIDGET_H
