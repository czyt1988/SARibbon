#ifndef SARIBBONQMLTYPES_H
#define SARIBBONQMLTYPES_H
#include "SARibbonQmlGlobal.h"
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
};

// compile-time value checks against the core originals (plan-04 S4 note)
static_assert(int(RibbonEnums::Large) == int(SARibbon::Core::SARibbonRowProportion::Large), "RowProportion drift");
static_assert(int(RibbonEnums::Medium) == int(SARibbon::Core::SARibbonRowProportion::Medium), "RowProportion drift");
static_assert(int(RibbonEnums::Small) == int(SARibbon::Core::SARibbonRowProportion::Small), "RowProportion drift");

// Leaf resource URL table (centralized, no factory class in P0, plan-04 S3)
namespace SARibbonQmlLeafUrls {
inline QUrl barLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonBar.qml")); }
inline QUrl categoryLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonCategory.qml")); }
inline QUrl tabLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonTab.qml")); }
inline QUrl panelLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonPanel.qml")); }
inline QUrl toolButtonLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonToolButton.qml")); }
inline QUrl controlContainerLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonControlContainer.qml")); }
inline QUrl galleryLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonGallery.qml")); }
inline QUrl separatorLeaf() { return QUrl(QStringLiteral("qrc:/SARibbon/RibbonSeparator.qml")); }
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
