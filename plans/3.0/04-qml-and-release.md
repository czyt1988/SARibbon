# 计划 04：SARibbonQml 首版与 3.0.0 发布（对应 v2 里程碑 M3 + M4）

> 前置计划：[03-build-ecosystem.md](03-build-ecosystem.md) 验收门全部通过
> （M3 技术上只依赖 M1：若并行开发，必须先满足计划 02 的 S1–S7；对 agent 建议线性执行）
> 设计依据：v2 计划 §5（QML 模块）、§7.2（跨前端一致性）、§8-M3/M4、§9-D5（混合模式）、§2.2（铁律）
> 参考项目（本地已核实，行号以检阅时的副本为准）：`F:\src\3rdparty\KDDockWidgets`（下称 **KDDW**，重点 `src/qtquick/`）、`F:\src\3rdparty\qwindowkit`（下称 **QWK**，重点 `src/quick/`）
> 分支：`dev-3.0`（延续）

## 1. 目标

1. **SARibbonQml P0 类型集**：以"C++ 结构宿主（QQuickItem，接收 core 引擎几何）+ QML 视觉叶子"的混合模式（v2 D5）实现：`RibbonTheme`（单例桥）、`RibbonMetrics`（单例）、`RibbonBar`、`RibbonCategory`、`RibbonTab`、`RibbonPanel`、`RibbonToolButton`；P1：`RibbonSeparator`/`RibbonLine`/`RibbonQuickAccessBar`（时间允许）。
2. **跨前端一致性套件**：`tests/common/RibbonConformance.h` 场景数据 + 双前端各自测试工程。
3. **QML 示例**：`examples/qml/QmlMainWindowExample`，与 widgets 版 `MainWindowExample` 同屏对比。
4. **M4 发布**：文档三模块重写、迁移指南、版本 3.0.0、tag 与发布物料。

**铁律（v2 §2.2）**：QML 侧发现需要"复制"widgets 的一段算法 = core 缺口 = 停下来补
core（必要时回计划 02 补一个引擎缺口），**禁止在 QML 里重写布局计算**。渲染允许双实现。

## 2. 范围与非目标

**非目标（v2 P2/Tier 2-3，明确推迟）**：`RibbonGallery`/`RibbonColorButton`/`RibbonMenu` 的 QML 版（触发 D8 决策流程）；结构控制器（D7）；core Action 抽象；Python 侧 QML 绑定；QuickAccessBar 之外的 P1 项不做也不留残缺代码。

## 3. 前置条件（逐项验证）

| # | 检查 | 期望 |
|---|------|------|
| P1 | 计划 03 验收门全绿 | 复核 [03-build-ecosystem.md](03-build-ecosystem.md) 第 6 节验收门全勾 |
| P2 | core 引擎可用 | `tests/core` 三引擎黄金测试全绿；`SARibbonMetrics`/契约接口在同步头目录可被 `<SARibbonCore/...>` include |
| P3 | QML 构建开关 | `SARIBBON_BUILD_QML` 存在且默认 OFF（计划 01 S4.3）；`src/qml/` 骨架在位（计划 01 S6.5） |
| P4 | 本机 Qt Quick 可用 | `find_package(Qt6 COMPONENTS Quick Qml QuickControls2)` 通过（Qt 6.7.3 全装通常满足；若缺组件，先装齐再开工。QuickControls2 供示例 `ApplicationWindow` 与 Controls 叶子使用，KDDW 库侧同样链接之，KDDW `src/CMakeLists.txt:496`） |
| P5 | 测试基线 | ctest == N₀（widgets 侧无回归） |
| P6 | 纯净脚本在位 | `tools/check_core_purity.py` 已由计划 01 S9 创建并通过 core 扫描（本计划 S8 在其上扩展，非从零新建） |

## 4. 全程纪律

- [README.md](README.md) R1–R4 适用。
- 每新增一个 QML 类型，同步更新 `tests/qml/` 的一致性测试与 `examples/qml/`（不留"只写类型不写测试"的空窗）。
- QML 模块**禁止链接 SARibbonWidgets**（依赖矩阵红线，v2 §5.5）；CI 有专项验证（S8）。

## 5. 执行步骤

### S1 模块骨架与注册双轨（§5.3）

**参考项目实测结论（评审时逐文件核实）**：KDDW 与 QWK 在 **Qt5/Qt6 两端都只用命令式注册**，均无 `qt_add_qml_module`、无 QML plugin、无手工 qmldir：

- KDDW：`src/qtquick/QmlTypes.cpp:21-30` `registerQmlTypes()`——`qmlRegisterType<MainWindowInstantiator>("com.kdab.dockwidgets", 2, 0, "DockingArea")` 等 4 个类型（:23-26）+ `qmlRegisterUncreatableMetaObject(KDDockWidgets::staticMetaObject, ...)` 暴露命名空间枚举（:28）；由库内 `Platform::init()` 自动调用（`src/qtquick/Platform.cpp:86`），应用侧零注册代码。QML 叶子文件走 qrc（`src/qtquick/kddockwidgets_qtquick.qrc`，前缀 `/kddockwidgets/qtquick/`，15 个 .qml），叶子由 C++ 以 `QQmlComponent(engine, url)` 创建（`src/qtquick/views/View.cpp:163-173` `createItem`；URL 表在 `ViewFactory.cpp:176-201`）。
- QWK：`src/quick/qwkquickglobal.cpp:12-26` 导出 `QWK::registerTypes(QQmlEngine*)`——`qmlRegisterType<QuickWindowAgent>("QWindowKit", 1, 0, "WindowAgent")` + `qmlRegisterModule("QWindowKit", 1, 0)`，`static bool once` 防重入；应用侧在 `engine.load` 前显式调用（`examples/qml/main.cpp:24-29`）。库 CMake 就是普通库（`src/quick/CMakeLists.txt`：`QT_LINKS Core Gui Quick`），无任何 qml module 机制。
- 两家的 Qt5/Qt6 双支持都**不靠注册机制分轨**（命令式 API 在两版 Qt 中同签名存在，Qt 6.7.3 `include/QtQml/qqml.h:297/645/674/727` 仍可查），差异只在 `QT_VERSION` 宏包裹的零星适配。

