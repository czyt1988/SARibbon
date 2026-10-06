# SARibbonQml 开发指引（3.0）

> 适用版本：3.0。SARibbonQml 是 3.0 新增的 QML 前端，与 widgets 前端共享同一套 core 布局引擎。

## 模块概览

- **构建开关**：`SARIBBON_BUILD_QML=ON`（默认 OFF）
- **import**：`import SARibbon 3.0`
- **依赖**：SARibbonCore + Qt（Core/Gui/Qml/Quick/**QuickControls2**，叶子弹出层用
  Controls 的 Popup/ToolTip），**禁止链接 SARibbonWidgets**（依赖矩阵红线，CI 组合矩阵
  `Widgets=OFF Qml=ON` 验证）

## 注册（命令式单轨）

应用在 `engine.load()` 前调用一次（Qt5/Qt6 同码）：

```cpp
#include <SARibbonQml/SARibbonQmlGlobal.h>

QQmlApplicationEngine engine;
saRibbonRegisterQmlTypes(&engine);   // 必须：漏调会报 "module SARibbon is not installed"
engine.load(QUrl("qrc:///main.qml"));
```

**单例注册范式（重要）**：本项目的 QML 单例一律用**回调式 `qmlRegisterSingletonType`**
并在回调内 `setObjectOwnership(CppOwnership)`。**禁止 `qmlRegisterSingletonInstance`**——
它把实例硬绑到首个引擎，多引擎场景第二个引擎取到 nullptr（Qt 5.14/6.7 源码实证）。

## P0 类型清单

| QML 类型 | 形态 | 说明 |
|---------|------|------|
| `RibbonTheme` | 单例 | core 主题数据桥（currentTheme、token 颜色查询） |
| `RibbonMetrics` | 单例 | core 度量桥（tabBarHeight 等只读属性） |
| `RibbonBar` | 类型 | bar 结构宿主（tab 行排布、高度、标题空闲区） |
| `RibbonCategory` | 类型 | category 结构宿主（panel 排布 + 引擎钳制滚动） |
| `RibbonTab` | 类型 | tab 宿主（text/current/contextColor） |
| `RibbonPanel` | 类型 | panel 结构宿主（驱动 PanelLayoutEngine，子项注册已泛化为任意 `RibbonLayoutItemHost`） |
| `RibbonToolButton` | 类型 | 按钮宿主（text/iconSource/proportion/checkable/**popupMode 三模式**/menuItems 菜单/禁用态） |
| `RibbonControlContainer` | 类型 | 控件容器宿主（`control` 属性嵌入任意 QQuickItem：ComboBox/CheckBox/SpinBox/TextField…，对标 widgets SARibbonCtrlContainer） |
| `RibbonCheckBox` | 类型（纯 QML） | 紧凑主题化复选框（14px 指示器，`indicatorSide` 可调；URL 注册，无 C++ 宿主） |
| `RibbonRadioButton` | 类型（纯 QML） | 紧凑主题化单选钮（14px 圆环；互斥用 Controls2 `ButtonGroup` 附加属性，对应 widgets QButtonGroup） |
| `RibbonComboBox` | 类型（纯 QML） | 紧凑主题化下拉框（弹出列表同走主题 token，暗色主题不穿帮；`editable`/`model`/`textRole` 等原生属性全继承） |
| `RibbonSpinBox` | 类型（纯 QML） | 紧凑主题化数字框（Windows 风格右侧上下步进钮；`from`/`to`/`stepSize`/`prefix`/`suffix` 等原生属性全继承） |
| `RibbonTextField` | 类型（纯 QML） | 紧凑主题化单行编辑框（`placeholderText`/`validator` 等原生属性全继承） |
| `RibbonMenuItem` | 类型 | 声明式菜单项（text/iconSource/enabled/separator），挂到按钮的 `menuItems` |
| `RibbonContextCategory` | 类型 | 上下文标签宿主（contextTitle/contextColor/active；激活时 bar 追加着色 tab 并发布色带，对标 widgets SARibbonContextCategory） |
| `RibbonGallery` | 类型 | 画廊宿主（Large 比例 + 水平伸展 + stretchFactor 参与 core 引擎加权分配；网格度量经 core `calcGalleryGridCellSize`） |
| `RibbonGalleryGroup` | 类型 | 画廊组（groupTitle + items，默认属性 items） |
| `RibbonGalleryItem` | 类型 | 画廊条目（text/iconSource/enabled/toolTip） |
| `RibbonSeparator` | 类型 | 面板分隔符（Large 比例独占一列的 1px 竖线，对标 widgets SARibbonSeparatorWidget） |
| `RibbonQuickAccessBar` | 类型 | 快速访问栏（标题行应用按钮后的小按钮排，宽度进 core TitleRectInput.hasQuickAccessBar，对标 widgets SARibbonQuickAccessBar） |
| `RibbonButtonGroup` | 类型 | 右侧按钮组（右对齐到 bar 右缘；紧凑样式在标题行系统按钮区前，宽松样式在 tab 行系统按钮条下方，对标 widgets SARibbonButtonGroupWidget） |
| `RibbonApplicationWindow` | 类型 | 应用窗口（File 按钮弹出的 Office 后台式覆盖层：coverageRatio 覆盖比例 + animation 进出动画 + 右上角 ✕/Esc/外点关闭，内部 close() 编程式关闭；点击优先级：窗口 > 菜单 > 仅信号，对标 widgets ApplicationWidget） |
| `Ribbon` | 不可实例化 | 枚举持有（`Ribbon.Large` / `Ribbon.ThreeRowMode` / `Ribbon.MenuButtonPopup` / `Ribbon.RibbonStyleCompactTwoRow` / ...） |

