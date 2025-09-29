# SARibbon主题切换

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