v2 §5.3 决策保留双轨（Qt6 声明式 + Qt5 命令式），执行细节与坑如下：

**操作**：
1. `src/qml/CMakeLists.txt`：`find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED COMPONENTS Core Gui Quick Qml QuickControls2)`；`sa_add_library(SARibbonQml SOURCES <显式清单，禁止 GLOB> PREFIX SA_RIBBON_QML QT_LINKS Core Gui Quick Qml LINKS SARibbonCore)`（函数签名见计划 01 S5；别名 `SARibbon::Qml` 由其自动生成）+ `sa_sync_include(SARibbonQml SARibbonQml)`。
2. **Qt6 声明式轨**：`qt_add_qml_module(SARibbonQml URI SARibbon VERSION 3.0 RESOURCE_PREFIX /qt/qml QML_FILES qml/RibbonPanel.qml ...)`。硬约束（本机 Qt 6.7.3 `lib/cmake/Qt6Qml/Qt6QmlMacros.cmake` 行号）：
   - **调用顺序与作用域**：必须在 `sa_add_library` 建出 target 之后、且与 target 创建**同一目录作用域**内调用（跨作用域要求 CMake ≥3.18，否则告警且依赖/可见性有问题，macros:196-209）——即写在 `src/qml/CMakeLists.txt` 末尾，不得挪去 `src/CMakeLists.txt`。
   - **backing target 静/动态**：已有库作 backing 时按其类型走——`STATIC_LIBRARY`→模块 STATIC、`SHARED/MODULE_LIBRARY`→SHARED（macros:234-241）；`SARIBBON_BUILD_STATIC_LIBS=ON` 组合下插件 target 默认名 `SARibbonQmlplugin`（macros:287），静态插件经 plugin_init object library 传播（macros:664-679），消费端极端情况需 `qt_import_qml_plugins()`（macros:3522）——静态组合必须在 S6 示例上单独验证。
   - **RESOURCE_PREFIX 必须显式 `/qt/qml`**：缺省是 `/`（macros:466），模块会落在 `qrc:/SARibbon/`——不在引擎内建导入路径上，应用得手工 `addImportPath` 或设 `QML_IMPORT_PATH`；而引擎无条件 `addImportPath("qrc:/qt/qml")`（qtdeclarative 源码 `src/qml/qml/qqmltypeloader.cpp:1213,1239`，本机 6.10.1 Src 已核），放 `/qt/qml` 后任何链接了 SARibbonQml 的应用 `import SARibbon` 即可解析。`AUTO_RESOURCE_PREFIX` 已废弃（macros:447-455），不要用。
   - **C++ 类型标注**：头文件 `#include <QtQml/qqmlintegration.h>` 后加 `QML_ELEMENT` / `QML_SINGLETON` / `QML_UNCREATABLE`（6.7.3 定义于 `include/QtQmlIntegration/qqmlintegration.h:45/86/63`）。这些宏 **Qt6 独有**（Qt 5.14.2 头树 grep 无定义），必须 `#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)` 包裹（`qt_add_qml_module` 本身需 Qt ≥6.2，v2 D1）。`QML_SINGLETON` 的类需可默认构造或提供 `static T* create(QQmlEngine*, QJSEngine*)`——RibbonTheme/RibbonMetrics 用后者转发到 `instance()`。
   - `NO_PLUGIN`/`NO_PLUGIN_OPTIONAL`/`NO_CREATE_PLUGIN_TARGET` 选项存在（macros:23-25）但**不要**在共享库默认路径使用：无插件时 QML_ELEMENT 类型注册依赖 backing target 被应用直接链接并触发 qmltyperegistrar 初始化，行为不如默认插件路径直观。
3. **Qt5（5.15）命令式轨**：导出函数 `saRibbonRegisterQmlTypes(QQmlEngine* = nullptr)`（仿 QWK，static-once 防重入；声明进 `SARibbonQmlGlobal.h`，`SA_RIBBON_QML_EXPORT` 导出）：
   - 普通类型：`qmlRegisterType<T>("SARibbon", 3, 0, "RibbonPanel")`；不可实例化枚举持有类：`qmlRegisterUncreatableType<...>`（KDDW QmlTypes.cpp:28 的 metaObject 变体要求枚举在 `Q_NAMESPACE` 内，SARibbon 枚举现状不满足，见 S3.4）。
   - 单例：优先 `qmlRegisterSingletonInstance("SARibbon", 3, 0, "RibbonTheme", RibbonTheme::instance())`（Qt 5.14+ 可用，5.14.2 `include/QtQml/qqml.h:629-687`；Qt 6.7.3 同 API 在 qqml.h:727，双轨可共用）。回调式签名为 `QObject* (*)(QQmlEngine*, QJSEngine*)`（5.14.2 qqml.h:645-660，另有 `QJSValue` 返回与 `std::function` 变体 :629/:663）。
   - **所有权坑（必做）**：回调/instance 注册的单例对象默认归**引擎**所有（引擎析构即 delete），而 RibbonTheme/RibbonMetrics 包装的是 core 进程级单例——注册前 `QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership)`，否则多引擎（测试逐个建 QQmlEngine）第二次即悬空。
   - QML 叶子文件走 `src/qml/saribbon_qml.qrc`（前缀 `/SARibbon/`，两轨共用；KDDW 同款做法），C++ 宿主经 `QQmlComponent` 从 `qrc:/SARibbon/RibbonPanel.qml` 创建叶子。命令式轨**无 qmldir/plugin**，与 KDDW/QWK 一致。
   - CMake 按 `QT_VERSION_MAJOR` 二选一注册源文件；`saRibbonRegisterQmlTypes` 在 Qt6 下也保留实现（内部转调一次即可），保证应用代码不分叉、且是声明式轨的兜底（见下）。
