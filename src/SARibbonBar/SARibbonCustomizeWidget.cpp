#include "SARibbonCustomizeWidget.h"
#include <QDebug>
#include <QtCore/QVariant>
#include <QAction>
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
#include "SARibbonPanel.h"
#include <QStandardItemModel>
#include <QButtonGroup>
#include <QInputDialog>
#include <QLineEdit>
#include <QDateTime>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "SARibbonCustomizeData.h"
#include "SARibbonBar.h"
#include <QFile>
#include <QMessageBox>

////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SARibbonCustomizeWidget
////////////////////////////////////////////////////////////////////////////////////////////////////////

bool sa_customize_datas_to_xml(QXmlStreamWriter* xml, const QList< SARibbonCustomizeData >& cds)
{
    if (cds.size() <= 0) {
        return (false);
    }

    xml->writeStartElement("sa-ribbon-customize");
    for (const SARibbonCustomizeData& d : cds) {
        xml->writeStartElement("customize-data");
        xml->writeAttribute("type", QString::number(d.actionType()));
        xml->writeAttribute("index", QString::number(d.indexValue));
        xml->writeAttribute("key", d.keyValue);
        xml->writeAttribute("category", d.categoryObjNameValue);
        xml->writeAttribute("panel", d.panelObjNameValue);
        xml->writeAttribute("row-prop", QString::number(d.actionRowProportionValue));

        xml->writeEndElement();
    }
    xml->writeEndElement();
    if (xml->hasError()) {
        qWarning() << "write has error";
    }
    return (true);
}

QList< SARibbonCustomizeData > sa_customize_datas_from_xml(QXmlStreamReader* xml, SARibbonActionsManager* mgr)
{
    // 先找到"sa-ribbon-customize"
    while (!xml->atEnd()) {

        if (xml->isStartElement() && (xml->name().toString() == "sa-ribbon-customize")) {
            break;
        }
        xml->readNext();
    }
    QList< SARibbonCustomizeData > res;

    // 开始遍历"customize-data"
    while (!xml->atEnd()) {
        if (xml->isStartElement() && (xml->name().toString() == "customize-data")) {
            // 首先读取属性type
            SARibbonCustomizeData d;
            QXmlStreamAttributes attrs = xml->attributes();
            if (!attrs.hasAttribute("type")) {
                // 说明异常，跳过这个
                xml->readNextStartElement();
                continue;
            }
            bool isOk = false;
            int v     = xml->attributes().value("type").toInt(&isOk);
            if (!isOk) {
                // 说明异常，跳过这个
                xml->readNextStartElement();
                continue;
            }
            d.setActionType(static_cast< SARibbonCustomizeData::ActionType >(v));
            // 开始读取子对象
            if (attrs.hasAttribute("index")) {
                v = xml->attributes().value("index").toInt(&isOk);
                if (isOk) {
                    d.indexValue = v;
                }
            }
            if (attrs.hasAttribute("key")) {
                d.keyValue = attrs.value("key").toString();
            }
            if (attrs.hasAttribute("category")) {
                d.categoryObjNameValue = attrs.value("category").toString();
            }
            if (attrs.hasAttribute("panel")) {
                d.panelObjNameValue = attrs.value("panel").toString();
            }
            if (attrs.hasAttribute("row-prop")) {
                v = xml->attributes().value("row-prop").toInt(&isOk);
                if (isOk) {
                    d.actionRowProportionValue = static_cast< SARibbonPanelItem::RowProportion >(v);
                }
            }
            d.setActionsManager(mgr);
            res.append(d);
        }
        xml->readNext();
    }
    if (xml->hasError()) {
        qWarning() << xml->errorString();
    }
    return (res);
}

int sa_customize_datas_apply(const QList< SARibbonCustomizeData >& cds, SARibbonBar* bar)
{
    int c = 0;

    for (const SARibbonCustomizeData& d : cds) {
        if (d.apply(bar)) {
            ++c;
        }
    }
    return (c);
}

int sa_customize_datas_reverse(const QList< SARibbonCustomizeData >& cds, SARibbonBar* bar)
{
    int c = 0;
    // todo 支持反向操作
    return (c);
}

bool sa_apply_customize_from_xml_file(const QString& filePath, SARibbonBar* bar, SARibbonActionsManager* mgr)
{
    QFile f(filePath);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return (false);
    }
    f.seek(0);
    QXmlStreamReader xml(&f);

    return (SARibbonCustomizeWidget::fromXml(&xml, bar, mgr));
}

/**
 * \if ENGLISH
 * @brief Build the UI for SARibbonCustomizeWidget
 * \endif
 *
 * \if CHINESE
 * @brief 构建SARibbonCustomizeWidget的Ui
 * \endif
 */
class SARibbonCustomizeWidgetUi
{
public:
    QHBoxLayout* horizontalLayoutMain;
    QVBoxLayout* verticalLayoutSelect;
    QLabel* labelSelectAction;
    QHBoxLayout* horizontalLayoutSearch;
    QComboBox* comboBoxActionIndex;
    QLineEdit* lineEditSearchAction;
    QListView* listViewSelect;
    QVBoxLayout* verticalLayoutMidButtons;
    QSpacerItem* verticalSpacerUp;
    QPushButton* pushButtonAdd;
    QPushButton* pushButtonDelete;
    QPushButton* pushButtonReset;
    QSpacerItem* verticalSpacerDown;
    QLabel* labelProportion;
    QComboBox* comboBoxActionProportion;
    QVBoxLayout* verticalLayoutResult;
    QLabel* labelCustomize;
    QHBoxLayout* horizontalLayoutCategorySelect;
    QRadioButton* radioButtonMainCategory;
    QRadioButton* radioButtonAllCategory;
    QButtonGroup* radioButtonGroup;
    QTreeView* treeViewResult;
    QHBoxLayout* horizontalLayoutActionOptBtns;
    QPushButton* pushButtonNewCategory;
    QPushButton* pushButtonNewPanel;
    QPushButton* pushButtonRename;
    QVBoxLayout* verticalLayoutRightButtons;
    QSpacerItem* verticalSpacerUp2;
    QToolButton* toolButtonUp;
    QToolButton* toolButtonDown;
    QSpacerItem* verticalSpacerDown2;

    void setupUi(QWidget* customizeWidget)
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
        treeViewResult->setAnimated(true);                                   // 支持动画
        treeViewResult->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 不允许直接在item上重命名

        verticalLayoutResult->addWidget(treeViewResult);

