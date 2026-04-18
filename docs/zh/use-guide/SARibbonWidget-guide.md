# 在非MainWindow场景使用SARibbon

本指南详细说明如何在 `QDialog`、`QWidget` 子窗口、`QMdiSubWindow` 等非 `QMainWindow` 场景下集成 SARibbon。

## SARibbonWidget 简介

`SARibbonWidget` 是 SARibbon 提供的 QWidget 版本容器类，专门为不需要完整 `QMainWindow` 的场景设计。它将 `SARibbonBar` 和内容区域统一管理，通过内部布局将 Ribbon 固定在顶部。

### 核心特性

| 特性 | 说明 |
|------|------|
| **无 QMainWindow 依赖** | 可在 `QDialog`、`QWidget`、`QMdiSubWindow` 等任意容器中嵌入 |
| **自动布局管理** | 内部使用布局自动将 `SARibbonBar` 放置在顶部，内容区自动填充剩余空间 |
| **Q_PROPERTY 支持** | 暴露 `ribbonTheme` 属性，支持 Qt 属性系统和快速切换主题 |
| **完整 Ribbon 功能** | 与 `SARibbonMainWindow` 相同的全部 Ribbon 能力（分类页、面板、快速访问栏等） |

---

## SARibbonWidget vs SARibbonMainWindow 对比

| 对比维度 | SARibbonMainWindow | SARibbonWidget |
|----------|-------------------|----------------|
| **继承基类** | `QMainWindow`（自带菜单栏、工具栏、状态栏等区域） | `QWidget`（轻量、无预设区域） |
| **ribbonBar() 获取方式** | `SARibbonMainWindow::ribbonBar()` 自动创建 | `SARibbonWidget::ribbonBar()` 自动创建 |
| **系统按钮栏** | 提供 `windowButtonBar()` — 最小化、最大化、关闭等系统按钮 | **无系统按钮栏**，需手动管理窗口边框和按钮 |
| **窗口边框** | 自定义无边框（FramelessHelper）或原生边框可选 | 使用宿主窗口的边框/属性，自身无边框处理 |
| **内容区域设置** | `setCentralWidget()` | `setWidget()` |
| **适用场景** | 应用程序主窗口、需要完整窗口管理功能的场景 | 对话框、子窗口、Tab 页、MDI 子窗口、插件容器等 |
| **布局注意事项** | 不需要手动布局 Ribbon | **不可**对 `SARibbonWidget` 自身添加布局，而是把内容 `setWidget` 进去 |

---

## SARibbonWidget 类 API 摘要

### Q_PROPERTY

```cpp
Q_PROPERTY(SARibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)
```

| 属性名 | 类型 | 访问器 | 说明 |
|--------|------|--------|------|
| `ribbonTheme` | `SARibbonTheme` | `ribbonTheme()` / `setRibbonTheme()` | Ribbon 主题（支持 6 种内置主题） |

### 关键方法

| 方法签名 | 说明 |
|----------|------|
| `SARibbonBar* ribbonBar() const` | 获取内部 `SARibbonBar` 指针 |
| `void setRibbonBar(SARibbonBar* bar)` | 设置自定义 `SARibbonBar` 实例 |
| `void setRibbonTheme(SARibbonTheme theme)` | 设置 Ribbon 主题 |
| `SARibbonTheme ribbonTheme() const` | 获取当前主题 |
| `bool isUseRibbon() const` | 判断当前是否在 Ribbon 模式 |
| `void setWidget(QWidget* w)` | 设置内容区域窗口 |
| `QWidget* widget() const` | 获取当前内容区域窗口 |
| `QWidget* takeWidget()` | 移除并返回内容区域窗口（所有权转移） |

---

## 使用场景与代码示例

### 场景 1：在 QDialog 中使用 Ribbon 菜单

