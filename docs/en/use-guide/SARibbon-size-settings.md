# Ribbon Size Settings

SARibbon allows you to finely adjust the height and spacing of various parts.

```cpp
// Set the title bar height
ribbonBar()->setTitleBarHeight(40);

// Set the tab bar height
ribbonBar()->setTabBarHeight(30);

// Set the Category height
ribbonBar()->setCategoryHeight(96);

// Set the panel title height
ribbonBar()->setPanelTitleHeight(20);

// Set the spacing between panels
ribbonBar()->setPanelSpacing(5);

// Set the icon size of panel tool buttons
ribbonBar()->setPanelToolButtonIconSize(QSize(32, 32));

// Set the maximum aspect ratio of buttons
ribbonBar()->setButtonMaximumAspectRatio(2.0);
```

Different sizes can be combined to produce different display effects, especially for the display requirements of icon sizes. Unlike toolbars that set a fixed icon size, the icons of SARibbon are determined by the height of the ribbonBar and the height of the text. When SARibbon is laid out, the icon size is determined according to the `CategoryHeight` and the text height. Therefore, there are two ways to make the icons larger:

- Adjust `CategoryHeight` to increase the height of the ribbonbar. With the font unchanged, the icons will become larger.
- Adjust the font line wrap mode (`SARibbonBar::setEnableWordWrap`). In double-line mode, if text wrapping is disabled, the icons will become larger while the height of the ribbonbar remains unchanged.

How to set the size of each component can be dynamically adjusted through the `Size` tab of the `example/MainWindowExample` example, and the effect can be viewed.

![example-size](../../assets/pic/example-size.png)