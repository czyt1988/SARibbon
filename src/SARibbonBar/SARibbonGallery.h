#ifndef SARIBBONGALLERY_H
#define SARIBBONGALLERY_H
#include "SARibbonGlobal.h"
#include <QFrame>
#include "SARibbonGalleryGroup.h"
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
    SARibbonGalleryGroup* addGalleryGroup();
    void addGalleryGroup(SARibbonGalleryGroup* group);
    //快速添加一组actions
    SARibbonGalleryGroup* addCategoryActions(const QString& title, QList< QAction* > actions);
    void setCurrentViewGroup(SARibbonGalleryGroup* group);
    SARibbonGalleryGroup* currentViewGroup() const;
    QActionGroup* getActionGroup() const;

public:
    //设置最右边三个控制按钮的最大宽度（默认15）
    static void setGalleryButtonMaximumWidth(int w);
signals:
    /**
     * @brief 等同QActionGroup的triggered
     * 所有加入SARibbonGallery的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
     * @param action
     */
    void triggered(QAction* action);
    void hovered(QAction* action);

protected slots:
    virtual void onPageDown();
    virtual void onPageUp();
    virtual void onShowMoreDetail();
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
    void addWidget(QWidget* w);

private:
    QVBoxLayout* m_layout;
};

#endif  // SARIBBONGALLERY_H
