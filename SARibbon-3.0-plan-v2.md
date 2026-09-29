# SARibbon 3.0 架构规划 v2：以「算法与契约下沉」为核心的 Core 设计

> 状态：草案 v2（基于 v1 审查修订）
> **存留说明（2026-09-29 注）**：v1 计划文档（`SARibbon-3.0-plan.md`）未随仓库归档，也从未进入
> git 历史；本文 §0/§1 的 v1 审查结论是关于 v1 内容的**唯一存留记录**。本文与 `plans/3.0/`
> 各执行计划中残留的 "v1 §x" 章节引用均为悬空引用，一律以本文与执行计划正文（内联展开的
> 设计）为准，处理约定详见 `plans/3.0/README.md` 的"计划文档自包含性说明"（NOTES.md B6）。
> 参考架构：
> - **QWindowKit**（`src/SARibbonBar/3rdparty/qwindowkit`，submodule 未初始化，本机参考副本
>   `F:\src\3rdparty\qwindowkit`）→ 借鉴其**构建体系**（多库拆分、导出宏、组件化 package；
>   其建库宏实名 `qwk_add_library`，见副本 `src/CMakeLists.txt:38`）
> - **KDDockWidgets**（`F:\src\3rdparty\KDDockWidgets`）→ 借鉴其**分层模式**（core 后端 + 多前端、ViewInterface 契约、纯几何 layouting 引擎、跨前端测试）
> 基线版本：SARibbon 2.9.5（单 target `SARibbonBar`；dev/v3/origin/dev 同指 `7a617fc`）

---

## 0. v1 → v2 修订摘要

| # | v1 的问题 | v2 的修订 |
|---|----------|----------|
| P1 | **贫血 core**：core 只装主题/颜色/工具数据类（约 8 个文件），全部交互逻辑、结构管理、布局留在 widgets，QML 模块被迫平行重实现（v1 §6.4 甚至明文允许"复制/改写"） | core 重新定义为 **Theme + Metrics + Layout Engines + Contracts/Data** 四子系统；"行为/算法禁止双实现，渲染允许双实现"写入铁律 |
| P2 | **布局策略全部滞留 widgets**：三个布局类约 5300 行中，核心几何算法（large/medium/small 装箱、panel 宽度分配、高度度量）本质是纯计算 | 新增 **layout 引擎子系统**：`PanelLayoutEngine` / `CategoryLayoutEngine` / `BarGeometryEngine` 下沉 core，widgets 侧 QLayout 退化为适配器 |
| P3 | **缺契约层**：没有任何接口抽象，widgets 与 QML 无法对接同一套逻辑 | 引入 KDDock 式 **ViewInterface 契约**：`SARibbonAbstractLayoutItem` / `SARibbonAbstractLayoutHost` 等窄接口 |
| P4 | **度量体系无归属**：tabBarHeight/categoryHeight/panelTitleHeight 等视觉一致性关键数值散落在 BarLayout 私有函数里 | 新增 **`SARibbonMetrics`**：全部尺寸常量与派生计算的唯一权威来源，双前端共用 |
| P5 | **无跨前端一致性测试策略**：单测仅"按模块拆分" | 引入 KDDW 的**跨前端测试**实践：黄金几何测试 + 前端一致性套件 |
| 保留 | v1 的构建体系（QWK 式三库 + `sa_add_library`）、ThemeManager 拆分方向、QML 双轨注册、兼容层策略 | 全部保留，仅增强 core 纯净性门禁 |

---

## 1. 对 v1 计划的审查结论

### 1.1 v1 做对了的（保留不动）

1. **三库拆分 + 严格单向依赖**（Core ← Widgets / Qml）。方向正确。
2. **构建体系采用 QWindowKit 模式**（`sa_add_library`——SARibbon 侧新函数名，QWK 原型为 `qwk_add_library`、统一导出宏、组件化 `find_package`、`SARibbon::Core` 命名空间 target）。KDDockWidgets 用的是"单库 + configure 时选前端源码拼合"（其 `src/CMakeLists.txt:304` 把 `KDDW_FRONTEND_QTWIDGETS_SRCS` 并入 `DOCKSLIBS_SRCS`，`:327` `add_library(kddockwidgets ...)` 单一 target），这由它的双许可与私有符号共享需求决定；对 SARibbon 而言**三库更优**——amalgamate 单文件发行和 Python 绑定都依赖清晰的库边界。维持 v1。
3. **ThemeManager 拆分**为 core 数据层 + widgets QSS 应用层。方向正确（细化见 §3.2）。
4. **QML 注册双轨**（Qt5 命令式 / Qt6 `qt_add_qml_module` 声明式）。
5. **兼容层**：转发头 + 兼容宏 + 旧 target 名过渡一个版本周期。
6. **CI 组合构建门禁**（`SARIBBON_BUILD_WIDGETS=OFF` 强制 core 独立编译）。

### 1.2 五个核心问题（v2 逐一修复）

#### P1：贫血 core —— 最严重的结构缺陷

v1 §3/§4.1 中 core 的全部内容：Global 头、Util、ThemePalette、ThemeData、ColorTool、Config、Qt5Compat、（待定的 GalleryItem）。这是**"数据仓库式 core"**——只下沉了没有行为的数据类，因为"有行为的类都碰了 QWidget"。

对照 KDDockWidgets 的 core（`src/core/`，114 个文件：69 个 .h + 45 个 .cpp，2026-09 实测）：它装了**全部业务逻辑**——DockRegistry、DragController、DockWidget/Group/TabBar/TitleBar/MainWindow 等全部 Controller（QObject，无 GUI 依赖）、ViewFactory 抽象工厂、Platform 平台单例、以及完全独立的 layouting 引擎。前端的 `qtwidgets/views/`、`qtquick/views/` 只做两件事：**渲染 + 把前端事件翻译回 controller**。

后果对比：

- KDDW：同一个 `Core::TabBar` 控制器，widgets 前端用 `QTabBar` 实现视图，quick 前端用 `QQuickItem` + `DockWidgetModel(QAbstractListModel)` 实现视图——**行为零漂移**，bug 修一次两端生效。
- v1 计划：QML 模块只拿主题和颜色，`RibbonBar/Category/Panel/ToolButton` 全部从零重写，§6.4 写着"允许 qml 模块内部复制/改写实现"。两个版本的行为一致性没有任何机制保障——**这不是一个共享库，是两个同名产品**。

v1 §6.4 的理由是"避免过早抽象把 widgets 拖入 Quick 依赖"。这个担忧本身合理，但结论下反了：KDDW 证明的正确解法是**把共享的东西放进 core，而不是允许复制**。"复制实现"应当只对**渲染**合法（QPainter 绘制 vs QML 场景图本来就是两套），对**行为和算法**必须非法。

