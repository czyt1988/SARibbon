#ifndef SARIBBONCUSTOMIZEWIDGET_H
#define SARIBBONCUSTOMIZEWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include "SARibbonActionsManager.h"
#include "SARibbonPannel.h"
#include "SARibbonCustomizeData.h"


class SARibbonCustomizeWidgetUi;
class SARibbonCustomizeWidgetPrivate;
class SARibbonMainWindow;
class QStandardItemModel;
class QStandardItem;
class QAbstractButton;
//
class QXmlStreamWriter;
class QXmlStreamReader;

class SA_RIBBON_EXPORT SARibbonCustomizeWidget : public QWidget
{
	Q_OBJECT
public:
	SARibbonCustomizeWidget(SARibbonMainWindow *ribbonWindow, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~SARibbonCustomizeWidget();

	enum RibbonTreeShowType {
		ShowAllCategory
		, ShowMainCategory
	};

	enum ItemRole {
		LevelRole = Qt::UserRole + 1
		, PointerRole = Qt::UserRole + 2
		, CanCustomizeRole = Qt::UserRole + 3
		, CustomizeRole = Qt::UserRole + 4
		, CustomizeObjNameRole = Qt::UserRole + 5
	};



	void setupActionsManager(SARibbonActionsManager *mgr);

	bool isChanged() const;

	const QStandardItemModel *model() const;

	void updateModel();

	void updateModel(RibbonTreeShowType type);

	bool applys();

	bool toXml(QXmlStreamWriter *xml) const;
	bool toXml(const QString& xmlpath) const;

	void fromXml(QXmlStreamReader *xml);
	void fromXml(const QString& xmlpath);

	static bool fromXml(QXmlStreamReader *xml, SARibbonMainWindow *w, SARibbonActionsManager *mgr);

	void clear();


protected:
	void simplify();

	SARibbonPannelItem::RowProportion selectedRowProportion() const;

	QAction *selectedAction() const;
	QAction *itemToAction(QStandardItem *item) const;

	QStandardItem *selectedItem() const;


	int selectedRibbonLevel() const;

	int itemLevel(QStandardItem *item) const;

	void setSelectItem(QStandardItem *item, bool ensureVisible = true);

	bool isItemCanCustomize(QStandardItem *item) const;
	bool isSelectedItemCanCustomize() const;

	bool isCustomizeItem(QStandardItem *item) const;
	bool isSelectedItemIsCustomize() const;

	void removeItem(QStandardItem *item);

private slots:
	void onComboBoxActionIndexCurrentIndexChanged(int index);
	void onRadioButtonGroupButtonClicked(QAbstractButton *b);
	void onPushButtonNewCategoryClicked();
	void onPushButtonNewPannelClicked();
	void onPushButtonRenameClicked();
	void onPushButtonAddClicked();
	void onPushButtonDeleteClicked();
	void onListViewSelectClicked(const QModelIndex& index);
	void onTreeViewResultClicked(const QModelIndex& index);
	void onToolButtonUpClicked();
	void onToolButtonDownClicked();
	void onItemChanged(QStandardItem *item);
	void onLineEditSearchActionTextEdited(const QString& text);
	void onPushButtonResetClicked();

private:
	void initConnection();

private:
	SARibbonCustomizeWidgetUi *ui;
	SARibbonCustomizeWidgetPrivate *m_d;
};


bool SA_RIBBON_EXPORT sa_customize_datas_to_xml(QXmlStreamWriter *xml, const QList<SARibbonCustomizeData>& cds);

QList<SARibbonCustomizeData> SA_RIBBON_EXPORT sa_customize_datas_from_xml(QXmlStreamReader *xml, SARibbonActionsManager *mgr);

int SA_RIBBON_EXPORT sa_customize_datas_apply(const QList<SARibbonCustomizeData>& cds, SARibbonMainWindow *w);

/**
 * static bool has_call = false;
 * if (!has_call) {
 *     has_call = sa_apply_customize_from_xml_file("customize.xml", this, m_actMgr);
 * }
 * @endcode
 */
bool SA_RIBBON_EXPORT sa_apply_customize_from_xml_file(const QString& filePath, SARibbonMainWindow *w, SARibbonActionsManager *mgr);

#endif // SARIBBONCUSTOMIZEWIDGET_H
