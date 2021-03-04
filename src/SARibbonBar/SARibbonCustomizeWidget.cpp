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
#include "SARibbonCustomizeData.h"
#include "SARibbonBar.h"

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
    } // setupUi


    void retranslateUi(QWidget *customizeWidget)
    {
        customizeWidget->setWindowTitle(QApplication::translate("SARibbonCustomizeWidget", "Customize Widget", Q_NULLPTR));
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
    bool isCustomizeItem(QStandardItem *item) const;

    //把item转换为category
    SARibbonCategory *itemToCategory(QStandardItem *item) const;

    //把item转换为SARibbonPannel
    SARibbonPannel *itemToPannel(QStandardItem *item) const;

    //获取item对应的object name
    QString itemObjectName(QStandardItem *item) const;

    //判断是否可以自定义
    bool isItemCanCustomize(QStandardItem *item) const;

    //从item转为action
    QAction *itemToAction(QStandardItem *item) const;
};

SARibbonCustomizeWidgetPrivate::SARibbonCustomizeWidgetPrivate(SARibbonCustomizeWidget *p)
    : mParent(p)
    , mShowType(SARibbonCustomizeWidget::ShowAllCategory)
    , mRibbonWindow(nullptr)
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
        if (c->isCanCustomize() && !c->isContextCategory()) {
            //上下文标签不做显示隐藏处理
            ci->setCheckable(true);
            ci->setCheckState(ribbonbar->isCategoryVisible(c) ? Qt::Checked : Qt::Unchecked);
            ci->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);//标记这个是可以自定义的
        }
        ci->setData(0, SARibbonCustomizeWidget::LevelRole);
        ci->setData(QVariant::fromValue<qintptr>(qintptr(c)), SARibbonCustomizeWidget::PointerRole);
        QList<SARibbonPannel *> pannels = c->pannelList();
        for (SARibbonPannel *p : pannels)
        {
            QStandardItem *pi = new QStandardItem(p->windowTitle());
            pi->setData(1, SARibbonCustomizeWidget::LevelRole);
            pi->setData(QVariant::fromValue<qintptr>(qintptr(p)), SARibbonCustomizeWidget::PointerRole);
            if (p->isCanCustomize()) {
                pi->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);//标记这个是可以自定义的
            }
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
                    if (i->widget()->windowTitle().isEmpty() && i->widget()->windowIcon().isNull()) {
                        delete ii;
                        continue;//如果窗口啥也没有，就跳过
                    }
                    ii->setText(i->widget()->windowTitle());
                    ii->setIcon(i->widget()->windowIcon());
                    if (SARibbonCustomizeData::isCanCustomize(i->widget())) {
                        ii->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);//标记这个是可以自定义的
                    }
                }else{
                    ii->setText(i->action->text());
                    ii->setIcon(i->action->icon());
                    if (SARibbonCustomizeData::isCanCustomize(i->action)) {
                        ii->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);//标记这个是可以自定义的
                    }
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
 * @brief 判断itemn为自定义的item，自定义的item都带有CustomizeRole角色
 * @param item
 * @return
 */