        horizontalLayoutActionOptBtns = new QHBoxLayout();
        horizontalLayoutActionOptBtns->setObjectName(QStringLiteral("horizontalLayoutActionOptBtns"));
        pushButtonNewCategory = new QPushButton(customizeWidget);
        pushButtonNewCategory->setObjectName(QStringLiteral("pushButtonNewCategory"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonNewCategory);

        pushButtonNewPanel = new QPushButton(customizeWidget);
        pushButtonNewPanel->setObjectName(QStringLiteral("pushButtonNewPanel"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonNewPanel);

        pushButtonRename = new QPushButton(customizeWidget);
        pushButtonRename->setObjectName(QStringLiteral("pushButtonRename"));

        horizontalLayoutActionOptBtns->addWidget(pushButtonRename);

        pushButtonReset = new QPushButton(customizeWidget);
        pushButtonReset->setObjectName(QStringLiteral("pushButtonReset"));
        horizontalLayoutActionOptBtns->addWidget(pushButtonReset);

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
    }  // setupUi

    void retranslateUi(QWidget* customizeWidget)
    {
        customizeWidget->setWindowTitle(QApplication::translate("SARibbonCustomizeWidget", "Customize Widget", Q_NULLPTR));
        labelSelectAction->setText(QApplication::translate("SARibbonCustomizeWidget", "Please Select", Q_NULLPTR));  // cn:请选择
        lineEditSearchAction->setInputMask(QString());
        lineEditSearchAction->setText(QString());
        lineEditSearchAction->setPlaceholderText(
            QApplication::translate("SARibbonCustomizeWidget", "Find Command", Q_NULLPTR));               // cn:查找命令
        pushButtonAdd->setText(QApplication::translate("SARibbonCustomizeWidget", "Add >>", Q_NULLPTR));  // cn:添加 >>
        pushButtonDelete->setText(QApplication::translate("SARibbonCustomizeWidget", "<< Remove", Q_NULLPTR));  // cn:<< 移除
        labelCustomize->setText(
            QApplication::translate("SARibbonCustomizeWidget", "Customize the Ribbon", Q_NULLPTR));  // cn:自定义功能区
        radioButtonMainCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "Main Category", Q_NULLPTR));  // cn:主选项卡
        radioButtonAllCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "All Category", Q_NULLPTR));  // cn:所有选项卡
        pushButtonNewCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "New Category", Q_NULLPTR));  // cn:新建选项卡
        pushButtonNewPanel->setText(QApplication::translate("SARibbonCustomizeWidget", "New Group", Q_NULLPTR));  // cn:新建组
        pushButtonRename->setText(QApplication::translate("SARibbonCustomizeWidget", "Rename", Q_NULLPTR));  // cn:重命名
        pushButtonReset->setText(QApplication::translate("SARibbonCustomizeWidget", "reset", Q_NULLPTR));    // cn:重置
        labelProportion->setText(QApplication::translate("SARibbonCustomizeWidget", "proportion:", Q_NULLPTR));  // cn:比例
    }  // retranslateUi
};

/**
 * \if ENGLISH
 * @brief Manage the business logic of SARibbonCustomizeWidget
 * \endif
 *
 * \if CHINESE
 * @brief 管理SARibbonCustomizeWidget的业务逻辑
 * \endif
 */
class SARibbonCustomizeWidget::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonCustomizeWidget)
public:
    SARibbonCustomizeWidget::RibbonTreeShowType mShowType { SARibbonCustomizeWidget::ShowAllCategory };  ///< 显示类型
    SARibbonBar* mRibbonBar { nullptr };                   ///< 保存SARibbonMainWindow的指针
    SARibbonActionsManager* mActionMgr { nullptr };        ///< action管理器
    SARibbonActionsManagerModel* mAcionModel { nullptr };  ///< action管理器对应的model
    QStandardItemModel* mRibbonModel { nullptr };          ///< 用于很成ribbon的树
    int mCustomizeCategoryCount { 0 };                     ///< 记录自定义Category的个数
    int mCustomizePanelCount { 0 };                        ///< 记录自定义Panel的个数
public:
    PrivateData(SARibbonCustomizeWidget* p);
    void updateModel();

    QList< SARibbonCustomizeData > mCustomizeDatasCache;    ///< 缓存记录所有的自定义动作
    QList< SARibbonCustomizeData > mCustomizeDatasApplied;  ///< 应用后的所有的自定义动作
    QList< SARibbonCustomizeData > mOldCustomizeDatas;      ///< 记录旧的自定义动作,本地文件缓存
    // 创建一个随机id，形如：pre_QDateTime::currentMSecsSinceEpoch_suf
    static QString makeRandomObjName(const QString& pre);

    int itemLevel(QStandardItem* item) const;

    //
    bool isCustomizeItem(QStandardItem* item) const;

    // 把item转换为category
    SARibbonCategory* itemToCategory(QStandardItem* item) const;

    // 把item转换为SARibbonPanel
    SARibbonPanel* itemToPanel(QStandardItem* item) const;

    // 获取item对应的object name
    QString itemObjectName(QStandardItem* item) const;

    // 判断是否可以自定义
    bool isItemCanCustomize(QStandardItem* item) const;

    // 从item转为action
    QAction* itemToAction(QStandardItem* item) const;
};

SARibbonCustomizeWidget::PrivateData::PrivateData(SARibbonCustomizeWidget* p)
    : q_ptr(p), mAcionModel(new SARibbonActionsManagerModel(p)), mRibbonModel(new QStandardItemModel(p))
{
}

void SARibbonCustomizeWidget::PrivateData::updateModel()
{
    if (mRibbonBar == nullptr) {
        return;
    }
    mRibbonModel->clear();
    SARibbonBar* ribbonbar               = mRibbonBar;
    QList< SARibbonCategory* > categorys = ribbonbar->categoryPages();

    for (const SARibbonCategory* c : sa_as_const(categorys)) {
        if ((mShowType == SARibbonCustomizeWidget::ShowMainCategory) && c->isContextCategory()) {
            // 如果是只显示主内容，如果是上下文标签就忽略
            continue;
        }
        QStandardItem* ci = new QStandardItem();
        if (c->isContextCategory()) {
            ci->setText(QString("[%1]").arg(c->categoryName()));
        } else {
            ci->setText(c->categoryName());
        }
        if (c->isCanCustomize() && !c->isContextCategory()) {
            // 上下文标签不做显示隐藏处理
            ci->setCheckable(true);
            ci->setCheckState(ribbonbar->isCategoryVisible(c) ? Qt::Checked : Qt::Unchecked);
            ci->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 标记这个是可以自定义的
        }
        ci->setData(0, SARibbonCustomizeWidget::LevelRole);
        ci->setData(QVariant::fromValue< qintptr >(qintptr(c)), SARibbonCustomizeWidget::PointerRole);
        QList< SARibbonPanel* > panels = c->panelList();
        for (const SARibbonPanel* p : sa_as_const(panels)) {
            QStandardItem* pi = new QStandardItem(p->panelName());
            pi->setData(1, SARibbonCustomizeWidget::LevelRole);
            pi->setData(QVariant::fromValue< qintptr >(qintptr(p)), SARibbonCustomizeWidget::PointerRole);
            if (p->isCanCustomize()) {
                pi->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 标记这个是可以自定义的
            }
            ci->appendRow(pi);
            const QList< SARibbonPanelItem* >& items = p->ribbonPanelItem();
            for (SARibbonPanelItem* i : sa_as_const(items)) {
                if (i->action->isSeparator()) {
                    continue;
                }
                QStandardItem* ii = new QStandardItem();
                //                if (i->customWidget) {
                //                    //如果是自定义窗口
                //                    if (i->widget()->windowTitle().isEmpty() && i->widget()->windowIcon().isNull()) {
                //                        delete ii;
                //                        continue;  //如果窗口啥也没有，就跳过
                //                    }
                //                    ii->setText(i->widget()->windowTitle());
                //                    ii->setIcon(i->widget()->windowIcon());
                //                    if (SARibbonCustomizeData::isCanCustomize(i->widget())) {
                //                        ii->setData(true, SARibbonCustomizeWidget::CanCustomizeRole); //标记这个是可以自定义的
                //                    }
                //                } else {
                //                    //不是自定义，说明是action
                //                    ii->setText(i->action->text());
                //                    ii->setIcon(i->action->icon());
                //                    if (SARibbonCustomizeData::isCanCustomize(i->action)) {
                //                        ii->setData(true, SARibbonCustomizeWidget::CanCustomizeRole); //标记这个是可以自定义的
                //                    }
                //                }
                ii->setText(i->action->text());
                ii->setIcon(i->action->icon());
                if (SARibbonCustomizeData::isCanCustomize(i->action)) {
                    ii->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 标记这个是可以自定义的
                }
                ii->setData(2, SARibbonCustomizeWidget::LevelRole);
                ii->setData(QVariant::fromValue< qintptr >(qintptr(i)), SARibbonCustomizeWidget::PointerRole);
                pi->appendRow(ii);
            }
        }
        mRibbonModel->appendRow(ci);
    }
}

