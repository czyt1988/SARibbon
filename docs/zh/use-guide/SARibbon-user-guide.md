# SARibbon使用说明

- ✅ **快速上手**：静态嵌入仅需2个文件，5行代码即可创建 Ribbon 界面
- ✅ **接口命名参考MFC**：Category/Panel/Action 与 MFC Ribbon 命名风格一致
- ✅ **上下文标签页**：SARibbonContextCategory 支持按条件显示/隐藏特定功能组
- ✅ **Gallery画廊控件**：以网格形式展示大量图标选项
- ✅ **自定义配置持久化**：支持用户自定义界面并保存/加载 XML 配置
- ✅ **12篇分步文档**：从导入到进阶，覆盖所有核心功能

---

SARibbon 是一个用于创建现代化 Ribbon 界面的 Qt 库，其界面风格类似于 Microsoft Office 或 WPS。它专为复杂桌面应用程序设计，能有效组织大量功能，常见于工业软件的界面开发。

## 快速上手

将 SARibbon 集成到您的 Qt 项目非常简单，最快的方式是**静态嵌入**——只需将 `src/SARibbon.h` 和 `src/SARibbon.cpp` 两个文件拷贝到您的工程目录中，并在 CMakeLists.txt 或 .pro 文件中添加它们即可。

下面是一个最简示例（参考自 `example/StaticExample`）：

```cpp
#include "SARibbon.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    // 高DPI支持（Qt5需要，Qt6默认开启）
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication a(argc, argv);
    // 静态库需要手动初始化资源
    Q_INIT_RESOURCE(SARibbonResource);

    SARibbonMainWindow w;
    SARibbonBar* ribbon = w.ribbonBar();
    // 添加一个分类页
    SARibbonCategory* category = ribbon->addCategoryPage("Main");
    // 添加一个面板
    SARibbonPanel* panel = category->addPanel("Operations");
    // 添加一个按钮
    QAction* action = new QAction(QIcon(":/icon/save.svg"), "Save", &w);
    panel->addLargeAction(action);

    w.show();
    return a.exec();
}
```

更详细的集成方式请参阅 [库的导入](./import-SARibbon.md)。

## 文档阅读指南

本系列文档按照从基础到进阶的顺序组织，建议按以下顺序阅读：

| 顺序 | 文档 | 说明 |
|------|------|------|
| 1 | [库的导入](./import-SARibbon.md) | 将 SARibbon 集成到您的项目中 |
| 2 | [创建Ribbon风格的窗口](./create-ribbon-style-window.md) | 创建基于 SARibbonMainWindow 或 SARibbonWidget 的窗口 |
| 3 | [Ribbon界面的层次结构](./ribbon-interface-hierarchy.md) | 理解 SARibbonBar 的组件层级关系 |
| 4 | [创建Ribbon界面](./create-ribbon-ui.md) | 添加 Category、Panel、按钮、Gallery 等 UI 元素 |
| 5 | [标题栏的设置](./titlebar-setting.md) | 自定义标题栏的颜色、文字和对齐方式 |
| 6 | [Ribbon的布局方式](./layout-of-SARibbon.md) | 了解六种布局模式及其切换 |
| 7 | [Ribbon按钮布局说明](./layout-of-ribbonbutton.md) | 理解按钮图标和文字的渲染方式 |
| 8 | [Ribbon尺寸设置](./SARibbon-size-settings.md) | 调整标题栏、标签栏、面板等的高度和间距 |
| 9 | [内容边距设置](./contents-margins-of-ribbon.md) | 设置窗口和 RibbonBar 的内容边距 |
| 10 | [Ribbon主题](./SARibbon-theme.md) | 切换内置的6种主题样式 |
| 11 | [自定义Ribbon主题](./design-your-theme.md) | 使用 QSS 深度定制界面风格 |
| 12 | [Ribbon的用户配置化](./persistence-configuration-ribbon.md) | 实现用户自定义 Ribbon 并保存/加载配置 |

## Ribbon界面和传统menubar+toolbar的异同

传统的 menubar+toolbar 无法直接转化为 Ribbon 界面。Ribbon 不仅仅是一个带 `QToolBar` 的工具栏，与传统菜单栏和工具栏相比，它有如下关键区别：

- **按钮渲染方式不同**：Ribbon 的按钮支持大图标、小图标、图文混排、文字换行等多种渲染模式，SARibbon 使用 `SARibbonToolButton` 对按钮进行了重新布局和渲染
- **上下文标签页**：Ribbon 拥有 `Context Category`（上下文标签页），例如在 Office Word 中选中一张图片时，会自动出现"图片编辑"标签页，提供裁剪、旋转等图片专属功能，取消选择后该标签页自动隐藏
- **特殊控件**：Ribbon 界面拥有 Gallery（画廊）等特有控件，例如 Word 的样式选择器就是一个 Gallery 控件
- **最小化模式**：Ribbon 支持双击标签页切换到最小化模式，仅显示标签栏，点击标签时临时弹出内容面板

如果您想查看传统菜单栏和 Ribbon 界面的对比效果，可以运行 `example/NormalMenuBarExample` 示例。

## SARibbon接口命名

`SARibbon` 在设计时参考了 `MFC Ribbon` 接口的命名风格。标签页称之为 `Category`（分类），每个 `Category` 下面有多个 `Panel`（面板），面板内管理着各种按钮。`Panel` 类似传统的 `Toolbar`，其层次结构如下图所示：

![msdn ribbon 介绍页](../../assets/pic/msdn-ribbon.png)

各核心类的对应关系如下：

| 概念 | SARibbon 类名 | 说明 |
|------|--------------|------|
| Ribbon 栏 | `SARibbonBar` | 管理整个 Ribbon 界面的顶层容器 |
| 分类页 | `SARibbonCategory` | 对应一个标签页，如"主页"、"插入" |
| 面板 | `SARibbonPanel` | Category 内的功能分组，类似增强版工具栏 |
| 工具按钮 | `SARibbonToolButton` | 支持大/小图标模式的 Ribbon 按钮 |
| 上下文标签 | `SARibbonContextCategory` | 按需显示的特殊标签页 |
| 画廊控件 | `SARibbonGallery` | 以网格形式展示图标选项的控件 |
| 快速访问栏 | `SARibbonQuickAccessBar` | 标题栏上的快捷操作工具栏 |
