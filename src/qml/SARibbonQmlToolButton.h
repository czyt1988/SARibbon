#ifndef RIBBONTOOLBUTTON_H
#define RIBBONTOOLBUTTON_H
#include "SARibbonQmlGlobal.h"
#include "SARibbonQmlLayoutItemHost.h"
#include "SARibbonQmlTypes.h"
// the single QAction include face of the module (contract D5, plan-05 S1);
// full type: the action Q_PROPERTY / menuTriggered(QAction*) signal need it
// complete here (moc pointer metatype, the B64 family). RibbonAction too:
// the menuActions QQmlListProperty template parameter
#include "SARibbonQmlActionCompat.h"
#include "SARibbonQmlAction.h"
#include <SARibbonCore/SARibbonEnums.h>
#include <SARibbonCore/SARibbonToolButtonLayout.h>
#include <QPointer>
#include <QQuickItem>
#include <QQmlListProperty>
#include <QRectF>
#include <QVector>

namespace SARibbonQml {

class RibbonMenuModel;
class RibbonBar;

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
 *          (leaf-side timer). Menu entries are QAction objects; activation is
 *          mediated by menuTriggered so tests can drive
 *          it without a windowed popup.
 *          The layout knobs are the core SARibbonToolButtonLayout::Factors and
 *          Input fields published as properties (spacing, the two text height
 *          factors, the aspect ratio pair, the two icon sizes), mirroring the
 *          public setters of SARibbonToolButton. The two aspect ratios also
 *          arrive through the bar propagation chain (widgets
 *          SARibbonBar::setButtonMaximumAspectRatio parity): a bar-level change
 *          overwrites the per-button value, exactly as on the widgets side.
 *          The icon/text display follows QToolButton::toolButtonStyle through
 *          the toolButtonStyle property (explicit wins; an unset button keeps
 *          TextBesideIcon in panels and resolves IconOnly inside the title-row
 *          containers, which also force the small rendering — proportion is
 *          meaningless there, matching the widgets QToolBar buttons).
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
 *          按住不放弹出菜单（叶子侧计时）。菜单项为 QAction 列表；激活经
 *          menuTriggered 中转，测试无需弹窗即可驱动。
 *          布局旋钮即 core SARibbonToolButtonLayout 的 Factors 与 Input 字段
 *          （spacing、两个文字高度系数、宽高比一对、两个图标尺寸），以属性形式
 *          发布，对应 SARibbonToolButton 的同名公开设置函数。两个宽高比还会经
 *          bar 传播链下发（对应 widgets SARibbonBar::setButtonMaximumAspectRatio）：
 *          bar 级改动会覆盖单按钮的值，与 widgets 侧行为一致。
 *          图标/文字的显示方式经 toolButtonStyle 属性对照
 *          QToolButton::toolButtonStyle（显式设置优先；未设置的面板按钮保持
 *          TextBesideIcon，标题行容器内的按钮解析为 IconOnly，且一律按小按钮
 *          渲染——proportion 在其中无意义，与 widgets 侧 QToolBar 按钮一致）。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonToolButton : public RibbonLayoutItemHost
{
    Q_OBJECT
    Q_PROPERTY(QAction* action READ action WRITE setAction NOTIFY actionChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(RibbonEnums::RowProportion proportion READ proportion WRITE setProportion NOTIFY proportionChanged)
    Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable NOTIFY checkableChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(bool wordWrap READ isWordWrap WRITE setWordWrap NOTIFY wordWrapChanged)
    Q_PROPERTY(bool iconRightText READ isIconRightText WRITE setIconRightText NOTIFY iconRightTextChanged)
    Q_PROPERTY(bool flat READ isFlat WRITE setFlat NOTIFY flatChanged)
    Q_PROPERTY(RibbonEnums::ToolButtonStyle toolButtonStyle READ toolButtonStyle WRITE setToolButtonStyle NOTIFY toolButtonStyleChanged)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(qreal twoLineHeightFactor READ twoLineHeightFactor WRITE setTwoLineHeightFactor NOTIFY layoutFactorsChanged)
    Q_PROPERTY(qreal oneLineHeightFactor READ oneLineHeightFactor WRITE setOneLineHeightFactor NOTIFY layoutFactorsChanged)
    Q_PROPERTY(qreal buttonMaximumAspectRatio READ buttonMaximumAspectRatio WRITE setButtonMaximumAspectRatio NOTIFY layoutFactorsChanged)
    Q_PROPERTY(qreal largeButtonMinimumWidthRatio READ largeButtonMinimumWidthRatio WRITE setLargeButtonMinimumWidthRatio NOTIFY layoutFactorsChanged)
    Q_PROPERTY(QSize smallIconSize READ smallIconSize WRITE setSmallIconSize NOTIFY iconSizesChanged)
    Q_PROPERTY(QSize largeIconSize READ largeIconSize WRITE setLargeIconSize NOTIFY iconSizesChanged)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip NOTIFY toolTipChanged)
    Q_PROPERTY(RibbonEnums::PopupMode popupMode READ popupMode WRITE setPopupMode NOTIFY popupModeChanged)
    Q_PROPERTY(QQmlListProperty< SARibbonQml::RibbonAction > menuActions READ menuActions NOTIFY menuActionsChanged)
    Q_PROPERTY(QVariantList menuModel READ menuModel NOTIFY menuModelChanged)
    Q_PROPERTY(bool menuVisible READ isMenuVisible WRITE setMenuVisible NOTIFY menuVisibleChanged)
    Q_PROPERTY(bool hasMenu READ hasMenu NOTIFY menuModelChanged)
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

    // Bound command (contract D1/D6). Non-null derivation: text/iconSource/
    // toolTip/checked/enabled/checkable mirror the action; a local write is
    // written through to it (the action stays the single authority). A null
    // action restores the plain-declarative storage (addWidget semantics)
    QAction* action() const;
    void setAction(QAction* act);

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

    // Transparent normal-state background (widgets theme-base QSS rule
    // `SARibbonButtonGroupWidget > QToolButton`: border none, background
    // transparent — only hover/pressed/checked paint). The quick access bar
    // and the right button group flip it on so their buttons sit flat on the
    // title row instead of carrying the content background box
    bool isFlat() const;
    void setFlat(bool on);

    // Explicit icon/text display style (QToolButton::toolButtonStyle parity).
    // Writing it through QML marks it explicit and it always wins; an unset
    // button resolves through the rendering context: the panel default keeps
    // TextBesideIcon, a title-row container (quick access bar / right group)
    // defaults to IconOnly like the widgets QToolBar buttons
    RibbonEnums::ToolButtonStyle toolButtonStyle() const;
    void setToolButtonStyle(RibbonEnums::ToolButtonStyle style);

    // Rendering context pushed by the title-row containers (quick access bar,
    // right button group): while on, the button renders toolbar-style — the
    // proportion is meaningless there (widgets parity: their quick access
    // buttons are plain QToolButtons created by QToolBar, never
    // SARibbonToolButtons) and the unset style default becomes IconOnly.
    // Cleared again when the button moves back into a panel
    bool isTitleRow() const;
    void setTitleRow(bool on);

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

    // Menu entries as QAction objects (plan-06 S3): authoring surface —
    // object literals, id references and C++ appends all land here. The leaf
    // renders the derived menuModel rows; activation resolves back to the
    // QAction and triggers it (contract §5: submenus via
    // RibbonAction.menuActions, bare QAction entries are flat-only)
    QQmlListProperty< SARibbonQml::RibbonAction > menuActions();
    int menuActionCount() const;
    QAction* menuActionAt(int index) const;
    // C++ append of any QAction (bare ones included)
    void addMenuAction(QAction* action);

    // Derived row maps for the leaf (live: rebuilt on every action change)
    QVariantList menuModel() const;

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
    void actionChanged();
    void textChanged();
    void iconSourceChanged();
    void proportionChanged();
    void checkableChanged();
    void checkedChanged();
    void wordWrapChanged();
    void iconRightTextChanged();
    void flatChanged();
    void toolButtonStyleChanged();
    void spacingChanged();
    void layoutFactorsChanged();
    void iconSizesChanged();
    void toolTipChanged();
    void popupModeChanged();
    void menuActionsChanged();
    void menuModelChanged();
    void menuVisibleChanged();
    void hitRectsChanged();
    void layoutChanged();
    void clicked();
    void toggled(bool checked);
    void menuTriggered(QAction* item);

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void largeHeightContextChanged() override;
    void updatePolish() override;  // RTL flip re-mirrors the hit rects
    // plan-05 S7 keyboard baseline: Space/Enter trigger the click path
    void keyPressEvent(QKeyEvent* event) override;
    // the shortcut collection follows the button across containers; an
    // enabled flip also writes through to the action (plan-05 S4)
    void itemChange(ItemChange change, const ItemChangeData& data) override;
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
        // ---- menu model (plan-06 S3) ----
    RibbonMenuModel* mMenuModel;
    void syncMenuModel();
    QVector< QAction* > mMenuActions;

    // QQmlListProperty callback types differ between Qt5 (int) and Qt6 (qsizetype)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using ListIndex = qsizetype;
#else
    using ListIndex = int;
#endif
    static void appendMenuActionCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop, SARibbonQml::RibbonAction* action);
    static ListIndex menuActionCountCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop);
    static SARibbonQml::RibbonAction* menuActionAtCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop, ListIndex index);
    static void clearMenuActionsCb(QQmlListProperty< SARibbonQml::RibbonAction >* prop);

    // ---- action binding (plan-05 S4) ----
    // QAction::changed is field-less: re-derive every mirrored property and
    // re-emit the notifies whose value actually moved (the plan-05 risk table
    // accepts the coarse granularity; correctness first)
    void onActionChanged();
    // QAction::triggered re-emitted as clicked() — the existing signal face
    // stays intact for leaves and tests
    void onActionTriggered();
    // Resolve the icon url of the bound action: RibbonAction route (direct
    // url) or bare-QAction route (image provider, plan-05 S3)
    QString actionIconSource() const;
    // (re)collect this button's action into the owning bar's shortcut matcher
    // (plan-05 S5); the ancestor walk resolves lazily because buttons attach
    // to their panel before the panel reaches a bar
    RibbonBar* findOwningBar() const;
    void syncShortcutCollection();
    // guard so action-driven updates never write back to the action
    bool mSyncingFromAction = false;
    // QPointer (review P1-2): the bound command may be deleted on the C++
    // side at any moment — every mAction use is a null-checked use
    QPointer< QAction > mAction;
    qint64 mLastIconKey     = 0;  ///< QIcon::cacheKey of the derived icon (change detection)

    QSize computeSizeHintFromMetrics();
    // Resolve the core style from the explicit value / rendering context and
    // apply the icon-less fallback (an IconOnly button without an icon would
    // render blank; text takes over instead)
    Qt::ToolButtonStyle effectiveToolButtonStyle() const;
    void emitMenuItemsChanged();

    QString mText;
    QString mIconSource;
    bool mCheckable = false;
    bool mChecked   = false;
    bool mWordWrap  = true;
    bool mIconRightText = false;
    bool mFlat = false;
    bool mTitleRow = false;  ///< rendering context pushed by the title-row containers
    RibbonEnums::ToolButtonStyle mToolButtonStyle = RibbonEnums::TextBesideIcon;  ///< explicit value (mToolButtonStyleSet gates it)
    bool mToolButtonStyleSet = false;  ///< true once QML/C++ wrote the property
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
    int mSpacing = SARibbon::Core::ToolButtonLayoutConstants::DEFAULT_SPACING;
    SARibbon::Core::SARibbonToolButtonLayout::Factors mFactors;
    QSize mSmallIconSize = QSize(22, 22);  ///< widgets SARibbonPanelLayout::mSmallToolButtonIconSize default
    QSize mLargeIconSize = QSize(32, 32);  ///< widgets SARibbonToolButton::PrivateData::mLargeButtonSizeHint default
};

}

#endif  // RIBBONTOOLBUTTON_H
