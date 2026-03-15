#include "SARibbonActionsManager.h"
#include <QMap>
#include <QHash>
#include <QDebug>
#include <QWidgetAction>
#include "SARibbonBar.h"

class SARibbonActionsManager::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonActionsManager)
public:
    PrivateData(SARibbonActionsManager* p);
    void clear();

    QMap< int, QList< QAction* > > mTagToActions;   ///< tag : QList<QAction*>
    QMap< int, QString > mTagToName;                ///< tag对应的名字
    QHash< QString, QAction* > mKeyToAction;        ///< key对应action
    QMap< QAction*, QString > mActionToKey;         ///< action对应key
    QMap< int, SARibbonCategory* > mTagToCategory;  ///< 仅仅在autoRegisteActions函数会有用
    int mSale;  ///< 盐用于生成固定的id，在用户不主动设置key时，id基于msale生成，只要SARibbonActionsManager的调用registeAction顺序不变，生成的id都不变，因为它是基于自增实现的
};

SARibbonActionsManager::PrivateData::PrivateData(SARibbonActionsManager* p) : q_ptr(p), mSale(0)
{
}

void SARibbonActionsManager::PrivateData::clear()
{
    mTagToActions.clear();
    mTagToName.clear();
    mKeyToAction.clear();
    mActionToKey.clear();
    mTagToCategory.clear();
    mSale = 0;
}

/**
 * \if ENGLISH
 * @brief Constructor
 * @param bar SARibbonBar pointer
 * \endif
 *
 * \if CHINESE
 * @brief 构造函数
 * @param bar SARibbonBar指针
 * \endif
 */
SARibbonActionsManager::SARibbonActionsManager(SARibbonBar* bar)
    : QObject(bar), d_ptr(new SARibbonActionsManager::PrivateData(this))
{
    autoRegisteActions(bar);
}

/**
 * \if ENGLISH
 * @brief Destructor
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * \endif
 */
SARibbonActionsManager::~SARibbonActionsManager()
{
}

/**
 * \if ENGLISH
 * @brief Set the name corresponding to the tag, this allows getting the mapping between tag and text
 * @param tag Tag value
 * @param name Tag name
 * @note In multi-language environments, this needs to be reset when language changes
 * \endif
 *
 * \if CHINESE
 * @brief 设置tag对应的名字，通过这个可以得到tag和文本的映射
 * @param tag 标签值
 * @param name 标签名
 * @note 在支持多语言的环境下，在语言切换时需要重新设置，以更新名字
 * \endif
 */
void SARibbonActionsManager::setTagName(int tag, const QString& name)
{
    d_ptr->mTagToName[ tag ] = name;
}

/**
 * \if ENGLISH
 * @brief Get the name corresponding to the tag
 * @param tag Tag value
 * @return Tag name
 * \endif
 *
 * \if CHINESE
 * @brief 获取tag对应的名字
 * @param tag 标签值
 * @return 标签名
 * \endif
 */
QString SARibbonActionsManager::tagName(int tag) const
{
    return (d_ptr->mTagToName.value(tag, ""));
}

/**
 * \if ENGLISH
 * @brief Remove tag
 * @param tag Tag to remove
 * @note This function is time-consuming
 * \endif
 *
 * \if CHINESE
 * @brief 移除tag
 * @param tag 要移除的标签
 * @note 注意，这个函数非常耗时
 * \endif
 */
void SARibbonActionsManager::removeTag(int tag)
{
    QList< QAction* > oldacts = actions(tag);

    // 开始移除
    d_ptr->mTagToActions.remove(tag);
    d_ptr->mTagToName.remove(tag);
    // 开始查找需要移出总表的action
    QList< QAction* > needRemoveAct;
    QList< QAction* > total;

    for (auto i = d_ptr->mTagToActions.begin(); i != d_ptr->mTagToActions.end(); ++i) {
        total += i.value();
    }
    for (QAction* a : sa_as_const(oldacts)) {
        if (!total.contains(a)) {
            needRemoveAct.append(a);
        }
    }
    // 从总表移除action
    for (QAction* a : sa_as_const(needRemoveAct)) {
        auto i = d_ptr->mActionToKey.find(a);
        if (i != d_ptr->mActionToKey.end()) {
            d_ptr->mKeyToAction.remove(i.value());
            d_ptr->mActionToKey.erase(i);
        }
    }
}

