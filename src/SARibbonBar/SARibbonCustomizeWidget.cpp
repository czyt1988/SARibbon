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
#include "SARibbonPannel.h"
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
///SARibbonCustomizeWidget
////////////////////////////////////////////////////////////////////////////////////////////////////////


bool sa_customize_datas_to_xml(QXmlStreamWriter *xml, const QList<SARibbonCustomizeData>& cds)
{
	if (cds.size() <= 0) {
		return (false);
	}

	xml->writeStartElement("sa-ribbon-customize");
	for (const SARibbonCustomizeData& d : cds)
	{
		xml->writeStartElement("customize-data");
		xml->writeAttribute("type", QString::number(d.actionType()));
		xml->writeAttribute("index", QString::number(d.indexValue));
		xml->writeAttribute("key", d.keyValue);
		xml->writeAttribute("category", d.categoryObjNameValue);
		xml->writeAttribute("pannel", d.pannelObjNameValue);
		xml->writeAttribute("row-prop", QString::number(d.actionRowProportionValue));

		xml->writeEndElement();
	}
	xml->writeEndElement();
	if (xml->hasError()) {
		qWarning() << "write has error";
	}
	return (true);
}


QList<SARibbonCustomizeData> sa_customize_datas_from_xml(QXmlStreamReader *xml, SARibbonActionsManager *mgr)
{
	while (!xml->atEnd())
	{
		qDebug() << "name:" << xml->name() << " qualifiedName:" << xml->qualifiedName();

		if (xml->isStartElement() && (xml->name().toString() == "sa-ribbon-customize")) {
			break;
		}
		xml->readNext();
	}
	QList<SARibbonCustomizeData> res;

	while (!xml->atEnd())
	{
		if (xml->isStartElement() && (xml->name().toString() == "customize-data")) {
			SARibbonCustomizeData d;
			QXmlStreamAttributes attrs = xml->attributes();
			if (!attrs.hasAttribute("type")) {
				xml->readNextStartElement();
				continue;
			}
			bool isOk = false;
			int v = xml->attributes().value("type").toInt(&isOk);
			if (!isOk) {
				xml->readNextStartElement();
				continue;
			}
			d.setActionType(static_cast<SARibbonCustomizeData::ActionType>(v));
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
			if (attrs.hasAttribute("pannel")) {
				d.pannelObjNameValue = attrs.value("pannel").toString();
			}
			if (attrs.hasAttribute("row-prop")) {
				v = xml->attributes().value("row-prop").toInt(&isOk);
				if (isOk) {
					d.actionRowProportionValue = static_cast<SARibbonPannelItem::RowProportion>(v);
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


int sa_customize_datas_apply(const QList<SARibbonCustomizeData>& cds, SARibbonMainWindow *w)
{
	int c = 0;

	for (const SARibbonCustomizeData& d : cds)
	{
		if (d.apply(w)) {
			++c;
		}
	}
	return (c);
}


bool sa_apply_customize_from_xml_file(const QString& filePath, SARibbonMainWindow *w, SARibbonActionsManager *mgr)
{
	QFile f(filePath);

	if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return (false);
	}
	f.seek(0);
	QXmlStreamReader xml(&f);

	return (SARibbonCustomizeWidget::fromXml(&xml, w, mgr));
}


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
	QPushButton *pushButtonReset;
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
		treeViewResult->setAnimated(true);
		treeViewResult->setEditTriggers(QAbstractItemView::NoEditTriggers);


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
	} // setupUi


	void retranslateUi(QWidget *customizeWidget)
	{
		//customizeWidget->setWindowTitle(QApplication::translate("SARibbonCustomizeWidget", "Customize Widget", Q_NULLPTR));
		customizeWidget->setWindowTitle("Customize Widget");

		//labelSelectAction->setText(QApplication::translate("SARibbonCustomizeWidget", "\344\273\216\344\270\213\345\210\227\351\200\211\351\241\271\345\215\241\351\200\211\346\213\251\345\221\275\344\273\244\357\274\232", Q_NULLPTR));
		labelSelectAction->setText("Choose commands from the following tabs:");

		lineEditSearchAction->setInputMask(QString());
		lineEditSearchAction->setText(QString());

		//lineEditSearchAction->setPlaceholderText(QApplication::translate("SARibbonCustomizeWidget", "\346\237\245\346\211\276\345\221\275\344\273\244", Q_NULLPTR));
		lineEditSearchAction->setPlaceholderText("find command");

		//pushButtonAdd->setText(QApplication::translate("SARibbonCustomizeWidget", "\346\267\273\345\212\240 >>", Q_NULLPTR));
		pushButtonAdd->setText("add>>");

		//pushButtonDelete->setText(QApplication::translate("SARibbonCustomizeWidget", "<< \345\210\240\351\231\244", Q_NULLPTR));
		pushButtonDelete->setText("<< delete");

		//labelCustomize->setText(QApplication::translate("SARibbonCustomizeWidget", "\350\207\252\345\256\232\344\271\211\345\212\237\350\203\275\345\214\272\357\274\232", Q_NULLPTR));
		labelCustomize->setText("Customize the Ribbon:");
		//radioButtonMainCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "\344\270\273\351\200\211\351\241\271\345\215\241", Q_NULLPTR));
		radioButtonMainCategory->setText("main tab");

		//radioButtonAllCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "\346\211\200\346\234\211\351\200\211\351\241\271\345\215\241", Q_NULLPTR));
		radioButtonAllCategory->setText("All tabs");

		//pushButtonNewCategory->setText(QApplication::translate("SARibbonCustomizeWidget", "\346\226\260\345\273\272\351\200\211\351\241\271\345\215\241", Q_NULLPTR));
		pushButtonNewCategory->setText("New tab");

		//pushButtonNewPannel->setText(QApplication::translate("SARibbonCustomizeWidget", "\346\226\260\345\273\272\347\273\204", Q_NULLPTR));
		pushButtonNewPannel->setText("New group");

		//pushButtonRename->setText(QApplication::translate("SARibbonCustomizeWidget", "\351\207\215\345\221\275\345\220\215", Q_NULLPTR));
		pushButtonRename->setText("Rename");

		//pushButtonReset->setText(QApplication::translate("SARibbonCustomizeWidget", "reset", Q_NULLPTR));
		pushButtonReset->setText("reset");

		//labelProportion->setText(QApplication::translate("SARibbonCustomizeWidget", "proportion:", Q_NULLPTR));
		labelProportion->setText("proportion");
	} // retranslateUi
};

class SARibbonCustomizeWidgetPrivate
{
public:
	SARibbonCustomizeWidget *mParent;
	SARibbonCustomizeWidget::RibbonTreeShowType mShowType;
	SARibbonMainWindow *mRibbonWindow;
	SARibbonActionsManager *mActionMgr;
	SARibbonActionsManagerModel *mAcionModel;
	QStandardItemModel *mRibbonModel;
	int mCustomizeCategoryCount;
	int mCustomizePannelCount;
	SARibbonCustomizeWidgetPrivate(SARibbonCustomizeWidget *p);
	void updateModel();

	QList<SARibbonCustomizeData> mCustomizeDatas;
	QList<SARibbonCustomizeData> mOldCustomizeDatas;
	//：pre_QDateTime::currentMSecsSinceEpoch_suf
	static QString makeRandomObjName(const QString& pre);

	int itemLevel(QStandardItem *item) const;

	//
	bool isCustomizeItem(QStandardItem *item) const;

	SARibbonCategory *itemToCategory(QStandardItem *item) const;

	SARibbonPannel *itemToPannel(QStandardItem *item) const;

	QString itemObjectName(QStandardItem *item) const;

	bool isItemCanCustomize(QStandardItem *item) const;

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
			continue;
		}
		QStandardItem *ci = new QStandardItem();
		if (c->isContextCategory()) {
			ci->setText(QString("[%1]").arg(c->windowTitle()));
		}
		else {
			ci->setText(c->windowTitle());
		}
		if (c->isCanCustomize() && !c->isContextCategory()) {
			ci->setCheckable(true);
			ci->setCheckState(ribbonbar->isCategoryVisible(c) ? Qt::Checked : Qt::Unchecked);
			ci->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);
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
				pi->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);
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
					if (i->widget()->windowTitle().isEmpty() && i->widget()->windowIcon().isNull()) {
						delete ii;
						continue;
					}
					ii->setText(i->widget()->windowTitle());
					ii->setIcon(i->widget()->windowIcon());
					if (SARibbonCustomizeData::isCanCustomize(i->widget())) {
						ii->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);
					}
				}
				else {
					ii->setText(i->action->text());
					ii->setIcon(i->action->icon());
					if (SARibbonCustomizeData::isCanCustomize(i->action)) {
						ii->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);
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


QString SARibbonCustomizeWidgetPrivate::makeRandomObjName(const QString& pre)
{
	return (QString("%1_%2").arg(pre).arg(QDateTime::currentMSecsSinceEpoch()));
}


int SARibbonCustomizeWidgetPrivate::itemLevel(QStandardItem *item) const
{
	return (item->data(SARibbonCustomizeWidget::LevelRole).toInt());
}


bool SARibbonCustomizeWidgetPrivate::isCustomizeItem(QStandardItem *item) const
{
	if (nullptr == item) {
		return (false);
	}
	return (item->data(SARibbonCustomizeWidget::CustomizeRole).isValid());
}


SARibbonCategory *SARibbonCustomizeWidgetPrivate::itemToCategory(QStandardItem *item) const
{
	int level = item->data(SARibbonCustomizeWidget::LevelRole).toInt();

	if (level != 0) {
		return (nullptr);
	}
	qintptr p = item->data(SARibbonCustomizeWidget::PointerRole).value<qintptr>();

	return (reinterpret_cast<SARibbonCategory *>(p));
}


SARibbonPannel *SARibbonCustomizeWidgetPrivate::itemToPannel(QStandardItem *item) const
{
	int level = item->data(SARibbonCustomizeWidget::LevelRole).toInt();

	if (level != 1) {
		return (nullptr);
	}
	qintptr p = item->data(SARibbonCustomizeWidget::PointerRole).value<qintptr>();

	return (reinterpret_cast<SARibbonPannel *>(p));
}


QString SARibbonCustomizeWidgetPrivate::itemObjectName(QStandardItem *item) const
{
	QString objName;

	if (isCustomizeItem(item)) {
		objName = item->data(SARibbonCustomizeWidget::CustomizeObjNameRole).toString();
	}
	else {
		int level = itemLevel(item);
		if (0 == level) {
			SARibbonCategory *category = itemToCategory(item);
			if (category) {
				objName = category->objectName();
			}
		}
		else if (1 == level) {
			SARibbonPannel *pannel = itemToPannel(item);
			if (pannel) {
				objName = pannel->objectName();
			}
		}
	}
	return (objName);
}


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


QAction *SARibbonCustomizeWidgetPrivate::itemToAction(QStandardItem *item) const
{
	if (2 != itemLevel(item)) {
		return (nullptr);
	}
	QAction *act = nullptr;

	if (item->data(SARibbonCustomizeWidget::CustomizeRole).toBool()) {
		act = reinterpret_cast<QAction *>(item->data(SARibbonCustomizeWidget::PointerRole).value<qintptr>());
	}
	else {
		SARibbonPannelItem *pi = reinterpret_cast<SARibbonPannelItem *>(item->data(SARibbonCustomizeWidget::PointerRole).value<qintptr>());
		act = (pi->action);
	}
	return (act);
}


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
	connect(ui->comboBoxActionIndex, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
		, this, &SARibbonCustomizeWidget::onComboBoxActionIndexCurrentIndexChanged);
	//    connect(ui->radioButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked)
	//        , this, &SARibbonCustomizeWidget::onRadioButtonGroupButtonClicked);
	connect(ui->radioButtonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked)
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
	connect(ui->lineEditSearchAction, &QLineEdit::textEdited
		, this, &SARibbonCustomizeWidget::onLineEditSearchActionTextEdited);
	connect(ui->pushButtonReset, &QPushButton::clicked
		, this, &SARibbonCustomizeWidget::onPushButtonResetClicked);
}