/**
 * \if ENGLISH
 * @brief Create a random ID in the format: pre_QDateTime::currentMSecsSinceEpoch
 * @param pre Prefix
 * @return Random object name
 * \endif
 *
 * \if CHINESE
 * @brief 创建一个随机id，形如：pre_QDateTime::currentMSecsSinceEpoch
 * @param pre 前缀
 * @return 随机对象名称
 * \endif
 */
QString SARibbonCustomizeWidget::PrivateData::makeRandomObjName(const QString& pre)
{
    return (QString("%1_%2").arg(pre).arg(QDateTime::currentMSecsSinceEpoch()));
}

/**
 * \if ENGLISH
 * @brief Get the level of the item
 * @param item QStandardItem to check
 * @return Level of the item
 * \endif
 *
 * \if CHINESE
 * @brief 获取item的level
 * @param item 要检查的项
 * @return 项的层级
 * \endif
 */
int SARibbonCustomizeWidget::PrivateData::itemLevel(QStandardItem* item) const
{
    return (item->data(SARibbonCustomizeWidget::LevelRole).toInt());
}

/**
 * \if ENGLISH
 * @brief Check if item is a customize item
 * @details Customize items have CustomizeRole role set
 * @param item QStandardItem to check
 * @return true if the item is a customize item
 * \endif
 *
 * \if CHINESE
 * @brief 判断itemn为自定义的item，自定义的item都带有CustomizeRole角色
 * @param item 要检查的项
 * @return 如果是自定义item返回true
 * \endif
 */
bool SARibbonCustomizeWidget::PrivateData::isCustomizeItem(QStandardItem* item) const
{
    if (nullptr == item) {
        return (false);
    }
    return (item->data(SARibbonCustomizeWidget::CustomizeRole).isValid());
}

/**
 * \if ENGLISH
 * @brief Convert item to category
 * @param item QStandardItem to convert
 * @return SARibbonCategory pointer, nullptr if conversion fails
 * \endif
 *
 * \if CHINESE
 * @brief 把item转换为category
 * @param item 要转换的项
 * @return 无法转换返回nullptr
 * \endif
 */
SARibbonCategory* SARibbonCustomizeWidget::PrivateData::itemToCategory(QStandardItem* item) const
{
    int level = item->data(SARibbonCustomizeWidget::LevelRole).toInt();

    if (level != 0) {
        return (nullptr);
    }
    qintptr p = item->data(SARibbonCustomizeWidget::PointerRole).value< qintptr >();

    return (reinterpret_cast< SARibbonCategory* >(p));
}

/**
 * \if ENGLISH
 * @brief Convert item to panel
 * @param item QStandardItem to convert
 * @return SARibbonPanel pointer, nullptr if conversion fails
 * \endif
 *
 * \if CHINESE
 * @brief 把item转换为panel
 * @param item 要转换的项
 * @return 无法转换返回nullptr
 * \endif
 */
SARibbonPanel* SARibbonCustomizeWidget::PrivateData::itemToPanel(QStandardItem* item) const
{
    int level = item->data(SARibbonCustomizeWidget::LevelRole).toInt();

    if (level != 1) {
        return (nullptr);
    }
    qintptr p = item->data(SARibbonCustomizeWidget::PointerRole).value< qintptr >();

    return (reinterpret_cast< SARibbonPanel* >(p));
}

/**
 * \if ENGLISH
 * @brief Get the object name corresponding to the item
 * @param item QStandardItem to get object name from
 * @return Object name, empty QString if unable to get
 * \endif
 *
 * \if CHINESE
 * @brief 获取item对应的objectname
 * @param item 要获取的项
 * @return 如果无法获取，返回一个空的QString
 * \endif
 */
QString SARibbonCustomizeWidget::PrivateData::itemObjectName(QStandardItem* item) const
{
    QString objName;

    if (isCustomizeItem(item)) {
        // 说明是自定义的
        objName = item->data(SARibbonCustomizeWidget::CustomizeObjNameRole).toString();
    } else {
        // 说明这个是非自定义的
        int level = itemLevel(item);
        if (0 == level) {
            SARibbonCategory* category = itemToCategory(item);
            if (category) {
                objName = category->objectName();
            }
        } else if (1 == level) {
            SARibbonPanel* panel = itemToPanel(item);
            if (panel) {
                objName = panel->objectName();
            }
        }
    }
    return (objName);
}

/**
 * \if ENGLISH
 * @brief Check if item can be customized
 * @param item QStandardItem to check
 * @return true if the item can be customized
 * \endif
 *
 * \if CHINESE
 * @brief 判断item是否可自定义
 * @param item 要检查的项
 * @return 是否可自定义
 * \endif
 */
bool SARibbonCustomizeWidget::PrivateData::isItemCanCustomize(QStandardItem* item) const
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
 * \if ENGLISH
 * @brief Convert item to action
 * @param item QStandardItem to convert
 * @return QAction pointer, nullptr if conversion fails
 * \endif
 *
 * \if CHINESE
 * @brief 从item 转为action
 * @param item 要转换的项
 * @return QAction指针
 * \endif
 */
QAction* SARibbonCustomizeWidget::PrivateData::itemToAction(QStandardItem* item) const
{
    if (2 != itemLevel(item)) {
        return (nullptr);
    }
    // 这里要非常注意，SARibbonCustomizeWidget::CustomizeRole为true时，说明这个是自定义的内容，
    // 这时PointerRole里存放的是action指针，不是SARibbonPanelItem
    QAction* act = nullptr;

    if (item->data(SARibbonCustomizeWidget::CustomizeRole).toBool()) {
        act = reinterpret_cast< QAction* >(item->data(SARibbonCustomizeWidget::PointerRole).value< qintptr >());
    } else {
        SARibbonPanelItem* pi =
            reinterpret_cast< SARibbonPanelItem* >(item->data(SARibbonCustomizeWidget::PointerRole).value< qintptr >());
        act = (pi->action);
    }
    return (act);
}

