
# SARibbon 库使用指南

SARibbon 是一个用于创建现代化 Ribbon 界面的 Qt 库，其界面风格类似于 Microsoft Office 或 WPS。

在开始编码之前，您需要将 SARibbon 库集成到您的 Qt 项目中。最简单的方式是**静态嵌入**，即直接将源文件`SARibbon.h`和`SARibbon.cpp`拷贝到您的工程即可。

# Ribbon界面和传统menubar+toolbar的异同


# 基于SARibbon创建Ribbon风格的窗口

## 创建Ribbon风格的`MainWindow`

SARibbon 的核心是`SARibbonBar`这个类，它可以用于`MainWindow`也可以用于`Widget`

如果用于`MainWindow`，您的主窗口应继承自 `SARibbonMainWindow`，它会自动为您创建一个 `SARibbonBar`

创建Ribbon风格的`MainWindow`示例代码

```cpp
#include "mainwindow.h"
#include "SARibbon.h"

//
class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr) : SARibbonMainWindow(parent)
    {
        // 获取 RibbonBar 指针
        SARibbonBar* ribbon = ribbonBar();
        ...
    }
};
```

> 注意：如果你的`MainWindow`带有`ui`文件，注意要删除`ui`里的原生菜单栏，否则原生菜单栏会覆盖Ribbon菜单界面

## 创建Ribbon风格的`Widget`

# Ribbon界面创建

## Ribbon界面的层次结构

这里主要讲述SARibbon的界面层次结构，从category，到pannel，还有quickaccessbar,applicationbutton,rightbuttongroup等结构

## 创建一个ribbon页面

### category(分类页)

### pannel(面板)

#### 创建面板并添加动作

#### 面板的布局占位说明

#### 在面板中添加复杂控件 (Widget)

#### 使用按钮组 (Button Group)

#### 使用 Gallery (画廊)

### context category(上下文标签页)

Context Category 是一种特殊标签页，通常在特定条件下（如选中某个对象）才显示。

### quick access bar（快速访问工具栏）

### right button group（右侧按钮组）

### application button

`Application Button` 是 Ribbon 界面左上角的主菜单按钮，通常用于“文件”操作。

#### 基本用法 (QMenu)

`Application Button`就是一个按钮，你可以给它添加一个菜单，点击就可以弹出菜单，`SARibbonBar::setApplicationButton`方法可以设置`Application Button`。如果你不想要`Application Button`，你可以传入`nullptr`

```cpp
ribbonBar()->setApplicationButton(nullptr); // 移除按钮
```

SARibbonBar创建时会默认创建一个`SARibbonApplicationButton`实例的`Application Button`，你通过`SARibbonBar::applicationButton`即可获取这个按钮的指针



#### 高级用法 (自定义 Widget)

有很多需求是弹出一个自定义的复杂窗口，类似office，点击左上角的application button，会弹出一个页面

SARibbon提供了`SARibbonApplicationWidget`，它能自动帮你覆盖主窗口，你的弹出式窗口可以继承`SARibbonApplicationWidget`来实现

### Window Button Bar（系统标题栏按钮旁的工具栏）


# Ribbon布局和主题设置

## SARibbon布局

SARibbon 支持四种布局方案：宽松三行、宽松两行、紧凑三行、紧凑两行。您可以动态切换它们。

SARibbon的样式枚举定义如下（位于SARibbonBar中）：
```cpp
enum RibbonStyleFlag
{
    RibbonStyleLoose    = 0x0001,  // bit:0000 0001
    RibbonStyleCompact  = 0x0002,  // bit:0000 0010
    RibbonStyleThreeRow = 0x0010,  // bit:0001 0000
    RibbonStyleTwoRow   = 0x0020,  // bit:0010 0000

    RibbonStyleLooseThreeRow   = RibbonStyleLoose | RibbonStyleThreeRow,    ///< 宽松结构，3行模式
    RibbonStyleCompactThreeRow = RibbonStyleCompact | RibbonStyleThreeRow,  ///< 紧凑结构，3行模式
    RibbonStyleLooseTwoRow     = RibbonStyleLoose | RibbonStyleTwoRow,      ///< 宽松结构，2行模式
    RibbonStyleCompactTwoRow   = RibbonStyleCompact | RibbonStyleTwoRow     ///< 紧凑结构，2行模式
};
```

