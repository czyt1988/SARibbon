# Ribbon界面的层次结构

理解 SARibbon 的层次结构是构建界面的基础。Ribbon 采用严格的四层嵌套模型：**RibbonBar → Category → Panel → ToolButton/Widget**，每一层都有明确的职责和 API。

## 主要功能特性

**特性**

- ✅ **四层嵌套布局**：SARibbonBar 管理 Category，Category 管理 Panel，Panel 管理 ToolButton 和 Widget
- ✅ **上下文分类页**：SARibbonContextCategory 支持按条件显示/隐藏特定标签组（如选中对象时出现"图片工具"标签）
- ✅ **画廊组件**：SARibbonGallery 提供网格形式的选项选择器，支持弹出式分组浏览
- ✅ **快速访问栏与右侧按钮组**：内置 SARibbonQuickAccessBar 和 SARibbonButtonGroupWidget，支持高频操作的一键访问
- ✅ **多层级导航查找**：按名称、索引、ObjectName 在各层级间快速定位组件

## 层次关系图

### 继承关系

SARibbonBar 继承自 QMenuBar，在 SARibbonMainWindow 中替代了传统菜单栏。其余所有 SARibbon 组件均继承自 QWidget 或其子类（QFrame、QTabBar 等）。继承关系仅此一处，下文重点讨论包含（组合）关系。

### 包含关系（核心层次结构）

Ribbon 界面的构建遵循严格的四层包含模型。为了清晰展示，将包含关系拆分为三个子图。

#### 1. 主链路 — 四层包含层次

从窗口到按钮的核心包含链路：

```mermaid
flowchart TD
    MW["SARibbonMainWindow<br/>顶级窗口容器"] -->|"ribbonBar()"| BAR["SARibbonBar<br/>Ribbon 栏（继承 QMenuBar）"]
    BAR -->|"addCategoryPage()"| CAT["SARibbonCategory<br/>分类页（如：开始、插入）"]
    CAT -->|"addPanel()"| PNL["SARibbonPanel<br/>功能面板（如：字体、段落）"]
    PNL -->|"addLargeAction()"| BTN["SARibbonToolButton<br/>工具按钮（自动包装 QAction）"]
    PNL -->|"addWidget()"| WGT["QWidget<br/>任意自定义控件"]
    PNL -->|"addGallery()"| GAL["SARibbonGallery<br/>画廊选择器"]

    classDef layer1 fill:#e1f5fe,stroke:#01579b,stroke-width:2px
    classDef layer2 fill:#fff3e0,stroke:#e65100,stroke-width:2px
    classDef layer3 fill:#f3e5f5,stroke:#4a148c,stroke-width:2px
    classDef layer4 fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px

    class MW layer1
    class BAR layer2
    class CAT,PNL layer3
    class BTN,WGT,GAL layer4
```

#### 2. SARibbonBar 的辅助组件

SARibbonBar 除管理 Category 外，还直接包含以下辅助组件：

```mermaid
flowchart LR
    BAR["SARibbonBar"] --> QA["SARibbonQuickAccessBar<br/>quickAccessBar()"]
    BAR --> RB["SARibbonButtonGroupWidget<br/>rightButtonGroup()"]
    BAR --> TAB["SARibbonTabBar<br/>ribbonTabBar()"]
    BAR --> APP["QAbstractButton<br/>applicationButton()"]

    QA -. "继承" .-> RB

    classDef bar fill:#fff3e0,stroke:#e65100,stroke-width:2px
    classDef aux fill:#e8f5e9,stroke:#2e7d32,stroke-width:1px

    class BAR bar
    class QA,RB,TAB,APP aux
```

#### 3. 上下文分类 — 条件触发的标签页

`SARibbonContextCategory` 是一种特殊的分类管理器，条件触发时动态显示/隐藏标签页：

```mermaid
flowchart TD
    BAR["SARibbonBar<br/>ribbonBar()"] -->|"addContextCategory()"| CTX["SARibbonContextCategory<br/>上下文分类组<br/>（如：绘图工具）"]
    CTX -->|"addCategoryPage()"| CTXCAT["SARibbonCategory<br/>条件触发的分类页<br/>（如：格式）"]
    CTXCAT -->|"addPanel()"| CTXPNL["SARibbonPanel<br/>面板"]

    classDef bar fill:#fff3e0,stroke:#e65100,stroke-width:2px
    classDef ctx fill:#fff4e6,stroke:#e67e22,stroke-width:2px

    class BAR bar
    class CTX,CTXCAT,CTXPNL ctx
```

