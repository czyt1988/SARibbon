# SARibbon使用指南

SARibbon 是一个用于创建现代化 Ribbon 界面的 Qt 库，其界面风格类似于 Microsoft Office 或 WPS。它专为复杂桌面应用程序设计，能有效组织大量功能，常见于工业软件的界面开发。

在开始编码之前，您需要将 SARibbon 库集成到您的 Qt 项目中。最简单的方式是**静态嵌入**，即直接将源文件`SARibbon.h`和`SARibbon.cpp`拷贝到您的工程即可。

## Ribbon界面和传统menubar+toolbar的异同

传统的menubar+toolbar是无法直接转化为ribbon界面的，Ribbon不仅仅是一个带`QToolBar`的工具栏，与传统菜单栏和工具栏相比它有如下特点：

- Ribbon的按钮渲染方式有一个明显的改变，导致无法直接使用ToolButton来模拟，SARibbon使用`SARibbonToolButton`针对Ribbon的按钮进行了重新布局和渲染
- Ribbon还有一种叫上下文标签页`Context Category`的特殊标签页，例如，在Office Word中选中一张图片，会自动出现一个“图片编辑”标签页，提供裁剪、旋转等图片专属功能，取消选择后该标签页自动隐藏
- Ribbon界面会带有一些特殊的控件，例如Gallery（word的样式选择就是Gallery控件）

## 基于SARibbon创建Ribbon风格的窗口

### 创建Ribbon风格的`MainWindow`

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

> **重要注意事项**：
>
> 如果你的 `MainWindow` 是通过 Qt Designer 创建的，**务必在 Designer 中删除默认的 `menuBar`**。因为 `SARibbonMainWindow` 会用自己的 `SARibbonBar` 替换掉原生的菜单栏，如果 `.ui` 文件中保留了原生菜单栏，会把ribbon菜单覆盖回原生的菜单。

`SARibbonMainWindow`的构造函数有三个参数，其定义如下：

```cpp
SARibbonMainWindow(QWidget* parent, SARibbonMainWindowStyles style, const Qt::WindowFlags flags)
```

最关键是第二个参数，第二个参数`SARibbonMainWindowStyles`决定了窗口的总体样式风格，常用的是下面两种搭配：

1. `SARibbonMainWindowStyleFlag::UseRibbonMenuBar|SARibbonMainWindowStyleFlag::UseRibbonFrame`

此风格为默认风格，使用ribbon风格菜单栏和ribbon风格边框，这时候窗口是一个自定义边框，不使用原生边框，界面效果如下：

![mainwindow-ribbonbar+ribbonframe](./pic/mainwindow-ribbonbar+ribbonframe.png)

2. `SARibbonMainWindowStyleFlag::UseRibbonMenuBar|SARibbonMainWindowStyleFlag::UseNativeFrame`

此风格为使用ribbon风格菜单栏和原生边框，这个界面的好处是支持操作系统的边框特效，例如win11的全局窗口快捷键，以及一些边框特效

![mainwindow-ribbonbar+nativeframe](./pic/mainwindow-ribbonbar+nativeframe.png)

在使用原生边框的时候，为了更适配原生边框，`SARibbonMainWindow`会把图标隐藏，同时把ribbonbar的样式设置为紧凑模式：

```cpp
// 在ribbon模式下使用本地边框，将隐藏icon，同时默认设置为紧凑模式
if (SARibbonBar* bar = ribbonBar()) {
    if (SARibbonTitleIconWidget* iconWidget = bar->titleIconWidget()) {
        // 隐藏icon
        iconWidget->hide();
    }
    // 设置为紧凑模式
    bar->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);
}
```

### 创建Ribbon风格的`Widget`

除了主窗口，您也可以在普通的 `QWidget` 或 `QDialog` 上使用 Ribbon 界面，这在创建复杂对话框或子窗口时非常有用。为此，SARibbon 提供了 `SARibbonWidget` 类。

```cpp
#include "SARibbonWidget.h"
class MyRibbonWidget : public SARibbonWidget
{
    Q_OBJECT

public:
    explicit MyRibbonWidget(QWidget *parent = nullptr);
};
```

实现文件：

```cpp
// myribbonwidget.cpp
#include "myribbonwidget.h"
#include "SARibbonBar.h"

MyRibbonWidget::MyRibbonWidget(QWidget *parent)
    : SARibbonWidget(parent)
{
    // 1. 获取 RibbonBar
    SARibbonBar* ribbon = ribbonBar();

    // 2. 针对 Widget 模式进行优化
    ribbon->setTitleVisible(false); // 隐藏标题栏，因为 QWidget 通常没有独立标题
    ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow); // 使用紧凑模式节省空间
    ribbon->setApplicationButton(nullptr); // 移除 Application Button，Widget 通常不需要

    // 创建你的ribbon菜单
}
```

通过 `setWidget()` 方法，您可以将任意 `QWidget` 嵌入到 `SARibbonWidget` 中

## Ribbon界面创建

### Ribbon界面的层次结构

理解 SARibbon 的层次结构是构建界面的基础。其结构清晰，层次分明：

1. **`SARibbonBar`**: 最顶层的Ribbon菜单，管理整个 Ribbon 界面。它包含：
    - **`Application Button`**: 左上角的主菜单按钮。
    - **`Quick Access Bar`**: 位于最顶部的快速工具栏。
    - **`SARibbonTabBar`**: 显示各个 `Category` 标签页的标签栏。
    - **`SARibbonStackedWidget`**: 一个堆栈窗口，用于显示当前激活的 `Category` 内容。
    - **`Right Button Group`**: 位于右上角的按钮组。
    - **`Window Button Bar`** : 系统按钮，通过它可以在系统按钮旁边添加按钮。

