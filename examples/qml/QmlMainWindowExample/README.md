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
| widget test 面板 | 颜色按钮 ×2：Font Color（`ColorUnderIcon`，图标下方色带）+ Fill Color（`ColorFillToIcon`，选中色填充图标框）；点击色带区直接触发 `colorClicked`，点击箭头弹出标准色 + 深浅色板 + 自定义色 + 无颜色项 | SARibbonColorToolButton（color 分类页） |
| Context Category 面板 | 上下文标签显隐开关（彩色 tab + 色带 + 页面切换） | setContextCategoryVisible |
| Delete 类别 | 动态面板增删（ListModel+Repeater：移除尾部/插入 0/尾/-1） | removePanel/insertPanel |
| Other 类别 | 画廊：2 组 17 项（Files/Apps），伸展分配 + 滚动 + 弹出视口 + 切组/滚动控制 + 标题三态切换（仅图标/单行/自动换行） | SARibbonGallery |
| Other 类别 | 定制对话框：左侧命令目录（tag 过滤 + 搜索）、右侧 ribbon 树预览（三档显示范围）、中间增删/上下移/重命名/新类别/新组/显隐；编辑先记为 core 定制记录并在影子树上预览，只有"确定"才落到真树，"取消"整批丢弃 | SARibbonCustomizeDialog |
| context 上下文 1 | 页 1：控件嵌入（SpinBox/TextField）+ 按钮态；页 2：弹出按钮组 | context category 页面 |
| context 上下文 2 | 双空页（多页结构演示） | context2 |
| 事件日志 | 底部追加式日志区（所有交互写入） | textBrowser |
| footer | 主题快捷按钮 ×3 | 主题演示 |

## 与 widgets 版的已知差异

- **定制对话框按稳定字符串 key 寻址**：widgets 版靠 `QAction*` 指针 + `objectName`，
  QML 版没有 action 桥（plan-04 D8 延后项），改用 `RibbonActionRegistry` 生成的
  key 加宿主树查询 API。记录层与 XML 格式两端共用 core 实现，故 widgets 写出的
  配置文件 QML 能直接加载并 apply，反之亦然。快速访问栏的用户勾选定制仍缺。
- **无边框窗口**：示例使用普通 ApplicationWindow（QML 无边框为独立主题）。
- **菜单内嵌任意控件**（`SARibbonMenu::addWidget`）：QML 版菜单只渲染 RibbonMenuItem 行。
- **颜色菜单不含取色对话框**：`RibbonColorMenu` 的"自定义颜色"行只发
  `customColorRequested()`，由示例接一个色块弹窗（`customColorPicker`）回灌
  `addCustomColor()`。QColorDialog 属于 widgets/QtQuick Dialogs，不进本模块。
  菜单内部的三张色块网格逐像素对齐 widgets，外框（Popup + Column）是 QML 原生的，
  与 QMenu 的尺寸协商结果不同。
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

`tests/qml/tst_color_qml.cpp`（12 用例）覆盖颜色控件族：
网格几何逐项对齐 core `colorGridCellSize`/`colorBandHeight`（含单行不限列、行最小高与
右侧留白）、互斥勾选语义、叶子按 core 矩形摆放色块并真实点击回传颜色、
无颜色斜线标记的渲染判定（grabWindow + 红像素计数）、
菜单数据对齐 widgets（标准色行 = `getStandardColorList`、深浅行 = `colorPaletteShades`、
行优先次序、自定义色记录满 10 后整体左移）、菜单叶子弹出后点选色块关闭并上报颜色、
颜色按钮几何跟随 core（色带矩形 = `calcColorUnderIconMetrics`、FillToIcon 按比例内缩、
无菜单时 Large 宽度不变而 Small 收窄、图标槽仍保留）、
色带与填充色的实际渲染色、以及点击分区（动作区触发 `colorClicked` 不弹菜单、
箭头区弹菜单、菜单内选色/选无颜色、`NoColorMenu` 与 `WithColorMenu` 来回切换）。

`tests/qml/tst_customize_qml.cpp`（12 用例）覆盖定制系统：注册表从声明式宿主树
自动收集（tag 划分、tag 名跟随改名、搜索、key↔item 双向解析、命令模板与可定制
标记）、目录模型按 tag/搜索词收窄且每行 role 满 key、记录经 core `simplify` 的
合并规则与 widgets 一致、apply 真的改动宿主树而 reverse 按 widgets
`sa_customize_datas_reverse` 的表恢复（含 Remove*/Rename* 无逆操作这一不对称性）、
`enforceCanCustomize` 闸门、XML 往返一致与**跨前端兼容**（widgets
`sa_customize_datas_to_xml` 的字节流 QML 能直接 apply，QML `appliedToXml` 的字节流
widgets 能逐字段读回）、树模型的三档显示范围与上下文页方括号标题、影子树预览
（预览期间真树一根手指都不动，`revision` 让行数不变的复位也能刷新按钮态），
以及对话框用真实鼠标点击走完"选范围 → 改名 → 加命令 → 调序 → 显隐 → 确定/取消"
全流程（选中行按地址跟随被移动的节点，而不是停在旧行号上）。
