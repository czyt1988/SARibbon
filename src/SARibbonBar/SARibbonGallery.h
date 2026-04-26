#ifndef SARIBBONGALLERY_H
#define SARIBBONGALLERY_H
#include "SARibbonGlobal.h"
#include <QFrame>
#include <QToolButton>
#include <QScrollArea>
#include "SARibbonGalleryGroup.h"
class QLabel;
class QVBoxLayout;
class QSizeGrip;
class QShowEvent;
class QHideEvent;
class SARibbonGalleryViewport;

/**
 * \if ENGLISH
 * @brief Button for SARibbonGallery control
 * \endif
 *
 * \if CHINESE
 * @brief 针对SARibbonGallery控件的按钮
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGalleryButton : public QToolButton
{
    Q_OBJECT
public:
    explicit SARibbonGalleryButton(QWidget* parent = nullptr);
    ~SARibbonGalleryButton();
};

/**
 * \if ENGLISH
 * @brief Gallery control widget
 *
 * The Gallery control consists of a currently active @sa SARibbonGalleryGroup and a popup @sa SARibbonGalleryViewport.
 *
 * @sa SARibbonGalleryGroup inherits from @sa QListView and displays actions through icons.
 *     Related properties can be set according to QListView.
 *
 * @sa SARibbonGalleryViewport is a window with an internal vertical layout that displays
 *     all managed SARibbonGalleryGroups when popped up.
 *
 * Example usage:
 * @code
 * SARibbonGallery* gallery = panel1->addGallery();
 * QList< QAction* > galleryActions;
 * ...create many actions ...
 * SARibbonGalleryGroup* group1 = gallery->addCategoryActions(tr("Files"), galleryActions);
 * galleryActions.clear();
 * ...create many actions ...
 * gallery->addCategoryActions(tr("Apps"), galleryActions);
 * gallery->setCurrentViewGroup(group1);
 * @endcode
 * \endif
 *
 * \if CHINESE
 * @brief Gallery控件
 *
 * Gallery控件是由一个当前激活的@sa SARibbonGalleryGroup 和弹出的 @sa SARibbonGalleryViewport 组成
 *
 * @sa SARibbonGalleryGroup 是继承@sa QListView actions通过icon展示出来，相关的属性可以按照QListView设置
 *
 * @sa SARibbonGalleryViewport 是一个内部有垂直布局的窗体，在弹出激活时，把管理的SARibbonGalleryGroup都展示出来
 *
 * 示例如下：
 * @code
 * SARibbonGallery* gallery = panel1->addGallery();
 * QList< QAction* > galleryActions;
 * ...create many actions ...
 * SARibbonGalleryGroup* group1 = gallery->addCategoryActions(tr("Files"), galleryActions);
 * galleryActions.clear();
 * ...create many actions ...
 * gallery->addCategoryActions(tr("Apps"), galleryActions);
 * gallery->setCurrentViewGroup(group1);
 * @endcode
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGallery : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonGallery)
public:
    /// Constructor for SARibbonGallery
    explicit SARibbonGallery(QWidget* parent = nullptr);
    /// Destructor for SARibbonGallery
    virtual ~SARibbonGallery();
    /// Get a blank SARibbonGalleryGroup
    SARibbonGalleryGroup* addGalleryGroup();
    /// Add an existing SARibbonGalleryGroup to the gallery
    void addGalleryGroup(SARibbonGalleryGroup* group);
    /// Quickly add a set of actions as a gallery group
    SARibbonGalleryGroup* addCategoryActions(const QString& title, QList< QAction* > actions);
    /// Set the currently displayed gallery group
    void setCurrentViewGroup(SARibbonGalleryGroup* group);
    /// Get the currently displayed gallery group
    SARibbonGalleryGroup* currentViewGroup() const;
    /// Get the popup viewport widget
    SARibbonGalleryViewport* getPopupViewPort() const;

public:
    /// Set the maximum width for gallery control buttons (default 15)
    static void setGalleryButtonMaximumWidth(int w);
    /// Set single-row display mode for gallery
    void setSingleRowMode(bool on);
    /// Check if gallery is in single-row mode
    bool isSingleRowMode() const;
Q_SIGNALS:
    /**
     * \if ENGLISH
     * @brief Forwards SARibbonGalleryGroup::triggered signal
     * All actions added to SARibbonGallery are managed by a QActionGroup
     * @param action Triggered action
     * \endif
     *
     * \if CHINESE
     * @brief 转发管理的SARibbonGalleryGroup::triggered信号
     * 所有加入SARibbonGallery的action都会被一个QActionGroup管理
     * @param action 被触发的动作
     * \endif
     */
    void triggered(QAction* action);
    /**
     * \if ENGLISH
     * @brief Forwards SARibbonGalleryGroup::hovered signal
     * @note This requires SARibbonGalleryGroup::setMouseTracking(true)
     * @param action Hovered action
     * \endif
     *
     * \if CHINESE
     * @brief 转发管理的SARibbonGalleryGroup::hovered信号
     * @note 此属性需要确保SARibbonGalleryGroup::setMouseTracking(true)
     * @param action 悬停的动作
     * \endif
     */
    void hovered(QAction* action);

public Q_SLOTS:
    /// Scroll up one page in the current gallery group
    virtual void pageUp();
    /// Scroll down one page in the current gallery group
    virtual void pageDown();
    /// Show the popup viewport with all gallery groups
    virtual void showMoreDetail();
protected Q_SLOTS:
    /// Slot called when an item in a gallery group is clicked
    void onItemClicked(const QModelIndex& index);
    /// Slot called when an action is triggered from the gallery
    virtual void onTriggered(QAction* action);

private:
    SARibbonGalleryViewport* ensureGetPopupViewPort();

protected:
    /// Handle resize events for the gallery
    void resizeEvent(QResizeEvent* event) override;
    /// Handle change events for the gallery
    void changeEvent(QEvent* event) override;
};

/**
 * \if ENGLISH
 * @brief Viewport class for SARibbonGallery
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGallery的Viewport类
 * \endif
 */
class SARibbonGalleryViewport : public QScrollArea
{
    Q_OBJECT
public:
    explicit SARibbonGalleryViewport(QWidget* parent);
    // 添加窗口不带标题
    void addWidget(QWidget* w);
    // 添加窗口，带标题
    void addWidget(QWidget* w, const QString& title);
    // 通过SARibbonGalleryGroup获取对应的标题，用户可以通过此函数设置QLabel的属性
    QLabel* titleLabel(QWidget* w);
    // 移除窗口
    void removeWidget(QWidget* w);
    // 获取此窗口下所有的SARibbonGalleryGroup
    QList< SARibbonGalleryGroup* > galleryGroupList() const;
    // 获取gallery的高度
    int galleryHeight() const;
    // 根据宽度计算高度推荐值
    int heightHintForWidth(int w) const;
public Q_SLOTS:
    void onTitleChanged(QWidget* w, const QString& title);

protected:  // 新增
    bool eventFilter(QObject* o, QEvent* e) override;
    void showEvent(QShowEvent* e) override;
    void hideEvent(QHideEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    QWidget* m_contentWidget { nullptr };            ///< 滚动区域的内容窗口
    QVBoxLayout* m_layout;                           ///< 内容窗口的布局
    QMap< QWidget*, QLabel* > m_widgetToTitleLabel;  ///< 窗口到标题标签的映射
    QSizeGrip* m_sizeGrip;                           ///< 尺寸调整手柄
};

#endif  // SARIBBONGALLERY_H
