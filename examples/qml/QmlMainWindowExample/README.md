# QmlMainWindowExample

QML 版 SARibbon 综合示例，对照 widgets 版 `MainWindowExample` 的功能清单逐项对标，
用于验证 SARibbonQml 的各项能力。

## 构建与运行

```bash
cmake -S . -B build -DSARIBBON_BUILD_QML=ON -DCMAKE_PREFIX_PATH=<Qt路径>
cmake --build build
./build/bin/QmlMainWindowExample   # Windows 下在 build/bin/ 内
```

依赖 Qt 5.12+（Qml/Quick/QuickControls2/Svg）与 SARibbonCore/SARibbonQml。

## 功能清单（对应 widgets 示例）

| 区域 | 功能 | 对应 widgets |
|------|------|--------------|
| 应用按钮 | 自定义应用窗口（列表 + Cancel + Esc 关闭，**点击优先级高于菜单**） | ApplicationWidget 模式 |
| 应用按钮 | 菜单模式（test1-3 + 分隔符 + 可勾选项/快捷键文本/多级子菜单，声明 RibbonApplicationWindow 后自动让位） | USE_APPLICATION_NORMAL_MENU |
| 快速访问栏 | Save/Undo/Redo + InstantPopup 菜单按钮（标题行应用按钮之后，菜单内含二级子菜单）+ Icons/Details 单选对（`exclusive: true`，行宿主实现 QActionGroup 语义） | SARibbonQuickAccessBar |
| 右侧按钮组 | Help/Visible（系统按钮区前右对齐） | SARibbonButtonGroupWidget |
| ribbon style 面板 | 6 种样式单选（Loose/Compact × 3/2/1 行，样式传播到全部面板与按钮） | 6 个 QRadioButton |
| ribbon style 面板 | 主题 8 项下拉（Windows7/2013/2016/2021×3/Dark×2） | RibbonTheme QComboBox |
| ribbon style 面板 | 主题色覆盖（Accent 取色弹窗，改键色后派生 token 全链重绘；切回内置主题即复位） | 无对应项（widgets 版走 QSS 换肤） |
| ribbon style 面板 | 字体族选择（度量链重建+全局重排）+ 字体增大/减小（应用级度量联动） | QFontComboBox + Larger/Smaller |
| ribbon style 面板 | RTL 切换（引擎经 saIsRTL 镜像布局） | Switch to RTL |
| ribbon style 面板 | tab 对齐（左/中/右） | Alignment Center |
| 类别区 | 内容溢出时自动出现左右滚动箭头（12px 贴边），滚轮横向滚动，箭头/滚轮走 300ms OutQuad 动画（`useAnimatingScroll` 可关，`wheelScrollStep` 可调） | SARibbonCategoryLayout 滚动按钮 |
| button states 面板 | 大按钮 6 态：Normal/Checked/Disabled（含解锁）/超长文本/超短文本 | 按钮状态演示 |
| toolbutton style 面板 | 弹出三模式 × 比例混合：MenuButtonPopup（分区命中）/InstantPopup/DelayedPopup（长按）+ checkable 变体 + 禁用带菜单 | SARibbonMenu 演示 |
| toolbutton style 面板 | optionAction（右下角对角按钮，点击触发信号） | 面板 optionAction |
| toolbutton style 面板 | 分隔符（Large 比例独占一列） | addSeparator |
| widget test 面板 | 控件嵌入：ComboBox（可编辑，带图标 + 标签条四态切换）/ComboBox/TextField/CheckBox/SpinBox（带 `suffixText` 尾随单位标签） | SARibbonCtrlContainer + SARibbonLineWidgetContainer |
| Context Category 面板 | 上下文标签显隐开关（彩色 tab + 色带 + 页面切换） | setContextCategoryVisible |
| Delete 类别 | 动态面板增删（ListModel+Repeater：移除尾部/插入 0/尾/-1） | removePanel/insertPanel |
| Other 类别 | 画廊：2 组 17 项（Files/Apps），伸展分配 + 滚动 + 弹出视口 + 切组/滚动控制 + 标题三态切换（仅图标/单行/自动换行） | SARibbonGallery |
| context 上下文 1 | 页 1：控件嵌入（SpinBox/TextField）+ 按钮态；页 2：弹出按钮组 | context category 页面 |
| context 上下文 2 | 双空页（多页结构演示） | context2 |
| 事件日志 | 底部追加式日志区（所有交互写入） | textBrowser |
| footer | 主题快捷按钮 ×3 | 主题演示 |

## 与 widgets 版的已知差异

- **定制系统**（customize widget/XML 加载）：2.x widgets 专属，QML 版暂无。
- **无边框窗口**：示例使用普通 ApplicationWindow（QML 无边框为独立主题）。
- **菜单内嵌任意控件**（`SARibbonMenu::addWidget`）：QML 版菜单只渲染 RibbonMenuItem 行。
- **菜单项 shortcut 仅为展示文本**：右对齐绘制在行尾，真正的按键绑定依赖
  QAction 抽象桥（plan-04 D8 延后项），故不会响应键盘。
- 面板 optionAction 已恢复完整（引擎预留 + 对角按钮渲染 + 触发信号，
  Debug/Release 双验证；早期 Qt 6.7.3 Debug 绑定形状问题见 NOTES B48）。

## 一致性测试

`tests/qml/tst_conformance_qml.cpp`（22 用例）与示例同步维护：
面板装箱黄金几何、tab 切换、按钮点击/弹出/禁用、控件嵌入、上下文标签、画廊、
六样式传播、分隔符、快速访问栏/右组、对齐/最小模式、RTL、optionAction、应用窗口、
菜单勾选/快捷键/多级子菜单（索引路径寻址 + 叶子渲染断言）、
画廊标题三态（度量逐项对齐 core `calcGalleryCellMetrics` + 叶子 wrapMode/字号/可见性）、
悬停信号（真实 mouseMove 进出栅格）与 selectable 语义（拒绝成为当前项但不拒绝激活）、
主题自定义（键色覆盖 + 派生 token 重算 + JSON/文件加载 + `RibbonThemeUserDefine`
保持自定义调色板 + 系统暗色开关桥接，并用 grabWindow 断言渲染色真的变了）、
容器尾随标签与标签条开关（后缀从容器里切出而非从控件里扣，
`enableShowIcon`/`enableShowTitle` 逐槽位让宽并断言无漂移）、
标题行按钮排互斥（默认为非互斥、打开开关不追溯取消勾选、
未受影响的兄弟按钮不产生 `toggled`、`checkedButton()` 对齐 `QActionGroup::checkedAction`）、
类别滚动（溢出时只出现尾随箭头、箭头矩形等于 core `scrollButtonRects` 输出、
真实点击箭头步进半视口并被 `clampScrollOffset` 夹住、滚轮走 core 的 delta 优先级与
×2 / ÷2 缩放、动画途中滚轮被丢弃、内容放得下的类别必须 `ignore` 滚轮而不是吃掉它）。
