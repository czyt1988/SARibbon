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
| `RibbonMenuItem` | 类型 | 声明式菜单项（text/iconSource/enabled/separator），挂到按钮的 `menuItems` |
| `Ribbon` | 不可实例化 | 枚举持有（`Ribbon.Large` / `Ribbon.ThreeRowMode` / `Ribbon.MenuButtonPopup` / ...） |

枚举一律通过 `Ribbon.` 前缀访问（如 `proportion: Ribbon.Large`），不散进各类型。

## 共享宿主基类

新增结构宿主时**不要**再复制叶子样板，直接继承：

- `RibbonQuickHost`（`src/qml/host/`）：持有视觉叶子生命周期（创建三部曲 + 安全拆除），
  暴露统一握手属性 `qmlLeaf`；子类只实现 `leafUrl()`。
- `RibbonLayoutItemHost`：面板子项基类 = `RibbonQuickHost` + core 布局契约
  （`SARibbonAbstractLayoutItem`）。基类已实现 isHidden/applyGeometry/debugName/
  expandingDirections 默认与大行高上下文传递；子类只补 `sizeHint()`（画廊类再加
  stretchFactor）。`RibbonPanel` 经 `qobject_cast<RibbonLayoutItemHost*>` 泛化收录子项。

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

## 视觉叶子规范

- 颜色/尺寸**一律绑定 RibbonTheme/RibbonMetrics 单例**，禁止散落颜色常量——
  叶子里出现字面量色值 = 评审打回（本项目特有增强）。
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
