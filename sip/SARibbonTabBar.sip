%Import QtWidgets/QtWidgetsmod.sip

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