上下文分类默认隐藏，通过 `showContextCategory()` / `hideContextCategory()` 控制显隐。

**图例说明：**
- 实线箭头表示**包含关系**（父组件持有并管理子组件）
- 虚线箭头表示**继承关系**（仅在辅助组件图中出现）
- 橙色节点表示上下文分类相关组件（条件触发，默认隐藏）
- 蓝色→橙色→紫色→绿色 对应四层包含层次（第一层→第四层）

包含关系说明了 Ribbon 的构建方向：从 `SARibbonMainWindow` 获取 `SARibbonBar`，然后逐层添加 Category、Panel，最后放入按钮和控件。

## 组件查找表

下表列出了 Ribbon 层次结构中每个核心组件的类名、职责和创建/获取方式：

| 层次 | 类名 | 职责 | 创建/获取方式 |
|------|------|------|--------------|
| 窗口容器 | `SARibbonMainWindow` | Ribbon 应用的顶级窗口，替代 QMainWindow | 继承此类作为主窗口 |
| Ribbon 栏 | `SARibbonBar` | 顶层管理器，替代 QMenuBar | `ribbonBar()` 从主窗口获取 |
| 标签栏 | `SARibbonTabBar` | 显示 Category 标签页的导航栏 | `ribbonTabBar()` 获取 |
| 分类页 | `SARibbonCategory` | 一个功能场景（如"主页"、"插入"） | `addCategoryPage()` 添加 |
| 上下文分类 | `SARibbonContextCategory` | 条件触发的分类组（如"图表工具"） | `addContextCategory()` 添加 |
| 面板 | `SARibbonPanel` | Category 内的功能分组容器 | `category->addPanel()` 添加 |
| 工具按钮 | `SARibbonToolButton` | 承载 QAction 的 Ribbon 专用按钮 | `panel->addLargeAction()` 等自动创建 |
| 画廊 | `SARibbonGallery` | 网格形式的选项选择器 | `panel->addGallery()` 添加 |
| 快速访问栏 | `SARibbonQuickAccessBar` | 顶部高频命令工具栏 | `quickAccessBar()` 获取 |
| 右侧按钮组 | `SARibbonButtonGroupWidget` | 右上角功能按钮组 | `rightButtonGroup()` 获取 |
| 应用按钮 | `QAbstractButton` | 左上角应用菜单入口 | `applicationButton()` 获取 |

## 层次结构文字说明

Ribbon 界面采用严格的**四层包含模型**。理解每一层的包含关系，是构建 Ribbon 界面的关键。

### 第一层：SARibbonBar（Ribbon 栏）

`SARibbonBar` 是 Ribbon 的顶层管理器（继承自 QMenuBar），在 `SARibbonMainWindow` 中完全替代了传统菜单栏。通过 `ribbonBar()` 方法从主窗口获取。

SARibbonBar 不仅管理 Category 的创建与切换，还直接包含以下辅助组件：

| 组件 | 获取方法 | 职责 |
|------|----------|------|
| `SARibbonTabBar` | `ribbonTabBar()` | 显示 Category 标签页的导航栏 |
| `SARibbonQuickAccessBar` | `quickAccessBar()` | 顶部高频命令快速访问栏 |
| `SARibbonButtonGroupWidget` | `rightButtonGroup()` | 右上角功能按钮组 |
| `QAbstractButton` | `applicationButton()` | 左上角应用菜单入口 |

### 第二层：SARibbonCategory（分类页）与 SARibbonContextCategory（上下文分类）

`SARibbonCategory` 代表一个完整的功能场景，例如 Word 中的"开始"、"插入"、"设计"。通过 `addCategoryPage()` 添加到 SARibbonBar 中。每个 Category 可以包含多个 Panel，并且通过 `SARibbonTabBar` 显示为用户可见的标签页。

`SARibbonContextCategory` 是一种特殊的分类管理器（继承自 QObject），它内部可以管理多个 `SARibbonCategory`。上下文分类通常用于特定条件触发，例如在 Word 中选中图片后才出现"图片工具"标签。通过 `show()`/`hide()` 控制其显隐。

