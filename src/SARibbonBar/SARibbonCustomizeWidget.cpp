#include "SARibbonCustomizeWidget.h"
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
////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SARibbonActionsManager
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <QMap>
#include <QSet>

class SARibbonActionsManagerPrivate
{
public:
    SARibbonActionsManager *mParent;
    QMap<int, QList<QAction *> > mTagToActions;     ///< tag : QSet<QAction*>
    QMap<int, QString> mTagToName;                  ///< tag对应的名字
    SARibbonActionsManagerPrivate(SARibbonActionsManager *p);
};

SARibbonActionsManagerPrivate::SARibbonActionsManagerPrivate(SARibbonActionsManager *p)
    : mParent(p)
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
 * @note 同一个action多次注册不同的tag可以通过tag索引到action，但通过action只能索引到最后一个注册的tag
 * @note tag的新增会触发actionTagChanged信号
 */
void SARibbonActionsManager::registeAction(QAction *act, int tag)
{
    if (nullptr == act) {
        return;
    }
    //记录tag 对 action
    bool isneedemit = !(m_d->mTagToActions.contains(tag));//记录是否需要发射信号

    m_d->mTagToActions[tag].append(act);
    //绑定槽
    connect(act, &QObject::destroyed, this, &SARibbonActionsManager::onActionDestroyed);
    if (isneedemit) {
        //说明新增tag
        emit actionTagChanged(tag, false);
    }
}


/**
 * @brief 取消action的注册
 *
 * 如果tag对应的最后一个action被撤销，tag也将一块删除
 * @param act
 * @note tag的删除会触发actionTagChanged信号
 */
void SARibbonActionsManager::unregisteAction(QAction *act)
{
    if (nullptr == act) {
        return;
    }
    //绑定槽
    disconnect(act, &QObject::destroyed, this, &SARibbonActionsManager::onActionDestroyed);
    auto i = m_d->mTagToActions.begin();

    while (i != m_d->mTagToActions.end())
    {
        while (i.value().removeOne(act))
        {
            //把所有都删除一直到找不到为止
        }
        if (i.value().size() > 0) {
            ++i;
        }else{
            //如果tag对应的actions空了，顺便把tag也删除了
            i = m_d->mTagToActions.erase(i);
            emit actionTagChanged(i.key(), true);
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
 * @brief action 被delete时，将触发此槽把管理的action删除
 * @param o
 * @note 这个函数不会触发actionTagChanged信号
 */
void SARibbonActionsManager::onActionDestroyed(QObject *o)
{
    QAction *act = static_cast<QAction *>(o);

    auto i = m_d->mTagToActions.begin();

    while (i != m_d->mTagToActions.end())
    {
        while (i.value().removeOne(act))
        {
            //把所有都删除一直到找不到为止
        }
        if (i.value().size() > 0) {
            ++i;
        }else{
            //如果tag对应的actions空了，顺便把tag也删除了
            i = m_d->mTagToActions.erase(i);
        }
    }
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
    if (!index.isValid()) {
        return (QVariant());
    }
    if (index.row() >= m_d->count()) {
        return (QVariant());
    }
    QAction *act = m_d->at(index.row());

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

        verticalLayoutMidButtons->addWidget(pushButtonAdd);

        pushButtonDelete = new QPushButton(customizeWidget);
        pushButtonDelete->setObjectName(QStringLiteral("pushButtonDelete"));

        verticalLayoutMidButtons->addWidget(pushButtonDelete);

        verticalSpacerDown = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayoutMidButtons->addItem(verticalSpacerDown);


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
    SARibbonCustomizeWidgetPrivate(SARibbonCustomizeWidget *p);
    void updateModel();
};

SARibbonCustomizeWidgetPrivate::SARibbonCustomizeWidgetPrivate(SARibbonCustomizeWidget *p)
    : mParent(p)
    , mShowType(SARibbonCustomizeWidget::ShowAllCategory)
    , mRibbonWindow(nullptr)
    , mIsChanged(false)
    , mActionMgr(nullptr)
{
    mAcionModel = new SARibbonActionsManagerModel(p);
    mRibbonModel = new QStandardItemModel(p);
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
 * @brief 把定义的内容转换为xml
 * @return xml
 */
QString SARibbonCustomizeWidget::toXml() const
{
    //TODO
    return (QString());
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
    QStandardItem *ni = new QStandardItem(tr("new category[customize]"));

    ni->setData(0, SARibbonCustomizeWidget::LevelRole);
    ni->setData(true, SARibbonCustomizeWidget::CustomizeRole);
    m_d->mRibbonModel->insertRow(row, ni);
    ui->treeViewResult->scrollTo(ni->index());
    //设置新增的为选中
    if (m) {
        m->select(ni->index(), QItemSelectionModel::ClearAndSelect);
    }
}


void SARibbonCustomizeWidget::onPushButtonNewPannelClicked()
{
    QItemSelectionModel *m = ui->treeViewResult->selectionModel();

    if ((nullptr == m) || !m->hasSelection()) {
        return;
    }
    QModelIndex i = m->currentIndex();
    QStandardItem *item = m_d->mRibbonModel->itemFromIndex(i);
    int level = item->data(SARibbonCustomizeWidget::LevelRole).toInt();

    QStandardItem *ni = new QStandardItem(tr("new pannel[customize]"));

    if (0 == level) {
        //说明是category,插入到最后
        ni->setData(1, SARibbonCustomizeWidget::LevelRole);
        ni->setData(true, SARibbonCustomizeWidget::CustomizeRole);
        item->appendRow(ni);
    }else if (1 == level) {
        //说明选择的是pannel，插入到此pannel之后
        QStandardItem *categoryItem = item->parent();
        if (nullptr == categoryItem) {
            return;
        }
        ni->setData(1, SARibbonCustomizeWidget::LevelRole);
        ni->setData(true, SARibbonCustomizeWidget::CustomizeRole);
        categoryItem->insertRow(item->row()+1, ni);
    }else{
        delete ni;
    }
    ui->treeViewResult->scrollTo(ni->index());
    if (m) {
        m->clearSelection();
        m->select(ni->index(), QItemSelectionModel::Select);
        ui->treeViewResult->setSelectionModel(m);
    }
}


void SARibbonCustomizeWidget::onPushButtonRenameClicked()
{
    QItemSelectionModel *m = ui->treeViewResult->selectionModel();
    QString text = "";

    if ((nullptr == m) || !m->hasSelection()) {
        return;
    }
    QModelIndex i = m->currentIndex();

    text = i.data().toString();
    bool ok;

    text = QInputDialog::getText(this, tr("rename"),
        tr("name:"), QLineEdit::Normal,
        text, &ok);

    if (ok && !text.isEmpty()) {
        QStandardItem *item = m_d->mRibbonModel->itemFromIndex(i);
        if (item) {
            item->setText(text);
        }
    }
}
