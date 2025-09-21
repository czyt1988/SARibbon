
# SARibbon 库使用指南

SARibbon 是一个用于创建现代化 Ribbon 界面的 Qt 库，其界面风格类似于 Microsoft Office 或 WPS。它专为复杂桌面应用程序设计，能有效组织大量功能，常见于工业软件的界面开发。

在开始编码之前，您需要将 SARibbon 库集成到您的 Qt 项目中。最简单的方式是**静态嵌入**，即直接将源文件`SARibbon.h`和`SARibbon.cpp`拷贝到您的工程即可。

# Ribbon界面和传统menubar+toolbar的异同

传统的menubar+toolbar是无法直接转化为ribbon界面的，Ribbon不仅仅是一个带`QToolBar`的工具栏，与传统菜单栏和工具栏相比它有如下特点：

- Ribbon的按钮渲染方式有一个明显的改变，导致无法直接使用ToolButton来模拟，SARibbon使用`SARibbonToolButton`针对Ribbon的按钮进行了重新布局和渲染
- Ribbon还有一种叫上下文标签页`Context Category`的特殊标签页，例如，在Office Word中选中一张图片，会自动出现一个“图片编辑”标签页，提供裁剪、旋转等图片专属功能，取消选择后该标签页自动隐藏
- Ribbon界面会带有一些特殊的控件，例如Gallery（word的样式选择就是Gallery控件）


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

![](./pic/mainwindow-ribbonbar+ribbonframe.png)

2. `SARibbonMainWindowStyleFlag::UseRibbonMenuBar|SARibbonMainWindowStyleFlag::UseNativeFrame`

此风格为使用ribbon风格菜单栏和原生边框，这个界面的好处是支持操作系统的边框特效，例如win11的全局窗口快捷键，以及一些边框特效

![](./pic/mainwindow-ribbonbar+nativeframe.png)

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

## 创建Ribbon风格的`Widget`

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

# Ribbon界面创建

## Ribbon界面的层次结构

理解 SARibbon 的层次结构是构建界面的基础。其结构清晰，层次分明：

1.  **`SARibbonBar`**: 最顶层的Ribbon菜单，管理整个 Ribbon 界面。它包含：
    *   **`Application Button`**: 左上角的主菜单按钮。
    *   **`Quick Access Bar`**: 位于最顶部的快速工具栏。
    *   **`SARibbonTabBar`**: 显示各个 `Category` 标签页的标签栏。
    *   **`SARibbonStackedWidget`**: 一个堆栈窗口，用于显示当前激活的 `Category` 内容。
    *   **`Right Button Group`**: 位于右上角的按钮组。
    *   **`Window Button Bar`** : 系统按钮，通过它可以在系统按钮旁边添加按钮。

2.  **`SARibbonCategory` (分类页)**: 代表一个功能场景，如“主页”、“插入”、“设计”。每个 `Category` 包含一个或多个 `Panel`。它被添加到 `SARibbonTabBar` 上作为一个标签，并在 `SARibbonStackedWidget` 中管理其内容。

3.  **`SARibbonPanel` (面板)**: 是 `Category` 内的功能分组容器，类似于一个增强版的工具栏。一个 `Panel` 可以包含多个 `QAction`（表现为按钮）、`QWidget`（如组合框、编辑框）或 `SARibbonButtonGroupWidget`（按钮组）。

4.  **`SARibbonContextCategory` (上下文分类页)**: 一种特殊的 `Category`，默认隐藏，仅在特定条件（如选中特定对象）下才显示。

其布局如下图所示

![](./pic/saribbonbar-level.png)

## 创建一个ribbon页面

### category(分类页)

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

### panel(面板)

#### 创建面板并添加动作

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

#### 创建菜单

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

#### 面板的布局占位说明

SARibbon 面板的布局目前有3种占位标志。当您调用 `addLargeAction`、`addMediumAction` 或 `addSmallAction` 时，您不仅是在添加一个按钮，还在指定它在网格布局中的“占位大小”。

