#include "SARibbonCustomizeWidget.h"
#include <QDebug>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QListView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "SARibbonMainWindow.h"
#include "SARibbonPannel.h"
#include <QStandardItemModel>
#include <QButtonGroup>
#include <QInputDialog>
#include <QLineEdit>
#include <QDateTime>
////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SARibbonActionsManager
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <QMap>
#include <QHash>

class SARibbonActionsManagerPrivate
{
public:
    SARibbonActionsManager *mParent;
    QMap<int, QList<QAction *> > mTagToActions;     ///< tag : QList<QAction*>
    QMap<int, QList<QString> > mTagToActionKeys;    ///< tag : keys
    QMap<int, QString> mTagToName;                  ///< tag对应的名字
    QHash<QString, QAction *> mKeyToAction;         ///< key对应action
    int mSale;                                      ///< 盐用于生成固定的id，在用户不主动设置key时，id基于msale生成，只要SARibbonActionsManager的调用registeAction顺序不变，生成的id都不变，因为它是基于自增实现的
    SARibbonActionsManagerPrivate(SARibbonActionsManager *p);
};

SARibbonActionsManagerPrivate::SARibbonActionsManagerPrivate(SARibbonActionsManager *p)
    : mParent(p)
    , mSale(0)
{
}


SARibbonActionsManager::SARibbonActionsManager(QObject *p) : QObject(p)
    , m_d(new SARibbonActionsManagerPrivate(this))
{
}


SARibbonActionsManager::~SARibbonActionsManager()
{
    delete m_d;
}


/**
 * @brief 设置tag对应的名字，通过这个可以得到tag和文本的映射
 * @param tag
 * @param name
 * @note 在支持多语言的环境下，在语言切换时需要重新设置，以更新名字
 */
void SARibbonActionsManager::setTagName(int tag, const QString& name)
{
    m_d->mTagToName[tag] = name;
}


/**
 * @brief 获取tag对应的中文名字
 * @param tag
 * @return
 */
QString SARibbonActionsManager::tagName(int tag) const
{
    return (m_d->mTagToName.value(tag, ""));
}


/**
 * @brief 把action注册到管理器中，实现action的管理
 * @param act
 * @param tag tag是可以按照位进行叠加，见 @ref ActionTag 如果
 * 要定义自己的标签，建议定义大于@ref ActionTag::UserDefineActionTag 的值，
 * registeAction的tag是直接记录进去的，如果要多个标签并存，在registe之前先或好tag
 * @param key key是action对应的key，一个key只对应一个action，是查找action的关键
 * ,默认情况为一个QString(),这时key是QAction的objectName
 * @note 同一个action多次注册不同的tag可以通过tag索引到action，但通过action只能索引到最后一个注册的tag
 * @note tag的新增会触发actionTagChanged信号
 */
bool SARibbonActionsManager::registeAction(QAction *act, int tag, const QString& key)
{
    if (nullptr == act) {
        return (false);
    }
    QString k = key;

    if (k.isEmpty()) {
        k = QString("id_%1_%2").arg(m_d->mSale++).arg(act->objectName());
    }
    if (m_d->mKeyToAction.contains(k)) {
        qDebug() << "key " << k << "have been exist,you can set key in an unique value when use SARibbonActionsManager::registeAction";
        return (false);
    }
    m_d->mKeyToAction[k] = act;
    //记录tag 对 action
    bool isneedemit = !(m_d->mTagToActions.contains(tag));//记录是否需要发射信号

    m_d->mTagToActions[tag].append(act);
    m_d->mTagToActionKeys[tag].append(k);//key也记录
    //绑定槽
    connect(act, &QObject::destroyed, this, &SARibbonActionsManager::onActionDestroyed);
    if (isneedemit) {
        //说明新增tag
        emit actionTagChanged(tag, false);
    }
    return (true);
}


/**
 * @brief 取消action的注册
 *
 * 如果tag对应的最后一个action被撤销，tag也将一块删除
 * @param act
 * @note tag的删除会触发actionTagChanged信号
 * @note 如果action关联了多个tag，这些tag里的action都会被删除，对应的key也同理
 */
void SARibbonActionsManager::unregisteAction(QAction *act, bool enableEmit)
{
    if (nullptr == act) {
        return;
    }
    //绑定槽
    disconnect(act, &QObject::destroyed, this, &SARibbonActionsManager::onActionDestroyed);
    removeAction(act, enableEmit);
}


/**
 * @brief 移除action
 *
 * 仅移除内存内容
 * @param act
 * @param enableEmit
 */
void SARibbonActionsManager::removeAction(QAction *act, bool enableEmit)
{
    QList<int> deletedTags;                         //记录删除的tag，用于触发actionTagChanged
    QMap<int, QList<QAction *> > tagToActions;      ///< tag : QList<QAction*>
    QMap<int, QList<QString> > tagToActionKeys;     ///< tag : keys

    for (auto i = m_d->mTagToActions.begin(); i != m_d->mTagToActions.end(); ++i)
    {
        //把不是act的内容转移到tagToActions和tagToActionKeys中，之后再和m_d里的替换
        auto k = m_d->mTagToActionKeys.find(i.key());
        auto tmpi = tagToActions.insert(i.key(), QList<QAction *>());
        auto tmpk = tagToActionKeys.insert(i.key(), QList<QString>());
        int count = 0;
        for (int j = 0; j < i.value().size(); ++j)
        {
            if (i.value()[j] != act) {
                tmpi.value().append(act);
                tmpk.value().append(k.value()[j]);
                ++count;
            }
        }
        if (0 == count) {
            //说明这个tag没有内容
            tagToActions.erase(tmpi);
            tagToActionKeys.erase(tmpk);
            deletedTags.append(i.key());
        }
    }
    //删除mKeyToAction
    auto k = m_d->mKeyToAction.begin();

    for ( ; k != m_d->mKeyToAction.end();)
    {
        if (k.value() == act) {
            k = m_d->mKeyToAction.erase(k);
        }else{
            ++k;
        }
    }
    //置换
    m_d->mTagToActions.swap(tagToActions);
    m_d->mTagToActionKeys.swap(tagToActionKeys);
    //发射信号
    if (enableEmit) {
        for (int tagdelete : deletedTags)
        {
            emit actionTagChanged(tagdelete, true);
        }
    }
}