### 第三层：SARibbonPanel（功能面板）

`SARibbonPanel` 是 Category 内的功能分组容器，通过 `category->addPanel()` 创建。每个 Panel 可放置以下内容：

- **QAction**：通过 `addLargeAction`、`addMediumAction`、`addSmallAction` 添加到 Panel，Panel 自动将其包装为 `SARibbonToolButton`
- **QWidget**：通过 `addWidget` 添加任意自定义控件（如组合框、编辑框、微调框等）
- **SARibbonGallery**：通过 `addGallery` 添加网格形式的选项选择器

Panel 支持两行和三行两种布局模式，分别对应 WPS 风格的紧凑布局和 Office 风格的宽松布局。

### 第四层：SARibbonToolButton / QWidget / SARibbonGallery（具体控件）

第四层是最终呈现给用户交互的具体控件：

- **SARibbonToolButton**：专为 Ribbon 设计的工具按钮，支持 Large/Small 两种尺寸模式。图标尺寸会根据按钮尺寸动态自适应，不能通过 `setIconSize` 手动设置。大按钮模式还支持文字自动换行以优化空间利用。
- **QWidget**：开发者可嵌入任意 Qt 控件，实现自定义功能区域。
- **SARibbonGallery**：网格形式的选项选择器，支持弹出式分组浏览，适合用在"样式选择"、"主题预览"等场景。

## 各层次导航方法

SARibbon 在各层级均提供了按名称、按索引、按 ObjectName 的查找方法，便于在复杂界面中定位组件。

### SARibbonBar 层级查找

| 方法 | 返回值 | 说明 |
|------|--------|------|
| `categoryByIndex(int index)` | `SARibbonCategory*` | 按索引获取 Category，越界返回 nullptr |
| `categoryByName(const QString& title)` | `SARibbonCategory*` | 按名称查找 Category |
| `categoryByObjectName(const QString& objname)` | `SARibbonCategory*` | 按 ObjectName 查找 Category |
| `categoryIndex(const SARibbonCategory* c)` | `int` | 获取 Category 在标签栏中的索引 |
| `categoryPages(bool getAll)` | `QList<SARibbonCategory*>` | 获取所有 Category 列表 |
| `iterateCategory(FpCategoryIterate fp)` | `bool` | 遍历所有 Category，回调返回 false 时停止 |
| `iteratePanel(FpPanelIterate fp)` | `bool` | 跨 Category 遍历所有 Panel |

### SARibbonCategory 层级查找

| 方法 | 返回值 | 说明 |
|------|--------|------|
| `panelByIndex(int index)` | `SARibbonPanel*` | 按索引获取 Panel，越界返回 nullptr |
| `panelByName(const QString& title)` | `SARibbonPanel*` | 按名称查找 Panel |
| `panelByObjectName(const QString& objname)` | `SARibbonPanel*` | 按 ObjectName 查找 Panel |
| `panelIndex(SARibbonPanel* p)` | `int` | 获取 Panel 在 Category 中的索引 |
| `panelList()` | `QList<SARibbonPanel*>` | 获取所有 Panel 列表 |
| `panelCount()` | `int` | 获取 Panel 数量 |
| `iteratePanel(FpPanelIterate fp)` | `bool` | 遍历所有 Panel |

### SARibbonPanel 层级查找

| 方法 | 返回值 | 说明 |
|------|--------|------|
| `actionToRibbonToolButton(QAction* action)` | `SARibbonToolButton*` | 通过 QAction 查找对应的 ToolButton |
| `actionIndex(QAction* act)` | `int` | 获取 Action 在 Panel 中的布局索引 |
| `ribbonToolButtons()` | `QList<SARibbonToolButton*>` | 获取所有按钮列表 |
| `iterateButton(FpRibbonToolButtonIterate fp)` | `bool` | 遍历所有按钮 |
| `lastAddActionButton()` | `SARibbonToolButton*` | 获取最后添加的按钮 |

!!! info "导航提示"

    `SARibbonPanel` 的方法（如 `actionToRibbonToolButton`）返回的按钮是由 Panel 自动创建管理的，开发者无需手动 new 这些按钮，只需创建 QAction 并通过 `addLargeAction`/`addSmallAction` 等方法添加到 Panel 即可。

## 完整代码示例

下面演示从一个继承 `SARibbonMainWindow` 的主窗口出发，如何逐层构建完整的 Ribbon 界面：