4. `src/qml/SARibbonQmlGlobal.h` 完善导出宏；私有实现头用 `_p.h`。

**兜底预案（记 NOTES 后启用）**：Qt6 保留全部命令式 API（6.7.3 qqml.h:297/645/674/727）——若声明式轨在插件加载、静态库或安装路径上遇到无法快速解决的问题，**退回两版 Qt 单轨命令式**（即 KDDW/QWK 的实证方案）：Qt6 侧同样走 `saRibbonRegisterQmlTypes()`，`qt_add_qml_module` 仅保留管 QML_FILES/资源或干脆换 qrc。URI（`SARibbon`）与版本（3.0）不变，应用侧只多一行调用，设计目标（§5.3）不受损。

**验证**：`cmake -B build-3.0-qml -DSARIBBON_BUILD_QML=ON ...` 配置+编译通过（此时仅有占位类型）；`SARIBBON_BUILD_QML=OFF` 默认路径不受影响；**Qt6 导入路径专项检查**：写最小 main（QQmlEngine + 内联 `import SARibbon 3.0` 组件串），不设 `QML_IMPORT_PATH` 能成功创建占位类型——RESOURCE_PREFIX 正确性的直接证据。

**提交**：`feat：SARibbonQml 模块骨架与双轨注册`

### S2 主题与度量桥（P0：RibbonTheme / RibbonMetrics）

**操作**：
1. `src/qml/theme/RibbonTheme.h/.cpp`（C++ QObject 单例）：包装 core `SARibbonThemeData::instance()`（计划 02 S2.1/S2.3 产物：QObject 单例，信号 `themeChanged(SARibbonTheme)`、`paletteChanged()`）——palette 颜色暴露为 QML 属性（`NOTIFY` 转发上述两信号）、`currentTheme` 可读写。**不实现任何颜色计算**（core 已有）。注册：Qt6 轨 `QML_SINGLETON + QML_ELEMENT` + `static create()` 转发 `instance()`；Qt5 轨 `qmlRegisterSingletonInstance`（S1.3），**两处都要设 CppOwnership**（S1.3 所有权坑）。
2. `src/qml/metrics/RibbonMetrics.h/.cpp`：桥 `SARibbonMetrics`（core 构造签名 = `QFontMetrics` + `devicePixelRatio`，计划 02 S3.1）。**度量全部在 C++ 内构造，QML 侧不参与**：
   - 默认 `QFontMetrics(QGuiApplication::font())`；暴露 `font` Q_PROPERTY(QFont) 允许覆盖（对应 widgets 侧"以 bar 的 fontMetrics 为准"的语义，现状度量公式即取 `ribbonBar->fontMetrics()`，`SARibbonBarLayout.cpp:381/407/432`）；
   - 在 `qApp` 上装事件过滤器监听 `QEvent::ApplicationFontChange`（与计划 02 S3.2 widgets 侧同一事件源），变化时重建 metrics 并 `Q_EMIT metricsChanged`；
   - `devicePixelRatio` 取 `QGuiApplication::devicePixelRatio()`；RibbonBar 宿主可用所在窗口的 `QQuickWindow::effectiveDevicePixelRatio()`（public，6.7.3 `qquickwindow.h:141`）覆盖；
   - 暴露 `tabBarHeight/categoryHeight/panelTitleHeight/...` 为只读属性。
   - 形态：P0 只做**单例**（v2 §5.2 "singleton 或 attached"二选一时取 singleton）；attached 形态需要多 RibbonBar 各带不同字体时才值得做（`QML_ATTACHED`/`qmlRegisterAttachedType`），推迟。
   - ⚠️ 原稿"从 qml fontMetrics 构造"不成立：QML 未暴露 `QFontMetrics` 类型；内建 `TextMetrics`（QtQuick）只有 advanceWidth/boundingRect/elidedText，**没有** `lineSpacing()`/`height()`——而 SARibbon 度量公式恰依赖这两者（上行号）。已按上述 C++ 方案改写。
3. `src/qml/qml/` 下视觉叶子起步（如叶子需要主题色，绑定 RibbonTheme 单例属性）。

**验证**：QML 单测 `tests/qml/tst_themeBridge.cpp`：C++ 改 `SARibbonThemeData::instance()` 的主题 setter，QML 侧断言属性变化信号触发——**双端同一信号源**（v2 §3.2-1 的验收）。测试经与应用相同的注册入口建 QQmlEngine（Qt6：`import SARibbon`；Qt5：先调 `saRibbonRegisterQmlTypes()`）；连续建两个引擎重复断言，顺带验证 CppOwnership 无悬空。

**提交**：`feat：QML 主题与度量桥接单例`

### S3 结构宿主：RibbonPanel（先 Panel，直接检验引擎复用）

