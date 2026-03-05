# SARibbon User Guide

SARibbon is a Qt library for creating modern Ribbon interfaces, with a style similar to Microsoft Office or WPS. It is designed for complex desktop applications, effectively organizing a large number of functions, and is commonly used in the interface development of industrial software.

Before starting coding, you need to integrate the SARibbon library into your Qt project. The simplest way is **static embedding**, which is to directly copy the source files `SARibbon.h` and `SARibbon.cpp` into your project.

## Quick Start

```cpp
#include "SARibbon.h"
#include <QApplication>

int main(int argc, char* argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    SARibbonBar::initHighDpi();
    QApplication a(argc, argv);
    
    SARibbonMainWindow w;
    w.setWindowTitle("SARibbon Quick Start");
    
    SARibbonBar* ribbon = w.ribbonBar();
    SARibbonCategory* cat = ribbon->addCategoryPage("Home");
    SARibbonPanel* panel = cat->addPanel("Actions");
    panel->addLargeAction(new QAction(QIcon(":/icon.svg"), "Click Me", &w));
    
    w.show();
    return a.exec();
}
```

## Documentation Reading Guide

| Topic | Document | Description |
|-------|----------|-------------|
| Building | [Build Instructions](../../en/build-guide/build-instructions.md) | How to build SARibbon (CMake/QMake) |
| Integration | [Importing the Library](./import-SARibbon.md) | Static/dynamic integration into your project |
| Window Setup | [Create a Ribbon-Style Window](./create-ribbon-style-window.md) | SARibbonMainWindow / SARibbonWidget usage |
| UI Creation | [Creating the Ribbon UI](./create-ribbon-ui.md) | Category, Panel, Action, Gallery, etc. |
| Layout | [Ribbon Layout Options](./layout-of-SARibbon.md) | Loose/Compact, 2-row/3-row modes |
| Theming | [Ribbon Themes](./SARibbon-theme.md) | Built-in themes and custom QSS styling |
| Customization | [User-Configurable Ribbon](./persistence-configuration-ribbon.md) | Runtime customization and XML persistence |

## Differences between Ribbon interface and traditional menubar+toolbar

The traditional menubar+toolbar cannot be directly converted into a ribbon interface. Ribbon is not just a toolbar with `QToolBar`. Compared with the traditional menu bar and toolbar, it has the following characteristics:

- The button rendering method of Ribbon has an obvious change, making it impossible to directly use ToolButton for simulation. SARibbon uses `SARibbonToolButton` to re-layout and render the buttons for Ribbon.
- Ribbon also has a special type of tab called `Context Category`. For example, when you select a picture in Office Word, a "Picture Editing" tab will automatically appear, providing picture-specific functions such as cropping and rotating. This tab will automatically hide when the selection is canceled.
- The Ribbon interface comes with some special controls, such as Gallery (the style selection in Word is a Gallery control).

## Terminology

| Term | SARibbon Class | Description |
|------|---------------|-------------|
| Ribbon Bar | `SARibbonBar` | The main Ribbon control at the top of the window |
| Category | `SARibbonCategory` | A tab page, equivalent to a functional group |
| Panel | `SARibbonPanel` | A group of related actions within a Category |
| Tool Button | `SARibbonToolButton` | Ribbon-specific button with custom painting |
| Context Category | `SARibbonContextCategory` | Conditional tab that appears based on context |
| Gallery | `SARibbonGallery` | Grid-style visual selector (e.g., styles in Word) |
| Quick Access Bar | `SARibbonQuickAccessBar` | Toolbar at the very top for frequently used actions |
| Application Button | `SARibbonApplicationButton` | The "File" button at the top-left corner |
