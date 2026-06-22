%Import QtWidgets/QtWidgetsmod.sip

class SARibbonGalleryGroup : QListView
{
    %TypeHeaderCode
    #include <SARibbonGalleryGroup.h>
    %End

public:
    enum GalleryGroupStyle
    {
        IconWithText,
        IconWithWordWrapText,
        IconOnly
    };

    enum DisplayRow
    {
        DisplayOneRow   = 1,
        DisplayTwoRow   = 2,
        DisplayThreeRow = 3
    };

public:
    SARibbonGalleryGroup(QWidget* w /TransferThis/ = 0);
    virtual ~SARibbonGalleryGroup();

    void recalcGridSize();
    void recalcGridSize(int galleryHeight);

    void setGalleryGroupStyle(GalleryGroupStyle style);
    GalleryGroupStyle galleryGroupStyle() const;

    void addItem(const QString& text, const QIcon& icon);
    void addItem(SARibbonGalleryItem* item /Transfer/);
    void addActionItem(QAction* act);
    void addActionItemList(const QList<QAction*>& acts);
    bool removeActionItem(QAction* act);

    void setGroupTitle(const QString& title);
    QString groupTitle() const;

    void selectByIndex(int i);

    void setDisplayRow(DisplayRow r);
    DisplayRow displayRow() const;

    void setGridMinimumWidth(int w);
    int gridMinimumWidth() const;

    void setGridMaximumWidth(int w);
    int gridMaximumWidth() const;

    QActionGroup* actionGroup() const;
    int gridRowCount() const;
    int gridColumnCount() const;

    int preferredHeightForWidth(int w) const;
    bool hasHeightForWidth() const;
    int heightForWidth(int w) const;

signals:
    void triggered(QAction* action);
    void hovered(QAction* action);
};
