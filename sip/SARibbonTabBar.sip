%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

class SARibbonTabBar : QTabBar
{
    %TypeHeaderCode
    #include <SARibbonTabBar.h>
    %End

public:
    explicit SARibbonTabBar(QWidget* parent /TransferThis/ = Q_NULLPTR);
    ~SARibbonTabBar();
    const QMargins& tabMargin() const;
    void setTabMargin(const QMargins& tabMargin);

protected:
    QSize tabSizeHint(int index) const;
};

%End
