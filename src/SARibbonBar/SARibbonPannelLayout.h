#ifndef SARIBBONPANNELLAYOUT_H
#define SARIBBONPANNELLAYOUT_H
#include "SARibbonGlobal.h"
#include <QLayout>
#include "SARibbonPannelItem.h"
class SARibbonPannel;



class SA_RIBBON_EXPORT SARibbonPannelLayout : public QLayout
{
	Q_OBJECT
		friend class SARibbonPannel;
public:

	SARibbonPannelLayout(QWidget *p = 0);
	~SARibbonPannelLayout();
	virtual int indexOf(QAction *action) const;

	void addItem(QLayoutItem *item) Q_DECL_OVERRIDE;

	void insertAction(int index, QAction *act, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::None);

	//
	QLayoutItem *itemAt(int index) const Q_DECL_OVERRIDE;
	QLayoutItem *takeAt(int index) Q_DECL_OVERRIDE;
	int count() const Q_DECL_OVERRIDE;

	bool isEmpty() const Q_DECL_OVERRIDE;
	void invalidate() Q_DECL_OVERRIDE;
	Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;

	void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
	QSize minimumSize() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;

	SARibbonPannelItem *pannelItem(QAction *action) const;

	SARibbonPannelItem *lastItem() const;

	QWidget *lastWidget() const;

	int defaultPannelHeight() const;

	void move(int from, int to);

	bool isDirty() const;

protected:
	void layoutActions();

	SARibbonPannelItem *createItem(QAction *action, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::None);
	void updateGeomArray(const QRect& setrect);

	void recalcExpandGeomArray(const QRect& setrect);

private:
	void columnWidthInfo(int colindex, int& width, int& maximum) const;

private:
	QList<SARibbonPannelItem *> m_items;
	int m_columnCount;
	bool m_expandFlag;
	QSize m_sizeHint;
	bool m_dirty;
	int m_largeHeight;
};

#endif // SARIBBONPANNELLAYOUT_H