/**
 * @brief 过滤得到actions对应的引用，实际是一个迭代器
 * @param tag
 * @return
 */
SARibbonActionsManager::ActionRef SARibbonActionsManager::filter(int tag)
{
    return (m_d->mTagToActions.find(tag));
}


/**
 * @brief 判断返回的ActionRefs是否有效，如果没有tag，返回的ActionRefs是无效的
 * @param r
 * @return 有效返回true
 */
bool SARibbonActionsManager::isActionRefsValid(SARibbonActionsManager::ActionRef r) const
{
    return (r != m_d->mTagToActions.end());
}


/**
 * @brief 直接得到一个无效的ActionRefs
 * @return
 */
SARibbonActionsManager::ActionRef SARibbonActionsManager::invalidActionRefs() const
{
    return (m_d->mTagToActions.end());
}


/**
 * @brief 获取所有的标签
 * @return
 */
QList<int> SARibbonActionsManager::actionTags() const
{
    return (m_d->mTagToActions.keys());
}


/**
 * @brief 通过key获取action
 * @param key
 * @return 如果没有key，返回nullptr
 */
QAction *SARibbonActionsManager::action(const QString& key) const
{
    return (m_d->mKeyToAction.value(key, nullptr));
}


/**
 * @brief 返回所有管理的action数
 * @return
 */
int SARibbonActionsManager::count() const
{
    return (m_d->mKeyToAction.size());
}


/**
 * @brief action 被delete时，将触发此槽把管理的action删除
 * @param o
 * @note 这个函数不会触发actionTagChanged信号
 */
void SARibbonActionsManager::onActionDestroyed(QObject *o)
{
    QAction *act = static_cast<QAction *>(o);

    removeAction(act, false);
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
    SARibbonActionsManager::ActionRef mActionRef;
    SARibbonActionsModelPrivete(SARibbonActionsManagerModel *m);
    bool isValidRef() const;
    void updateRef();
    int count() const;
    QAction *at(int index);
    bool isNull() const;
};

SARibbonActionsModelPrivete::SARibbonActionsModelPrivete(SARibbonActionsManagerModel *m)
    : mParent(m)
    , mMgr(nullptr)
    , mTag(SARibbonActionsManager::CommonlyUsedActionTag)
{
}


bool SARibbonActionsModelPrivete::isValidRef() const
{
    if (isNull()) {
        return (false);
    }
    return (mMgr->isActionRefsValid(mActionRef));
}


void SARibbonActionsModelPrivete::updateRef()
{
    if (isNull()) {
        return;
    }
    mActionRef = mMgr->filter(mTag);
}


int SARibbonActionsModelPrivete::count() const
{
    if (isNull()) {
        return (0);
    }
    if (isValidRef()) {
        return (mActionRef.value().size());
    }
    return (0);
}