bool SARibbonCustomizeWidgetPrivate::isCustomizeItem(QStandardItem *item) const
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

    if (isCustomizeItem(item)) {
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
 * @brief 判断item是否可自定义
 * @param item
 * @return
 */
bool SARibbonCustomizeWidgetPrivate::isItemCanCustomize(QStandardItem *item) const
{
    if (nullptr == item) {
        return (false);
    }
    QVariant v = item->data(SARibbonCustomizeWidget::CanCustomizeRole);

    if (v.isValid()) {
        return (v.toBool());
    }
    return (false);
}


/**
 * @brief 从item 转为action
 * @param item
 * @return
 */
QAction *SARibbonCustomizeWidgetPrivate::itemToAction(QStandardItem *item) const
{
    if (2 != itemLevel(item)) {
        return (nullptr);
    }
    //这里要非常注意，SARibbonCustomizeWidget::CustomizeRole为true时，说明这个是自定义的内容，
    //这时PointerRole里存放的是action指针，不是SARibbonPannelItem
    QAction *act = nullptr;

    if (item->data(SARibbonCustomizeWidget::CustomizeRole).toBool()) {
        act = reinterpret_cast<QAction *>(item->data(SARibbonCustomizeWidget::PointerRole).value<qintptr>());
    }else{
        SARibbonPannelItem *pi = reinterpret_cast<SARibbonPannelItem *>(item->data(SARibbonCustomizeWidget::PointerRole).value<qintptr>());
        act = (pi->action);
    }
    return (act);
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
    connect(m_d->mRibbonModel, &QStandardItemModel::itemChanged
        , this, &SARibbonCustomizeWidget::onItemChanged);
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
    return (m_d->mCustomizeDatas.size() > 0);
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
    if (m_d->mRibbonWindow) {
        SARibbonBar *bar = m_d->mRibbonWindow->ribbonBar();
        if (bar) {
            ui->comboBoxActionProportion->clear();
            if (bar->isTwoRowStyle()) {
                ui->comboBoxActionProportion->addItem(tr("large"), SARibbonPannelItem::Large);
                ui->comboBoxActionProportion->addItem(tr("small"), SARibbonPannelItem::Small);
            }else{
                ui->comboBoxActionProportion->addItem(tr("large"), SARibbonPannelItem::Large);
                ui->comboBoxActionProportion->addItem(tr("medium"), SARibbonPannelItem::Medium);
                ui->comboBoxActionProportion->addItem(tr("small"), SARibbonPannelItem::Small);
            }
        }
    }
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
    qDebug() << "applys "<<m_d->mCustomizeDatas.size() << " customize data";
    return (isSucc);
}


/**
 * @brief 获取当前界面选中的行属性
 * @return
 */
SARibbonPannelItem::RowProportion SARibbonCustomizeWidget::selectedRowProportion() const
{
    return (static_cast<SARibbonPannelItem::RowProportion>(ui->comboBoxActionProportion->currentData().toInt()));
}


/**
 * @brief 获取listview中选中的action
 * @return 如果没有选中action，返回nullptr
 * @note 如果要获取treeview选中的action，使用@ref itemToAction 函数
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
 * @brief 把item转换为action
 * @param item
 * @return 如果没有action可转换，返回nullptr
 */
QAction *SARibbonCustomizeWidget::itemToAction(QStandardItem *item) const
{
    return (m_d->itemToAction(item));
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


/**
 * @brief 判断itemn能否改动，可以改动返回true
 * @param item
 * @return
 */
bool SARibbonCustomizeWidget::isCustomizeItem(QStandardItem *item) const
{
    return (m_d->isCustomizeItem(item));
}


bool SARibbonCustomizeWidget::isSelectedItemIsCustomize() const
{
    return (isCustomizeItem(selectedItem()));
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
    ni->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);//有CustomizeRole，必有CanCustomizeRole
    ni->setData(true, SARibbonCustomizeWidget::CustomizeRole);
    ni->setData(d.categoryObjNameValue, SARibbonCustomizeWidget::CustomizeObjNameRole);
}


void SARibbonCustomizeWidget::onPushButtonNewPannelClicked()
{
    QStandardItem *item = selectedItem();

    if (nullptr == item) {
        return;
    }
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
    ni->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);//有CustomizeRole，必有CanCustomizeRole
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
        QString pannelObjName = m_d->itemObjectName(item);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRenamePannelCustomizeData(text, cateObjName, pannelObjName);
        m_d->mCustomizeDatas.append(d);
    }else{
        //action 不允许改名
        return;
    }
    item->setText(text);
}


void SARibbonCustomizeWidget::onPushButtonAddClicked()
{
    QAction *act = selectedAction();
    QStandardItem *item = selectedItem();

    if ((nullptr == act) || (nullptr == item)) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        //选中category不进行操作
        return;
    }else if (2 == level) {
        //选中action，添加到这个action之后,把item设置为pannel
        item = item->parent();
    }
    QString pannelObjName = m_d->itemObjectName(item);
    QString categoryObjName = m_d->itemObjectName(item->parent());
    QString key = m_d->mActionMgr->key(act);

    SARibbonCustomizeData d = SARibbonCustomizeData::makeAddActionCustomizeData(key
        , m_d->mActionMgr
        , selectedRowProportion()
        , categoryObjName
        , pannelObjName);

    m_d->mCustomizeDatas.append(d);

    QStandardItem *actItem = new QStandardItem(act->icon(), act->text());

    actItem->setData(2, SARibbonCustomizeWidget::LevelRole);
    actItem->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);//有CustomizeRole，必有CanCustomizeRole
    actItem->setData(true, SARibbonCustomizeWidget::CustomizeRole);
    actItem->setData(act->objectName(), SARibbonCustomizeWidget::CustomizeObjNameRole);
    actItem->setData(qintptr(act), SARibbonCustomizeWidget::PointerRole);//把action指针传入
    item->appendRow(actItem);
}


