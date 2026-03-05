# 标题栏设置

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

![chang-title-background](../../assets/pic/chang-title-background.png)

## 常用场景

| 场景 | 推荐设置 | 说明 |
|------|---------|------|
| 未注册/试用提示 | 红色背景 + 白色文字 | 醒目提示用户软件状态 |
| 只读模式 | 灰色背景 + 深色文字 | 表示当前文档不可编辑 |
| 隐藏标题栏 | `setTitleVisible(false)` | 适用于紧凑模式、原生边框模式 |
| 自定义对齐 | `setWindowTitleAligment(Qt::AlignLeft)` | 标题左对齐，类似 WPS 风格 |

## 动态恢复标题栏

在某些应用场景中，你可能需要动态改变标题栏颜色后再恢复为默认状态：

```cpp
void MainWindow::resetTitleBar()
{
    SARibbonBar* ribbon = ribbonBar();
    if (!ribbon) {
        return;
    }
    // 恢复为透明背景（使用主题默认色）
    ribbon->setWindowTitleBackgroundBrush(Qt::NoBrush);
    // 恢复为默认文字颜色（跟随主题）
    ribbon->setWindowTitleTextColor(QColor());
    ribbon->update();
}
```

!!! tips "提示"
    标题栏设置仅在宽松模式（Loose）下可见。在紧凑模式下，标题栏和 Tab 栏合并，标题栏背景色设置不会有明显效果，但文字颜色仍然生效。