/**
 * \if ENGLISH
 * @brief Register action to the manager
 * @param act Action to register
 * @param tag Tag can be combined by bitwise OR, see @ref ActionTag.
 * For custom tags, it's recommended to use values greater than @ref ActionTag::UserDefineActionTag
 * @param key Key for the action, one key corresponds to one action. Default is QString(), which uses QAction's objectName
 * @param enableEmit Whether to emit @ref actionTagChanged signal
 * @return true if registration succeeded
 * @note Same action registered with different tags can be found by tag, but only the last registered tag can be found by action
 * @note New tag will trigger actionTagChanged signal
 * \endif
 *
 * \if CHINESE
 * @brief 把action注册到管理器中，实现action的管理
 * @param act 要注册的action
 * @param tag tag是可以按照位进行叠加，见 @ref ActionTag 如果
 * 要定义自己的标签，建议定义大于@ref ActionTag::UserDefineActionTag 的值，
 * registeAction的tag是直接记录进去的，如果要多个标签并存，在registe之前先或好tag
 * @param key key是action对应的key，一个key只对应一个action，是查找action的关键
 * ,默认情况为一个QString(),这时key是QAction的objectName
 * @param enableEmit 控制是否发射@ref actionTagChanged 信号
 * @return 注册成功返回true
 * @note 同一个action多次注册不同的tag可以通过tag索引到action，但通过action只能索引到最后一个注册的tag
 * @note tag的新增会触发actionTagChanged信号
 * \endif
 */
bool SARibbonActionsManager::registeAction(QAction* act, int tag, const QString& key, bool enableEmit)
{
    if (nullptr == act) {
        return (false);
    }
    QString k = key;

    if (k.isEmpty()) {
        k = QString("id_%1_%2").arg(d_ptr->mSale++).arg(act->objectName());
    }
    if (d_ptr->mKeyToAction.contains(k)) {
        qWarning() << "key: "
                   << k << " have been exist,you can set key in an unique value when use SARibbonActionsManager::registeAction";
        return (false);
    }
    d_ptr->mKeyToAction[ k ]   = act;
    d_ptr->mActionToKey[ act ] = k;
    // 记录tag 对 action
    bool isneedemit = !(d_ptr->mTagToActions.contains(tag));  // 记录是否需要发射信号
    d_ptr->mTagToActions[ tag ].append(act);
    // 绑定槽
    connect(act, &QObject::destroyed, this, &SARibbonActionsManager::onActionDestroyed);
    if (isneedemit && enableEmit) {
        // 说明新增tag
        Q_EMIT actionTagChanged(tag, false);
    }
    return (true);
}

/**
 * \if ENGLISH
 * @brief Unregister action
 *
 * If the last action corresponding to a tag is removed, the tag will also be deleted
 * @param act Action to unregister
 * @param enableEmit Whether to emit signal
 * @note Tag deletion will trigger actionTagChanged signal
 * @note If action is associated with multiple tags, the action will be removed from all tags, same for key
 * \endif
 *
 * \if CHINESE
 * @brief 取消action的注册
 *
 * 如果tag对应的最后一个action被撤销，tag也将一块删除
 * @param act 要取消注册的action
 * @param enableEmit 是否发射信号
 * @note tag的删除会触发actionTagChanged信号
 * @note 如果action关联了多个tag，这些tag里的action都会被删除，对应的key也同理
 * \endif
 */
