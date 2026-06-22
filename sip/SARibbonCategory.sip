%Import QtWidgets/QtWidgetsmod.sip

class SARibbonCategory : QFrame
{
    %TypeHeaderCode
    #include <SARibbonCategory.h>
    %End

public:
    explicit SARibbonCategory(QWidget* p /TransferThis/ = Q_NULLPTR);
    explicit SARibbonCategory(const QString& name, QWidget* p /TransferThis/ = Q_NULLPTR);
    ~SARibbonCategory();

    QString categoryName() const;
    void setCategoryName(const QString& title);

    SARibbonPanel::PanelLayoutMode panelLayoutMode() const;
    void setPanelLayoutMode(SARibbonPanel::PanelLayoutMode m);

    SARibbonPanel* addPanel(const QString& title);
    void addPanel(SARibbonPanel* panel /Transfer/);

    SARibbonPanel* insertPanel(const QString& title, int index);
    void insertPanel(SARibbonPanel* panel, int index /Transfer/);

    SARibbonPanel* panelByName(const QString& title) const;
    SARibbonPanel* panelByObjectName(const QString& objname) const;
    SARibbonPanel* panelByIndex(int index) const;

    int panelIndex(SARibbonPanel* p) const;
    void movePanel(int from, int to);
    bool takePanel(SARibbonPanel* panel /TransferBack/);
    bool removePanel(SARibbonPanel* panel);
    bool removePanel(int index);
    QList<SARibbonPanel*> panelList() const;

    bool isContextCategory() const;
    int panelCount() const;

    bool isCanCustomize() const;
    void setCanCustomize(bool b);

    int panelTitleHeight() const;
    void setPanelTitleHeight(int h);

    bool isEnableShowPanelTitle() const;
    void setEnableShowPanelTitle(bool on);

    void setCategoryAlignment(int al);
    %MethodCode
        sipCpp->setCategoryAlignment((::SARibbonAlignment)a0);
    %End
    int categoryAlignment() const;
    %MethodCode
        sipRes = (int)sipCpp->categoryAlignment();
    %End

    void setPanelSpacing(int n);
    int panelSpacing() const;

    void setPanelLargeIconSize(const QSize& largeSize);
    QSize panelLargeIconSize() const;

    void setPanelSmallIconSize(const QSize& smallSize);
    QSize panelSmallIconSize() const;

    SARibbonBar* ribbonBar() const;

    void updateItemGeometry();

    void setUseAnimatingScroll(bool useAnimating);
    bool isUseAnimatingScroll() const;

    void setWheelScrollStep(int step);
    int wheelScrollStep() const;

    void setAnimationDuration(int duration);
    int animationDuration() const;

    QSize sizeHint() const;

signals:
    void categoryNameChanged(const QString& n);
    void actionTriggered(QAction* action);

protected:
    virtual bool event(QEvent* e);
    void wheelEvent(QWheelEvent* event);
    void changeEvent(QEvent* event);
};