#### P2：布局策略滞留 widgets —— 你的直觉是对的

对 2.x 三个布局类逐行分析（这是 v2 修订的事实依据）：

| 类 | 行数 | 内容拆解 | 结论 |
|----|------|---------|------|
| `SARibbonPanelLayout` | 1863 | `updateGeomArray()`（:795-1203，约 409 行）+ `recalcExpandGeomArray()`（:1204-1387，约 184 行）是 **large/medium/small 行列装箱纯算法**：输入 items 的 (sizeHint, rowProportion, expanding, isEmpty) 与可用矩形/度量，输出每项的 (rowIndex, columnIndex, geometry)。widget 耦合仅两处：`item->widget()` 取 sizeHint（做缓存 key）、`widget()->setGeometry()` 应用结果 | **算法 100% 可下沉**，耦合点恰好是 P3 契约接口的位置 |
| `SARibbonCategoryLayout` | 1425 | `updateGeometryArr()`（:431）为 panel 分配 x/宽度、`categoryContentSize()`（:408）、scroll 偏移计算是纯几何；`scrollByAnimate()`（:971）依赖 QPropertyAnimation | 几何部分下沉；动画属前端 |
| `SARibbonBarLayout` | 1993 | `tabBarHeight()/categoryHeight()/panelTitleHeight()/normalModeMainBarHeight()/minimumModeMainBarHeight()/calcMinTabBarWidth()`（.h :66/:89/:92/:96/:108/:114）是**纯度量计算**（QML 必须拿到同样数值否则双版本高矮不一）；`layoutTitleRect()`（.cpp :1242）是标题区几何；`doLayout()` 中 windowButtonBar/frameless 部分是 widget 专属 | 度量与标题区几何可下沉；系统按钮栏、无边框逻辑留守 |

KDDW 的对应物是 `src/core/layouting/`（Item、SizingInfo、LayoutingHost、LayoutingGuest）——一个**零 GUI 依赖**、注释里明说"可被非 KDDW 项目复用"（`LayoutingGuest_p.h:29` 原注 "reused by non-KDDW projects"）的独立布局引擎。宿主与成员各自只需实现极小的纯虚接口（`LayoutingGuest`：`minSize/maxSizeHint/setGeometry/setVisible/geometry/setHost/host/id` 8 个纯虚，见 `LayoutingGuest_p.h:31-43`）。SARibbon 的布局远比停靠布局简单，没有理由做不到同样的分离。

#### P3：缺契约层

v1 没有任何接口抽象。而 KDDW 在 core 与前端之间有**三层契约**：

1. `Core::View`（通用视图接口，约 60 个纯虚：`src/core/View.h` 275 行、60 处 `= 0`，2026-09 实测）——SARibbon **不需要照抄**（KDDW 需要跨窗口拖放、平台窗口操作，SARibbon 没有）；
2. **`core/views/*ViewInterface.h`**（按视图类型的小接口族，如 `TabBarViewInterface.h`：`text()` :42、`tabAt()` :44、`moveTabTo()` :45、`rectForTab()` :46、`insertDockWidget()` :53）——前端视图多重继承实现，controller 通过它回调前端能力。**这正是 SARibbon 布局引擎需要的模式**；
3. `ViewFactory` 抽象工厂在 core 定义、前端子类化——对应 SARibbon 现成的 `SARibbonElementFactory` 定制点（v1 把它整体留在 widgets 且"首版不做"抽象，见 P6/D7）。

v2 引入第 2 类窄接口（`SARibbonAbstractLayoutItem` 等，§3.4.1），不引入第 1 类巨型接口。

#### P4：度量体系无归属

Ribbon 视觉一致性的根基是一堆尺寸数值：三行模式大按钮高度、panel 标题高、tab 行高、category 高、最小模式条高、间距常量……目前散落在 BarLayout 私有函数和各控件 `sizeHint()` 的硬编码里。QML 版本若各自硬编码，与 widget 版**必然**高矮不一、间距不一。v1 完全没有这部分设计。v2 新增 `SARibbonMetrics`（§3.3）。

#### P5：无跨前端一致性测试

KDDW 的杀手级实践：`Platform::tests_*` 抽象（`tests_pressOn/tests_waitForResize/tests_createView...`，`src/core/Platform.h:207-263`）让**同一套测试代码跑两个前端**（`src/qtwidgets/TestHelpers.cpp` 与 `src/qtquick/TestHelpers.cpp` 各自实现）。v1 只有"25 个单测按模块拆分"（现仓库实际：ctest 注册 26 项 = `tests/` 顶层 25 个 .cpp + `tests/auto/SARibbonThemePalette/tst_themepalette.cpp`，见 `plans/3.0/NOTES.md` B8）。v2 补充：黄金几何测试 + 跨前端一致性套件（§7）。

### 1.3 v1 次要问题（顺带修订）

| # | 问题 | 修订 |
|---|------|------|
| P6 | `SARibbonElementFactory` 整体留 widgets、"core 层工厂接口首版不做"——但 ElementFactory 是 SARibbon 现有的用户定制点，不做则 QML 版无对等定制能力 | 3.0 在 core 定义 `SARibbonElementFactory` 的抽象接口（创建签名以 core 类型表达），widgets 实现类保持 2.x 名字与 API；QML 后续可子类化。与 KDDW `Core::ViewFactory` → `QtWidgets::ViewFactory` 同构 |
| P7 | 命名拼写混乱：2.x 用 `SARibbonPannel`（双 n），v1 文档混用 Panel/Pannel | 3.0 统一修正为 `Panel`，列入迁移指南（转发头 + `SARibbonPannel = SARibbonPanel` 类型别名过渡）。**【已完成于 2.9.x，f553de7"把Pannel的命名错误调整为Panel"：现仓库类名/文件名均为单 n，源码无 `SARibbonPannel` 残留，别名过渡不再需要，迁移指南只作结论性说明，见 plans/3.0/NOTES.md B1】** |
| P8 | v1 §4.2 对 `SARibbonActionsManager`（792 行，QAction 注册/检索/tag 过滤/搜索）只字未提归属 | 明确归 widgets（Qt5 的 QAction 在 QtWidgets）；core 的 actions 抽象列为 D8 决策（3.1+） |
| P9 | `SARibbonCustomizeData`（自定义操作记录）纯数据部分 v1 说"如 QML 需要再下沉" | CustomizeData 的记录结构（增/删/改/换位 + 目标描述）是双前端定制功能的共享基础，**3.0 直接下沉**纯数据部分，apply 逻辑留 widgets |

---

## 2. 修订后的架构总览

### 2.1 分层图