//===================================================
// SARibbonCustomizeWidget
//===================================================
/**
 * \if ENGLISH
 * @brief Constructor for SARibbonCustomizeWidget
 * @param ribbonWindow SARibbonMainWindow pointer to manage
 * @param parent Parent widget for UI generation, can be the same as SARibbonMainWindow
 * @param f Window flags, same as QWidget::QWidget second parameter
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonCustomizeWidget构造函数
 * @param ribbonWindow 传入需要管理的SARibbonMainWindow指针
 * @param parent 用于界面生成的parent，可以和SARibbonMainWindow一样
 * @param f 同QWidget::QWidget的第二个参数
 * \endif
 */
SARibbonCustomizeWidget::SARibbonCustomizeWidget(SARibbonMainWindow* ribbonWindow, QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f), d_ptr(new SARibbonCustomizeWidget::PrivateData(this)), ui(new SARibbonCustomizeWidgetUi)
{
    init(ribbonWindow->ribbonBar());
}

SARibbonCustomizeWidget::SARibbonCustomizeWidget(SARibbonBar* ribbonbar, QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f), d_ptr(new SARibbonCustomizeWidget::PrivateData(this)), ui(new SARibbonCustomizeWidgetUi)
{
    init(ribbonbar);
}

void SARibbonCustomizeWidget::init(SARibbonBar* ribbonbar)
{
    d_ptr->mRibbonBar = ribbonbar;
    ui->setupUi(this);
    ui->listViewSelect->setModel(d_ptr->mAcionModel);
    ui->treeViewResult->setModel(d_ptr->mRibbonModel);
    initConnection();
    updateModel();
}

SARibbonCustomizeWidget::~SARibbonCustomizeWidget()
{
    delete ui;
}

void SARibbonCustomizeWidget::initConnection()
{
    //    这个需要qt5.8以上支持
    //    connect(ui->comboBoxActionIndex, QOverload<int>::of(&QComboBox::currentIndexChanged)
    //        , this, &SARibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged);
    connect(ui->comboBoxActionIndex,
            static_cast< void (QComboBox::*)(int) >(&QComboBox::currentIndexChanged),
            this,
            &SARibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged);
    //    这个需要qt5.8以上支持
    //    connect(ui->radioButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked)
    //        , this, &SARibbonCustomizeWidget::onRadioButtonGroupButtonClicked);
    connect(ui->radioButtonGroup,
            static_cast< void (QButtonGroup::*)(QAbstractButton*) >(&QButtonGroup::buttonClicked),
            this,
            &SARibbonCustomizeWidget::onRadioButtonGroupButtonClicked);
    connect(ui->pushButtonNewCategory, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonNewCategoryClicked);
    connect(ui->pushButtonNewPanel, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonNewPanelClicked);
    connect(ui->pushButtonRename, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonRenameClicked);
    connect(ui->pushButtonAdd, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonAddClicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonDeleteClicked);
    connect(ui->listViewSelect, &QAbstractItemView::clicked, this, &SARibbonCustomizeWidget::onListViewSelectClicked);
    connect(ui->treeViewResult, &QAbstractItemView::clicked, this, &SARibbonCustomizeWidget::onTreeViewResultClicked);
    connect(ui->toolButtonUp, &QToolButton::clicked, this, &SARibbonCustomizeWidget::onToolButtonUpClicked);
    connect(ui->toolButtonDown, &QToolButton::clicked, this, &SARibbonCustomizeWidget::onToolButtonDownClicked);
    connect(d_ptr->mRibbonModel, &QStandardItemModel::itemChanged, this, &SARibbonCustomizeWidget::onItemChanged);
    connect(ui->lineEditSearchAction, &QLineEdit::textEdited, this, &SARibbonCustomizeWidget::onLineEditSearchActionTextEdited);
    connect(ui->pushButtonReset, &QPushButton::clicked, this, &SARibbonCustomizeWidget::onPushButtonResetClicked);
}

/**
 * \if ENGLISH
 * @brief Set the action manager
 * @param mgr SARibbonActionsManager pointer
 * \endif
 *
 * \if CHINESE
 * @brief 设置action管理器
 * @param mgr SARibbonActionsManager 指针
 * \endif
 */
void SARibbonCustomizeWidget::setupActionsManager(SARibbonActionsManager* mgr)
{
    d_ptr->mActionMgr = mgr;
    if (d_ptr->mActionMgr) {
        d_ptr->mAcionModel->uninstallActionsManager();
    }
    d_ptr->mAcionModel->setupActionsManager(mgr);
    // 更新左边复选框
    QList< int > tags = mgr->actionTags();

    ui->comboBoxActionIndex->clear();
    for (int tag : sa_as_const(tags)) {
        if (mgr->tagName(tag).isEmpty())
            continue;
        ui->comboBoxActionIndex->addItem(mgr->tagName(tag), tag);
    }
}

/**
 * \if ENGLISH
 * @brief Check if there is content to store, corresponding to save action
 * @return true if there is content to save
 * \endif
 *
 * \if CHINESE
 * @brief 判断用户是否有要存储的内容，对应save动作
 * @return 是否有内容可保存
 * \endif
 */
bool SARibbonCustomizeWidget::isApplied() const
{
    return (d_ptr->mCustomizeDatasApplied.size() > 0);
}

/**
 * \if ENGLISH
 * @brief Check if there are modified contents, corresponding to apply action
 * @return true if there are cached changes
 * \endif
 *
 * \if CHINESE
 * @brief 判断用户是否有改动内容，对应apply动作
 * @return 是否有缓存的改动
 * \endif
 */
bool SARibbonCustomizeWidget::isCached() const
{
    return (d_ptr->mCustomizeDatasCache.size() > 0);
}

/**
 * \if ENGLISH
 * @brief Get the model
 * @return QStandardItemModel pointer
 * \endif
 *
 * \if CHINESE
 * @brief 获取model
 * @return QStandardItemModel指针
 * \endif
 */
const QStandardItemModel* SARibbonCustomizeWidget::model() const
{
    return (d_ptr->mRibbonModel);
}

/**
 * \if ENGLISH
 * @brief Update the model based on current radiobutton selection
 * \endif
 *
 * \if CHINESE
 * @brief 根据当前的radiobutton选项来更新model
 * \endif
 */
void SARibbonCustomizeWidget::updateModel()
{
    updateModel(ui->radioButtonAllCategory->isChecked() ? ShowAllCategory : ShowMainCategory);
    if (d_ptr->mRibbonBar) {
        SARibbonBar* bar = d_ptr->mRibbonBar;
        if (bar) {
            ui->comboBoxActionProportion->clear();
            if (bar->isTwoRowStyle()) {
                ui->comboBoxActionProportion->addItem(tr("large"), SARibbonPanelItem::Large);
                ui->comboBoxActionProportion->addItem(tr("small"), SARibbonPanelItem::Small);
            } else {
                ui->comboBoxActionProportion->addItem(tr("large"), SARibbonPanelItem::Large);
                ui->comboBoxActionProportion->addItem(tr("medium"), SARibbonPanelItem::Medium);
                ui->comboBoxActionProportion->addItem(tr("small"), SARibbonPanelItem::Small);
            }
        }
    }
}

