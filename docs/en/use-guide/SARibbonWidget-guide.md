# Using SARibbon in Non-MainWindow Scenarios

This guide explains how to integrate SARibbon in `QDialog`, `QWidget` sub-windows, `QMdiSubWindow`, and other non-`QMainWindow` scenarios.

## SARibbonWidget Overview

`SARibbonWidget` is the QWidget-based container class provided by SARibbon, designed specifically for scenarios that do not require a full `QMainWindow`. It unifies management of `SARibbonBar` and the content area, using an internal layout to keep the Ribbon fixed at the top.

### Core Features

| Feature | Description |
|---------|-------------|
| **No QMainWindow dependency** | Can be embedded in `QDialog`, `QWidget`, `QMdiSubWindow`, or any other container |
| **Automatic layout management** | Uses an internal layout to automatically place `SARibbonBar` at the top, with the content area filling the remaining space |
| **Q_PROPERTY support** | Exposes the `ribbonTheme` property, supporting the Qt property system and quick theme switching |
| **Full Ribbon functionality** | Same complete Ribbon capabilities as `SARibbonMainWindow` (category pages, panels, quick access bar, etc.) |

---

## SARibbonWidget vs SARibbonMainWindow Comparison

| Comparison Aspect | SARibbonMainWindow | SARibbonWidget |
|-------------------|-------------------|----------------|
| **Base class** | `QMainWindow` (comes with menu bar, toolbar, status bar, etc.) | `QWidget` (lightweight, no predefined areas) |
| **ribbonBar() access** | `SARibbonMainWindow::ribbonBar()` auto-created | `SARibbonWidget::ribbonBar()` auto-created |
| **System button bar** | Provides `windowButtonBar()` — minimize, maximize, close and other system buttons | **No system button bar**, requires manual management of window frame and buttons |
| **Window frame** | Custom frameless (FramelessHelper) or native frame options available | Uses the host window's frame/attributes, no frameless handling of its own |
| **Content area setting** | `setCentralWidget()` | `setWidget()` |
| **Applicable scenarios** | Application main window, scenarios requiring full window management | Dialogs, sub-windows, tab pages, MDI sub-windows, plugin containers, etc. |
| **Layout considerations** | No need to manually layout the Ribbon | **Cannot** add a layout to `SARibbonWidget` itself; instead, set content via `setWidget()` |

---

## SARibbonWidget Class API Summary

### Q_PROPERTY

```cpp
Q_PROPERTY(SARibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)
```

| Property Name | Type | Accessors | Description |
|---------------|------|-----------|-------------|
| `ribbonTheme` | `SARibbonTheme` | `ribbonTheme()` / `setRibbonTheme()` | Ribbon theme (supports 6 built-in themes) |

### Key Methods

| Method Signature | Description |
|------------------|-------------|
| `SARibbonBar* ribbonBar() const` | Get the internal `SARibbonBar` pointer |
| `void setRibbonBar(SARibbonBar* bar)` | Set a custom `SARibbonBar` instance |
| `void setRibbonTheme(SARibbonTheme theme)` | Set the Ribbon theme |
| `SARibbonTheme ribbonTheme() const` | Get the current theme |
| `bool isUseRibbon() const` | Check whether currently in Ribbon mode |
| `void setWidget(QWidget* w)` | Set the content area widget |
| `QWidget* widget() const` | Get the current content area widget |
| `QWidget* takeWidget()` | Remove and return the content area widget (ownership transferred) |

---

## Usage Scenarios and Code Examples

### Scenario 1: Using Ribbon Menu in QDialog

Suitable for complex dialogs that need a Ribbon-style interface (such as settings dialogs, data editing dialogs).

