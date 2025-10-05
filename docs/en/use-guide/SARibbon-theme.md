# SARibbon Theme Switching

SARibbon ships with several built-in themes: Windows 7, Office 2013, Office 2016, dark variants, etc.  
They are defined in the `SARibbonTheme` enum:

```cpp
enum class SARibbonTheme
{
    RibbonThemeOffice2013,      ///< Office 2013 look
    RibbonThemeOffice2016Blue,  ///< Office 2016 blue
    RibbonThemeOffice2021Blue,  ///< Office 2021 blue
    RibbonThemeWindows7,        ///< Windows 7 look
    RibbonThemeDark,            ///< Dark theme
    RibbonThemeDark2            ///< Dark theme #2
};
```

Apply a theme through  
`SARibbonMainWindow::setRibbonTheme()` / `SARibbonWidget::setRibbonTheme()`:

```cpp
mainWindow->setRibbonTheme(SARibbonTheme::RibbonThemeDark);
```

!!! warning
    On some Qt versions calling `setRibbonTheme` inside the constructor does **not** fully take effect.  
    Defer it with a zero-timeout timer:

    ```cpp
    MainWindow::MainWindow(QWidget* par) : SARibbonMainWindow(par)
    {
        ...
        QTimer::singleShot(0, this, [this] {
            setRibbonTheme(SARibbonTheme::RibbonThemeDark);
        });
    }
    ```

Preview of each theme:

Windows 7  
![SARibbon-theme-win7](../../assets/screenshot/SARibbon-theme-win7.png)

Office 2013  
![SARibbon-theme-office2013](../../assets/screenshot/SARibbon-theme-office2013.png)

Office 2016  
![SARibbon-theme-office2016](../../assets/screenshot/SARibbon-theme-office2016.png)

Office 2021  
![SARibbon-theme-office2021](../../assets/screenshot/SARibbon-theme-office2021.png)

Dark  
![SARibbon-theme-dark](../../assets/screenshot/SARibbon-theme-dark.png)

Dark2  
![SARibbon-theme-dark2](../../assets/screenshot/SARibbon-theme-dark2.png)

All themes are implemented with standard **QSS**.  
If your application already applies its own style sheets, **merge** the Ribbon QSS into yours; otherwise the last sheet loaded will overwrite the previous ones.