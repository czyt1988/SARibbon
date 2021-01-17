#ifndef SARIBBONCATEGORYLAYOUT_H
#define SARIBBONCATEGORYLAYOUT_H
#include "SARibbonGlobal.h"
#include <QLayout>
#include <QList>
#include <QMap>
#include "SARibbonCategory.h"
class SARibbonPannel;
class SARibbonCategoryLayoutPrivate;
class SA_RIBBON_EXPORT SARibbonCategoryLayout : public QLayout
{
    Q_OBJECT
public:
    SARibbonCategoryLayout(SARibbonCategory *parent);
    ~SARibbonCategoryLayout();

    SARibbonCategory *ribbonCategory();

    virtual void addItem(QLayoutItem *item) Q_DECL_OVERRIDE;
    virtual QLayoutItem *itemAt(int index) const Q_DECL_OVERRIDE;
    virtual QLayoutItem *takeAt(int index) Q_DECL_OVERRIDE;

    virtual int count() const Q_DECL_OVERRIDE;

    //追加一个pannel
    void addPannel(SARibbonPannel *pannel);

    //插入一个pannel
    void insertPannel(int index, SARibbonPannel *pannel);

    void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSize() const Q_DECL_OVERRIDE;
    Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
    void invalidate() Q_DECL_OVERRIDE;

    //更新尺寸
    void updateGeometryArr();

    //执行位置调整
    void doLayout();

private slots:
    void onLeftScrollButtonClicked();
    void onRightScrollButtonClicked();

private:
    SARibbonCategoryLayoutPrivate *m_d;
};

#endif // SARIBBONCATEGORYLAYOUT_H