void SARibbonCustomizeWidget::setupActionsManager(SARibbonActionsManager *mgr)
{
	m_d->mActionMgr = mgr;
	if (m_d->mActionMgr) {
		m_d->mAcionModel->uninstallActionsManager();
	}
	m_d->mAcionModel->setupActionsManager(mgr);
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


const QStandardItemModel *SARibbonCustomizeWidget::model() const
{
	return (m_d->mRibbonModel);
}


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
			}
			else {
				ui->comboBoxActionProportion->addItem(tr("large"), SARibbonPannelItem::Large);
				ui->comboBoxActionProportion->addItem(tr("medium"), SARibbonPannelItem::Medium);
				ui->comboBoxActionProportion->addItem(tr("small"), SARibbonPannelItem::Small);
			}
		}
	}
}


void SARibbonCustomizeWidget::updateModel(RibbonTreeShowType type)
{
	m_d->mShowType = type;
	m_d->updateModel();
}


bool SARibbonCustomizeWidget::applys()
{
	simplify();
	return (sa_customize_datas_apply(m_d->mCustomizeDatas, m_d->mRibbonWindow) > 0);
}


/**
 * dlg.setupActionsManager(m_actMgr);
 * if (SARibbonCustomizeDialog::Accepted == dlg.exec()) {
 *    dlg.applys();
 *    QByteArray str;
 *    QXmlStreamWriter xml(&str);//QXmlStreamWriter
 *    xml.setAutoFormatting(true);
 *    xml.setAutoFormattingIndent(2);
 *    xml.setCodec("utf-8");//writeStartDocument
 *    xml.writeStartDocument();
 *    bool isok = dlg.toXml(&xml);
 *    xml.writeEndDocument();
 *    if (isok) {
 *        QFile f("customize.xml");
 *        if (f.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Truncate)) {
 *            QTextStream s(&f);
 *            s.setCodec("utf-8");
 *            s << str;
 *            s.flush();
 *        }
 *        m_edit->append("write xml:");//m_edit
 *        m_edit->append(str);
 *    }
 * }
 * @endcode
 * @see sa_customize_datas_to_xml
 */
