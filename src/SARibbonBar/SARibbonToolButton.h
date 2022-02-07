#ifndef SARIBBONTOOLBUTTON_H
#define SARIBBONTOOLBUTTON_H
#include "SARibbonGlobal.h"
#include <QToolButton>



class SA_RIBBON_EXPORT SARibbonToolButton : public QToolButton
{
	Q_OBJECT
		Q_ENUMS(RibbonButtonType)
public:


	enum RibbonButtonType {
		LargeButton
		, SmallButton
	};


	enum LargeButtonType {
		Normal
		, Lite
	};

	SARibbonToolButton(QWidget *parent = Q_NULLPTR);
	SARibbonToolButton(QAction *defaultAction, QWidget *parent = Q_NULLPTR);
	RibbonButtonType buttonType() const;
	void setButtonType(const RibbonButtonType& buttonType);
	virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

	void setLargeButtonType(LargeButtonType type);
	LargeButtonType largeButtonType() const;

	virtual int liteLargeButtonIconHeight(int buttonHeight) const;

protected:
	void calcIconRect(const QStyleOptionToolButton& opt);
	QRect calcTextRect(const QStyleOptionToolButton& opt) const;
	QRect calcTextRect(const QRect& buttonRect, bool hasMenu = false) const;
	QRect calcIndicatorArrowDownRect(const QStyleOptionToolButton& opt);
	QPixmap createIconPixmap(const QStyleOptionToolButton& opt);
	bool event(QEvent *e) Q_DECL_OVERRIDE;
	virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	virtual void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	virtual void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	virtual void focusOutEvent(QFocusEvent *e) Q_DECL_OVERRIDE;
	virtual void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
	virtual void paintLargeButton(QPaintEvent *e);
	virtual void paintSmallButton(QPaintEvent *e);
	virtual bool hitButton(const QPoint& pos) const Q_DECL_OVERRIDE;
	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
	virtual void drawIconAndLabel(QPainter& p, QStyleOptionToolButton& opt);

private:
	static void drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton,
		const QRect& rect, QPainter *painter, const QWidget *widget = 0);

private:
	RibbonButtonType m_buttonType;
	LargeButtonType m_largeButtonType;
	bool m_mouseOnSubControl;
	bool m_menuButtonPressed;
	QRect m_iconRect;
	bool m_isWordWrap;
};

#endif // SARIBBONTOOLBUTTON_H
