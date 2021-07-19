#ifndef SARIBBONCONTEXTCATEGORY_H
#define SARIBBONCONTEXTCATEGORY_H
#include "SARibbonGlobal.h"
#include "SARibbonCategory.h"
#include <QWidget>
class SARibbonContextCategoryPrivate;


/**
 * @brief 管理上下文标签的类
 */
class SA_RIBBON_EXPORT SARibbonContextCategory : public QObject
{
    Q_OBJECT
public:
    SARibbonContextCategory(QWidget *parent = 0);
    ~SARibbonContextCategory();
    //上下文目录添加下属目录
    SARibbonCategory *addCategoryPage(const QString& title);

    //获取上下文标签下管理的标签个数
    int categoryCount() const;

    //设置id
    void setId(const QVariant& id);
    QVariant id() const;

    //设置上下文颜色
    void setContextColor(const QColor color);
    QColor contextColor() const;

    //上下文标签的内容
    QString contextTitle() const;
    void setContextTitle(const QString& contextTitle);

    //获取对应的tab页
    SARibbonCategory *categoryPage(int index);

    //获取所有的SARibbonCategory*
    QList<SARibbonCategory *> categoryList() const;

    //移除category
    bool takeCategory(SARibbonCategory *category);

signals:
    void categoryPageAdded(SARibbonCategory *category);
    void categoryPageRemoved(SARibbonCategory *category);

protected:
    //获取父级窗口
    QWidget *parentWidget() const;
    virtual bool eventFilter(QObject *watched, QEvent *e) override;

private:
    SARibbonContextCategoryPrivate *m_d;
};

#endif // SARIBBONCONTEXTCATEGORY_H