```cpp
// mainwindow.h
#include "SARibbonMainWindow.h"
class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
private:
    void setupRibbon();
};
```

上述头文件声明了主窗口类。它必须继承 `SARibbonMainWindow`，这是使用 SARibbon 的前提条件。

```cpp
// mainwindow.cpp
#include "mainwindow.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPanel.h"
#include "SARibbonGallery.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonContextCategory.h"
#include <QAction>
#include <QToolBar>

MainWindow::MainWindow(QWidget* parent) : SARibbonMainWindow(parent)
{
    setupRibbon();
}
```

构造函数中直接调用 `setupRibbon()` 初始化 Ribbon 界面。

```cpp
void MainWindow::setupRibbon()
{
    // 步骤1：获取 RibbonBar（第一层）
    SARibbonBar* ribbon = ribbonBar();
    ribbon->setRibbonStyle(SARibbonBar::RibbonStyleLooseThreeRow);
    ribbon->setWindowTitleAligment(Qt::AlignCenter);

    // 步骤2：创建 Category 分类页（第二层）
    SARibbonCategory* categoryHome = ribbon->addCategoryPage(tr("开始"));
    categoryHome->setObjectName("categoryHome");

    SARibbonCategory* categoryInsert = ribbon->addCategoryPage(tr("插入"));
    categoryInsert->setObjectName("categoryInsert");

    // 步骤3：在"开始"Category 中创建 Panel 面板（第三层）
    SARibbonPanel* panelFile = categoryHome->addPanel(tr("文件操作"));
    SARibbonPanel* panelEdit = categoryHome->addPanel(tr("编辑操作"));

    // 步骤4：在 Panel 中添加 Action 按钮（第四层）
    QAction* actNew = new QAction(QIcon(":/icons/new.svg"), tr("新建"), this);
    actNew->setObjectName("actNew");
    panelFile->addLargeAction(actNew);

    QAction* actOpen = new QAction(QIcon(":/icons/open.svg"), tr("打开"), this);
    actOpen->setObjectName("actOpen");
    panelFile->addLargeAction(actOpen);

    QAction* actSave = new QAction(QIcon(":/icons/save.svg"), tr("保存"), this);
    actSave->setObjectName("actSave");
    panelFile->addMediumAction(actSave);

    QAction* actUndo = new QAction(QIcon(":/icons/undo.svg"), tr("撤销"), this);
    actUndo->setObjectName("actUndo");
    panelEdit->addSmallAction(actUndo);

    QAction* actRedo = new QAction(QIcon(":/icons/redo.svg"), tr("重做"), this);
    actRedo->setObjectName("actRedo");
    panelEdit->addSmallAction(actRedo);

    // 步骤5：添加 Gallery 画廊组件
    SARibbonGallery* gallery = panelInsert->addGallery();
    QList<QAction*> galleryActions;
    // ... 添加 galleryActions ...
    gallery->addCategoryActions(tr("样式"), galleryActions);

    // 步骤6：配置快速访问栏
    SARibbonQuickAccessBar* quickBar = ribbon->quickAccessBar();
    quickBar->addAction(actSave);
    quickBar->addAction(actUndo);

    // 步骤7：配置右侧按钮组
    SARibbonButtonGroupWidget* rightGroup = ribbon->rightButtonGroup();
    QAction* actHelp = new QAction(QIcon(":/icons/help.svg"), tr("帮助"), this);
    rightGroup->addAction(actHelp);

    // 步骤8：创建上下文分类（条件触发显示）
    SARibbonContextCategory* ctxCategory = ribbon->addContextCategory(
        tr("绘图工具"), QColor(255, 100, 50));
    SARibbonCategory* ctxPage = ctxCategory->addCategoryPage(tr("格式"));
    SARibbonPanel* ctxPanel = ctxPage->addPanel(tr("形状"));
    // ... 为 ctxPanel 添加 actions ...
    ribbon->hideContextCategory(ctxCategory);  // 默认隐藏
}
```

上述代码完整演示了从获取 `SARibbonBar` 到创建 Category、Panel、添加 Action 的全流程。每一步对应 Ribbon 层次结构中的一个层级。

