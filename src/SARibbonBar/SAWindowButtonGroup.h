#ifndef SAWINDOWBUTTONGROUP_H
#define SAWINDOWBUTTONGROUP_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QPushButton>
class SAWindowButtonGroupPrivate;


class SA_RIBBON_EXPORT SAWindowButtonGroup : public QWidget
{
	Q_OBJECT
public:
	SAWindowButtonGroup(QWidget *parent);
	SAWindowButtonGroup(QWidget *parent, Qt::WindowFlags flags);
	~SAWindowButtonGroup();
	void setupMinimizeButton(bool on);
	void setupMaximizeButton(bool on);
	void setupCloseButton(bool on);
	void updateWindowFlag();
	void updateWindowFlag(Qt::WindowFlags flags);

	void setButtonWidthStretch(int close = 4, int max = 3, int min = 3);

	void setIconScale(qreal iconscale = 0.5);

	void setWindowStates(Qt::WindowStates s);

	Qt::WindowFlags windowButtonFlags() const;

protected:
	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
	virtual bool eventFilter(QObject *watched, QEvent *e) Q_DECL_OVERRIDE;
	virtual void parentResize();
	virtual void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

protected slots:
	Q_SLOT void closeWindow();
	Q_SLOT void minimizeWindow();
	Q_SLOT void maximizeWindow();

private:
	friend class SAWindowButtonGroupPrivate;
	SAWindowButtonGroupPrivate *m_d;
};

class SAWindowToolButton : public QPushButton
{
	Q_OBJECT
public:
	SAWindowToolButton(QWidget *p = nullptr);
};
#endif // SAWINDOWBUTTONGROUP_H