void SARibbonActionsManager::unregisteAction(QAction* act, bool enableEmit)
{
    if (nullptr == act) {
        return;
    }
    // 绑定槽
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
void SARibbonActionsManager::removeAction(QAction* act, bool enableEmit)
{
    QList< int > deletedTags;                     // 记录删除的tag，用于触发actionTagChanged
    QMap< int, QList< QAction* > > tagToActions;  ///< tag : QList<QAction*>

    for (auto i = d_ptr->mTagToActions.begin(); i != d_ptr->mTagToActions.end(); ++i) {
        // 把不是act的内容转移到tagToActions和tagToActionKeys中，之后再和m_d里的替换
        auto tmpi = tagToActions.insert(i.key(), QList< QAction* >());
        int count = 0;
        for (int j = 0; j < i.value().size(); ++j) {
            if (i.value()[ j ] != act) {
                tmpi.value().append(act);
                ++count;
            }
        }
        if (0 == count) {
            // 说明这个tag没有内容
            tagToActions.erase(tmpi);
            deletedTags.append(i.key());
        }
    }
    // 删除mKeyToAction
    QString key = d_ptr->mActionToKey.value(act);

    d_ptr->mActionToKey.remove(act);
    d_ptr->mKeyToAction.remove(key);

    // 置换
    d_ptr->mTagToActions.swap(tagToActions);
    // 发射信号
    if (enableEmit) {
        for (int tagdelete : sa_as_const(deletedTags)) {
            Q_EMIT actionTagChanged(tagdelete, true);
        }
    }
}

/**
 * \if ENGLISH
 * @brief Equivalent to actions function
 * @param tag Action tag
 * @return Reference to action list for the tag
 * \endif
 *
 * \if CHINESE
 * @brief 等同actions函数
 * @param tag 动作标签
 * @return 对应标签的动作列表引用
 * \endif
 */
QList< QAction* >& SARibbonActionsManager::filter(int tag)
{
    return (actions(tag));
}

/**
 * \if ENGLISH
 * @brief Get actions by tag
 * @param tag Action tag
 * @return Reference to action list for the tag
 * \endif
 *
 * \if CHINESE
 * @brief 根据tag得到actions
 * @param tag 动作标签
 * @return 对应标签的动作列表引用
 * \endif
 */
QList< QAction* >& SARibbonActionsManager::actions(int tag)
{
    return (d_ptr->mTagToActions[ tag ]);
}

/**
 * \if ENGLISH
 * @brief Get actions by tag (const version)
 * @param tag Action tag
 * @return Action list for the tag
 * \endif
 *
 * \if CHINESE
 * @brief 根据tag得到actions（常量版本）
 * @param tag 动作标签
 * @return 对应标签的动作列表
 * \endif
 */
const QList< QAction* > SARibbonActionsManager::actions(int tag) const
{
    return (d_ptr->mTagToActions[ tag ]);
}

/**
 * \if ENGLISH
 * @brief Get all tags
 * @return List of all action tags
 * \endif
 *
 * \if CHINESE
 * @brief 获取所有的标签
 * @return 所有动作标签的列表
 * \endif
 */
QList< int > SARibbonActionsManager::actionTags() const
{
    return (d_ptr->mTagToActions.keys());
}

/**
 * \if ENGLISH
 * @brief Get action by key
 * @param key Action key
 * @return Action pointer, returns nullptr if key not found
 * \endif
 *
 * \if CHINESE
 * @brief 通过key获取action
 * @param key 动作键
 * @return 动作指针，如果没有key，返回nullptr
 * \endif
 */
QAction* SARibbonActionsManager::action(const QString& key) const
{
    return (d_ptr->mKeyToAction.value(key, nullptr));
}

/**
 * \if ENGLISH
 * @brief Get key by action
 * @param act Action pointer
 * @return Action key, returns empty QString if action not found
 * \endif
 *
 * \if CHINESE
 * @brief 通过action找到key
 * @param act 动作指针
 * @return 动作键，如果找不到，返回QString()
 * \endif
 */
QString SARibbonActionsManager::key(QAction* act) const
{
    return (d_ptr->mActionToKey.value(act, QString()));
}

/**
 * \if ENGLISH
 * @brief Get count of all managed actions
 * @return Number of managed actions
 * \endif
 *
 * \if CHINESE
 * @brief 返回所有管理的action数
 * @return 管理的动作数量
 * \endif
 */
int SARibbonActionsManager::count() const
{
    return (d_ptr->mKeyToAction.size());
}

/**
 * \if ENGLISH
 * @brief Get all managed actions
 * @return List of all managed actions
 * \endif
 *
 * \if CHINESE
 * @brief 返回所有管理的actions
 * @return 所有管理的动作列表
 * \endif
 */
QList< QAction* > SARibbonActionsManager::allActions() const
{
    return (d_ptr->mKeyToAction.values());
}

/**
 * @brief 自动加载SARibbonBar的action
 * 此函数会遍历@ref SARibbonBar的父窗口(一般是SARibbonMainWindow)下的所有子object，找到action注册，
 * 并会遍历所有@ref SARibbonCategory,把SARibbonCategory下的action按SARibbonCategory的title name进行分类
 *
 * 此函数会把所有category下的action生成tag并注册，返回的QMap<int, SARibbonCategory *>是记录了category对应的tag
 *
 * 此函数还会把SARibbonBar的父窗口(一般是SARibbonMainWindow)下面的action，但不在任何一个category下的作为NotInRibbonCategoryTag标签注册，默认名字会赋予not
 * in ribbon， 可以通过@ref setTagName 改变
 *
 * @param w
 * @return
 * @note 此函数的调用最好在category设置了标题后调用，因为会以category的标题作为标签的命名
 */
QMap< int, SARibbonCategory* > SARibbonActionsManager::autoRegisteActions(SARibbonBar* bar)
{
    QMap< int, SARibbonCategory* > res;
    // 先遍历SARibbonBar的父窗口(一般是SARibbonMainWindow)下的所有子对象，把所有action找到
    QWidget* parWidget = bar->parentWidget();
    QSet< QAction* > mainwindowActions;
    if (parWidget) {
        for (QObject* o : sa_as_const(parWidget->children())) {
            if (QAction* a = qobject_cast< QAction* >(o)) {
                // 说明是action
                if (!a->objectName().isEmpty()) {
                    mainwindowActions.insert(a);
                }
            }
        }
    }
    // 开始遍历每个category，加入action

    if (nullptr == bar) {
        // 非ribbon模式，直接退出
        return (res);
    }
    QSet< QAction* > categoryActions;
    QList< SARibbonCategory* > categorys = bar->categoryPages();
    int tag                              = AutoCategoryDistinguishBeginTag;

    for (SARibbonCategory* c : sa_as_const(categorys)) {
        QList< SARibbonPanel* > panels = c->panelList();
        for (SARibbonPanel* p : sa_as_const(panels)) {
            categoryActions += autoRegisteWidgetActions(p, tag, false);
        }
        setTagName(tag, c->categoryName());
        res[ tag ] = c;
        ++tag;
    }
    // 找到不在功能区的actions
    QSet< QAction* > notincategory = mainwindowActions - categoryActions;

    for (QAction* a : sa_as_const(notincategory)) {
        if (!a->objectName().isEmpty()) {
            registeAction(a, NotInRibbonCategoryTag, a->objectName(), false);
        }
    }
    if (notincategory.size() > 0) {
        setTagName(NotInRibbonCategoryTag, tr("not in ribbon"));
    }
    for (auto i = res.begin(); i != res.end(); ++i) {
        connect(i.value(), &SARibbonCategory::categoryNameChanged, this, &SARibbonActionsManager::onCategoryTitleChanged);
    }
    d_ptr->mTagToCategory = res;
    return (res);
}

/**
 * @brief 自动加载action
 * @param w
 * @param tag
 * @param enableEmit
 * @return 返回成功加入SARibbonActionsManager管理的action
 */
QSet< QAction* > SARibbonActionsManager::autoRegisteWidgetActions(QWidget* w, int tag, bool enableEmit)
{
    QSet< QAction* > res;
    QList< QAction* > was = w->actions();

    for (QAction* a : sa_as_const(was)) {
        if (res.contains(a) || a->objectName().isEmpty()) {
            // 重复内容不重复加入
            // 没有object name不加入
            continue;
        }
        if (registeAction(a, tag, a->objectName(), enableEmit)) {
            res.insert(a);
        }
    }
    return (res);
}

/**
 * @brief 根据标题查找action
 * @param text
 * @return
 */
QList< QAction* > SARibbonActionsManager::search(const QString& text)
{
    QList< QAction* > res;

    if (text.isEmpty()) {
        return (res);
    }
    QStringList kws = text.split(" ");

    if (kws.isEmpty()) {
        kws.append(text);
    }

    for (const QString& k : sa_as_const(kws)) {
        for (auto i = d_ptr->mActionToKey.begin(); i != d_ptr->mActionToKey.end(); ++i) {
            if (i.key()->text().contains(k, Qt::CaseInsensitive)) {
                res.append(i.key());
            }
        }
    }
    return (res);
}

void SARibbonActionsManager::clear()
{
    d_ptr->clear();
}

/**
 * @brief action 被delete时，将触发此槽把管理的action删除
 * @param o
 * @note 这个函数不会触发actionTagChanged信号
 */
void SARibbonActionsManager::onActionDestroyed(QObject* o)
{
    QAction* act = static_cast< QAction* >(o);

    removeAction(act, false);
}

/**
 * @brief autoRegisteActions函数会关联此槽，在标签内容改变时改变tag 对应 文本
 * @param title
 */
void SARibbonActionsManager::onCategoryTitleChanged(const QString& title)
{
    SARibbonCategory* c = qobject_cast< SARibbonCategory* >(sender());

    if (nullptr == c) {
        return;
    }
    int tag = d_ptr->mTagToCategory.key(c, -1);

    if (tag == -1) {
        return;
    }
    setTagName(tag, title);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SARibbonActionsModel
////////////////////////////////////////////////////////////////////////////////////////////////////////

class SARibbonActionsManagerModel::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonActionsManagerModel)
public:
    PrivateData(SARibbonActionsManagerModel* p);
    void updateRef();
    int count() const;
    QAction* at(int index);
    bool isNull() const;

public:
    SARibbonActionsManager* mMgr { nullptr };
    int mTag { SARibbonActionsManager::CommonlyUsedActionTag };
    QString mSeatchText;
    QList< QAction* > mActions;
};

