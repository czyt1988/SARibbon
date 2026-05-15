# Coding Standards and Comment Conventions

This specification defines style consistency requirements and comment formatting standards for all code in the SARibbon project, along with Git commit conventions. Following these standards ensures code maintainability, searchable documentation, and smooth team collaboration.

## Code Style

### Basic Principles

- **Strictly maintain consistency with existing code**: naming conventions, indentation, header file organization, etc.
- **Follow Qt development best practices**: use macros like `Q_PROPERTY`, `Q_SIGNALS`, `Q_SLOT`. Never use lowercase variants like `slot`, `signal` (see [Qt Integration Guide](qt-integration.md) for details).

### Naming Conventions

| Category | Convention | Example |
|----------|------------|---------|
| Class names | SARibbon prefix + PascalCase | `SARibbonBar`, `SARibbonCategory`, `SARibbonPanel`, `SARibbonToolButton` |
| Method names | camelCase, Qt style | `setRibbonStyle()`, `addCategoryPage()`, `addPanel()` |
| Property names | camelCase, Qt style | `ribbonStyle`, `categoryName`, `panelName` |
| Signals | xxxChanged pattern | `ribbonStyleChanged()`, `categoryNameChanged()`, `actionTriggered()` |
| Private data class | PrivateData | Inner class holding PIMPL implementation details |

## Comment Conventions (Mandatory)

All new code must use **Doxygen format** comments with bilingual (English/Chinese) support.

### Prohibited Patterns (Mandatory)

The following patterns are **prohibited** in all new code. AI-generated code is especially prone to violating these rules. Check each one carefully.

#### 1. Prohibited: Bilingual Doxygen blocks for header file public functions; use single-line English comments instead

```cpp
// WRONG: Bilingual Doxygen block for public function in header
/**
 * \if ENGLISH
 * @brief Gets the category name
 * @return The category name string
 * \endif
 *
 * \if CHINESE
 * @brief 获取类别名称
 * @return 类别名称字符串
 * \endif
 */
QString categoryName() const;

// CORRECT: Single-line English comment for public function in header
/// Get the category name
QString categoryName() const;
```

#### 2. Prohibited: Bilingual Doxygen comments on Q_PROPERTY; add no comments at all

```cpp
// WRONG: Bilingual Doxygen comment above Q_PROPERTY
/**
 * \if ENGLISH
 * @property SARibbonBar::ribbonStyle
 * @brief Ribbon style setting
 * @details Defines the visual style of the ribbon bar.
 * @accessors READ currentRibbonStyle WRITE setRibbonStyle
 * \endif
 *
 * \if CHINESE
 * @property SARibbonBar::ribbonStyle
 * @brief Ribbon样式设置
 * @details 定义Ribbon工具栏的视觉样式。
 * @accessors READ currentRibbonStyle WRITE setRibbonStyle
 * \endif
 */
Q_PROPERTY(RibbonStyles ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)

// CORRECT: Q_PROPERTY with no comments. Grouping comments are acceptable (e.g., // == Ribbon properties ==)
// == Ribbon properties ==
Q_PROPERTY(RibbonStyles ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)
Q_PROPERTY(bool minimumMode READ isMinimumMode WRITE setMinimumMode)
Q_PROPERTY(bool minimumModeButton READ haveShowMinimumModeButton WRITE showMinimumModeButton)
```

#### 3. Prohibited: Using @param, @class, @ingroup in class comments; only use @brief/@details/@note/@see