QAction *SARibbonActionsModelPrivete::at(int index)
{
    if (isNull()) {
        return (nullptr);
    }
    if (!isValidRef()) {
        return (nullptr);
    }
    if (index >= count()) {
        return (nullptr);
    }
    return (mActionRef.value().at(index));
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
    if (parent.isValid()) {//非顶层
        return (0);
    }
    //顶层
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
    m_d->mActionRef = m->filter(m_d->mTag);
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


void SARibbonActionsManagerModel::onActionTagChanged(int tag, bool isdelete)
{
    if (isdelete && (tag == m_d->mTag)) {
        m_d->mTag = SARibbonActionsManager::InvalidActionTag;
        update();
    }else{
        if (tag == m_d->mTag) {
            update();
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SARibbonCustomizeData
////////////////////////////////////////////////////////////////////////////////////////////////////////


SARibbonCustomizeData::SARibbonCustomizeData()
    : m_type(UnknowActionType)
    , m_actionsManagerPointer(nullptr)
{
}


SARibbonCustomizeData::SARibbonCustomizeData(ActionType type, SARibbonActionsManager *mgr)
    : m_type(type)
    , m_actionsManagerPointer(mgr)
{
}


SARibbonCustomizeData::ActionType SARibbonCustomizeData::actionType() const
{
    return (m_type);
}


/**
 * @brief 判断是否是一个正常的CustomizeData
 *
 * 实际逻辑actionType() != UnknowActionType
 * @return 有用的CustomizeData返回true
 */
bool SARibbonCustomizeData::isValid() const
{
    return (actionType() != UnknowActionType);
}


/**
 * @brief 应用SARibbonCustomizeData到SARibbonMainWindow
 * @param m
 * @return 如果应用失败，返回false,如果actionType==UnknowActionType直接返回false
 */
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
        //添加标签
        SARibbonCategory *c = bar->insertCategoryPage(keyValue, indexValue);
        if (nullptr == c) {
            return (false);
        }
        c->setObjectName(categoryObjNameValue);
        return (true);
    }

    case AddPannelActionType:
    {
        //添加pannel
        SARibbonCategory *c = bar->categoryByObjectName(categoryObjNameValue);
        if (nullptr == c) {
            return (false);
        }
        SARibbonPannel *p = c->insertPannel(keyValue, indexValue);
        p->setObjectName(pannelObjNameValue);
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
        SARibbonPannel *pannel = c->pannelList().value(indexValue, nullptr);
        if (nullptr == pannel) {
            return (false);
        }
        pannel->setWindowTitle(keyValue);
        return (true);
    }

    default:
        break;
    }
    return (false);
}


/**
 * @brief 创建一个AddCategoryActionType的SARibbonCustomizeData
 * @param title category 的标题
 * @param index category要插入的位置
 * @param objName category的object name
 * @return 返回AddCategoryActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddCategoryCustomizeData(const QString& title, int index, const QString& objName)
{
    SARibbonCustomizeData d(AddCategoryActionType);

    d.indexValue = index;
    d.keyValue = title;
    d.categoryObjNameValue = objName;
    return (d);
}


/**
 * @brief 创建一个AddPannelActionType的SARibbonCustomizeData
 * @param title pannel的标题
 * @param index pannel的index
 * @param categoryobjName pannel的category的objectname
 * @param objName pannel的objname
 * @return 返回AddPannelActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeAddPannelCustomizeData(const QString& title, int index, const QString& categoryobjName, const QString& objName)
{
    SARibbonCustomizeData d(AddPannelActionType);

    d.indexValue = index;
    d.keyValue = title;
    d.pannelObjNameValue = objName;
    d.categoryObjNameValue = categoryobjName;
    return (d);
}


/**
 * @brief 创建一个RenameCategoryActionType的SARibbonCustomizeData
 * @param newname 新名字
 * @param index category的索引
 * @return 返回RenameCategoryActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRenameCategoryCustomizeData(const QString& newname, const QString& categoryobjName)
{
    SARibbonCustomizeData d(RenameCategoryActionType);

    d.keyValue = newname;
    d.categoryObjNameValue = categoryobjName;
    return (d);
}


/**
 * @brief 创建一个RenamePannelActionType的SARibbonCustomizeData
 * @param newname pannel的名字
 * @param indexValue pannel的索引
 * @param categoryobjName pannel对应的category的object name
 * @return 返回RenamePannelActionType的SARibbonCustomizeData
 */
SARibbonCustomizeData SARibbonCustomizeData::makeRenamePannelCustomizeData(const QString& newname, int pannelIndex, const QString& categoryobjName)
{
    SARibbonCustomizeData d(RenamePannelActionType);

    d.keyValue = newname;
    d.indexValue = pannelIndex;
    d.categoryObjNameValue = categoryobjName;
    return (d);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
///SARibbonCustomizeWidget
////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * @brief 构建SARibbonCustomizeWidget的Ui
 */
class SARibbonCustomizeWidgetUi
{
public:
    QHBoxLayout *horizontalLayoutMain;
    QVBoxLayout *verticalLayoutSelect;
    QLabel *labelSelectAction;
    QHBoxLayout *horizontalLayoutSearch;
    QComboBox *comboBoxActionIndex;
    QLineEdit *lineEditSearchAction;
    QListView *listViewSelect;
    QVBoxLayout *verticalLayoutMidButtons;
    QSpacerItem *verticalSpacerUp;
    QPushButton *pushButtonAdd;
    QPushButton *pushButtonDelete;
    QSpacerItem *verticalSpacerDown;
    QLabel *labelProportion;
    QComboBox *comboBoxActionProportion;
    QVBoxLayout *verticalLayoutResult;
    QLabel *labelCustomize;
    QHBoxLayout *horizontalLayoutCategorySelect;
    QRadioButton *radioButtonMainCategory;
    QRadioButton *radioButtonAllCategory;
    QButtonGroup *radioButtonGroup;
    QTreeView *treeViewResult;
    QHBoxLayout *horizontalLayoutActionOptBtns;
    QPushButton *pushButtonNewCategory;
    QPushButton *pushButtonNewPannel;
    QPushButton *pushButtonRename;
    QVBoxLayout *verticalLayoutRightButtons;
    QSpacerItem *verticalSpacerUp2;
    QToolButton *toolButtonUp;
    QToolButton *toolButtonDown;
    QSpacerItem *verticalSpacerDown2;

    void setupUi(QWidget *customizeWidget)
    {
        if (customizeWidget->objectName().isEmpty()) {
            customizeWidget->setObjectName(QStringLiteral("SARibbonCustomizeWidget"));
        }
        customizeWidget->resize(800, 600);
        horizontalLayoutMain = new QHBoxLayout(customizeWidget);
        horizontalLayoutMain->setObjectName(QStringLiteral("horizontalLayoutMain"));
        verticalLayoutSelect = new QVBoxLayout();
        verticalLayoutSelect->setObjectName(QStringLiteral("verticalLayoutSelect"));
        labelSelectAction = new QLabel(customizeWidget);
        labelSelectAction->setObjectName(QStringLiteral("labelSelectAction"));

        verticalLayoutSelect->addWidget(labelSelectAction);

        horizontalLayoutSearch = new QHBoxLayout();
        horizontalLayoutSearch->setObjectName(QStringLiteral("horizontalLayoutSearch"));
        comboBoxActionIndex = new QComboBox(customizeWidget);
        comboBoxActionIndex->setObjectName(QStringLiteral("comboBoxActionIndex"));
        comboBoxActionIndex->setEditable(false);

        horizontalLayoutSearch->addWidget(comboBoxActionIndex);

        lineEditSearchAction = new QLineEdit(customizeWidget);
        lineEditSearchAction->setObjectName(QStringLiteral("lineEditSearchAction"));

        horizontalLayoutSearch->addWidget(lineEditSearchAction);


        verticalLayoutSelect->addLayout(horizontalLayoutSearch);

        listViewSelect = new QListView(customizeWidget);
        listViewSelect->setObjectName(QStringLiteral("listViewSelect"));

        verticalLayoutSelect->addWidget(listViewSelect);


        horizontalLayoutMain->addLayout(verticalLayoutSelect);

        verticalLayoutMidButtons = new QVBoxLayout();
        verticalLayoutMidButtons->setObjectName(QStringLiteral("verticalLayoutMidButtons"));
        verticalSpacerUp = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutMidButtons->addItem(verticalSpacerUp);

        pushButtonAdd = new QPushButton(customizeWidget);
        pushButtonAdd->setObjectName(QStringLiteral("pushButtonAdd"));
        pushButtonAdd->setEnabled(false);

        verticalLayoutMidButtons->addWidget(pushButtonAdd);

        pushButtonDelete = new QPushButton(customizeWidget);
        pushButtonDelete->setObjectName(QStringLiteral("pushButtonDelete"));
        pushButtonDelete->setEnabled(false);

        verticalLayoutMidButtons->addWidget(pushButtonDelete);

        verticalSpacerDown = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutMidButtons->addItem(verticalSpacerDown);

        labelProportion = new QLabel(customizeWidget);
        labelProportion->setObjectName(QStringLiteral("labelProportion"));
        verticalLayoutMidButtons->addWidget(labelProportion);

        comboBoxActionProportion = new QComboBox(customizeWidget);
        comboBoxActionProportion->setObjectName(QStringLiteral("comboBoxActionProportion"));
        comboBoxActionProportion->setEditable(false);
        verticalLayoutMidButtons->addWidget(comboBoxActionProportion);
        horizontalLayoutMain->addLayout(verticalLayoutMidButtons);

        verticalLayoutResult = new QVBoxLayout();
        verticalLayoutResult->setObjectName(QStringLiteral("verticalLayoutResult"));
        labelCustomize = new QLabel(customizeWidget);
        labelCustomize->setObjectName(QStringLiteral("labelCustomize"));

        verticalLayoutResult->addWidget(labelCustomize);

        horizontalLayoutCategorySelect = new QHBoxLayout();
        horizontalLayoutCategorySelect->setObjectName(QStringLiteral("horizontalLayoutCategorySelect"));
        radioButtonMainCategory = new QRadioButton(customizeWidget);
        radioButtonMainCategory->setObjectName(QStringLiteral("radioButtonMainCategory"));
        radioButtonMainCategory->setChecked(false);

        horizontalLayoutCategorySelect->addWidget(radioButtonMainCategory);

        radioButtonAllCategory = new QRadioButton(customizeWidget);
        radioButtonAllCategory->setObjectName(QStringLiteral("radioButtonAllCategory"));
        radioButtonAllCategory->setChecked(true);

        horizontalLayoutCategorySelect->addWidget(radioButtonAllCategory);

        radioButtonGroup = new QButtonGroup(customizeWidget);
        radioButtonGroup->addButton(radioButtonMainCategory);
        radioButtonGroup->addButton(radioButtonAllCategory);

        verticalLayoutResult->addLayout(horizontalLayoutCategorySelect);

        treeViewResult = new QTreeView(customizeWidget);
        treeViewResult->setObjectName(QStringLiteral("treeViewResult"));
        treeViewResult->setHeaderHidden(true);
        treeViewResult->setSelectionMode(QAbstractItemView::SingleSelection);
        treeViewResult->setAnimated(true);                                      //支持动画
        treeViewResult->setEditTriggers(QAbstractItemView::NoEditTriggers);     //不允许直接在item上重命名


        verticalLayoutResult->addWidget(treeViewResult);

        horizontalLayoutActionOptBtns = new QHBoxLayout();
        horizontalLayoutActionOptBtns->setObjectName(QStringLiteral("horizontalLayoutActionOptBtns"));
        pushButtonNewCategory = new QPushButton(customizeWidget);
        pushButtonNewCategory->setObjectName(QStringLiteral("pushButtonNewCategory"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonNewCategory);

        pushButtonNewPannel = new QPushButton(customizeWidget);
        pushButtonNewPannel->setObjectName(QStringLiteral("pushButtonNewPannel"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonNewPannel);

        pushButtonRename = new QPushButton(customizeWidget);
        pushButtonRename->setObjectName(QStringLiteral("pushButtonRename"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonRename);


        verticalLayoutResult->addLayout(horizontalLayoutActionOptBtns);


        horizontalLayoutMain->addLayout(verticalLayoutResult);

        verticalLayoutRightButtons = new QVBoxLayout();
        verticalLayoutRightButtons->setObjectName(QStringLiteral("verticalLayoutRightButtons"));
        verticalSpacerUp2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutRightButtons->addItem(verticalSpacerUp2);

        toolButtonUp = new QToolButton(customizeWidget);
        toolButtonUp->setObjectName(QStringLiteral("pushButtonUp"));
        toolButtonUp->setArrowType(Qt::UpArrow);
        toolButtonUp->setAutoRaise(true);

        verticalLayoutRightButtons->addWidget(toolButtonUp);

        toolButtonDown = new QToolButton(customizeWidget);
        toolButtonDown->setObjectName(QStringLiteral("pushButtonDown"));
        toolButtonDown->setArrowType(Qt::DownArrow);
        toolButtonDown->setAutoRaise(true);

        verticalLayoutRightButtons->addWidget(toolButtonDown);

        verticalSpacerDown2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutRightButtons->addItem(verticalSpacerDown2);


        horizontalLayoutMain->addLayout(verticalLayoutRightButtons);

        retranslateUi(customizeWidget);

        QMetaObject::connectSlotsByName(customizeWidget);
    } // setupUi


    void retranslateUi(QWidget *customizeWidget)
    {
        customizeWidget->setWindowTitle(QApplication::translate("SARibbonCustomizeWidget", "SARibbonCustomizeWidget", Q_NULLPTR));
        labelSelectAction->setText(QApplication::translate("SARibbonCustomizeWidget", "\344\273\216\344\270\213\345\210\227\351\200\211\351\241\271\345\215\241\351\200\211\346\213\251\345\221\275\344\273\244\357\274\232", Q_NULLPTR));
        lineEditSearchAction->setInputMask(QString());
        lineEditSearchAction->setText(QString());
        lineEditSearchAction->setPlaceholderText(QApplication::translate("SARibbonCustomizeWidget", "\346\237\245\346\211\276\345\221\275\344\273\244", Q_NULLPTR));
        pushButtonAdd->setText(QApplication::translate("SARibbonCustomizeWidget", "\346\267\273\345\212\240 >>", Q_NULLPTR));
        pushButtonDelete->setText(QApplication::translate("SARibbonCustomizeWidget", "<< \345\210\240\351\231\244", Q_NULLPTR));
        labelCustomize->setText(QApplication::translate("SARibbonCustomizeWidget", "\350\207\252\345\256\232\344\271\211\345\212\237\350\203\275\345\214\272\357\274\232", Q_NULLPTR));
        radioButtonMainCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "\344\270\273\351\200\211\351\241\271\345\215\241", Q_NULLPTR));
        radioButtonAllCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "\346\211\200\346\234\211\351\200\211\351\241\271\345\215\241", Q_NULLPTR));
        pushButtonNewCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "\346\226\260\345\273\272\351\200\211\351\241\271\345\215\241", Q_NULLPTR));
        pushButtonNewPannel->setText(QApplication::translate("SARibbonCustomizeWidget", "\346\226\260\345\273\272\347\273\204", Q_NULLPTR));
        pushButtonRename->setText(QApplication::translate("SARibbonCustomizeWidget", "\351\207\215\345\221\275\345\220\215", Q_NULLPTR));
        labelProportion->setText(QApplication::translate("SARibbonCustomizeWidget", "proportion:", Q_NULLPTR));
    } // retranslateUi
};