/**
 * \if ENGLISH
 * @brief Update the model with specified type
 * @param type RibbonTreeShowType to use
 * \endif
 *
 * \if CHINESE
 * @brief 更新model
 * @param type 显示类型
 * \endif
 */
void SARibbonCustomizeWidget::updateModel(RibbonTreeShowType type)
{
    d_ptr->mShowType = type;
    d_ptr->updateModel();
}

/**
 * \if ENGLISH
 * @brief Apply all settings
 * @return true if any setting applied successfully
 * @note Returns true if any setting is applied successfully
 * \endif
 *
 * \if CHINESE
 * @brief 应用所有设定
 * @return 应用成功返回true
 * @note 所有设定有一个应用成功都会返回true
 * \endif
 */
bool SARibbonCustomizeWidget::applys()
{
    simplify();
    if (sa_customize_datas_apply(d_ptr->mCustomizeDatasCache, d_ptr->mRibbonBar) > 0) {
        // 将临时操作存入已应用操作，并清空临时操作
        makeActionsApplied();
        clearCache();
        return true;
    } else {
        return false;
    }
}

/**
 * \if ENGLISH
 * @brief Convert to XML
 * @details This function only writes elements, not document related content.
 *          If you need to write document, call QXmlStreamWriter::writeStartDocument() before this function,
 *          and call QXmlStreamWriter::writeEndDocument() after this function.
 * @note Before passing QXmlStreamWriter, you need to set the encoding to utf-8: xml->setCodec("utf-8");
 * @note Since QXmlStreamWriter does not support encoding when QString is used as io, and this cannot guarantee
 *       that Chinese characters will not appear in the customization process, QXmlStreamWriter should not be
 *       constructed through QString. If you need to use string, you need to construct through QByteArray.
 * @return false if exception occurs, also returns false if there is no customization data
 * @see sa_customize_datas_to_xml
 * \endif
 *
 * \if CHINESE
 * @brief 转换为xml
 * @details 此函数仅会写element，不会写document相关内容，因此如果需要写document，
 *          需要在此函数前调用QXmlStreamWriter::writeStartDocument(),在此函数后调用QXmlStreamWriter::writeEndDocument()
 * @note 注意，在传入QXmlStreamWriter之前，需要设置编码为utf-8:xml->setCodec("utf-8");
 * @note 由于QXmlStreamWriter在QString作为io时，是不支持编码的，而此又无法保证自定义过程不出现中文字符，
 *       因此，QXmlStreamWriter不应该通过QString进行构造，如果需要用到string，也需要通过QByteArray构造
 * @return 如果出现异常，返回false,如果没有自定义数据也会返回false
 * @see sa_customize_datas_to_xml
 * \endif
 */
bool SARibbonCustomizeWidget::toXml(QXmlStreamWriter* xml) const
{
    QList< SARibbonCustomizeData > res = d_ptr->mOldCustomizeDatas;

    if (isApplied())
        res << d_ptr->mCustomizeDatasApplied;
    if (isCached())
        res << d_ptr->mCustomizeDatasCache;

    res = SARibbonCustomizeData::simplify(res);
    return (sa_customize_datas_to_xml(xml, res));
}

/**
 * \if ENGLISH
 * @brief Write configuration to file
 * @param xmlpath XML file path
 * @return true if successful
 * \endif
 *
 * \if CHINESE
 * @brief 把配置写入文件中
 * @param xmlpath XML文件路径
 * @return 是否成功
 * \endif
 */
bool SARibbonCustomizeWidget::toXml(const QString& xmlpath) const
{
    QFile f(xmlpath);

    if (!f.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        return (false);
    }
    QXmlStreamWriter xml(&f);

    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)  // QXmlStreamWriter always encodes XML in UTF-8.
    xml.setCodec("utf-8");                  // 在writeStartDocument之前指定编码
#endif
    xml.writeStartDocument();
    bool isOK = toXml(&xml);

    xml.writeEndDocument();
    f.close();
    return (isOK);
}

/**
 * \if ENGLISH
 * @brief Load QList<SARibbonCustomizeData> from XML
 * @details For file-based settings, it is recommended to call this function before the dialog is displayed
 *          to ensure correct recording of stacked settings
 * @param xml XML stream reader
 * @note This function should be called after @ref setupActionsManager
 * \endif
 *
 * \if CHINESE
 * @brief 从xml中加载QList<SARibbonCustomizeData>
 * @details 对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
 * @param xml XML流读取器
 * @note 此函数要在@ref setupActionsManager 函数之后调用
 * \endif
 */
void SARibbonCustomizeWidget::fromXml(QXmlStreamReader* xml)
{
    QList< SARibbonCustomizeData > cds = sa_customize_datas_from_xml(xml, d_ptr->mActionMgr);

    d_ptr->mOldCustomizeDatas = cds;
}

/**
 * \if ENGLISH
 * @brief Load QList<SARibbonCustomizeData> from XML file
 * @details For file-based settings, it is recommended to call this function before the dialog is displayed
 *          to ensure correct recording of stacked settings
 * @param xmlpath XML file path
 * @note This function should be called after @ref setupActionsManager
 * @note If the program loads custom configuration after startup, you need to call this function when calling this window
 *       to load the original configuration. The old actions will be saved when generating new actions,
 *       but this loaded action will not be called when applys is called.
 * \endif
 *
 * \if CHINESE
 * @brief 从xml中加载QList<SARibbonCustomizeData>
 * @details 对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
 * @param xmlpath XML文件路径
 * @note 此函数要在@ref setupActionsManager 函数之后调用
 * @note 如果程序启动后加载了自定义配置，再调用此窗口时需要调用此函数，把原来的配置加载进来，
 *       在生成新动作时会把旧动作保存，但在调用applys时不会调用此加载的动作
 * \endif
 */
void SARibbonCustomizeWidget::fromXml(const QString& xmlpath)
{
    QFile f(xmlpath);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    f.seek(0);
    QXmlStreamReader xml(&f);

    fromXml(&xml);
}

/**
 * \if ENGLISH
 * @brief Apply XML configuration
 * @note Repeating loading a configuration file will cause exceptions. To avoid this, generally use a variable to ensure it is only loaded once.
 * @param xml XML stream reader
 * @param bar SARibbonBar pointer
 * @param mgr SARibbonActionsManager pointer
 * @return Returns true if any setting is applied successfully
 * @see sa_customize_datas_from_xml sa_customize_datas_apply sa_apply_customize_from_xml_file
 * \endif
 *
 * \if CHINESE
 * @brief 应用xml配置
 * @note 重复加载一个配置文件会发生异常，为了避免此类事件发生，一般通过一个变量保证只加载一次
 * @param xml XML流读取器
 * @param bar SARibbonBar指针
 * @param mgr SARibbonActionsManager指针
 * @return 所有设定有一个应用成功都会返回true
 * @see sa_customize_datas_from_xml sa_customize_datas_apply sa_apply_customize_from_xml_file
 * \endif
 */