枚举一律通过 `Ribbon.` 前缀访问（如 `proportion: Ribbon.Large`），不散进各类型。

## 共享宿主基类

新增结构宿主时**不要**再复制叶子样板，直接继承：

- `RibbonQuickHost`（`src/qml/SARibbonQmlQuickHost.h`）：持有视觉叶子生命周期（创建三部曲 + 安全拆除），
  暴露统一握手属性 `qmlLeaf`；子类只实现 `leafUrl()`。
- `RibbonLayoutItemHost`：面板子项基类 = `RibbonQuickHost` + core 布局契约
  （`SARibbonAbstractLayoutItem`）。基类已实现 isHidden/applyGeometry/debugName/
  expandingDirections 默认与大行高上下文传递；子类只补 `sizeHint()`（画廊类再加
  stretchFactor）。`RibbonPanel` 经 `qobject_cast<RibbonLayoutItemHost*>` 泛化收录子项。
- **面板子项的登记顺序 ≠ 声明顺序**：Repeater 代理在其 Repeater 的
  componentComplete（**反向**兄弟序）才到达，且 `ItemChildAddedChange` 触发时代理
  还挂在 childItems **末尾**（QQuickRepeater 在通知之后才 restack 到最终位置）——
  注册时无从锚定。面板把新登记项挂入 pending 列表，在**下一次布局**时按已稳定的
  视觉顺序落位（`settlePendingOrder`）；`attachChildItem`/`moveChildItem` 的显式
  顺序优先于延迟锚定。混排静态声明与 Repeater 生成子项因此安全。

## 基础输入控件（URL 注册的纯 QML 类型）

`RibbonCheckBox` / `RibbonRadioButton` / `RibbonComboBox` / `RibbonSpinBox` /
`RibbonTextField` 是**纯 QML 文档**，经 `qmlRegisterType(QUrl("qrc:/SARibbon/RibbonXxx.qml"))`
注册进模块（命令式单轨的自然延伸，仍无 qmldir/qmltypes 安装物）。它们**没有 C++ 宿主**：
嵌入面板时几何权威属于 `RibbonControlContainer`（拉伸到行高、压缩 padding），
自身只负责渲染。视觉对齐 widgets QSS 对 `SARibbonPanel > Q{CheckBox,RadioButton,ComboBox,LineEdit}`
的特化（1px inputBorder 边框、hover 转 inputFocus、selectionBg 选区），
弹出列表同样走主题 token。颜色全部绑定 `RibbonTheme`，字号绑定
`RibbonMetrics.fontPointSize`（面板行高随 ribbon 字体变，文字同步变）。
根类型是 `QtQuick.Templates` 的模板类（非样式化控件），外观不随应用的
`QQuickStyle` 变化——这五个文件本身就是"样式实现"。

```qml
RibbonPanel {
    panelTitle: "widget test"
    RibbonControlContainer { text: "Check:"; control: RibbonCheckBox { } }
    RibbonControlContainer { text: "Spin:"; suffixText: "px";
        control: RibbonSpinBox { from: 0; to: 100; value: 12 } }
    RibbonControlContainer { text: "Font:";
        control: RibbonComboBox { model: [ "a", "b" ] } }
    RibbonControlContainer { control: RibbonRadioButton { text: "pick me" } }
    RibbonControlContainer { control: RibbonTextField { placeholderText: "type" } }
}
```

- 额外属性只有 `RibbonCheckBox/RadioButton` 的 `indicatorSide`（默认 14，指示器边长）；
  其余能力全部继承原生 Controls 属性（`editable`/`model`/`currentIndex`/`from`/`to`/
  `stepSize`/`placeholderText`/`validator`……）。
