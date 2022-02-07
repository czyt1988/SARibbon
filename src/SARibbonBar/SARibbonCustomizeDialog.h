#ifndef SARIBBONCUSTOMIZEDIALOG_H
#define SARIBBONCUSTOMIZEDIALOG_H
#include "SARibbonGlobal.h"
#include <QDialog>
#include "SARibbonCustomizeWidget.h"
class SARibbonActionsManager;
class SARibbonCustomizeDialogUi;
class QXmlStreamWriter;

class SA_RIBBON_EXPORT SARibbonCustomizeDialog : public QDialog
{
	Q_OBJECT
public:
	SARibbonCustomizeDialog(SARibbonMainWindow *ribbonWindow, QWidget *p = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	void setupActionsManager(SARibbonActionsManager *mgr);

	bool applys();

	void clear();

	bool toXml(QXmlStreamWriter *xml) const;
	bool toXml(const QString& xmlpath) const;

	void fromXml(QXmlStreamReader *xml);
	void fromXml(const QString& xmlpath);

	SARibbonCustomizeWidget *customizeWidget() const;

private:
	void initConnection();

	SARibbonCustomizeDialogUi *ui;
};

#endif // SARIBBONCUSTOMIZEDIALOG_H