```cpp
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include "SARibbonWidget.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPanel.h"

class RibbonDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RibbonDialog(QWidget* parent = nullptr) : QDialog(parent)
    {
        // 1. Create SARibbonWidget
        SARibbonWidget* ribbonWidget = new SARibbonWidget(this);
        SARibbonBar* ribbon = ribbonWidget->ribbonBar();

        // 2. Recommended settings for Widget mode
        ribbon->setTitleVisible(false);                    // Hide title bar
        ribbon->setApplicationButton(nullptr);             // Remove application button
        ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);  // Compact mode

        // 3. Add category pages and panels
        SARibbonCategory* cat = ribbon->addCategoryPage(tr("Edit"));
        SARibbonPanel* panel = cat->addPanel(tr("Tools"));

        QAction* actFind = new QAction(QIcon(":/icon/find.svg"), tr("Find"), this);
        QAction* actReplace = new QAction(QIcon(":/icon/replace.svg"), tr("Replace"), this);
        panel->addLargeAction(actFind);
        panel->addLargeAction(actReplace);

        // 4. Set content area
        QTextEdit* editor = new QTextEdit(this);
        ribbonWidget->setWidget(editor);

        // 5. Layout — note: set layout on QDialog, not on SARibbonWidget
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(ribbonWidget);

        resize(800, 500);
    }
};
```

---

### Scenario 2: Using Ribbon in QWidget Sub-windows

Suitable for sub-panels that are part of a main window (such as tab pages in a `QTabWidget`), where each tab page has its own independent Ribbon menu.

```cpp
#include <QWidget>
#include <QPlainTextEdit>
#include "SARibbonWidget.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPanel.h"
#include "SARibbonQuickAccessBar.h"

class RibbonTabWidget : public SARibbonWidget
{
    Q_OBJECT
public:
    explicit RibbonTabWidget(const QString& tabName, QWidget* parent = nullptr)
        : SARibbonWidget(parent)
    {
        SARibbonBar* ribbon = ribbonBar();

        // Recommended settings for Widget mode
        ribbon->setTitleVisible(false);
        ribbon->setApplicationButton(nullptr);
        ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);

        // Create category pages
        SARibbonCategory* category = ribbon->addCategoryPage(tabName);
        SARibbonPanel* panel = category->addPanel(tr("Operations"));

        QAction* actSave = new QAction(QIcon(":/icon/save.svg"), tr("Save"), this);
        QAction* actPrint = new QAction(QIcon(":/icon/print.svg"), tr("Print"), this);
        panel->addLargeAction(actSave);
        panel->addSmallAction(actPrint);

        // Quick access bar
        SARibbonQuickAccessBar* qbar = ribbon->quickAccessBar();
        qbar->addAction(actSave);

        // Content area
        QPlainTextEdit* textEdit = new QPlainTextEdit(this);
        textEdit->setPlaceholderText(tr("Content area for %1").arg(tabName));
        setWidget(textEdit);
    }
};

// Usage — embedded in main window
class HostWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HostWidget(QWidget* parent = nullptr) : QWidget(parent)
    {
        QTabWidget* tabs = new QTabWidget(this);
        tabs->addTab(new RibbonTabWidget(tr("Document 1")), tr("Doc 1"));
        tabs->addTab(new RibbonTabWidget(tr("Document 2")), tr("Doc 2"));
        tabs->addTab(new RibbonTabWidget(tr("Document 3")), tr("Doc 3"));

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(tabs);
    }
};
```

---

### Scenario 3: Using Ribbon in MDI Sub-windows

Embed an independent Ribbon interface in each child window of a `QMdiArea`.