**宿主实现要点（API 均已对照本机 Qt 头文件核实，Qt6.7.3 `include/QtQuick/qquickitem.h` / Qt5.14.2 同行号语义一致）**：
- 几何应用 API 真实存在且为 **public**：`setPosition(const QPointF&)`（6.7.3:207；5.14.2:229）、`setSize(const QSizeF&)`（6.7.3:226；5.14.2:244）、`setWidth/setHeight/setX/setY`。KDDW 宿主同款用法：`View::setGeometry(QRect)` = `setSize(rect.width(), rect.height()) + move(rect.topLeft())`（KDDW `src/qtquick/views/View.cpp:157-162`）。
- `setImplicitSize(qreal, qreal)` 是 **protected**（6.7.3:433；5.14.2:411，protected 段自 :420 起）——宿主自身是 QQuickItem 子类，类内调用合法；类外只能走 public 的 `setImplicitWidth/setImplicitHeight`。
- 布局入口 `updatePolish()`：protected virtual（6.7.3:468；5.14.2:450），由 `polish()`（6.7.3:322）请求、场景图每帧 polish 阶段对**挂在 QQuickWindow 上的 item** 调用——裸堆上 item 不会触发（测试注意事项见 S7）。KDDW 未用 updatePolish（其几何由 controller 驱动），SARibbon 无 controller 层（D7），updatePolish 是正确的挂点：每帧至多一次、天然合帧，满足 v2 R4 的性能要求。
- **子项收集不用 `childItems()` 顺序做布局序**：`childItems()` 是 public（6.7.3:189）但返回的是堆叠序（z 值会扰动），且混入非按钮装饰子项。方案：`RibbonToolButton` 在自己的 `componentComplete()`（QQuickItem protected virtual，6.7.3:435-436 `classBegin/componentComplete`）向 `qobject_cast` 出的父宿主登记，宿主自维护显式列表；宿主 override `itemChange()`（6.7.3:424）监听 `ItemChildRemovedChange/ItemVisibleChange` 同步列表并 `polish()`。KDDW 对逻辑顺序同样用显式模型（`DockWidgetModel : QAbstractListModel`，`TabBar.h:113-150`）而非 childItems；其"声明完成才真正建对象"的时机处理见 `DockWidgetInstantiator.h:28-33` 注释（QQmlParserStatus，属性全部就位后再构造实体）。
- 声明顺序 = componentComplete 触发顺序（同一父下按声明序），显式列表按登记序即得用户期望的布局序；运行期 `moveItem(from,to)` 之类的重排 API P0 不做（ customization 属 Tier 2）。

**操作**：
1. `src/qml/panel/RibbonPanel.h/.cpp`：C++ `QQuickItem` 子类；`updatePolish()` 中取显式子项列表的 sizeHint → 构造 `QVector<SARibbonAbstractLayoutItem*>`（QML 侧适配器实现契约接口，或直接让 RibbonToolButton 实现契约接口）→ 调 `SARibbon::Core::SARibbonPanelLayoutEngine::layout(...)` → 按结果对每个子项 `setPosition(QPointF)` + `setSize(QSizeF)`，宿主自身 `setImplicitSize`（类内，protected 合法）。**引擎一个字都不改**（v2 §5.1）。
2. `panelTitle`/`optionButton` 作伪项传入（引擎不感知）。
3. `src/qml/qml/RibbonPanel.qml`：视觉叶子（标题条、边框、背景色绑定 RibbonTheme）；宿主经 `QQmlComponent` 加载为自身子项（KDDW `View::createItem`，View.cpp:163-173）。叶子只做渲染，**不写任何几何计算**。
4. 输入参数（`PanelLayoutMode`、`RowProportion` 等）的 QML 枚举暴露：**不用 `QML_ENUM`——该宏在 Qt 5.14.2 / 6.7.3 / 6.10.1 公共头树中均不存在（grep `define QML_ENUM` 无命中），KDDW 也未使用**。可行做法（两轨通用）：在已注册的 QObject 子类内定义镜像枚举 + `Q_ENUM`（`qmlRegisterType` 与 `QML_ELEMENT` 两条注册路径都会把 Q_ENUM 带进 QML），如 `RibbonToolButton` 内 `enum RowProportion { None, Large, Medium, Small }; Q_ENUM(RowProportion)` + 同名 Q_PROPERTY。core 侧枚举现状：`RowProportion` 是 `SARibbonPanelItem`（非 QObject）的类内普通 enum（`SARibbonPanelItem.h:36`），全局的 `SARibbonAlignment/SARibbonTheme` 是全局 enum class（`SARibbonGlobal.h:197/220`）——都不在 Q_NAMESPACE 内，无法直接用 KDDW 的 `qmlRegisterUncreatableMetaObject` 方案；跨类型共享的枚举放一个 uncreatable 注册类（暂名 `Ribbon`，对应 v2 §5.1 示例的 `Ribbon.Large` 写法）统一挂 Q_ENUM。镜像枚举与 core 枚举间的转换函数集中在一个 `_p` 头，static_assert 值一致。

**验证**：`tests/qml/tst_ribbonPanel.cpp`：给定一组固定 sizeHint 的按钮 + 固定 metrics，断言每项 `QQuickItem` 的 `x/y/width/height` 与 `tests/core` fixture 的黄金几何**逐项相等**（这是 v2 §7.2 "双前端各自把引擎几何正确应用到了真实 item 上"的 Panel 部分）。注意 item 必须挂窗曝光后 polish 才跑，见 S7 的测试要点。

**提交**：`feat：RibbonPanel QML 结构宿主复用 PanelLayoutEngine`

