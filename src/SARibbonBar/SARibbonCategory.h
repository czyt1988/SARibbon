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
 * @brief Ribbon控件中的分类选项卡页面（Category）
 *
 * 每个Category代表Ribbon中的一个标签页，包含多个面板(Panel)
 *
 * The `SARibbonCategory` is a tab page in the Ribbon interface that contains multiple panels (@ref SARibbonPanel).
 * It acts as a container for organizing related actions and controls into logical groups.
 *
 * `SARibbonCategory`是Ribbon界面中的一个标签页，包含多个面板 (@ref SARibbonPanel)。
 * 它作为容器，用于将相关的操作和控件按逻辑分组组织起来。
 *
 * ---
 *
 * ### 添加面板 / Adding Panels
 *
 * You can add panels to a category using functions like @ref addPanel, @ref insertPanel, etc.
 * Panels can be added either by specifying a title (which creates a new panel) or by passing an existing panel object.
 *
 * 可以使用 @ref addPanel、@ref insertPanel 等函数向分类中添加面板。
 * 面板可以通过指定标题（创建新面板）或传递现有面板对象来添加。
 *
 * ---
 *
 * ### 上下文分类 / Context Categories
 *
 * A category can be marked as a context category using @ref markIsContextCategory.
 * Context categories are typically used for specific scenarios or modes, such as formatting options when text is selected.
 *
 * 可以使用 @ref markIsContextCategory 将分类标记为上下文分类。
 * 上下文分类通常用于特定场景或模式，例如选择文本时的格式化选项。
 *
 * ---
 *
 * ### 自定义 / Customization
 *
 * Categories support customization features. You can control whether a category can be customized using @ref setCanCustomize.
 *
 * 分类支持自定义功能。您可以使用 @ref setCanCustomize 控制分类是否可以自定义。
 *
 * ---
 *
 * ### 滚动 / Scrolling
 *
 * When the content of a category exceeds its width, it can be scrolled horizontally using the mouse wheel or scroll buttons.
 * The scrolling behavior can be configured with functions like @ref setUseAnimatingScroll, @ref setWheelScrollStep, and @ref setAnimationDuration.
 *
 * 当分类的内容超过其宽度时，可以通过鼠标滚轮或滚动按钮进行水平滚动。
 * 滚动行为可以通过 @ref setUseAnimatingScroll、@ref setWheelScrollStep 和 @ref setAnimationDuration 等函数进行配置。
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
    explicit SARibbonCategory(QWidget* p = nullptr);
    explicit SARibbonCategory(const QString& name, QWidget* p = nullptr);
    ~SARibbonCategory();

    // Get the category name/获取Category名称
    QString categoryName() const;

    // Set the category name/设置Category名称
    void setCategoryName(const QString& title);

    // Get the panel layout mode/获取面板布局模式
    SARibbonPanel::PanelLayoutMode panelLayoutMode() const;

    // Set the panel layout mode/设置面板布局模式
    void setPanelLayoutMode(SARibbonPanel::PanelLayoutMode m);

    //  Add a panel (panel)/添加面板(panel)
    SARibbonPanel* addPanel(const QString& title);

    // Add panel/添加panel
    void addPanel(SARibbonPanel* panel);

    // Qt Designer specific/Qt Designer专用
    Q_INVOKABLE void addPanel(QWidget* panel);

    // Create a new panel and insert it at the index position/新建一个panel，并插入到index位置
    SARibbonPanel* insertPanel(const QString& title, int index);

    // Find panel by name/通过名字查找panel
    SARibbonPanel* panelByName(const QString& title) const;

    // Find panel by ObjectName/通过ObjectName查找panel
    SARibbonPanel* panelByObjectName(const QString& objname) const;

    // Find panel by index, returns nullptr if the index is out of range/通过索引找到panel，如果超过索引范围，会返回nullptr
    SARibbonPanel* panelByIndex(int index) const;

    // Find the index corresponding to panel/查找panel对应的索引
    int panelIndex(SARibbonPanel* p) const;

    // Move a Panel from from index to to index/移动一个Panel从from index到to index
    void movePanel(int from, int to);

    // Detach panel from SARibbonCategory management/把panel脱离SARibbonCategory的管理
    bool takePanel(SARibbonPanel* panel);

    // Remove Panel, Category will directly recycle SARibbonPanel memory/移除Panel，Category会直接回收SARibbonPanel内存
    bool removePanel(SARibbonPanel* panel);

    // Remove panel/移除panel
    bool removePanel(int index);

    // Return all panels under Category/返回Category下的所有panel
    QList< SARibbonPanel* > panelList() const;

    // Returns true if it is a ContextCategory/如果是ContextCategory，此函数返回true
    bool isContextCategory() const;

    // Return the number of panels/返回panel的个数
    int panelCount() const;

    // Determine if customization is allowed/判断是否可以自定义
    bool isCanCustomize() const;
    // Set whether customization is allowed/设置是否可以自定义
    void setCanCustomize(bool b);

    // Height of panel title bar/panel标题栏的高度
    int panelTitleHeight() const;
    // Set the height of panel/设置panel的高度
    void setPanelTitleHeight(int h);

    // Whether the panel title bar is displayed/是否panel显示标题栏
    bool isEnableShowPanelTitle() const;
    // Set to display panel title/设置显示panel标题
    void setEnableShowPanelTitle(bool on);

    // Set the alignment of Category/设置Category的对齐方式
    void setCategoryAlignment(SARibbonAlignment al);
    // Category alignment/Category的对齐方式
    SARibbonAlignment categoryAlignment() const;

    // Set the spacing of panel/设置panel的spacing
    void setPanelSpacing(int n);
    // panel spacing/panel的spacing
    int panelSpacing() const;

    // 大按钮图标尺寸
    void setPanelLargeIconSize(const QSize& largeSize);
    QSize panelLargeIconSize() const;

    // 小按钮图标尺寸
    void setPanelSmallIconSize(const QSize& smallSize);
    QSize panelSmallIconSize() const;

    // Set the icon size of panel buttons/设置panel按钮的icon尺寸
    void setPanelToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);
    // Icon size of panel buttons/panel按钮的icon尺寸
    QPair< QSize, QSize > panelToolButtonIconSize() const;

    // Get the corresponding ribbonbar, returns null if not managed by ribbonbar/获取对应的ribbonbar，如果没有加入ribbonbar的管理，此值为null
    SARibbonBar* ribbonBar() const;

    // Refresh the category layout, suitable for calling after changing the ribbon mode/刷新category的布局，适用于改变ribbon的模式之后调用
    void updateItemGeometry();

    // Set whether to use animation when scrolling/设置滚动时是否使用动画
    void setUseAnimatingScroll(bool useAnimating);
    // Whether to use animation when scrolling/滚动时是否使用动画
    bool isUseAnimatingScroll() const;

    // Set wheel scroll step (px)/设置滚轮滚动步长（px）
    void setWheelScrollStep(int step);
    // Wheel scroll step/滚轮的滚动步长
    int wheelScrollStep() const;

    // Set animation duration(ms)/设置动画持续时间(ms)
    void setAnimationDuration(int duration);
    // Animation duration(ms)/动画持续时间(ms)
    int animationDuration() const;

    // Determine whether the text of panel is allowed to wrap/判断panel的文字是否允许换行
    bool isEnableWordWrap() const;

    // Button maximum aspect ratio, this coefficient determines the maximum width of the button/按钮最大宽高比，这个系数决定按钮的最大宽度
    qreal buttonMaximumAspectRatio() const;

    // This function will iterate through all panels under Category and execute the function pointer/此函数会遍历Category下的所有panel,执行函数指针
    bool iteratePanel(FpPanelIterate fp) const;

    QSize sizeHint() const Q_DECL_OVERRIDE;
