#ifndef RIBBONCONTROLCONTAINER_H
#define RIBBONCONTROLCONTAINER_H
#include "SARibbonQmlGlobal.h"
#include "SARibbonQmlLayoutItemHost.h"
#include "SARibbonQmlTypes.h"
#include <QRectF>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Control container host: embeds an arbitrary QQuickItem into a panel
 * @details The QML counterpart of the widgets SARibbonCtrlContainer
 *          (icon|text|widget layout, extended with the trailing label of
 *          SARibbonLineWidgetContainer): a label strip (icon + text, width
 *          computed in C++ from core metrics) reserves the leading edge, the
 *          `control` item fills the middle and an optional `suffixText` strip
 *          reserves the trailing edge. `enableShowIcon`/`enableShowTitle`
 *          collapse the leading strip parts exactly like the widgets
 *          setVisible on the icon/text labels, so the control reclaims the
 *          freed width. The control is an arbitrary user QQuickItem
 *          (ComboBox, CheckBox, SpinBox, TextField, ...) — exactly what the
 *          widgets example embeds via addSmallWidget. The sizeHint combines
 *          both label widths with the control's implicit size; implicit size
 *          changes of the control invalidate the panel layout, mirroring
 *          QWidgetItem::sizeHint propagation.
 * \endif
 *
 * \if CHINESE
 * @brief 控件容器宿主：把任意 QQuickItem 嵌入面板
 * @details 对应 widgets 侧 SARibbonCtrlContainer（icon|text|widget 布局），并
 *          合并 SARibbonLineWidgetContainer 的尾随标签：前端是标签条（icon +
 *          text，宽度在 C++ 侧由 core 度量推导），中间填充 `control` 项，尾部
 *          可选 `suffixText` 再占一条。`enableShowIcon`/`enableShowTitle` 与
 *          widgets 对 icon/text 两个 QLabel 调 setVisible 完全等价——收起后
 *          control 拿回腾出的宽度。control 是任意用户 QQuickItem（ComboBox、
 *          CheckBox、SpinBox、TextField……）——正是 widgets 示例经
 *          addSmallWidget 嵌入的那些控件。sizeHint 由两条标签宽度与 control 的
 *          implicit 尺寸合成；control 的 implicit 尺寸变化会触发面板重排，
 *          对应 QWidgetItem::sizeHint 的传导。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonControlContainer : public RibbonLayoutItemHost
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString suffixText READ suffixText WRITE setSuffixText NOTIFY suffixTextChanged)
    Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(QQuickItem* control READ control WRITE setControl NOTIFY controlChanged)
    Q_PROPERTY(RibbonEnums::RowProportion proportion READ proportion WRITE setProportion NOTIFY proportionChanged)
    Q_PROPERTY(bool enableShowIcon READ isEnableShowIcon WRITE setEnableShowIcon NOTIFY enableShowIconChanged)
    Q_PROPERTY(bool enableShowTitle READ isEnableShowTitle WRITE setEnableShowTitle NOTIFY enableShowTitleChanged)
    Q_PROPERTY(qreal labelWidth READ labelWidth NOTIFY labelWidthChanged)
    Q_PROPERTY(qreal suffixWidth READ suffixWidth NOTIFY suffixWidthChanged)
public:
    explicit RibbonControlContainer(QQuickItem* parent = nullptr);
    ~RibbonControlContainer() override;

    QString text() const;
    void setText(const QString& t);

    // Trailing label after the control (widgets SARibbonLineWidgetContainer suffix)
    QString suffixText() const;
    void setSuffixText(const QString& t);

    QString iconSource() const;
    void setIconSource(const QString& s);

    // The embedded control item; assigning reparents it into this container
    QQuickItem* control() const;
    void setControl(QQuickItem* item);

    RibbonEnums::RowProportion proportion() const;
    void setProportion(RibbonEnums::RowProportion rp);

    bool isEnableShowIcon() const;
    void setEnableShowIcon(bool on);

    bool isEnableShowTitle() const;
    void setEnableShowTitle(bool on);

    // Leading label strip width (icon + text + spacing); the leaf renders the
    // label inside it and the control is placed after it
    qreal labelWidth() const;

    // Trailing strip width reserved for suffixText; 0 when there is no suffix
    qreal suffixWidth() const;

    // ---- contract implementation (engine inputs/outputs) ----
    QSize sizeHint() const override;

Q_SIGNALS:
    void textChanged();
    void suffixTextChanged();
    void iconSourceChanged();
    void controlChanged();
    void proportionChanged();
    void enableShowIconChanged();
    void enableShowTitleChanged();
    void labelWidthChanged();
    void suffixWidthChanged();

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void largeHeightContextChanged() override;
    void applyGeometry(const QRect& rect) override;

private:
    void updateSizeHint();
    void updateLabelWidth();
    void updateSuffixWidth();
    void positionControl();
    int computeLabelWidthFromMetrics() const;
    int computeSuffixWidthFromMetrics() const;

    QString mText;
    QString mSuffixText;
    QString mIconSource;
    QQuickItem* mControl = nullptr;
    RibbonEnums::RowProportion mProportion = RibbonEnums::Small;
    QSize mCachedSizeHint;
    int mLabelWidth = 0;
    int mSuffixWidth = 0;
    bool mEnableShowIcon = true;
    bool mEnableShowTitle = true;
};

}

#endif  // RIBBONCONTROLCONTAINER_H