适用于需要 Ribbon 风格的复杂对话框（如设置对话框、数据编辑对话框）。

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
        // 1. 创建 SARibbonWidget
        SARibbonWidget* ribbonWidget = new SARibbonWidget(this);
        SARibbonBar* ribbon = ribbonWidget->ribbonBar();

        // 2. Widget 模式推荐设置
        ribbon->setTitleVisible(false);                    // 隐藏标题栏
        ribbon->setApplicationButton(nullptr);             // 移除应用按钮
        ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);  // 紧凑模式

        // 3. 添加分类页和面板
        SARibbonCategory* cat = ribbon->addCategoryPage(tr("Edit"));
        SARibbonPanel* panel = cat->addPanel(tr("Tools"));

        QAction* actFind = new QAction(QIcon(":/icon/find.svg"), tr("Find"), this);
        QAction* actReplace = new QAction(QIcon(":/icon/replace.svg"), tr("Replace"), this);
        panel->addLargeAction(actFind);
        panel->addLargeAction(actReplace);

        // 4. 设置内容区域
        QTextEdit* editor = new QTextEdit(this);
        ribbonWidget->setWidget(editor);

        // 5. 布局 - 注意：对 QDialog 设置布局，而非 SARibbonWidget
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(ribbonWidget);

        resize(800, 500);
    }
};
```

---

### 场景 2：在 QWidget 子窗口中使用 Ribbon

适用于作为主窗口一部分的子面板（如 `QTabWidget` 中的 Tab 页），每个 Tab 页都带有独立的 Ribbon 菜单。

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

        // Widget 模式推荐设置
        ribbon->setTitleVisible(false);
        ribbon->setApplicationButton(nullptr);
        ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);

        // 创建分类页
        SARibbonCategory* category = ribbon->addCategoryPage(tabName);
        SARibbonPanel* panel = category->addPanel(tr("Operations"));

        QAction* actSave = new QAction(QIcon(":/icon/save.svg"), tr("Save"), this);
        QAction* actPrint = new QAction(QIcon(":/icon/print.svg"), tr("Print"), this);
        panel->addLargeAction(actSave);
        panel->addSmallAction(actPrint);

        // 快速访问栏
        SARibbonQuickAccessBar* qbar = ribbon->quickAccessBar();
        qbar->addAction(actSave);

        // 内容区
        QPlainTextEdit* textEdit = new QPlainTextEdit(this);
        textEdit->setPlaceholderText(tr("Content area for %1").arg(tabName));
        setWidget(textEdit);
    }
};

// 使用方式 - 嵌入主窗口
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

### 场景 3：在 MDI 子窗口中使用 Ribbon

在 `QMdiArea` 的每个子窗口中嵌入独立的 Ribbon 界面。

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

        // 创建 SARibbonWidget 作为子窗口的内容
        SARibbonWidget* ribbonWidget = new SARibbonWidget();
        SARibbonBar* ribbon = ribbonWidget->ribbonBar();

        // MDI 子窗口推荐紧凑设置
        ribbon->setTitleVisible(false);
        ribbon->setApplicationButton(nullptr);
        ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);

        // 创建编辑菜单
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

        // 内容区
        QTextEdit* editor = new QTextEdit();
        ribbonWidget->setWidget(editor);

        // 将 SARibbonWidget 设置为 QMdiSubWindow 的 widget
        // 注意：QMdiSubWindow 需要使用 setWidget（Qt 原生方法）
        // 但 SARibbonWidget 本身已是完整的容器，直接设置即可
        QWidget* container = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(ribbonWidget);
        setWidget(container);

        resize(640, 480);
    }
};

// 在主窗口的 MDI 区域中使用
// MainWindow::addNewDocument()
// {
//     auto* child = new MdiChildRibbon(QString("Document %1").arg(docCount++));
//     ui->mdiArea->addSubWindow(child);
//     child->show();
// }
```

---

## 布局注意事项

> !!! tip "Widget 模式下的布局优化"
>
> **核心规则：不要对 `SARibbonWidget` 自身调用 `setLayout()`。**
>
> `SARibbonWidget` 内部已经使用布局管理来将 `SARibbonBar` 固定在顶部。如果您对其额外添加布局，会导致布局冲突和 Ribbon 显示异常。
>
> **正确做法**：将内容窗口通过 `setWidget()` 设置进 `SARibbonWidget`，然后将 `SARibbonWidget` 放入宿主窗口的布局中：
>
> ```cpp
> // ✅ 正确
> SARibbonWidget* rw = new SARibbonWidget();
> rw->setWidget(myContentWidget);       // 内容交给 SARibbonWidget 管理
> parentLayout->addWidget(rw);          // 将 SARibbonWidget 加入父布局
>
> // ❌ 错误
> SARibbonWidget* rw = new SARibbonWidget();
> QVBoxLayout* layout = new QVBoxLayout(rw);  // 会破坏内部布局！
> layout->addWidget(rw->ribbonBar());
> layout->addWidget(myContentWidget);
> ```
>
> **紧凑设置推荐**：在 Widget 模式下，Ribbon 占用空间越小越好，建议统一设置：
>
> ```cpp
> ribbon->setTitleVisible(false);
> ribbon->setApplicationButton(nullptr);
> ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);
> ```

## 主题切换注意事项

在 `SARibbonWidget` 构造函数中直接设置主题可能导致不完全生效。推荐使用 `QTimer` 延迟到事件队列末尾执行：

```cpp
// SARibbonWidget 构造函数中
SARibbonWidget::MyRibbonWidget(QWidget* parent) : SARibbonWidget(parent)
{
    // ... 初始化 Ribbon ...

    // 延迟设置主题确保完全生效
    QTimer::singleShot(0, this, [this]() {
        setRibbonTheme(SARibbonTheme::RibbonThemeDark);
    });
}
```

---

## 与 SARibbonMainWindow 协同使用

在 `SARibbonMainWindow` 主窗口中，对话框或子窗口可以搭配使用 `SARibbonWidget` 来保持界面风格一致：

```cpp
// 主窗口（SARibbonMainWindow）中弹出 Ribbon 对话框
void MainWindow::showSettingsDialog()
{
    RibbonDialog dlg(this);
    dlg.exec();
}

// 主窗口中添加 Ribbon Tab 子窗口
void MainWindow::addRibbonTab()
{
    ui->tabWidget->addTab(new RibbonTabWidget(tr("New Tab")), tr("New"));
}
```

这种组合方式让主窗口保持完整窗口管理功能的同时，子组件也能获得一致的 Ribbon 体验。
