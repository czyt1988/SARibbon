# 常见问题说明

- ✅ **高分屏适配**：涵盖Qt5/6高DPI缩放属性与缩放策略配置
- ✅ **快捷键全局响应**：解决Ribbon模式下隐藏面板快捷键失效的问题
- ✅ **主题设置时机**：构造函数中主题不生效的解决方案，详见 [主题切换](./use-guide/SARibbon-theme.md)
- ✅ **SVG图标依赖**：运行环境缺少Qt SVG插件时的排查方法

## 1、高分屏显示问题

针对高分屏显示，有如下两个方面准备

1 - 在main函数中为QApplication设置`Qt::AA_EnableHighDpiScaling`属性

这个属性使得应用程序自动检测显示器的像素密度来实现自动缩放，示例代码如下：

```cpp
int main(int argc, char* argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication a(argc, argv);
    ......
}
```

2 - 在main函数中为QApplication设置缩放策略：`QApplication::setHighDpiScaleFactorRoundingPolicy`

Qt5.6提供了`Qt::AA_EnableHighDpiScaling`，但不能完全解决，Qt5.14开始提供了高分屏缩放策略设置`QApplication::setHighDpiScaleFactorRoundingPolicy`，同`AA_EnableHighDpiScaling`一样需要在main函数前面设置

```cpp
int main(int argc, char* argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
    QApplication a(argc, argv);
    ......
}
```

!!! tip "Qt6 说明"
    Qt6 默认启用了高DPI缩放，不再需要手动设置 `AA_EnableHighDpiScaling` 和 `AA_UseHighDpiPixmaps`（这两个属性在 Qt6 中已被移除）。如果你使用 Qt6，只需关注 `setHighDpiScaleFactorRoundingPolicy` 即可。

    更多关于Ribbon尺寸配置的信息，请参阅 [尺寸设置](./use-guide/SARibbon-size-settings.md)。

如果你使用OpenGL窗口发生了一些奇怪的问题，你可以把上面这些语句去掉看看，最新版Qt已经不需要进行上述的处理了

## 2、快捷键问题

经常有人反馈使用`SARibbon`后，没有被激活的tab页的快捷键没有响应，只有激活的标签页的快捷键才有反应，如果是传统的toolbar模式，由于action所在的toolbar一直在最前端，因此快捷键一直生效，但如果是`SARibbon`，action所在的panel是会隐藏的，隐藏后快捷键就不生效，如果想快捷键无论Panel是否隐藏都生效，设置快捷键的`shortcutContext`属性为`Qt::ApplicationShortcut`也无效，这时，可以通过Qt的`QWidget::addAction`函数把带快捷键的action添加到MainWindow中

例如：

```cpp
void MainWindow::initRibbon(){
    SARibbonCategory* categoryMain = ribbon->addCategoryPage(tr("Main"));
    SARibbonPanel* panel = categoryMain->addPanel(tr("io"));
    //这里省略action的创建过程
    QAction* actSave = new QAction(this);
    ...
    //给action设置快捷键
    actSave->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    //把action添加到panel中
    panel->addLargeAction(actSave);
    //把action添加到MainWindow中，这样切换到其他标签页，也可以响应快捷键
    addAction(actSave);
}
```

## 3、主题设置不生效

某些版本的qt，在构造函数设置主题会不完全生效，可以使用QTimer投放到队列最后执行，如：

```cpp
QTimer::singleShot(0, this, [ this ]() { 
    this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); 
    });
```

详细说明和更多主题切换用法，请参阅 [主题切换](./use-guide/SARibbon-theme.md)。

## 4、最大最小化图标不在右上角而在左上角

如果你遇到这个问题，确认编译的库文件和头文件是否匹配，通常这个问题发生在局部更新上，也就是仅仅替换了dll，而没有替换h文件导致的，有些工程在拉取了最新的SARibbon版本后，更新完直接替换lib和dll文件，头文件没有替换就会发生此问题，修复此问题的方法是确保所有文件的版本一致性，你可以把原来涉及的文件都删除掉，如果你用cmake安装的话，将涉及如下文件/文件夹：

```txt
bin/SARibbonBar.dll
include/SARibbonBar[文件夹]
lib/SARibbonBar.lib
lib/qmake/SARibbonBar[文件夹]
lib/cmake/SARibbonBar[文件夹]
SARibbonBar_amalgamate
```

关于标题栏的更多配置，请参阅 [标题栏设置](./use-guide/titlebar-setting.md)。

## 5、图标没有显示或提示 "Could not create pixmap from xxx.svg"

如果你遇到图标不显示（如最大最小化按钮有按钮但无图标），或控制台提示 `Could not create pixmap from :\SARibbon\image\resource\xxx.svg`，说明运行环境缺少Qt的SVG插件。你的程序目录下需要有 `imageformats/qsvg.dll`（Windows）或对应的 `qsvg` 插件。

解决方法：

- 运行 `windeployqt` 拉取程序依赖，自动拷贝所需插件
- 或确保环境变量 `PATH` 中能找到 `plugins/imageformats` 文件夹

!!! tip
    此问题在所有依赖SVG资源的控件中都会出现，包括Ribbon图标、Gallery项等。
