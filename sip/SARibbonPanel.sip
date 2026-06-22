%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

class SARibbonPanelLabel : QLabel
{
    %TypeHeaderCode
    #include <SARibbonPanel.h>
    %End

public:
    SARibbonPanelLabel(QWidget* parent /TransferThis/ = Q_NULLPTR);
};

class SARibbonPanel : QFrame
{
    %TypeHeaderCode
    #include <SARibbonPanel.h>
    %End

public:
    enum PanelLayoutMode
    {
        ThreeRowMode,
        TwoRowMode,
        SingleRowMode
    };

public:
    explicit SARibbonPanel(QWidget* parent /TransferThis/ = Q_NULLPTR);
    explicit SARibbonPanel(const QString& name, QWidget* parent /TransferThis/ = Q_NULLPTR);
    ~SARibbonPanel();

    void addAction(QAction* action, SARibbonPanelItem::RowProportion rowProportion);
    void addAction(QAction* act,
                   QToolButton::ToolButtonPopupMode popMode,
                   SARibbonPanelItem::RowProportion rowProportion = SARibbonPanelItem::Large);
    void addLargeAction(QAction* action);
    void addMediumAction(QAction* action);
    void addSmallAction(QAction* action);
    void addSmallAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
    void addLargeAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
    void addMediumAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);

    void addMenu(QMenu* menu,
                 SARibbonPanelItem::RowProportion rowProportion,
                 QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    void addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    void addMediumMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    void addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

    QAction* addWidget(QWidget* w, SARibbonPanelItem::RowProportion rowProportion);
    QAction* addSmallWidget(QWidget* w);
    QAction* addMediumWidget(QWidget* w);
    QAction* addLargeWidget(QWidget* w);

    SARibbonGallery* addGallery(bool expanding = true);
    QAction* addSeparator();

    SARibbonToolButton* actionToRibbonToolButton(QAction* action);

    void setOptionAction(QAction* action);
    bool isHaveOptionAction() const;

    QList<SARibbonToolButton*> ribbonToolButtons() const;

    PanelLayoutMode panelLayoutMode() const;
    void setPanelLayoutMode(PanelLayoutMode mode);

    void resetToolButtonSize();
    bool isTwoRow() const;

    void setExpanding(bool isExpanding = true);
    bool isExpanding() const;

    int titleHeight() const;
    bool isEnableShowTitle() const;

    int actionIndex(QAction* act) const;
    void moveAction(int from, int to);

    bool isCanCustomize() const;
    void setCanCustomize(bool b);

    QString panelName() const;
    void setPanelName(const QString& title);

    int largeButtonHeight() const;

signals:
    void actionTriggered(QAction* action);

protected:
    virtual bool event(QEvent* e);
};

%End