/**
 * @brief 管理SARibbonCustomizeWidget的业务逻辑
 */
class SARibbonCustomizeWidgetPrivate
{
public:
    SARibbonCustomizeWidget *mParent;
    SARibbonCustomizeWidget::RibbonTreeShowType mShowType;  ///< 显示类型
    SARibbonMainWindow *mRibbonWindow;                      ///< 保存SARibbonMainWindow的指针
    bool mIsChanged;                                        ///< 判断用户是否有改动内容
    SARibbonActionsManager *mActionMgr;                     ///< action管理器
    SARibbonActionsManagerModel *mAcionModel;               ///< action管理器对应的model
    QStandardItemModel *mRibbonModel;                       ///< 用于很成ribbon的树
    int mCustomizeCategoryCount;                            ///< 记录自定义Category的个数
    int mCustomizePannelCount;                              ///< 记录自定义Pannel的个数
    SARibbonCustomizeWidgetPrivate(SARibbonCustomizeWidget *p);
    void updateModel();

    QList<SARibbonCustomizeData> mCustomizeDatas; ///< 记录所有的自定义动作
    //创建一个随机id，形如：pre_QDateTime::currentMSecsSinceEpoch_suf
    static QString makeRandomObjName(const QString& pre);

    int itemLevel(QStandardItem *item) const;