```
                    ┌──────────────────────────────────────────────┐
                    │  SARibbonCore  (Qt Core + Qt Gui，无 Widgets/Quick) │
                    │                                              │
                    │  theme/      主题枚举、Palette、明暗分类、变更通知   │
                    │  metrics/    SARibbonMetrics 尺寸度量体系        │
                    │  layout/     PanelLayoutEngine                 │
                    │              CategoryLayoutEngine              │
                    │              BarGeometryEngine                 │
                    │  contract/   AbstractLayoutItem / Host 等窄接口  │
                    │  data/       RowProportion 等枚举、CustomizeData │
                    │  factory/    SARibbonElementFactory 抽象接口      │
                    │  global/     导出宏、PIMPL、Qt5Compat、Config    │
                    └───────────┬───────────────────┬──────────────┘
                                │                   │
              ┌─────────────────┴─────┐   ┌─────────┴──────────────┐
              │ SARibbonWidgets       │   │ SARibbonQml            │
              │ (Qt Widgets)          │   │ (Qt Quick/Qml)         │
              │                       │   │                        │
              │ · QWidget 视图        │   │ · C++ QQuickItem 结构宿主│
              │ · QLayout 适配器      │   │   （接收引擎几何）        │
              │   （驱动 core 引擎）   │   │ · QML 视觉叶子           │
              │ · QSS 主题应用        │   │ · Instantiator 声明式包装 │
              │ · MainWindow/无边框    │   │ · 主题单例桥             │
              │ · ActionsManager      │   │ · Gallery/颜色控件(后期)  │
              │ · Gallery/colorWidgets│   │                        │
              └───────────────────────┘   └────────────────────────┘
```

### 2.2 归属判断准则（开发时的 checklist）

挪动/新增任何一段代码前回答：

1. **是纯计算吗？**（输入数据 → 输出几何/尺寸/颜色，无 IO 无控件）→ core（layout/、metrics/）
2. **是双端都要遵循的行为语义吗？**（"Medium 在 3 行模式两列排布"、"上下文标签页改变 tab 颜色"）→ core
3. **是渲染吗？**（QPainter/QSS vs QML 场景图）→ 前端，允许双实现
4. **是前端原生交互吗？**（QPropertyAnimation、QSS、QQuickItem 动画、无边框窗口）→ 前端
5. **只被一个前端用吗？** → 该前端，**禁止**"以防万一"塞进 core
6. **core 禁区**：`QWidget`/`QLayout`/`QSS`/`QQuickItem`/`QQmlEngine`/平台窗口 API。core 里 `#include <QWidget>` = 构建失败（CI 门禁，§6.2）

**铁律：行为与算法禁止双实现；渲染允许且只允许渲染双实现。** 若 QML 发现需要"复制"一段 widgets 的算法，那是 core 的设计缺陷，修 core，而不是复制。

### 2.3 KDDockWidgets 模式取舍表（哪些抄、哪些不抄）

| KDDW 模式 | SARibbon 3.0 取舍 | 理由 |
|-----------|-------------------|------|
| Controller/View 全面分离（每个交互类一个 controller） | **部分采用**：3.0 只为布局引擎引入契约接口；结构控制器（RibbonBar/Category/Panel 的 model）列为 Tier 2，按 QML 实际需求触发（D7） | SARibbon 复杂度集中在几何与渲染，不在交互状态机；全面拆分动 40 个类的内部结构，与 2.x API 兼容目标冲突，收益/成本比不成立 |
| `core/views/*ViewInterface` 窄接口族 | **采用**（`contract/`） | 布局引擎回调用，规模小、无侵入 |
| 纯几何 layouting 引擎独立于 GUI | **采用**（`layout/`） | 本方案核心 |
| 通用 `Core::View` 巨型接口（约 60 纯虚，`src/core/View.h`） | **不采用** | SARibbon 无跨窗口拖放/平台窗口操作需求 |
| `Platform` 单例抽象 | **不采用**（3.0） | 无跨前端平台操作需求；若 Tier 2 引入结构控制器时需要 tests 抽象，再引入最小版 |
| `qtcommon` 中间层（双 Qt 前端共享胶水） | **不采用**（3.0） | SARibbon 的 Qt 胶水量远小于 KDDW（无 Wayland 拖放等）；Qt5Compat 留 core。若日后膨胀可增设 |
| 跨前端测试套件 | **采用**（§7） | 一致性的唯一可执行保障 |
| 单库拼合式构建 | **不采用**（保留 QWK 三库） | amalgamate / Python 绑定依赖库边界 |
| 自研 `Core::Action` 包装 QAction | **观察**（D8，3.1+） | 仅当 Gallery/ActionsManager 需要进 core 时才值得付这个成本 |

---

## 3. SARibbonCore 模块详细设计

### 3.1 目录结构

```
src/core/
├── CMakeLists.txt
├── SARibbonCoreGlobal.h           # SA_RIBBON_CORE_EXPORT + PIMPL 宏 + 全模块共用宏
├── SARibbonCoreConfig.h(.in)      # 版本 + feature 开关
├── SARibbonQt5Compat.hpp
├── global/
│   ├── SARibbonEnums.h            # SARibbonTheme、RowProportion、PanelLayoutMode、
│   │                              # RibbonButtonStyle、ToolButtonPopupMode、
│   │                              # BarMode(3行/2行/最小) 等全部公共枚举
│   └── SARibbonUtil.h/.cpp        # 无 widget 的工具函数（SARibbon::Core 命名空间）
├── theme/
│   ├── SARibbonThemePalette.h/.cpp
│   ├── SARibbonThemeData.h/.cpp   # 主题数据 + 变更通知（QObject + themeChanged 信号）
│   └── SARibbonColorTool.h/.cpp
├── metrics/
│   └── SARibbonMetrics.h/.cpp     # §3.3，全部尺寸常量与派生计算的唯一权威
├── contract/
│   ├── SARibbonAbstractLayoutItem.h
│   ├── SARibbonAbstractLayoutHost.h
│   └── SARibbonAbstractMetricsProvider.h   # 可选：按窗口/屏幕提供 metrics
├── layout/
│   ├── SARibbonPanelLayoutEngine.h/.cpp
│   ├── SARibbonCategoryLayoutEngine.h/.cpp
│   └── SARibbonBarGeometryEngine.h/.cpp
├── data/
│   ├── SARibbonCustomizeData.h/.cpp  # 纯数据记录（无 apply 逻辑）
│   └── SARibbonQuickAccessItemSpec.h # 后续按需
└── factory/
    └── SARibbonElementFactoryInterface.h  # §3.6
```

依赖：`Qt::Core`、`Qt::Gui`（QFontMetrics、QIcon、QColor 等；**不链** Widgets/Quick）。

### 3.2 theme/ 子系统（细化 v1 的拆分）

v1 方向对，v2 细化三点：