2. **`SARibbonCategory` (分类页)**: 代表一个功能场景，如“主页”、“插入”、“设计”。每个 `Category` 包含一个或多个 `Panel`。它被添加到 `SARibbonTabBar` 上作为一个标签，并在 `SARibbonStackedWidget` 中管理其内容。

3. **`SARibbonPanel` (面板)**: 是 `Category` 内的功能分组容器，类似于一个增强版的工具栏。一个 `Panel` 可以包含多个 `QAction`（表现为按钮）、`QWidget`（如组合框、编辑框）或 `SARibbonButtonGroupWidget`（按钮组）。

4. **`SARibbonContextCategory` (上下文分类页)**: 一种特殊的 `Category`，默认隐藏，仅在特定条件（如选中特定对象）下才显示。

其布局如下图所示（宽松模式）

![saribbonbar-level](./pic/saribbonbar-level.png)


### 创建一个ribbon页面

#### Category(分类页)

创建分类页有两种方式：

**方式一：通过ribbonbar添加**

```cpp
SARibbonBar* ribbon = ribbonBar();
// 直接添加一个名为“主页”的分类页
SARibbonCategory* homeCategory = ribbon->addCategoryPage("主页");
// 如果你需要进行ribbon界面自定义配置工具（SARibbonCustomizeDialog），你必须设置 objectName用于区分
homeCategory->setObjectName("homeCategory");
```

**方式二：先创建后添加**

```cpp
// 先创建一个分类页对象
SARibbonCategory* insertCategory = new SARibbonCategory(this);
insertCategory->setCategoryName("插入");
insertCategory->setObjectName("insertCategory");

// 再将其添加到 RibbonBar
ribbon->addCategoryPage(insertCategory);
```

#### Panel(面板)

##### 创建面板并添加动作

面板是category下面的一组功能。您可以通过 `Category` 的 `addPanel` 方法创建面板，并通过 `addAction` 系列方法添加功能按钮

每个category必须有一个panel

```cpp
// 在 "主页" 分类下创建一个 "文件" 面板
SARibbonPanel* filePanel = homeCategory->addPanel("文件");
filePanel->setObjectName("filePanel");

// 创建动作
QAction* newAction = new QAction(QIcon(":/new.svg"), "新建", this);
newAction->setObjectName("action_new");
newAction->setShortcut(QKeySequence::New);

// 将动作添加到面板
// addLargeAction: 大按钮，图标+文字
filePanel->addLargeAction(newAction);
// addSmallAction: 小按钮，通常只有图标
QAction* saveAction = new QAction(QIcon(":/save.svg"), "保存", this);
saveAction->setObjectName("action_save");
filePanel->addSmallAction(saveAction);

// 连接信号槽
connect(newAction, &QAction::triggered, [] {
    qDebug() << "New document created.";
});
```

##### 创建菜单

在 Ribbon 界面中，菜单（Menu）是组织复杂功能的重要手段。SARibbon允许您将菜单以不同风格的按钮形式添加到面板中。主要区别在于按钮的弹出模式 (QToolButton::ToolButtonPopupMode)，它决定了用户如何与菜单进行交互

SARibbon 支持3种主要的弹出模式，您可以根据功能需求选择最合适的模式：

1. DelayedPopup (延迟弹出):

    用户点击按钮，会执行按钮的默认动作,用户长按按钮，菜单才会弹出（长按直至菜单弹出，按钮的默认动作不会执行）。适用场景主要是当按钮的主要功能是执行一个常用操作，菜单提供的是次要或高级选项时才选用此模式

2. MenuButtonPopup (菜单按钮弹出)：

    这是 Ribbon 界面最具特色的模式。按钮会被视觉上分割为上下两部分，点击按钮的上半部分（图标区域）会执行默认动作；点击下半部分（文本或下拉箭头区域）则会直接弹出菜单，不执行默认动作。鼠标悬停时，两部分会有不同的高亮效果。

    这是最推荐的模式，尤其适用于 Office 风格的应用。它清晰地区分了主要操作和次要操作，用户体验最佳。例如，一个“粘贴”按钮，点击图标区域执行标准粘贴，点击下拉箭头区域则弹出“选择性粘贴”、“粘贴为纯文本”等选项。

3. InstantPopup (即时弹出)：

    用户点击按钮的任何部分，都会立即弹出菜单，不会执行按钮的默认动作。这种模式适用于当按钮的功能完全由其下拉菜单定义，没有单一的“默认”操作时。例如，“页面布局”按钮，点击后直接弹出设置页边距、纸张方向等选项的菜单。

SARibbon添加菜单有两种方法，第一种是调用`addXXAction`函数：

```cpp
// 把action加入到panel，并以小图标显示
void addSmallAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
// 把action加入到panel，并以大图标显示
void addLargeAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
// 把action加入到panel，在三行模式下会以中图标显示
void addMediumAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
```

使用这种方法是action包含了menu，第二个参数明确指定了按钮的样式

下面是使用此方法的例子：