    //
    bool isItemCanCustomize(QStandardItem *item) const;

    //把item转换为category
    SARibbonCategory *itemToCategory(QStandardItem *item) const;

    //把item转换为SARibbonPannel
    SARibbonPannel *itemToPannel(QStandardItem *item) const;

    //获取item对应的object name
    QString itemObjectName(QStandardItem *item) const;
};

SARibbonCustomizeWidgetPrivate::SARibbonCustomizeWidgetPrivate(SARibbonCustomizeWidget *p)
    : mParent(p)
    , mShowType(SARibbonCustomizeWidget::ShowAllCategory)
    , mRibbonWindow(nullptr)
    , mIsChanged(false)
    , mActionMgr(nullptr)
    , mAcionModel(new SARibbonActionsManagerModel(p))
    , mRibbonModel(new QStandardItemModel(p))
    , mCustomizeCategoryCount(0)
    , mCustomizePannelCount(0)
{
}


void SARibbonCustomizeWidgetPrivate::updateModel()
{
    if (mRibbonWindow == nullptr) {
        return;
    }
    mRibbonModel->clear();
    SARibbonBar *ribbonbar = mRibbonWindow->ribbonBar();
    QList<SARibbonCategory *> categorys = ribbonbar->categoryPages();

    for (SARibbonCategory *c : categorys)
    {
        if ((mShowType == SARibbonCustomizeWidget::ShowMainCategory) && c->isContextCategory()) {
            //如果是只显示主内容，如果是上下文标签就忽略
            continue;
        }
        QStandardItem *ci = new QStandardItem();
        if (c->isContextCategory()) {
            ci->setText(QString("[%1]").arg(c->windowTitle()));
        }else{
            ci->setText(c->windowTitle());
        }
        ci->setCheckable(true);
        ci->setCheckState(Qt::Checked);
        ci->setData(0, SARibbonCustomizeWidget::LevelRole);
        ci->setData(QVariant::fromValue<qintptr>(qintptr(c)), SARibbonCustomizeWidget::PointerRole);
        QList<SARibbonPannel *> pannels = c->pannelList();
        for (SARibbonPannel *p : pannels)
        {
            QStandardItem *pi = new QStandardItem(p->windowTitle());
            pi->setData(1, SARibbonCustomizeWidget::LevelRole);
            pi->setData(QVariant::fromValue<qintptr>(qintptr(p)), SARibbonCustomizeWidget::PointerRole);
            ci->appendRow(pi);
            const QList<SARibbonPannelItem *>& items = p->ribbonPannelItem();
            for (SARibbonPannelItem *i : items)
            {
                if (i->action->isSeparator()) {
                    continue;
                }
                QStandardItem *ii = new QStandardItem();
                if (i->customWidget) {
                    //如果是自定义窗口
                    ii->setText(i->widget()->windowTitle());
                    ii->setIcon(i->widget()->windowIcon());
                }else{
                    ii->setText(i->action->text());
                    ii->setIcon(i->action->icon());
                }
                ii->setData(2, SARibbonCustomizeWidget::LevelRole);
                ii->setData(QVariant::fromValue<qintptr>(qintptr(i)), SARibbonCustomizeWidget::PointerRole);
                pi->appendRow(ii);
            }
        }
        mRibbonModel->appendRow(ci);
    }
}


