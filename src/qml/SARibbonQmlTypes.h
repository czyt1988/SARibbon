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
}

// Create the visual leaf of a host from qrc: QQmlComponent create -> handshake
// property injection -> reparent onto the host (plan-04 S3 creation trilogy);
// returns nullptr with a warning on engine/resource/creation failure
SA_RIBBON_QML_EXPORT QQuickItem* createVisualLeaf(QQuickItem* host, const QUrl& leafUrl, const char* handshakeProperty);

}  // namespace SARibbonQml

#endif  // SARIBBONQMLTYPES_H
