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
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

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
    QMap<QAction *, int> mActionToTags;             ///< QAction:tags
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
    m_d->mActionToTags[act] = tag;
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
    m_d->mActionToTags.remove(act);
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
 * @brief action 被delete时，将触发此槽把管理的action删除
 * @param o
 * @note 这个函数不会触发actionTagChanged信号
 */
void SARibbonActionsManager::onActionDestroyed(QObject *o)
{
    QAction *act = static_cast<QAction *>(o);

    m_d->mActionToTags.remove(act);
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
    SARibbonActionsModel *mParent;
    SARibbonActionsManager *mMgr;
    int mTag;
    SARibbonActionsManager::ActionRef mActionRef;
    SARibbonActionsModelPrivete(SARibbonActionsModel *m);
    bool isValidRef() const;
    void updateRef();
    int count() const;
    QAction *at(int index);
};

SARibbonActionsModelPrivete::SARibbonActionsModelPrivete(SARibbonActionsModel *m)
    : mParent(m)
{
}


bool SARibbonActionsModelPrivete::isValidRef() const
{
    return (mMgr->isActionRefsValid(mActionRef));
}


void SARibbonActionsModelPrivete::updateRef()
{
    mActionRef = mMgr->filter(mTag);
}


int SARibbonActionsModelPrivete::count() const
{
    if (isValidRef()) {
        return (mActionRef.value().size());
    }
    return (0);
}


QAction *SARibbonActionsModelPrivete::at(int index)
{
    if (!isValidRef()) {
        return (nullptr);
    }
    if (index >= count()) {
        return (nullptr);
    }
    return (mActionRef.value().at(index));
}


SARibbonActionsModel::SARibbonActionsModel(SARibbonActionsManager *m, QObject *p) : QAbstractItemModel(p)
    , m_d(new SARibbonActionsModelPrivete(this))
{
    m_d->mMgr = m;
    m_d->mTag = SARibbonActionsManager::CommonlyUsedActionTag;
    m_d->mActionRef = m->filter(m_d->mTag);
    connect(m, &SARibbonActionsManager::actionTagChanged
        , this, &SARibbonActionsModel::onActionTagChanged);
}


SARibbonActionsModel::~SARibbonActionsModel()
{
    delete m_d;
}


QModelIndex SARibbonActionsModel::index(int row, int column, const QModelIndex& parent) const
{
    if ((row < 0) || (column < 0) || parent.isValid()) {
        return (QModelIndex());
    }
    if (column >= 1) {//只能有一列，大于的都是异常
        return (QModelIndex());
    }
    QAction *act = m_d->at(row);

    //范围内
    if (act) {
        return (createIndex(row, column, act));
    }
    return (QModelIndex());
}


QModelIndex SARibbonActionsModel::parent(const QModelIndex& index) const
{
    Q_UNUSED(index);
    //列表无层级
    return (QModelIndex());
}


int SARibbonActionsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {//非顶层
        return (0);
    }
    //顶层
    return (m_d->count());
}


int SARibbonActionsModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {//非顶层
        return (0);
    }
    //顶层
    return (1);
}


QVariant SARibbonActionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return (QVariant());
    }
    if (Qt::Horizontal == orientation) {
        return (tr("action name"));
    }
    return (QVariant());
}


Qt::ItemFlags SARibbonActionsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return (Qt::NoItemFlags);
    }
    return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}


QVariant SARibbonActionsModel::data(const QModelIndex& index, int role) const
{
}


void SARibbonActionsModel::setFilter(int tag)
{
    m_d->mTag = tag;
    update();
}


void SARibbonActionsModel::update()
{
    beginResetModel();
    m_d->updateRef();
    endResetModel();
}


void SARibbonActionsModel::onActionTagChanged(int tag, bool isdelete)
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
    QTreeView *treeViewSelect;
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

        treeViewSelect = new QTreeView(customizeWidget);
        treeViewSelect->setObjectName(QStringLiteral("treeViewSelect"));

        verticalLayoutSelect->addWidget(treeViewSelect);


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
        radioButtonMainCategory->setChecked(true);

        horizontalLayoutCategorySelect->addWidget(radioButtonMainCategory);

        radioButtonAllCategory = new QRadioButton(customizeWidget);
        radioButtonAllCategory->setObjectName(QStringLiteral("radioButtonAllCategory"));

        horizontalLayoutCategorySelect->addWidget(radioButtonAllCategory);


        verticalLayoutResult->addLayout(horizontalLayoutCategorySelect);

        treeViewResult = new QTreeView(customizeWidget);
        treeViewResult->setObjectName(QStringLiteral("treeViewResult"));

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
    SARibbonMainWindow *mRibbonWindow;      ///< 保存SARibbonMainWindow的指针
    bool mIsChanged;                        ///< 判断用户是否有改动内容
    SARibbonActionsManager *mActionMgr;     ///< action管理器
    SARibbonCustomizeWidgetPrivate(SARibbonCustomizeWidget *p);
};

SARibbonCustomizeWidgetPrivate::SARibbonCustomizeWidgetPrivate(SARibbonCustomizeWidget *p)
    : mParent(p)
    , mRibbonWindow(nullptr)
    , mIsChanged(false)
{
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
    ui->setupUi(this);
    m_d->mRibbonWindow = ribbonWindow;
}


SARibbonCustomizeWidget::~SARibbonCustomizeWidget()
{
    delete ui;
    delete m_d;
}


/**
 * @brief 设置action管理器
 * @param mgr
 */
void SARibbonCustomizeWidget::setupActionsManager(SARibbonActionsManager *mgr)
{
    m_d->mActionMgr = mgr;
}


bool SARibbonCustomizeWidget::isChanged() const
{
    return (m_d->mIsChanged);
}


/**
 * @brief 把定义的内容转换为xml
 * @return xml
 */
QString SARibbonCustomizeWidget::toXml() const
{
    //TODO
}
