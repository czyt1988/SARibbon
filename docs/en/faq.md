# Frequently Asked Questions (FAQ)

- ✅ **High-DPI adaptation**: covers Qt5/6 high-DPI scaling attributes and rounding policy configuration
- ✅ **Global shortcut response**: resolves hidden-panel shortcut failures in Ribbon mode
- ✅ **Theme timing**: workaround for themes not applying in constructors, see [Theme Switching](use-guide/SARibbon-theme.md)
- ✅ **Dark mode auto-switch**: how to disable the automatic theme switch when the OS is in dark mode
- ✅ **SVG icon dependency**: troubleshooting missing Qt SVG plugin at runtime
- ✅ **Multi-monitor DPI jittering**: resolving window jump when dragging across monitors with different DPI scaling

## 1. High-DPI Display Issues

To address high-DPI display issues, the following two steps are recommended:

### 1. Enable High-DPI Scaling in `main()` for `QApplication`

Set the `Qt::AA_EnableHighDpiScaling` attribute in the `main()` function. This allows the application to automatically detect the display's pixel density and apply appropriate scaling. Example code:

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

Although Qt 5.6 introduced `Qt::AA_EnableHighDpiScaling`, it doesn't fully resolve all scaling issues. Starting from Qt 5.14, you can use `QApplication::setHighDpiScaleFactorRoundingPolicy` to fine-tune scaling behavior. Like the attribute above, this must also be set before constructing the `QApplication` object:

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

!!! tip "Qt6 note"
    Qt6 enables high-DPI scaling by default and no longer requires `AA_EnableHighDpiScaling` or `AA_UseHighDpiPixmaps` (both attributes were removed in Qt6). For Qt6 users, you only need to configure `setHighDpiScaleFactorRoundingPolicy`.

    For more Ribbon sizing configuration, see [Size Settings](use-guide/SARibbon-size-settings.md).

!!! note
    If you're using an OpenGL window and encounter strange rendering issues, try removing the above settings. Recent versions of Qt no longer require them.

---

## 2. Keyboard Shortcut Issues

Users often report that when using `SARibbon`, keyboard shortcuts only work for actions in the currently active tab. In traditional toolbar mode, actions remain visible and their shortcuts always work. However, with `SARibbon`, panels (and their actions) may be hidden when switching tabs, causing shortcuts to stop functioning.

