#include "SARibbonCustomizeDialog.h"
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSpacerItem>

/**
 * @brief The SARibbonCustomizeDialogUi class
 */
class SARibbonCustomizeDialogUi {
public:
	SARibbonCustomizeWidget *customWidget;
	QVBoxLayout *verticalLayoutMain;
	QHBoxLayout *horizontalLayoutButtonGroup;
	QPushButton *pushButtonCancel;
	QPushButton *pushButtonOk;
	QSpacerItem *spacerItemleft;
	void setupUi(SARibbonMainWindow *ribbonWindow, QWidget *customizeDialog)
	{
		if (customizeDialog->objectName().isEmpty()) {
			customizeDialog->setObjectName(QStringLiteral("SARibbonCustomizeDialog"));
		}
		customizeDialog->resize(800, 600);
		verticalLayoutMain = new QVBoxLayout(customizeDialog);
		verticalLayoutMain->setObjectName(QStringLiteral("verticalLayoutMain"));

		customWidget = new SARibbonCustomizeWidget(ribbonWindow, customizeDialog);
		customWidget->setObjectName(QStringLiteral("customWidget"));
		verticalLayoutMain->addWidget(customWidget);

		horizontalLayoutButtonGroup = new QHBoxLayout();
		horizontalLayoutButtonGroup->setObjectName(QStringLiteral("horizontalLayoutButtonGroup"));

		spacerItemleft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		horizontalLayoutButtonGroup->addItem(spacerItemleft);

		pushButtonCancel = new QPushButton(customizeDialog);
		pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
		horizontalLayoutButtonGroup->addWidget(pushButtonCancel);

		pushButtonOk = new QPushButton(customizeDialog);
		pushButtonOk->setObjectName(QStringLiteral("pushButtonCancel"));
		horizontalLayoutButtonGroup->addWidget(pushButtonOk);
		verticalLayoutMain->addItem(horizontalLayoutButtonGroup);
		retranslateUi(customizeDialog);
	}


	void retranslateUi(QWidget *customizeDialog)
	{
		customizeDialog->setWindowTitle(QApplication::translate("SARibbonCustomizeDialog", "Customize Dialog", Q_NULLPTR));
		pushButtonCancel->setText(QApplication::translate("SARibbonCustomizeDialog", "Cancel", Q_NULLPTR));
		pushButtonOk->setText(QApplication::translate("SARibbonCustomizeDialog", "OK", Q_NULLPTR));
	}
};


////////////////////////////////////////////////////////////////////

SARibbonCustomizeDialog::SARibbonCustomizeDialog(SARibbonMainWindow *ribbonWindow, QWidget *p, Qt::WindowFlags f)
	: QDialog(p, f)
	, ui(new SARibbonCustomizeDialogUi)
{
	ui->setupUi(ribbonWindow, this);
	initConnection();
}


void SARibbonCustomizeDialog::setupActionsManager(SARibbonActionsManager *mgr)
{
	ui->customWidget->setupActionsManager(mgr);
}


void SARibbonCustomizeDialog::initConnection()
{
	connect(ui->pushButtonOk, &QPushButton::clicked, this, &QDialog::accept);
	connect(ui->pushButtonCancel, &QPushButton::clicked, this, &QDialog::reject);
}


bool SARibbonCustomizeDialog::applys()
{
	return (ui->customWidget->applys());
}


void SARibbonCustomizeDialog::clear()
{
	ui->customWidget->clear();
}


bool SARibbonCustomizeDialog::toXml(QXmlStreamWriter *xml) const
{
	return (ui->customWidget->toXml(xml));
}


bool SARibbonCustomizeDialog::toXml(const QString& xmlpath) const
{
	return (ui->customWidget->toXml(xmlpath));
}


void SARibbonCustomizeDialog::fromXml(QXmlStreamReader *xml)
{
	ui->customWidget->fromXml(xml);
}


void SARibbonCustomizeDialog::fromXml(const QString& xmlpath)
{
	ui->customWidget->fromXml(xmlpath);
}


SARibbonCustomizeWidget *SARibbonCustomizeDialog::customizeWidget() const
{
	return (ui->customWidget);
}
