#ifndef SARIBBONQMLTYPES_H
#define SARIBBONQMLTYPES_H
#include "SARibbonQmlGlobal.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <SARibbonCore/SARibbonMetrics.h>
#include <SARibbonCore/SARibbonThemeData.h>
#include <QObject>
#include <QFont>
#include <QColor>
#include <QMargins>
#include <QUrl>

class QQuickItem;

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Uncreatable enum holder for user QML (`Ribbon.Large` etc., plan-04 S3-4)
 * @details Mirrors the core enums via Q_ENUM on a registered-but-uncreatable class.
 *          Q_NAMESPACE / qmlRegisterUncreatableMetaObject is closed for 3.0:
 *          the core enums live in the global namespace (plan-02 S1 decision).
 * \endif
 *
 * \if CHINESE
 * @brief 用户 QML 的枚举持有类（`Ribbon.Large` 等写法，计划 04 S3-4）
 * @details 通过已注册但不可实例化类的 Q_ENUM 镜像 core 枚举。
 *          Q_NAMESPACE 路线 3.0 封死：core 枚举维持全局命名空间（计划 02 S1 决策）。
 * \endif
 */
class RibbonEnums : public QObject
{
    Q_OBJECT
public:
    explicit RibbonEnums(QObject* parent = nullptr) : QObject(parent) {}
    // mirrored enums; values must stay identical to the core originals
    enum RowProportion { None = 0, Large = 1, Medium = 2, Small = 3 };
    Q_ENUM(RowProportion)
    enum LayoutMode { ThreeRowMode = 3, TwoRowMode = 2, SingleRowMode = 1 };
    Q_ENUM(LayoutMode)
    enum Alignment { AlignLeft = 0, AlignCenter = 1, AlignRight = 2 };
    Q_ENUM(Alignment)
    enum Theme {
        RibbonThemeWindows7 = 0,
        RibbonThemeOffice2013,
        RibbonThemeOffice2016Blue,
        RibbonThemeOffice2016Green,
        RibbonThemeOffice2016Dark,
        RibbonThemeOffice2021Blue,
        RibbonThemeOffice2021Green,
        RibbonThemeOffice2021Dark,
        RibbonThemeDark,
        RibbonThemeDark2,
        RibbonThemeUserDefine = 1000
    };
    Q_ENUM(Theme)
    // Tool button popup modes (values mirror QToolButton::ToolButtonPopupMode;
    // the QML counterpart of the widgets SARibbonToolButton popup modes)
    enum PopupMode { DelayedPopup = 0, MenuButtonPopup = 1, InstantPopup = 2 };
    Q_ENUM(PopupMode)
    // Ribbon styles (bit values mirror the widgets SARibbonBar::RibbonStyleFlag)
    enum RibbonStyle {
        RibbonStyleLoose           = 0x0001,
        RibbonStyleCompact         = 0x0002,
        RibbonStyleThreeRow        = 0x0010,
        RibbonStyleTwoRow          = 0x0020,
        RibbonStyleSingleRow       = 0x0040,
        RibbonStyleLooseThreeRow   = 0x0011,
        RibbonStyleCompactThreeRow = 0x0012,
        RibbonStyleLooseTwoRow     = 0x0021,
        RibbonStyleCompactTwoRow   = 0x0022,
        RibbonStyleLooseSingleRow   = 0x0041,
        RibbonStyleCompactSingleRow = 0x0042
    };
    Q_ENUM(RibbonStyle)
    // Gallery cell caption styles. NAMES follow the widgets
    // SARibbonGalleryGroup::GalleryGroupStyle (the vocabulary a ribbon user
    // already knows) while VALUES follow the core SA::GalleryCaptionStyle the
    // metrics helper switches on; the widgets declaration order differs from
    // the core one, so the two cannot both be mirrored value-for-value. The
    // static_asserts below pin the value mapping instead.
    enum GalleryCaptionStyle { GalleryIconOnly = 0, GalleryIconWithText = 1, GalleryIconWithWordWrapText = 2 };
    Q_ENUM(GalleryCaptionStyle)
    // Color button rendering styles. Names and order mirror the widgets
    // SARibbonColorToolButton::ColorStyle. No static_assert pins them: the
    // originals live in a widgets header, which SARibbonQml must not include
    enum ColorStyle { ColorUnderIcon = 0, ColorFillToIcon = 1 };
    Q_ENUM(ColorStyle)
    // Whether a color button builds its own color menu. Mirrors the widgets
    // SAColorToolButton::ColorToolButtonStyle declaration order
    enum ColorMenuStyle { WithColorMenu = 0, NoColorMenu = 1 };
    Q_ENUM(ColorMenuStyle)
    // Customize tree row kinds. Values mirror the widgets
    // SARibbonCustomizeWidget LevelRole numbers so a record produced by either
    // front end describes the same row shape; the widgets side keeps them as
    // bare integers in its item model, hence no static_assert
    enum CustomizeNodeType {
        CategoryNode          = 0,
        PanelNode             = 1,
        ActionNode            = 2,
        QuickAccessNode       = 3,
        QuickAccessActionNode = 4
    };
    Q_ENUM(CustomizeNodeType)
    // Customize tree scope. Mirrors the widgets
    // SARibbonCustomizeWidget::RibbonTreeShowType declaration order
    enum CustomizeTreeShowType { ShowAllCategory = 0, ShowMainCategory = 1, ShowQuickAccessBar = 2 };
    Q_ENUM(CustomizeTreeShowType)
    // Command catalogue tags, mirrored value-for-value from the core
    // SARibbonActionTag so both front ends speak the same tag language
    enum ActionTag {
        UnknowActionTag                 = 0,
        CommonlyUsedActionTag           = 0x01,
        NotInFunctionalAreaActionTag    = 0x02,
        AutoCategoryDistinguishBeginTag = 0x1000,
        AutoCategoryDistinguishEndTag   = 0x2000,
        NotInRibbonCategoryTag          = 0x2001,
        UserDefineActionTag             = 0x8000
    };
    Q_ENUM(ActionTag)
};