- 也可脱离 ribbon 面板独立使用（就是普通 Controls 控件）。
- 单选互斥：`ButtonGroup { id: g }` + `RibbonRadioButton { ButtonGroup.group: g }`。
- Canvas 型指示器（箭头）颜色依赖主题 token，必须遵守"颜色变化触发 `requestPaint`"
  的叶子重绘规则，否则换主题后箭头颜色滞留。

## 按钮弹出模式

`popupMode` 遵循 `QToolButton::ToolButtonPopupMode` 语义（枚举值一致）：

- `MenuButtonPopup`：按钮分为动作区与菜单区，命中分区由宿主发布为
  `actionRect`/`menuRect`（几何权威在 C++，叶子只渲染/绑定 MouseArea）；
- `InstantPopup`：整个按钮即菜单区（无动作区）；
- `DelayedPopup`：整个按钮保持动作区，长按弹菜单（叶子侧计时）。

菜单项为 `RibbonMenuItem` 列表（`QQmlListProperty`），激活一律经宿主的
`menuTriggered` 信号中转（叶子行点击 → `activateMenuItem(index)`），测试无需弹窗即可
驱动；禁用项与分隔项被忽略。禁用态（`enabled: false`）由宿主 `click()` 吞掉点击，
叶子以透明度呈现灰态。

## 上下文标签页

```qml
RibbonContextCategory {
    contextTitle: "context"; contextColor: "#2d7d9a"; active: false
    RibbonCategory { title: "Page1" ... }   // 页面声明为子项，自动登记
}
```

- 激活由 **`active` 属性**驱动（不是 item 的 visible——本项是透明结构容器，页面显隐
  由 bar 布局控制）；激活时 bar 为每页追加一个着色 tab（`RibbonTab.contextColor`），
  关闭时移除并钳制 currentIndex。
- 色带经 bar 的 `contextBands` 属性发布（x/width/title/color/highlight/textColor 的
  map 列表），由 bar 叶子渲染（z=-1 层，位于 tab 之下）；高亮色取 core
  `SARibbonThemeData::themeContextHighlight`（与 widgets ThemeManager 安装的同一 fp）。
- **已知陷阱**：C++ 创建的上下文 tab 挂到 bar 时会触发 bar 的 `itemChange`，必须经
  `isOwnedContextTab` 排除，否则会被误注册为普通 tab（挤占自动 tab 名额）。

## 画廊

```qml
RibbonGallery {
    stretchFactor: 1
    RibbonGalleryGroup { groupTitle: "Files"; RibbonGalleryItem { ... } ... }
}
```

- 契约面对齐 widgets：Large 比例 + `expandingDirections()==Qt::Horizontal` +
  `stretchFactor()` 参与 core 面板引擎的加权额外宽度分配（`recalcExpandGeomArray`）。
- 网格单元尺寸 = core **`SA::calcGalleryGridCellSize`**（自 widgets
  SARibbonGalleryGroup::recalcGridSize 下沉的双前端共性函数）；宿主发布
  `gridSize/gridColumns/totalRows/scrollRow`，叶子只按度量摆格子。
- 模型类（GalleryGroup/GalleryItem）用 `Q_CLASSINFO("DefaultProperty", ...)`
  声明默认属性，声明式子项直接进列表；Repeater 代理无 QObject 父级（见叶子规范）。

## 六种 Ribbon 样式

`RibbonBar.ribbonStyle`（枚举值对齐 widgets `SARibbonBar::RibbonStyleFlag` 的位标志）
遵循 widgets `setRibbonStyle` 的传播语义：

- **Loose/Compact**：Compact（tabOnTitle）把 tab 行叠进标题行（`categoryRowY`
  收缩为标题高，bar 总高少一个 tabH）；
- **ThreeRow/TwoRow/SingleRow**：行数传播到所有面板（含上下文标签页的面板）的
  layoutMode；三行开 wordWrap，单行隐藏面板标题并启用 iconRightText（按钮一律
  图标左、文字右渲染，`effectiveButtonType` 对等）；
- 传播链：bar → category → panel（行数 + 标题开关）→ 按钮（wordWrap/iconRightText），
  迟注册的面板/按钮经各层存储的样式字段继承；
- 类目高度经 core `SARibbonMetrics::calcCategoryHeight(three, single)`（单行不含
  面板标题条）；主栏总高 = `calcMainBarHeight(tabOnTitle)`。

```qml
RibbonBar { ribbonStyle: Ribbon.RibbonStyleCompactTwoRow }
```

## 标题行容器与应用按钮