### S4 结构宿主：RibbonBar / RibbonCategory / RibbonTab

**操作**：
1. `src/qml/bar/RibbonBar.h/.cpp`：C++ 宿主——tab 行排布调 `SARibbonBarGeometryEngine`（若计划 02 S7 走了 D6 降级，则此处只用 metrics 部分，tab 几何在宿主内用**从 widgets 语义移植**的规则，**移植前先把该规则补进 core 引擎**——铁律检查点）；`RibbonBar` 高度从 `RibbonMetrics` 取。tab 条本身作为 RibbonBar 的内部子宿主实现（暂名 `RibbonTabBar`）：它**不在 v2 §5.2 的 P0 用户类型清单内**，注册为 uncreatable 或干脆不注册（仅 C++ 内部类），避免公共 API 面膨胀。
2. `src/qml/category/RibbonCategory.h/.cpp`：panel 排布调 `SARibbonCategoryLayoutEngine`；`clampScrollOffset` 用引擎值，滚动动画用 QML `Behavior on x`（前端动画，v2 §3.4.3）。
3. `src/qml/tab/RibbonTab.h/.cpp`：上下文标签页颜色规则从 core theme 取（context 色进 theme，计划 02 S2 已保证；若缺，回补 core）。
4. 对应 `.qml` 视觉叶子；各宿主的子项收集/重 polish 机制复用 S3 要点（显式登记列表 + `itemChange` + `polish()`）。

**验证**：`tests/qml/tst_ribbonBar.cpp`：三 category + context category 场景，断言 tab 几何与高度和 widgets 版一致（读 `QQuickItem` 属性对比黄金值）。

**提交**：`feat：RibbonBar/Category/Tab QML 结构宿主`

### S5 RibbonToolButton 与 P1 杂项

**操作**：
1. `src/qml/button/RibbonToolButton.h/.cpp`（若需自绘）+ `src/qml/qml/RibbonToolButton.qml`：大/中/小三态（`RowProportion`）、图标+文字、菜单弹出的最小实现；视觉复杂度允许 `QQuickPaintedItem`（public 头 `qquickpainteditem.h:13`，继承 QQuickItem；D5：渲染层允许混用）。
2. `action` 属性（模块归属已核：Qt6 `QAction` 在 QtGui——`6.7.3/include/QtGui/qaction.h` 存在；Qt5 在 QtWidgets——`5.14.2/include/QtWidgets/qaction.h` 存在且 QtGui 下无此头。v2 计划行文中的"Qt6/GuiGui"系笔误，以本条为准）：core 一律不 include QAction（v2 计划 §3.7）；QML 按钮此处用自有属性（`text/iconSource/shortcut`）承接，QAction 桥接列入 D8/Tier 3 不做。**为什么不学 KDDW 自研 Action**：KDDW 的 `Core::Action` 存在理由是其 controller 层在 core、要跨 QtQuick 与 Flutter 双 GUI 前端驱动按钮（`src/core/Action.h:17` 注释原文 "Class to abstract QAction, so code still works with QtQuick and Flutter"；QtQuick 端实现是纯 QObject，`src/qtquick/Action.h:21-80`）。SARibbon 3.0 的交互控制留在各前端（D7 Tier 2），core 不消费 action，抽象层没有消费方——与 v2 §2.3 "自研 Core::Action：观察（D8）"一致。
3. P1：`RibbonSeparator.qml`、`RibbonLine.qml`、`RibbonQuickAccessBar`（C++ 宿主 + metrics）。

**验证**：`tests/qml/tst_toolButton.cpp`（sizeHint 三态与黄金值一致）；`examples/qml` 冒烟。

**提交**：`feat：RibbonToolButton 及 P1 QML 类型`

### S6 QML 示例

**参考样板（已核）**：QWK `examples/qml/`——CMake 为普通可执行 + qml 文件进 qrc（`CMakeLists.txt`：`SOURCES main.cpp qml.qrc`，`QT_LINKS Core Gui Qml Quick`）；`main.cpp:24-29`：`QGuiApplication` + `QQmlApplicationEngine` + 注册函数 + `engine.load(QUrl("qrc:///main.qml"))`；`main.qml:5` `import QWindowKit 1.0`。KDDW `examples/qtquick/dockwidgets/CMakeLists.txt` 同构（add_executable + qrc + 链库，无 qml module）。**示例侧不用 `qt_add_qml_module`**（那是库侧机制），qrc 方式两版 Qt 通吃。

**操作**：`examples/qml/QmlMainWindowExample/`：
1. CMake：`add_executable(QmlMainWindowExample main.cpp qml.qrc [main.qml ...])`；`target_link_libraries(... PRIVATE SARibbon::Qml Qt${QT_VERSION_MAJOR}::QuickControls2)`；Windows 下 `WIN32_EXECUTABLE TRUE`（照 `example/MainWindowExample/CMakeLists.txt` 现例）；`SARIBBON_BUILD_QML=ON` 时才 add_subdirectory。
2. `main.cpp`：`QGuiApplication` + `QQmlApplicationEngine`；Qt5 分支（`#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)`）在 load 前调 `saRibbonRegisterQmlTypes(&engine)`（QWK main.cpp:26-28 用法）；Qt6 声明式轨无需调用（模块随链接资源自动可导入，前提 S1 的 `/qt/qml` 前缀）。用 `ApplicationWindow` 时设 `QT_QUICK_CONTROLS_STYLE`（QWK main.cpp:11-16 按版本设 Basic/Default）。
3. QML：`ApplicationWindow + RibbonBar/Category/Panel/ToolButton` 声明式搭建，对应 widgets 版 `MainWindowExample` 的主场景（几个 category、大中小按钮混合、主题切换按钮、3 行/最小模式切换）。

