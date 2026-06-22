# Python 绑定使用指南

本文档详细介绍如何在 Python 中使用 SARibbon 绑定（PyQtSARibbon）。

## 子文档导航

| 文档 | 内容 |
|------|------|
| [构建 Python 绑定](./build-python-bindings.md) | 编译安装 PyQtSARibbon |
| [使用 Python 绑定](./use-python-bindings.md) | 在 Python 中使用 SARibbon |

## 快速开始

### 基本示例

下面是一个最简单的 PyQtSARibbon 示例：

``python
import sys
from PyQt5.QtWidgets import QApplication, QAction
from PyQtSARibbon import SARibbonBar, SARibbonCategory, SARibbonPanel

# 创建应用程序
app = QApplication(sys.argv)

# 创建 Ribbon 工具栏
ribbon = SARibbonBar()

# 创建类别（标签页）
home_category = SARibbonCategory("主页")
ribbon.addCategoryPage(home_category)

# 创建面板
clipboard_panel = SARibbonPanel("剪贴板", home_category)
home_category.addPanel(clipboard_panel)

# 添加动作
paste_action = QAction("粘贴", clipboard_panel)
clipboard_panel.addLargeAction(paste_action)

cut_action = QAction("剪切", clipboard_panel)
clipboard_panel.addSmallAction(cut_action)

copy_action = QAction("复制", clipboard_panel)
clipboard_panel.addSmallAction(copy_action)

# 显示 Ribbon
ribbon.show()

sys.exit(app.exec_())
``

## 核心概念

PyQtSARibbon 的界面层次结构：

`
SARibbonBar (工具栏)
└── SARibbonCategory (类别/标签页)
    └── SARibbonPanel (面板)
        ├── QAction (动作/按钮)
        └── SARibbonGallery (画廊)
`

### SARibbonBar - 主工具栏

SARibbonBar 是整个 Ribbon 控件的容器。

``python
from PyQtSARibbon import SARibbonBar

# 创建 Ribbon 工具栏
ribbon = SARibbonBar()

# 设置样式
ribbon.setRibbonStyle(SARibbonBar.RibbonStyleLooseThreeRow)

# 获取当前选中的类别索引
index = ribbon.currentIndex()

# 设置当前类别
ribbon.setCurrentIndex(1)
``

### SARibbonCategory - 类别（标签页）

SARibbonCategory 代表 Ribbon 中的一个标签页。

``python
from PyQtSARibbon import SARibbonCategory

# 创建类别
category = SARibbonCategory("主页")

# 添加到 Ribbon
ribbon.addCategoryPage(category)

# 获取类别名称
name = category.categoryName()

# 设置类别名称
category.setCategoryName("开始")
``

### SARibbonPanel - 面板

SARibbonPanel 是类别中的功能分组。

``python
from PyQtSARibbon import SARibbonPanel

# 创建面板
panel = SARibbonPanel("剪贴板", category)

# 添加到类别
category.addPanel(panel)

# 添加动作
panel.addLargeAction(action)  # 大按钮
panel.addSmallAction(action)  # 小按钮
panel.addSeparator()          # 分隔符
``

## 添加动作到面板

### 大按钮（Large Action）

大按钮显示图标和文字，图标在上方。

``python
from PyQt5.QtWidgets import QAction
from PyQt5.QtGui import QIcon

# 创建动作
paste_action = QAction(QIcon("paste.png"), "粘贴", panel)
paste_action.triggered.connect(lambda: print("粘贴被点击"))

# 添加大按钮
panel.addLargeAction(paste_action)
``

### 小按钮（Small Action）

小按钮只显示小图标和文字。

``python
# 创建动作
cut_action = QAction("剪切", panel)
copy_action = QAction("复制", panel)

# 添加小按钮
panel.addSmallAction(cut_action)
panel.addSmallAction(copy_action)
``

### 中按钮（Medium Action）

中按钮显示图标和文字并排。

``python
# 添加中按钮
panel.addMediumAction(action)
``

### 分隔符

``python
# 添加分隔符
panel.addSeparator()
``

## 创建画廊（Gallery）

画廊是一种下拉菜单，显示多个选项。

``python
from PyQtSARibbon import SARibbonGallery

# 创建画廊面板
gallery_panel = SARibbonPanel("样式", category)
category.addPanel(gallery_panel)

# 添加画廊
gallery = gallery_panel.addGallery()

# 创建动作列表
actions = []
for i in range(12):
    action = QAction(f"样式 {i+1}")
    actions.append(action)

# 添加到画廊
group = gallery.addCategoryActions("预设样式", actions)

# 设置当前显示
gallery.setCurrentViewGroup(group)
``

## 上下文类别

上下文类别是在特定条件下显示的标签页（例如选中图片时显示"图片工具"）。

``python
from PyQt5.QtCore import Qt

# 创建上下文类别
context = ribbon.addContextCategory("绘图工具", Qt.red)

# 在上下文中添加类别
format_category = context.addCategoryPage("格式")

# 添加面板
format_panel = format_category.addPanel("形状")
format_panel.addLargeAction(fill_action)

# 显示上下文类别
ribbon.setContextCategoryVisible(context, True)

# 隐藏上下文类别
ribbon.setContextCategoryVisible(context, False)
``

## 完整示例

下面是一个完整的 Ribbon 应用程序示例：

