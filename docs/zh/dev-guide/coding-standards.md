# 代码风格与注释规范

本规范规定了 SARibbon 项目中所有代码的风格一致性要求和注释格式标准，同时包含 Git 提交规范。遵循这些规范确保代码可维护、注释可检索、团队协作顺畅。

## 代码风格

### 基本原则

- **严格保持与现有代码一致**：命名规范、缩进、头文件组织等
- **遵循 Qt 开发最佳实践**：使用 `Q_PROPERTY`、`Q_SIGNALS`、`Q_SLOT` 等宏，禁止使用 `slot`、`signal` 等小写命名的宏（详见 [Qt集成规范](qt-integration.md)）

### 命名规范

| 类别 | 规范 | 示例 |
|------|------|------|
| 类名 | SARibbon前缀 + 大驼峰 | `SARibbonBar`、`SARibbonCategory`、`SARibbonPanel`、`SARibbonToolButton` |
| 方法名 | 小驼峰，Qt风格 | `setRibbonStyle()`、`addCategoryPage()`、`addPanel()` |
| 属性名 | 小驼峰，Qt风格 | `ribbonStyle`、`categoryName`、`panelName` |
| 信号 | xxxChanged模式 | `ribbonStyleChanged()`、`categoryNameChanged()`、`actionTriggered()` |
| 私有数据类 | PrivateData | 内部类，存放PIMPL实现细节 |

## 注释规范（强制）

所有新增代码必须使用 **Doxygen 格式**，并区分中英文。

### 禁止模式（强制）

以下模式**禁止**出现在任何新增代码中。AI 生成代码尤其容易违反这些规则，务必逐条检查。

#### 1. 头文件 public 函数双语 Doxygen 禁止，单行英文注释

```cpp
// 禁止：头文件 public 函数加双语 Doxygen 块
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

// 正确：头文件 public 函数仅用单行英文注释
/// Get the category name
QString categoryName() const;
```

#### 2. Q_PROPERTY 加双语 Doxygen 注释禁止，不加任何注释

```cpp
// 禁止：Q_PROPERTY 上方加双语 Doxygen 注释
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

// 正确：Q_PROPERTY 不加任何注释，仅可用分组注释（如 // == Ribbon properties ==）
// == Ribbon properties ==
Q_PROPERTY(RibbonStyles ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)
Q_PROPERTY(bool minimumMode READ isMinimumMode WRITE setMinimumMode)
Q_PROPERTY(bool minimumModeButton READ haveShowMinimumModeButton WRITE showMinimumModeButton)
```

#### 3. 类注释中使用 @param、@class、@ingroup 禁止，仅用 @brief/@details/@note/@see

```cpp
// 禁止：类注释包含 @param、@class、@ingroup
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

// 正确：类注释仅使用 @brief/@details/@note/@see，禁止 @param/@class/@ingroup
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

!!! danger "强制规则"
    - 头文件 public 函数：禁止双语 Doxygen，只能用单行英文 `//`
    - Q_PROPERTY：禁止加任何注释（包括 Doxygen 块）
    - 类注释：禁止 `@param`、`@class`、`@ingroup`，仅允许 `@brief`/`@details`/`@note`/`@see`

### 源文件（.cpp）注释规范

源文件中的函数实现必须使用完整的中英文双块 Doxygen 注释：

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

!!! warning "重要"
    原则上详细函数注释应该写在对应的 `.cpp` 文件中，而不是头文件中。

### 头文件（.h）注释规范

头文件中的注释遵循以下规则：

1. **`public` 函数声明**：仅添加**单行英文简要注释**（使用 `//`）
2. **禁止**在头文件的类成员函数中写入双语 Doxygen 块
3. **特例**：Qt 信号（`Q_SIGNALS` 区域下的函数）和类的注释需在头文件中使用双语 Doxygen

```cpp
// 以下为真实类 SARibbonCategory 的注释模板（参见 src/SARibbonBar/SARibbonCategory.h）
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

    // Q_PROPERTY 不加注释！
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

**再次强调：**禁止在头文件的**类成员函数**中写入双语 Doxygen 块

### 类的 doxygen 注释规范

类的 doxygen 注释需要在**头文件**中按双语要求添加，**仅允许** `@brief`/`@details`/`@note`/`@see`，禁止 `@param`/`@class`/`@ingroup`：

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

    // Q_PROPERTY 不加注释！
    Q_PROPERTY(RibbonStyles ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)
    Q_PROPERTY(bool minimumMode READ isMinimumMode WRITE setMinimumMode)

public:
    /// Check if the ribbon style is two-row mode
    static bool isTwoRowStyle(RibbonStyles s);

    /// Add a category page
    SARibbonCategory* addCategoryPage(const QString& title);
};
```

!!! tip "技巧"
    对于功能性较强的类，类的注释中应该加入使用示例，以便使用者了解如何使用。

### 信号的 doxygen 注释规范

Qt 信号没有 `.cpp` 中的定义，因此信号的 doxygen 注释需要在**头文件**中使用双语格式添加：

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

### 注释位置总结

| 注释类型 | 位置 | 格式 |
|----------|------|------|
| 函数详细注释 | `.cpp` | 双语 Doxygen 块 |
| public 函数简要注释 | `.h` | 单行英文 `///` |
| 类注释 | `.h` | 双语 Doxygen 块（仅 @brief/@details/@note/@see） |
| 信号注释 | `.h` | 双语 Doxygen 块 |
| **Q_PROPERTY 注释** | **禁止** | **不加任何注释** |

## Git 提交规范

在完成当前任务后，需提交所有更改到 Git 仓库。创建有意义的提交信息保证下次任务能清楚了解这次任务的实现情况。

### 提交信息格式

提交信息最好包含以下信息：

1. **任务类型**（例如：实现、修复、文档更新）
2. **实现内容的简要描述**
3. **相关文件列表**
4. **关联到计划书**（如果适用）

### 提交信息示例

```
修复：SARibbonCategory布局计算错误

- 修复了在紧凑模式下面板高度计算不正确的问题
- 调整了SARibbonCategoryLayout中的minimumSizeHint计算
- 更新了SARibbonPanel的sizeHint以适应新的布局逻辑
- 添加了对TwoRowMode的特殊处理

关联计划：Ribbon布局优化计划
```

## 参考

- 相关规范：[Qt集成规范](qt-integration.md)、[PIMPL开发规范](pimpl-dev-guide.md)
- 源码位置：`src/SARibbonBar/`
- 核心头文件：`SARibbonGlobal.h`（包含SA_RIBBON_EXPORT、SA_RIBBON_DECLARE_PRIVATE等宏定义）