**验证**：运行并与 widgets 版**同屏对比**：同字体下条高、间距、装箱一致（v2 M3 判据）；截图存 `docs/3.0/screenshots/`（png 不入 mkdocs nav 无告警；`SARIBBON_BUILD_STATIC_LIBS=ON` 组合补跑一次，验证 S1.2 的静态插件路径）。

**提交**：`示例：新增 examples/qml/QmlMainWindowExample`

### S7 跨前端一致性套件（§7.2）

**框架选型（对照 KDDW 已核）**：两侧都用 **QtTest C++**，QML 侧配 QQmlEngine/QQmlApplicationEngine，**不用 QtQuickTest**——KDDW `tests/qtquick/tst_qtquick.cpp` 是纯 C++ QTest 类（include `<QtTest/QTest>` + `QQmlApplicationEngine/QQmlContext`，:28-30），仓库内无任何 .qml 测试文件；CMake 两个前端各一个可执行、共享源码编入两侧（`tests/CMakeLists.txt:28-29` `TESTING_SRCS utils.cpp`，:55-63 tst_qtwidgets、:66-75 tst_qtquick，均链 `Qt::Test`）。测试环境：QGuiApplication 即可（KDDW `src/qtquick/TestHelpers.cpp:53-58`），CI 用 offscreen QPA（同文件 :54 `maybeSetOffscreenQPA`，即 `QT_QPA_PLATFORM=offscreen`）。

**QML 侧几何断言的关键坑**：`updatePolish()` 只对挂进 QQuickWindow 并进入 polish 阶段的 item 执行（S3 要点）——裸堆 item 断言几何必失败。测试统一模式：`QQuickView`（offscreen 下合法）`setSource`/`setContent` + `show()` + `QTest::qWaitForWindowExposed(&view)` + `QTRY_COMPARE(item->property..., 黄金值)`；或（更快的白盒路径）直接调用宿主的内部布局入口函数后同步断言。二选一在 `tests/qml/` 内统一，写进测试 README 注释。

**操作**：
1. `tests/common/RibbonConformance.h`：场景数据结构（**纯头、零依赖**，两侧各自 include 编译——对应 KDDW 共享 utils.cpp 的做法）：场景 = 一组 (sizeHint, rowProportion) + metrics 输入 + 期望（黄金值引用 `tests/core/layout_fixtures.h`）。
2. widgets 侧：`tests/widgets/tst_conformance_widgets.cpp` 构造真实 `SARibbonPanel` 场景，断言 `QWidget::geometry()` == 黄金。
3. qml 侧：`tests/qml/tst_conformance_qml.cpp` 构造同场景 QML 树（内联组件串或 test 资源 qrc 中的 .qml），断言 `QQuickItem` 的 `x/y/width/height` == 黄金。
4. 主题切换断言：双端各注册监听 `SARibbonThemeData`，切换后各自重渲染标记变化。
5. CMake 接线：`tests/CMakeLists.txt` 增加 `if(SARIBBON_BUILD_QML) add_subdirectory(qml) endif()`；`tests/qml/CMakeLists.txt` 仿 `tests/widgets/` 的 `add_saribbon_test()` 模式建 tst_* 可执行 + `add_test`（链 `SARibbon::Qml`、`Qt::Test`、`Qt::Quick`；需要 QApplication 的混合场景才链 Widgets——一致性套件 qml 侧**不应**链 Widgets，主题联动断言经 core 信号完成）。
6. CI：`SARIBBON_BUILD_QML=ON` 的 job 跑 `tests/qml`（linux-qt6.8 增加 QML=ON matrix 项，win 本地为主验证；offscreen QPA 环境变量在 workflow step 设置）。

**验证**：双前端一致性测试全绿。

**提交**：`测试：跨前端一致性套件（tests/common 场景 + 双前端断言）`

### S8 QML 纯净与依赖矩阵 CI

**操作**：
1. `tools/check_core_purity.py` 扩展（脚本本体由计划 01 S9 新建，其 CLI 为**路径参数**形式 `python tools/check_core_purity.py <目录> [--forbid-include ...]`，01 规格中无 `--module` 参数）。两种落地任选其一（记 NOTES）：
   - **a（推荐，零改脚本）**：直接复用路径形式，为 qml 指定专属禁用清单：
     `python tools/check_core_purity.py src/qml --forbid-include QWidget QApplication QLayout QStyle QMainWindow QDialog QPushButton QToolBar SARibbonWidgets`（`SARibbonWidgets` 作为 include 前缀子串匹配即可拦截 `<SARibbonWidgets/...>`）；
   - **b**：给脚本新增 `--module qml` 开关（内置 qml 清单，等价于 a 的清单固化）。
   - ⚠️ **qml 禁用清单 ≠ core 禁用清单**：core 清单（01 S9）禁 `QQuickItem/QQmlEngine/QQuickPaintedItem`，qml 模块**必须放行**这三者与 `QGuiApplication`；qml 禁的是 QtWidgets 模块类与 `SARibbonWidgets` 头。词法级"类型名出现"扫描同理需要 qml 白名单。`QAction` 在 qml 模块不禁止（Qt6 属 QtGui），但按 S5.2 决策 P0 不使用。
2. CI：新增（或并入现有）job：`SARIBBON_BUILD_QML=ON SARIBBON_BUILD_WIDGETS=OFF` 组合构建——验证 qml 不依赖 widgets（组合矩阵要求即 v2 §6.2 的两个矩阵项；原稿此处引"v1 §7.4"，**v1 计划文件从未入库、引用悬空**，内容已按 v2 §6.2 内联）。
3. v2 §6.2 组合矩阵终态复核：`Widgets=OFF Qml=ON`、`Widgets=OFF Qml=OFF` 两项齐备。

