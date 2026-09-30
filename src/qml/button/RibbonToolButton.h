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
 * QQuickItem half receives engine geometry via applyGeometry. Interaction
 * mirrors SARibbonToolButton on the widgets side: the visual leaf's
 * MouseArea calls the click() invokable, which emits clicked() and toggles
 * checked when checkable; hover/press feedback stays leaf-side and reads the
 * checked state back through this host.
 * \endif
 *
 * \if CHINESE
 * @brief 工具按钮结构宿主：契约项 + QQuickItem（计划 04 S3/S5）
 * @details 契约侧提供引擎输入（sizeHint 在 C++ 侧由 core 度量推导——绝不用 QML
 *          implicit 尺寸，铁律）；QQuickItem 侧经 applyGeometry 接收引擎几何。
 *          交互对照 widgets 侧 SARibbonToolButton：视觉叶子的 MouseArea 调用
 *          click() 可调用方法，由宿主发射 clicked() 并在 checkable 时翻转
 *          checked；hover/press 反馈留在叶子侧，checked 状态经宿主读回。
 * \endif
 */
class RibbonToolButton : public QQuickItem, public SARibbon::Core::SARibbonAbstractLayoutItem
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(RibbonEnums::RowProportion proportion READ proportion WRITE setProportion NOTIFY proportionChanged)
    Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable NOTIFY checkableChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
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

    bool isCheckable() const;
    void setCheckable(bool on);

    bool isChecked() const;
    void setChecked(bool on);

    // Invokable trigger used by the visual leaf's MouseArea; also usable from
    // user QML/tests to simulate a click
    Q_INVOKABLE void click();

    // handshake property (plan-04 R5): visual leaf assigns itself back
    QQuickItem* buttonQmlItem() const;
    void setButtonQmlItem(QQuickItem* item);

    // Panel context (set by RibbonPanel after each engine pass): the large
    // button sizeHint width depends on the current large row height exactly
    // like SARibbonToolButton on the widgets side
    void setLargeButtonHeightContext(int h);

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
    void checkableChanged();
    void checkedChanged();
    void buttonQmlItemChanged();
    void clicked();
    void toggled(bool checked);

protected:
    void componentComplete() override;  // register into the parent panel host

private:
    void updateSizeHint();
    void ensureQmlItem();
    QSize computeSizeHintFromMetrics();

    QString mText;
    QString mIconSource;
    bool mCheckable = false;
    bool mChecked = false;
    QQuickItem* mButtonQmlItem = nullptr;
    QSize mCachedSizeHint;
    int mLargeButtonHeightContext = 0;
};

}

#endif  // RIBBONTOOLBUTTON_H