```cpp
SARibbonPanel* panelToolButtonStyle = page->addPanel(("sa ribbon toolbutton style"));

QMenu* menu = new QMenu;
//...构建菜单

// DelayedPopup模式
QAction* act = createAction(tr("Delayed\nPopup"), ":/icon/icon/folder-cog.svg");
act->setMenu(menu);
panelToolButtonStyle->addLargeAction(act, QToolButton::DelayedPopup);
connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupTriggered);

act = createAction(tr("Menu Button Popup"), ":/icon/icon/folder-star.svg");
act->setMenu(menu);
panelToolButtonStyle->addLargeAction(act, QToolButton::MenuButtonPopup);
connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupTriggered);

act = createAction(tr("Instant Popup"), ":/icon/icon/folder-stats.svg");
act->setMenu(menu);
panelToolButtonStyle->addLargeAction(act, QToolButton::InstantPopup);
connect(act, &QAction::triggered, this, &MainWindow::onInstantPopupTriggered);
```

上面的代码将会创建如下三个菜单按钮

![](./pic/SARibbon-Toolbutton-Style.gif)

这三个按钮共享一个菜单

SARibbon还提供了另外一个接口来快速添加菜单(方法位于`SARibbonPanel`类)：

```cpp
// 添加menu
void addMenu(QMenu* menu,
                SARibbonPanelItem::RowProportion rowProportion,
                QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
// 添加普通大菜单
void addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
// 添加普通小按钮菜单
void addMediumMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
// 添加普通小按钮菜单
void addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
```

这几个函数实际利用率menu自身的action进行管理，无需再单独创建一个action，注意，这几个方法会修改 `menu->menuAction()` 的部分属性（icon/text/objectName），如需保留自定义值，请提前设置。

##### 面板的布局占位说明

SARibbon 面板的布局目前有3种占位标志。当您调用 `addLargeAction`、`addMediumAction` 或 `addSmallAction` 时，您不仅是在添加一个按钮，还在指定它在网格布局中的“占位大小”。

- **Large Action**: 高度上占据2行或3行（取决于当前是两行还是三行模式），显示大图标和文字。
- **Medium Action**: 高度上占据1行或1.5行的网格（取决于当前是两行还是三行模式），通常显示图标和文字。
- **Small Action**: 高度上占据1行

这些行占比在不同的布局方案会有不同的呈现效果

SARibbonPanel支持两种布局方案，分别是2行模式和3行模式，通过`SARibbonBar::setRibbonStyle`函数你可以设置不同的布局风格

对于3行模式，上诉三种行占位的效果如下图所示

![pannelLayout3row](./pic/pannelLayout3row.png)

对于2行模式，上诉三种行占位的效果如下图所示

![pannelLayout2row](./pic/pannelLayout2row.png)

2行模式下medium和small占位(`SARibbonPannelItem::RowProportion`)是一样的，不做区分。

##### 在面板中添加复杂控件 (Widget)

除了 `QAction`，您还可以直接在面板中添加任何 `QWidget`，例如组合框、微调框或自定义控件。

```cpp
SARibbonCategory* page = ribbonBar()->addCategoryPage(tr("Main"));
SARibbonPanel* panelWidgetTest = page->addPanel(tr("widget test"));

QComboBox* com1 = new QComboBox(this);
com1->setWindowTitle(tr("QComboBox test"));
for (int i = 0; i < 40; ++i) {
    com1->addItem(QString("QComboBox test%1").arg(i + 1));
}
com1->setEditable(true);
panelWidgetTest->addSmallWidget(com1);

QComboBox* com2 = new QComboBox(this);
com2->setWindowTitle("ComboBox Editable");
for (int i = 0; i < 40; ++i) {
    com2->addItem(QString("item %1").arg(i + 1));
}
panelWidgetTest->addSmallWidget(com2);


QLineEdit* lineEdit = new QLineEdit(this);
lineEdit->setText("LineEdit");
panelWidgetTest->addSmallWidget(lineEdit);

panelWidgetTest->addSeparator();

QCalendarWidget* calendarWidget = new QCalendarWidget(this);
panelWidgetTest->addLargeWidget(calendarWidget);
```

上面代码的效果如下：

![widget-in-panel](./pic/widget-in-panel.png)

##### 使用按钮组 (Button Group)

按钮组`SARibbonButtonGroupWidget` 可以将多个小按钮紧密排列在一起，和`QToolBar`类似，常用于一组相近功能的操作，如文本对齐

`SARibbonButtonGroupWidget`继承`QToolBar`，在一个按钮组里的`action`,你不需要再组个aciton绑定信号，使用`QToolBar::actionTriggered`信号即可

下面这段代码演示如何创建按钮组，以及如何绑定信号

```cpp
// 按钮组2
SARibbonButtonGroupWidget* btnGroup2 = new SARibbonButtonGroupWidget(panel1);
btnGroup2->setObjectName("SARibbonButtonGroupWidget2");

QAction* titleAlgnmentRight = createAction(tr("Align Right"), ":/icon/icon/Align-Right.svg");
titleAlgnmentRight->setProperty("align", (int)Qt::AlignRight | Qt::AlignVCenter);
btnGroup2->addAction(titleAlgnmentRight);
QAction* titleAlgnmentLeft = createAction(tr("Align Left"), ":/icon/icon/Align-Left.svg");
titleAlgnmentLeft->setProperty("align", (int)Qt::AlignLeft | Qt::AlignVCenter);
btnGroup2->addAction(titleAlgnmentLeft);
QAction* titleAlgnmentCenter = createAction(tr("Align Center"), ":/icon/icon/Align-Center.svg");
titleAlgnmentCenter->setProperty("align", (int)Qt::AlignCenter);
btnGroup2->addAction(titleAlgnmentCenter);
panel1->addWidget(btnGroup2, SARibbonPanelItem::Medium);
connect(btnGroup2, &SARibbonButtonGroupWidget::actionTriggered, this, &MainWindow::onButtonGroupActionTriggered);
```