void SARibbonCustomizeWidget::onPushButtonDeleteClicked()
{
    QStandardItem *item = selectedItem();

    if (nullptr == item) {
        return;
    }
    if (!isItemCanCustomize(item)) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        //删除category
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRemoveCategoryCustomizeData(m_d->itemObjectName(item));
        m_d->mCustomizeDatas.append(d);
    }else if (1 == level) {
        //删除pannel
        QString catObjName = m_d->itemObjectName(item->parent());
        QString pannelObjName = m_d->itemObjectName(item);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRemovePannelCustomizeData(catObjName, pannelObjName);
        m_d->mCustomizeDatas.append(d);
    }else if (2 == level) {
        //删除Action
        QString catObjName = m_d->itemObjectName(item->parent()->parent());
        QString pannelObjName = m_d->itemObjectName(item->parent());
        QAction *act = itemToAction(item);
        QString key = m_d->mActionMgr->key(act);
        if (key.isEmpty() || catObjName.isEmpty() || pannelObjName.isEmpty()) {
            return;
        }

        SARibbonCustomizeData d = SARibbonCustomizeData::makeRemoveActionCustomizeData(catObjName
            , pannelObjName
            , key
            , m_d->mActionMgr);
        m_d->mCustomizeDatas.append(d);
    }
    //执行删除操作
    removeItem(item);
    //删除后重新识别
    ui->pushButtonAdd->setEnabled(selectedAction() && isSelectedItemIsCustomize() && selectedRibbonLevel() > 0);
    ui->pushButtonDelete->setEnabled(isSelectedItemIsCustomize());
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
    int level = itemLevel(item);

    ui->pushButtonAdd->setEnabled(selectedAction() && (level > 0) && isItemCanCustomize(item));
    ui->pushButtonDelete->setEnabled(isItemCanCustomize(item));                     //有CustomizeRole，必有CanCustomizeRole
    ui->pushButtonRename->setEnabled(level != 2 || isItemCanCustomize(item));       //QAction 不能改名 ， 有CustomizeRole，必有CanCustomizeRole
}


void SARibbonCustomizeWidget::onToolButtonUpClicked()
{
    QStandardItem *item = selectedItem();

    if ((nullptr == item) || (0 == item->row())) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        //移动category
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(m_d->itemObjectName(item), -1);
        m_d->mCustomizeDatas.append(d);
        int r = item->row();
        item = m_d->mRibbonModel->takeItem(r);
        m_d->mRibbonModel->removeRow(r);
        m_d->mRibbonModel->insertRow(r-1, item);
    }else if (1 == level) {
        QStandardItem *paritem = item->parent();
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangePannelOrderCustomizeData(
            m_d->itemObjectName(paritem)
            , m_d->itemObjectName(item)
            , -1);
        m_d->mCustomizeDatas.append(d);
        int r = item->row();
        item = paritem->takeChild(r);
        paritem->removeRow(r);
        paritem->insertRow(r-1, item);
    }else if (2 == level) {
        QStandardItem *pannelItem = item->parent();
        QStandardItem *categoryItem = pannelItem->parent();
        QAction *act = itemToAction(item);
        if (!act) {
            return;
        }
        QString key = m_d->mActionMgr->key(act);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeActionOrderCustomizeData(
            m_d->itemObjectName(categoryItem)
            , m_d->itemObjectName(pannelItem)
            , key
            , m_d->mActionMgr
            , -1);
        m_d->mCustomizeDatas.append(d);
        int r = item->row();
        item = pannelItem->takeChild(r);
        pannelItem->removeRow(r);
        pannelItem->insertRow(r-1, item);
    }
}


void SARibbonCustomizeWidget::onToolButtonDownClicked()
{
    QStandardItem *item = selectedItem();

    if (item == nullptr) {
        return;
    }
    int count = 0;

    if (item->parent()) {
        count = item->parent()->rowCount();
    }else{
        count = m_d->mRibbonModel->rowCount();
    }
    if ((nullptr == item) || ((count-1) == item->row())) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        //移动category
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(m_d->itemObjectName(item), 1);
        m_d->mCustomizeDatas.append(d);
        int r = item->row();
        item = m_d->mRibbonModel->takeItem(item->row());
        m_d->mRibbonModel->removeRow(r);
        m_d->mRibbonModel->insertRow(r+1, item);
    }else if (1 == level) {
        QStandardItem *paritem = item->parent();
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangePannelOrderCustomizeData(
            m_d->itemObjectName(paritem)
            , m_d->itemObjectName(item)
            , 1);
        m_d->mCustomizeDatas.append(d);
        int r = item->row();
        item = paritem->takeChild(r);
        paritem->removeRow(r);
        paritem->insertRow(r+1, item);
    }else if (2 == level) {
        QStandardItem *pannelItem = item->parent();
        QStandardItem *categoryItem = pannelItem->parent();
        QAction *act = itemToAction(item);
        if (!act) {
            return;
        }
        QString key = m_d->mActionMgr->key(act);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeActionOrderCustomizeData(
            m_d->itemObjectName(categoryItem)
            , m_d->itemObjectName(pannelItem)
            , key
            , m_d->mActionMgr
            , -1);
        m_d->mCustomizeDatas.append(d);
        int r = item->row();
        item = pannelItem->takeChild(r);
        pannelItem->removeRow(r);
        pannelItem->insertRow(r+1, item);
    }
}


void SARibbonCustomizeWidget::onItemChanged(QStandardItem *item)
{
    if (item == nullptr) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        if (item->isCheckable()) {
            QString objname = m_d->itemObjectName(item);
            SARibbonCustomizeData d = SARibbonCustomizeData::makeVisibleCategoryCustomizeData(objname, item->checkState() == Qt::Checked);
            m_d->mCustomizeDatas.append(d);
        }
    }
}