```cpp
// WRONG: Class comment contains @param, @class, @ingroup
/**
 * \if ENGLISH
 * @brief Ribbon category page containing multiple panels
 * @class SARibbonCategory
 * @ingroup ribbon_components
 * @details Each Category represents a tab page in the Ribbon...
 * @param[in] parent Parent QWidget (optional)
 * \endif
 *
 * \if CHINESE
 * @brief 包含多个面板的Ribbon类别页面
 * @class SARibbonCategory
 * @ingroup ribbon_components
 * @details 每个Category代表Ribbon中的一个标签页...
 * @param[in] parent 父QWidget对象（可选）
 * \endif
 */

// CORRECT: Class comment uses only @brief/@details/@note/@see, no @param/@class/@ingroup
/**
 * \if ENGLISH
 * @brief Ribbon category page containing multiple panels
 *
 * Each Category represents a tab page in the Ribbon, containing multiple panels (SARibbonPanel).
 * It acts as a container for organizing related actions and controls into logical groups.
 * @note Uses the PIMPL pattern via SA_RIBBON_DECLARE_PRIVATE for encapsulation.
 * @see SARibbonBar, SARibbonPanel, SARibbonToolButton
 * \endif
 *
 * \if CHINESE
 * @brief 包含多个面板的Ribbon类别页面
 *
 * 每个Category代表Ribbon中的一个标签页，包含多个面板（SARibbonPanel）。
 * 它作为一个容器，用于将相关的操作和控件组织成逻辑组。
 * @note 通过SA_RIBBON_DECLARE_PRIVATE采用PIMPL模式实现封装。
 * @see SARibbonBar, SARibbonPanel, SARibbonToolButton
 * \endif
 */
```

!!! danger "Mandatory Rules"
    - Header file public functions: no bilingual Doxygen, only single-line English `//` comments
    - Q_PROPERTY: no comments at all (including Doxygen blocks)
    - Class comments: no `@param`, `@class`, `@ingroup`; only `@brief`/`@details`/`@note`/`@see` allowed

### Source File (.cpp) Comment Conventions

Function implementations in source files must use complete bilingual Doxygen comment blocks:

```cpp
/**
 * \if ENGLISH
 * @brief [English brief description]
 * @param[in] param_name [English parameter description]
 * @return [English return value description]
 * @details [English detailed explanation]
 * \endif
 *
 * \if CHINESE
 * @brief [中文简要说明]
 * @param[in] param_name [中文参数描述]
 * @return [中文返回值描述]
 * @details [中文详细说明]
 * \endif
 */
```

!!! warning "Important"
    In principle, detailed function comments should be written in the corresponding `.cpp` file, not in the header file.

### Header File (.h) Comment Conventions

Header file comments follow these rules:

1. **`public` function declarations**: only add **single-line English brief comments** (using `//`)
2. **Prohibited**: bilingual Doxygen blocks for class member functions in header files
3. **Exception**: Qt signals (functions under `Q_SIGNALS` section) and class comments must use bilingual Doxygen in the header file

```cpp
// Below is the comment template for the real class SARibbonCategory (see src/SARibbonBar/SARibbonCategory.h)
/**
 * \if ENGLISH
 * @brief Ribbon category page containing multiple panels
 *
 * Each Category represents a tab page in the Ribbon, containing multiple panels (SARibbonPanel).
 * It acts as a container for organizing related actions and controls into logical groups.
 * @note Uses the PIMPL pattern via SA_RIBBON_DECLARE_PRIVATE for encapsulation.
 * @see SARibbonBar, SARibbonPanel, SARibbonToolButton
 * \endif
 *
 * \if CHINESE
 * @brief 包含多个面板的Ribbon类别页面
 *
 * 每个Category代表Ribbon中的一个标签页，包含多个面板（SARibbonPanel）。
 * 它作为一个容器，用于将相关的操作和控件组织成逻辑组。
 * @note 通过SA_RIBBON_DECLARE_PRIVATE采用PIMPL模式实现封装。
 * @see SARibbonBar, SARibbonPanel, SARibbonToolButton
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCategory : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonCategory)
    friend class SARibbonBar;
    friend class SARibbonContextCategory;

    // Q_PROPERTY has no comments!
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
    Q_PROPERTY(QString categoryName READ categoryName WRITE setCategoryName)

public:
    // Constructor
    explicit SARibbonCategory(QWidget* p = nullptr);
    // Constructor with name
    explicit SARibbonCategory(const QString& name, QWidget* p = nullptr);
    // Destructor
    ~SARibbonCategory();

    // Get the category name
    QString categoryName() const;

    // Set the category name
    void setCategoryName(const QString& title);

    // Add a panel with title
    SARibbonPanel* addPanel(const QString& title);

Q_SIGNALS:
    /**
     * \if ENGLISH
     * @brief Emitted when category name changes
     * @param title New category name
     * \endif
     *
     * \if CHINESE
     * @brief 标签名改变触发的信号
     * @param title 新的标签名
     * \endif
     */
    void categoryNameChanged(const QString& title);
};
```