上面的代码生成了如下界面（具体例子可见example/MainWindowExample/mainWindow.cpp,createCategoryOther函数）

![button-group](./pic/button-group.png)

##### 使用 Gallery (画廊)

Gallery (画廊)是一个Ribbon特有的控件，用于以网格形式展示带图标的选项，常用于在有限空间显示大量图标动作，例如word的样式选择

SARibbon的Gallery (画廊)对应窗口为`SARibbonGallery`，通过`SARibbonPanel::addGallery`即可创建一个Gallery (画廊)，此函数返回创建的`SARibbonGallery`指针

`SARibbonGallery`可以管理多个动作组，一个动作组对应的类名为`SARibbonGalleryGroup`,你可以使用`SARibbonGallery::addCategoryActions`方法快速添加动作组，其参数是动作组标题和动作组的action列表

```cpp
SARibbonGalleryGroup* addCategoryActions(const QString& title, QList< QAction* > actions);
```

下面这段代码演示了如何创建一个Gallery (画廊)，并添加2个动作组

```cpp
QAction MyRibbonMainWindow::createAction(const QString& text, const QString& iconurl){
    QAction* act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    return act;
}

void MyRibbonMainWindow::buildGalleryExample(){
    SARibbonPanel* panelWithGallery = ...;
    ...
    // 创建一个Gallery (画廊)
    SARibbonGallery* gallery = panelWithGallery->addGallery();
    // 添加动作组1
    QList< QAction* > galleryActions1;
    galleryActions1.append(createAction("Document File", ":/gallery-icon/icon/gallery/Document-File.svg"));
    galleryActions1.append(createAction("Download File", ":/gallery-icon/icon/gallery/Download-File.svg"));
    ......
    galleryActions1.append(createAction("File Settings", ":/gallery-icon/icon/gallery/File-Settings.svg"));
    galleryActions1.append(createAction("Presentation File", ":/gallery-icon/icon/gallery/Presentation-File.svg"));
    SARibbonGalleryGroup* group1 = gallery->addCategoryActions(tr("Files"), galleryActions1);
    group1->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    group1->setGridMinimumWidth(80);

    // 添加动作组2
    QList< QAction* > galleryActions2;
    galleryActions2.append(createAction("Photoshop", ":/gallery-icon/icon/gallery/Photoshop.svg"));
    galleryActions2.append(createAction("Internet-Explorer", ":/gallery-icon/icon/gallery/Internet-Explorer.svg"));
    galleryActions2.append(createAction("Illustrator", ":/gallery-icon/icon/gallery/Illustrator.svg"));
    galleryActions2.append(createAction("Google-Maps", ":/gallery-icon/icon/gallery/Google-Maps.svg"));
    galleryActions2.append(createAction("Adobe", ":/gallery-icon/icon/gallery/Adobe.svg"));
    galleryActions2.append(createAction("Word", ":/gallery-icon/icon/gallery/Word.svg"));
    SARibbonGalleryGroup* group2 = gallery->addCategoryActions(tr("Apps"), galleryActions2);
    group2->setGridMinimumWidth(80);
    // 设置当前显示的组
    gallery->setCurrentViewGroup(group1);
}

```

通过上面代码，创建的gallery效果如下:

![saribbon-gallery](./pic/saribbon-gallery.png)

此gallery展开后效果如下：

![saribbon-gallery-expand](./pic/saribbon-gallery-expand.png)

#### Context Category(上下文标签页)

Context Category 是一种特殊标签页，通常在特定条件下（如选中某个对象）才显示，最常见的就是word中的“图片工具”和“表格工具”，是在选中图片或表格时才出现，这种有条件显示的标签称之为上下文标签

SARibbon的Context Category(上下文标签页)对应窗口为`SARibbonContextCategory`，通过`SARibbonBar::addContextCategory`即可创建一个Context Category(上下文标签页)，此函数返回创建的`SARibbonContextCategory`指针

`SARibbonContextCategory`并不是一个窗口(它的基类是`QObject`)，只是一个标签页管理器，需要再`SARibbonContextCategory`上添加标签页，通过`SARibbonContextCategory::addCategoryPage`方法即可在对应的Context Category(上下文标签页)上添加标签页

Context Category(上下文标签页)默认情况下不会显示，需要调用`SARibbonBar::showContextCategory`方法显示，隐藏上下文标签页则可通过`SARibbonBar::hideContextCategory`实现，`SARibbonContextCategory`也带有`show`/`hide`槽函数来实现上下文标签页的显示和隐藏

下面这段代码演示了如何创建一个Context Category(上下文标签页)，并添加两个标签页

```cpp
//m_contextCategory为MainWindow的成员变量：SARibbonContextCategory* m_contextCategory;

void MainWindow::createContextCategory(){
    SARibbonBar* ribbon = ribbonBar();
    m_contextCategory    = ribbon->addContextCategory(tr("context"), QColor(), 1);

    SARibbonCategory* contextCategoryPage1 = m_contextCategory->addCategoryPage(tr("Page1"));
    //创建contextCategoryPage1对应的内容
    createContextCategoryPage1(contextCategoryPage1);

    SARibbonCategory* contextCategoryPage2 = m_contextCategory->addCategoryPage(tr("Page2"));
    //创建contextCategoryPage2对应的内容
    createContextCategoryPage2(contextCategoryPage2);
}
```