Q_SIGNALS:
    /**
     * @brief Category title changed signal/category标题发生了改变信号
     * @param n New category name/新的分类名称
     */
    void categoryNameChanged(const QString& n);

    /**
     * @brief Refer to QToolBar::actionTriggered signal/参考QToolBar::actionTriggered的信号
     * @param action Triggered action/被触发的动作
     */
    void actionTriggered(QAction* action);

protected:
    virtual bool event(QEvent* e) override;
    // 处理滚轮事件
    void wheelEvent(QWheelEvent* event) override;
    //
    void changeEvent(QEvent* event) override;

    // Mark this as a context label/标记这个是上下文标签
    void markIsContextCategory(bool isContextCategory = true);

    // Get SARibbonCategoryLayoutlayout/获取SARibbonCategoryLayoutlayout
    SARibbonCategoryLayout* categoryLayout() const;

    // Set whether the text of panel buttons is allowed to wrap/设置panel的按钮文字允许换行
    void setEnableWordWrap(bool on);

    // Set button maximum aspect ratio, this coefficient determines the maximum width of the button/设置按钮最大宽高比，这个系数决定按钮的最大宽度
    void setButtonMaximumAspectRatio(qreal fac = 1.4);
};

/**
 * @brief SARibbonCategory无法完全显示时，显示的调整按钮
 *
 * 重新定义是为了防止被外部的样式影响,同时可以使用SARibbonCategoryScrollButton的样式定义
 */
class SA_RIBBON_EXPORT SARibbonCategoryScrollButton : public QToolButton
{
    Q_OBJECT
public:
    explicit SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget* p = nullptr);
    ~SARibbonCategoryScrollButton();
};

#endif  // SARIBBONCATEGORY_H