```cpp
#include <QMdiSubWindow>
#include <QVBoxLayout>
#include <QTextEdit>
#include "SARibbonWidget.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPanel.h"

class MdiChildRibbon : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit MdiChildRibbon(const QString& title, QWidget* parent = nullptr)
        : QMdiSubWindow(parent)
    {
        setWindowTitle(title);
        setAttribute(Qt::WA_DeleteOnClose);

        // Create SARibbonWidget as the child window content
        SARibbonWidget* ribbonWidget = new SARibbonWidget();
        SARibbonBar* ribbon = ribbonWidget->ribbonBar();

        // Recommended compact settings for MDI child windows
        ribbon->setTitleVisible(false);
        ribbon->setApplicationButton(nullptr);
        ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);

        // Create editing menu
        SARibbonCategory* cat = ribbon->addCategoryPage(tr("Format"));
        SARibbonPanel* fontPanel = cat->addPanel(tr("Font"));

        QAction* actBold = new QAction(QIcon(":/icon/bold.svg"), tr("Bold"), this);
        QAction* actItalic = new QAction(QIcon(":/icon/italic.svg"), tr("Italic"), this);
        fontPanel->addSmallAction(actBold);
        fontPanel->addSmallAction(actItalic);

        SARibbonPanel* alignPanel = cat->addPanel(tr("Alignment"));
        QAction* actLeft = new QAction(QIcon(":/icon/align-left.svg"), tr("Left"), this);
        QAction* actCenter = new QAction(QIcon(":/icon/align-center.svg"), tr("Center"), this);
        QAction* actRight = new QAction(QIcon(":/icon/align-right.svg"), tr("Right"), this);
        alignPanel->addSmallAction(actLeft);
        alignPanel->addSmallAction(actCenter);
        alignPanel->addSmallAction(actRight);

        // Content area
        QTextEdit* editor = new QTextEdit();
        ribbonWidget->setWidget(editor);

        // Set SARibbonWidget as the QMdiSubWindow's widget
        // Note: QMdiSubWindow needs to use setWidget (Qt's native method)
        // but SARibbonWidget itself is already a complete container, just set it directly
        QWidget* container = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(ribbonWidget);
        setWidget(container);

        resize(640, 480);
    }
};

// Usage in the main window's MDI area
// MainWindow::addNewDocument()
// {
//     auto* child = new MdiChildRibbon(QString("Document %1").arg(docCount++));
//     ui->mdiArea->addSubWindow(child);
//     child->show();
// }
```

---

## Layout Considerations

> !!! tip "Layout Optimization in Widget Mode"
>
> **Core rule: Do not call `setLayout()` on `SARibbonWidget` itself.**
>
> `SARibbonWidget` already uses an internal layout to keep `SARibbonBar` fixed at the top. If you add an additional layout to it, it will cause layout conflicts and abnormal Ribbon display.
>
> **Correct approach**: Set the content widget into `SARibbonWidget` via `setWidget()`, then place `SARibbonWidget` into the host window's layout:
>
> ```cpp
> // ✅ Correct
> SARibbonWidget* rw = new SARibbonWidget();
> rw->setWidget(myContentWidget);       // Content managed by SARibbonWidget
> parentLayout->addWidget(rw);          // Add SARibbonWidget to parent layout
>
> // ❌ Wrong
> SARibbonWidget* rw = new SARibbonWidget();
> QVBoxLayout* layout = new QVBoxLayout(rw);  // This will break the internal layout!
> layout->addWidget(rw->ribbonBar());
> layout->addWidget(myContentWidget);
> ```
>
> **Recommended compact settings**: In Widget mode, the Ribbon should occupy as little space as possible. It is recommended to uniformly set:
>
> ```cpp
> ribbon->setTitleVisible(false);
> ribbon->setApplicationButton(nullptr);
> ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);
> ```

## Theme Switching Considerations

Setting the theme directly in the `SARibbonWidget` constructor may not take full effect. It is recommended to use `QTimer` to defer execution until the end of the event queue:

```cpp
// In SARibbonWidget constructor
SARibbonWidget::MyRibbonWidget(QWidget* parent) : SARibbonWidget(parent)
{
    // ... Initialize Ribbon ...

    // Delayed theme setting to ensure full effect
    QTimer::singleShot(0, this, [this]() {
        setRibbonTheme(SARibbonTheme::RibbonThemeDark);
    });
}
```

---

## Using Together with SARibbonMainWindow

In a `SARibbonMainWindow` main window, dialogs or sub-windows can use `SARibbonWidget` to maintain a consistent interface style:

```cpp
// Show a Ribbon dialog from the main window (SARibbonMainWindow)
void MainWindow::showSettingsDialog()
{
    RibbonDialog dlg(this);
    dlg.exec();
}

// Add a Ribbon Tab sub-window in the main window
void MainWindow::addRibbonTab()
{
    ui->tabWidget->addTab(new RibbonTabWidget(tr("New Tab")), tr("New"));
}
```

This combination approach allows the main window to retain full window management capabilities, while sub-components also get a consistent Ribbon experience.