上面的代码创建完Context Category(上下文标签页)后并不会显示，如果需要显示，你还需在特定条件下触发

```cpp
void MainWindow::onShowContextCategory(bool on)
{
    if (m_contextCategory == nullptr) {
        createContextCategory();
    }
    if (on) {
        m_contextCategory->show();
    } else {
        m_contextCategory->hide();
    }
}
```

上面的代码效果如下：

![SARibbonBar-Context-Category](./screenshot/SARibbonBar-Context-Category.png)

为了区分上下文标签页和普通的标签页，上下文标签页会有特殊的颜色进行标识，如上图所示。这个标识的颜色可以通过`SARibbonBar::setContextCategoryColor`设置

#### Application Button

`Application Button` 是 Ribbon 界面左上角的主菜单按钮，通常用于“文件”操作

`SARibbon` 在创建 `SARibbonBar` 时，会默认创建一个`SARibbonApplicationButton`实例作为Application Button。您可以通过 `SARibbonBar::applicationButton`方法获取该按钮的指针，并对其进行操作，例如设置文本、设置图标、设置快捷键、设置菜单，等等

如：

```cpp
void MainWindow::createRibbonApplicationButton()
{
    SARibbonBar* ribbon = ribbonBar();
    QAbstractButton* btn = ribbon->applicationButton();
    btn->setText(("&File"));

    SARibbonApplicationButton* appBtn = qobject_cast< SARibbonApplicationButton* >(btn);
    if (appBtn) {
        QMenu* menu = new QMenu(this);
        ...
        appBtn->setMenu(menu);
    }
}
```

如果你有自定义的按钮，你可以通过 `SARibbonBar::setApplicationButton` 方法设置 `Application Button`。如果你不想要`Application Button`，你可以传入`nullptr`

```cpp
ribbonBar()->setApplicationButton(nullptr); // 移除按钮
```

application button可以通过qss设置比较丰富的样式，下面是模仿win7风格的ribbon样式设置的qss演示：

```css
SARibbonApplicationButton{
  color:white;
  border: 1px solid #416ABD;
  border-top-left-radius: 4px;
  border-top-right-radius: 4px;
  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #467FBD, stop:0.5 #2A5FAC,stop:0.51 #1A4088,
stop:1 #419ACF);
}

SARibbonApplicationButton:hover{
  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #7BB2EB, stop:0.5 #477ECD,stop:0.51 #114ECF,
stop:1 #80E1FF);
}

SARibbonApplicationButton:pressed{
  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,stop:0 #467BBB, stop:0.5 #2F63AE,stop:0.51 #1C438A,
stop:1 #358BC9);
}

SARibbonApplicationButton:focus{
  outline: none;
}

SARibbonApplicationButton::menu-indicator {
  /*subcontrol-position: right;*/
  width:0px;
}
```

上面的效果如下：

![win7style-application-button](./pic/win7style-application-button.png)

##### 高级用法 (Application Widget)

在许多现代化应用（如 Microsoft Office）中，点击 `Application Button` 并不会弹出一个简单的下拉菜单，而是会弹出一个全屏或半屏的复杂页面，该页面可以包含最近文件列表、模板选择、账户设置、应用选项等丰富内容

SARibbon提供了`SARibbonApplicationWidget`，它能自动帮你覆盖主窗口，你的弹出式窗口可以继承`SARibbonApplicationWidget`来实现

`SARibbonApplicationWidget`提供了`resizeToParent`函数来决定如何相对父窗口进行缩放，默认是完全覆盖父窗口，如果你想覆盖一半或者其他尺寸，你可以重写此函数

使用`Application Widget`的例子如下：

继承`SARibbonApplicationWidget`实现自己的窗口

```cpp
class ApplicationWidget : public SARibbonApplicationWidget
{
    Q_OBJECT
public:
    explicit ApplicationWidget(SARibbonMainWindow* parent = nullptr);
    ~ApplicationWidget();
};
```

在MainWindow中创建`Application Widget`的成员变量指针`m_appWidget`，初始化时绑定`applicationButton`的响应：

```cpp
void MainWindow::createRibbonApplicationButton()
{
    SARibbonBar* ribbon = ribbonBar();
    if (!ribbon) {
        return;
    }
    QAbstractButton* btn = ribbon->applicationButton();
    btn->setText(("&File"));
    // 这里演示用SARibbonApplicationWidget实现一个类似office的弹出式窗口
    m_appWidget = new ApplicationWidget(this);
    m_appWidget->hide();
    connect(btn, &QAbstractButton::clicked, this, [ this ](bool c) {
        Q_UNUSED(c);
        this->m_appWidget->show();
    });
}
```

上面的代码效果如下：

![application-widget](./screenshot/application-widget.gif)

#### Quick Access Bar（快速访问工具栏）

快速访问工具栏位于窗口最顶部，用于放置最常用的操作，如保存、撤销、重做。快速访问工具栏对应的类为`SARibbonQuickAccessBar`,它继承自`QToolBar`，你可以和操作工具栏一样使用它

