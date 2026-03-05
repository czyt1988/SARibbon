# Ribbon尺寸设置

SARibbon 允许您精细调整各个部分的高度和间距，以适应不同的界面需求。

## 基本尺寸设置

以下是各部件的尺寸调整方法：

```cpp
SARibbonBar* ribbon = ribbonBar();

// 设置标题栏高度（默认值取决于布局模式）
ribbon->setTitleBarHeight(40);

// 设置标签栏高度
ribbon->setTabBarHeight(30);

// 设置Category高度（面板内容区域的总高度）
ribbon->setCategoryHeight(96);

// 设置面板标题高度
ribbon->setPanelTitleHeight(20);

// 设置面板间的间距
ribbon->setPanelSpacing(5);

// 设置工具按钮的小图标和大图标的大小
ribbon->setPanelToolButtonIconSize(QSize(20, 20), QSize(32, 32));

// 设置按钮最大宽高比（控制按钮的最大宽度）
ribbon->setButtonMaximumAspectRatio(1.4);
```

## 图标大小的动态计算

SARibbon 的图标大小并非像传统工具栏那样设置一个固定值，而是根据 RibbonBar 的高度和文字的高度动态计算。在布局时，SARibbon 会根据 `CategoryHeight` 以及文字高度来自动决定图标的大小。

如果您希望让图标变大，有两种方法：

- **增大 Category 高度**：调用 `setCategoryHeight()` 让 RibbonBar 变高，字体不变的情况下，图标区域会自动增大
- **调整文字换行方式**：调用 `SARibbonBar::setEnableWordWrap(false)` 关闭文字换行。在双行模式下，关闭换行后文字只占一行，从而为图标腾出更多空间

## 各部件尺寸示意

下表列出了各尺寸参数的作用及建议值：

| 参数 | 设置方法 | 说明 | 默认值参考 |
|------|---------|------|-----------|
| 标题栏高度 | `setTitleBarHeight(h)` | 仅在宽松模式下可见 | 约30px |
| 标签栏高度 | `setTabBarHeight(h)` | 标签页标签的高度 | 约28px |
| Category高度 | `setCategoryHeight(h)` | 面板内容区域的总高度 | 约96px |
| 面板标题高度 | `setPanelTitleHeight(h)` | 面板底部标题区域的高度 | 约21px |
| 面板间距 | `setPanelSpacing(n)` | 相邻面板之间的水平间距 | 0px |
| 大图标尺寸 | `setPanelToolButtonIconSize(s, l)` | Large 按钮的图标大小 | 32x32 |
| 小图标尺寸 | `setPanelToolButtonIconSize(s, l)` | Small/Medium 按钮的图标大小 | 20x20 |
| 按钮宽高比 | `setButtonMaximumAspectRatio(r)` | 按钮最大宽度=高度*此比值 | 1.4 |

## 尺寸调整的完整示例

以下示例展示了如何创建一个紧凑风格的 Ribbon 界面：

```cpp
MainWindow::MainWindow(QWidget* par) : SARibbonMainWindow(par)
{
    SARibbonBar* ribbon = ribbonBar();

    // 使用紧凑三行模式
    ribbon->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);

    // 缩小Category高度，使界面更紧凑
    ribbon->setCategoryHeight(80);

    // 缩小面板标题高度
    ribbon->setPanelTitleHeight(16);

    // 设置较小的图标
    ribbon->setPanelToolButtonIconSize(QSize(16, 16), QSize(24, 24));

    // 增大面板间距，增加视觉分隔
    ribbon->setPanelSpacing(4);
}
```

!!! example "尺寸设置例子"
    各个部件的尺寸如何设置，可以通过运行`example/MainWindowExample`例子的`Size`标签页进行动态调整，并查看效果
    ![example-size](../../assets/pic/example-size.png)
