#ifndef RIBBONCONTROLCONTAINER_H
#define RIBBONCONTROLCONTAINER_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonLayoutItemHost.h"
#include "../SARibbonQmlTypes.h"
#include <QRectF>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Control container host: embeds an arbitrary QQuickItem into a panel
 * @details The QML counterpart of the widgets SARibbonCtrlContainer
 *          (icon|text|widget layout): a label strip (icon + text, width
 *          computed in C++ from core metrics) reserves the leading edge and
 *          the `control` item fills the rest. The control is an arbitrary
 *          user QQuickItem (ComboBox, CheckBox, SpinBox, TextField, ...) —
 *          exactly what the widgets example embeds via addSmallWidget. The
 *          sizeHint combines the label width with the control's implicit
 *          size; implicit size changes of the control invalidate the panel
 *          layout, mirroring QWidgetItem::sizeHint propagation.
 * \endif
 *
 * \if CHINESE
 * @brief 控件容器宿主：把任意 QQuickItem 嵌入面板
 * @details 对应 widgets 侧 SARibbonCtrlContainer（icon|text|widget 布局）：
 *          前端是标签条（icon + text，宽度在 C++ 侧由 core 度量推导），其余
 *          空间填充 `control` 项。control 是任意用户 QQuickItem（ComboBox、
 *          CheckBox、SpinBox、TextField……）——正是 widgets 示例经
 *          addSmallWidget 嵌入的那些控件。sizeHint 由标签宽度与 control 的
 *          implicit 尺寸合成；control 的 implicit 尺寸变化会触发面板重排，
 *          对应 QWidgetItem::sizeHint 的传导。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonControlContainer : public RibbonLayoutItemHost
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(QQuickItem* control READ control WRITE setControl NOTIFY controlChanged)
    Q_PROPERTY(RibbonEnums::RowProportion proportion READ proportion WRITE setProportion NOTIFY proportionChanged)
    Q_PROPERTY(qreal labelWidth READ labelWidth NOTIFY labelWidthChanged)
public:
    explicit RibbonControlContainer(QQuickItem* parent = nullptr);
    ~RibbonControlContainer() override;

    QString text() const;
    void setText(const QString& t);

    QString iconSource() const;
    void setIconSource(const QString& s);

    // The embedded control item; assigning reparents it into this container
    QQuickItem* control() const;
    void setControl(QQuickItem* item);

    RibbonEnums::RowProportion proportion() const;
    void setProportion(RibbonEnums::RowProportion rp);

    // Leading label strip width (icon + text + spacing); the leaf renders the
    // label inside it and the control is placed after it
    qreal labelWidth() const;

    // ---- contract implementation (engine inputs/outputs) ----
    QSize sizeHint() const override;

Q_SIGNALS:
    void textChanged();
    void iconSourceChanged();
    void controlChanged();
    void proportionChanged();
    void labelWidthChanged();

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void largeHeightContextChanged() override;
    void applyGeometry(const QRect& rect) override;

private:
    void updateSizeHint();
    void updateLabelWidth();
    void positionControl();
    int computeLabelWidthFromMetrics() const;

    QString mText;
    QString mIconSource;
    QQuickItem* mControl = nullptr;
    RibbonEnums::RowProportion mProportion = RibbonEnums::Small;
    QSize mCachedSizeHint;
    int mLabelWidth = 0;
};

}

#endif  // RIBBONCONTROLCONTAINER_H