你可以通过SARibbonBar::setRibbonStyle设置SARibbon的布局风格

```
void setRibbonStyle(RibbonStyles v);
RibbonStyles currentRibbonStyle() const;
```
## 切换内置主题

SARibbon 提供了多种内置主题，如 Windows 7、Office 2013、Office 2016、暗色主题等。



## 使用 QSS 自定义样式

您可以通过加载 QSS (Qt Style Sheet) 文件来实现更精细的界面定制。




# 动作管理与界面自定义 (ActionsManager & Customize)

`SARibbonActionsManager` 是实现 Ribbon 界面自定义功能的核心。它管理所有动作，并允许用户通过对话框重新排列它们。

## 创建和注册动作管理器

```cpp
MainWindow::MainWindow(QWidget* parent = nullptr) : SARibbonMainWindow(parent)
{
    // 创建管理器，并将 RibbonBar 传入
    m_actionsManager = new SARibbonActionsManager(ribbonBar());

    // 注册不在 Ribbon 上显示的额外动作，供自定义时使用
    m_otherAction1 = new QAction("Action 1", this);
    m_otherAction2 = new QAction("Action 2", this);
    m_otherAction3 = new QAction("Action 3", this);

    // 将动作注册到特定标签下
    int customTag = SARibbonActionsManager::UserDefineActionTag + 1;
    m_actionsManager->registeAction(m_otherAction1, customTag);
    m_actionsManager->registeAction(m_otherAction2, customTag);
    m_actionsManager->registeAction(m_otherAction3, customTag);

    // 为标签设置显示名称
    m_actionsManager->setTagName(customTag, tr("My Custom Actions"));
}
```

## 启动自定义对话框

SARibbon 提供了两种方式来启动自定义界面。

**方式一：使用独立的 `SARibbonCustomizeWidget`**

```cpp
void MainWindow::onActionCustomizeTriggered()
{
    if (nullptr == mWidgetForCustomize) {
        mWidgetForCustomize = new SARibbonCustomizeWidget(
            this, this, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::Dialog
        );
        mWidgetForCustomize->setWindowModality(Qt::ApplicationModal);
        mWidgetForCustomize->setupActionsManager(mActionsManager);
    }
    mWidgetForCustomize->show();
    mWidgetForCustomize->applys(); // 应用当前设置
}
```

**方式二：使用内置的 `SARibbonCustomizeDialog` (推荐)**

此对话框内置了“应用”、“保存”、“取消”按钮，并支持将配置保存到 XML 文件。

```cpp
void MainWindow::onActionCustomizeAndSaveTriggered()
{
    SARibbonCustomizeDialog dlg(this);
    dlg.setupActionsManager(mActionsManager);

    // 从 XML 文件加载上次的自定义配置
    dlg.fromXml("customize.xml");

    if (dlg.exec() == SARibbonCustomizeDialog::Accepted) {
        // 应用更改
        if (dlg.isCached()) {
            dlg.applys();
        }

        // 将新的配置保存到 XML 文件
        QByteArray xmlData;
        QXmlStreamWriter xml(&xmlData);
        xml.setAutoFormatting(true);
        xml.writeStartDocument();
        bool success = dlg.toXml(&xml);
        xml.writeEndDocument();

        if (success) {
            QFile file("customize.xml");
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.write(xmlData);
                file.close();
            }
        }
    }
}
```

# 其它功能与技巧

## 控制标题栏

您可以动态改变 Ribbon 标题栏的背景色和文字颜色。


## 调整 Ribbon 尺寸

SARibbon 允许您精细调整各个部分的高度和间距。

```cpp
// 设置标题栏高度
ribbonBar()->setTitleBarHeight(40);

// 设置标签栏高度
ribbonBar()->setTabBarHeight(30);

// 设置面板标题高度
ribbonBar()->setPanelTitleHeight(20);

// 设置面板间间距
ribbonBar()->setPanelSpacing(5);

// 设置工具按钮图标大小
ribbonBar()->setPanelToolButtonIconSize(QSize(32, 32));

// 设置按钮最大宽高比
ribbonBar()->setButtonMaximumAspectRatio(2.0);
```

## 颜色选择按钮

`SARibbonColorToolButton` 是一个专门用于颜色选择的按钮。