- **RibbonQuickAccessBar / RibbonButtonGroup**：共享基类 `RibbonButtonRowHost`
  （itemChange 登记按钮、按 sizeHint 排行、发布 rowWidth）；按钮无面板引擎——
  行宿主即子项布局权威。bar 把前者摆在应用按钮之后、后者右对齐到 bar 右缘
  8px 边距处；无边框宿主经 `RibbonBar.systemButtonStripWidth`（默认 0，原生
  边框不预留）声明系统按钮区后，**只有紧凑样式**（tab 行骑在标题行上）的
  右侧组与 tab 行右界左移让位——宽松样式的右侧组在 tab 行上、位于只覆盖
  标题行的系统按钮条**下方**，必须贴到 bar 右缘（widgets resizeInLooseStyle
  对等：strip 只进标题自由区计算），宽度进入 `TitleRectInput.hasQuickAccessBar`。
- **应用按钮三种模式**（点击优先级：应用窗口 > 菜单 > 仅信号）：
  - `RibbonApplicationWindow`（widgets ApplicationWidget 对等）：声明为 bar
    子项的自定义内容项，叶子惰性 Popup 以 **Office 后台式覆盖层**承载——
    挂在窗口 `Overlay.overlay` 上按 `coverageRatio`（0.05..1.0，1.0 全屏、
    2/3 或自定义）覆盖窗口、全高、可选滑出/淡入进出动画（`animation` +
    `animationDuration`）。内容在 `aboutToShow` **显式 reparent 进弹层
    contentItem**——直接赋值 `Popup.contentItem` 不会收养已有视觉父项的
    item（QQuickControl 只收养无父项者，内容会留在 bar 里裸渲染，即"无背景
    融合"bug）。Esc/外点/右上角 ✕（`showCloseButton`，全屏覆盖必备）关闭；
    内部 `close()` 经 closeRequested → bar → requestApplicationWindowClose →
    叶子弹层链路。视觉 reparent 只动 parentItem，bar 的登记按 QObject 父项
    判定存活。
  - `applicationMenuItems` + `applicationMenuTriggered`（widgets 菜单模式）：
    弹出层**必须惰性创建**——bar 叶子在 bar 的 componentComplete 期间创建
    （场景窗口尚未就绪），此时实例化 Popup 会得到一个游离的原生窗口并使
    主窗口场景空白（第 4 轮实测）；用 `Loader { active: false }` 在首次打开时创建。

## 视觉叶子规范

- 颜色/尺寸**一律绑定 RibbonTheme/RibbonMetrics 单例**，禁止散落颜色常量——
  叶子里出现字面量色值 = 评审打回（本项目特有增强）。
- **绑定形状铁律（第 8 轮实测）**：宿主属性绑定一律用单依赖形状
  `cppHost ? cppHost.yyy : fallback`；**禁止** `cppHost && cppHost.xxx ? cppHost.yyy : fallback`
  的双依赖短路形状——该形状在 Qt 6.7.3 Debug 构建的 QQmlData teardown 中
  触发 V4 堆踩坏（NOTES B48，optionAction 崩溃链根因）。功能等价：禁用路径
  由宿主发布空值承载，不由绑定条件承载。
- 宿主↔叶子配对（统一握手契约）：叶子根声明 `property QtObject cppHost`
  （C++ `setProperty("cppHost")` 注入），`onCppHostChanged` 把自己赋回宿主继承自
  `RibbonQuickHost` 的 `qmlLeaf` 属性（兼作测试可达性入口）。
- 叶子创建三部曲（基类 `ensureQmlLeaf()`）：`QQmlComponent` → `create()` →
  `setProperty("cppHost")` → 双 setParent；失败检查 `errorString()` + `QFile::exists(qrc路径)`。
- 析构：基类统一 `setParent(nullptr)` + `deleteLater()`，**禁止直接 delete**。
- 弹出层（菜单等）用 Controls 的 `Popup` + 自绘行（Repeater 代理），颜色走
  RibbonTheme token；**注意 Repeater 代理没有 QObject 父级**，C++ 测试须经
  `childItems()` item 树查找，`findChild` 找不到。

## 铁律

QML 侧发现需要"复制"widgets 的一段算法 = core 缺口 = 停下来补 core，
**禁止在 QML 里重写布局计算**。渲染允许双实现。

- sizeHint 传导链（S5）：core metrics → 宿主 C++ `sizeHint()` → 引擎装箱 →
  `resultGeometry` → `applyGeometry`。**不得取 QML 叶子的 implicitWidth/Height 反推**。

## 滚动动画

Category 滚动的动画用 QML `Behavior on x`（前端动画），目标值经引擎
`clampScrollOffset` 钳制——动画属表现层，钳制属算法层。
