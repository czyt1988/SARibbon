%Import QtWidgets/QtWidgetsmod.sip

class SARibbonContextCategory : QObject
{
    %TypeHeaderCode
    #include <SARibbonContextCategory.h>
    %End

public:
    explicit SARibbonContextCategory(QWidget* parent /TransferThis/ = Q_NULLPTR);
    ~SARibbonContextCategory();

    SARibbonCategory* addCategoryPage(const QString& title);
    void addCategoryPage(SARibbonCategory* category /Transfer/);
    int categoryCount() const;

    void setId(const QVariant& id);
    QVariant id() const;

    void setContextColor(const QColor color);
    QColor contextColor() const;

    QString contextTitle() const;
    void setContextTitle(const QString& contextTitle);

    SARibbonCategory* categoryPage(int index);
    QList<SARibbonCategory*> categoryList() const;

    bool takeCategory(SARibbonCategory* category /TransferBack/);
    bool isHaveCategory(SARibbonCategory* category) const;

public slots:
    void hide();
    void show();

signals:
    void categoryPageAdded(SARibbonCategory* category);
    void categoryPageRemoved(SARibbonCategory* category);
    void contextTitleChanged(const QString& title);
    void categoryTitleChanged(SARibbonCategory* category, const QString& title);

protected:
    virtual bool eventFilter(QObject* watched, QEvent* e);
};
