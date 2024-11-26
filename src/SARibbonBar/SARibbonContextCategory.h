#ifndef SARIBBONCONTEXTCATEGORY_H
#define SARIBBONCONTEXTCATEGORY_H
#include "SARibbonGlobal.h"
#include "SARibbonCategory.h"
#include <QWidget>

/**
 * @brief 管理上下文标签的类
 */
class SA_RIBBON_EXPORT SARibbonContextCategory : public QObject
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonContextCategory)
public:
    explicit SARibbonContextCategory(QWidget* parent = nullptr);
    ~SARibbonContextCategory();
    //上下文目录添加下属目录
    SARibbonCategory* addCategoryPage(const QString& title);
    void addCategoryPage(SARibbonCategory* category);
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
    SARibbonCategory* categoryPage(int index);

    //获取所有的SARibbonCategory*
    QList< SARibbonCategory* > categoryList() const;

    //移除category
    bool takeCategory(SARibbonCategory* category);

    //判断上下文是否维护了此SARibbonCategory
    bool isHaveCategory(SARibbonCategory* category) const;
Q_SIGNALS:
    /**
     * @brief 标签加入上下文
     * @param category
     */
    void categoryPageAdded(SARibbonCategory* category);

    /**
     * @brief 标签从上下文移除
     * @param category
     */
    void categoryPageRemoved(SARibbonCategory* category);

    /**
     * @brief 上下文的标题发生改变
     * @param title
     */
    void contextTitleChanged(const QString& title);

    /**
     * @brief 上下文标签维护的标签页名字发生了改变
     * @param category 发生改变的上下文标签页
     * @param title 新名字
     */
    void categoryTitleChanged(SARibbonCategory* category, const QString& title);
private Q_SLOTS:
    void onCategoryTitleChanged(const QString& title);

protected:
    //获取父级窗口
    QWidget* parentWidget() const;
    virtual bool eventFilter(QObject* watched, QEvent* e) override;
};

#endif  // SARIBBONCONTEXTCATEGORY_H