bool SARibbonCustomizeWidget::fromXml(QXmlStreamReader* xml, SARibbonBar* bar, SARibbonActionsManager* mgr)
{
    // 先找到sa-ribbon-customize标签
    QList< SARibbonCustomizeData > cds = sa_customize_datas_from_xml(xml, mgr);

    return (sa_customize_datas_apply(cds, bar) > 0);
}

/**
 * \if ENGLISH
 * @brief Make actions applied
 * \endif
 *
 * \if CHINESE
 * @brief 清除已应用的动作
 * \endif
 */
void SARibbonCustomizeWidget::makeActionsApplied()
{
    d_ptr->mCustomizeDatasApplied << d_ptr->mCustomizeDatasCache;
}

/**
 * \if ENGLISH
 * @brief Clear applied actions
 * @note Need to clear applied actions after cancel operation
 * \endif
 *
 * \if CHINESE
 * @brief 清除applied的动作
 * @note cancel操作后需要清空已应用的动作
 * \endif
 */
void SARibbonCustomizeWidget::clearApplied()
{
    d_ptr->mCustomizeDatasApplied.clear();
}

/**
 * \if ENGLISH
 * @brief Clear cached actions
 * @note After executing applys function, if you want to continue calling, you should clear, otherwise it will cause exceptions
 * \endif
 *
 * \if CHINESE
 * @brief 清除缓存动作
 * @note 在执行applys函数后，如果要继续调用，应该clear，否则会导致异常
 * \endif
 */
void SARibbonCustomizeWidget::clearCache()
{
    d_ptr->mCustomizeDatasCache.clear();
}

/**
 * \if ENGLISH
 * @brief Clear all actions, excluding locally read data
 * \endif
 *
 * \if CHINESE
 * @brief 清除所有动作，不包含本地读取的数据
 * \endif
 */
void SARibbonCustomizeWidget::clear()
{
    clearApplied();
    clearCache();
}

/**
 * \if ENGLISH
 * @brief Simplify customize data
 * \endif
 *
 * \if CHINESE
 * @brief 精简
 * \endif
 */
void SARibbonCustomizeWidget::simplify()
{
    d_ptr->mCustomizeDatasCache = SARibbonCustomizeData::simplify(d_ptr->mCustomizeDatasCache);
}

/**
 * \if ENGLISH
 * @brief Get the row proportion selected in the current interface
 * @return SARibbonPanelItem::RowProportion
 * \endif
 *
 * \if CHINESE
 * @brief 获取当前界面选中的行属性
 * @return 行比例
 * \endif
 */
SARibbonPanelItem::RowProportion SARibbonCustomizeWidget::selectedRowProportion() const
{
    return (static_cast< SARibbonPanelItem::RowProportion >(ui->comboBoxActionProportion->currentData().toInt()));
}

/**
 * \if ENGLISH
 * @brief Get the action selected in listview
 * @return Returns nullptr if no action is selected
 * @note To get the action selected in treeview, use @ref itemToAction function
 * \endif
 *
 * \if CHINESE
 * @brief 获取listview中选中的action
 * @return 如果没有选中action，返回nullptr
 * @note 如果要获取treeview选中的action，使用@ref itemToAction 函数
 * \endif
 */
QAction* SARibbonCustomizeWidget::selectedAction() const
{
    QItemSelectionModel* m = ui->listViewSelect->selectionModel();

    if ((nullptr == m) || !m->hasSelection()) {
        return (nullptr);
    }
    QModelIndex i = m->currentIndex();

    return (d_ptr->mAcionModel->indexToAction(i));
}

/**
 * \if ENGLISH
 * @brief Convert item to action
 * @param item QStandardItem to convert
 * @return Returns nullptr if no action can be converted
 * \endif
 *
 * \if CHINESE
 * @brief 把item转换为action
 * @param item 要转换的项
 * @return 如果没有action可转换，返回nullptr
 * \endif
 */
QAction* SARibbonCustomizeWidget::itemToAction(QStandardItem* item) const
{
    return (d_ptr->itemToAction(item));
}

/**
 * \if ENGLISH
 * @brief Get the selected item in ribbon tree
 * @return Selected QStandardItem pointer
 * \endif
 *
 * \if CHINESE
 * @brief 获取ribbon tree选中的item
 * @return 选中的项
 * \endif
 */
QStandardItem* SARibbonCustomizeWidget::selectedItem() const
{
    QItemSelectionModel* m = ui->treeViewResult->selectionModel();

    if ((nullptr == m) || !m->hasSelection()) {
        return (nullptr);
    }
    QModelIndex i = m->currentIndex();

    return (d_ptr->mRibbonModel->itemFromIndex(i));
}

/**
 * \if ENGLISH
 * @brief Get the level of selected ribbon tree item
 * @return -1 for abnormal selection, 0 for category selected, 1 for panel selected, 2 for action selected
 * \endif
 *
 * \if CHINESE
 * @brief 获取选中的ribbon tree 的level
 * @return -1为选中异常，0代表选中了category 1代表选中了panel 2代表选中了action
 * \endif
 */
int SARibbonCustomizeWidget::selectedRibbonLevel() const
{
    QStandardItem* item = selectedItem();

    if (item) {
        return (itemLevel(item));
    }
    return (-1);
}

/**
 * \if ENGLISH
 * @brief Get the level of StandardItem
 * @param item QStandardItem to check
 * @return Level of the item
 * \endif
 *
 * \if CHINESE
 * @brief 获取StandardItem 的level
 * @param item 要检查的项
 * @return 项的层级
 * \endif
 */
int SARibbonCustomizeWidget::itemLevel(QStandardItem* item) const
{
    return (d_ptr->itemLevel(item));
}

/**
 * \if ENGLISH
 * @brief Set an item to be selected
 * @param item QStandardItem to select
 * @param ensureVisible Whether to ensure the item is visible
 * \endif
 *
 * \if CHINESE
 * @brief 设置某个item被选中
 * @param item 要选中的项
 * @param ensureVisible 是否确保项可见
 * \endif
 */
void SARibbonCustomizeWidget::setSelectItem(QStandardItem* item, bool ensureVisible)
{
    QItemSelectionModel* m = ui->treeViewResult->selectionModel();

    if (nullptr == m) {
        return;
    }
    if (m) {
        m->clearSelection();
        m->setCurrentIndex(item->index(), QItemSelectionModel::SelectCurrent);
    }
    if (ensureVisible) {
        ui->treeViewResult->scrollTo(item->index());
    }
}

/**
 * \if ENGLISH
 * @brief Check if item can be customized
 * @param item QStandardItem to check
 * @return true if the item can be customized
 * \endif
 *
 * \if CHINESE
 * @brief 判断itemn能否改动，可以改动返回true
 * @param item 要检查的项
 * @return 是否可自定义
 * \endif
 */