SARibbonActionsManagerModel::PrivateData::PrivateData(SARibbonActionsManagerModel* p) : q_ptr(p)
{
}

void SARibbonActionsManagerModel::PrivateData::updateRef()
{
    if (isNull()) {
        return;
    }
    if (!mSeatchText.isEmpty()) {
        mActions = mMgr->search(mSeatchText);
    } else {
        mActions = mMgr->actions(mTag);
    }
}

int SARibbonActionsManagerModel::PrivateData::count() const
{
    if (isNull()) {
        return (0);
    }
    return (mActions.size());
}

QAction* SARibbonActionsManagerModel::PrivateData::at(int index)
{
    if (isNull()) {
        return (nullptr);
    }
    if (index >= count()) {
        return (nullptr);
    }
    return (mActions.at(index));
}

bool SARibbonActionsManagerModel::PrivateData::isNull() const
{
    return (mMgr == nullptr);
}

//===================================================
// SARibbonActionsManagerModel
//===================================================

SARibbonActionsManagerModel::SARibbonActionsManagerModel(QObject* p)
    : QAbstractListModel(p), d_ptr(new SARibbonActionsManagerModel::PrivateData(this))
{
}

SARibbonActionsManagerModel::SARibbonActionsManagerModel(SARibbonActionsManager* m, QObject* p)
    : QAbstractListModel(p), d_ptr(new SARibbonActionsManagerModel::PrivateData(this))
{
    setupActionsManager(m);
}

