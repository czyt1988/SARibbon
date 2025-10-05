# SARibbon User Guide

SARibbon is a Qt library for creating modern Ribbon interfaces, with a style similar to Microsoft Office or WPS. It is designed for complex desktop applications, effectively organizing a large number of functions, and is commonly used in the interface development of industrial software.

Before starting coding, you need to integrate the SARibbon library into your Qt project. The simplest way is **static embedding**, which is to directly copy the source files `SARibbon.h` and `SARibbon.cpp` into your project.

## Differences between Ribbon interface and traditional menubar+toolbar

The traditional menubar+toolbar cannot be directly converted into a ribbon interface. Ribbon is not just a toolbar with `QToolBar`. Compared with the traditional menu bar and toolbar, it has the following characteristics:

- The button rendering method of Ribbon has an obvious change, making it impossible to directly use ToolButton for simulation. SARibbon uses `SARibbonToolButton` to re-layout and render the buttons for Ribbon.
- Ribbon also has a special type of tab called `Context Category`. For example, when you select a picture in Office Word, a "Picture Editing" tab will automatically appear, providing picture-specific functions such as cropping and rotating. This tab will automatically hide when the selection is canceled.
- The Ribbon interface comes with some special controls, such as Gallery (the style selection in Word is a Gallery control).