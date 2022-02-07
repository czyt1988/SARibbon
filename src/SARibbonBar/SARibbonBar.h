#ifndef SARIBBONBAR_H
#define SARIBBONBAR_H
#include "SARibbonGlobal.h"
#include <QMenuBar>
#include <QVariant>
#include "SARibbonCategory.h"
#include "SARibbonContextCategory.h"
#include <QScopedPointer>

class QAbstractButton;
class SARibbonElementCreateDelegate;
class SARibbonBarPrivate;
class SARibbonTabBar;
class SARibbonButtonGroupWidget;
class SARibbonQuickAccessBar;


/**
 * void MainWindow::MainWindow()
 * {
 *    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
 *    QToolBar *fileToolBar = addToolBar(tr("File"));
 *    QAction *newAct = new QAction(newIcon, tr("&New"), this);
 *    fileMenu->addAction(newAct);
 *    fileToolBar->addAction(newAct);
 *
 *    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
 *    fileMenu->addAction(openAct);
 *    fileToolBar->addAction(openAct);
 * }
 * @endcode
 *
 * void setupRibbonUi()
 * {
 *     ......
 *     SARibbonBar* ribbon = ribbonwindow->ribbonBar();
 *     ribbon->setRibbonStyle(SARibbonBar::WpsLiteStyle);
 *     categoryMain = ribbon->addCategoryPage(QStringLiteral("Main"));
 *     FilePannel = categoryMain->addPannel(QStringLiteral("FilePannel"));
 *     FilePannel->addLargeAction(actionNew);
 *     FilePannel->addLargeAction(actionOpen);
 *     FilePannel->addLargeAction(actionSave);
 *     FilePannel->addSmallAction(actionImportMesh);
 *     FilePannel->addSmallAction(actionImportGeometry);
 * }
 * @endcode
 */
class SA_RIBBON_EXPORT SARibbonBar : public QMenuBar
{
	friend class SARibbonBarPrivate;
	Q_OBJECT
		Q_PROPERTY(RibbonStyle ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)
		Q_PROPERTY(bool minimumMode READ isMinimumMode WRITE setMinimumMode)
		Q_PROPERTY(bool minimumModeButton READ haveShowMinimumModeButton WRITE showMinimumModeButton)

public:
	enum RibbonStyle {
		OfficeStyle = 0x0000
		, WpsLiteStyle = 0x0001
		, OfficeStyleTwoRow = 0x0100
		, WpsLiteStyleTwoRow = 0x0101
	};
	Q_ENUMS(RibbonStyle)

		enum RibbonState {
		MinimumRibbonMode
		, NormalRibbonMode
	};
	Q_ENUMS(RibbonState)
public:

public:
	static bool isTwoRowStyle(RibbonStyle s);

	static bool isOfficeStyle(RibbonStyle s);

	SARibbonBar(QWidget *parent = nullptr);
	~SARibbonBar() Q_DECL_OVERRIDE;
	QAbstractButton *applicationButton();

	void setApplicationButton(QAbstractButton *btn);

	SARibbonTabBar *ribbonTabBar();


	SARibbonCategory *addCategoryPage(const QString& title);
	void addCategoryPage(SARibbonCategory *category);

	Q_INVOKABLE void addCategoryPage(QWidget *category);

	SARibbonCategory *insertCategoryPage(const QString& title, int index);
	void insertCategoryPage(SARibbonCategory *category, int index);

	SARibbonCategory *categoryByName(const QString& title) const;

	SARibbonCategory *categoryByObjectName(const QString& objname) const;

	SARibbonCategory *categoryByIndex(int index) const;

	void hideCategory(SARibbonCategory *category);

	void showCategory(SARibbonCategory *category);

	bool isCategoryVisible(SARibbonCategory *category) const;

	int categoryIndex(SARibbonCategory *c) const;

	void moveCategory(int from, int to);

	QList<SARibbonCategory *> categoryPages(bool getAll = true) const;

	void removeCategory(SARibbonCategory *category);

	SARibbonContextCategory *addContextCategory(const QString& title, const QColor& color = QColor(), const QVariant& id = QVariant());
	void addContextCategory(SARibbonContextCategory *context);

	void showContextCategory(SARibbonContextCategory *context);

	void hideContextCategory(SARibbonContextCategory *context);

	bool isContextCategoryVisible(SARibbonContextCategory *context);

	void setContextCategoryVisible(SARibbonContextCategory *context, bool visible);

	QList<SARibbonContextCategory *> contextCategoryList() const;

	void destroyContextCategory(SARibbonContextCategory *context);

	void setMinimumMode(bool isHide);

	bool isMinimumMode() const;

	void showMinimumModeButton(bool isShow = true);

	bool haveShowMinimumModeButton() const;

	int tabBarHeight() const;

	int titleBarHeight() const;

	void activeRightButtonGroup();

	SARibbonButtonGroupWidget *rightButtonGroup();

	SARibbonQuickAccessBar *quickAccessBar();

	void setRibbonStyle(RibbonStyle v);

	RibbonStyle currentRibbonStyle() const;

	RibbonState currentRibbonState() const;

	void setCurrentIndex(int index);

	int currentIndex();

	void raiseCategory(SARibbonCategory *category);

	bool isTwoRowStyle() const;

	bool isOfficeStyle() const;

	void setWindowButtonSize(const QSize& size);


signals:

	void applicationButtonClicked();

	void currentRibbonTabChanged(int index);

protected:
	bool eventFilter(QObject *obj, QEvent *e) override;

	int calcMinTabBarWidth() const;

	virtual int mainBarHeight() const;

protected slots:
	void onWindowTitleChanged(const QString& title);
	void onCategoryWindowTitleChanged(const QString& title);
	void onStackWidgetHided();
	virtual void onCurrentRibbonTabChanged(int index);
	virtual void onCurrentRibbonTabClicked(int index);
	virtual void onCurrentRibbonTabDoubleClicked(int index);
	void onContextsCategoryPageAdded(SARibbonCategory *category);
	void onTabMoved(int from, int to);

private:
	int tabIndex(SARibbonCategory *obj);
	void updateRibbonElementGeometry();
	void resizeInOfficeStyle();
	void resizeInWpsLiteStyle();
	void paintInNormalStyle();
	void paintInWpsLiteStyle();
	void resizeStackedContainerWidget();

	void updateContextCategoryManagerData();

protected:
	void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
	void moveEvent(QMoveEvent *event) Q_DECL_OVERRIDE;
	virtual void paintBackground(QPainter& painter);
	virtual void paintWindowTitle(QPainter& painter, const QString& title, const QRect& titleRegion);
	virtual void paintContextCategoryTab(QPainter& painter, const QString& title, QRect contextRect, const QColor& color);

private:
	SARibbonBarPrivate *m_d;
};



#endif // SARIBBONBAR_H
