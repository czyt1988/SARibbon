# Qt Integration Guide

SARibbon is a Qt Ribbon UI control library that directly inherits from Qt Widgets classes (QMenuBar, QFrame, QToolButton, etc.) to provide Microsoft Office-like Ribbon interfaces for Qt applications. This document describes how SARibbon integrates with the Qt framework.

## Overview

SARibbon fully leverages Qt's signal-slot mechanism, property system, and meta-object system. All UI interactions use Qt-native approaches for event communication. The class inheritance hierarchy is as follows:

- `SARibbonBar` inherits `QMenuBar`
- `SARibbonCategory` inherits `QFrame`
- `SARibbonPanel` inherits `QFrame`
- `SARibbonToolButton` inherits `QToolButton`
- `SARibbonMainWindow` inherits `QMainWindow`

## Signal-Slot Design

### Design Principles

SARibbon fully leverages Qt's signal-slot mechanism, with all UI interactions using signals and slots for event communication. Every important state change triggers a corresponding signal, enabling external objects to listen and respond.

### Signal Naming Convention

SARibbon signal names follow Qt conventions, using descriptive names that reflect the event type:

| Signal | Description |
|------|------|
| `currentRibbonTabChanged(int index)` | Tab page changed |
| `ribbonModeChanged(SARibbonBar::RibbonMode nowState)` | Ribbon mode changed (minimized/normal) |
| `ribbonStyleChanged(SARibbonBar::RibbonStyles nowStyle)` | Ribbon style changed |
| `applicationButtonClicked()` | Application button clicked |
| `actionTriggered(QAction* action)` | Action triggered |
| `titleBarHeightChanged(int oldHeight, int newHeight)` | Title bar height changed |
| `categoryNameChanged(const QString& n)` | Category name changed |
| `panelNameChanged(const QString& n)` | Panel name changed |

### Typical Signal Comments

Signal comments in header files use the bilingual `@brief` Doxygen format:

```cpp
Q_SIGNALS:
    /**
     * \if ENGLISH
     * @brief Application button click response - the button in the top-left corner
     *
     * For example, clicking the button can pop up a full-screen window (like in Office)
     * \endif
     *
     * \if CHINESE
     * @brief 应用按钮点击响应 - 左上角的按钮，通过关联此信号触发应用按钮点击的效果
     *
     * 例如想点击按钮后弹出一个全屏的窗口（如office这些）
     * \endif
     */
    void applicationButtonClicked();

    /**
     * \if ENGLISH
     * @brief Signal triggered when tab page changes
     * @param index New tab index
     * \endif
     *
     * \if CHINESE
     * @brief 标签页变化触发的信号
     * @param index 新的标签页索引
     * \endif
     */
    void currentRibbonTabChanged(int index);

    /**
     * \if ENGLISH
     * @brief Signal triggered when ribbon state changes
     * @param nowState New ribbon state after change
     * \endif
     *
     * \if CHINESE
     * @brief ribbon的状态发生了变化后触发此信号
     * @param nowState 变更之后的ribbon状态
     * \endif
     */
    void ribbonModeChanged(SARibbonBar::RibbonMode nowState);

    /**
     * \if ENGLISH
     * @brief Signal triggered when ribbon style changes
     * @param nowStyle New ribbon style after change
     * \endif
     *
     * \if CHINESE
     * @brief ribbon的状态发生了变化后触发此信号
     * @param nowStyle 变更之后的ribbon样式
     * \endif
     */
    void ribbonStyleChanged(SARibbonBar::RibbonStyles nowStyle);

    /**
     * \if ENGLISH
     * @brief Signal triggered when title bar height changes
     * @param oldHeight Old title bar height
     * @param newHeight New title bar height
     * \endif
     *
     * \if CHINESE
     * @brief 标题栏高度发生了变化的信号
     * @param oldHeight 旧的标题栏高度
     * @param newHeight 新的标题栏高度
     * \endif
     */
    void titleBarHeightChanged(int oldHeight, int newHeight);

    /**
     * \if ENGLISH
     * @brief Signal similar to QToolBar::actionTriggered
     * @param action Triggered action
     * \endif
     *
     * \if CHINESE
     * @brief 参考QToolBar::actionTriggered的信号
     * @param action 触发的action
     * \endif
     */
    void actionTriggered(QAction* action);
```

## Q_PROPERTY Attribute Exposure

### Design Principles

SARibbon classes use `Q_PROPERTY` to expose configuration attributes, making properties accessible via the Qt meta-object system (QMetaObject). This supports QSS styling and potential future QML integration.

### Property Declaration Pattern

**Properties in SARibbonBar:**