SARibbon默认会创建一个快速访问工具栏，你可以调用`SARibbonBar::quickAccessBar`函数获取快速访问工具栏的指针，然后对快速访问工具栏进行操作

下面是快速访问工具栏的用法：

```cpp
void MainWindow::createQuickAccessBar()
{
    SARibbonQuickAccessBar* quickAccessBar = ribbonBar()->quickAccessBar();

    QAction* actionSave = new QAction(QIcon(":/icon/icon/save.svg"),"save",this);
    quickAccessBar->addAction(actionSave);
    quickAccessBar->addSeparator();

    QAction* actionUndo = new QAction(QIcon(":/icon/icon/undo.svg"),"undo",this);
    quickAccessBar->addAction(actionUndo);

    QAction* actionRedo = new QAction(QIcon(":/icon/icon/redo.svg"),"redo",this);
    quickAccessBar->addAction(actionRedo);

    quickAccessBar->addSeparator();

    QMenu* menu1 = new QMenu("Presentation File 1", this);
    menu1->setIcon(QIcon(":/icon/icon/presentationFile.svg"));
    // 创建菜单
    ...
    // 创建 QAction
    quickAccessBar->addMenuAction(menu1->menuAction());//效果等同quickAccessBar->addMenuAction(menu1);

    QMenu* menu2 = new QMenu("Presentation File 2", this);
    menu2->setIcon(QIcon(":/icon/icon/presentationFile.svg"));
    // 创建菜单
    ...
    quickAccessBar->addMenuAction(menu2, QToolButton::MenuButtonPopup);
    ...
    mSearchEditor = new QLineEdit(this);
    mSearchEditor->setMinimumWidth(150);
    mSearchEditor->setPlaceholderText("Search");
    quickAccessBar->addWidget(mSearchEditor);
}
```

上面的代码创建了一个快速访问工具栏，效果如下：

![quick-access-bar](./pic/quick-access-bar.png)

#### Right Button Group（右侧按钮组）

`Right Button Group`（右侧按钮组）是 Ribbon 界面右上角按钮组，通常用于“帮助”、“设置”、“关于”等操作。`Right Button Group`（右侧按钮组）对应的类为`SARibbonButtonGroupWidget`,它继承自`QToolBar`，你可以和操作工具栏一样使用它

SARibbon默认会创建一个`Right Button Group`（右侧按钮组），你可以调用`SARibbonBar::rightButtonGroup`函数获取`Right Button Group`的指针，然后对它进行操作

下面是`Right Button Group`的用法：

```cpp
void MainWindow::createRightButtonGroup()
{
    SARibbonBar* ribbon = ribbonBar();
    SARibbonButtonGroupWidget* rightBar = ribbon->rightButtonGroup();
    QAction* actionHelp                 = new QAction(QIcon(":/icon/icon/help.svg"),"help",this);
    QAction* actionVisible                   = new QAction(QIcon(":/icon/icon/visible-true.svg"),"Visible",this);
    rightBar->addAction(actionHelp);
    rightBar->addAction(actionVisible);
}
```

`Right Button Group`（右侧按钮组）效果如下：

![right-button-group](./pic/right-button-group.png)

#### Window Button Bar（系统标题栏按钮旁的工具栏）

`Window Button Bar`（系统标题栏按钮旁的工具栏）是 Ribbon 界面系统最大最小化合关闭按钮组，`Window Button Bar`对应的类名为`SARibbonSystemButtonBar`，它默认会创建好窗口需要的最大最小化及关闭按钮，它会自动处理窗口的状态调整系统按钮的显示，`SARibbonSystemButtonBar`内部有一个`SARibbonButtonGroupWidget`，提供了`addAction`、`addSeparator`、`addWidget`等函数，用于在最大最小化旁边添加按钮

你可以通过`SARibbonMainWindow`的`windowButtonBar`函数获取`SARibbonSystemButtonBar`指针

> **注意：**
>
> `SARibbonSystemButtonBar`不是`SARibbonBar`的子窗口，是`SARibbonMainWindow`的子窗口

`SARibbonSystemButtonBar`的使用示例如下：

```cpp
void MainWindow::createWindowButtonGroupBar()
{
    SARibbonSystemButtonBar* wbar = windowButtonBar();
    if (!wbar) {
        return;
    }
    QAction* actionLogin = new QAction(QIcon(), "Login",this);
    QAction* actionHelp = new QAction(QIcon(":/icon/icon/help.svg"),"help",this);
    ...
    wbar->addAction(a);
    wbar->addAction(actionHelp);
}
```

上面代码的效果如下：

![window-button-bar](./pic/window-button-bar.png)

#### 控制标题栏

SARibbon 的标题栏（Title Bar）是位于 Ribbon 界面最顶部的区域，用于显示应用程序的窗口标题（windowTitle）。SARibbon 允许您对标题栏高度进行定制，同时可修改其文字颜色、背景颜色以及对齐方式。在office系列软件，会通过改变标题栏的背景颜色来进行明显的提示，如软件没有注册，会把标题栏变为红色。

SARibbon通过下面方法改变标题栏的背景和字体及对齐方式（位于`SARibbonBar`类）：

```cpp
// 设置标题颜色
void setWindowTitleTextColor(const QColor& clr);
QColor windowTitleTextColor() const;

// 设置是否显示标题
void setTitleVisible(bool on = false);
bool isTitleVisible() const;

// 设置标题的背景颜色
void setWindowTitleBackgroundBrush(const QBrush& bk);
QBrush windowTitleBackgroundBrush() const;

// 设置标题的对齐方式
void setWindowTitleAligment(Qt::Alignment al);
Qt::Alignment windowTitleAligment() const;
```