``python
import sys
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QAction, QTextEdit,
    QFileDialog, QMessageBox
)
from PyQt5.QtGui import QFont
from PyQt5.QtCore import Qt
from PyQtSARibbon import SARibbonBar, SARibbonCategory, SARibbonPanel

class RibbonWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("SARibbon Python 示例")
        self.resize(1024, 680)
        
        # 创建 Ribbon
        self.ribbon = SARibbonBar()
        self.setMenuWidget(self.ribbon)
        
        # 创建中央编辑器
        self.editor = QTextEdit()
        self.editor.setFont(QFont("Consolas", 11))
        self.setCentralWidget(self.editor)
        
        # 设置 Ribbon 界面
        self._setup_ribbon()
    
    def _setup_ribbon(self):
        # === 主页标签 ===
        home = SARibbonCategory("主页")
        self.ribbon.addCategoryPage(home)
        
        # 文件面板
        file_panel = SARibbonPanel("文件", home)
        home.addPanel(file_panel)
        
        new_action = QAction("新建", file_panel)
        new_action.setShortcut("Ctrl+N")
        new_action.triggered.connect(self._on_new)
        file_panel.addLargeAction(new_action)
        
        open_action = QAction("打开", file_panel)
        open_action.setShortcut("Ctrl+O")
        open_action.triggered.connect(self._on_open)
        file_panel.addLargeAction(open_action)
        
        save_action = QAction("保存", file_panel)
        save_action.setShortcut("Ctrl+S")
        save_action.triggered.connect(self._on_save)
        file_panel.addLargeAction(save_action)
        
        # 编辑面板
        edit_panel = SARibbonPanel("编辑", home)
        home.addPanel(edit_panel)
        
        undo_action = QAction("撤销", edit_panel)
        undo_action.triggered.connect(self.editor.undo)
        edit_panel.addSmallAction(undo_action)
        
        redo_action = QAction("重做", edit_panel)
        redo_action.triggered.connect(self.editor.redo)
        edit_panel.addSmallAction(redo_action)
        
        edit_panel.addSeparator()
        
        cut_action = QAction("剪切", edit_panel)
        cut_action.triggered.connect(self.editor.cut)
        edit_panel.addSmallAction(cut_action)
        
        copy_action = QAction("复制", edit_panel)
        copy_action.triggered.connect(self.editor.copy)
        edit_panel.addSmallAction(copy_action)
        
        paste_action = QAction("粘贴", edit_panel)
        paste_action.triggered.connect(self.editor.paste)
        edit_panel.addSmallAction(paste_action)
        
        # === 查看标签 ===
        view = SARibbonCategory("查看")
        self.ribbon.addCategoryPage(view)
        
        zoom_panel = SARibbonPanel("缩放", view)
        view.addPanel(zoom_panel)
        
        zoom_in = QAction("放大", zoom_panel)
        zoom_in.triggered.connect(self._zoom_in)
        zoom_panel.addLargeAction(zoom_in)
        
        zoom_out = QAction("缩小", zoom_panel)
        zoom_out.triggered.connect(self._zoom_out)
        zoom_panel.addLargeAction(zoom_out)
    
    def _on_new(self):
        self.editor.clear()
    
    def _on_open(self):
        path, _ = QFileDialog.getOpenFileName(
            self, "打开文件", "", "文本文件 (*.txt);;所有文件 (*)"
        )
        if path:
            with open(path, "r", encoding="utf-8") as f:
                self.editor.setPlainText(f.read())
    
    def _on_save(self):
        QMessageBox.information(self, "提示", "保存功能已触发")
    
    def _zoom_in(self):
        self.editor.zoomIn(1)
    
    def _zoom_out(self):
        self.editor.zoomOut(1)


def main():
    app = QApplication(sys.argv)
    window = RibbonWindow()
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
``

## 可用类列表

PyQtSARibbon 提供以下类：

| 类名 | 说明 |
|------|------|
| SARibbonBar | 主 Ribbon 工具栏 |
| SARibbonCategory | Ribbon 类别（标签页） |
| SARibbonPanel | 面板（功能分组） |
| SARibbonToolButton | Ribbon 按钮 |
| SARibbonGallery | 画廊控件 |
| SARibbonGalleryGroup | 画廊分组 |
| SARibbonGalleryItem | 画廊项目 |
| SARibbonContextCategory | 上下文类别 |
| SARibbonButtonGroupWidget | 按钮组 |
| SARibbonTabBar | 标签栏 |
| SARibbonQuickAccessBar | 快速访问工具栏 |

## 面板方法参考

### 添加动作

| 方法 | 说明 |
|------|------|
| addLargeAction(action) | 添加大按钮（图标在上，文字在下） |
| addMediumAction(action) | 添加中按钮（图标和文字并排） |
| addSmallAction(action) | 添加小按钮（只显示图标） |

### 添加控件

| 方法 | 说明 |
|------|------|
| addLargeWidget(widget) | 添加大控件 |
| addMediumWidget(widget) | 添加中控件 |
| addSmallWidget(widget) | 添加小控件 |

### 其他方法

| 方法 | 说明 |
|------|------|
| addGallery() | 添加画廊控件 |
| addMenu(menu) | 添加菜单按钮 |
| addSeparator() | 添加分隔符 |

## 下一步

- 查看 pyexamples/ribbon_demo.py 获取完整示例代码
- 参考 C++ API 文档了解更多功能
