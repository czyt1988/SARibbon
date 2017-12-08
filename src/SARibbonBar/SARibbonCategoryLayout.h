#ifndef SARIBBONCATEGORYLAYOUT_H
#define SARIBBONCATEGORYLAYOUT_H
#include "SARibbonGlobal.h"
#include <QLayout>
#include <QList>
#include <QMap>
#include "SARibbonCategory.h"
class SARibbonReduceActionInfo;
class SARibbonPannel;
class SA_RIBBON_EXPORT SARibbonCategoryLayout : public QLayout
{
public:
    SARibbonCategoryLayout(SARibbonCategory *parent);
    SARibbonCategoryLayout();
    ~SARibbonCategoryLayout();

    SARibbonCategory* ribbonCategory();

    virtual void addItem(QLayoutItem *item) Q_DECL_OVERRIDE;
    virtual QLayoutItem *itemAt(int index) const Q_DECL_OVERRIDE;
    virtual QLayoutItem *takeAt(int index) Q_DECL_OVERRIDE;
    virtual int count() const Q_DECL_OVERRIDE;
    
    void setGeometry(const QRect &rect) Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSize() const Q_DECL_OVERRIDE;
    Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
    void invalidate() Q_DECL_OVERRIDE;
protected:
    int buildReduceModePannel(SARibbonPannel* realPannel, int x, int y);
    static QPoint calcPopupPannelPosition(SARibbonCategory* category,SARibbonPannel *pannel, int x);
private:
    bool m_isChanged;
    QList<QLayoutItem *> itemList;
    QMap<SARibbonPannel*,SARibbonReduceActionInfo> m_pannelReduceInfo;
};

#endif // SARIBBONCATEGORYLAYOUT_H
