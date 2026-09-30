#ifndef RIBBONCONFORMANCE_H
#define RIBBONCONFORMANCE_H

// Cross-frontend conformance scene data (plan-04 S7): pure header, zero deps.
// Both the widgets-side and the qml-side test binaries include this file so the
// two front ends are driven by THE SAME scenario definition.
//
// Test conventions (plan-04 S7):
//  - the QML side uses QQuickView + show() + qWaitForWindowExposed (updatePolish
//    only fires for items attached to a window);
//  - fonts: the conformance scenario uses explicit sizeHint inputs wherever the
//    engine is driven directly; for widget-tree scenarios both ends must set the
//    APPLICATION font (widgets listens to per-widget FontChange, QML metrics to
//    ApplicationFontChange).

#include <QSize>
#include <QVector>

namespace conformance {

struct ButtonSpec {
    enum Proportion { Large, Medium, Small };
    Proportion proportion = Large;
    QString text;
    QSize explicitSizeHint;  // set when driving the engine directly (golden path)
};

struct PanelScene {
    QString title;
    QVector< ButtonSpec > buttons;
};

// Panel scene used by both front ends: same button set as the recorded widgets
// golden baseline (tests/core/golden_panel_geometry.txt scenario shape).
inline PanelScene clipboardPanel()
{
    PanelScene p;
    p.title = QStringLiteral("Clipboard");
    p.buttons << ButtonSpec{ ButtonSpec::Large, QStringLiteral("Paste") };
    p.buttons << ButtonSpec{ ButtonSpec::Small, QStringLiteral("Cut") };
    p.buttons << ButtonSpec{ ButtonSpec::Small, QStringLiteral("Copy") };
    p.buttons << ButtonSpec{ ButtonSpec::Medium, QStringLiteral("Format") };
    p.buttons << ButtonSpec{ ButtonSpec::Small, QStringLiteral("Redo") };
    return p;
}

}  // namespace conformance

#endif  // RIBBONCONFORMANCE_H
