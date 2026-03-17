#ifndef SARIBBONGALLERYITEM_H
#define SARIBBONGALLERYITEM_H
#include "SARibbonGlobal.h"
#include <QIcon>
#include <QVariant>
#include <QMap>
#include <QAction>
class SARibbonGalleryGroup;

/**
 * \if ENGLISH
 * @brief GalleryItem similar to QStandardItem
 * \endif
 *
 * \if CHINESE
 * @brief 类似QStandardItem的GalleryItem
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGalleryItem
{
    friend class SARibbonGalleryGroupModel;

public:
    /// Default constructor
    explicit SARibbonGalleryItem();
    /// Constructor with text and icon
    explicit SARibbonGalleryItem(const QString& text, const QIcon& icon);
    /// Constructor with QAction
    explicit SARibbonGalleryItem(QAction* act);
    /// Destructor
    virtual ~SARibbonGalleryItem();
    /// Set data for role
    void setData(int role, const QVariant& data);
    /// Get data for role
    virtual QVariant data(int role) const;

    /// Set text
    void setText(const QString& text);
    /// Get text
    QString text() const;

    /// Set tool tip
    void setToolTip(const QString& text);
    /// Get tool tip
    QString toolTip() const;

    /// Set icon
    void setIcon(const QIcon& ico);
    /// Get icon
    QIcon icon() const;

    /// Check if selectable
    bool isSelectable() const;
    /// Set selectable
    void setSelectable(bool isSelectable);

    /// Check if enabled
    bool isEnable() const;
    /// Set enable
    void setEnable(bool isEnable);

    /// Set item flags
    void setFlags(Qt::ItemFlags flag);
    /// Get item flags
    virtual Qt::ItemFlags flags() const;

    /// Set action
    void setAction(QAction* act);
    /// Get action
    QAction* action();

    /// Set text alignment
    void setTextAlignment(Qt::Alignment a);
    /// Get text alignment
    Qt::Alignment textAlignment() const;

private:
    QMap< int, QVariant > mDatas;
    Qt::ItemFlags mFlags;
    QAction* mAction;
};

#endif  // SARIBBONGALLERYITEM_H
