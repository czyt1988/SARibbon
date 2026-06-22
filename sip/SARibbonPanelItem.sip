%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

class SARibbonPanelItem : QWidgetItem
{
    %TypeHeaderCode
    #include <SARibbonPanelItem.h>
    %End

public:
    enum RowProportion
    {
        None,
        Large,
        Medium,
        Small
    };

    explicit SARibbonPanelItem(QWidget* widget);
    ~SARibbonPanelItem();
    bool isEmpty() const;
};

%End
