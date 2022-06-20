#ifndef SARIBBONGALLERY_H
#define SARIBBONGALLERY_H
#include "SARibbonGlobal.h"
#include <QFrame>
#include "SARibbonGalleryGroup.h"
#include <QSizeGrip>
class QLabel;
class QVBoxLayout;
class SARibbonGalleryPrivate;
class SARibbonGalleryViewport;

/**
 * @brief Gallery控件
 *
 * Gallery控件是由一个当前激活的@sa SARibbonGalleryGroup 和弹出的 @sa SARibbonGalleryViewport 组成
 *
 * @sa SARibbonGalleryGroup 是继承@sa QListView actions通过icon展示出来，相关的属性可以按照QListView设置
 *
 * @sa SARibbonGalleryViewport 是一个内部有垂直布局的窗体，在弹出激活时，把管理的SARibbonGalleryGroup都展示出来
 *
 * 示例如下：
 * @code
 * SARibbonGallery* gallery = pannel1->addGallery();
 * QList< QAction* > galleryActions;
 * ...create many actions ...
 * SARibbonGalleryGroup* group1 = gallery->addCategoryActions(tr("Files"), galleryActions);
 * galleryActions.clear();
 * ...create many actions ...
 * gallery->addCategoryActions(tr("Apps"), galleryActions);
 * gallery->setCurrentViewGroup(group1);
 * @endcode
 */
class SA_RIBBON_EXPORT SARibbonGallery : public QFrame
{
    Q_OBJECT
public:
    SARibbonGallery(QWidget* parent = 0);
    virtual ~SARibbonGallery();
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    //添加一个GalleryGroup
    SARibbonGalleryGroup* addGalleryGroup();
    //添加一个GalleryGroup
    void addGalleryGroup(SARibbonGalleryGroup* group);
    //快速添加一组actions
    SARibbonGalleryGroup* addCategoryActions(const QString& title, QList< QAction* > actions);
    //设置当前显示的SARibbonGalleryGroup
    void setCurrentViewGroup(SARibbonGalleryGroup* group);
    //获取当前显示的SARibbonGalleryGroup
    SARibbonGalleryGroup* currentViewGroup() const;
    //获取弹出窗口指针
    SARibbonGalleryViewport* getPopupViewPort() const;

public:
    //设置最右边三个控制按钮的最大宽度（默认15）
    static void setGalleryButtonMaximumWidth(int w);
signals:
    /**
     * @brief 转发管理的SARibbonGalleryGroup::triggered
     * 所有加入SARibbonGallery的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
     * @param action
     */
    void triggered(QAction* action);
    /**
     * @brief 转发管理的SARibbonGalleryGroup::hovered
     * @note 此属性需要确保SARibbonGalleryGroup::setMouseTracking(true)
     * @param action
     */
    void hovered(QAction* action);

public slots:
    //上翻页
    virtual void pageUp();
    //下翻页
    virtual void pageDown();
    //显示更多触发，默认弹出内部管理的SARibbonGalleryViewport，用户可重载此函数实现自定义的弹出
    virtual void showMoreDetail();
protected slots:
    void onItemClicked(const QModelIndex& index);
    virtual void onTriggered(QAction* action);

private:
    SARibbonGalleryViewport* ensureGetPopupViewPort();

protected:
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

private:
    friend class SARibbonGalleryPrivate;
    SARibbonGalleryPrivate* m_d;
};

///
/// \brief SARibbonGallery的Viewport class
///
class SARibbonGalleryViewport : public QWidget
{
    Q_OBJECT
public:
    SARibbonGalleryViewport(QWidget* parent);
    //添加窗口不带标题
    void addWidget(QWidget* w);
    //添加窗口，带标题
    void addWidget(QWidget* w, const QString& title);
    //通过SARibbonGalleryGroup获取对应的标题，用户可以通过此函数设置QLabel的属性
    QLabel* getWidgetTitleLabel(QWidget* w);
    //移除窗口
    void removeWidget(QWidget* w);
public slots:
    void widgetTitleChanged(QWidget* w, const QString& title);

private:
    QVBoxLayout* m_layout;
    QMap< QWidget*, QLabel* > _widgetToTitleLable;  ///< QWidget和lable的对应
};

#endif  // SARIBBONGALLERY_H