bool SARibbonCustomizeWidget::toXml(QXmlStreamWriter *xml) const
{
	QList<SARibbonCustomizeData> res;

	res = m_d->mOldCustomizeDatas + m_d->mCustomizeDatas;
	res = SARibbonCustomizeData::simplify(res);
	return (sa_customize_datas_to_xml(xml, res));
}

bool SARibbonCustomizeWidget::toXml(const QString& xmlpath) const
{
	QFile f(xmlpath);

	if (!f.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
		return (false);
	}
	QXmlStreamWriter xml(&f);

	xml.setAutoFormatting(true);
	xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0) // QXmlStreamWriter always encodes XML in UTF-8.
	xml.setCodec("utf-8");//writeStartDocument
#endif
	xml.writeStartDocument();
	bool isOK = toXml(&xml);

	xml.writeEndDocument();
	f.close();
	return (isOK);
}

void SARibbonCustomizeWidget::fromXml(QXmlStreamReader *xml)
{
	QList<SARibbonCustomizeData> cds = sa_customize_datas_from_xml(xml, m_d->mActionMgr);

	m_d->mOldCustomizeDatas = cds;
}

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
 * static bool has_call = false;
 * if (!has_call) {
 *     QFile f("customize.xml");
 *     if (!f.open(QIODevice::ReadWrite|QIODevice::Text)) {
 *         return;
 *     }
 *     f.seek(0);
 *     QXmlStreamReader xml(&f);
 *     has_call = SARibbonCustomizeWidget::fromXml(&xml, this, m_actMgr);
 * }
 * @endcode
 * @param xml
 * @param w
 * @see sa_customize_datas_from_xml sa_customize_datas_apply sa_apply_customize_from_xml_file
 */
