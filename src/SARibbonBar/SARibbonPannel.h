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
/// \brief pannel页窗口，pannel是ribbon的面板用于盛放控件
///
class SA_RIBBON_EXPORT SARibbonPannel : public QWidget
{
    Q_OBJECT
public:
    SARibbonPannel(QWidget *parent = 0);
    ~SARibbonPannel();
    using QWidget::addAction;
    enum PannelLayoutMode {
        ThreeRowMode    ///< 三行布局模式，office就是三行布局模式，pannel能布置3行小toolbutton，默认模式
        , TwoRowMode    ///< 两行布局模式，wps的后续布局模式就是两行布局模式，pannel能布置2行小toolbutton
    };

    SARibbonToolButton *addLargeToolButton(const QString& text, const QIcon& icon, QToolButton::ToolButtonPopupMode popMode);
    SARibbonToolButton *addLargeAction(QAction *action);
    SARibbonToolButton *addSmallToolButton(const QString& text, const QIcon& icon, QToolButton::ToolButtonPopupMode popMode);
    SARibbonToolButton *addSmallAction(QAction *action);
    SARibbonToolButton *addMediumAction(QAction *action);
    SARibbonToolButton *addLargeMenu(SARibbonMenu *menu);
    SARibbonToolButton *addSmallMenu(SARibbonMenu *menu);
    SARibbonToolButton *addLargeActionMenu(QAction *action, SARibbonMenu *menu);
    SARibbonGallery *addGallery();

    //添加分割线
    void addSeparator();
    void addSmallWidget(QWidget *w);
    void addLargeWidget(QWidget *w);
    void addOptionAction(QAction *action);

    //获取所有的buttons
    QList<SARibbonToolButton *> toolButtons() const;

    //设置PannelLayoutMode
    void setPannelLayoutMode(PannelLayoutMode mode);
    PannelLayoutMode pannelLayoutMode() const;
    int gridLayoutColumnCount() const;
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    void setReduce(bool isReduce);
    void setExpanding(bool isExpanding = true);
    bool isExpanding() const;

protected:
    void addWidget(QWidget *w, int row, int rowSpan);
    void addWidget(QWidget *w, int row, int rowSpan, int column, int columnSpan);
    void resetLayout(PannelLayoutMode newmode);
    void resetLargeToolButtonStyle();
    static QSize maxHightIconSize(const QSize& size, int h);
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual int separatorHeight() const;

private:
    SARibbonPannelPrivate *m_d;
};

#endif // SARIBBONPANNEL_H