1. **变更通知机制归 core**：`SARibbonThemeData`（QObject）持有当前主题 + palette，发 `themeChanged(SARibbonTheme)` / `paletteChanged()` 信号。widgets 侧 `applyRibbonTheme()` 监听它刷 QSS；QML 侧主题单例（§5.4）监听它刷属性。**双端切换主题的行为由同一个信号源驱动**，杜绝"widget 版切了 QML 版没切"。
2. **明暗分类、色板派生规则（makeColorVibrant 等）入 core**；QSS 模板渲染留 widgets（QSS 是 widget 专属产物）。
3. 主题枚举、ThemeManager 的 `isDarkTheme()` 类判断函数入 core。

### 3.3 metrics/ 子系统（新增，v1 缺失）

```cpp
// core/metrics/SARibbonMetrics.h
namespace SARibbon::Core {

class SARIBBON_CORE_EXPORT SARibbonMetrics
{
public:
    // —— 输入 ——
    QFontMetrics fontMetrics;      // 按当前应用字体计算（构造时传入，不查 widget）
    int devicePixelRatio = 1;

    // —— 基础常量（原散落于各控件的硬编码，统一收口）——
    int spacing = 1;               // PanelLayout 项间距
    int rowSpacing = 1;            // 行间距
    int titleSpace = 2;
    int iconSizeLarge() const;     // 由字体高度 + 模式推导
    int iconSizeSmall() const;

    // —— 行高推导（对应 v1 计划滞留 widgets 的 BarLayout 私有函数）——
    int tabBarHeight() const;              // 原 SARibbonBarLayout::tabBarHeight
    int categoryHeight() const;            // 原 SARibbonBarLayout::categoryHeight
    int panelTitleHeight() const;          // 原 SARibbonBarLayout::panelTitleHeight
    int titleBarHeight() const;
    int normalModeMainBarHeight() const;   // 原 normalModeMainBarHeight
    int minimumModeMainBarHeight() const;  // 原 minimumModeMainBarHeight
    int windowButtonSize() const;

    // —— 度量一致性锚点 ——
    // 三行模式给定 category 高度下：大/中/小按钮行高（原 updateGeomArray 内联计算）
    int largeHeight(int categoryHeight) const;
    int smallHeight(int categoryHeight, BarMode) const;
};
}
```

要点：

- **只依赖 QFontMetrics / 数值**，构造参数全显式传入，core 内不查询任何控件 → 可单测、可被 QML 用同样字体构造出同样数值。
- 现有 BarLayout 里的 setter（`setTabBarHeight` 等）保留在 widgets 侧作为用户定制 API，写入 `SARibbonMetrics` 实例；引擎全部从 metrics 取值。
- 迁移期允许默认值与 2.x 现值不同，但**必须**出一份"2.9.5 vs 3.0 默认度量对照表"并截图对照（风险表 R3）。

### 3.4 layout/ 子系统（本方案核心）

#### 3.4.1 契约接口（contract/，KDDW `LayoutingGuest` 模式的窄化）

```cpp
// core/contract/SARibbonAbstractLayoutItem.h
namespace SARibbon::Core {

class SARIBBON_CORE_EXPORT SARibbonAbstractLayoutItem
{
public:
    virtual ~SARibbonAbstractLayoutItem();

    // —— 前端提供（引擎的输入）——
    virtual QSize sizeHint() const = 0;
    virtual QSize minimumSizeHint() const = 0;
    virtual bool isHidden() const = 0;                  // 引擎跳过隐藏项
    virtual Qt::Orientations expandingDirections() const = 0;

    // —— 前端实现（引擎的输出回写）——
    virtual void applyGeometry(const QRect& rect) = 0;  // widgets: widget->setGeometry;
                                                        // qml: QQuickItem::setGeometry

    // —— 引擎回写（引擎填，双端读取）——
    int rowIndex = -1;
    int columnIndex = -1;
    QRect geometry;
    bool isExpandItem = false;

    // —— 共享数据 ——
    RowProportion rowProportion = RowProportion::None;  // 枚举移入 core/global
};
}
```

```cpp
// core/contract/SARibbonAbstractLayoutHost.h
namespace SARibbon::Core {
class SARIBBON_CORE_EXPORT SARibbonAbstractLayoutHost
{
public:
    virtual ~SARibbonAbstractLayoutHost();
    virtual const SARibbonMetrics& metrics() const = 0; // 度量来源
};
}
```

设计要点：

- 接口保持**最小**。sizeHint 缓存策略（2.x 的 `mButtonSizeHintCache`）在引擎内部以 item 指针为 key 实现，`largeHeight` 变化时统一失效——缓存逻辑也随算法进 core，QML 免费获得同等性能优化。
- **不用** `std::shared_ptr`/句柄层（KDDW 因 Flutter 前端生命周期不同才需要），Qt 前端父子所有权模型足够。

#### 3.4.2 PanelLayoutEngine（从 `SARibbonPanelLayout::updateGeomArray` + `recalcExpandGeomArray` 提取）

```cpp
// core/layout/SARibbonPanelLayoutEngine.h
namespace SARibbon::Core {

class SARIBBON_CORE_EXPORT SARibbonPanelLayoutEngine
{
public:
    struct Input {
        PanelLayoutMode mode = PanelLayoutMode::ThreeRowMode;  // 2.x: ThreeRowMode/TwoRowMode/SingleRow
        bool showPanelTitle = true;
        bool enableExpanding = true;   // recalcExpandGeomArray 对应开关
        // 标题/选项按钮作为伪项由调用方包装成 AbstractLayoutItem 一并传入，
        // 引擎不感知"这是 label 还是按钮"
    };
    struct Result {
        QSize sizeHint;         // 原 SARibbonPanelLayout::sizeHint
        QSize minimumSize;
        int columnCount = 0;
        int largeHeight = 0;    // 供缓存失效判断
    };

    // 纯计算：不触碰任何控件。可对同一 items 反复调用（resize 路径）。
    Result layout(QVector<SARibbonAbstractLayoutItem*>& items,
                  const QRect& availableRect,
                  const SARibbonMetrics& metrics,
                  const Input& in);
};
}
```

提取方法学（两步走，M1 执行，**先接口化、后搬移**）：

1. **Step A（在 2.x 原地）**：把 `updateGeomArray` 改为只通过上述接口读写（`item->widget()` 取 sizeHint → `item->sizeHint()`；`widget()->setGeometry()` → 记录到 `itemWillSetGeometry` 后统一 `applyGeometry`），`SARibbonPanelItem` 同时继承 `QWidgetItem` 与实现接口。跑通全部现有单测。
2. **Step B（物理搬移）**：算法函数整体移入 `SARibbonPanelLayoutEngine::layout()`，widgets 的 `SARibbonPanelLayout::doLayout()` 退化为"收集 items → 调引擎 → 通知重绘"的适配器（预计 < 300 行）。**搬移是纯 move，不改一行算法逻辑**，diff 可 review。