bool SARibbonCustomizeWidget::isItemCanCustomize(QStandardItem* item) const
{
    return (d_ptr->isItemCanCustomize(item));
}

bool SARibbonCustomizeWidget::isSelectedItemCanCustomize() const
{
    return (isItemCanCustomize(selectedItem()));
}

/**
 * \if ENGLISH
 * @brief Check if item is a customize item
 * @param item QStandardItem to check
 * @return true if the item is a customize item
 * \endif
 *
 * \if CHINESE
 * @brief 判断itemn能否改动，可以改动返回true
 * @param item 要检查的项
 * @return 是否是自定义项
 * \endif
 */
bool SARibbonCustomizeWidget::isCustomizeItem(QStandardItem* item) const
{
    return (d_ptr->isCustomizeItem(item));
}

bool SARibbonCustomizeWidget::isSelectedItemIsCustomize() const
{
    return (isCustomizeItem(selectedItem()));
}

void SARibbonCustomizeWidget::removeItem(QStandardItem* item)
{
    if (item->parent()) {
        item->parent()->removeRow(item->row());
    } else {
        d_ptr->mRibbonModel->removeRow(item->row());
    }
}

void SARibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged(int index)
{
    int tag = ui->comboBoxActionIndex->itemData(index).toInt();

    d_ptr->mAcionModel->setFilter(tag);
}

void SARibbonCustomizeWidget::onRadioButtonGroupButtonClicked(QAbstractButton* b)
{
    updateModel(b == ui->radioButtonAllCategory ? ShowAllCategory : ShowMainCategory);
}

void SARibbonCustomizeWidget::onPushButtonNewCategoryClicked()
{
    int row                = d_ptr->mRibbonModel->rowCount();
    QItemSelectionModel* m = ui->treeViewResult->selectionModel();

    if (m && m->hasSelection()) {
        QModelIndex i = m->currentIndex();
        while (i.parent().isValid()) {
            i = i.parent();
        }
        // 获取选中的最顶层item
        row = i.row() + 1;
    }
    QStandardItem* ni = new QStandardItem(tr("new category[customize]%1").arg(++(d_ptr->mCustomizeCategoryCount)));

    ni->setData(0, SARibbonCustomizeWidget::LevelRole);
    d_ptr->mRibbonModel->insertRow(row, ni);
    // 设置新增的为选中
    setSelectItem(ni);
    // 把动作插入动作列表中
    SARibbonCustomizeData d = SARibbonCustomizeData::makeAddCategoryCustomizeData(
        ni->text(), ni->row(), SARibbonCustomizeWidget::PrivateData::makeRandomObjName("category"));

    d_ptr->mCustomizeDatasCache.append(d);
    ni->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 有CustomizeRole，必有CanCustomizeRole
    ni->setData(true, SARibbonCustomizeWidget::CustomizeRole);
    ni->setData(d.categoryObjNameValue, SARibbonCustomizeWidget::CustomizeObjNameRole);
}

void SARibbonCustomizeWidget::onPushButtonNewPanelClicked()
{
    QStandardItem* item = selectedItem();

    if (nullptr == item) {
        return;
    }
    int level = selectedRibbonLevel();

    QStandardItem* ni = new QStandardItem(tr("new panel[customize]%1").arg(++(d_ptr->mCustomizePanelCount)));

    ni->setData(1, SARibbonCustomizeWidget::LevelRole);

    if (0 == level) {
        // 说明是category,插入到最后
        item->appendRow(ni);
    } else if (1 == level) {
        // 说明选择的是panel，插入到此panel之后
        QStandardItem* categoryItem = item->parent();
        if (nullptr == categoryItem) {
            return;
        }
        categoryItem->insertRow(item->row() + 1, ni);
    } else {
        // 不符合就删除退出
        delete ni;
        ni = nullptr;
        return;
    }
    // 查找category的object name
    QStandardItem* categoryItem = ni->parent();
    QString categoryObjName     = "";

    categoryObjName         = d_ptr->itemObjectName(categoryItem);
    SARibbonCustomizeData d = SARibbonCustomizeData::makeAddPanelCustomizeData(
        ni->text(), ni->row(), categoryObjName, SARibbonCustomizeWidget::PrivateData::makeRandomObjName("panel"));

    d_ptr->mCustomizeDatasCache.append(d);
    ni->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 有CustomizeRole，必有CanCustomizeRole
    ni->setData(true, SARibbonCustomizeWidget::CustomizeRole);
    ni->setData(d.panelObjNameValue, SARibbonCustomizeWidget::CustomizeObjNameRole);
    setSelectItem(ni);
}

void SARibbonCustomizeWidget::onPushButtonRenameClicked()
{
    QStandardItem* item = selectedItem();

    if (nullptr == item) {
        return;
    }
    bool ok;
    QString text = "";

    text = QInputDialog::getText(this, tr("rename"), tr("name:"), QLineEdit::Normal, item->text(), &ok);

    if (!ok || text.isEmpty()) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 改Category名
        QString cateObjName     = d_ptr->itemObjectName(item);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRenameCategoryCustomizeData(text, cateObjName);
        d_ptr->mCustomizeDatasCache.append(d);
    } else if (1 == level) {
        QString cateObjName     = d_ptr->itemObjectName(item->parent());
        QString panelObjName    = d_ptr->itemObjectName(item);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRenamePanelCustomizeData(text, cateObjName, panelObjName);
        d_ptr->mCustomizeDatasCache.append(d);
    } else {
        // action 不允许改名
        return;
    }
    item->setText(text);
}

void SARibbonCustomizeWidget::onPushButtonAddClicked()
{
    QAction* act        = selectedAction();
    QStandardItem* item = selectedItem();

    if ((nullptr == act) || (nullptr == item)) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 选中category不进行操作
        return;
    } else if (2 == level) {
        // 选中action，添加到这个action之后,把item设置为panel
        item = item->parent();
    }
    QString panelObjName    = d_ptr->itemObjectName(item);
    QString categoryObjName = d_ptr->itemObjectName(item->parent());
    QString key             = d_ptr->mActionMgr->key(act);

    SARibbonCustomizeData d = SARibbonCustomizeData::makeAddActionCustomizeData(
        key, d_ptr->mActionMgr, selectedRowProportion(), categoryObjName, panelObjName);

    d_ptr->mCustomizeDatasCache.append(d);

    QStandardItem* actItem = new QStandardItem(act->icon(), act->text());

    actItem->setData(2, SARibbonCustomizeWidget::LevelRole);
    actItem->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);  // 有CustomizeRole，必有CanCustomizeRole
    actItem->setData(true, SARibbonCustomizeWidget::CustomizeRole);
    actItem->setData(act->objectName(), SARibbonCustomizeWidget::CustomizeObjNameRole);
    actItem->setData(qintptr(act), SARibbonCustomizeWidget::PointerRole);  // 把action指针传入
    item->appendRow(actItem);
}

