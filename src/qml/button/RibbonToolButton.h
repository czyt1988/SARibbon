#ifndef RIBBONTOOLBUTTON_H
#define RIBBONTOOLBUTTON_H
#include "SARibbonQmlGlobal.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonAbstractLayoutItem.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <QQuickItem>

namespace SARibbonQml {

class RibbonPanel;

/**
 * \if ENGLISH
 * @brief Tool button structural host: contract item + QQuickItem (plan-04 S3/S5)
 * @details The contract half supplies engine inputs (sizeHint computed in C++
 * from core metrics — never from QML implicit sizes, the iron rule); the
 * QQuickItem half receives engine geometry via applyGeometry. The QML-visible
 * property names (proportion) differ from the contract field (rowProportion)
 * because the base field name cannot be shadowed by member functions.
 * \endif
 *
 * \if CHINESE
 * @brief 工具按钮结构宿主：契约项 + QQuickItem（计划 04 S3/S5）
 * @details 契约侧提供引擎输入（sizeHint 在 C++ 侧由 core 度量推导——绝不用 QML
 *          implicit 尺寸，铁律）；QQuickItem 侧经 applyGeometry 接收引擎几何。
 *          QML 属性名（proportion）与契约字段名（rowProportion）不同——基类字段
 *          名不能被成员函数遮蔽。
 * \endif
 */
class RibbonToolButton : public QQuickItem, public SARibbon::Core::SARibbonAbstractLayoutItem
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(RibbonEnums::RowProportion proportion READ proportion WRITE setProportion NOTIFY proportionChanged)
    Q_PROPERTY(QQuickItem* buttonQmlItem READ buttonQmlItem WRITE setButtonQmlItem NOTIFY buttonQmlItemChanged)
public:
    explicit RibbonToolButton(QQuickItem* parent = nullptr);
    ~RibbonToolButton() override;

    QString text() const;
    void setText(const QString& t);

    QString iconSource() const;
    void setIconSource(const QString& s);

    // QML-facing enum view of the contract field rowProportion (plan-04 S5)
    RibbonEnums::RowProportion proportion() const;
    void setProportion(RibbonEnums::RowProportion rp);

    // handshake property (plan-04 R5): visual leaf assigns itself back
    QQuickItem* buttonQmlItem() const;
    void setButtonQmlItem(QQuickItem* item);

    // ---- contract implementation (engine inputs/outputs) ----
    QSize sizeHint() const override;                 // C++: core metrics derivation
    bool isHidden() const override;
    Qt::Orientations expandingDirections() const override;
    void applyGeometry(const QRect& rect) override;  // setPosition + setSize
    QString debugName() const override;

Q_SIGNALS:
    void textChanged();
    void iconSourceChanged();
    void proportionChanged();
    void buttonQmlItemChanged();

protected:
    void componentComplete() override;  // register into the parent panel host

private:
    void updateSizeHint();
    void ensureQmlItem();
    QSize computeSizeHintFromMetrics();

    QString mText;
    QString mIconSource;
    QQuickItem* mButtonQmlItem = nullptr;
    QSize mCachedSizeHint;
};

}
#endif  // RIBBONTOOLBUTTON_H