/**
 * @brief 创建一个随机id，形如：pre_QDateTime::currentMSecsSinceEpoch
 * @param pre 前缀
 * @return
 */
QString SARibbonCustomizeWidgetPrivate::makeRandomObjName(const QString& pre)
{
    return (QString("%1_%2").arg(pre).arg(QDateTime::currentMSecsSinceEpoch()));
}


/**
 * @brief 获取item的level
 * @param item
 * @return
 */
int SARibbonCustomizeWidgetPrivate::itemLevel(QStandardItem *item) const
{
    return (item->data(SARibbonCustomizeWidget::LevelRole).toInt());
}


/**
 * @brief 判断itemn能否改动，可以改动返回true
 * @param item
 * @return
 */
bool SARibbonCustomizeWidgetPrivate::isItemCanCustomize(QStandardItem *item) const
{
    if (nullptr == item) {
        return (false);
    }
    return (item->data(SARibbonCustomizeWidget::CustomizeRole).isValid());
}


/**
 * @brief 把item转换为category
 * @param item
 * @return无法转换返回nullptr
 */
SARibbonCategory *SARibbonCustomizeWidgetPrivate::itemToCategory(QStandardItem *item) const
{
    int level = item->data(SARibbonCustomizeWidget::LevelRole).toInt();

    if (level != 0) {
        return (nullptr);
    }
    qintptr p = item->data(SARibbonCustomizeWidget::PointerRole).value<qintptr>();

    return (reinterpret_cast<SARibbonCategory *>(p));
}


/**
 * @brief 把item转换为pannel
 * @param item
 * @return 无法转换返回nullptr
 */
SARibbonPannel *SARibbonCustomizeWidgetPrivate::itemToPannel(QStandardItem *item) const
{
    int level = item->data(SARibbonCustomizeWidget::LevelRole).toInt();

    if (level != 1) {
        return (nullptr);
    }
    qintptr p = item->data(SARibbonCustomizeWidget::PointerRole).value<qintptr>();

    return (reinterpret_cast<SARibbonPannel *>(p));
}


/**
 * @brief 获取item对应的objectname
 * @param item
 * @return 如果无法获取，返回一个空的QString
 */
QString SARibbonCustomizeWidgetPrivate::itemObjectName(QStandardItem *item) const
{
    QString objName;

    if (isItemCanCustomize(item)) {
        //说明是自定义的
        objName = item->data(SARibbonCustomizeWidget::CustomizeObjNameRole).toString();
    }else{
        //说明这个是非自定义的
        int level = itemLevel(item);
        if (0 == level) {
            SARibbonCategory *category = itemToCategory(item);
            if (category) {
                objName = category->objectName();
            }
        }else if (1 == level) {
            SARibbonPannel *pannel = itemToPannel(item);
            if (pannel) {
                objName = pannel->objectName();
            }
        }
    }
    return (objName);
}


/**
 * @brief SARibbonCustomizeWidget::SARibbonCustomizeWidget
 * @param ribbonWindow 传入需要管理的SARibbonMainWindow指针
 * @param parent 用于界面生成的parent，可以和SARibbonMainWindow一样
 * @param f 同QWidget::QWidget的第二个参数
 */
