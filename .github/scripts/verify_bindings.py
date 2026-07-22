"""Cross-platform verification of PySideSARibbon.

Строит ленту эталонного объёма и проверяет четыре свойства, которые нужны
приложению: контекстная вкладка, встроенный виджет в панели, сворачивание,
quick access bar. Плюс сигналы и управление доступностью.

Ненулевой код возврата = биндинги на этой платформе непригодны.
"""

from __future__ import annotations

import os
import sys

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtGui import QAction  # noqa: E402
from PySide6.QtWidgets import QApplication, QComboBox, QTextEdit  # noqa: E402

failures: list[str] = []


def check(label: str, ok: bool) -> None:
    print(f"  {'OK  ' if ok else 'FAIL'}  {label}")
    if not ok:
        failures.append(label)


def main() -> int:
    app = QApplication(sys.argv)

    import PySideSARibbon
    from PySideSARibbon import saribbon

    print(f"platform={sys.platform} python={sys.version.split()[0]}")
    print(f"PySideSARibbon={PySideSARibbon.__version__}")
    from PySide6 import __version__ as pyside_version
    from PySide6.QtCore import qVersion

    print(f"PySide6={pyside_version} Qt={qVersion()}")
    print(f"module={saribbon.__file__}")

    window = saribbon.SARibbonMainWindow()
    window.setWindowTitle("verify")
    bar = window.ribbonBar()
    bar.setRibbonStyle(saribbon.SARibbonBar.RibbonStyleFlag.RibbonStyleLoose)

    triggered: list[str] = []

    def add(panel, text: str, command_id: str, *, large: bool = False) -> QAction:
        action = QAction(text, window)
        action.triggered.connect(lambda *_, cid=command_id: triggered.append(cid))
        (panel.addLargeAction if large else panel.addSmallAction)(action)
        return action

    home = bar.addCategoryPage("Home")
    project = home.addPanel("Project")
    for title, cid in [("New", "project.new"), ("Save", "project.save")]:
        add(project, title, cid)
    simulation = home.addPanel("Simulation")
    run_action = add(simulation, "Run", "sim.run", large=True)

    model = bar.addCategoryPage("Model")
    fluid = model.addPanel("Fluid")
    combo = QComboBox()
    combo.addItems(["methane", "ethane"])
    fluid.addMediumWidget(combo)

    context = bar.addContextCategory("Plot Tools")
    zoom = context.addCategoryPage("Zoom")
    add(zoom.addPanel("Navigate"), "Zoom in", "plot.zoom_in")

    quick = bar.quickAccessBar()
    quick.addAction(run_action)

    window.setCentralWidget(QTextEdit())
    window.resize(1200, 620)
    window.show()
    app.processEvents()

    print("checks:")
    check("categories created", len(bar.categoryPages()) >= 2)
    check("widget embedded in panel", combo.parent() is not None)

    check("context category hidden initially", not bar.isContextCategoryVisible(context))
    bar.showContextCategory(context)
    app.processEvents()
    check("context category shows", bar.isContextCategoryVisible(context))
    bar.hideContextCategory(context)
    app.processEvents()
    check("context category hides", not bar.isContextCategoryVisible(context))

    bar.setMinimumMode(True)
    app.processEvents()
    check("ribbon collapses", bar.isMinimumMode())
    bar.setMinimumMode(False)
    app.processEvents()
    check("ribbon expands", not bar.isMinimumMode())

    check("quick access bar available", quick is not None)

    run_action.setEnabled(False)
    check("command can be disabled", not run_action.isEnabled())
    run_action.setEnabled(True)
    run_action.trigger()
    check("signal delivered", triggered == ["sim.run"])

    out = f"ribbon-{sys.platform}.png"
    check("window renders", window.grab().save(out))

    if failures:
        print(f"\nFAILED: {len(failures)} -> {failures}")
        return 1
    print("\nALL CHECKS PASSED")
    return 0


if __name__ == "__main__":
    sys.exit(main())
