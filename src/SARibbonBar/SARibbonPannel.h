#ifndef SARIBBONPANNEL_H
#define SARIBBONPANNEL_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QLayout>
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
    void addSeparator(int top=6, int bottom=6);
    void addSmallWidget(QWidget *w);
    void addLargeWidget(QWidget *w);
    //添加操作action，如果要去除，传入nullptr指针即可，SARibbonPannel不会对QAction的所有权进行管理
    void addOptionAction(QAction *action);

    //获取所有的buttons
    QList<SARibbonToolButton *> toolButtons() const;

    //设置PannelLayoutMode
    void setPannelLayoutMode(PannelLayoutMode mode);
    PannelLayoutMode pannelLayoutMode() const;
    int gridLayoutColumnCount() const;
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    void setExpanding(bool isExpanding = true);
    bool isExpanding() const;
signals:
    /**
     * @brief 等同于QToolBar::actionTriggered
     * @param action
     */
    void actionTriggered(QAction *action);
protected:
    void addWidget(QWidget *w, int row, int rowSpan);
    void addWidget(QWidget *w, int row, int rowSpan, int column, int columnSpan);
    void resetLayout(PannelLayoutMode newmode);
    void resetLargeToolButtonStyle();
    static QSize maxHightIconSize(const QSize& size, int h);
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent *e) Q_DECL_OVERRIDE;

private:
    SARibbonPannelPrivate *m_d;
};

class SARibbonPannelItem : public QWidgetItem
{
public:
    SARibbonPannelItem(QWidget *widget);
    bool isEmpty() const Q_DECL_OVERRIDE;

    QAction *action;
    bool customWidget;
};

class SA_RIBBON_EXPORT SARibbonPannelLayout : public QLayout
{
    Q_OBJECT
public:
    SARibbonPannelLayout(QWidget *p = 0);
    ~SARibbonPannelLayout();
    //SARibbonPannelLayout additem 无效
    void addItem(QLayoutItem *item) Q_DECL_OVERRIDE;
    //
    QLayoutItem *itemAt(int index) const Q_DECL_OVERRIDE;
    QLayoutItem *takeAt(int index) Q_DECL_OVERRIDE;
    int count() const Q_DECL_OVERRIDE;

    bool isEmpty() const Q_DECL_OVERRIDE;
    void invalidate() Q_DECL_OVERRIDE;
    Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;

    void setGeometry(const QRect &rect) Q_DECL_OVERRIDE;
    QSize minimumSize() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
public:
    bool isExpand() const;
protected:
    //把action转换为item，此函数会创建SARibbonToolButton
    SARibbonPannelItem* createItem(QAction* action);
    void updateGeomArray() const;
private:
    QList<SARibbonPannelItem*> m_items;
    bool m_expandFlag; ///< 标记是否是会扩展的
};


#endif // SARIBBONPANNEL_H