SARibbonCustomizeWidget::SARibbonCustomizeWidget(SARibbonMainWindow *ribbonWindow, QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
    , ui(new SARibbonCustomizeWidgetUi)
    , m_d(new SARibbonCustomizeWidgetPrivate(this))
{
    m_d->mRibbonWindow = ribbonWindow;
    ui->setupUi(this);
    ui->listViewSelect->setModel(m_d->mAcionModel);
    ui->treeViewResult->setModel(m_d->mRibbonModel);
    initConnection();
    updateModel();
}


SARibbonCustomizeWidget::~SARibbonCustomizeWidget()
{
    delete ui;
    delete m_d;
}


void SARibbonCustomizeWidget::initConnection()
{
    connect(ui->comboBoxActionIndex, QOverload<int>::of(&QComboBox::currentIndexChanged)
        , this, &SARibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged);
    connect(ui->radioButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked)
        , this, &SARibbonCustomizeWidget::onRadioButtonGroupButtonClicked);
    connect(ui->pushButtonNewCategory, &QPushButton::clicked
        , this, &SARibbonCustomizeWidget::onPushButtonNewCategoryClicked);
    connect(ui->pushButtonNewPannel, &QPushButton::clicked
        , this, &SARibbonCustomizeWidget::onPushButtonNewPannelClicked);
    connect(ui->pushButtonRename, &QPushButton::clicked
        , this, &SARibbonCustomizeWidget::onPushButtonRenameClicked);
    connect(ui->pushButtonAdd, &QPushButton::clicked
        , this, &SARibbonCustomizeWidget::onPushButtonAddClicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked
        , this, &SARibbonCustomizeWidget::onPushButtonDeleteClicked);
    connect(ui->listViewSelect, &QAbstractItemView::clicked
        , this, &SARibbonCustomizeWidget::onListViewSelectClicked);
    connect(ui->treeViewResult, &QAbstractItemView::clicked
        , this, &SARibbonCustomizeWidget::onTreeViewResultClicked);
    connect(ui->toolButtonUp, &QToolButton::clicked
        , this, &SARibbonCustomizeWidget::onToolButtonUpClicked);
    connect(ui->toolButtonDown, &QToolButton::clicked
        , this, &SARibbonCustomizeWidget::onToolButtonDownClicked);
}


/**
 * @brief 设置action管理器
 * @param mgr
 */
void SARibbonCustomizeWidget::setupActionsManager(SARibbonActionsManager *mgr)
{
    m_d->mActionMgr = mgr;
    if (m_d->mActionMgr) {
        m_d->mAcionModel->uninstallActionsManager();
    }
    m_d->mAcionModel->setupActionsManager(mgr);
    //更新左边复选框
    QList<int> tags = mgr->actionTags();

    ui->comboBoxActionIndex->clear();
    for (int tag : tags)
    {
        ui->comboBoxActionIndex->addItem(mgr->tagName(tag), tag);
    }
}


bool SARibbonCustomizeWidget::isChanged() const
{
    return (m_d->mIsChanged);
}


/**
 * @brief 获取model
 * @return
 */
const QStandardItemModel *SARibbonCustomizeWidget::model() const
{
    return (m_d->mRibbonModel);
}


/**
 * @brief 根据当前的radiobutton选项来更新model
 */
void SARibbonCustomizeWidget::updateModel()
{
    updateModel(ui->radioButtonAllCategory->isChecked() ? ShowAllCategory : ShowMainCategory);
}


/**
 * @brief 更新model
 */
void SARibbonCustomizeWidget::updateModel(RibbonTreeShowType type)
{
    m_d->mShowType = type;
    m_d->updateModel();
}


/**
 * @brief 应用所有设定
 * @return 应用成功返回true
 * @note 所有设定有一个应用成功都会返回true
 */
bool SARibbonCustomizeWidget::applys()
{
    bool isSucc = false;

    for (SARibbonCustomizeData& d : m_d->mCustomizeDatas)
    {
        isSucc |= (d.apply(m_d->mRibbonWindow));
    }
    return (isSucc);
}


/**
 * @brief 获取选中的action
 * @return
 */
QAction *SARibbonCustomizeWidget::selectedAction() const
{
    QItemSelectionModel *m = ui->listViewSelect->selectionModel();

    if ((nullptr == m) || !m->hasSelection()) {
        return (nullptr);
    }
    QModelIndex i = m->currentIndex();

    return (m_d->mAcionModel->indexToAction(i));
}


/**
 * @brief 获取ribbon tree选中的item
 * @return
 */
QStandardItem *SARibbonCustomizeWidget::selectedItem() const
{
    QItemSelectionModel *m = ui->treeViewResult->selectionModel();

    if ((nullptr == m) || !m->hasSelection()) {
        return (nullptr);
    }
    QModelIndex i = m->currentIndex();

    return (m_d->mRibbonModel->itemFromIndex(i));
}


/**
 * @brief 获取选中的ribbon tree 的level
 * @return -1为选中异常，0代表选中了category 1代表选中了pannel 2代表选中了action
 */
int SARibbonCustomizeWidget::selectedRibbonLevel() const
{
    QStandardItem *item = selectedItem();

    if (item) {
        return (itemLevel(item));
    }
    return (-1);
}


/**
 * @brief 获取StandardItem 的level
 * @param item
 * @return
 */
int SARibbonCustomizeWidget::itemLevel(QStandardItem *item) const
{
    return (m_d->itemLevel(item));
}


/**
 * @brief 设置某个item被选中
 * @param item
 */
void SARibbonCustomizeWidget::setSelectItem(QStandardItem *item, bool ensureVisible)
{
    QItemSelectionModel *m = ui->treeViewResult->selectionModel();

    if (nullptr == m) {
        return;
    }
    if (m) {
        m->clearSelection();
        m->select(item->index(), QItemSelectionModel::Select);
    }
    if (ensureVisible) {
        ui->treeViewResult->scrollTo(item->index());
    }
}


/**
 * @brief 判断itemn能否改动，可以改动返回true
 * @param item
 * @return
 */
bool SARibbonCustomizeWidget::isItemCanCustomize(QStandardItem *item) const
{
    return (m_d->isItemCanCustomize(item));
}


bool SARibbonCustomizeWidget::isSelectedItemCanCustomize() const
{
    return (isItemCanCustomize(selectedItem()));
}


void SARibbonCustomizeWidget::removeItem(QStandardItem *item)
{
    if (item->parent()) {
        item->parent()->removeRow(item->row());
    }else{
        m_d->mRibbonModel->removeRow(item->row());
    }
}


void SARibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged(int index)
{
    int tag = ui->comboBoxActionIndex->itemData(index).toInt();

    m_d->mAcionModel->setFilter(tag);
}


