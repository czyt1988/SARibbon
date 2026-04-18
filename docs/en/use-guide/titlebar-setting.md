# Title Bar Settings

The SARibbon title bar is the area at the very top of the Ribbon interface, used to display the application's window title (`windowTitle`). SARibbon allows you to fully customize the title bar's height, background color, text color, alignment, as well as the title icon and functional buttons.

**Key Features**

- ✅ **Custom title height**: precisely control title bar height via `setTitleBarHeight()`
- ✅ **Title color customization**: support setting title text color and background color (QBrush)
- ✅ **Title alignment**: support left-aligned and center-aligned, simulating WPS style
- ✅ **Title icon support**: display window icon and right-click menu via `SARibbonTitleIconWidget`
- ✅ **Show/hide toggle**: support dynamically hiding or showing the title bar
- ✅ **Word wrap control**: control whether title text automatically wraps via `enableWordWrap`

## Core API

`SARibbonBar` provides the following title bar-related properties and methods:

| Method | Parameters | Return Value | Description |
|------|------|--------|------|
| `setWindowTitleTextColor()` | `const QColor&` | `void` | Set title bar text color |
| `windowTitleTextColor()` | None | `QColor` | Get current title text color |
| `setWindowTitleAligment()` | `Qt::Alignment` | `void` | Set title text alignment |
| `windowTitleAligment()` | None | `Qt::Alignment` | Get title text alignment |
| `setWindowTitleBackgroundBrush()` | `const QBrush&` | `void` | Set title bar background brush |
| `windowTitleBackgroundBrush()` | None | `QBrush` | Get title bar background brush |
| `setTitleVisible()` | `bool` | `void` | Set title bar visibility |
| `isTitleVisible()` | None | `bool` | Query whether the title bar is visible |
| `setTitleBarHeight()` | `int, bool` | `void` | Set title bar height |
| `titleBarHeight()` | None | `int` | Get title bar height |
| `setTabBarBaseLineColor()` | `const QColor&` | `void` | Set tab bar baseline color |
| `tabBarBaseLineColor()` | None | `QColor` | Get tab bar baseline color |
| `setEnableWordWrap()` | `bool` | `void` | Enable/disable title word wrap |
| `isEnableWordWrap()` | None | `bool` | Query whether word wrap is enabled |
| `setEnableShowPanelTitle()` | `bool` | `void` | Enable/disable panel title display |
| `isEnableShowPanelTitle()` | None | `bool` | Query whether panel titles are displayed |
| `setTabOnTitle()` | `bool` | `void` | Set tabs to overlay the title bar |
| `isTabOnTitle()` | None | `bool` | Query whether tabs overlay the title |
| `setTitleIconVisible()` | `bool` | `void` | Set title icon visibility |
| `isTitleIconVisible()` | None | `bool` | Query whether the title icon is visible |
| `titleIconWidget()` | None | `SARibbonTitleIconWidget*` | Get title icon widget pointer |

## Common Scenarios

| Scenario | Recommended Method | Description |
|------|---------|------|
| Unregistered / trial prompt | `setWindowTitleBackgroundBrush()` + `setWindowTitleTextColor()` | Red background + white text, prominently indicating software status to the user |
| Read-only mode | Gray background + dark text | Indicates the current document cannot be edited |
| Hide title bar | `setTitleVisible(false)` | Suitable for compact mode, native frame mode |
| Custom alignment | `setWindowTitleAligment(Qt::AlignLeft)` | Title left-aligned, similar to WPS style |
| Title icon display | `setTitleIconVisible(true)` | Display window icon, supports left-click and right-click menu |

## Setting Title Bar Color and Style

You can achieve special title bar display effects with the following code:

```cpp
void MainWindow::setWindowTitleColor()
{
    SARibbonBar* ribbon = ribbonBar();
    if (!ribbon) {
        return;
    }
    // Set title background to red
    ribbon->setWindowTitleBackgroundBrush(QColor(222, 79, 79));
    // Set title text color to white
    ribbon->setWindowTitleTextColor(Qt::white);
    // Update display
    ribbon->update();
}
```

The display effect of the above code is as follows:

![chang-title-background](../../assets/pic/chang-title-background.png)

## Using QSS Style Sheets to Set the Title Bar

In addition to code-based settings, you can also customize title bar styles through Qt Style Sheets (QSS):

```css
/* Set title bar background color and text color */
SARibbonBar {
    background-color: #4A90E2;        /* Blue background */
    color: #FFFFFF;                   /* White text */
    font-family: "Microsoft YaHei";  /* Font */
    font-size: 13px;
    font-weight: bold;               /* Bold */
}

/* Hover highlight effect */
SARibbonBar:hover {
    background-color: #5BA0F2;
}
```

!!! tip "QSS Live Preview"
    After modifying QSS, you need to call `ribbon->update()` to see the effect immediately. You can use a QSS debugging tool during development to preview styles in real time.

## Resetting the Title Bar

In some scenarios, you need to restore the title bar to its default state after dynamically changing its color:

```cpp
void MainWindow::resetTitleBar()
{
    SARibbonBar* ribbon = ribbonBar();
    if (!ribbon) {
        return;
    }
    // Restore to transparent background (use theme default color)
    ribbon->setWindowTitleBackgroundBrush(Qt::NoBrush);
    // Restore to default text color (follow theme)
    ribbon->setWindowTitleTextColor(QColor());
    ribbon->update();
}
```

!!! info "Default Values"
    Calling `setWindowTitleTextColor(QColor())` or `setWindowTitleBackgroundBrush(Qt::NoBrush)` restores to theme defaults. `QColor()` constructs an invalid color object, indicating the system default color should be used.

## Complete Code Example

The following example demonstrates initializing the title bar completely in the `MainWindow` constructor:

```cpp
MainWindow::MainWindow(QWidget* parent)
    : SARibbonMainWindow(parent)
{
    // Create SARibbonBar
    SARibbonBar* ribbon = new SARibbonBar(this);
    setMenuBar(ribbon);

    // Set title bar height
    ribbon->setTitleBarHeight(40, true);

    // Set title text color and alignment
    ribbon->setWindowTitleTextColor(QColor(33, 33, 33));
    ribbon->setWindowTitleAligment(Qt::AlignCenter);

    // Enable title icon
    ribbon->setTitleIconVisible(true);

    // Enable tabs overlaying title bar (compact style)
    ribbon->setTabOnTitle(true);

    // Set panel title display
    ribbon->setEnableShowPanelTitle(true);

    // Create Ribbon categories and panels
    SARibbonCategory* category = ribbon->addCategoryPage(tr("Home"));
    category->addPanel(tr("File"));

    setCentralWidget(new QWidget(this));
}
```

!!! tip "Note"
    Title bar settings are only visible in Loose mode. In Compact mode, the title bar and tab bar are merged, so title bar background color settings will not have a noticeable effect, but text color still takes effect.