**Emphasized again:** Prohibited to write bilingual Doxygen blocks for **class member functions** in header files.

### Class Doxygen Comment Conventions

Class Doxygen comments must be added in the **header file** following bilingual requirements. **Only** `@brief`/`@details`/`@note`/`@see` are allowed. `@param`/`@class`/`@ingroup` are prohibited:

```cpp
/**
 * \if ENGLISH
 * @brief SARibbonBar inherits from QMenuBar and directly replaces the original QMenuBar in SARibbonMainWindow
 *
 * Set the ribbon style using the setRibbonStyle function:
 *
 * @code
 * void setRibbonStyle(RibbonStyles v);
 * @endcode
 *
 * SARibbonBar references Office and WPS, providing four styles of Ribbon modes, @ref SARibbonBar::RibbonStyles
 *
 * If you want the ribbon to occupy minimal space, WpsLiteStyleTwoRow mode can save 35% height space compared to OfficeStyle
 * @note Uses the PIMPL pattern via SA_RIBBON_DECLARE_PRIVATE for encapsulation.
 * @see SARibbonCategory, SARibbonPanel, SARibbonToolButton
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonBar继承于QMenuBar,在SARibbonMainWindow中直接替换了原来的QMenuBar
 *
 * 通过setRibbonStyle函数设置ribbon的风格:
 *
 * @code
 * void setRibbonStyle(RibbonStyles v);
 * @endcode
 *
 * SARibbonBar参考office和wps，提供了四种风格的Ribbon模式,@ref SARibbonBar::RibbonStyles
 *
 * 如果想ribbon占用的空间足够小，WpsLiteStyleTwoRow模式能比OfficeStyle节省35%的高度空间
 * @note 通过SA_RIBBON_DECLARE_PRIVATE采用PIMPL模式实现封装。
 * @see SARibbonCategory, SARibbonPanel, SARibbonToolButton
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonBar : public QMenuBar
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonBar)

    // Q_PROPERTY has no comments!
    Q_PROPERTY(RibbonStyles ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)
    Q_PROPERTY(bool minimumMode READ isMinimumMode WRITE setMinimumMode)

public:
    /// Check if the ribbon style is two-row mode
    static bool isTwoRowStyle(RibbonStyles s);

    /// Add a category page
    SARibbonCategory* addCategoryPage(const QString& title);
};
```

!!! tip "Tip"
    For functionally focused classes, include usage examples in the class comments so users understand how to use them.

### Signal Doxygen Comment Conventions

Qt signals have no definitions in `.cpp` files, so signal Doxygen comments must be added in the **header file** using bilingual format:

```cpp
Q_SIGNALS:
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
```

### Comment Location Summary

| Comment Type | Location | Format |
|--------------|----------|--------|
| Detailed function comments | `.cpp` | Bilingual Doxygen block |
| Public function brief comments | `.h` | Single-line English `///` |
| Class comments | `.h` | Bilingual Doxygen block (only @brief/@details/@note/@see) |
| Signal comments | `.h` | Bilingual Doxygen block |
| **Q_PROPERTY comments** | **Prohibited** | **No comments at all** |

## Git Commit Conventions

After completing a task, commit all changes to the Git repository. Write meaningful commit messages so future work can clearly understand what was implemented.

### Commit Message Format

Commit messages should ideally contain:

1. **Task type** (e.g., feature, fix, docs update)
2. **Brief description of what was implemented**
3. **List of relevant files**
4. **关联到计划书** (if applicable)

### Commit Message Example

```
修复：SARibbonCategory布局计算错误

- 修复了在紧凑模式下面板高度计算不正确的问题
- 调整了SARibbonCategoryLayout中的minimumSizeHint计算
- 更新了SARibbonPanel的sizeHint以适应新的布局逻辑
- 添加了对TwoRowMode的特殊处理

关联计划：Ribbon布局优化计划
```

## References

- Related guides: [Qt Integration Guide](qt-integration.md), [PIMPL Development Guide](pimpl-dev-guide.md)
- Source location: `src/SARibbonBar/`
- Core header: `SARibbonGlobal.h` (contains macro definitions like SA_RIBBON_EXPORT, SA_RIBBON_DECLARE_PRIVATE)
