#ifndef SARIBBONPANNEL_H
#define SARIBBONPANNEL_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include "SARibbonToolButton.h"
class SARibbonMenu;
class SARibbonGallery;
class QGridLayout;
class SARibbonPannelOptionButton;
class SARibbonPannelPrivate;
///
/// \brief The SARibbonPannel class
///
class SA_RIBBON_EXPORT SARibbonPannel : public QWidget
{
    Q_OBJECT
public:
    SARibbonPannel(QWidget* parent = 0);
    ~SARibbonPannel();
    using QWidget::addAction;
    SARibbonToolButton* addLargeToolButton(const QString& text,const QIcon& icon,QToolButton::ToolButtonPopupMode popMode);
    SARibbonToolButton* addLargeAction(QAction *action);
    SARibbonToolButton* addSmallToolButton(const QString& text,const QIcon& icon,QToolButton::ToolButtonPopupMode popMode);
    SARibbonToolButton *addSmallAction(QAction *action);
    SARibbonToolButton *addMediumAction(QAction *action);
    SARibbonToolButton *addLargeMenu(SARibbonMenu *menu);
    SARibbonToolButton *addSmallMenu(SARibbonMenu *menu);
    SARibbonToolButton* addLargeActionMenu(QAction *action,SARibbonMenu *menu);
    SARibbonGallery* addGallery();
    void addSeparator();
    void addWidget(QWidget* w);
    void addWidget(QWidget* w, int row,int rowSpan);
    void addWidget(QWidget* w, int row,int rowSpan,int column,int columnSpan);
    int gridLayoutColumnCount() const;
    void addOptionAction(QAction* action);
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    void setReduce(bool isReduce);
    void setExpanding(bool isExpanding = true);
    bool isExpanding() const;
protected:
    static QSize maxHightIconSize(const QSize& size,int height);
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
private:
    SARibbonPannelPrivate* m_d;
};

#endif // SARIBBONPANNEL_H
