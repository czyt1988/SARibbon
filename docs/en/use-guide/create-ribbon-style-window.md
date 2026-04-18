# Creating a Ribbon-style Window

## Creating a Ribbon-style MainWindow

The core of SARibbon is the `SARibbonBar` class, which can be used in both `MainWindow` and `Widget`.

If used in `MainWindow`, your main window should inherit from `SARibbonMainWindow`, which will automatically create a `SARibbonBar` for you.

Sample code for creating a Ribbon-style `MainWindow`:

```cpp
#include "mainwindow.h"
#include "SARibbon.h"

//
class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr) : SARibbonMainWindow(parent)
    {
        // Get the RibbonBar pointer
        SARibbonBar* ribbon = ribbonBar();
        ...
    }
};
```

!!! warning "Note:"
    If your `MainWindow` is created through Qt Designer, **be sure to delete the default `menuBar` in Designer**. Because `SARibbonMainWindow` will replace the native menu bar with its own `SARibbonBar`. If the native menu bar is retained in the `.ui` file, it will override the ribbon menu back to the native one.

The constructor of `SARibbonMainWindow` has three parameters, defined as follows:

```cpp
SARibbonMainWindow(QWidget* parent, SARibbonMainWindowStyles style, const Qt::WindowFlags flags)
```

The most critical is the second parameter. The second parameter `SARibbonMainWindowStyles` determines the overall style of the window. The commonly used combinations are the following two:

1. `SARibbonMainWindowStyleFlag::UseRibbonMenuBar|SARibbonMainWindowStyleFlag::UseRibbonFrame`

    This is the default style, using a ribbon-style menu bar and a ribbon-style frame. In this case, the window has a custom frame instead of the native frame. The interface effect is as follows:

    ![mainwindow-ribbonbar+ribbonframe](../../assets/pic/mainwindow-ribbonbar+ribbonframe.png)

2. `SARibbonMainWindowStyleFlag::UseRibbonMenuBar|SARibbonMainWindowStyleFlag::UseNativeFrame`

    This style uses a ribbon-style menu bar and a native frame. The advantage of this interface is that it supports the operating system's border effects, such as Windows 11's global window shortcuts and some border special effects.

    ![mainwindow-ribbonbar+nativeframe](../../assets/pic/mainwindow-ribbonbar+nativeframe.png)

When using the native frame, to better adapt to the native border, `SARibbonMainWindow` will hide the icon and set the ribbonbar style to compact mode:

```cpp
// When using the native border in ribbon mode, the icon will be hidden and the default mode will be set to compact
if (SARibbonBar* bar = ribbonBar()) {
    if (SARibbonTitleIconWidget* iconWidget = bar->titleIconWidget()) {
        // Hide the icon
        iconWidget->hide();
    }
    // Set to compact mode
    bar->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);
}
```

## Creating a Ribbon-style Widget

In addition to the main window, you can also use the Ribbon interface on ordinary `QWidget` or `QDialog`, which is very useful when creating complex dialog boxes or sub-windows. For this purpose, SARibbon provides the `SARibbonWidget` class.

```cpp
#include "SARibbonWidget.h"
class MyRibbonWidget : public SARibbonWidget
{
    Q_OBJECT

public:
    explicit MyRibbonWidget(QWidget *parent = nullptr);
};
```

Implementation file:

```cpp
// myribbonwidget.cpp
#include "myribbonwidget.h"
#include "SARibbonBar.h"

MyRibbonWidget::MyRibbonWidget(QWidget *parent)
    : SARibbonWidget(parent)
{
    // 1. Get the RibbonBar
    SARibbonBar* ribbon = ribbonBar();

    // 2. Optimize for Widget mode
    ribbon->setTitleVisible(false); // Hide the title bar, as QWidget usually does not have an independent title
    ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow); // Use compact mode to save space
    ribbon->setApplicationButton(nullptr); // Remove the Application Button, which is usually not needed for Widget

    // Create your ribbon menu
}
```