void SARibbonCustomizeWidget::onRadioButtonGroupButtonClicked(QAbstractButton *b)
{
    updateModel(b == ui->radioButtonAllCategory ? ShowAllCategory : ShowMainCategory);
}


void SARibbonCustomizeWidget::onPushButtonNewCategoryClicked()
{
    int row = m_d->mRibbonModel->rowCount();
    QItemSelectionModel *m = ui->treeViewResult->selectionModel();

    if (m && m->hasSelection()) {
        QModelIndex i = m->currentIndex();
        while (i.parent().isValid())
        {
            i = i.parent();
        }
        //获取选中的最顶层item
        row = i.row()+1;
    }
    QStandardItem *ni = new QStandardItem(tr("new category[customize]%1").arg(++(m_d->mCustomizeCategoryCount)));

    ni->setData(0, SARibbonCustomizeWidget::LevelRole);
    m_d->mRibbonModel->insertRow(row, ni);
    //设置新增的为选中
    setSelectItem(ni);
    //把动作插入动作列表中
    SARibbonCustomizeData d = SARibbonCustomizeData::makeAddCategoryCustomizeData(ni->text()
        , ni->row()
        , SARibbonCustomizeWidgetPrivate::makeRandomObjName("category"));

    m_d->mCustomizeDatas.append(d);
    ni->setData(true, SARibbonCustomizeWidget::CustomizeRole);
    ni->setData(d.categoryObjNameValue, SARibbonCustomizeWidget::CustomizeObjNameRole);
}


void SARibbonCustomizeWidget::onPushButtonNewPannelClicked()
{
    QStandardItem *item = selectedItem();
    int level = selectedRibbonLevel();

    QStandardItem *ni = new QStandardItem(tr("new pannel[customize]%1").arg(++(m_d->mCustomizePannelCount)));

    ni->setData(1, SARibbonCustomizeWidget::LevelRole);

    if (0 == level) {
        //说明是category,插入到最后
        item->appendRow(ni);
    }else if (1 == level) {
        //说明选择的是pannel，插入到此pannel之后
        QStandardItem *categoryItem = item->parent();
        if (nullptr == categoryItem) {
            return;
        }
        categoryItem->insertRow(item->row()+1, ni);
    }else{
        //不符合就删除退出
        delete ni;
        ni = nullptr;
        return;
    }
    //查找category的object name
    QStandardItem *categoryItem = ni->parent();
    QString categoryObjName = "";


    categoryObjName = m_d->itemObjectName(categoryItem);
    SARibbonCustomizeData d = SARibbonCustomizeData::makeAddPannelCustomizeData(ni->text()
        , ni->row()
        , categoryObjName
        , SARibbonCustomizeWidgetPrivate::makeRandomObjName("pannel"));

    m_d->mCustomizeDatas.append(d);
    ni->setData(true, SARibbonCustomizeWidget::CustomizeRole);
    ni->setData(d.pannelObjNameValue, SARibbonCustomizeWidget::CustomizeObjNameRole);
    setSelectItem(ni);
}


void SARibbonCustomizeWidget::onPushButtonRenameClicked()
{
    QStandardItem *item = selectedItem();

    if (nullptr == item) {
        return;
    }
    bool ok;
    QString text = "";

    text = QInputDialog::getText(this, tr("rename"),
        tr("name:"), QLineEdit::Normal,
        item->text(), &ok);

    if (!ok || text.isEmpty()) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        //改Category名
        QString cateObjName = m_d->itemObjectName(item);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRenameCategoryCustomizeData(text, cateObjName);
        m_d->mCustomizeDatas.append(d);
    }else if (1 == level) {
        QString cateObjName = m_d->itemObjectName(item->parent());
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRenamePannelCustomizeData(text, item->row(), cateObjName);
        m_d->mCustomizeDatas.append(d);
    }else{
        //action 不允许改名
        return;
    }
    item->setText(text);
}


void SARibbonCustomizeWidget::onPushButtonAddClicked()
{
}


void SARibbonCustomizeWidget::onPushButtonDeleteClicked()
{
    QStandardItem *item = selectedItem();

    if (isItemCanCustomize(item)) {
        removeItem(item);
        //删除后重新识别
        ui->pushButtonAdd->setEnabled(selectedAction() && isSelectedItemCanCustomize() && selectedRibbonLevel() > 0);
        ui->pushButtonDelete->setEnabled(isSelectedItemCanCustomize());
    }
}


void SARibbonCustomizeWidget::onListViewSelectClicked(const QModelIndex& index)
{
    //每次点击，判断是否可以进行操作，决定pushButtonAdd和pushButtonDelete的显示状态
    //点击了listview，判断treeview的状态
    Q_UNUSED(index);
    ui->pushButtonAdd->setEnabled(isSelectedItemCanCustomize() && selectedRibbonLevel() > 0);
    ui->pushButtonDelete->setEnabled(isSelectedItemCanCustomize());
}


void SARibbonCustomizeWidget::onTreeViewResultClicked(const QModelIndex& index)
{
    Q_UNUSED(index);
    //每次点击，判断是否可以进行操作，决定pushButtonAdd和pushButtonDelete的显示状态
    QStandardItem *item = selectedItem();

    if (nullptr == item) {
        return;
    }
    ui->pushButtonAdd->setEnabled(selectedAction() && (itemLevel(item) > 0) && isItemCanCustomize(item));
    ui->pushButtonDelete->setEnabled(isItemCanCustomize(item));
}


void SARibbonCustomizeWidget::onToolButtonUpClicked()
{
}


void SARibbonCustomizeWidget::onToolButtonDownClicked()
{
}