调试插桩（`SARibbonPanelLayout_DEBUG_PRINT` 相关 qDebug 序号日志）**不随算法迁移**，留在 widgets 适配器层——core 不带调试打印。

#### 3.4.3 CategoryLayoutEngine（从 `SARibbonCategoryLayout::updateGeometryArr` 等提取）

```cpp
class SARIBBON_CORE_EXPORT SARibbonCategoryLayoutEngine
{
public:
    struct Result {
        QSize contentSize;      // 原 categoryContentSize()
        int totalWidth = 0;
        // 每个 panel 的 x/宽度已通过 item->applyGeometry 回写
    };
    Result layout(QVector<SARibbonAbstractLayoutItem*>& panels,  // 每项包装一个 panel
                  const QRect& availableRect,
                  const SARibbonMetrics& metrics);

    // 滚动偏移纯计算（原 scroll/scrollTo 的数值部分）；动画（QPropertyAnimation/
    // QML Behavior）由前端拿目标值自行播放
    int clampScrollOffset(int requested, int contentWidth, int viewportWidth) const;
};
```

panel 的 `sizeHint`/`titleHeight` 由 widgets 侧的 `SARibbonCategoryLayoutItem`（实现契约接口，包装 `SARibbonPanel`）提供；QML 侧由 `RibbonPanel` QQuickItem 的宿主包装。**panel 宽度分配规则、扩展列规则只此一份**。

#### 3.4.4 BarGeometryEngine（从 `SARibbonBarLayout` 部分提取）

只下沉纯几何与度量部分：

| 下沉 | 留 widgets |
|------|-----------|
| 标题行/Tab 行/右侧区域划分算法（`layoutTitleRect` 的几何推导） | `doLayout()` 对具体 QWidget 的摆放执行 |
| `calcMinTabBarWidth()`（最小 tab 宽估算） | 系统按钮栏 `SARibbonSystemButtonBar` 尺寸联动 |
| 上下文标签页的 tab 几何排布（context category 颜色规则进 theme） | frameless/`SAFramelessHelper`、窗口图标控件 |
| 最小模式/正常模式高度切换的度量（→ `SARibbonMetrics`） | QSS 应用、mdi 控件样式 |

QML 侧 `RibbonBar` 的 C++ 宿主调用同一引擎排 tab 行——这是双版本"看起来是同一个产品"的关键。

### 3.5 data/ 子系统

- **`RowProportion` / `PanelLayoutMode` / `RibbonButtonStyle` / `ToolButtonPopupMode` 等全部枚举入 `global/SARibbonEnums.h`**。`RowProportion` 现定义在 `SARibbonPanelItem.h:36-42`（已核实）且与 QAction 属性名约定（`_sa_RowProportion` 等，`SARibbonPanelItem.h:59-66` 的 `SA_ActionPropertyName_*` 宏）耦合——**属性名字符串常量也入 core**，QML 侧经 `QAction`（Qt6 属 QtGui）或 QML 属性映射使用同一约定。
  > 【round1 评审修正与决策】① 实名核对：`RibbonButtonStyle` 实为 `RibbonButtonType`；`ToolButtonPopupMode` 是 Qt 原生枚举（`QToolButton::ToolButtonPopupMode`），**不可下沉**；"BarMode"实为 `RibbonBar::RibbonMode`（嵌套枚举）。全量 16 项枚举盘点表见 `plans/3.0/02-core-sinking.md` 附录 A。② 命名空间决策：下沉的既有公共枚举**维持全局命名空间**（兼容 2.x 用户代码，`SARibbonTheme` 等不改名）；`SARibbon::Core` 命名空间仅用于 core 新增类型（`SARibbonMetrics`、契约接口、三布局引擎、`SARibbonThemeData` 等）。§3.3/§3.4 代码块中的 `namespace SARibbon::Core` 与此不冲突。
- **`SARibbonCustomizeData` 纯数据记录下沉**（§1.3-P9）：操作类型枚举 + 目标描述（category/panel/action 的 key 引用），不含 QWidget 指针。apply/读取 UI（`SARibbonCustomizeDialog/Widget`）留 widgets。QML 版定制功能（3.1+）复用同一记录结构。

### 3.6 factory/ 子系统（P6 修订）

```cpp
// core/factory/SARibbonElementFactoryInterface.h
namespace SARibbon::Core {
class SARIBBON_CORE_EXPORT SARibbonElementFactoryInterface
{
public:
    virtual ~SARibbonElementFactoryInterface();
    // 以 core 类型/枚举表达创建契约；返回 void* 不类型安全，
    // 改为按前端拆分接口（KDDW ViewFactory 模式：core 定义，前端子类实现）
};
}
```

实际做法对齐 KDDW：**接口定义放 core、实现类留 widgets**。widgets 的 `SARibbonElementFactory` 保持 2.x 类名与全部 API（用户子类无感知），只是虚函数的落点改从 core 接口继承。QML 模块 3.0 不提供 ElementFactory（QML 用户用 delegation 定制），接口预留使其 3.x 可以提供而不破坏 core。**若 M1 阶段评估发现接口化成本高，允许降级为"仅命名空间对齐、暂不继承"，列入 D7 决策一并定**——不为纯预留付大成本。

### 3.7 core 的禁区（CI 强制）

- 禁止 include：`QWidget`、`QLayout`、`QSS`/`QStyle`、`QQuickItem`、`QQml*`、`QApplication`
- 禁止链接：`Qt::Widgets`、`Qt::Quick`、`Qt::Qml`
- 唯一 Qt 模块依赖：`Qt::Core`、`Qt::Gui`
- Qt6 下 `QAction` 属 QtGui 但 **Qt5 属 QtWidgets** → core 一律不 include `QAction`（D3 维持方案 a：GalleryItem 整体留 widgets；见 §9-D3/D8）

---

## 4. SARibbonWidgets 模块设计

### 4.1 总体形态：从"逻辑拥有者"到"渲染 + 适配器"

2.x 每个控件类 = 结构管理 + 布局计算 + 事件处理 + 渲染 四合一。3.0 widgets 模块的角色变化：

| 2.x 组成 | 3.0 去向 |
|---------|---------|
| 结构管理（addCategory/addPanel/addAction、context category 切换） | 暂留 widgets（Tier 2 再评估下沉，D7）；QML 侧独立实现但共享枚举/语义 |
| 布局计算 | **core 引擎**；widgets 只剩 QLayout 适配器 |
| 事件处理（hover/click/popup） | widgets（渲染与原生交互） |
| 渲染（paintEvent/QSS） | widgets（合法双实现） |
| 主题应用 | core 数据 + widgets QSS 渲染 |
| ActionsManager / Gallery / CustomizeDialog | widgets（QAction/复杂弹层） |