void SARibbonCustomizeWidget::onPushButtonDeleteClicked()
{
    QStandardItem* item = selectedItem();

    if (nullptr == item) {
        return;
    }
    if (!isItemCanCustomize(item)) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 删除category
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRemoveCategoryCustomizeData(d_ptr->itemObjectName(item));
        d_ptr->mCustomizeDatasCache.append(d);
    } else if (1 == level) {
        // 删除panel
        QString catObjName      = d_ptr->itemObjectName(item->parent());
        QString panelObjName    = d_ptr->itemObjectName(item);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeRemovePanelCustomizeData(catObjName, panelObjName);
        d_ptr->mCustomizeDatasCache.append(d);
    } else if (2 == level) {
        // 删除Action
        QString catObjName   = d_ptr->itemObjectName(item->parent()->parent());
        QString panelObjName = d_ptr->itemObjectName(item->parent());
        QAction* act         = itemToAction(item);
        QString key          = d_ptr->mActionMgr->key(act);
        if (key.isEmpty() || catObjName.isEmpty() || panelObjName.isEmpty()) {
            return;
        }

        SARibbonCustomizeData d =
            SARibbonCustomizeData::makeRemoveActionCustomizeData(catObjName, panelObjName, key, d_ptr->mActionMgr);
        d_ptr->mCustomizeDatasCache.append(d);
    }
    // 执行删除操作
    removeItem(item);
    // 删除后重新识别
    ui->pushButtonAdd->setEnabled(selectedAction() && isSelectedItemIsCustomize() && selectedRibbonLevel() > 0);
    ui->pushButtonDelete->setEnabled(isSelectedItemIsCustomize());
}

void SARibbonCustomizeWidget::onListViewSelectClicked(const QModelIndex& index)
{
    // 每次点击，判断是否可以进行操作，决定pushButtonAdd和pushButtonDelete的显示状态
    // 点击了listview，判断treeview的状态
    Q_UNUSED(index);
    ui->pushButtonAdd->setEnabled(isSelectedItemCanCustomize() && selectedRibbonLevel() > 0);
    ui->pushButtonDelete->setEnabled(isSelectedItemCanCustomize());
}

void SARibbonCustomizeWidget::onTreeViewResultClicked(const QModelIndex& index)
{
    Q_UNUSED(index);
    // 每次点击，判断是否可以进行操作，决定pushButtonAdd和pushButtonDelete的显示状态
    QStandardItem* item = selectedItem();

    if (nullptr == item) {
        return;
    }
    int level = itemLevel(item);

    ui->pushButtonAdd->setEnabled(selectedAction() && (level > 0) && isItemCanCustomize(item));
    ui->pushButtonDelete->setEnabled(isItemCanCustomize(item));  // 有CustomizeRole，必有CanCustomizeRole
    ui->pushButtonRename->setEnabled(
        level != 2 || isItemCanCustomize(item));  // QAction 不能改名 ， 有CustomizeRole，必有CanCustomizeRole
}

void SARibbonCustomizeWidget::onToolButtonUpClicked()
{
    QStandardItem* item = selectedItem();

    if ((nullptr == item) || (0 == item->row())) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 移动category
        SARibbonCustomizeData d =
            SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(d_ptr->itemObjectName(item), -1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = d_ptr->mRibbonModel->takeItem(r);
        d_ptr->mRibbonModel->removeRow(r);
        d_ptr->mRibbonModel->insertRow(r - 1, item);
    } else if (1 == level) {
        QStandardItem* paritem  = item->parent();
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangePanelOrderCustomizeData(
            d_ptr->itemObjectName(paritem), d_ptr->itemObjectName(item), -1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = paritem->takeChild(r);
        paritem->removeRow(r);
        paritem->insertRow(r - 1, item);
    } else if (2 == level) {
        QStandardItem* panelItem    = item->parent();
        QStandardItem* categoryItem = panelItem->parent();
        QAction* act                = itemToAction(item);
        if (!act) {
            return;
        }
        QString key             = d_ptr->mActionMgr->key(act);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeActionOrderCustomizeData(
            d_ptr->itemObjectName(categoryItem), d_ptr->itemObjectName(panelItem), key, d_ptr->mActionMgr, -1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = panelItem->takeChild(r);
        panelItem->removeRow(r);
        panelItem->insertRow(r - 1, item);
    }

    // 保持焦点，方便连续操作
    setSelectItem(item);
    onTreeViewResultClicked(item->index());
}

void SARibbonCustomizeWidget::onToolButtonDownClicked()
{
    QStandardItem* item = selectedItem();

    if (item == nullptr) {
        return;
    }
    int count = 0;

    if (item->parent()) {
        count = item->parent()->rowCount();
    } else {
        count = d_ptr->mRibbonModel->rowCount();
    }
    if ((nullptr == item) || ((count - 1) == item->row())) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        // 移动category
        SARibbonCustomizeData d =
            SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(d_ptr->itemObjectName(item), 1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = d_ptr->mRibbonModel->takeItem(item->row());
        d_ptr->mRibbonModel->removeRow(r);
        d_ptr->mRibbonModel->insertRow(r + 1, item);
    } else if (1 == level) {
        QStandardItem* paritem  = item->parent();
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangePanelOrderCustomizeData(
            d_ptr->itemObjectName(paritem), d_ptr->itemObjectName(item), 1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = paritem->takeChild(r);
        paritem->removeRow(r);
        paritem->insertRow(r + 1, item);
    } else if (2 == level) {
        QStandardItem* panelItem    = item->parent();
        QStandardItem* categoryItem = panelItem->parent();
        QAction* act                = itemToAction(item);
        if (!act) {
            return;
        }
        QString key             = d_ptr->mActionMgr->key(act);
        SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeActionOrderCustomizeData(
            d_ptr->itemObjectName(categoryItem), d_ptr->itemObjectName(panelItem), key, d_ptr->mActionMgr, -1);
        d_ptr->mCustomizeDatasCache.append(d);
        int r = item->row();
        item  = panelItem->takeChild(r);
        panelItem->removeRow(r);
        panelItem->insertRow(r + 1, item);
    }

    // 保持焦点，方便连续操作
    setSelectItem(item);
    onTreeViewResultClicked(item->index());
}

void SARibbonCustomizeWidget::onItemChanged(QStandardItem* item)
{
    if (item == nullptr) {
        return;
    }
    int level = itemLevel(item);

    if (0 == level) {
        if (item->isCheckable()) {
            QString objname = d_ptr->itemObjectName(item);
            SARibbonCustomizeData d =
                SARibbonCustomizeData::makeVisibleCategoryCustomizeData(objname, item->checkState() == Qt::Checked);
            d_ptr->mCustomizeDatasCache.append(d);
        }
    }
}

void SARibbonCustomizeWidget::onLineEditSearchActionTextEdited(const QString& text)
{
    d_ptr->mAcionModel->search(text);
}

void SARibbonCustomizeWidget::onPushButtonResetClicked()
{
    int btn = QMessageBox::warning(this,
                                   tr("Warning"),
                                   tr("Are you sure reset all customize setting?"),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);

    if (btn == QMessageBox::Yes) {
        clear();
    }
}
