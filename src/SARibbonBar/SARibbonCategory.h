#ifndef SARIBBONCATEGORY_H
#define SARIBBONCATEGORY_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include "SARibbonPannel.h"
#include <QScopedPointer>
#include <QPushButton>
#include <QWheelEvent>
class SARibbonCategoryProxyPrivate;
class SARibbonCategoryPrivate;
class QHBoxLayout;
class QWheelEvent;
class SARibbonBar;


class SA_RIBBON_EXPORT SARibbonCategory : public QWidget
{
	Q_OBJECT
		friend class SARibbonBar;
	friend class SARibbonContextCategory;
	Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
		Q_PROPERTY(QString categoryName READ categoryName WRITE setCategoryName)
public:
	SARibbonCategory(QWidget *p = nullptr);
	~SARibbonCategory();

	//category
	QString categoryName() const;

	void setCategoryName(const QString& title);

	SARibbonPannel::PannelLayoutMode ribbonPannelLayoutMode() const;

	//pannel
	SARibbonPannel *addPannel(const QString& title);

	//pannel
	void addPannel(SARibbonPannel *pannel);

	//qt designer
	Q_INVOKABLE void addPannel(QWidget *pannel);

	//pannel
	SARibbonPannel *insertPannel(const QString& title, int index);

	//pannel
	SARibbonPannel *pannelByName(const QString& title) const;

	//ObjectName,pannel
	SARibbonPannel *pannelByObjectName(const QString& objname) const;

	SARibbonPannel *pannelByIndex(int index) const;

	int pannelIndex(SARibbonPannel *p) const;

	void movePannel(int from, int to);


	bool takePannel(SARibbonPannel *pannel);

	bool removePannel(SARibbonPannel *pannel);
	bool removePannel(int index);

	void setBackgroundBrush(const QBrush& brush);

	QList<SARibbonPannel *> pannelList() const;

	//
	QSize sizeHint() const Q_DECL_OVERRIDE;

	bool isContextCategory() const;

	int pannelCount() const;

	bool isCanCustomize() const;
	void setCanCustomize(bool b);

	SARibbonBar *ribbonBar() const;

protected slots:
	void onLeftScrollButtonClicked();
	void onRightScrollButtonClicked();

protected:
	bool event(QEvent *e) Q_DECL_OVERRIDE;

	//
	void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

	void setRibbonPannelLayoutMode(SARibbonPannel::PannelLayoutMode m);
	bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

	void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

	void markIsContextCategory(bool isContextCategory = true);

private:
	void setRibbonBar(SARibbonBar *bar);

	SARibbonCategoryPrivate *m_d;
};

class SA_RIBBON_EXPORT SARibbonCategoryScrollButton : public QToolButton
{
	Q_OBJECT
public:
	SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget *p = nullptr);
};

#endif // SARIBBONCATEGORY_H