SARibbonActionsManagerModel::~SARibbonActionsManagerModel()
{
}

int SARibbonActionsManagerModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {  // 非顶层
        return (0);
    }
    // 顶层
    return (d_ptr->count());
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
    QAction* act = indexToAction(index);

    if (nullptr == act) {
        return (QVariant());
    }

    switch (role) {
    case Qt::DisplayRole: {
        QString str = act->text();
        if (!str.isEmpty()) {
            return str;
        }
        // 如果文本为空，一般是没有设置文本，或者是窗口，如果是窗口，那么用windowtitle作为文本
        if (QWidgetAction* wa = qobject_cast< QWidgetAction* >(act)) {
            if (QWidget* w = wa->defaultWidget()) {
                str = w->windowTitle();
            }
        }
        return str;
    }
    case Qt::DecorationRole:
        return (act->icon());

    default:
        break;
    }
    return (QVariant());
}

void SARibbonActionsManagerModel::setFilter(int tag)
{
    d_ptr->mTag = tag;
    update();
}

void SARibbonActionsManagerModel::update()
{
    beginResetModel();
    d_ptr->updateRef();
    endResetModel();
}

void SARibbonActionsManagerModel::setupActionsManager(SARibbonActionsManager* m)
{
    d_ptr->mMgr     = m;
    d_ptr->mTag     = SARibbonActionsManager::CommonlyUsedActionTag;
    d_ptr->mActions = m->filter(d_ptr->mTag);
    connect(m, &SARibbonActionsManager::actionTagChanged, this, &SARibbonActionsManagerModel::onActionTagChanged);
    update();
}

void SARibbonActionsManagerModel::uninstallActionsManager()
{
    if (!d_ptr->isNull()) {
        disconnect(
            d_ptr->mMgr, &SARibbonActionsManager::actionTagChanged, this, &SARibbonActionsManagerModel::onActionTagChanged);
        d_ptr->mMgr = nullptr;
        d_ptr->mTag = SARibbonActionsManager::CommonlyUsedActionTag;
    }
    update();
}

QAction* SARibbonActionsManagerModel::indexToAction(QModelIndex index) const
{
    if (!index.isValid()) {
        return (nullptr);
    }
    if (index.row() >= d_ptr->count()) {
        return (nullptr);
    }
    return (d_ptr->at(index.row()));
}

/**
 * @brief 查找
 * @param text
 */
void SARibbonActionsManagerModel::search(const QString& text)
{
    d_ptr->mSeatchText = text;
    update();
}

void SARibbonActionsManagerModel::onActionTagChanged(int tag, bool isdelete)
{
    if (isdelete && (tag == d_ptr->mTag)) {
        d_ptr->mTag = SARibbonActionsManager::UnknowActionTag;
        update();
    } else {
        if (tag == d_ptr->mTag) {
            update();
        }
    }
}