Setting the action's `shortcutContext` to `Qt::ApplicationShortcut` does **not** solve this issue. Instead, you should explicitly add the action to the main window using `QWidget::addAction()`.

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
    this->setRibbonTheme(SARibbonTheme::RibbonThemeDark);
});
```

For detailed explanations and more theme switching usage, see [Theme Switching](use-guide/SARibbon-theme.md).

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

For more title bar configuration, see [Title Bar Settings](use-guide/titlebar-setting.md).

---

## 5. Icons Not Displaying or "Could not create pixmap from xxx.svg"

If icons (e.g., minimize/maximize buttons) appear as blank buttons, or the console shows `Could not create pixmap from :\SARibbon\image\resource\xxx.svg`, the Qt SVG plugin is missing from your runtime environment. Your application directory must include `imageformats/qsvg.dll` (Windows) or the equivalent `qsvg` plugin.

**Fix:**

- Run `windeployqt` to automatically copy required plugins, **or**
- Ensure the `plugins/imageformats` folder is in your system's `PATH` environment variable.

!!! tip
    This issue affects all widgets that depend on SVG resources, including Ribbon icons and Gallery items.

---

## 6. Window Jitter When Dragging Across Multi-Monitor DPI Boundaries

When you have multiple monitors with different scaling factors (for example, Screen A at 200 percent and Screen B at 150 percent), dragging a window from the high-scaling screen toward the low-scaling screen edge can cause the window to shake violently, jumping back and forth between the two screens.

### Cause

When a window crosses a DPI boundary, Qt recalculates the logical size based on the new monitor's scaling factor. For example, moving from Screen A (2.0x) to Screen B (1.5x):

- On Screen A: logical size is 2816 x 2804, corresponding to physical pixels 5632 x 5608
- When crossing to Screen B: logical size is recalculated to 2112 x 2103
- Because the logical size suddenly shrinks, the right edge of the window moves sharply left, causing the window center to fall back onto Screen A. Qt triggers DPI switching again, creating an infinite loop

### Solutions

1. **Recommended: Enable the QWindowKit frameless solution**

    QWindowKit is a cross-platform third-party frameless solution with improved handling of multi-monitor DPI differences. It also supports Windows 11 Snap Layout and other system features. Set the CMake option when building:

    ```cmake
    SARIBBON_USE_FRAMELESS_LIB=ON
    ```

    !!! note
        Enabling QWindowKit requires C++17. The [QWindowKit](https://github.com/stdware/qwindowkit) source is bundled in the repository and is built automatically, so no separate installation is needed. For configuration details, see the [Build Guide](./build-guide/build-SARibbon.md).

2. **Use Qt6 with the latest SARibbon**

    Qt5 has known multi-monitor DPI bugs. We recommend using Qt6 (version 6.2 or later). With the latest SARibbon, multi-monitor DPI works correctly even without QWindowKit (using the pure Qt frameless simulation).

3. **Set `AA_DontCreateNativeWidgetSiblings` when embedding native HWND windows**

    After enabling QWindowKit, if you embed a native HWND window inside `SARibbonMainWindow` (for example, a DockWidget containing a Win32 native control), it may cause the window handle to break and dragging via the title bar to fail. This is similar to [QWindowKit Issue #32](https://github.com/stdware/qwindowkit/issues/32). Add the following in your `main` function:

    ```cpp
    int main(int argc, char* argv[])
    {
        QGuiApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
        QApplication a(argc, argv);
        // ...
    }
    ```

    !!! warning
        `Qt::AA_DontCreateNativeWidgetSiblings` is a global Qt attribute that affects the native widget creation behavior of all QWidgets. Set this attribute only when you actually need to embed a native HWND window.
---

## 7. Embedded Native Render Windows (e.g. Qt3DWindow) Start at an Offset Position

When embedding a Qt3D render window (or other native QWindow) into the central area of `SARibbonMainWindow` via `QWidget::createWindowContainer(new Qt3DWindow)`, the embedded container starts at an offset position.

### Cause

The container returned by `createWindowContainer` is a **native child window** (it holds an independent native window handle). Combined with a frameless main window (`UseRibbonFrame` mode), there are two known pitfalls:

1. **Missing `Qt::AA_DontCreateNativeWidgetSiblings`**: by default Qt creates native sibling windows for native child windows to keep stacking correct, which conflicts with frameless windows (which rely on painting the whole client area), causing clipping/offset issues (same as [QWindowKit Issue #32](https://github.com/stdware/qwindowkit/issues/32), see section 6 item 3).
2. **Geometry sync issue of `createWindowContainer` in older Qt versions**: some Qt 5.12/5.14 versions do not sync the native child window's initial geometry with the container layout on first show, showing up as a leftward offset that disappears after the window is resized once.

Verified on Qt 5.15.16 + Windows 10 with the recommended usage below: the container's global position matches the layout expectation exactly (the offset equals the layout margins and is independent of DPI).

### Solution

1. **Set the attribute at the very beginning of `main` (before QApplication is created)**:

    ```cpp
    int main(int argc, char* argv[])
    {
        QGuiApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
        QApplication a(argc, argv);
        // ...
    }
    ```

2. **Use Qt 5.15 or later** (Qt 6 recommended). The geometry sync issue of `createWindowContainer` in older versions has been fixed.

3. **Check layout margins before assuming a bug**: `QVBoxLayout` and friends have default 9px margins, and the frameless `SARibbonMainWindow` has a 2px content margin (`setContentsMargins(2,0,2,0)`). These are normal layout behavior, not window offsets. Confirm with `layout()->contentsMargins()` and `contentsMargins()`.

4. **Reproduction and measurement tool**: the repository provides `example/Qt3DWindowExample`, which prints the geometry of the main window / central widget / container (offset, DPR, native window geometry) on startup, and supports a `--dump` argument (print and exit). If you see an offset, please attach the `qt3d-geometry.log` output when filing an issue.

---

## 8. How to Disable the Automatic Theme Switch in System Dark Mode

The constructors of `SARibbonMainWindow` and `SARibbonWidget` detect the operating system color scheme: when the system is in dark mode and the theme is still the default `RibbonThemeOffice2021Blue`, it is automatically switched to `RibbonThemeDark`.

To prevent the theme from following the system color scheme, call the following before constructing any window:

```cpp
#include "SARibbonUtil.h"

SA::setEnableSystemDarkModeAutoSwitch(false);
```

The function is declared in the `SA` namespace in `SARibbonUtil.h` and is enabled by default. Once disabled, the default theme stays `RibbonThemeOffice2021Blue`, and explicit `setRibbonTheme()` calls keep working as usual.

See [Theme Switching](use-guide/SARibbon-theme.md) for details.