You can embed any `QWidget` into `SARibbonWidget` through the `setWidget()` method. Here is a typical usage example:

```cpp
// Using SARibbonWidget in a QDialog and setting the central content
SARibbonWidget* ribbonWidget = new SARibbonWidget(this);

// Configure the RibbonBar (recommended settings for Widget mode)
SARibbonBar* ribbon = ribbonWidget->ribbonBar();
ribbon->setTitleVisible(false);
ribbon->setApplicationButton(nullptr);
ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);

// Create the central content widget
QWidget* contentWidget = new QWidget(this);
// ... Set up the layout and configuration for contentWidget ...

// Embed the central content into SARibbonWidget
ribbonWidget->setWidget(contentWidget);

// Add SARibbonWidget to the dialog layout
QVBoxLayout* layout = new QVBoxLayout(this);
layout->setContentsMargins(0, 0, 0, 0);
layout->addWidget(ribbonWidget);
```

!!! tip "setWidget() Usage Tips"
    - `setWidget()` automatically manages the passed `QWidget` as the central area of `SARibbonWidget`, without the need for manual layout.
    - After calling `setWidget()`, the previously set central widget will be replaced. To remove the central widget, call `setWidget(nullptr)`.
    - The central widget will automatically adjust to follow the size changes of `SARibbonWidget`.

!!! info "More Information"
    For more details on using `SARibbonWidget` in dialogs, sub-windows, and other scenarios, including different embedding modes, layout strategies, and advanced usage, please refer to the [SARibbonWidget Detailed Guide](./SARibbonWidget-guide.md).

## Complete Native Frame Example

Here is a complete example using the native frame mode (referenced from `example/UseNativeFrameExample`):

```cpp
#include "SARibbon.h"

class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* par = nullptr)
        : SARibbonMainWindow(par,
            SARibbonMainWindowStyleFlag::UseNativeFrame
            | SARibbonMainWindowStyleFlag::UseRibbonMenuBar)
    {
        SARibbonBar* ribbon = ribbonBar();
        // Compact mode is recommended when using the native frame
        ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);
        // Disable vertical expansion of the Application Button
        ribbon->setApplicationButtonVerticalExpansion(false);

        // Create category pages and panels
        SARibbonCategory* category = ribbon->addCategoryPage(tr("Main"));
        SARibbonPanel* panel = category->addPanel(tr("Operations"));

        QAction* act = new QAction(QIcon(":/icon/save.svg"), tr("Save"), this);
        panel->addLargeAction(act);

        resize(800, 600);
    }
};
```

## Complete SARibbonWidget Example

Here is a complete example of using `SARibbonWidget` in an ordinary dialog (referenced from `example/WidgetWithRibbon`):

```cpp
#include "SARibbon.h"
#include <QVBoxLayout>
#include <QTextEdit>

class MyDialog : public QDialog
{
    Q_OBJECT
public:
    MyDialog(QWidget* parent = nullptr) : QDialog(parent)
    {
        // Create SARibbonWidget
        SARibbonWidget* ribbonWidget = new SARibbonWidget(this);
        SARibbonBar* ribbon = ribbonWidget->ribbonBar();

        // Recommended settings for Widget mode
        ribbon->setTitleVisible(false);
        ribbon->setApplicationButton(nullptr);
        ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);

        // Add Ribbon content
        SARibbonCategory* cat = ribbon->addCategoryPage(tr("Edit"));
        SARibbonPanel* panel = cat->addPanel(tr("Tools"));
        panel->addLargeAction(new QAction(QIcon(":/icon/edit.svg"), tr("Edit"), this));

        // Set the central content
        QTextEdit* editor = new QTextEdit(this);
        ribbonWidget->setWidget(editor);

        // Layout
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(ribbonWidget);
        resize(600, 400);
    }
};
```