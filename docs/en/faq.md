# Frequently Asked Questions (FAQ)

## 1. High-DPI Display Issues

To address high-DPI display issues, the following two steps are recommended:

### 1. Enable High-DPI Scaling in `main()` for `QApplication`

Set the `Qt::AA_EnableHighDpiScaling` attribute in the `main()` function. This allows the application to automatically detect the display’s pixel density and apply appropriate scaling. Example code:

```cpp
int main(int argc, char* argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication a(argc, argv);
    ......
}
```

### 2. Set High-DPI Scale Factor Rounding Policy

Although Qt 5.6 introduced `Qt::AA_EnableHighDpiScaling`, it doesn’t fully resolve all scaling issues. Starting from Qt 5.14, you can use `QApplication::setHighDpiScaleFactorRoundingPolicy` to fine-tune scaling behavior. Like the attribute above, this must also be set before constructing the `QApplication` object:

```cpp
int main(int argc, char* argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
    QApplication a(argc, argv);
    ......
}
```

!!! note
    If you're using an OpenGL window and encounter strange rendering issues, try removing the above settings—recent versions of Qt may no longer require them.

---

## 2. Keyboard Shortcut Issues

Users often report that when using `SARibbon`, keyboard shortcuts only work for actions in the currently active tab. In traditional toolbar mode, actions remain visible and their shortcuts always work. However, with `SARibbon`, panels (and their actions) may be hidden when switching tabs, causing shortcuts to stop functioning.

Setting the action’s `shortcutContext` to `Qt::ApplicationShortcut` does **not** solve this issue. Instead, you should explicitly add the action to the main window using `QWidget::addAction()`.

**Example:**

```cpp
void MainWindow::initRibbon(){
    SARibbonCategory* categoryMain = ribbon->addCategoryPage(tr("Main"));
    SARibbonPanel* panel = categoryMain->addPanel(tr("io"));
    // Action creation omitted for brevity
    QAction* actSave = new QAction(this);
    ...
    // Set shortcut
    actSave->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    // Add action to panel
    panel->addLargeAction(actSave);
    // Also add action to MainWindow to ensure shortcut works globally
    addAction(actSave);
}
```

---

## 3. Theme Settings Not Taking Effect

In some Qt versions, setting the ribbon theme in the constructor may not fully apply. A workaround is to defer the theme setting using `QTimer` to ensure it runs after the UI is fully initialized:

```cpp
QTimer::singleShot(0, this, [this]() {
    this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark);
});
```

---

## 4. Minimize/Maximize Icons Appear in Top-Left Instead of Top-Right

If you encounter this issue, verify that your compiled library files (`.dll`, `.lib`) and header files are from the **same version** of SARibbonBar. This problem commonly occurs when only the DLLs are updated without replacing the corresponding header files.

**Solution:** Ensure full version consistency by deleting and reinstalling all related files. If you use CMake to install SARibbonBar, the following files/directories should be updated together:

- `bin/SARibbonBar.dll`
- `include/SARibbonBar/` (directory)
- `lib/SARibbonBar.lib`
- `lib/qmake/SARibbonBar/` (directory)
- `lib/cmake/SARibbonBar/` (directory)
- `SARibbonBar_amalgamate`

---

## 5. Icons Not Displaying

If icons (e.g., minimize/maximize buttons) appear as blank buttons, it usually means the Qt SVG plugin is missing. Your application directory must include the `imageformats/qsvg.dll` plugin.

**Fix:**  
- Run `windeployqt` to automatically copy required plugins, **or**  
- Ensure the `plugins/imageformats` folder is in your system’s `PATH` environment variable.

---

## 6. Error: "Could not create pixmap from :\SARibbon\image\resource\xxx.svg"

This error is related to **Issue #5** above. It occurs when the SVG plugin (`qsvg.dll`) is missing.

**Solution:** Make sure `imageformats/qsvg.dll` is present in your application’s runtime directory.