**提交**：`CI：QML 模块纯净扫描与组合构建矩阵`

### S9 文档（M4）

**仓库现实（评审已核）**：`mkdocs.yml` 在（material 主题 + i18n 插件，`docs_structure: folder`，docs/en 默认语言 + docs/zh，**nav 为两种语言的显式列表**）；`docs/zh/` 现有 `build-guide/ dev-guide/ python-guide/ use-guide/ index.md faq.md doc-writing-guide.md`，**无 migration 目录**；API 文档 = Doxygen（`page.yml:27-34` 跑 `docs/doxygen-doc-file/Doxyfile-wiki-cn`，其 `INPUT = ../../src/SARibbonBar`（:952）、`RECURSIVE = NO`（:1049））+ mkdocs nav 外链 doxygen html；站点部署由 `page.yml` 在 push master 触发（:3-5），`mkdocs build --clean` 无 `--strict`（:50）——不在 nav 的 .md 只告警不失败，但验收要求"链接有效"，故新页必须进 nav。

**操作**：
1. `docs/zh/build-guide/`、`docs/zh/dev-guide/`（及 docs/en 对应页）：三模块架构总览、core 引擎与契约、QML 模块（类型清单/注册/主题桥）、贡献者指南更新（目录、规范、`dev-3.0` 流程）。
2. **迁移指南** `docs/zh/migration-3.0.md` + `docs/en/migration-3.0.md`（docs/zh 根下，无 migration 目录即新建单页；**两语言 nav 都要加条目**，建议新增"迁移指南"分组或挂 dev-guide 下）：
   - include 路径 `<SARibbonBar/...>` → `<SARibbonWidgets/...>`（转发头过渡一个周期）；
   - CMake target `SARibbonBar` → `SARibbon::Widgets`（别名过渡）；
   - 最低要求：CMake 3.16、Qt 5.15、C++17（计划 01 S4）；
   - `SARibbonPannel` 拼写：**已于 2.9.x（f553de7）改为 `SARibbonPanel`，3.0 无需别名**（v2 P7 的现实修正，照 NOTES.md B1 表述）；
   - **度量对照表**（计划 02 S3 产物 `docs/3.0/metrics-comparison.md` 链接；该 .md 在 mkdocs 根下未入 nav 会有构建告警——要么加 nav、要么挪 `docs/zh/` 内并入 nav，执行时定并记 NOTES）；
   - 单文件发行的使用方式变化（生成脚本）；
   - Python 绑定包名不变、版本 3.0。
3. QML API 文档：**不用 QDoc**（仓库无 QDoc 设施；现 API 文档链路是 Doxygen）——① `Doxyfile-wiki-cn`（及 Doxyfile-qch-cn）的 `INPUT` 追加 `../../src/qml`（注意 INPUT 还停留在 `../../src/SARibbonBar`，计划 01 搬移后本就要改为 `../../src/widgets`，两处一起改）；② mkdocs 增手写"QML 类型清单"页（P0 类型 + 属性/信号表 + import 用法），进双语言 nav。
4. `readme.md` / `readme-cn.md`（小写实名，根目录）：加 3.0 模块图与 QML 一节；**同步改 badge**：`readme.md:8` 现为 `Qt-5.14+`，3.0 最低 Qt 为 5.15（v2 D1）。

**提交**：`文档：3.0 架构/QML/迁移指南全套`

### S10 发布 3.0.0

**仓库现实（评审已核）**：版本号现值——根 `CMakeLists.txt:7-13` 为 2.9.5（三变量 + `project(VERSION ...)`，计划 01 S4 已改为 `project(SARibbon VERSION 3.0.0)` 单一来源）；`pyproject.toml:9`、`pyproject-pyqt6.toml:9`、`pyside6/pyproject.toml:11` 现值均 `2.8.0`（落后于库版本，计划 03 S3.2/S3.4 已升 3.0.0，本步核对即可）；`changlog.md`（实名拼写无 e）条目格式 `## YYYY-MM-DD -> X.Y.Z`，计划 03 S7 已落"3.0.0（未发布）"草稿段；现有 tag 风格 `v2.5.7`…`v2.9.5`（`git tag -l`）→ `v3.0.0` 一致；`.github/workflows/` 只有 6 个 cmake-* + page.yml + publish-python-bindings.yml，**无 release 自动化 workflow**。

