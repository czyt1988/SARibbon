# Ribbon Button Layout Guide

- ✅ **Strategy Pattern architecture**: v2.7.0+ introduces SARibbonButtonLayoutStrategy hierarchy, clean layout logic separation
- ✅ **Two layout schemes**: Word-wrap (Office style) and non-wrap (WPS style), choose by language context
- ✅ **Three button sizes**: Large(32×32)/Medium/Small(20×20), icon size auto-adapts to button mode
- ✅ **Single-row horizontal layout**: v2.8.0 adds enableIconRightText property, icon-left text-right horizontal arrangement
- ✅ **Binary search line-break**: Long text auto-wraps with up to 10 iterations for optimal width
- ✅ **MenuButtonPopup vertical split**: Ribbon-specific button mode, top half executes default action, bottom pops menu

---

The Ribbon UI cannot be implemented with a simple `QTabBar + QToolButton` combination mainly because ribbon buttons are rendered in a special way. They differ significantly from classic menu buttons. The difference is obvious when a **menu is present**, especially in `MenuPopup` mode: classic toolbars split the button **horizontally** (icon | arrow), while **Ribbon splits it vertically** (icon on top, text + arrow at the bottom).

![ribbon-toolbutton](../../assets/pic/ribbon-toolbutton.gif)

`SARibbon` uses `SARibbonToolButton`, a subclass of `QToolButton` with custom painting.  
Each button is divided into **three rectangles**:

1. Icon area  
2. Text area  
3. Indicator (drop-down arrow) area  

There are **two layout strategies**, chosen automatically according to **word-wrap** mode.

## Layout Strategy Architecture (v2.7.0+)

Starting from v2.7.0, the layout calculation of `SARibbonToolButton` has been refactored using the **Strategy Pattern**, introducing the `SARibbonButtonLayoutStrategy` class hierarchy:

```mermaid
classDiagram
    class SARibbonButtonLayoutStrategy {
        <<abstract>>
        +layout(SARibbonButtonLayoutContext*) QRectList
        +iconRect() QRect
        +textRect() QRect
        +indicatorRect() QRect
    }
    class SARibbonLargeButtonLayoutStrategy {
        +layout(context) QRectList
        +iconRect() QRect
        +textRect() QRect
        +indicatorRect() QRect
    }
    class SARibbonSmallButtonLayoutStrategy {
        +layout(context) QRectList
        +iconRect() QRect
        +textRect() QRect
        +indicatorRect() QRect
    }
    class SARibbonButtonLayoutContext {
        +buttonSize : QSize
        +iconSize : QSize
        +spacing : int
        +wordWrap : bool
        +hasMenu : bool
        +text : QString
        +fontMetrics : QFontMetrics
    }
    SARibbonButtonLayoutStrategy <|-- SARibbonLargeButtonLayoutStrategy
    SARibbonButtonLayoutStrategy <|-- SARibbonSmallButtonLayoutStrategy
    SARibbonButtonLayoutStrategy --> SARibbonButtonLayoutContext : uses
```

This design enables:

- Cleaner separation of layout logic
- Easy extension for new button type layouts
- Better unit test support

The layout context `SARibbonButtonLayoutContext` encapsulates all parameters needed for layout calculation, including spacing, icon sizes, and word-wrap settings.

---

## Word-Wrap Mode (Office Word style)

When word-wrap is **on**, the text area occupies **two lines**.

![office-word-wrodwrap](../../assets/pic/office-word-wrodwrap.png)

- If the text **fits on one line**, the second line stays **blank** (first panel above).  
- If the text **wraps**, it fills the second line (second panel).  
- When a **menu is present**, the arrow is placed at the **right end of the second line**, even if the text is short.

![office-word-wrodwrap2](../../assets/pic/office-word-wrodwrap2.png)

Layout in `SARibbonToolButton`:

![ribbon-toolbutton-largebutton](../../assets/pic/ribbon-toolbutton-largebutton.png)

Enable / disable wrapping globally:

```cpp
ribbonBar()->setEnableWordWrap(true);   // or false
```

**Automatic line-break algorithm (Optimized in v2.7.0)**  
For long **English** labels, the button uses a **binary search** algorithm (up to 10 iterations) to find the optimal width, which is more efficient than the previous linear approach.

Algorithm logic:

1. Start by halving the text width (1/2)
2. Use binary search within the [1/2, original width] range
3. Ensure the text fits within two lines

!!! warning
    You can **force** a break by inserting `\n` in the text.  
    If `\n` is present, the automatic algorithm is **skipped**.

!!! tip
    In **single-line mode** any `\n` characters are **ignored**.

---

## Non-Wrap Mode (WPS style)

WPS keeps **everything on one line**; the arrow sits at the far right.

![wps-word-nowrodwrap](../../assets/pic/wps-word-nowrodwrap.png)

This mode is perfect for **short Chinese** labels, but may truncate **long English** words.  
If your UI is mainly English, prefer **word-wrap** mode; for Chinese, non-wrap looks cleaner.

Layout in `SARibbonToolButton`:

![ribbon-toolbutton-largebutton-nowordwrap](../../assets/pic/ribbon-toolbutton-largebutton-nowordwrap.png)

---

## Building a Visually Pleasing Ribbon

When **all labels are short** and **no menus** are attached, word-wrap mode leaves an **empty second line** on almost every button.  
Too much blank space can look unbalanced:

![office-excel-oneline-spacing](../../assets/pic/office-excel-oneline-spacing.png)

Remedies:

1. **Mix button sizes** – e.g. two large buttons + three small ones in the same row.  
2. **Force a break** with `\n`, especially for 4-character titles (two + two).

---

## Single-row Mode Button Rendering (v2.8.0+)

Starting from v2.8.0, SARibbon adds the single-row layout mode (`SingleRowMode`). In this mode:

- All buttons (Large/Medium/Small) use Small horizontal layout: icon on the left, text on the right
- `SARibbonBar::setEnableIconRightText(true)` forces all buttons to use horizontal icon-left, text-right layout (also usable in non-single-row modes)
- Text does not wrap; long text will be truncated

How to enable:

```cpp
// Method 1: Auto cascade via setRibbonStyle
ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleCompactSingleRow);
// setRibbonStyle automatically enables enableIconRightText in SingleRow mode

// Method 2: Enable enableIconRightText separately (works in any mode)
ribbonBar()->setEnableIconRightText(true);
```