### 4.2 QLayout 适配器模式

```cpp
// widgets: SARibbonPanelLayout 变形后（示意）
QSize SARibbonPanelLayout::sizeHint() const
{
    if (mDirty) {
        auto items = collectItems();            // SARibbonPanelItem* → 契约接口数组
        mCache = mEngine.layout(items, geometry(), hostMetrics(), panelInput());
        mDirty = false;
    }
    return mCache.sizeHint;
}

void SARibbonPanelLayout::doLayout()
{
    auto items = collectItems();
    const auto r = mEngine.layout(items, geometry(), hostMetrics(), panelInput());
    for (auto* it : items)
        it->applyGeometry(it->geometry);        // 2.x 的 widget()->setGeometry 在此归口
    layoutTitleAndOptionButton(r);              // 伪项同样由引擎排布
}
```

`SARibbonPanelItem : public QWidgetItem, public SARibbon::Core::SARibbonAbstractLayoutItem`——`sizeHint()` 直接用 `QWidgetItem` 实现，`applyGeometry()` 调 `widget()->setGeometry()`。**2.x 的重入守卫、LayoutRequest 投递等 Qt 布局系统交互全部留在适配器**（这正是"算法归 core、系统耦合归前端"的分界线）。

`SARibbonBarLayout`、`SARibbonCategoryLayout` 同构改造：几何计算委托 `BarGeometryEngine`/`CategoryLayoutEngine`，动画、系统按钮、无边框逻辑留守。

### 4.3 留守清单（明确不移的）

`SARibbonMainWindow`、`SAFramelessHelper`、`SARibbonSystemButtonBar`、`SARibbonStackedWidget`、`SARibbonMdiControlsStyle`、`SARibbonActionsManager(+Model)`、`SARibbonGallery`/`GalleryGroup`/`GalleryItem`、`SARibbonCustomizeDialog/Widget`、`colorWidgets/` 全部、各按钮/容器 QWidget、QSS 资源。

### 4.4 兼容层（保留 v1 方案）

- 转发头：`include/SARibbonBar/SARibbonBar.h → include/SARibbonWidgets/SARibbonBar.h`
- 兼容宏：`SA_RIBBON_EXPORT ≡ SA_RIBBON_WIDGETS_EXPORT` 等
- `SARibbonPannel` 拼写修正：过渡期提供 `using SARibbonPannel = SARibbonPanel;` 类别名（P7）
  **【现实更新：拼写修正已由 2.9.x 完成（f553de7），现仓库公开 API 只有单 n 的 `SARibbonPanel`，
  此别名过渡不再需要——见 plans/3.0/NOTES.md B1；本条仅存档 v1/v2 原始方案】**
- 2.x 公共 API 全量保留：布局算法下沉是**内部实现替换**，`SARibbonPanelLayout` 类名、公共方法、信号均不变

---

## 5. SARibbonQml 模块设计（修订 v1 §6）

### 5.1 实现模式：C++ 结构宿主 + QML 视觉叶子（KDDW 混合模式，替代 v1 的 D5 二选一）

v1 D5 在 `QQuickPaintedItem` 与"纯 QML/Controls2 Style"之间二选一，两个选项都有明显短板。KDDW qtquick 的实际做法是第三条路：

- **结构宿主 = C++ QQuickItem**（`qtquick/views/TabBar.h:44` —— C++ 类，实现 `Core::TabBarViewInterface`，持有 `DockWidgetModel`）；负责接收引擎几何、管理模型、处理事件
- **视觉叶子 = .qml 文件**（`qtquick/views/qml/TabBar.qml` 等 15 个）组合样式
- **Instantiator = 声明式包装**（`DockWidgetInstantiator` 等，`QmlTypes.cpp:23-26` 注册）供用户在 QML 里声明式构建

SARibbonQml 照搬此模式：

```qml
// 用户侧 QML（示意）
import SARibbon 3.0

ApplicationWindow {
    RibbonBar {
        RibbonCategory {
            title: qsTr("Home")   // PanelMode.ThreeRow 由 metrics 决定
            RibbonPanel {
                title: qsTr("Clipboard")
                RibbonToolButton { action: cutAction; rowProportion: Ribbon.Large }
                RibbonToolButton { action: pasteAction; rowProportion: Ribbon.Small }
            }
        }
    }
}
```

内部：`RibbonPanel`（C++ QQuickItem）在 `updatePolish()` 收集子按钮的 sizeHint → 调 `PanelLayoutEngine::layout()` → 按结果 `setGeometry`/`setImplicitSize`。**引擎一个字都不改**。按钮视觉用 QML（Rectangle/Icon/Text）或 `QQuickPaintedItem` 逐个评估——渲染层允许两种并存，结构层不允许。

### 5.2 类型清单（修订 v1 §6.2）

| QML 类型 | 实现层 | 依赖的 core 部分 | 优先级 |
|----------|--------|-----------------|--------|
| `RibbonTheme`（singleton） | C++ 桥 | theme/ 全部 | P0 |
| `RibbonMetrics`（singleton 或 attached） | C++ 桥 | metrics/ | P0（v1 缺失，新增） |
| `RibbonBar` | C++ 宿主 + qml | BarGeometryEngine、theme | P0 |
| `RibbonCategory` / `RibbonTab` | C++ 宿主 | 枚举、theme（context 色） | P0 |
| `RibbonPanel` | C++ 宿主 + qml | **PanelLayoutEngine**、metrics | P0 |
| `RibbonToolButton` | qml（QQuickPaintedItem 视觉复杂度而定） | RowProportion、theme、metrics | P0 |
| `RibbonSeparator` / `RibbonLine` | qml | theme | P1 |
| `RibbonQuickAccessBar` | C++ 宿主 | metrics | P1（v1 未列，补） |
| `RibbonGallery` / `RibbonColorButton` / `RibbonMenu` | 后期 | （GalleryItem 在 widgets，需 D8） | P2 |

### 5.3 注册方式（保留 v1 双轨）

Qt5：`saRibbonRegisterQmlTypes(QQmlEngine*)` 命令式；Qt6：`qt_add_qml_module(URI SARibbon VERSION 3.0)`。URI 统一 `SARibbon`，版本 3.0。qmldir/plugin 资源随 qml 模块安装。

### 5.4 主题桥

`RibbonTheme` 单例（`qmlRegisterSingletonType`）包装 core 的 `SARibbonThemeData`：暴露 palette 颜色为 QML 属性、转发 `themeChanged`。QML 控件绑定这些属性 → core 一处切主题，widget 窗口与 QML 窗口同步变化。

### 5.5 与 widgets 的关系（修订 v1 §6.4 的表述）

