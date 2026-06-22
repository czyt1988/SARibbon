%Import QtWidgets/QtWidgetsmod.sip

class SARibbonGalleryButton : QToolButton
{
    %TypeHeaderCode
    #include <SARibbonGallery.h>
    %End

public:
    explicit SARibbonGalleryButton(QWidget* parent /TransferThis/ = Q_NULLPTR);
    ~SARibbonGalleryButton();
};

class SARibbonGalleryViewport : QScrollArea
{
    %TypeHeaderCode
    #include <SARibbonGallery.h>
    %End

public:
    explicit SARibbonGalleryViewport(QWidget* parent /TransferThis/);
    void addWidget(QWidget* w);
    void addWidget(QWidget* w, const QString& title);
    QLabel* titleLabel(QWidget* w);
    void removeWidget(QWidget* w);
    QList<SARibbonGalleryGroup*> galleryGroupList() const;
    int galleryHeight() const;
    int heightHintForWidth(int w) const;
};

class SARibbonGallery : QFrame
{
    %TypeHeaderCode
    #include <SARibbonGallery.h>
    %End

public:
    explicit SARibbonGallery(QWidget* parent /TransferThis/ = Q_NULLPTR);
    virtual ~SARibbonGallery();

    SARibbonGalleryGroup* addGalleryGroup();
    void addGalleryGroup(SARibbonGalleryGroup* group /Transfer/);
    SARibbonGalleryGroup* addCategoryActions(const QString& title, QList<QAction*> actions);
    void setCurrentViewGroup(SARibbonGalleryGroup* group);
    SARibbonGalleryGroup* currentViewGroup() const;
    SARibbonGalleryViewport* getPopupViewPort() const;

    static void setGalleryButtonMaximumWidth(int w);
    void setSingleRowMode(bool on);
    bool isSingleRowMode() const;

signals:
    void triggered(QAction* action);
    void hovered(QAction* action);

public slots:
    virtual void pageUp();
    virtual void pageDown();
    virtual void showMoreDetail();

protected:
    void resizeEvent(QResizeEvent* event);
    void changeEvent(QEvent* event);
};