bool SARibbonCustomizeWidget::fromXml(QXmlStreamReader *xml, SARibbonMainWindow *w, SARibbonActionsManager *mgr)
{
	QList<SARibbonCustomizeData> cds = sa_customize_datas_from_xml(xml, mgr);

	return (sa_customize_datas_apply(cds, w) > 0);
}


void SARibbonCustomizeWidget::clear()
{
	m_d->mCustomizeDatas.clear();
}


void SARibbonCustomizeWidget::simplify()
{
	m_d->mCustomizeDatas = SARibbonCustomizeData::simplify(m_d->mCustomizeDatas);
}


SARibbonPannelItem::RowProportion SARibbonCustomizeWidget::selectedRowProportion() const
{
	return (static_cast<SARibbonPannelItem::RowProportion>(ui->comboBoxActionProportion->currentData().toInt()));
}


QAction *SARibbonCustomizeWidget::selectedAction() const
{
	QItemSelectionModel *m = ui->listViewSelect->selectionModel();

	if ((nullptr == m) || !m->hasSelection()) {
		return (nullptr);
	}
	QModelIndex i = m->currentIndex();

	return (m_d->mAcionModel->indexToAction(i));
}


QAction *SARibbonCustomizeWidget::itemToAction(QStandardItem *item) const
{
	return (m_d->itemToAction(item));
}