依旧**不依赖** SARibbonWidgets，平行依赖 core。但删除"允许复制/改写"条款，替换为 §2.2 铁律：**发现需要复制算法 = core 缺口 = 补 core**。Gallery 这类 P2 控件在 widgets 侧有 QAction 依赖时，先走 D8 决策（core Action 抽象）再实现 QML 版，宁可推迟也不复制。

---

## 6. 构建体系（保留 v1 §5，增强两点）

### 6.1 保留

`sa_add_library` / `sa_sync_include` / 组件化 `SARibbonConfig.cmake` / 命名空间 target / 静态导出宏方案 / `SARIBBON_BUILD_QML` 默认 OFF / amalgamate 按模块改造 / Python 绑定路径适配——全部维持 v1 设计，不赘述。（2026-09 注：QWK 原型宏实名 `qwk_add_library`，`sa_*` 为 SARibbon 侧新命名，由计划 01 S5 实现；现仓库 `cmake/SARibbonUtils.cmake` 仅含一个未被调用的 16 行 `saribbon_set_bin_name` 宏，见 plans/3.0/NOTES.md 与 cross-findings。）

### 6.2 增强：core 纯净性门禁（新增）

v1 只有"core 独立编译"一个 CI job，**挡不住** core 源码里 sneak 进 `#include <QWidget>`（QtGui 传递可见性下照样编过）。增加：

```yaml
# CI step: core purity scan
- script: |
    # 1) 头文件扫描：禁用 include 清单
    python tools/check_core_purity.py src/core/ \
        --forbid-include QWidget QLayout QStyle QQuickItem QQmlEngine QApplication QAction
    # 2) 组合构建：Widgets=OFF Qml=ON、Widgets=OFF Qml=OFF 两个矩阵项
```

扫描脚本进 `tools/`，本地 `cmake --build` 前也建议跑（可选 pre-commit hook）。

### 6.3 增强：测试目录升格（见 §7）

`tests/core/` 承载引擎黄金值测试，成为**双前端共同依赖的契约测试**，CI 中强制最先运行。

---

## 7. 测试策略（v2 新增）

### 7.1 黄金几何测试（core 引擎）

布局算法下沉后第一次可以脱离 QApplication widget 树做确定性测试：

```cpp
// tests/core/tst_panelLayoutEngine.cpp
void TestPanelLayoutEngine::threeRowMixedProportions()
{
    FakeItem largeBtn(QSize(48, 68)), small1(QSize(24, 22)), small2(QSize(24, 22));
    largeBtn.rowProportion = RowProportion::Large;
    // ...
    SARibbonMetrics m = makeTestMetrics(fontOf14px);   // 固定输入，跨平台稳定
    auto r = engine.layout(items, QRect(0, 0, 200, 86), m, {});
    QCOMPARE(items[0].geometry, QRect(0, 0, 48, 82));  // 黄金值：与 2.9.5 实测一致
    QCOMPARE(items[1].rowIndex, 0);
    // ...
}
```

- 黄金值先用 2.9.5 跑现有面板录制（保证行为不变），再作为 3.0 回归基线
- `FakeLayoutItem`（纯内存实现契约接口）使测试无需真实控件，**双前端共享同一套期望值**
- 覆盖：三行/两行/单行 × Large/Medium/Small 任意排列、隐藏项、expand 项（`recalcExpandGeomArray`）、sizeHint 缓存失效

### 7.2 跨前端一致性套件（KDDW tests_* 模式的轻量版）

P0 QML 类型交付时，构造**同一场景**分别在 widgets 与 qml 下实例化，断言引擎输出层的一致（不是像素级）：

- 同一 metrics + 同一组 (sizeHint, rowProportion) 输入 → 引擎结果一致（天然满足，7.1 已覆盖）
- 双前端各自把引擎几何正确应用到了真实 item 上（widgets 读 `QWidget::geometry()`，qml 读 `QQuickItem::x/y/width/height`）
- 主题切换信号双端都触发了重渲染

实现上暂不需要 KDDW 的 `Platform::tests_*` 全套——用一个共享的 `tests/common/RibbonConformance.h` 定义场景数据，两个前端测试工程各自包含执行即可。

### 7.3 现有 26 个单测迁移（v1 已有，保留）

widgets 类的测试随类走；主题/颜色测试下沉 `tests/core/`。
（2026-09 实测：`tests/CMakeLists.txt` 共注册 26 个 `add_saribbon_test()`——`tests/` 顶层
25 个 .cpp 各一项，另有 `tests/auto/SARibbonThemePalette/tst_themepalette.cpp` 注册为
`SARibbonThemePaletteTest`；ctest 基线 N₀=26，见 plans/3.0/NOTES.md B8。）

---

## 8. 实施阶段计划（修订 v1 §8）

| 里程碑 | 内容 | 交付判据 |
|--------|------|---------|
| **M0 准备**（不变） | `dev-3.0` 分支；`cmake/SARibbonUtils.cmake`；三模块空骨架；**core 纯净性扫描脚本**（v2 提前到 M0） | 三空 target 可配置安装；扫描脚本在 CI 运行 |
| **M1 Core 下沉**（v2 重排：**引擎提取成为关键路径**） | ① 枚举/Util/Qt5Compat/Global 入 core；② ThemeManager 拆分（数据+通知入 core，QSS 应用留 widgets）；③ **Metrics 收口**（BarLayout 度量函数迁入 `SARibbonMetrics`，widgets 委托）；④ **三布局引擎两步提取**（Step A 接口化原地重构 → Step B 物理搬移）；⑤ **黄金几何测试**：先对 2.9.5 行为录制期望值，搬移后跑通；⑥ CustomizeData 纯数据下沉 | 仅 Core+Widgets 构建通过；现有 example/tests 全绿；**黄金几何测试 100% 通过（行为零变化证明）**；core-only 独立编译 + 纯净扫描绿 |
| **M2 构建收尾**（不变） | amalgamate 按模块（Core 版 + Core+Widgets 版）；sip/pyside 路径；CI 全矩阵 + 纯净扫描矩阵项 | CI 绿；单文件产物可编译 StaticExample；轮子可构建 |
| **M3 QML 首版**（v2 细化） | P0 类型（Theme/Metrics 单例、Bar/Category/Panel/ToolButton）；C++ 宿主 + QML 叶子混合实现；**跨前端一致性套件**；`examples/qml` | QML 版与 widgets 版同屏对比视觉一致（同一字体下条高/间距/装箱一致）；一致性测试绿 |
| **M4 发布**（不变） | 文档三模块重写；QML API 文档；迁移指南（含 Pannel→Panel 拼写【注：修正已在 2.9.x 完成（f553de7），指南只作结论性说明，无别名过渡】、度量对照表）；3.0.0 | 文档、发布包、轮子齐备 |