// compile-time value checks against the core originals (plan-04 S4 note)
static_assert(int(RibbonEnums::Large) == int(SARibbon::Core::SARibbonRowProportion::Large), "RowProportion drift");
static_assert(int(RibbonEnums::Medium) == int(SARibbon::Core::SARibbonRowProportion::Medium), "RowProportion drift");
static_assert(int(RibbonEnums::Small) == int(SARibbon::Core::SARibbonRowProportion::Small), "RowProportion drift");
static_assert(int(RibbonEnums::GalleryIconOnly) == int(SA::GalleryCaptionStyle::None), "GalleryCaptionStyle drift");
static_assert(int(RibbonEnums::GalleryIconWithText) == int(SA::GalleryCaptionStyle::SingleLine), "GalleryCaptionStyle drift");
static_assert(int(RibbonEnums::GalleryIconWithWordWrapText) == int(SA::GalleryCaptionStyle::WordWrap), "GalleryCaptionStyle drift");
static_assert(int(RibbonEnums::UnknowActionTag) == int(SARibbon::Core::UnknowActionTag), "ActionTag drift");
static_assert(int(RibbonEnums::CommonlyUsedActionTag) == int(SARibbon::Core::CommonlyUsedActionTag), "ActionTag drift");
static_assert(int(RibbonEnums::NotInFunctionalAreaActionTag) == int(SARibbon::Core::NotInFunctionalAreaActionTag),
              "ActionTag drift");
static_assert(int(RibbonEnums::AutoCategoryDistinguishBeginTag) == int(SARibbon::Core::AutoCategoryDistinguishBeginTag),
              "ActionTag drift");
static_assert(int(RibbonEnums::AutoCategoryDistinguishEndTag) == int(SARibbon::Core::AutoCategoryDistinguishEndTag),
              "ActionTag drift");
static_assert(int(RibbonEnums::NotInRibbonCategoryTag) == int(SARibbon::Core::NotInRibbonCategoryTag), "ActionTag drift");
static_assert(int(RibbonEnums::UserDefineActionTag) == int(SARibbon::Core::UserDefineActionTag), "ActionTag drift");

// Leaf resource URL table (centralized, no factory class in P0, plan-04 S3)
namespace SARibbonQmlLeafUrls {
inline QUrl barLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonBar.qml")); }
inline QUrl categoryLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonCategory.qml")); }
// Overlay leaf (scroll arrows) of RibbonCategory: created in addition to the
// background leaf and raised above the panels, see RibbonCategory::ensureScrollOverlay
inline QUrl categoryScrollLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonCategoryScroll.qml")); }
inline QUrl tabLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonTab.qml")); }
inline QUrl panelLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonPanel.qml")); }
inline QUrl toolButtonLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonToolButton.qml")); }
inline QUrl controlContainerLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonControlContainer.qml")); }
inline QUrl galleryLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonGallery.qml")); }
inline QUrl separatorLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonSeparator.qml")); }
inline QUrl colorGridLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonColorGrid.qml")); }
inline QUrl colorMenuLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonColorMenu.qml")); }
inline QUrl colorToolButtonLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonColorToolButton.qml")); }
// Application-facing customize picker (not a host leaf): an app instantiates it
// by URL, the same way RibbonMenu nests itself
inline QUrl customizeDialogLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonCustomizeDialog.qml")); }
// Frameless system button row rendered inside the bar leaf (not a host leaf
// either): loaded by RibbonBar.qml when the frameless agent is active
inline QUrl windowButtonRowLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonWindowButtonRow.qml")); }
}

// Uniform handshake contract: every leaf root declares `property QtObject
// cppHost`; this function injects the host into that property and the leaf
// assigns itself back into the host's inherited `qmlLeaf` property.
// Create the visual leaf of a host from qrc: QQmlComponent create -> handshake
// injection -> reparent onto the host (plan-04 S3 creation trilogy);
// returns nullptr with a warning on engine/resource/creation failure
SA_RIBBON_QML_EXPORT QQuickItem* createVisualLeaf(QQuickItem* host, const QUrl& leafUrl);

}  // namespace SARibbonQml

#endif  // SARIBBONQMLTYPES_H