!!! tip "最佳实践"

    - 为每个 Category 和 QAction 设置 `setObjectName`，便于后续通过 `categoryByObjectName` 等方法快速定位组件
    - 上下文分类创建后默认是隐藏的，需要通过 `showContextCategory()` 或 `hideContextCategory()` 控制显隐
    - `setRibbonStyle` 可在运行时动态调用，切换时会自动重排所有 Panel 和按钮的布局
    - Gallery 组件适合用在"样式选择"、"主题预览"等需要网格展示多个选项的场景

## API 核心方法摘要

### SARibbonBar 核心方法

| 方法 | 返回值 | 说明 |
|------|--------|------|
| `addCategoryPage(const QString& title)` | `SARibbonCategory*` | 创建并添加一个新的分类页 |
| `insertCategoryPage(const QString& title, int index)` | `SARibbonCategory*` | 在指定索引位置插入分类页 |
| `removeCategory(SARibbonCategory* category)` | `void` | 移除并删除分类页 |
| `addContextCategory(...)` | `SARibbonContextCategory*` | 创建上下文分类组 |
| `showContextCategory(...)` / `hideContextCategory(...)` | `void` | 控制上下文分类显隐 |
| `setRibbonStyle(RibbonStyles v)` | `void` | 设置 Ribbon 布局样式 |
| `quickAccessBar()` | `SARibbonQuickAccessBar*` | 获取快速访问栏 |
| `rightButtonGroup()` | `SARibbonButtonGroupWidget*` | 获取右侧按钮组 |
| `ribbonTabBar()` | `SARibbonTabBar*` | 获取标签栏 |
| `applicationButton()` | `QAbstractButton*` | 获取应用按钮 |

### SARibbonCategory 核心方法

| 方法 | 返回值 | 说明 |
|------|--------|------|
| `addPanel(const QString& title)` | `SARibbonPanel*` | 创建并添加面板 |
| `insertPanel(const QString& title, int index)` | `SARibbonPanel*` | 在指定位置插入面板 |
| `removePanel(SARibbonPanel* panel)` | `bool` | 移除并删除面板 |
| `panelByIndex(int index)` | `SARibbonPanel*` | 按索引获取面板 |
| `panelByName(const QString& title)` | `SARibbonPanel*` | 按名称查找面板 |
| `panelList()` | `QList<SARibbonPanel*>` | 获取所有面板列表 |
| `isContextCategory()` | `bool` | 判断是否为上下文分类 |

### SARibbonPanel 核心方法

| 方法 | 返回值 | 说明 |
|------|--------|------|
| `addLargeAction(QAction* action)` | `void` | 添加大尺寸按钮 |
| `addMediumAction(QAction* action)` | `void` | 添加中等尺寸按钮（三行模式） |
| `addSmallAction(QAction* action)` | `void` | 添加小尺寸按钮 |
| `addWidget(QWidget* w, RowProportion rp)` | `QAction*` | 添加自定义控件 |
| `addGallery(bool expanding)` | `SARibbonGallery*` | 添加画廊组件 |
| `addSeparator()` | `QAction*` | 添加分隔线 |
| `actionToRibbonToolButton(QAction* action)` | `SARibbonToolButton*` | 通过 Action 获取对应按钮 |

!!! note "Qt版本兼容性"

    SARibbon 同时支持 Qt 5.12+ 和 Qt 6.x。在 Qt 5 中使用 `QOverload` 连接信号，在 Qt 6 中可直接使用函数指针语法。SARibbon 内部已处理了版本差异，用户代码无需额外适配。

## 参考资料

- 完整示例项目：`example/MainWindowExample/mainwindow.cpp`
- SARibbonBar 类参考：`src/SARibbonBar/SARibbonBar.h`
- SARibbonCategory 类参考：`src/SARibbonBar/SARibbonCategory.h`
- SARibbonPanel 类参考：`src/SARibbonBar/SARibbonPanel.h`
- SARibbonToolButton 类参考：`src/SARibbonBar/SARibbonToolButton.h`
- SARibbonContextCategory 类参考：`src/SARibbonBar/SARibbonContextCategory.h`
- SARibbonGallery 类参考：`src/SARibbonBar/SARibbonGallery.h`
- Ribbon 界面布局说明：[layout-of-SARibbon.md](../use-guide/layout-of-SARibbon.md)
- Ribbon 按钮布局说明：[layout-of-ribbonbutton.md](../use-guide/layout-of-ribbonbutton.md)