**操作**：
1. 全量回归：6 workflow 绿 + tests/core + tests/widgets + tests/qml + amalgamation + python dry-run。
2. `changlog.md` 3.0.0 定稿（草稿段补日期、迁移要点与面向用户条目）；版本号核对四处：根 CMake `project(VERSION 3.0.0)`、pyproject×3（3.0.0）、QML module `VERSION 3.0`（S1）、readme badge（S9.4）。
3. PR：`dev-3.0` → `master`（标题 `3.0.0`），人工 review 后合并；**不合并入 `dev`**（2.x 线）。合并即触发 page.yml 重建文档站（push master，page.yml:3-5）——S9 文档必须随本 PR 落地。
4. **发布触发链核对（评审修正）**：`publish-python-bindings.yml` 现库（2.9.5）触发器是 `release: types:[published]` + `workflow_dispatch`（:3-6）——**并非 tag 触发**；计划 01 S10.4 暂停为仅 workflow_dispatch，计划 03 S4 恢复为 **tag `v3.0.*`** 触发。本步前提是 03 S4 已生效（P1 门保证）：打 tag `v3.0.0` 并 push → publish workflow 自动跑（真实上传需维护者密钥——agent 只验证 tag 事件触发的 workflow 启动与构建段，upload 段由维护者确认）。执行前 `grep -A3 "^on:" .github/workflows/publish-python-bindings.yml` 复核触发器实际状态，防止 03 未按预期落地时 tag 静默不触发。
5. GitHub Release：无自动化 workflow，由维护者经 web 或 `gh release create v3.0.0 <附件>` 创建；agent 准备附件与文案——amalgamation 产物（`SARibbonCore.h/.cpp`、`SARibbonWidgets.h/.cpp` 打包 zip）、变更摘要、迁移指南链接。**顺序注意**：若触发器仍是旧版 `release:published`（第 4 步复核发现 03 S4 未生效），则"创建 Release"本身会触发 publish——此时先与维护者确认再建 Release，避免意外发 PyPI。
6. 2.x 转 `support/2.x` 分支只修严重 bug（原稿引"v1 §7.5"，**v1 文件从未入库、引用悬空**；按 v2 R7 双分支策略内联：2.x bugfix 落 `dev`/`support/2.x`，布局引擎相关 fix 须手动同步 core 并跑黄金测试）。

**提交**：`发布：SARibbon 3.0.0`（changelog 与版本定稿）

## 6. 完成验收门（= v2 M3 + M4 交付判据）

- [ ] `examples/qml/QmlMainWindowExample` 运行，与 widgets 版同屏对比**同字体下条高/间距/装箱一致**（截图存档）
- [ ] 跨前端一致性套件全绿（`tests/common` 场景在 widgets 与 qml 双端断言通过）
- [ ] `SARIBBON_BUILD_QML=ON` CI job 绿；`Widgets=OFF Qml=ON` 组合构建绿
- [ ] qml 模块纯净扫描绿（S8 清单，不含 widgets 头）
- [ ] `src/qml/` 无任何布局算法副本（铁律审查，**可操作判据**——原稿的 `rowIndex|columnIndex` grep 会误报：二者是契约接口的共享数据字段（计划 02 S4.1），QML 侧适配器实现契约必然出现这些名字）：
  1. `git grep -nE "updateGeomArray|recalcExpandGeomArray|updateGeometryArr|calcMinTabBarWidth" src/qml/` **为空**（算法函数名，源出 `SARibbonPanelLayout.h:61/153/155`、`SARibbonCategoryLayout.h:74`、`SARibbonBarLayout.h:66`，不得在 qml 出现）；
  2. `git grep -nE "SARibbonPanelLayout|SARibbonCategoryLayout|SARibbonBarLayout" src/qml/` **为空**（widgets 布局类名不得出现）；
  3. `git grep -n "setPosition\|setSize\|setX(\|setY(\|setWidth\|setHeight" src/qml/` 列出全部几何应用点，**人工复核**：右值必须是引擎输出（契约 item 的 `geometry()`、metrics 字段）或常量边距，不得含行/列索引运算、比例分摊等局部几何计算表达式；
  4. include 级规则由 S8 纯净扫描兜底。
- [ ] 迁移指南、三模块文档、QML 文档、度量对照表齐备且链接有效（含 mkdocs nav 条目）
- [ ] tag `v3.0.0` 创建，GitHub Release 发布，Python publish workflow 正常触发（触发器状态按 S10.4 复核）
- [ ] ctest 全绿（widgets N₀ 无回归 + core + qml）
- [ ] NOTES.md 汇总本计划全部决策与截图

## 7. 风险与缓解

| 风险 | 缓解 |
|------|------|
| QML 宿主无 QLayout 惰性机制导致每帧重算（v2 R4） | 引擎自带 sizeHint 缓存与 dirty（计划 02 已随算法进 core）；仅 `updatePolish()` 触发布局（场景图合帧，每帧至多一次）；示例做 resize 压测（窗口连续拉伸观察 CPU） |
| 视觉叶子与 widgets 版风格漂移 | 渲染允许双实现，但颜色/尺寸必须绑定 RibbonTheme/RibbonMetrics；同屏对比截图入验收 |
| QML 类型注册 Qt5/Qt6 差异踩坑（**含 Qt6 声明式轨特有坑**：plugin 加载路径、RESOURCE_PREFIX 缺省 `/` 不在导入路径、静态库 plugin_init、QML_ELEMENT 宏 Qt5 不存在） | 双轨注册函数集中一处；`RESOURCE_PREFIX /qt/qml` + S1 导入路径专项验证；`QML_*` 宏一律 `QT_VERSION` 包裹；**兜底：两版 Qt 退回单轨命令式（KDDW/QWK 实证方案，Qt6 命令式 API 齐全）**；CI 两版本都跑（Qt5.15 走 CI） |
| QML 单例所有权（引擎析构连带 delete 包装对象，多引擎测试悬空） | 注册前统一 `QQmlEngine::setObjectOwnership(CppOwnership)`（S1.3/S2.1）；tst_themeBridge 双引擎重复断言兜底 |
| 结构宿主需要 widgets 语义但 core 没有对应引擎（铁律触发） | 停下，回计划 02 补引擎（记 NOTES），禁止 QML 内重写 |
| 发布流程需要维护者权限（tag/PyPI/Release 附件） | agent 完成到"可触发"状态并列 checklist 交维护者确认，不阻塞其余验收；触发器状态在 S10.4/S10.5 双重复核，防止意外发 PyPI |

## 8. 已知偏差

见 [NOTES.md](NOTES.md)。
