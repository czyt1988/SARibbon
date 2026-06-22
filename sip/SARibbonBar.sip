%Import QtWidgets/QtWidgetsmod.sip

class SARibbonBar : QMenuBar
{
    %TypeHeaderCode
    #include <SARibbonBar.h>
    %End

public:
    enum RibbonStyleFlag
    {
        RibbonStyleLoose       = 0x0001,
        RibbonStyleCompact     = 0x0002,
        RibbonStyleThreeRow    = 0x0010,
        RibbonStyleTwoRow      = 0x0020,
        RibbonStyleSingleRow   = 0x0040,
        RibbonStyleLooseThreeRow   = 0x0011,
        RibbonStyleCompactThreeRow = 0x0012,
        RibbonStyleLooseTwoRow     = 0x0021,
        RibbonStyleCompactTwoRow   = 0x0022,
        RibbonStyleLooseSingleRow   = 0x0041,
        RibbonStyleCompactSingleRow = 0x0042
    };
    typedef QFlags<SARibbonBar::RibbonStyleFlag> RibbonStyles;

    enum RibbonMode
    {
        MinimumRibbonMode,
        NormalRibbonMode
    };

    bool isTwoRowStyle(RibbonStyles s);
    bool isThreeRowStyle(RibbonStyles s);
    bool isLooseStyle(RibbonStyles s);
    bool isCompactStyle(RibbonStyles s);
    bool isSingleRowStyle(RibbonStyles s);
    QString versionString();
    QList<QColor> defaultContextCategoryColorList();
    void initHighDpi();

public:
    explicit SARibbonBar(QWidget* parent /TransferThis/ = Q_NULLPTR);
    ~SARibbonBar();

    QAbstractButton* applicationButton();
    void setApplicationButton(QAbstractButton* btn /Transfer/);
    SARibbonTabBar* ribbonTabBar();

    SARibbonCategory* addCategoryPage(const QString& title);
    void addCategoryPage(SARibbonCategory* category /Transfer/);

    SARibbonCategory* insertCategoryPage(const QString& title, int index);
    void insertCategoryPage(SARibbonCategory* category /Transfer/, int index);

    SARibbonCategory* categoryByName(const QString& title) const;
    SARibbonCategory* categoryByObjectName(const QString& objname) const;
    SARibbonCategory* categoryByIndex(int index) const;

    void hideCategory(SARibbonCategory* category);
    void showCategory(SARibbonCategory* category);
    bool isCategoryVisible(const SARibbonCategory* c) const;
    int categoryIndex(const SARibbonCategory* c) const;
    void moveCategory(int from, int to);
    QList<SARibbonCategory*> categoryPages(bool getAll = true) const;
    void removeCategory(SARibbonCategory* category /TransferBack/);

    SARibbonContextCategory* addContextCategory(const QString& title, const QColor& color = QColor(), const QVariant& id = QVariant());
    void addContextCategory(SARibbonContextCategory* context /Transfer/);
    void showContextCategory(SARibbonContextCategory* context);
    void hideContextCategory(SARibbonContextCategory* context);
    bool isContextCategoryVisible(SARibbonContextCategory* context);
    void setContextCategoryVisible(SARibbonContextCategory* context, bool visible);
    QList<SARibbonContextCategory*> contextCategoryList() const;
    void destroyContextCategory(SARibbonContextCategory* context /TransferBack/);

    void setMinimumMode(bool isHide);
    bool isMinimumMode() const;
    void showMinimumModeButton(bool isShow = true);
    bool haveShowMinimumModeButton() const;
    QAction* minimumModeAction() const;

    bool isEnableTabDoubleClickToMinimumMode() const;
    void setTabDoubleClickToMinimumMode(bool on = true) const;

    RibbonMode currentRibbonState() const;

    int tabBarHeight() const;
    void setTabBarHeight(int h);
    int titleBarHeight() const;
    void setTitleBarHeight(int h);

    RibbonStyles currentRibbonStyle() const;
    void setRibbonStyle(RibbonStyles s);

    bool isEnableWordWrap() const;
    void setEnableWordWrap(bool on = true);

    bool isEnableShowPanelTitle() const;
    void setEnableShowPanelTitle(bool on = true);

    bool isEnableIconRightText() const;
    void setEnableIconRightText(bool on = true);

    bool isTabOnTitle() const;
    void setTabOnTitle(bool on = true);

    SARibbonPanel::PanelLayoutMode panelLayoutMode() const;
    void setPanelLayoutMode(SARibbonPanel::PanelLayoutMode m);

    int panelTitleHeight() const;
    void setPanelTitleHeight(int h);

    int panelSpacing() const;
    void setPanelSpacing(int n);

    void setPanelToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);

    QColor windowTitleTextColor() const;
    void setWindowTitleTextColor(const QColor& c);

    QColor tabBarBaseLineColor() const;
    void setTabBarBaseLineColor(const QColor& c);

    Qt::Alignment windowTitleAligment() const;
    void setWindowTitleAligment(Qt::Alignment a);

    SARibbonQuickAccessBar* quickAccessBar();
    SARibbonButtonGroupWidget* rightButtonGroup();

    int currentIndex();
    void setCurrentIndex(int index);


signals:
    void currentRibbonTabChanged(int index);
    void ribbonModeChanged(SARibbonBar::RibbonMode nowState);
    void ribbonStyleChanged(SARibbonBar::RibbonStyles nowStyle);
    void titleBarHeightChanged(int oldHeight, int newHeight);
    void actionTriggered(QAction* action);

protected:
    bool eventFilter(QObject* obj, QEvent* e);
};
