#ifndef RIBBONTOOLBUTTON_H
#define RIBBONTOOLBUTTON_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonLayoutItemHost.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonEnums.h>
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
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip NOTIFY toolTipChanged)
    Q_PROPERTY(RibbonEnums::PopupMode popupMode READ popupMode WRITE setPopupMode NOTIFY popupModeChanged)
    Q_PROPERTY(QQmlListProperty< SARibbonQml::RibbonMenuItem > menuItems READ menuItems NOTIFY menuItemsChanged)
    Q_PROPERTY(bool menuVisible READ isMenuVisible WRITE setMenuVisible NOTIFY menuVisibleChanged)
    Q_PROPERTY(bool hasMenu READ hasMenu NOTIFY menuItemsChanged)
    Q_PROPERTY(QRectF actionRect READ actionRect NOTIFY hitRectsChanged)
    Q_PROPERTY(QRectF menuRect READ menuRect NOTIFY hitRectsChanged)
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
    bool hasMenu() const;

    // Hit zones (host-computed; the leaf binds its MouseAreas to them and the
    // tests assert them). actionRect is empty for InstantPopup, menuRect is
    // empty without a menu or for DelayedPopup/normal buttons
    QRectF actionRect() const;
    QRectF menuRect() const;

    // Invokable trigger used by the visual leaf's MouseArea; also usable from
    // user QML/tests to simulate a click. A disabled host swallows the click
    Q_INVOKABLE void click();

    // Popup control: the leaf layer renders the popup; the host tracks the
    // visibility flag so tests can assert the open/close transitions
    Q_INVOKABLE void openMenu();
    Q_INVOKABLE void closeMenu();

    // Activate a menu entry by index (leaf MenuItem triggers + tests); emits
    // menuTriggered; disabled/separators are ignored
    Q_INVOKABLE void activateMenuItem(int index);

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
    void toolTipChanged();
    void popupModeChanged();
    void menuItemsChanged();
    void menuVisibleChanged();
    void hitRectsChanged();
    void clicked();
    void toggled(bool checked);
    void menuTriggered(SARibbonQml::RibbonMenuItem* item);

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void largeHeightContextChanged() override;
    void updatePolish() override;  // RTL flip re-mirrors the hit rects
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

    void updateSizeHint();
    void updateHitRects();
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
    QVector< RibbonMenuItem* > mMenuItems;
};

}

#endif  // RIBBONTOOLBUTTON_H