QStandardItem *SARibbonCustomizeWidget::selectedItem() const
{
	QItemSelectionModel *m = ui->treeViewResult->selectionModel();

	if ((nullptr == m) || !m->hasSelection()) {
		return (nullptr);
	}
	QModelIndex i = m->currentIndex();

	return (m_d->mRibbonModel->itemFromIndex(i));
}


int SARibbonCustomizeWidget::selectedRibbonLevel() const
{
	QStandardItem *item = selectedItem();

	if (item) {
		return (itemLevel(item));
	}
	return (-1);
}


int SARibbonCustomizeWidget::itemLevel(QStandardItem *item) const
{
	return (m_d->itemLevel(item));
}


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


bool SARibbonCustomizeWidget::isItemCanCustomize(QStandardItem *item) const
{
	return (m_d->isItemCanCustomize(item));
}


bool SARibbonCustomizeWidget::isSelectedItemCanCustomize() const
{
	return (isItemCanCustomize(selectedItem()));
}


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
	}
	else {
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
		row = i.row() + 1;
	}
	QStandardItem *ni = new QStandardItem(tr("new category[customize]%1").arg(++(m_d->mCustomizeCategoryCount)));

	ni->setData(0, SARibbonCustomizeWidget::LevelRole);
	m_d->mRibbonModel->insertRow(row, ni);
	setSelectItem(ni);
	SARibbonCustomizeData d = SARibbonCustomizeData::makeAddCategoryCustomizeData(ni->text()
		, ni->row()
		, SARibbonCustomizeWidgetPrivate::makeRandomObjName("category"));

	m_d->mCustomizeDatas.append(d);
	ni->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);
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
		item->appendRow(ni);
	}
	else if (1 == level) {
		QStandardItem *categoryItem = item->parent();
		if (nullptr == categoryItem) {
			return;
		}
		categoryItem->insertRow(item->row() + 1, ni);
	}
	else {
		delete ni;
		ni = nullptr;
		return;
	}
	QStandardItem *categoryItem = ni->parent();
	QString categoryObjName = "";


	categoryObjName = m_d->itemObjectName(categoryItem);
	SARibbonCustomizeData d = SARibbonCustomizeData::makeAddPannelCustomizeData(ni->text()
		, ni->row()
		, categoryObjName
		, SARibbonCustomizeWidgetPrivate::makeRandomObjName("pannel"));

	m_d->mCustomizeDatas.append(d);
	ni->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);
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
		QString cateObjName = m_d->itemObjectName(item);
		SARibbonCustomizeData d = SARibbonCustomizeData::makeRenameCategoryCustomizeData(text, cateObjName);
		m_d->mCustomizeDatas.append(d);
	}
	else if (1 == level) {
		QString cateObjName = m_d->itemObjectName(item->parent());
		QString pannelObjName = m_d->itemObjectName(item);
		SARibbonCustomizeData d = SARibbonCustomizeData::makeRenamePannelCustomizeData(text, cateObjName, pannelObjName);
		m_d->mCustomizeDatas.append(d);
	}
	else {
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
		return;
	}
	else if (2 == level) {
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
	actItem->setData(true, SARibbonCustomizeWidget::CanCustomizeRole);
	actItem->setData(true, SARibbonCustomizeWidget::CustomizeRole);
	actItem->setData(act->objectName(), SARibbonCustomizeWidget::CustomizeObjNameRole);
	actItem->setData(qintptr(act), SARibbonCustomizeWidget::PointerRole);
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
		SARibbonCustomizeData d = SARibbonCustomizeData::makeRemoveCategoryCustomizeData(m_d->itemObjectName(item));
		m_d->mCustomizeDatas.append(d);
	}
	else if (1 == level) {
		QString catObjName = m_d->itemObjectName(item->parent());
		QString pannelObjName = m_d->itemObjectName(item);
		SARibbonCustomizeData d = SARibbonCustomizeData::makeRemovePannelCustomizeData(catObjName, pannelObjName);
		m_d->mCustomizeDatas.append(d);
	}
	else if (2 == level) {
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
	removeItem(item);
	ui->pushButtonAdd->setEnabled(selectedAction() && isSelectedItemIsCustomize() && selectedRibbonLevel() > 0);
	ui->pushButtonDelete->setEnabled(isSelectedItemIsCustomize());
}