**Tier 分级（控制范围蔓延）**：

- **Tier 1（3.0 必须）**：theme、metrics、三个布局引擎、契约接口、枚举/CustomizeData、factory 接口评估
- **Tier 2（3.1，按 QML 用户实际反馈触发）**：结构控制器（RibbonBar/Category/Panel model 下沉，widgets 类转视图+转发，见 D7）、QuickAccessBar 数据模型、跨端定制（CustomizeData apply 下沉）
- **Tier 3（3.2+）**：core Action 抽象（D8）、Gallery/ActionsManager 下沉、QML P2 控件

依赖：M0 → M1 → M2 → M4；M3 依赖 M1（引擎可用）可与 M2 并行。

---

## 9. 决策点（v1 D1–D5 修订 + 新增 D6–D8）

| # | 决策 | 建议 |
|---|------|------|
| D1 最低 Qt 版本 | **5.15**（维持 v1 建议）。理由补充：`qt_add_qml_module` 需 6.2+，5.15 是最后支持双轨注册的 5.x；QWindowKit 亦以 5.15 为界 | ✅ 采纳 5.15 |
| D2 target 命名 | `SARibbonCore/Widgets/Qml` + 别名 `SARibbon::Widgets`；**额外**提供 `SARibbonBar` 兼容别名 target 一个版本周期 | ✅ v1 方案 + 别名增强 |
| D3 GalleryItem 归属 | **方案 a：整体留 widgets**。补充论证：KDDW 为解同类问题自研 `Core::Action`，成本一个独立抽象层——仅当 Gallery 要进 QML（P2/Tier 3）时才值得付，与 D8 合并决策 | ✅ 维持 a |
| D4 colorWidgets 宏 | 废除独立宏并入 widgets 导出宏（v1 建议）。colorWidgets 是纯 widget 控件，无 QML 共享诉求 | ✅ 维持 |
| D5 QML 实现技术 | **废弃二选一，改混合模式**：结构宿主 C++ QQuickItem（接引擎几何/模型/事件），视觉叶子 QML 优先、复杂自绘项允许 QQuickPaintedItem（KDDW qtquick 同款）。渲染层允许混用，结构层禁止 QML 自行计算布局 | 🔄 v2 修订 |
| **D6 布局引擎提取深度**（新） | 本方案 §3.4 全量（三引擎 + 契约 + metrics）。替代 v1 的"布局全留 widgets" | 建议：全量。若 M1 排期压力，**降级顺序**：BarGeometryEngine 可只下沉度量部分（doLayout 几何留 widgets），Panel/Category 引擎不可降——它们是 QML P0 的一致性根基 |
| **D7 结构控制器时机**（新） | Tier 2（3.1）。gate 条件：出现 QML 用户需要 ① C++ 运行时驱动同一 ribbon 数据双端显示，或 ② 跨端运行时定制。满足任一才启动 controller 拆分（widgets 类转为视图+API 转发，工作量大）；否则维持"双端各自结构管理 + 共享枚举/语义/一致性测试" | 建议：3.0 不做，写死 gate |
| **D8 core Action 抽象**（新） | 不做（3.0/3.1）。触发条件：Gallery 或 ActionsManager 需要进 core（即 QML P2 启动时）。届时按 KDDW `Core::Action` 模式评估：包装层 vs Qt6-only 双轨 | 建议：推迟 |

---

## 10. 风险与缓解（修订 v1 §9）

| 风险 | 缓解 |
|------|------|
| R1 引擎提取引入行为回归（三布局 5300 行动刀） | 两步走（接口化→纯 move）；黄金几何测试先录 2.9.5 基线；M1 期间禁改算法逻辑（发现 bug 单独提交、单独测试） |
| R2 布局重入/Qt 布局系统交互（`SARibbonPanelLayout.cpp:1831` 的 setGeometry 重入守卫等）留在适配器后仍出问题 | 这些守卫**全部留 widgets 适配器**不动；QML 侧无 QLayout 系统，天然没有此类问题；一致性测试双端验证应用结果 |
| R3 metrics 收口后默认值与 2.x 不一致 | 度量对照表 + 截图对照入 M1 验收 |
| R4 QML 结构宿主接引擎后性能（QQuickItem 无 QLayout 惰性机制） | 引擎内部保留 sizeHint 缓存与 dirty 标记（随算法进 core）；`updatePolish()` 驱动而非每帧重算；示例加 resize 压测 |
| R5 黄金值测试平台字体差异 | 测试固定字体（QFont("fixed test font") 或部署字体文件），不依赖系统字体 |
| R6 Tier 2 结构控制器迟迟不做导致双端行为漂移 | §7.2 一致性套件持续在 CI 对拍双端语义；漂移在测试层暴露而非用户层 |
| R7 双分支同步（2.x bugfix vs 3.0） | 维持 v1 策略：2.x bugfix 尽量落在将整体搬移的文件内；**引擎文件除外**——引擎内的 2.x fix 必须手动同步 core 版并跑黄金测试 |
| R8 core 纯净性被悄悄破坏 | §6.2 扫描脚本 CI 前置，merge blocking |

---

## 附：与 KDDockWidgets 的结构对照（速查）

| KDDockWidgets | SARibbon 3.0 (v2) | 说明 |
|---------------|-------------------|------|
| `src/core/`（114 文件：69 .h + 45 .cpp，全部 controller + 引擎） | `src/core/`（theme/metrics/layout/contract/data/factory） | KDDW 全面 controller 化；SARibbon 取其引擎与契约，控制器分期（D7） |
| `src/core/layouting/`（Item/SizingInfo，零 GUI） | `src/core/layout/`（三引擎 + FakeItem 可测） | 同一思想：布局引擎独立成库内最纯净的部分 |
| `core/views/TabBarViewInterface.h`（窄契约） | `contract/SARibbonAbstractLayoutItem.h` 等 | KDDW 契约面向 controller↔view；SARibbon 面向 engine↔前端 |
| `src/qtcommon/`（双 Qt 前端共享胶水） | 不设（Qt5Compat 留 core） | SARibbon 胶水量小，膨胀时再设 |
| `src/qtwidgets/views/` + `src/qtquick/views/`（每 controller 一对视图） | widgets 控件 + qml C++ 宿主 | SARibbon widgets 侧保持 2.x 类名做视图 |
| `qtquick/*Instantiator` + `views/qml/*.qml` | `Ribbon*` QML 类型（C++ 宿主 + qml 叶子） | 声明式 API 模式一致 |
| `Core::Platform` + `tests_*` 跨前端测试 | `tests/common/` 一致性套件 | 取其神（同套测试双端跑），不搬其形（Platform 单例） |
| 单库拼合构建 | QWK 三库 | 保持 v1 |
