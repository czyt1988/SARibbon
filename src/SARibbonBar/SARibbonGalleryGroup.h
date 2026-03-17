#ifndef SARIBBONGALLERYGROUP_H
#define SARIBBONGALLERYGROUP_H
#include "SARibbonGlobal.h"
#include <QList>
#include <QListView>
#include <QStyledItemDelegate>
#include "SARibbonGalleryItem.h"

/**
 * \if ENGLISH
 * @brief Delegate for SARibbonGalleryGroup display
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryGroup对应的显示代理
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGalleryGroupItemDelegate : public QStyledItemDelegate
{
public:
    /// Constructor for SARibbonGalleryGroupItemDelegate
    explicit SARibbonGalleryGroupItemDelegate(SARibbonGalleryGroup* group, QObject* parent = nullptr);
    /// Destructor for SARibbonGalleryGroupItemDelegate
    ~SARibbonGalleryGroupItemDelegate();
    /// Paint the item
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /// Get size hint
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    /// Paint only icon
    virtual void paintIconOnly(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    /// Paint icon with text
    virtual void paintIconWithText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    /// Paint icon with word wrap text
    virtual void
    paintIconWithTextWordWrap(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    SARibbonGalleryGroup* mGroup;
};

/**
 * \if ENGLISH
 * @brief Model for SARibbonGalleryGroup
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryGroup对应的model
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGalleryGroupModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /// Constructor for SARibbonGalleryGroupModel
    SARibbonGalleryGroupModel(QObject* parent = Q_NULLPTR);
    /// Destructor for SARibbonGalleryGroupModel
    ~SARibbonGalleryGroupModel();
    /// Get row count
    virtual int rowCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;
    /// Get item flags
    virtual Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    /// Get data at index
    virtual QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    /// Create model index
    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const Q_DECL_OVERRIDE;
    /// Set data at index
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;
    /// Clear all items
    void clear();
    /// Get item at row
    SARibbonGalleryItem* at(int row) const;
    /// Insert item at row
    void insert(int row, SARibbonGalleryItem* item);
    /// Take item at row
    SARibbonGalleryItem* take(int row);
    /// Append item
    void append(SARibbonGalleryItem* item);
    /// Remove item by action object name
    bool remove(const QString& act_object_name);
    /// Get item count
    int itemSize() const;

private:
    QList< SARibbonGalleryItem* > mItems;
};

/**
 * \if ENGLISH
 * @brief Gallery group
 * @details The group is responsible for displaying and managing Gallery Items
 * \endif
 *
 * \if CHINESE
 * @brief Gallery的组
 * @details 组负责显示管理Gallery Item
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGalleryGroup : public QListView
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonGalleryGroup)
public:
    /**
     * @brief GalleryGroup display style
     */
    enum GalleryGroupStyle
    {
        IconWithText,          ///< Icon with text
        IconWithWordWrapText,  ///< Icon with text that wraps, only works for DisplayOneRow
        IconOnly               ///< Icon only
    };

    /**
     * @brief Define the number of icon rows displayed under a panel
     */
    enum DisplayRow
    {
        DisplayOneRow   = 1,  ///< Display 1 row (default)
        DisplayTwoRow   = 2,  ///< Display 2 rows
        DisplayThreeRow = 3   ///< Display 3 rows
    };

    /// Constructor
    SARibbonGalleryGroup(QWidget* w = 0);

    /// Destructor
    virtual ~SARibbonGalleryGroup();
    /// Recalculate grid size
    void recalcGridSize();
    /// Recalculate grid size
    void recalcGridSize(int galleryHeight);
    /// Set display style
    void setGalleryGroupStyle(GalleryGroupStyle style);
    /// Get gallery group style
    GalleryGroupStyle galleryGroupStyle() const;
    /// Add item
    void addItem(const QString& text, const QIcon& icon);
    /// Add item
    void addItem(SARibbonGalleryItem* item);
    /// Add action as item
    void addActionItem(QAction* act);
    /// Add action item list
    void addActionItemList(const QList< QAction* >& acts);
    /// Remove action item
    bool removeActionItem(QAction* act);
    /// Get group model
    SARibbonGalleryGroupModel* groupModel() const;
    /// Set group title
    void setGroupTitle(const QString& title);
    /// Get group title
    QString groupTitle() const;
    /// Select by index
    void selectByIndex(int i);
    /// Set display row
    void setDisplayRow(DisplayRow r);
    /// Get display row
    DisplayRow displayRow() const;
    /// Set grid minimum width (default 0: no limit)
    void setGridMinimumWidth(int w);
    /// Get grid minimum width
    int gridMinimumWidth() const;
    /// Set grid maximum width (default 0: no limit)
    void setGridMaximumWidth(int w);
    /// Get grid maximum width
    int gridMaximumWidth() const;
    /// Get action group
    QActionGroup* actionGroup() const;
    /// Get grid row count
    int gridRowCount() const;
    /// Get grid column count
    int gridColumnCount() const;
    /// Calculate preferred height for width
    int preferredHeightForWidth(int w) const;
    /// Check if has height for width
    bool hasHeightForWidth() const override;
    /// Get height for width
    int heightForWidth(int w) const override;
private Q_SLOTS:
    void onItemClicked(const QModelIndex& index);
    void onItemEntered(const QModelIndex& index);
Q_SIGNALS:
    /**
     * \if ENGLISH
     * @brief Signal when group title changed
     * @param title New title
     * \endif
     *
     * \if CHINESE
     * @brief 组标题改变时的信号
     * @param title 新标题
     * \endif
     */
    void groupTitleChanged(const QString& title);
    /**
     * \if ENGLISH
     * @brief Equivalent to QActionGroup::triggered
     * @details All actions added to SARibbonGalleryGroup are managed by a QActionGroup
     * @param action Triggered action
     * \endif
     *
     * \if CHINESE
     * @brief 等同QActionGroup的triggered
     * @details 所有加入SARibbonGalleryGroup的action都会被一个QActionGroup管理
     * @param action 触发的动作
     * \endif
     */
    void triggered(QAction* action);
    /**
     * \if ENGLISH
     * @brief Equivalent to QActionGroup::hovered
     * @details All actions added to SARibbonGalleryGroup are managed by a QActionGroup
     * @note This property needs to be activated by QAbstractItemView::entered(const QModelIndex &index), so ensure setMouseTracking(true) is set
     * @param action Hovered action
     * \endif
     *
     * \if CHINESE
     * @brief 等同QActionGroup的hovered
     * @details 所有加入SARibbonGalleryGroup的action都会被一个QActionGroup管理
     * @note 此属性需要通过QAbstractItemView::entered(const QModelIndex &index)激活，因此要保证设置了setMouseTracking(true)
     * @param action 悬停的动作
     * \endif
     */
    void hovered(QAction* action);

private:
    /// Setup group model
    SARibbonGalleryGroupModel* setupGroupModel();
};

#endif  // SARIBBONGALLERYGROUP_H
