%Import QtWidgets/QtWidgetsmod.sip

class SARibbonGalleryItem
{
    %TypeHeaderCode
    #include <SARibbonGalleryItem.h>
    %End

public:
    explicit SARibbonGalleryItem();
    explicit SARibbonGalleryItem(const QString& text, const QIcon& icon);
    explicit SARibbonGalleryItem(QAction* act);
    virtual ~SARibbonGalleryItem();

    void setData(int role, const QVariant& data);
    virtual QVariant data(int role) const;

    void setText(const QString& text);
    QString text() const;

    void setToolTip(const QString& text);
    QString toolTip() const;

    void setIcon(const QIcon& ico);
    QIcon icon() const;

    bool isSelectable() const;
    void setSelectable(bool isSelectable);

    bool isEnable() const;
    void setEnable(bool isEnable);

    void setFlags(Qt::ItemFlags flag);
    virtual Qt::ItemFlags flags() const;

    void setAction(QAction* act);
    QAction* action();

    void setTextAlignment(Qt::Alignment a);
    Qt::Alignment textAlignment() const;
};