你可以通过这些函数实现特殊的标题栏显示：

```cpp
void MainWindow::setWindowTitleColor()
{
    SARibbonBar* ribbon = ribbonBar();
    if (!ribbon) {
        return;
    }
    // 设置标题背景为红色
    ribbon->setWindowTitleBackgroundBrush(QColor(222, 79, 79));
    // 设置标题颜色为白色
    ribbon->setWindowTitleTextColor(Qt::white);
    // 更新显示
    ribbon->update();
}
```

上面代码显示效果如下：

![chang-title-background](./pic/chang-title-background.png)

## Ribbon布局和主题设置

### SARibbon布局

SARibbon 支持四种布局方案：宽松三行、宽松两行、紧凑三行、紧凑两行，你可以动态切换它们的模式

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

宽松模式下各个控件的布局如下图所示

![saribbonbar-level](./pic/saribbonbar-level.png)

SARibbon中把带有标题栏和tab结合一起的布局方式称之为紧凑布局（Compact），紧凑模式下各个控件的布局如下图所示

![saribbonbar-level](./pic/saribbonbar-level-2.png)

在使用SARibbonWidget时，建议使用紧凑模式，避免有较大的标题栏留白

在使用原生边框的时候（`SARibbonMainWindowStyleFlag::UseRibbonMenuBar|SARibbonMainWindowStyleFlag::UseNativeFrame`），建议使用紧凑模式，避免有较大的标题栏留白

你可以运行`example/MainWindowExample`例子，此例子可以设置不同的样式观察不同样式的ribbon风格和布局

![ribbon-style-example](./pic/ribbon-style-example.png)

SARibbon提供了`SARibbonBar::setRibbonStyle`函数，可以定义当前的布局方案，枚举`SARibbonBar::RibbonStyle`定义了四种布局方案：

- `SARibbonBar::RibbonStyleLooseThreeRow`宽松结构，3行模式(v0.x版本为`SARibbonBar::OfficeStyle`)

![SARibbonBar::RibbonStyleLooseThreeRow](./screenshot/office-3-style.png)

- `SARibbonBar::RibbonStyleLooseTwoRow`宽松结构，2行模式(v0.x版本为`SARibbonBar::OfficeStyleTwoRow`)(文字换行效果)

![SARibbonBar::RibbonStyleLooseTwoRow](./screenshot/office-2-style.png)

- `SARibbonBar::RibbonStyleCompactThreeRow`紧凑结构，3行模式(v0.x版本为`SARibbonBar::WpsLiteStyle`)

![SARibbonBar::RibbonStyleCompactThreeRow](./screenshot/wps-3-style.png)

- `SARibbonBar::RibbonStyleCompactTwoRow`紧凑结构，2行模式(v0.x版本为`SARibbonBar::WpsLiteStyleTwoRow`)(文字换行效果)

![SARibbonBar::RibbonStyleCompactTwoRow](./screenshot/wps-2-style.png)

上面可以看到，在2行模式下，文字换行会导致图标非常小，因此，建议2行模式下，不要使用文字换行，可以通过`SARibbonBar::setEnableWordWrap`函数设置是否文字换行

关于SARibbon的按钮布局，你可以参阅：[SARibbon按钮布局说明](./SARibbon-Button-Layout-Guide(cn).md)

### SARibbon尺寸设置

SARibbon 允许您精细调整各个部分的高度和间距。

```cpp
// 设置标题栏高度
ribbonBar()->setTitleBarHeight(40);

// 设置标签栏高度
ribbonBar()->setTabBarHeight(30);

// 设置Category高度
ribbonBar()->setCategoryHeight(96);

// 设置面板标题高度
ribbonBar()->setPanelTitleHeight(20);

// 设置面板间间距
ribbonBar()->setPanelSpacing(5);

// 设置工具按钮图标大小
ribbonBar()->setPanelToolButtonIconSize(QSize(32, 32));

// 设置按钮最大宽高比
ribbonBar()->setButtonMaximumAspectRatio(2.0);
```

不同的尺寸能组合出不一样的显示效果，尤其针对图标大小的显示需求，SARibbon的图标不像工具栏那样设置一个固定的图标大小，而是根据ribbonBar的高度和文字的高度来决定图标的大小，SARibbon在布局的时候，会根据`CategoryHeight`,以及文字高度来决定图标的大小，因此，你想让图标变大，有两种方法：

- 调整`CategoryHeight`，让ribbonbar变高，字体不变情况下，图标会变大
- 调整字体换行方式（`SARibbonBar::setEnableWordWrap`），如果是双行模式，设置文字不换行，则在ribbonbar高度不变的情况下，会让图标变大

各个部件的尺寸如何设置，可以通过运行`example/MainWindowExample`例子的`Size`标签页进行动态调整，并查看效果

![example-size](./pic/example-size.png)

### 切换内置主题

SARibbon 提供了多种内置主题，如 Windows 7、Office 2013、Office 2016、暗色主题等，主题定义在`SARibbonTheme`枚举类中：

```cpp
enum class SARibbonTheme
{
    RibbonThemeOffice2013,      ///< office2013主题
    RibbonThemeOffice2016Blue,  ///< office2016-蓝色主题
    RibbonThemeOffice2021Blue,  ///< office2021-蓝色主题
    RibbonThemeWindows7,        ///< win7主题
    RibbonThemeDark,            ///< 暗色主题
    RibbonThemeDark2            ///< 暗色主题2
};
```

