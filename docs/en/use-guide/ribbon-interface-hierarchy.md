# Ribbon Interface Hierarchy

Understanding the SARibbon hierarchy is the foundation for building a Ribbon UI.  
The structure is clear and layered:

1. **`SARibbonBar`** – the top-level Ribbon menu that manages the entire interface.  
   It contains:

   - **`Application Button`** – the main-menu button in the top-left corner.  
   - **`Quick Access Bar`** – the row of frequently-used actions at the very top.  
   - **`SARibbonTabBar`** – the tab bar that shows every `Category` label.  
   - **`SARibbonStackedWidget`** – a stacked widget holding the contents of the currently active `Category`.  
   - **`Right Button Group`** – a button group anchored to the top-right corner.  
   - **`Window Button Bar`** – system buttons (min / max / close) and any extra buttons you add beside them.

2. **`SARibbonCategory` (Category page)** – represents a functional scene such as *Home*, *Insert*, or *Design*.  
   Each `Category` owns one or more `Panel`s.  
   It appears as a tab on `SARibbonTabBar` and its contents are managed by `SARibbonStackedWidget`.

3. **`SARibbonPanel` (Panel)** – a functional group inside a `Category`; think of it as an enhanced toolbar.  
   A panel can hold:
   - `QAction`s (rendered as buttons)  
   - arbitrary `QWidget`s (combos, line-edits, etc.)  
   - `SARibbonButtonGroupWidget` (logical button groups)

4. **`SARibbonContextCategory` (Context Category)** – a special `Category` that is hidden by default and appears only when certain conditions are met (e.g. a specific object is selected).

!!! warning
    Context categories are **not** visible until you explicitly call `show()` or bind them to a context object.

The overall layout (loose style) is shown below:

![saribbonbar-level](../../assets/pic/saribbonbar-level.png)