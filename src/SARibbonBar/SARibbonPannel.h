#ifndef SARIBBONPANNEL_H
#define SARIBBONPANNEL_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QLayout>
#include "SARibbonToolButton.h"
#include "SARibbonPannelLayout.h"
#include "SARibbonPannelItem.h"
class SARibbonMenu;
class SARibbonGallery;
class QGridLayout;
class SARibbonPannelOptionButton;
class SARibbonPannelPrivate;


class SA_RIBBON_EXPORT SARibbonPannel : public QWidget
{
	Q_OBJECT
		friend class SARibbonCategory;
	friend class SARibbonCategoryPrivate;
	friend class SARibbonCustomizeWidgetPrivate;
	friend class SARibbonCustomizeWidget_2Private;
	Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
		Q_PROPERTY(bool isExpanding READ isExpanding WRITE setExpanding)
		Q_PROPERTY(QString pannelName READ pannelName WRITE setPannelName)
public:
	SARibbonPannel(QWidget *parent = nullptr);
	SARibbonPannel(const QString& name, QWidget *parent = nullptr);
	~SARibbonPannel() Q_DECL_OVERRIDE;
	using QWidget::addAction;
	enum PannelLayoutMode {
		ThreeRowMode
		, TwoRowMode
	};

	static void setActionRowProportionProperty(QAction *action, SARibbonPannelItem::RowProportion rp);

	static SARibbonPannelItem::RowProportion getActionRowProportionProperty(QAction *action);

	void setActionRowProportion(QAction *action, SARibbonPannelItem::RowProportion rp);

	SARibbonToolButton *addAction(QAction *action, SARibbonPannelItem::RowProportion rp);

	SARibbonToolButton *addLargeAction(QAction *action);

	SARibbonToolButton *addMediumAction(QAction *action);

	SARibbonToolButton *addSmallAction(QAction *action);

	QAction *addAction(const QString& text, const QIcon& icon
		, QToolButton::ToolButtonPopupMode popMode
		, SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::Large);

	SARibbonToolButton *addMenu(QMenu *menu
		, SARibbonPannelItem::RowProportion rp
		, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

	SARibbonToolButton *addActionMenu(QAction *action, QMenu *menu, SARibbonPannelItem::RowProportion rp);

	SARibbonToolButton *addLargeActionMenu(QAction *action, QMenu *menu);

	SARibbonToolButton *addLargeMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

	SARibbonToolButton *addSmallMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

	QAction *addWidget(QWidget *w, SARibbonPannelItem::RowProportion rp);

	QAction *addSmallWidget(QWidget *w);

	QAction *addLargeWidget(QWidget *w);

	SARibbonGallery *addGallery();

	QAction *addSeparator(int top = 6, int bottom = 6);

	SARibbonToolButton *actionToRibbonToolButton(QAction *action);

	void addOptionAction(QAction *action);

	bool isHaveOptionAction() const;

	QList<SARibbonToolButton *> ribbonToolButtons() const;

	PannelLayoutMode pannelLayoutMode() const;

	bool isTwoRow() const { return (TwoRowMode == pannelLayoutMode()); }

	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
	virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

	void setExpanding(bool isExpanding = true);

	bool isExpanding() const;

	virtual int titleHeight() const;

	virtual QSize optionActionButtonSize() const;

	int actionIndex(QAction *act) const;

	void moveAction(int from, int to);

	bool isCanCustomize() const;
	void setCanCustomize(bool b);
	QString pannelName() const;
	void setPannelName(const QString& title);

signals:

	void actionTriggered(QAction *action);

protected:
	void setPannelLayoutMode(PannelLayoutMode mode);
	void resetLayout(PannelLayoutMode newmode);
	void resetLargeToolButtonStyle();
	static QSize maxHightIconSize(const QSize& size, int h);
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	virtual void actionEvent(QActionEvent *e) Q_DECL_OVERRIDE;

	const QList<SARibbonPannelItem *>& ribbonPannelItem() const;

private:
	SARibbonPannelPrivate *m_d;
};



#endif // SARIBBONPANNEL_H
