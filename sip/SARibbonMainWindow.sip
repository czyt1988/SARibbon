%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

class SARibbonMainWindow : QMainWindow
{
    %TypeHeaderCode
    #include <SARibbonMainWindow.h>
    %End

public:
    SARibbonMainWindow(QWidget* parent /TransferThis/ = nullptr);
    ~SARibbonMainWindow();

    SARibbonBar* ribbonBar() const;
    void setRibbonBar(SARibbonBar* ribbon /Transfer/);
    void updateWindowFlag(Qt::WindowFlags flags);
    void setRibbonTheme(int theme);
    %MethodCode
        sipCpp->setRibbonTheme((::SARibbonTheme)a0);
    %End

    int ribbonTheme() const;
    %MethodCode
        sipRes = (int)sipCpp->ribbonTheme();
    %End

    bool isUseRibbon() const;
    int ribbonMainwindowStyle() const;

protected:
    SARibbonBar* createRibbonBar();
};

%End