通过`SARibbonMainWindow::setRibbonTheme`/`SARibbonWidget::setRibbonTheme`函数，可以设置Ribbon的主题，此函数的参数为`SARibbonTheme`对象

> **注：**
>
> 某些Qt版本，在构造函数设置主题会不完全生效，可以使用QTimer投放到队列最后执行：
>
> ```cpp
> MainWindow::MainWindow(QWidget* par) : SARibbonMainWindow(par)
> {
>   ...
>   QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
> }
> ```

各个主题效果如下图所示：

win7主题：

![SARibbon-theme-win7](./screenshot/SARibbon-theme-win7.png)

office2013主题：

![SARibbon-theme-office2013](./screenshot/SARibbon-theme-office2013.png)

office2016主题：

![SARibbon-theme-office2016](./screenshot/SARibbon-theme-office2016.png)

office2021主题：

![SARibbon-theme-office2021](./screenshot/SARibbon-theme-office2021.png)

dark主题：

![SARibbon-theme-dark](./screenshot/SARibbon-theme-dark.png)

dark2主题：

![SARibbon-theme-dark](./screenshot/SARibbon-theme-dark2.png)

SARibbon的主题是通过qss实现的，如果你的窗口已经存在qss样式，你需要把你现有的qss样式和ribbon的qss样式进行合并，否则，最后设置的样式将会覆盖之前设置的样式

## Ribbon的自定义

### SARibbon的自定义功能

ribbon的自定义是ribbon的一个特色，参考了office和wps的自定义界面，用户可以为自己的ribbon定义非常多的内容，甚至可以定义出一个完全和原来不一样的界面。

以下是office的自定义界面

![office的自定义界面](./screenshot/customize/customization-office-ui.png)

SARibbon参考office和wps的界面，封装了方便使用的`SARibbonCustomize**`类，包括如下5个类：

> - SARibbonCustomizeDialog
> - SARibbonCustomizeWidget
> - SARibbonCustomizeData
> - SARibbonActionsManager
> - SARibbonActionsManagerModel

实际用户使用仅会面对`SARibbonActionsManager`和`SARibbonCustomizeDialog`/`SARibbonCustomizeWidget`，其余类用户正常不会使用。

`SARibbonActionsManager`是用来管理`QAction`，把想要自定义的`QAction`添加到`SARibbonActionsManager`中管理，并可以对`QAction`进行分类，以便在`SARibbonCustomizeDialog`/`SARibbonCustomizeWidget`中显示

`SARibbonCustomizeDialog`/`SARibbonCustomizeWidget`是具体的显示窗口，`SARibbonCustomizeDialog`把`SARibbonCustomizeWidget`封装为对话框，如果要实现office那样集成到配置对话框中可以使用`SARibbonCustomizeWidget`，`SARibbonCustomizeDialog`的效果如下图所示：

![SARibbon的自定义界面](./screenshot/customize/customization-saribbon-ui.png)

### 给界面添加自定义功能

这里演示如何添加自定义功能

首先定义`SARibbonActionsManager`作为MainWindow的成员变量

```cpp
//MainWindow.h 中定义成员变量
SARibbonActionsManager* m_ribbonActionMgr;///< 用于管理所有action
```

在MainWindow的初始化过程中，还需要创建大量的`QAction`，`QAction`的父对象指定为MainWindow，另外还会生成ribbon布局，例如添加category，添加pannel等操作，在上述操作完成后添加如下步骤，自动让`SARibbonActionsManager`管理所有的`QAction`

```cpp
//MainWindow的初始化，生成QAction
//生成ribbon布局
m_ribbonActionMgr = new SARibbonActionsManager(mainWinowPtr);
m_ribbonActionMgr->autoRegisteActions(mainWinowPtr);
```

`SARibbonActionsManager`的关键函数`autoRegisteActions`可以遍历 `SARibbonMainWindow`下的所有子object，找到action并注册，并会遍历所有`SARibbonCategory`,把`SARibbonCategory`下的action按`SARibbonCategory`的title name进行分类，此函数还会把`SARibbonMainWindow`下面的action，但不在任何一个category下的作为NotInRibbonCategoryTag标签注册，默认名字会赋予not in ribbon

在需要调用`SARibbonCustomizeDialog`的地方如下操作：

```cpp
QString cfgpath = "customization.xml";
SARibbonCustomizeDialog dlg(this, this);

dlg.setupActionsManager(m_ribbonActionMgr);
dlg.fromXml(cfgpath);//调用这一步是为了把已经存在的自定义步骤加载进来，在保存时能基于原有的自定义步骤上追加
if (QDialog::Accepted == dlg.exec()) {
    dlg.applys();//应用自定义步骤
    dlg.toXml(cfgpath);//把自定义步骤保存到文件中
}
```

在MainWindow生成前还需要把自定义的内容加载，因此在构造函数最后应该加入如下语句：

```cpp
//MainWindow的构造函数最后
sa_apply_customize_from_xml_file("customization.xml", this, m_ribbonActionMgr);
```

`sa_apply_customize_from_xml_file`是`SARibbonCustomizeWidget.h`中提供的函数，直接把配置文件中的自定义内容应用到MainWindow中。

这样软件每次启动都会按照配置文件加载。