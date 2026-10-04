#ifndef RIBBONTAB_H
#define RIBBONTAB_H
#include "SARibbonQmlGlobal.h"
#include "SARibbonQmlQuickHost.h"
#include <QColor>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Tab host inside the bar's tab row (plan-04 S4)
 * @details Tab geometry is computed by the bar host and applied via
 *          setPosition/setSize; this host exposes text/highlight state.
 * \endif
 *
 * \if CHINESE
 * @brief bar 标签行内的 tab 宿主（计划 04 S4）
 * @details tab 几何由 bar 宿主统一计算并 setPosition/setSize 应用；本宿主暴露
 *          文本/高亮状态。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonTab : public RibbonQuickHost
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool current READ isCurrent WRITE setCurrent NOTIFY currentChanged)
    Q_PROPERTY(QColor contextColor READ contextColor WRITE setContextColor NOTIFY contextColorChanged)
public:
    explicit RibbonTab(QQuickItem* parent = nullptr);
    ~RibbonTab() override;

    QString text() const;
    void setText(const QString& t);

    bool isCurrent() const;
    void setCurrent(bool c);

    QColor contextColor() const;
    void setContextColor(const QColor& c);

Q_SIGNALS:
    void textChanged();
    void currentChanged();
    void contextColorChanged();
    void clicked();

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QString mText;
    bool mCurrent = false;
    QColor mContextColor;
};

}

#endif  // RIBBONTAB_H
