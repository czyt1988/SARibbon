%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

class SARibbonToolButton : QToolButton
{
    %TypeHeaderCode
    #include <SARibbonToolButton.h>
    %End

public:
    enum RibbonButtonType
    {
        LargeButton,
        SmallButton
    };

public:
    explicit SARibbonToolButton(QWidget* parent /TransferThis/ = Q_NULLPTR);
    explicit SARibbonToolButton(QAction* defaultAction, QWidget* parent /TransferThis/ = Q_NULLPTR);
    ~SARibbonToolButton();

    RibbonButtonType buttonType() const;
    void setButtonType(const RibbonButtonType& buttonType);

    void setEnableIconRightText(bool on);
    bool isEnableIconRightText() const;

    bool isSmallRibbonButton() const;
    bool isLargeRibbonButton() const;

    int spacing() const;
    void setSpacing(int v);

    void updateRect();

    void setEnableWordWrap(bool on);
    bool isEnableWordWrap() const;

    void setButtonMaximumAspectRatio(qreal v = 1.4);
    qreal buttonMaximumAspectRatio() const;

    void invalidateSizeHint();

    void setLargeIconSize(const QSize& largeSize);
    QSize largeIconSize() const;

    void setSmallIconSize(const QSize& smallSize);
    QSize smallIconSize() const;

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void resizeEvent(QResizeEvent* e);
    virtual bool event(QEvent* e);
};

%End
