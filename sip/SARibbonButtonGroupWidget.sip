%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

class SARibbonButtonGroupWidget : QToolBar
{
    %TypeHeaderCode
    #include <SARibbonButtonGroupWidget.h>
    %End

public:
    explicit SARibbonButtonGroupWidget(QWidget* parent /TransferThis/ = Q_NULLPTR);
    ~SARibbonButtonGroupWidget();

    void addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
    QAction* addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
};

class SARibbonQuickAccessBar : SARibbonButtonGroupWidget
{
    %TypeHeaderCode
    #include <SARibbonQuickAccessBar.h>
    %End

public:
    explicit SARibbonQuickAccessBar(QWidget* parent /TransferThis/ = Q_NULLPTR);
    ~SARibbonQuickAccessBar();
};

%End
