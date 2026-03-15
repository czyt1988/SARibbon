#ifndef SARIBBONCATEGORY_H
#define SARIBBONCATEGORY_H
#include "SARibbonGlobal.h"
#include <QFrame>
#include "SARibbonPanel.h"
#include <QScopedPointer>
#include <QPushButton>
#include <QWheelEvent>

class QAction;
class QHBoxLayout;
class QWheelEvent;
class SARibbonBar;
class SARibbonCategoryLayout;

/**
 * \if ENGLISH
 * @brief Ribbon category page containing multiple panels
 *
 * Each Category represents a tab page in the Ribbon, containing multiple panels (SARibbonPanel).
 * It acts as a container for organizing related actions and controls into logical groups.
 * \endif
 *
 * \if CHINESE
 * @brief 包含多个面板的Ribbon类别页面
 *
 * 每个Category代表Ribbon中的一个标签页，包含多个面板（SARibbonPanel）。
 * 它作为一个容器，用于将相关的操作和控件组织成逻辑组。
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCategory : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonCategory)
    friend class SARibbonBar;
    friend class SARibbonContextCategory;
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(QString categoryName READ categoryName WRITE setCategoryName)
public:
    using FpPanelIterate = std::function< bool(SARibbonPanel*) >;

public:
    /// Constructor
    explicit SARibbonCategory(QWidget* p = nullptr);
    /// Constructor with name
    explicit SARibbonCategory(const QString& name, QWidget* p = nullptr);
    /// Destructor
    ~SARibbonCategory();

    /// Get the category name
    QString categoryName() const;

    /// Set the category name
    void setCategoryName(const QString& title);

    /// Get the panel layout mode
    SARibbonPanel::PanelLayoutMode panelLayoutMode() const;

    /// Set the panel layout mode
    void setPanelLayoutMode(SARibbonPanel::PanelLayoutMode m);

    /// Add a panel with title
    SARibbonPanel* addPanel(const QString& title);

    /// Add an existing panel
    void addPanel(SARibbonPanel* panel);

    /// Add panel for Qt Designer
    Q_INVOKABLE void addPanel(QWidget* panel);

    /// Create and insert a new panel at index position
    SARibbonPanel* insertPanel(const QString& title, int index);

    /// Find panel by name
    SARibbonPanel* panelByName(const QString& title) const;

    /// Find panel by ObjectName
    SARibbonPanel* panelByObjectName(const QString& objname) const;

    /// Find panel by index, returns nullptr if out of range
    SARibbonPanel* panelByIndex(int index) const;

    /// Get the index of a panel
    int panelIndex(SARibbonPanel* p) const;

    /// Move a panel from one index to another
    void movePanel(int from, int to);

    /// Detach panel from SARibbonCategory management
    bool takePanel(SARibbonPanel* panel);

    /// Remove panel and delete it
    bool removePanel(SARibbonPanel* panel);

    /// Remove panel by index
    bool removePanel(int index);

    /// Get all panels
    QList< SARibbonPanel* > panelList() const;

    /// Check if this is a context category
    bool isContextCategory() const;

    /// Get the number of panels
    int panelCount() const;

    /// Check if customization is allowed
    bool isCanCustomize() const;
    /// Set whether customization is allowed
    void setCanCustomize(bool b);

    /// Get panel title bar height
    int panelTitleHeight() const;
    /// Set panel title bar height
    void setPanelTitleHeight(int h);

    /// Check if panel title bar is displayed
    bool isEnableShowPanelTitle() const;
    /// Set whether to display panel title
    void setEnableShowPanelTitle(bool on);

    /// Set category alignment
    void setCategoryAlignment(SARibbonAlignment al);
    /// Get category alignment
    SARibbonAlignment categoryAlignment() const;

    /// Set panel spacing
    void setPanelSpacing(int n);
    /// Get panel spacing
    int panelSpacing() const;

    /// Set panel large icon size
    void setPanelLargeIconSize(const QSize& largeSize);
    /// Get panel large icon size
    QSize panelLargeIconSize() const;

    /// Set panel small icon size
    void setPanelSmallIconSize(const QSize& smallSize);
    /// Get panel small icon size
    QSize panelSmallIconSize() const;

    /// Set panel tool button icon sizes
    void setPanelToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);
    /// Get panel tool button icon sizes
    QPair< QSize, QSize > panelToolButtonIconSize() const;

    /// Get the parent ribbonbar, returns null if not managed
    SARibbonBar* ribbonBar() const;

    /// Refresh category layout, call after changing ribbon mode
    void updateItemGeometry();

    /// Set whether to use animation when scrolling
    void setUseAnimatingScroll(bool useAnimating);
    /// Check if animation is used when scrolling
    bool isUseAnimatingScroll() const;

    /// Set wheel scroll step in pixels
    void setWheelScrollStep(int step);
    /// Get wheel scroll step
    int wheelScrollStep() const;

    /// Set animation duration in milliseconds
    void setAnimationDuration(int duration);
    /// Get animation duration in milliseconds
    int animationDuration() const;

    /// Check if panel text word wrap is enabled
    bool isEnableWordWrap() const;

    /// Get button maximum aspect ratio
    qreal buttonMaximumAspectRatio() const;

    /// Iterate through all panels
    bool iteratePanel(FpPanelIterate fp) const;

    QSize sizeHint() const Q_DECL_OVERRIDE;
Q_SIGNALS:
    /// Emitted when category name changes
    void categoryNameChanged(const QString& n);

    /// Emitted when an action is triggered
    void actionTriggered(QAction* action);

protected:
    virtual bool event(QEvent* e) override;
    /// Handle wheel event
    void wheelEvent(QWheelEvent* event) override;
    /// Handle change event
    void changeEvent(QEvent* event) override;

    /// Mark this as a context category
    void markIsContextCategory(bool isContextCategory = true);

    /// Get the category layout
    SARibbonCategoryLayout* categoryLayout() const;

    /// Set whether panel button text word wrap is enabled
    void setEnableWordWrap(bool on);

    /// Set button maximum aspect ratio
    void setButtonMaximumAspectRatio(qreal fac = 1.4);
};

/// Scroll button for SARibbonCategory when content exceeds width
class SA_RIBBON_EXPORT SARibbonCategoryScrollButton : public QToolButton
{
    Q_OBJECT
public:
    explicit SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget* p = nullptr);
    ~SARibbonCategoryScrollButton();
};

#endif  // SARIBBONCATEGORY_H
