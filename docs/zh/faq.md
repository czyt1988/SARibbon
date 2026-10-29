# 常见问题说明

- ✅ **高分屏适配**：涵盖 Qt5/6 高 DPI 缩放属性与缩放策略配置
- ✅ **快捷键全局响应**：解决 Ribbon 模式下隐藏面板快捷键失效的问题
- ✅ **主题设置时机**：构造函数中主题不生效的解决方案，详见 [主题切换](./use-guide/SARibbon-theme.md)
- ✅ **暗色模式自动切换**：关闭主题跟随系统颜色模式自动切换的方法
- ✅ **SVG图标依赖**：运行环境缺少 Qt SVG 插件时的排查方法
- ✅ **多屏不同 DPI 跨屏抖动**：多显示器缩放比例不同时窗口拖动抖动的排查与解决

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
    this->setRibbonTheme(SARibbonTheme::RibbonThemeDark); 
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
    此问题在所有依赖 SVG 资源的控件中都会出现，包括 Ribbon 图标、Gallery 项等。

## 6、多显示器不同 DPI 缩放时窗口跨屏拖动抖动

当电脑连接了多个显示器且设置了不同的缩放比例（例如屏幕 A 为 200%，屏幕 B 为 150%），将窗口从高缩放屏幕拖向低缩放屏幕边缘时，窗口会发生剧烈抖动，在两个屏幕之间反复跳变。

### 原因

窗口跨越 DPI 边界时，Qt 会根据新屏幕的缩放比重新计算逻辑尺寸（Logical Size）。以屏幕 A（2.0x）到屏幕 B（1.5x）为例：

- 在屏幕 A 上：逻辑尺寸为 2816 × 2804，对应物理像素 5632 × 5608
- 窗口跨越到屏幕 B 时：逻辑尺寸被重新计算为 2112 × 2103
- 由于逻辑尺寸骤然变小，窗口右侧边缘向左猛缩，导致窗口重心回落到屏幕 A，系统再次触发 DPI 切换，陷入死循环

### 解决方法

1. **推荐开启 QWindowKit 无边框方案**

    QWindowKit 是一个跨平台的第三方无边框方案，对多屏不同 DPI 的处理更加完善，同时也支持 Windows 11 贴边特效（Snap Layout）等系统特性。编译时设置 CMake 选项：

    ```cmake
    SARIBBON_USE_FRAMELESS_LIB=ON
    ```

    !!! note
        开启 QWindowKit 需要 C++17；仓库内已包含 [QWindowKit](https://github.com/stdware/qwindowkit) 源码，开启后会自动编译，无需单独安装。具体配置请参阅 [构建指引](./build-guide/build-SARibbon.md)。

2. **使用 Qt6 + 最新版 SARibbon**

    Qt5 本身存在多屏 DPI 的已知 Bug，建议使用 Qt6（6.2 以上版本）。配合最新版 SARibbon，即使不开启 QWindowKit（使用纯 Qt 模拟无边框方案），多屏不同 DPI 也可以正常工作。

3. **如需嵌入原生 HWND 窗口，设置 `AA_DontCreateNativeWidgetSiblings`**

    开启 QWindowKit 后，如果在 SARibbonMainWindow 中嵌入了原生 HWND 窗口（如 DockWidget 内包含 Win32 原生控件），可能导致窗口句柄（Window Handle）损坏，拖动标题栏失效。此问题与 [QWindowKit Issue #32](https://github.com/stdware/qwindowkit/issues/32) 类似，解决方法是在 `main` 函数中添加：

    ```cpp
    int main(int argc, char* argv[])
    {
        QGuiApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
        QApplication a(argc, argv);
        // ...
    }
    ```

    !!! warning
        `Qt::AA_DontCreateNativeWidgetSiblings` 是 Qt 的全局属性，会影响所有 QWidget 的原生窗口创建行为。仅在确实需要嵌入原生 HWND 窗口时才设置此属性。

---

## 7、嵌入 Qt3DWindow 等原生渲染窗口时起始位置偏移

通过 `QWidget::createWindowContainer(new Qt3DWindow)` 把 Qt3D 渲染窗口（或其他原生 QWindow，如 QQuickWidget 之外的 OpenGL 窗口）嵌入 `SARibbonMainWindow` 中心区时，窗口启动时嵌入容器的起始位置出现偏移。

### 原因

`createWindowContainer` 返回的容器是**原生子窗口**（内部持有独立的原生窗口句柄），它与无边框主窗口（`UseRibbonFrame` 模式）的组合有两个已知坑：

1. **缺少 `Qt::AA_DontCreateNativeWidgetSiblings`**：Qt 默认会为原生窗口创建同级原生兄弟窗口来保证堆叠正确，这与无边框窗口（依赖整个客户区自绘）交互时会产生裁剪/偏移问题（同 [QWindowKit Issue #32](https://github.com/stdware/qwindowkit/issues/32)，见第 6 节第 3 条）。
2. **旧版本 Qt 的 `createWindowContainer` 几何同步问题**：部分 Qt 5.12/5.14 版本在窗口首次显示时，原生子窗口的初始几何没有跟随容器布局同步，表现为向左偏移若干像素，窗口 resize 一次后恢复。

在 Qt 5.15.16 + Windows 10 上按推荐用法实测：容器的全局位置与布局期望完全一致（偏移量恒等于布局自身的边距，与 DPI 无关），没有额外偏移。

### 解决方法

1. **在 `main` 函数最前面设置（QApplication 创建之前）**：

    ```cpp
    int main(int argc, char* argv[])
    {
        QGuiApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
        QApplication a(argc, argv);
        // ...
    }
    ```

2. **使用 Qt 5.15 及以上版本**（建议 Qt 6）。旧版本的 `createWindowContainer` 几何同步问题已在新版本修复。

3. **如仍出现偏移，先检查是否是布局边距**：`QVBoxLayout` 等布局默认有 9px 边距，`SARibbonMainWindow` 无边框模式还有 `setContentsMargins(2,0,2,0)` 的 2px 内容边距，这些是正常布局行为，不是窗口偏移。可用 `layout()->contentsMargins()` 与 `contentsMargins()` 分别确认。

4. **复现与量化工具**：仓库提供了 `example/Qt3DWindowExample` 复现工程，启动后自动输出主窗口/中心区/容器的几何对照数据（左偏量、DPR、原生窗口几何），支持 `--dump` 参数（打印后自动退出）。如遇偏移，请携带该工程的 `qt3d-geometry.log` 日志提 issue。

---

## 8、如何关闭系统暗色模式下的自动主题切换

`SARibbonMainWindow` 和 `SARibbonWidget` 构造时会检测操作系统的颜色模式：若系统处于暗色模式且当前主题为默认的 `RibbonThemeOffice2021Blue`，会自动切换为 `RibbonThemeDark`。

若不希望主题跟随系统颜色模式自动切换，在构造任何窗口之前调用：

```cpp
#include "SARibbonUtil.h"

SA::setEnableSystemDarkModeAutoSwitch(false);
```

该接口声明于 `SARibbonUtil.h` 的 `SA` 命名空间，默认开启。关闭后默认主题保持 `RibbonThemeOffice2021Blue`，显式的 `setRibbonTheme()` 调用不受影响。

详细说明参见 [主题切换](./use-guide/SARibbon-theme.md)。
