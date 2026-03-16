#ifndef SARIBBONBARLAYOUT_H
#define SARIBBONBARLAYOUT_H

#include "SARibbonGlobal.h"
#include "SARibbonBar.h"
#include <QLayout>
#include <QPointer>

class SARibbonTabBar;
class SARibbonStackedWidget;
class SARibbonQuickAccessBar;
class SARibbonButtonGroupWidget;
class SARibbonContextCategory;

/**
 * \if ENGLISH
 * @brief Layout class for SARibbonBar
 * @details This class handles the layout of all components in the ribbon bar, including title bar, tab bar, quick access bar, and categories.
 * \endif
 * 
 * \if CHINESE
 * @brief SARibbonBar 的布局类
 * @details 此类处理功能区栏中所有组件的布局，包括标题栏、标签栏、快速访问栏和类别。
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonBarLayout : public QLayout
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonBarLayout)
public:
	// Constructor for SARibbonBarLayout
	explicit SARibbonBarLayout(SARibbonBar* parent);
	// Destructor for SARibbonBarLayout
	virtual ~SARibbonBarLayout();

	// QLayout interface implementations
	// Add a layout item
	void addItem(QLayoutItem* item) override;
	// Get the layout item at the specified index
	QLayoutItem* itemAt(int index) const override;
	// Take and remove the layout item at the specified index
	QLayoutItem* takeAt(int index) override;
	// Get the number of layout items
	int count() const override;
	// Get the recommended size for the layout
	QSize sizeHint() const override;
	// Get the minimum size for the layout
	QSize minimumSize() const override;
	// Set the geometry of the layout
	void setGeometry(const QRect& rect) override;

	// Perform the layout
	virtual void doLayout();

	// Style related methods
	// Check if the current style is loose style
	bool isLooseStyle() const;
	// Check if the current style is compact style
	bool isCompactStyle() const;

	// Title rectangle
	// Get the title rectangle
	const QRect& titleRect() const;

	// Calculate minimum TabBar width
	int calcMinTabBarWidth() const;

	// Reset size, called when ribbon mode changes
	void resetSize();

	// Update title rectangle
	void layoutTitleRect();

	// Adjust StackedContainerWidget position
	void layoutStackedContainerWidget();

	// Re-layout category, called when category alignment changes
	void layoutCategory();

	// Set system button size
	void setSystemButtonSize(const QSize& size);

	// Set whether tab is on title bar
	void setTabOnTitle(bool on);
	// Check if tab is on title bar
	bool isTabOnTitle() const;

	// Get main bar height in minimum mode
	int minimumModeMainBarHeight() const;

	// Get main bar height in normal mode
	int normalModeMainBarHeight() const;

	// Ribbon tab bar height
	// Get tab bar height
	int tabBarHeight() const;
	// Set tab bar height
	void setTabBarHeight(int h);

	// Title bar height
	// Get title bar height
	int titleBarHeight() const;
	// Set title bar height
	void setTitleBarHeight(int h);

	// Category height
	// Get category height
	int categoryHeight() const;
	// Set category height
	void setCategoryHeight(int h);

	// Panel title height
	// Get panel title height
	int panelTitleHeight() const;
	// Set panel title height
	void setPanelTitleHeight(int h);

    // Window icon
    // Set window icon
    void setWindowIcon(const QIcon& icon);
    // Get window icon
    QIcon windowIcon() const;

    // Application button vertical expansion
    // Set whether application button is vertically expanded
    void setApplicationButtonVerticalExpansion(bool on = true);
    // Check if application button is vertically expanded
    bool isApplicationButtonVerticalExpansion() const;

	// Get elements
	// Get ribbon bar
	SARibbonBar* ribbonBar() const;
	// Get ribbon tab bar
	SARibbonTabBar* ribbonTabBar() const;
	// Get stacked container widget
	SARibbonStackedWidget* stackedContainerWidget() const;
	// Get quick access bar
	SARibbonQuickAccessBar* quickAccessBar() const;
	// Get right button group
	SARibbonButtonGroupWidget* rightButtonGroup() const;
	// Get application button
	QAbstractButton* applicationButton() const;
	// Get title icon widget
    SARibbonTitleIconWidget* titleIconWidget() const;

protected:
	// Resize in loose style
	void resizeInLooseStyle();
	// Resize in compact style
	void resizeInCompactStyle();

private:
	// Initialize the layout
	void init();
};

#endif  // SARIBBONBARLAYOUT_H
