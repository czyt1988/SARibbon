%Import QtWidgets/QtWidgetsmod.sip

class SARibbonParamGrid : QWidget
{
    %TypeHeaderCode
    #include <SARibbonParamGrid.h>
    %End

public:
    enum LabelSides
    {
        LabelNone,
        LabelLeading,
        LabelTrailing,
        LabelBoth
    };

    enum FlowPolicy
    {
        BalancedFlow,
        ColumnFirstFlow
    };

public:
    explicit SARibbonParamGrid(QWidget* parent /TransferThis/ = Q_NULLPTR);
    ~SARibbonParamGrid();

    int addRow(QWidget* field);
    int addRow(const QString& leading, QWidget* field);
    int addRow(const QString& leading, QWidget* field, const QString& trailing);
    int addRow(QWidget* field,
               LabelSides sides,
               const QString& leading = QString(),
               const QString& trailing = QString());
    int addSpanRow(QWidget* widget);
    bool removeRow(int rowId);
    void clearRows();
    int rowCount() const;
    QWidget* fieldAt(int rowId) const;

    void setRowText(int rowId, const QString& leading, const QString& trailing = QString());
    void setRowLabelSides(int rowId, LabelSides sides);
    QLabel* leadingLabel(int rowId) const;
    QLabel* trailingLabel(int rowId) const;
    void setLabelAlignment(Qt::Alignment alignment);
    int labelColumnWidth() const;
    void setLabelColumnWidth(int px);
    void setLabelColumnWidthRange(int minPx, int maxPx);
    void recalcColumnWidths();

    void setMaxRows(int rows);
    int maxRows() const;
    void setFlowPolicy(FlowPolicy policy);
    FlowPolicy flowPolicy() const;
    int currentRows() const;
    int currentColumns() const;
    void setRowSpacing(int px);
    void setColumnSpacing(int px);
    void setLabelSpacing(int px);
    int labelSpacing() const;

    void attachToPanel(SARibbonPanel* panel);
    SARibbonPanel* panel() const;

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
};