*   **Large Action**: 高度上占据2行或3行（取决于当前是两行还是三行模式），显示大图标和文字。
*   **Medium Action**: 高度上占据1行或1.5行的网格（取决于当前是两行还是三行模式），通常显示图标和文字。
*   **Small Action**: 高度上占据1行

这些行占比在不同的布局方案会有不同的呈现效果

SARibbonPanel支持两种布局方案，分别是2行模式和3行模式，通过`SARibbonBar::setRibbonStyle`函数你可以设置不同的布局风格

对于3行模式，上诉三种行占位的效果如下图所示

![](./pic/pannelLayout3row.png)

对于2行模式，上诉三种行占位的效果如下图所示

![](./pic/pannelLayout2row.png)



#### 在面板中添加复杂控件 (Widget)

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

![](./pic/widget-in-panel.png)

#### 使用按钮组 (Button Group)

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

![](./pic/button-group.png)

#### 使用 Gallery (画廊)

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
	galleryActions1.append(createAction("Drive File Four Word", ":/gallery-icon/icon/gallery/Drive-File.svg"));
	galleryActions1.append(createAction("Dropbox File", ":/gallery-icon/icon/gallery/Dropbox-File.svg"));
	galleryActions1.append(createAction("Email File", ":/gallery-icon/icon/gallery/Email-File.svg"));
	galleryActions1.append(createAction("Encode File", ":/gallery-icon/icon/gallery/Encode-File.svg"));
	galleryActions1.append(createAction("Favorit File", ":/gallery-icon/icon/gallery/Favorit-File.svg"));
	galleryActions1.append(createAction("File Error", ":/gallery-icon/icon/gallery/File-Error.svg"));
	galleryActions1.append(createAction("File Read Only", ":/gallery-icon/icon/gallery/File-Readonly.svg"));
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

![](./pic/saribbon-gallery.png)

此gallery展开后效果如下：

![](./pic/saribbon-gallery-expand.png)

### context category(上下文标签页)

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

![](./screenshot/SARibbonBar-Context-Category.png)

为了区分上下文标签页和普通的标签页，上下文标签页会有特殊的颜色进行标识，如上图所示。这个标识的颜色可以通过`SARibbonBar::setContextCategoryColor`设置

### quick access bar（快速访问工具栏）

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

	QAction* customize = createAction("customize", ":/icon/icon/customize0.svg", "customize2");
	quickAccessBar->addAction(customize);
	connect(customize, &QAction::triggered, this, &MainWindow::onActionCustomizeTriggered);

	QAction* actionCustomizeAndSave = createAction("customize and save", ":/icon/icon/customize.svg");
	quickAccessBar->addAction(actionCustomizeAndSave);
	connect(actionCustomizeAndSave, &QAction::triggered, this, &MainWindow::onActionCustomizeAndSaveTriggered);

	QAction* actionCustomizeAndSaveWithApply = createAction("customize and save with apply", ":/icon/icon/customize.svg");
	quickAccessBar->addAction(actionCustomizeAndSaveWithApply);
	connect(actionCustomizeAndSaveWithApply, &QAction::triggered, this, &MainWindow::onActionCustomizeAndSaveWithApplyTriggered);

	mSearchEditor = new QLineEdit(this);
	mSearchEditor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	mSearchEditor->setMinimumWidth(150);
	mSearchEditor->setPlaceholderText("Search");
	auto widgetAct = quickAccessBar->addWidget(mSearchEditor);
	connect(mSearchEditor, &QLineEdit::editingFinished, this, [ this ]() {
		this->mTextedit->append(this->mSearchEditor->text());
	});
	widgetAct->setVisible(true);
}
```

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

`SARibbonCustomizeWidget`可以让你把自定义界面嵌入到你的程序的设置窗口中

**方式二：使用内置的 `SARibbonCustomizeDialog`**

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