```cpp
class SA_RIBBON_EXPORT SARibbonBar : public QMenuBar
{
    Q_OBJECT
    Q_PROPERTY(RibbonStyles ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)
    Q_PROPERTY(bool minimumMode READ isMinimumMode WRITE setMinimumMode)
    Q_PROPERTY(QColor windowTitleTextColor READ windowTitleTextColor WRITE setWindowTitleTextColor)
    Q_PROPERTY(Qt::Alignment windowTitleAligment READ windowTitleAligment WRITE setWindowTitleAligment)
    Q_PROPERTY(bool enableWordWrap READ isEnableWordWrap WRITE setEnableWordWrap)
    Q_PROPERTY(SARibbonPanel::PanelLayoutMode panelLayoutMode READ panelLayoutMode WRITE setPanelLayoutMode)
    // ...
};
```

**Properties in SARibbonCategory:**

```cpp
class SA_RIBBON_EXPORT SARibbonCategory : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(QString categoryName READ categoryName WRITE setCategoryName)
    // ...
};
```

**Properties in SARibbonPanel:**

```cpp
class SA_RIBBON_EXPORT SARibbonPanel : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(bool isExpanding READ isExpanding WRITE setExpanding)
    Q_PROPERTY(QString panelName READ panelName WRITE setPanelName)
    // ...
};
```

**Properties in SARibbonMainWindow:**

```cpp
class SA_RIBBON_EXPORT SARibbonMainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(SARibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)
    // ...
};
```

### Property Naming Convention

SARibbon property naming follows Qt conventions:

| Property type | getter | setter | signal |
|----------|--------|--------|------|
| General (QColor, etc.) | `windowTitleTextColor()` | `setWindowTitleTextColor()` | No direct signal |
| Boolean | `isMinimumMode()` | `setMinimumMode()` | `ribbonModeChanged()` |
| Enum | `currentRibbonStyle()` | `setRibbonStyle()` | `ribbonStyleChanged()` |
| String | `categoryName()` | `setCategoryName()` | `categoryNameChanged()` |
| Alignment | `windowTitleAligment()` | `setWindowTitleAligment()` | No direct signal |

!!! tip "Boolean Property Naming"
    SARibbon boolean property getters use the `is*` prefix:
    - `isMinimumMode()`
    - `isCanCustomize()`
    - `isExpanding()`
    - `isEnableWordWrap()`
    - `isTabOnTitle()`

## Qt Signal-Slot Macro Usage

### Mandatory Rules

**Lowercase Qt signal-slot macros are forbidden**. Always use the uppercase versions:

| Forbidden (lowercase) | Required (uppercase) | Description |
|-------------|----------------|------|
| `slots` | `Q_SLOTS` | Slot declaration block |
| `signals` | `Q_SIGNALS` | Signal declaration block |
| `emit` | `Q_EMIT` | Emit a signal |

### Example

```cpp
class SA_RIBBON_EXPORT SARibbonBar : public QMenuBar
{
    Q_OBJECT

protected Q_SLOTS:  // Correct: uppercase macros
    /// Slot for window title changed
    void onWindowTitleChanged(const QString& title);

Q_SIGNALS:  // Correct: uppercase macros
    void currentRibbonTabChanged(int index);
};

void SARibbonBar::onWindowTitleChanged(const QString& title)
{
    // ...
    Q_EMIT currentRibbonTabChanged(currentIndex());  // Correct: uppercase macros
}
```

### Q_SLOTS Visibility Patterns

SARibbon uses different Q_SLOTS visibility levels:

```cpp
// SARibbonContextCategory.h
class SA_RIBBON_EXPORT SARibbonContextCategory : public QObject
{
    Q_OBJECT
public Q_SLOTS:  // Public slots
    void hide();
    void show();
};

// SARibbonGallery.h
class SA_RIBBON_EXPORT SARibbonGallery : public QFrame
{
    Q_OBJECT
public Q_SLOTS:  // Public slots
    virtual void pageUp();
    virtual void pageDown();
    virtual void showMoreDetail();
protected Q_SLOTS:  // Protected slots
    void onItemClicked(const QModelIndex& index);
    virtual void onTriggered(QAction* action);
};

// SARibbonBar.h
class SA_RIBBON_EXPORT SARibbonBar : public QMenuBar
{
    Q_OBJECT
protected Q_SLOTS:  // Protected slots
    void onWindowTitleChanged(const QString& title);
    void onWindowIconChanged(const QIcon& i);
    void onCategoryWindowTitleChanged(const QString& title);
    virtual void onCurrentRibbonTabChanged(int index);
    void onTabMoved(int from, int to);
};

// SARibbonMainWindow.h
class SA_RIBBON_EXPORT SARibbonMainWindow : public QMainWindow
{
    Q_OBJECT
private Q_SLOTS:  // Private slots
    void onPrimaryScreenChanged(QScreen* screen);
};
```

!!! warning "Forbidden"
    ```cpp
    slots:      // Error: lowercase forbidden
    signals:    // Error: lowercase forbidden
    emit xxx(); // Error: lowercase forbidden
    ```

## References

- [Coding Standards](coding-standards.md)
- [PIMPL Development Guide](pimpl-dev-guide.md)
