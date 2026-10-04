#ifndef RIBBONCONTEXTCATEGORY_H
#define RIBBONCONTEXTCATEGORY_H
#include "SARibbonQmlGlobal.h"
#include <QQuickItem>
#include <QColor>
#include <QVector>

namespace SARibbonQml {

class RibbonCategory;

/**
 * \if ENGLISH
 * @brief Context category host: groups category pages under a colored band
 * @details The QML counterpart of the widgets SARibbonContextCategory: a
 *          structural item holding RibbonCategory pages plus the context
 *          identity (title + color + active flag). The bar renders the
 *          colored band (theme highlight via core SARibbonThemeData, the
 *          same function the widgets ThemeManager installs) and appends one
 *          colored tab per page to the tab row while active. The `active`
 *          property — NOT the item visibility — drives activation: the item
 *          itself is a transparent structural container whose pages are
 *          shown/hidden by the bar layout.
 * \endif
 *
 * \if CHINESE
 * @brief 上下文标签宿主：在彩色带下分组管理标签页
 * @details 对应 widgets 侧 SARibbonContextCategory：一个结构项持有
 *          RibbonCategory 页面与上下文身份（标题 + 颜色 + active 标志）。
 *          激活期间由 bar 渲染彩色带（高亮经 core SARibbonThemeData，与
 *          widgets ThemeManager 安装的是同一函数）并为每个页面在 tab 行
 *          追加一个着色 tab。激活由 `active` 属性驱动——不是 item 的
 *          visible：本项是透明结构容器，页面的显隐由 bar 布局控制。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonContextCategory : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString contextTitle READ contextTitle WRITE setContextTitle NOTIFY contextTitleChanged)
    Q_PROPERTY(QColor contextColor READ contextColor WRITE setContextColor NOTIFY contextColorChanged)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
public:
    explicit RibbonContextCategory(QQuickItem* parent = nullptr);
    ~RibbonContextCategory() override;

    QString contextTitle() const;
    void setContextTitle(const QString& t);

    QColor contextColor() const;
    void setContextColor(const QColor& c);

    // Activation flag (drives the bar's tab row + band), NOT the item visible
    bool isActive() const;
    void setActive(bool on);

    // Managed category pages (declaration order)
    QVector< RibbonCategory* > categories() const;

    void registerCategory(RibbonCategory* c);
    void unregisterCategory(RibbonCategory* c);

Q_SIGNALS:
    void contextTitleChanged();
    void contextColorChanged();
    void activeChanged();
    void categoryPagesChanged();

protected:
    void itemChange(ItemChange change, const ItemChangeData& data) override;

private:
    QString mContextTitle;
    QColor mContextColor;
    bool mActive = false;
    QVector< RibbonCategory* > mCategories;
};

}

#endif  // RIBBONCONTEXTCATEGORY_H
