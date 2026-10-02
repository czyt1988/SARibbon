#ifndef RIBBONTOOLBUTTON_H
#define RIBBONTOOLBUTTON_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonLayoutItemHost.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonEnums.h>
#include <SARibbonCore/SARibbonToolButtonLayout.h>
#include <QQuickItem>
#include <QQmlListProperty>
#include <QRectF>
#include <QVector>

namespace SARibbonQml {

class RibbonMenuItem;

/**
 * \if ENGLISH
 * @brief Tool button structural host: contract item + QQuickItem
 * @details The contract half supplies engine inputs (sizeHint computed in C++
 *          from core metrics — never from QML implicit sizes, the iron rule);
 *          the QQuickItem half receives engine geometry via applyGeometry.
 *          Interaction mirrors SARibbonToolButton on the widgets side: the
 *          visual leaf's MouseAreas call the click() invokable, which emits
 *          clicked() and toggles checked when checkable; a disabled host
 *          swallows clicks entirely. Popup modes follow
 *          QToolButton::ToolButtonPopupMode semantics: MenuButtonPopup splits
 *          the button into an action zone and a menu zone (published as
 *          actionRect/menuRect — geometry authority stays here), InstantPopup
 *          turns the whole button into the menu zone, DelayedPopup keeps the
 *          whole button as the action zone and opens the menu on press-hold
 *          (leaf-side timer). Menu entries are declarative RibbonMenuItem
 *          objects; activation is mediated by menuTriggered so tests can drive
 *          it without a windowed popup.
 *          The layout knobs are the core SARibbonToolButtonLayout::Factors and
 *          Input fields published as properties (spacing, the two text height
 *          factors, the aspect ratio pair, the two icon sizes), mirroring the
 *          public setters of SARibbonToolButton. The two aspect ratios also
 *          arrive through the bar propagation chain (widgets
 *          SARibbonBar::setButtonMaximumAspectRatio parity): a bar-level change
 *          overwrites the per-button value, exactly as on the widgets side.
 * \endif
 *
 * \if CHINESE
 * @brief 工具按钮结构宿主：契约项 + QQuickItem
 * @details 契约侧提供引擎输入（sizeHint 在 C++ 侧由 core 度量推导——绝不用 QML
 *          implicit 尺寸，铁律）；QQuickItem 侧经 applyGeometry 接收引擎几何。
 *          交互对照 widgets 侧 SARibbonToolButton：视觉叶子的 MouseArea 调用
 *          click() 可调用方法，由宿主发射 clicked() 并在 checkable 时翻转
 *          checked；禁用态的宿主完全吞掉点击。弹出模式遵循
 *          QToolButton::ToolButtonPopupMode 语义：MenuButtonPopup 把按钮分为
 *          动作区与菜单区（发布为 actionRect/menuRect——几何权威留在宿主），
 *          InstantPopup 整个按钮即菜单区，DelayedPopup 整个按钮保持动作区、
 *          按住不放弹出菜单（叶子侧计时）。菜单项为声明式 RibbonMenuItem；
 *          激活经 menuTriggered 中转，测试无需弹窗即可驱动。
 *          布局旋钮即 core SARibbonToolButtonLayout 的 Factors 与 Input 字段
 *          （spacing、两个文字高度系数、宽高比一对、两个图标尺寸），以属性形式
 *          发布，对应 SARibbonToolButton 的同名公开设置函数。两个宽高比还会经
 *          bar 传播链下发（对应 widgets SARibbonBar::setButtonMaximumAspectRatio）：
 *          bar 级改动会覆盖单按钮的值，与 widgets 侧行为一致。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonToolButton : public RibbonLayoutItemHost
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(RibbonEnums::RowProportion proportion READ proportion WRITE setProportion NOTIFY proportionChanged)
    Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable NOTIFY checkableChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(bool wordWrap READ isWordWrap WRITE setWordWrap NOTIFY wordWrapChanged)
    Q_PROPERTY(bool iconRightText READ isIconRightText WRITE setIconRightText NOTIFY iconRightTextChanged)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(qreal twoLineHeightFactor READ twoLineHeightFactor WRITE setTwoLineHeightFactor NOTIFY layoutFactorsChanged)
    Q_PROPERTY(qreal oneLineHeightFactor READ oneLineHeightFactor WRITE setOneLineHeightFactor NOTIFY layoutFactorsChanged)
    Q_PROPERTY(qreal buttonMaximumAspectRatio READ buttonMaximumAspectRatio WRITE setButtonMaximumAspectRatio NOTIFY layoutFactorsChanged)
    Q_PROPERTY(qreal largeButtonMinimumWidthRatio READ largeButtonMinimumWidthRatio WRITE setLargeButtonMinimumWidthRatio NOTIFY layoutFactorsChanged)
    Q_PROPERTY(QSize smallIconSize READ smallIconSize WRITE setSmallIconSize NOTIFY iconSizesChanged)
    Q_PROPERTY(QSize largeIconSize READ largeIconSize WRITE setLargeIconSize NOTIFY iconSizesChanged)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip NOTIFY toolTipChanged)
    Q_PROPERTY(RibbonEnums::PopupMode popupMode READ popupMode WRITE setPopupMode NOTIFY popupModeChanged)
    Q_PROPERTY(QQmlListProperty< SARibbonQml::RibbonMenuItem > menuItems READ menuItems NOTIFY menuItemsChanged)
    Q_PROPERTY(bool menuVisible READ isMenuVisible WRITE setMenuVisible NOTIFY menuVisibleChanged)
    Q_PROPERTY(bool hasMenu READ hasMenu NOTIFY menuItemsChanged)
    Q_PROPERTY(QRectF actionRect READ actionRect NOTIFY hitRectsChanged)
    Q_PROPERTY(QRectF menuRect READ menuRect NOTIFY hitRectsChanged)
    Q_PROPERTY(bool largeType READ isLargeType NOTIFY layoutChanged)
    Q_PROPERTY(QString displayText READ displayText NOTIFY layoutChanged)
    Q_PROPERTY(QRectF iconGeometry READ iconGeometry NOTIFY layoutChanged)
    Q_PROPERTY(QRectF textGeometry READ textGeometry NOTIFY layoutChanged)
    Q_PROPERTY(QRectF indicatorGeometry READ indicatorGeometry NOTIFY layoutChanged)
    Q_PROPERTY(bool textWordWrap READ isTextWordWrap NOTIFY layoutChanged)
    Q_PROPERTY(int iconSide READ iconSide NOTIFY layoutChanged)
public:
    explicit RibbonToolButton(QQuickItem* parent = nullptr);
    ~RibbonToolButton() override;

    QString text() const;
    void setText(const QString& t);

    QString iconSource() const;
    void setIconSource(const QString& s);

    // QML-facing enum view of the contract field rowProportion
    RibbonEnums::RowProportion proportion() const;
    void setProportion(RibbonEnums::RowProportion rp);

    bool isCheckable() const;
    void setCheckable(bool on);

    bool isChecked() const;
    void setChecked(bool on);

    // Large-button text wrapping (bar ribbonStyle propagation; widgets
    // setEnableWordWrap parity — false forces single-line elided captions)
    bool isWordWrap() const;
    void setWordWrap(bool on);

    // Icon-left/text-right rendering regardless of proportion (widgets
    // setEnableIconRightText parity — single-row styles enable it)
    bool isIconRightText() const;
    void setIconRightText(bool on);

    // Gap between the drawn elements (widgets setSpacing parity)
    int spacing() const;
    void setSpacing(int v);

    // Core SARibbonToolButtonLayout::Factors, exposed one by one; the defaults
    // equal the ToolButtonLayoutConstants the host used to hardcode
    qreal twoLineHeightFactor() const;
    void setTwoLineHeightFactor(qreal v);
    qreal oneLineHeightFactor() const;
    void setOneLineHeightFactor(qreal v);
    qreal buttonMaximumAspectRatio() const;
    void setButtonMaximumAspectRatio(qreal v);
    qreal largeButtonMinimumWidthRatio() const;
    void setLargeButtonMinimumWidthRatio(qreal v);

    // Icon boxes fed to the core algorithm and published to the leaf through
    // iconSide (widgets setSmallIconSize/setLargeIconSize parity); the panel
    // host pushes its own values down, the bar-level setters reach them through
    // the propagation chain
    QSize smallIconSize() const;
    void setSmallIconSize(const QSize& size);
    QSize largeIconSize() const;
    void setLargeIconSize(const QSize& size);

    QString toolTip() const;
    void setToolTip(const QString& t);

    RibbonEnums::PopupMode popupMode() const;
    void setPopupMode(RibbonEnums::PopupMode mode);

    // Declarative menu entries (rendered by the leaf, mediated by menuTriggered)
    QQmlListProperty< SARibbonQml::RibbonMenuItem > menuItems();
    int menuItemCount() const;
    RibbonMenuItem* menuItemAt(int index) const;

    bool isMenuVisible() const;
    void setMenuVisible(bool on);
    // Whether this button carries a popup at all. Virtual so a subclass owning a
    // different popup (RibbonColorToolButton and its color menu) can take over:
    // the indicator arrow, the hit-zone split and the popup modes all key off it
    virtual bool hasMenu() const;

    // Hit zones (host-computed; the leaf binds its MouseAreas to them and the
    // tests assert them). actionRect is empty for InstantPopup, menuRect is
    // empty without a menu or for DelayedPopup/normal buttons
    QRectF actionRect() const;
    QRectF menuRect() const;

    // ---- core layout publication (SARibbonToolButtonLayout parity) ----
    /// Effective large-button type (proportion Large and not iconRightText)
    bool isLargeType() const;
    /// Caption to render: elided for small/single-line large buttons, raw for wrapped large
    QString displayText() const;
    QRectF iconGeometry() const;
    QRectF textGeometry() const;
    QRectF indicatorGeometry() const;
    /// True when the caption is laid out as a two-line top-aligned box
    bool isTextWordWrap() const;
    /// Natural icon side length the leaf renders (widgets realIconSize parity)
    int iconSide() const;

    // Invokable trigger used by the visual leaf's MouseArea; also usable from
    // user QML/tests to simulate a click. A disabled host swallows the click
    Q_INVOKABLE void click();

    // Popup control: the leaf layer renders the popup; the host tracks the
    // visibility flag so tests can assert the open/close transitions. Virtual
    // for the same reason hasMenu() is: a subclass with its own popup takes over
    Q_INVOKABLE virtual void openMenu();
    Q_INVOKABLE virtual void closeMenu();

    // Activate a menu entry by index (leaf MenuItem triggers + tests); emits
    // menuTriggered; disabled/separators are ignored
    Q_INVOKABLE void activateMenuItem(int index);

    // Activate a nested entry by index path (the leaf publishes one path per
    // activation, [row] for top level, [row, subrow, ...] below it); the same
    // refusals as activateMenuItem apply at the addressed entry
    Q_INVOKABLE void activateMenuItemPath(const QVariantList& indexPath);

    // ---- contract implementation (engine inputs/outputs) ----
    QSize sizeHint() const override;  // C++: core metrics derivation

Q_SIGNALS:
    void textChanged();
    void iconSourceChanged();
    void proportionChanged();
    void checkableChanged();
    void checkedChanged();
    void wordWrapChanged();
    void iconRightTextChanged();
    void spacingChanged();
    void layoutFactorsChanged();
    void iconSizesChanged();
    void toolTipChanged();
    void popupModeChanged();
    void menuItemsChanged();
    void menuVisibleChanged();
    void hitRectsChanged();
    void layoutChanged();
    void clicked();
    void toggled(bool checked);
    void menuTriggered(SARibbonQml::RibbonMenuItem* item);

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void largeHeightContextChanged() override;
    void updatePolish() override;  // RTL flip re-mirrors the hit rects
    // Recompute and publish the draw/hit geometry. Virtual so a subclass can run
    // its own pass on top (every recompute entry point funnels through here)
    virtual void updateLayout();
    // The core algorithm's input. Virtual so a subclass can retarget a field: a
    // color button always paints something into the icon slot, hence hasIcon
    virtual SARibbon::Core::SARibbonToolButtonLayout::Input layoutInput() const;
    // Recompute the cached sizeHint and re-run the panel layout. Protected so a
    // subclass can retrigger it after changing what layoutInput() returns (the
    // base constructor's own call still resolves to the base override)
    void updateSizeHint();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#else
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#endif

private:
    // QQmlListProperty callback types differ between Qt5 (int) and Qt6 (qsizetype)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using ListIndex = qsizetype;
#else
    using ListIndex = int;
#endif
    static void appendMenuItem(QQmlListProperty< SARibbonQml::RibbonMenuItem >* prop, SARibbonQml::RibbonMenuItem* item);
    static ListIndex menuItemCountCb(QQmlListProperty< SARibbonQml::RibbonMenuItem >* prop);
    static SARibbonQml::RibbonMenuItem* menuItemAtCb(QQmlListProperty< SARibbonQml::RibbonMenuItem >* prop, ListIndex index);
    static void clearMenuItems(QQmlListProperty< SARibbonQml::RibbonMenuItem >* prop);

    QSize computeSizeHintFromMetrics();
    void emitMenuItemsChanged();

    QString mText;
    QString mIconSource;
    bool mCheckable = false;
    bool mChecked   = false;
    bool mWordWrap  = true;
    bool mIconRightText = false;
    QString mToolTip;
    RibbonEnums::PopupMode mPopupMode = RibbonEnums::DelayedPopup;
    bool mMenuVisible = false;
    QSize mCachedSizeHint;
    QRectF mActionRect;
    QRectF mMenuRect;
    QRectF mIconGeometry;
    QRectF mTextGeometry;
    QRectF mIndicatorGeometry;
    QString mDisplayText;
    bool mIsTextNeedWrap = false;
    QVector< RibbonMenuItem* > mMenuItems;
    int mSpacing = SARibbon::Core::ToolButtonLayoutConstants::DEFAULT_SPACING;
    SARibbon::Core::SARibbonToolButtonLayout::Factors mFactors;
    QSize mSmallIconSize = QSize(22, 22);  ///< widgets SARibbonPanelLayout::mSmallToolButtonIconSize default
    QSize mLargeIconSize = QSize(32, 32);  ///< widgets SARibbonToolButton::PrivateData::mLargeButtonSizeHint default
};

}

#endif  // RIBBONTOOLBUTTON_H
