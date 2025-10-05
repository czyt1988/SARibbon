# SARibbon Layout Methods

SARibbon supports four layout schemes: loose three-row, loose two-row, compact three-row, and compact two-row. You can dynamically switch between these modes.

The style enumeration definitions of SARibbon are as follows (located in SARibbonBar):

```cpp
enum RibbonStyleFlag
{
    RibbonStyleLoose    = 0x0001,  // bit:0000 0001
    RibbonStyleCompact  = 0x0002,  // bit:0000 0010
    RibbonStyleThreeRow = 0x0010,  // bit:0001 0000
    RibbonStyleTwoRow   = 0x0020,  // bit:0010 0000

    RibbonStyleLooseThreeRow   = RibbonStyleLoose | RibbonStyleThreeRow,    ///< Loose structure, 3-row mode
    RibbonStyleCompactThreeRow = RibbonStyleCompact | RibbonStyleThreeRow,  ///< Compact structure, 3-row mode
    RibbonStyleLooseTwoRow     = RibbonStyleLoose | RibbonStyleTwoRow,      ///< Loose structure, 2-row mode
    RibbonStyleCompactTwoRow   = RibbonStyleCompact | RibbonStyleTwoRow     ///< Compact structure, 2-row mode
};
```

The layout of each control in loose mode is shown in the following figure:

![saribbonbar-level](../../assets/pic/saribbonbar-level.png)

In SARibbon, the layout that combines the title bar and tab is called compact layout (Compact). The layout of each control in compact mode is shown in the following figure:

![saribbonbar-level](../../assets/pic/saribbonbar-level-2.png)

When using SARibbonWidget, it is recommended to use the compact mode to avoid large blank spaces in the title bar.

When using the native border (`SARibbonMainWindowStyleFlag::UseRibbonMenuBar|SARibbonMainWindowStyleFlag::UseNativeFrame`), it is recommended to use the compact mode to avoid large blank spaces in the title bar.

You can run the `example/MainWindowExample` example, which allows you to set different styles to observe different ribbon styles and layouts.

![ribbon-style-example](../../assets/pic/ribbon-style-example.png)

SARibbon provides the `SARibbonBar::setRibbonStyle` function, which can define the current layout scheme. The enumeration `SARibbonBar::RibbonStyle` defines four layout schemes:

- `SARibbonBar::RibbonStyleLooseThreeRow`: Loose structure, 3-row mode (equivalent to `SARibbonBar::OfficeStyle` in v0.x version)

![SARibbonBar::RibbonStyleLooseThreeRow](../../assets/screenshot/office-3-style.png)

- `SARibbonBar::RibbonStyleLooseTwoRow`: Loose structure, 2-row mode (equivalent to `SARibbonBar::OfficeStyleTwoRow` in v0.x version) (with text wrapping effect)

![SARibbonBar::RibbonStyleLooseTwoRow](../../assets/screenshot/office-2-style.png)

- `SARibbonBar::RibbonStyleCompactThreeRow`: Compact structure, 3-row mode (equivalent to `SARibbonBar::WpsLiteStyle` in v0.x version)

![SARibbonBar::RibbonStyleCompactThreeRow](../../assets/screenshot/wps-3-style.png)

- `SARibbonBar::RibbonStyleCompactTwoRow`: Compact structure, 2-row mode (equivalent to `SARibbonBar::WpsLiteStyleTwoRow` in v0.x version) (with text wrapping effect)

![SARibbonBar::RibbonStyleCompactTwoRow](../../assets/screenshot/wps-2-style.png)

As can be seen above, in the 2-row mode, text wrapping will cause the icons to be very small. Therefore, it is recommended not to use text wrapping in 2-row mode. You can set whether to wrap text through the `SARibbonBar::setEnableWordWrap` function.

For the button layout of SARibbon, you can refer to: [Explanation of Ribbon Button Layout](./layout-of-ribbonbutton.md)