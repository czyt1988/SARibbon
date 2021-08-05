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
    QMap<int, QString> mTagToName;                  ///< tag对应的名字
    QHash<QString, QAction *> mKeyToAction;         ///< key对应action
    QMap<QAction *, QString> mActionToKey;          ///< action对应key
    QMap<int, SARibbonCategory *> mTagToCategory;   ///< 仅仅在autoRegisteActions函数会有用
    int mSale;                                      ///< 盐用于生成固定的id，在用户不主动设置key时，id基于msale生成，只要SARibbonActionsManager的调用registeAction顺序不变，生成的id都不变，因为它是基于自增实现的
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
 * @brief 移除tag
 * @note 注意，这个函数非常耗时
 * @param tag
 */
void SARibbonActionsManager::removeTag(int tag)
{
    QList<QAction *> oldacts = actions(tag);

    //开始移除
    m_d->mTagToActions.remove(tag);
    m_d->mTagToName.remove(tag);
    //开始查找需要移出总表的action
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
    //从总表移除action
    for (QAction *a:needRemoveAct)
    {
        auto i = m_d->mActionToKey.find(a);
        if (i != m_d->mActionToKey.end()) {
            m_d->mKeyToAction.remove(i.value());
            m_d->mActionToKey.erase(i);
        }
    }
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
    //记录tag 对 action
    bool isneedemit = !(m_d->mTagToActions.contains(tag));//记录是否需要发射信号

    m_d->mTagToActions[tag].append(act);
    //绑定槽
    connect(act, &QObject::destroyed, this, &SARibbonActionsManager::onActionDestroyed);
    if (isneedemit && enableEmit) {
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

    for (auto i = m_d->mTagToActions.begin(); i != m_d->mTagToActions.end(); ++i)
    {
        //把不是act的内容转移到tagToActions和tagToActionKeys中，之后再和m_d里的替换
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
            //说明这个tag没有内容
            tagToActions.erase(tmpi);
            deletedTags.append(i.key());
        }
    }
    //删除mKeyToAction
    QString key = m_d->mActionToKey.value(act);

    m_d->mActionToKey.remove(act);
    m_d->mKeyToAction.remove(key);

    //置换
    m_d->mTagToActions.swap(tagToActions);
    //发射信号
    if (enableEmit) {
        for (int tagdelete : deletedTags)
        {
            emit actionTagChanged(tagdelete, true);
        }
    }
}


/**
 * @brief 等同actions
 * @param tag
 * @return
 */
QList<QAction *>& SARibbonActionsManager::filter(int tag)
{
    return (actions(tag));
}


/**
 * @brief 根据tag得到actions
 * @param tag
 * @return
 */
QList<QAction *>& SARibbonActionsManager::actions(int tag)
{
    return (m_d->mTagToActions[tag]);
}


const QList<QAction *> SARibbonActionsManager::actions(int tag) const
{
    return (m_d->mTagToActions[tag]);
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
 * @brief 通过action找到key
 * @param act
 * @return 如果找不到，返回QString()
 */
QString SARibbonActionsManager::key(QAction *act) const
{
    return (m_d->mActionToKey.value(act, QString()));
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
 * @brief 返回所有管理的actions
 * @return
 */
QList<QAction *> SARibbonActionsManager::allActions() const
{
    return (m_d->mKeyToAction.values());
}


/**
 * @brief 自动加载SARibbonMainWindow的action
 * 此函数会遍历@ref SARibbonMainWindow 下的所有子object，找到action注册，
 * 并会遍历所有@ref SARibbonCategory,把SARibbonCategory下的action按SARibbonCategory的title name进行分类
 *
 * 此函数会把所有category下的action生成tag并注册，返回的QMap<int, SARibbonCategory *>是记录了category对应的tag
 *
 * 此函数还会把SARibbonMainWindow下面的action，但不在任何一个category下的作为NotInRibbonCategoryTag标签注册，默认名字会赋予not in ribbon，
 * 可以通过@ref setTagName 改变
 *
 * @param w
 * @return
 * @note 此函数的调用最好在category设置了标题后调用，因为会以category的标题作为标签的命名
 */
QMap<int, SARibbonCategory *> SARibbonActionsManager::autoRegisteActions(SARibbonMainWindow *w)
{
    QMap<int, SARibbonCategory *> res;
    //先遍历SARibbonMainWindow下的所有子对象，把所有action找到
    QSet<QAction *> mainwindowActions;

    for (QObject *o : w->children())
    {
        if (QAction *a = qobject_cast<QAction *>(o)) {
            //说明是action
            if (!a->objectName().isEmpty()) {
                mainwindowActions.insert(a);
            }
        }
    }
    //开始遍历每个category，加入action
    SARibbonBar *bar = w->ribbonBar();

    if (nullptr == bar) {
        //非ribbon模式，直接退出
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
    //找到不在功能区的actions
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


/**
 * @brief 自动加载action
 * @param w
 * @param tag
 * @param enableEmit
 * @return 返回成功加入SARibbonActionsManager管理的action
 */
QSet<QAction *> SARibbonActionsManager::autoRegisteWidgetActions(QWidget *w, int tag, bool enableEmit)
{
    QSet<QAction *> res;
    QList<QAction *> was = w->actions();

    for (QAction *a : was)
    {
        if (res.contains(a) || a->objectName().isEmpty()) {
            //重复内容不重复加入
            //没有object name不加入
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


/**
 * @brief autoRegisteActions函数会关联此槽，在标签内容改变时改变tag 对应 文本
 * @param title
 */
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
    }else{
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


/**
 * @brief 查找
 * @param text
 */
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
    }else{
        if (tag == m_d->mTag) {
            update();
        }
    }
}
