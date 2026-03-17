#ifndef SARIBBONCATEGORYLAYOUT_H
#define SARIBBONCATEGORYLAYOUT_H
#include "SARibbonGlobal.h"
#include <QLayout>
#include <QList>
#include <QMap>
#include "SARibbonCategory.h"

class SARibbonPanel;
class SARibbonCategoryLayoutItem;
class SARibbonSeparatorWidget;

/**
 * \if ENGLISH
 * @brief Layout class for SARibbonCategory
 * @details This class handles the layout of panels in a SARibbonCategory, including scrolling and animation support
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonCategory 的布局类
 * @details 此类处理 SARibbonCategory 中面板的布局，包括滚动和动画支持
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayout : public QLayout
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonCategoryLayout)
public:
    Q_PROPERTY(int scrollPosition READ scrollPosition WRITE setScrollPosition)
public:
    /// Constructor for SARibbonCategoryLayout
    explicit SARibbonCategoryLayout(SARibbonCategory* parent);
    /// Destructor for SARibbonCategoryLayout
    ~SARibbonCategoryLayout();

    /// Get the parent SARibbonCategory
    SARibbonCategory* ribbonCategory() const;

    /// Add a layout item (overridden from QLayout)
    virtual void addItem(QLayoutItem* item) override;
    /// Get the layout item at the specified index (overridden from QLayout)
    virtual QLayoutItem* itemAt(int index) const override;
    /// Take the layout item at the specified index (overridden from QLayout)
    virtual QLayoutItem* takeAt(int index) override;
    /// Take the panel item at the specified index
    SARibbonCategoryLayoutItem* takePanelItem(int index);
    /// Take the panel item for the specified panel
    SARibbonCategoryLayoutItem* takePanelItem(SARibbonPanel* panel);
    /// Remove a panel and its separator
    bool takePanel(SARibbonPanel* panel);
    /// Get the number of layout items (overridden from QLayout)
    virtual int count() const override;
    /// Set the geometry of the layout (overridden from QLayout)
    void setGeometry(const QRect& rect) override;
    /// Get the size hint of the layout (overridden from QLayout)
    QSize sizeHint() const override;
    /// Get the minimum size of the layout (overridden from QLayout)
    QSize minimumSize() const override;
    /// Get the expanding directions of the layout (overridden from QLayout)
    Qt::Orientations expandingDirections() const override;
    /// Invalidate the layout (overridden from QLayout)
    void invalidate() override;
    /// Add a panel to the layout
    void addPanel(SARibbonPanel* panel);
    /// Insert a panel at the specified index
    void insertPanel(int index, SARibbonPanel* panel);
    /// Get the content size of the category
    QSize categoryContentSize() const;
    /// Update geometry of the layout
    void updateGeometryArr();

    /// Execute layout adjustment
    void doLayout();

    /// Get all panels in the layout
    QList< SARibbonPanel* > panels() const;

    /// Find a panel by its object name
    SARibbonPanel* panelByObjectName(const QString& objname) const;
    /// Find a panel by its name
    SARibbonPanel* panelByName(const QString& panelname) const;
    /// Find a panel by its index
    SARibbonPanel* panelByIndex(int i) const;
    /// Move a panel from one position to another
    void movePanel(int from, int to);
    /// Get the number of panels
    int panelCount() const;
    /// Find the index of a panel
    int panelIndex(SARibbonPanel* p) const;
    /// Get all panels in the layout
    QList< SARibbonPanel* > panelList() const;
    /// Execute scrolling
    void scroll(int px);
    /// Scroll to a specified position
    void scrollTo(int targetX);
    /// Animate scrolling by a specified distance
    void scrollByAnimate(int px);
    /// Animate scrolling to a specified position
    void scrollToByAnimate(int targetX);
    /// Get the current scroll position
    int scrollPosition() const;
    /// Set the scroll position
    void setScrollPosition(int pos);
    /// Check if scrolling animation is in progress
    bool isAnimatingScroll() const;
    /// Check if the layout has been scrolled
    bool isScrolled() const;
    /// Get the total width of the content
    int categoryTotalWidth() const;
    /// Set the alignment of the category
    void setCategoryAlignment(SARibbonAlignment al);
    /// Get the alignment of the category
    SARibbonAlignment categoryAlignment() const;
    /// Set the duration of the animation
    void setAnimationDuration(int duration);
    /// Get the duration of the animation
    int animationDuration() const;
private Q_SLOTS:
    void onLeftScrollButtonClicked();
    void onRightScrollButtonClicked();

private:
    void setupAnimateScroll();
};

/**
 * \if ENGLISH
 * @brief Layout item for SARibbonCategoryLayout
 * @details This class is used to identify items in SARibbonCategoryLayout, including panels and their associated separators
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonCategoryLayoutItem，用于标识SARibbonCategoryLayout的item
 * @details 此类用于标识 SARibbonCategoryLayout 中的项目，包括面板及其关联的分隔符
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayoutItem : public QWidgetItem
{
public:
    /// Constructor for SARibbonCategoryLayoutItem
    explicit SARibbonCategoryLayoutItem(SARibbonPanel* w);
    /// Destructor for SARibbonCategoryLayoutItem
    ~SARibbonCategoryLayoutItem();
    SARibbonSeparatorWidget* separatorWidget;
    /// Convert the internal widget to a SARibbonPanel
    SARibbonPanel* toPanelWidget();
    QRect mWillSetGeometry;           ///< panel将要设置的Geometry
    QRect mWillSetSeparatorGeometry;  ///< panel将要设置的Separator的Geometry
};
#endif  // SARIBBONCATEGORYLAYOUT_H
