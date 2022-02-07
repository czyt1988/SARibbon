#ifndef SARIBBONELEMENTCREATEDELEGATE_H
#define SARIBBONELEMENTCREATEDELEGATE_H
#include "SARibbonGlobal.h"
#include <QColor>
#include <QMargins>
#include <QSize>
class QWidget;
class SARibbonBar;
class SARibbonTabBar;
class SARibbonApplicationButton;
class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonPannel;
class SARibbonSeparatorWidget;
class SARibbonGallery;
class SARibbonGalleryGroup;
class SARibbonToolButton;
class SARibbonControlButton;
class SARibbonButtonGroupWidget;
class SARibbonStackedWidget;
class SARibbonQuickAccessBar;
class SARibbonPannelOptionButton;



class SA_RIBBON_EXPORT SARibbonStyleOption {
public:
	SARibbonStyleOption();
	int mainbarHeightOfficeStyleThreeRow;
	int mainbarHeightWPSStyleThreeRow;
	int mainbarHeightOfficeStyleTwoRow;
	int mainbarHeightWPSStyleTwoRow;
	int titleBarHight;
	int tabBarHight;
	QColor titleTextColor;
	QMargins widgetBord;
	QColor tabBarBaseLineColor;
	int applicationButtonWidth;
	int galleryFixedHeight;
	int galleryMinimumWidth;
	int galleryButtonMaximumWidth;
	int pannelTitleHeight;
	QSize toolButtonLargeIconSize;
	QSize toolButtonSmallIconSize;
};

class SA_RIBBON_EXPORT SARibbonElementCreateDelegate
{
public:
	SARibbonElementCreateDelegate();
	virtual ~SARibbonElementCreateDelegate();
	virtual SARibbonTabBar *createRibbonTabBar(QWidget *parent);
	virtual SARibbonApplicationButton *createRibbonApplicationButton(QWidget *parent);
	virtual SARibbonCategory *createRibbonCategory(QWidget *parent);
	virtual SARibbonContextCategory *createRibbonContextCategory(QWidget *parent);
	virtual SARibbonPannel *createRibbonPannel(QWidget *parent);
	virtual SARibbonSeparatorWidget *createRibbonSeparatorWidget(int value, QWidget *parent);
	virtual SARibbonSeparatorWidget *createRibbonSeparatorWidget(QWidget *parent);
	virtual SARibbonGallery *createRibbonGallery(QWidget *parent);
	virtual SARibbonGalleryGroup *createRibbonGalleryGroup(QWidget *parent);
	virtual SARibbonToolButton *createRibbonToolButton(QWidget *parent);
	virtual SARibbonStackedWidget *createRibbonStackedWidget(SARibbonBar *parent);

	virtual SARibbonControlButton *createHidePannelButton(SARibbonBar *parent);
	virtual SARibbonButtonGroupWidget *craeteButtonGroupWidget(QWidget *parent);
	virtual SARibbonQuickAccessBar *createQuickAccessBar(QWidget *parent);

	//
	virtual const SARibbonStyleOption& getRibbonStyleOption() const;
	virtual void setRibbonStyleOption(SARibbonStyleOption *opt);

	virtual SARibbonPannelOptionButton *createRibbonPannelOptionButton(SARibbonPannel *pannel);

private:
	SARibbonStyleOption *m_opt;
};

#endif // SARIBBONELEMENTCREATEDELEGATE_H
