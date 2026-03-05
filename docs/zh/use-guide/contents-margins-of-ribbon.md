# SARibbon的内容边距设置

SARibbon 通过 `SARibbonMainWindow` 和 `SARibbonBar` 的 `setContentsMargins` 方法组合设置内容边距，从而实现窗口边框效果或调整 Ribbon 控件的位置。

## SARibbonMainWindow 的边距

`SARibbonMainWindow` 的 `setContentsMargins` 方法为整个主窗口设置内容边距。如果您想让窗口显示出类似边框的效果，可以在构造函数中进行如下设置：

```cpp
MainWindow::MainWindow(QWidget* par) : SARibbonMainWindow(par)
{
    setContentsMargins(5, 0, 5, 0);
}
```

效果如下（左右各有5px的边距）：

![contents-margins](../../assets/pic/contents-margins.png)

!!! info "默认边距"
    `SARibbonMainWindow` 默认会给窗口的左右设置2px的边距。如果想取消这个默认设置，可以在构造函数中调用 `setContentsMargins(0, 0, 0, 0)`。

## SARibbonBar 的边距

您也可以给 `SARibbonBar` 设置内容边距，让 RibbonBar 内的内容进一步缩进：

```cpp
MainWindow::MainWindow(QWidget* par) : SARibbonMainWindow(par)
{
    setContentsMargins(2, 0, 2, 0);
    ribbonBar()->setContentsMargins(5, 0, 5, 0);
}
```

效果如下：

![contents-margins-of-ribbon](../../assets/pic/contents-margins-ribbonbar.png)

## 组合效果与推荐配置

两种边距可以组合使用，产生不同的视觉效果：

| 场景 | MainWindow 边距 | RibbonBar 边距 | 说明 |
|------|----------------|----------------|------|
| 默认 | `(2, 0, 2, 0)` | `(0, 0, 0, 0)` | 默认效果，左右有细边框 |
| 无边框 | `(0, 0, 0, 0)` | `(0, 0, 0, 0)` | 完全无边框效果 |
| 宽边框 | `(5, 0, 5, 0)` | `(0, 0, 0, 0)` | 左右较宽的边框 |
| 缩进式 | `(2, 0, 2, 0)` | `(5, 0, 5, 0)` | 窗口有边框，Ribbon 内容额外缩进 |
| 原生边框 | `(0, 0, 0, 0)` | `(5, 0, 5, 0)` | 使用原生边框时，仅 Ribbon 内容缩进 |

!!! tip "使用原生边框时的建议"
    当使用原生边框模式（`SARibbonMainWindowStyleFlag::UseNativeFrame`）时，操作系统会绘制窗口边框，此时可以将 `SARibbonMainWindow` 的边距设为 `(0, 0, 0, 0)`，仅通过 `SARibbonBar` 的边距来调整 Ribbon 内容的位置。

!!! tip "提示"
    SARibbonMainWindow默认会设置`2px`的左右边距