void SARibbonCustomizeWidget::onListViewSelectClicked(const QModelIndex& index)
{
	Q_UNUSED(index);
	ui->pushButtonAdd->setEnabled(isSelectedItemCanCustomize() && selectedRibbonLevel() > 0);
	ui->pushButtonDelete->setEnabled(isSelectedItemCanCustomize());
}


void SARibbonCustomizeWidget::onTreeViewResultClicked(const QModelIndex& index)
{
	Q_UNUSED(index);
	QStandardItem *item = selectedItem();

	if (nullptr == item) {
		return;
	}
	int level = itemLevel(item);

	ui->pushButtonAdd->setEnabled(selectedAction() && (level > 0) && isItemCanCustomize(item));
	ui->pushButtonDelete->setEnabled(isItemCanCustomize(item));
	ui->pushButtonRename->setEnabled(level != 2 || isItemCanCustomize(item));
}


void SARibbonCustomizeWidget::onToolButtonUpClicked()
{
	QStandardItem *item = selectedItem();

	if ((nullptr == item) || (0 == item->row())) {
		return;
	}
	int level = itemLevel(item);

	if (0 == level) {
		SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(m_d->itemObjectName(item), -1);
		m_d->mCustomizeDatas.append(d);
		int r = item->row();
		item = m_d->mRibbonModel->takeItem(r);
		m_d->mRibbonModel->removeRow(r);
		m_d->mRibbonModel->insertRow(r - 1, item);
	}
	else if (1 == level) {
		QStandardItem *paritem = item->parent();
		SARibbonCustomizeData d = SARibbonCustomizeData::makeChangePannelOrderCustomizeData(
			m_d->itemObjectName(paritem)
			, m_d->itemObjectName(item)
			, -1);
		m_d->mCustomizeDatas.append(d);
		int r = item->row();
		item = paritem->takeChild(r);
		paritem->removeRow(r);
		paritem->insertRow(r - 1, item);
	}
	else if (2 == level) {
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
		pannelItem->insertRow(r - 1, item);
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
	}
	else {
		count = m_d->mRibbonModel->rowCount();
	}
	if ((nullptr == item) || ((count - 1) == item->row())) {
		return;
	}
	int level = itemLevel(item);

	if (0 == level) {
		SARibbonCustomizeData d = SARibbonCustomizeData::makeChangeCategoryOrderCustomizeData(m_d->itemObjectName(item), 1);
		m_d->mCustomizeDatas.append(d);
		int r = item->row();
		item = m_d->mRibbonModel->takeItem(item->row());
		m_d->mRibbonModel->removeRow(r);
		m_d->mRibbonModel->insertRow(r + 1, item);
	}
	else if (1 == level) {
		QStandardItem *paritem = item->parent();
		SARibbonCustomizeData d = SARibbonCustomizeData::makeChangePannelOrderCustomizeData(
			m_d->itemObjectName(paritem)
			, m_d->itemObjectName(item)
			, 1);
		m_d->mCustomizeDatas.append(d);
		int r = item->row();
		item = paritem->takeChild(r);
		paritem->removeRow(r);
		paritem->insertRow(r + 1, item);
	}
	else if (2 == level) {
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
		pannelItem->insertRow(r + 1, item);
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


void SARibbonCustomizeWidget::onLineEditSearchActionTextEdited(const QString& text)
{
	m_d->mAcionModel->search(text);
}


void SARibbonCustomizeWidget::onPushButtonResetClicked()
{
	int btn = QMessageBox::warning(this, tr("Warning"), tr("Are you sure reset all customize setting?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

	if (btn == QMessageBox::Yes) {
		clear();
	}
}
