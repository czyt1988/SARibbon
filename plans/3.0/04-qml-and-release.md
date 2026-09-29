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

### S1 模块骨架与注册（命令式单轨，§5.3）

**路线决策（第2轮评审修订，**整合者已复核采纳**：v2 §5.3 已同步修订为命令式单轨，裁决记录见 reviews/round2/synthesis-findings.md 与 NOTES.md B9；实证详见 reviews/round2/kddw-qtquick-findings.md §三）**：3.0 采用**命令式单轨（Qt5/Qt6 同码）**为唯一实现路线；`qt_add_qml_module`（Qt6 声明式轨）**降为可选优化（3.1+ 再评估）**，3.0 不实现。原第1轮的"双轨+兜底"设计中兜底路线转正。证据（第2轮逐文件核实，副本版本：**KDDW 2.0.1**（`CMakeLists.txt:81-84`）、**QWK 1.0.1**（`CMakeLists.txt:3`））：

- KDDW 2.0.1 在 **Qt 6.2+**（`QT_MIN_VERSION "6.2.0"`，KDDW `CMakeLists.txt:146`，Qt5 侧 :149 `"5.15"`）下也**只用命令式注册**：`src/qtquick/QmlTypes.cpp:21-30` 全文 = 4 个 `qmlRegisterType`（:23-26，全部是 Instantiator 族，见 S3 附注）+ 1 个 `qmlRegisterUncreatableMetaObject`（:28）；全仓 grep 无 `qt_add_qml_module`、无任何 qmldir 文件（第2轮复核）。由库内 `Platform::init()` 自动调用（`src/qtquick/Platform.cpp:80-92`，:86 `registerQmlTypes()`），触发点是应用显式 `initFrontend(FrontendType::QtQuick)`（`src/KDDockWidgets.cpp:29`；示例 `examples/qtquick/dockwidgets/main.cpp:36`）。
- QWK 1.0.1 同：`src/quick/qwkquickglobal.cpp:14-25` `registerTypes(QQmlEngine*)` = `static bool once` 防重入（:17-21）+ `qmlRegisterType<QuickWindowAgent>("QWindowKit", 1, 0, "WindowAgent")`（:23）+ `qmlRegisterModule("QWindowKit", 1, 0)`（:24，保证带版本 URI 的 `import` 在无 qmldir 下成立；QWK `README.md:246` 明确 Qt6 下该 import 方式不变）。`src/quick/` 整个模块 **7 个源文件、零 QT_VERSION 分支**（grep 无命中）——单码路同时编 Qt5.12+ 与 Qt6。库 CMake 就是普通库（`src/quick/CMakeLists.txt`：`qwk_add_library(... QT_LINKS Core Gui Quick)`）。
- **命令式 API 在两版 Qt 同签名齐备（本机头文件第2轮复核）**：`qmlRegisterType`（6.7.3 `include/QtQml/qqml.h:300/336`；5.14.2 :291/:322）、`qmlRegisterSingletonInstance`（6.7.3 :727；5.14.2 :681，Qt 5.14+ 可用）、`qmlRegisterSingletonType` 回调式 `QObject* (*)(QQmlEngine*, QJSEngine*)`（6.7.3 :674-706；5.14.2 :645，`QJSValue` 变体 :629）、`qmlRegisterModule`（6.7.3 :645；5.14.2 :616）、`qmlRegisterUncreatableMetaObject`（6.7.3 :297；5.14.2 :288）、`qmlRegisterUncreatableType`（6.7.3 :148；5.14.2 :147）。
- 单轨化收益：整类消除声明式轨风险（plugin 加载、RESOURCE_PREFIX、静态 plugin_init、`QML_*` 宏 Qt5 不存在——原风险表第3行）；`SARIBBON_BUILD_STATIC_LIBS=ON` 组合不再需要 S6 单独验证插件路径（改为验证 Q_INIT_RESOURCE，见操作3）；应用侧代价仅 `engine.load` 前一行 `saRibbonRegisterQmlTypes()`（QWK 示例同款：`examples/qml/main.cpp:25-27`）。损失：qmltypes IDE 补全、qmllint/qmlcachegen AOT——均非 3.0 验收项，3.1+ 若需工具链收益再按本节"附注"启用声明式轨。
- v2 §5.3 原文为"保留 v1 双轨"——**round2 整合已按本变更完成 v2 §5.3 修订**（默认命令式单轨，声明式轨降为 3.1+ 候选，见 v2 §5.3 与 §9"3.1+ 候选项清单"B-3；NOTES.md B9）；URI（`SARibbon`）与版本（3.0）不变，§5.3 的对外承诺（`import SARibbon 3.0`）不受损。

**操作**：
1. `src/qml/CMakeLists.txt`：`find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED COMPONENTS Core Gui Quick Qml QuickControls2)`；`sa_add_library(SARibbonQml SOURCES <显式清单，禁止 GLOB，含 qml/saribbon_qml.qrc> PREFIX SA_RIBBON_QML QT_LINKS Core Gui Quick Qml LINKS SARibbonCore)`（函数签名见计划 01 S5；别名 `SARibbon::Qml` 由其自动生成）+ `sa_sync_include(SARibbonQml SARibbonQml)`。**qrc 直接列入库 SOURCES 由 AUTORCC 编进二进制**（KDDW 同款：`src/CMakeLists.txt:132-158` 源清单末行 :158 即 `qtquick/kddockwidgets_qtquick.qrc`）；QuickControls2 若仅示例用则链在示例侧（KDDW 库侧 PUBLIC 链接是因为其叶子直接用 Controls 的 TabBar/TabButton，`src/CMakeLists.txt:495-496` + `views/qml/TabBar.qml:12-13,82-107`；SARibbon 叶子用不用 Controls 在 S5 定，用了才进库链接）。
2. QML 叶子文件走 `src/qml/qml/saribbon_qml.qrc`（前缀 `/SARibbon/`；KDDW 同款：`kddockwidgets_qtquick.qrc:2` 前缀 `/kddockwidgets/qtquick/`，与 `ViewFactory.cpp:174-202` 的叶子 URL 表一一对应），C++ 宿主经 `QQmlComponent` 从 `qrc:/SARibbon/RibbonPanel.qml` 创建叶子（S3 骨架）。**无 qmldir/plugin/RESOURCE_PREFIX 事项**。
3. 导出函数 `saRibbonRegisterQmlTypes(QQmlEngine* = nullptr)`（声明进 `SARibbonQmlGlobal.h`，`SA_RIBBON_QML_EXPORT` 导出；实现集中在一个 `SARibbonQmlTypes.cpp`，仿 QWK `qwkquickglobal.cpp:14-25`）：

   ```cpp
   void saRibbonRegisterQmlTypes(QQmlEngine* engine)
   {
       Q_UNUSED(engine);                    // 注册进 QQmlMetaType 全局注册表，engine 参数仅为签名兼容（QWK 同款 Q_UNUSED，qwkquickglobal.cpp:15-16）
       static bool once = false;              // QWK qwkquickglobal.cpp:17-21
       if (once) return;
       once = true;
   #if defined(SA_RIBBON_QML_STATIC) || defined(QT_STATIC)   // 宏名按计划01 S5 静态导出宏方案定
       Q_INIT_RESOURCE(saribbon_qml);         // ⚠️必做：静态库的 qrc 不会自动加载（KDDW Platform.cpp:40-46 同款守卫，:82-84 在 init() 调用；Q_INIT_RESOURCE 必须在全局命名空间调用）
   #endif
       qmlRegisterType<SARibbonQml::RibbonBar>("SARibbon", 3, 0, "RibbonBar");
       // … RibbonCategory / RibbonTab / RibbonPanel / RibbonToolButton（S3-S5 逐个补）
       qmlRegisterUncreatableType<SARibbonQml::RibbonEnums>("SARibbon", 3, 0, "Ribbon", "Enum access only"); // S3.4
       // 单例（S2）：先 QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership) 再注册
       qmlRegisterSingletonInstance("SARibbon", 3, 0, "RibbonTheme", RibbonTheme::instance());
       qmlRegisterModule("SARibbon", 3, 0);   // QWK qwkquickglobal.cpp:24：无 qmldir 时保证 import 模块解析
   }
   ```

   - 普通类型：`qmlRegisterType<T>("SARibbon", 3, 0, "RibbonPanel")`；不可实例化枚举持有类：`qmlRegisterUncreatableType<...>`（KDDW QmlTypes.cpp:28 的 metaObject 变体要求枚举在 `Q_NAMESPACE` 内——KDDW 在 `namespace KDDockWidgets` 上挂 `Q_NAMESPACE`（`src/KDDockWidgets.h:51-53`）+ 枚举 `Q_ENUM_NS`（:65-71）；SARibbon 枚举现状不满足且计划 02:67 已决策**维持全局命名空间**，见 S3.4）。
   - **所有权坑（必做）**：`qmlRegisterSingletonInstance`/回调式注册的单例对象默认归**引擎**所有（引擎析构即 delete），而 RibbonTheme/RibbonMetrics 包装的是 core 进程级单例——注册前 `QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership)`，否则多引擎（测试逐个建 QQmlEngine）第二次即悬空。
   - **调用时机模型 = QWK 显式调用**：应用在 `engine.load()` 前调一次（QWK `examples/qml/main.cpp:25-27`）。KDDW 的"库内自动调用"（Platform.cpp:86）依赖其 frontend-init 单例步骤，SARibbon 无此步骤，不模仿；static-once 保证多次调用无害。写入迁移指南与示例模板（S6/S9）。
4. `src/qml/SARibbonQmlGlobal.h` 完善导出宏——**QWK `qwkquickglobal.h:9-19` 的三态宏（`QWK_QUICK_STATIC` 空 / `QWK_QUICK_LIBRARY` 导出 / 否则导入）可逐字照抄**为 `SA_RIBBON_QML_STATIC`/`SA_RIBBON_QML_LIBRARY`/`SA_RIBBON_QML_EXPORT`（与计划 01 S5 的 PREFIX 方案衔接）；私有实现头用 `_p.h`。

**附注：声明式轨预案（3.1+ 启用时照此执行；硬约束为第1轮对 Qt 6.7.3 `Qt6QmlMacros.cmake` 的核实结论，第2轮未重复核）**：`qt_add_qml_module(SARibbonQml URI SARibbon VERSION 3.0 RESOURCE_PREFIX /qt/qml QML_FILES ...)`——①必须与 target 创建同目录作用域（macros:196-209）；②backing target 静/动态决定模块类型，静态组合有 `SARibbonQmlplugin`/plugin_init/`qt_import_qml_plugins()` 链（macros:234-241/287/664-679/3522）；③RESOURCE_PREFIX 必须显式 `/qt/qml`（缺省 `/` 不在引擎内建导入路径，macros:466；引擎无条件 addImportPath("qrc:/qt/qml")，qtdeclarative `qqmltypeloader.cpp:1213,1239`）；④`QML_ELEMENT/QML_SINGLETON/QML_UNCREATABLE` 宏 Qt6 独有需 `QT_VERSION` 包裹（qqmlintegration.h:45/86/63），`QML_SINGLETON` 类提供 `static T* create(QQmlEngine*, QJSEngine*)` 转发 `instance()`；⑤`NO_PLUGIN` 系选项（macros:23-25）共享库默认路径不要用。届时 `saRibbonRegisterQmlTypes` 保留为空壳兼容（应用代码不分叉）。

**验证**：`cmake -B build-3.0-qml -DSARIBBON_BUILD_QML=ON ...` 配置+编译通过（此时仅有占位类型）；`SARIBBON_BUILD_QML=OFF` 默认路径不受影响；**Qt6 导入专项检查**：最小 main——`saRibbonRegisterQmlTypes()` 后以 QQmlEngine 创建内联 `import SARibbon 3.0` 组件串（不设 `QML_IMPORT_PATH`、无任何 qmldir），成功创建占位类型 = 命令式注册在 Qt6 成立的直接证据（QWK README:246 的本地复验）；**`SARIBBON_BUILD_STATIC_LIBS=ON` 组合重跑同一检查** = Q_INIT_RESOURCE 静态资源路径生效的证据（叶子 `QFile::exists` 可过，参照 KDDW `View.cpp:857-860` 的存在性检查）。

**提交**：`feat：SARibbonQml 模块骨架与命令式注册`

### S2 主题与度量桥（P0：RibbonTheme / RibbonMetrics）

**操作**：
1. `src/qml/theme/RibbonTheme.h/.cpp`（C++ QObject 单例）：包装 core `SARibbonThemeData::instance()`（计划 02 S2.1/S2.3 产物：QObject 单例，信号 `themeChanged(SARibbonTheme)`、`paletteChanged()`）——palette 颜色暴露为 QML 属性（`NOTIFY` 转发上述两信号）、`currentTheme` 可读写。**不实现任何颜色计算**（core 已有）。注册：单轨 `qmlRegisterSingletonInstance`（S1.3，Qt5.14+/Qt6 同签名：5.14.2 qqml.h:681、6.7.3 :727），**注册前设 CppOwnership**（S1.3 所有权坑）。（原"Qt6 轨 QML_SINGLETON + static create()"随声明式轨移入 S1 附注；`static create()` 转发函数可以顺手保留——3.1+ 启用声明式轨时是必需件，现在无害。）
2. `src/qml/metrics/RibbonMetrics.h/.cpp`：桥 `SARibbonMetrics`（core 构造签名 = `QFontMetrics` + `devicePixelRatio`，计划 02 S3.1）。**度量全部在 C++ 内构造，QML 侧不参与**：
   - 默认 `QFontMetrics(QGuiApplication::font())`；暴露 `font` Q_PROPERTY(QFont) 允许覆盖（对应 widgets 侧"以 bar 的 fontMetrics 为准"的语义，现状度量公式即取 `ribbonBar->fontMetrics()`，`SARibbonBarLayout.cpp:381/407/432`）；
   - 在 `qApp` 上装事件过滤器监听 `QEvent::ApplicationFontChange`（与计划 02 S3.2 widgets 侧同一事件源），变化时重建 metrics 并 `Q_EMIT metricsChanged`；
   - `devicePixelRatio` 取 `QGuiApplication::devicePixelRatio()`；RibbonBar 宿主可用所在窗口的 `QQuickWindow::effectiveDevicePixelRatio()`（public，6.7.3 `qquickwindow.h:141`）覆盖；
   - 暴露 `tabBarHeight/categoryHeight/panelTitleHeight/...` 为只读属性。
   - 形态：P0 只做**单例**（v2 §5.2 "singleton 或 attached"二选一时取 singleton）；attached 形态需要多 RibbonBar 各带不同字体时才值得做（`QML_ATTACHED`/`qmlRegisterAttachedType`），推迟。
   - ⚠️ 原稿"从 qml fontMetrics 构造"不成立：QML 未暴露 `QFontMetrics` 类型；内建 `TextMetrics`（QtQuick）只有 advanceWidth/boundingRect/elidedText，**没有** `lineSpacing()`/`height()`——而 SARibbon 度量公式恰依赖这两者（上行号）。已按上述 C++ 方案改写。
3. `src/qml/qml/` 下视觉叶子起步（如叶子需要主题色，绑定 RibbonTheme 单例属性）。

**验证**：QML 单测 `tests/qml/tst_themeBridge.cpp`：C++ 改 `SARibbonThemeData::instance()` 的主题 setter，QML 侧断言属性变化信号触发——**双端同一信号源**（v2 §3.2-1 的验收）。测试经与应用相同的注册入口建 QQmlEngine（统一：先调 `saRibbonRegisterQmlTypes()`，S1.3 static-once 保证多引擎重复调用无害）；连续建两个引擎重复断言，顺带验证 CppOwnership 无悬空。

**提交**：`feat：QML 主题与度量桥接单例`

### S3 结构宿主：RibbonPanel（先 Panel，直接检验引擎复用）

**宿主实现要点（API 均已对照本机 Qt 头文件核实，Qt6.7.3 `include/QtQuick/qquickitem.h` / Qt5.14.2 同行号语义一致）**：
- 几何应用 API 真实存在且为 **public**：`setPosition(const QPointF&)`（6.7.3:207；5.14.2:229）、`setSize(const QSizeF&)`（6.7.3:226；5.14.2:244）、`setWidth/setHeight/setX/setY`。KDDW 宿主同款用法：`View::setGeometry(QRect)` = `setSize(rect.width(), rect.height()) + move(rect.topLeft())`（KDDW `src/qtquick/views/View.cpp:157-161`）。
- `setImplicitSize(qreal, qreal)` 是 **protected**（6.7.3:433；5.14.2:411，protected 段自 :420 起）——宿主自身是 QQuickItem 子类，类内调用合法；类外只能走 public 的 `setImplicitWidth/setImplicitHeight`。
- 布局入口 `updatePolish()`：protected virtual（6.7.3:468；5.14.2:450），由 `polish()`（6.7.3:322）请求、场景图每帧 polish 阶段对**挂在 QQuickWindow 上的 item** 调用——裸堆上 item 不会触发（测试注意事项见 S7）。KDDW 未用 updatePolish（其几何由 controller 驱动），SARibbon 无 controller 层（D7），updatePolish 是正确的挂点：每帧至多一次、天然合帧，满足 v2 R4 的性能要求。
- **子项收集不用 `childItems()` 顺序做布局序**：`childItems()` 是 public（6.7.3:189）但返回的是堆叠序（z 值会扰动），且混入非按钮装饰子项。方案：`RibbonToolButton` 在自己的 `componentComplete()`（QQuickItem protected virtual，6.7.3:435-436 `classBegin/componentComplete`）向 `qobject_cast` 出的父宿主登记，宿主自维护显式列表；宿主 override `itemChange()`（6.7.3:424）监听 `ItemChildRemovedChange/ItemVisibleChange` 同步列表并 `polish()`。KDDW 对逻辑顺序同样用显式模型（`DockWidgetModel : QAbstractListModel`，`TabBar.h:114-151`）而非 childItems；其"声明完成才真正建对象"的时机处理见 `DockWidgetInstantiator.h:28-33` 注释（QQmlParserStatus，属性全部就位后再构造实体）。
- 声明顺序 = componentComplete 触发顺序（同一父下按声明序），显式列表按登记序即得用户期望的布局序；运行期 `moveItem(from,to)` 之类的重排 API P0 不做（ customization 属 Tier 2）。

**RibbonPanel C++ 宿主类骨架（第2轮评审补，逐处对照 KDDW 2.0.1 实证；KDDW 副本路径 `src/qtquick/`）**：

```cpp
// src/qml/panel/RibbonPanel.h —— class 声明级别骨架
namespace SARibbonQml {

class RibbonPanel : public QQuickItem   // 直接继承 QQuickItem：KDDW 宿主基类同款（View.h:52 `class View : public QQuickItem, public QtCommon::View_qt`）。
{                                       // KDDW 第二基类是 controller 架构的 view 接口适配层；SARibbon 无 controller（D7），
    Q_OBJECT                            // 契约适配用内嵌类/成员承担（见下 PrivateData），不搞第二继承链。
    // —— Q_PROPERTY 面（对照 KDDW TabBar.h:47-51 / Group.h:40-46 / TitleBar.h:33-48 的规模：每宿主 4-6 个）——
    Q_PROPERTY(QString panelTitle READ panelTitle WRITE setPanelTitle NOTIFY panelTitleChanged)   // KDDW TitleBar.h:36 title READ NOTIFY 同款
    Q_PROPERTY(RibbonLayoutMode layoutMode READ layoutMode WRITE setLayoutMode NOTIFY layoutModeChanged) // 枚举镜像类见操作4
    Q_PROPERTY(QQuickItem* panelQmlItem READ panelQmlItem WRITE setPanelQmlItem NOTIFY panelQmlItemChanged)
    // ↑ 握手属性（KDDW TabBar.h:47-48 tabBarQmlItem READ WRITE NOTIFY）：叶子 QML 在完成绑定后把自己赋回来
    //   （TabBarBase.qml:65-73 onTabBarCppChanged 内 `tabBarCpp.tabBarQmlItem = this`，注释明言供单测访问内部项）。
    //   SARibbon 用途：宿主需要调叶子内 JS 函数/读叶子属性时的通道 + 测试可达性。
public:
    explicit RibbonPanel(QQuickItem* parent = nullptr);
    ~RibbonPanel() override;
    // —— 子项登记 API（显式列表，不用 childItems() 做布局序，上节要点4）——
    void registerChildItem(RibbonToolButton* item);    // 由子项 componentComplete 调用（下）
    void unregisterChildItem(RibbonToolButton* item);
    // KDDW 对逻辑顺序同样用显式数据结构：DockWidgetModel : QAbstractListModel（TabBar.h:114-151），
    // insert/remove 走显式调用（TabBar.cpp:251-263 removeDockWidget/insertDockWidget → model）
protected:
    void componentComplete() override;   // QQmlParserStatus（QQuickItem 已实现该接口）：
                                         // ① 向父宿主（qobject_cast<RibbonCategory*>(parentItem()) 链）registerChildItem
                                         // ② 创建视觉叶子（见下 ensureQmlItem）
                                         // 时机实证：KDDW DockWidgetInstantiator.h:27-34 注释——"QML 解析结束、所有属性就位后"才构造实体
    void updatePolish() override;        // 布局唯一入口（上节要点3）：
                                         //   取显式列表 sizeHint → 构造 QVector<SARibbonAbstractLayoutItem*>（契约适配器）
                                         //   → SARibbonPanelLayoutEngine::layout(...)
                                         //   → 逐项 item->setPosition(QPointF) + item->setSize(QSizeF)
                                         //     （KDDW 几何应用同款：View.cpp:157-161 setGeometry = setSize + move；View.cpp:232-244 move = setX/setY）
                                         //   → setImplicitSize(w, h)（类内调 protected 合法）
    void itemChange(ItemChange change, const ItemChangeData& data) override;
                                         // ItemChildRemovedChange/ItemVisibleHasChanged → 同步显式列表 + polish()
                                         // KDDW 先例：View.cpp:202-209 用 itemChange 补 QQuickItem 不发的 Visible 事件
private:
    void ensureQmlItem();                // 叶子创建三部曲（KDDW Group.cpp:105-116 逐行同款）：
                                         //   QQmlComponent component(qmlEngine(this), leafUrl);  // 引擎获取见下注
                                         //   m_qmlItem = component.create();
                                         //   m_qmlItem->setProperty("panelCpp", QVariant::fromValue(this)); // 自注入
                                         //   m_qmlItem->setParentItem(this); m_qmlItem->setParent(this);    // 双 setParent 都要
                                         //   失败检查：component.errorString()（View.cpp:167-170）+ QFile::exists(qrc 路径)（View.cpp:857-860）
    class PrivateData;                   // PIMPL 照仓库规范（AGENTS.md）；契约适配器类放 .cpp（仿 QWK
    std::unique_ptr<PrivateData> d_ptr;  //   QuickItemDelegate：纯虚窄接口逐个映射 QQuickItem 语义，quickitemdelegate_p.h:25-48）
};
}
```

骨架补充注记（均有实证）：
- **析构**：叶子 `setParent(nullptr)` + `deleteLater()`，禁止直接 delete——KDDW `Group.cpp:63-72` 及注释（QML item 可能正处于其鼠标处理调用栈中，直接删会崩）。
- **引擎获取**：P0 宿主一律由 QML 声明创建，`componentComplete` 时 `qmlEngine(this)` 必有效；KDDW 的兜底链（沿 parentItem 向上找 engine、再 fallback 到全局 Platform::setQmlEngine 存的引擎，`View.cpp:840-855` + `Platform.cpp:159-187`）是为"C++ 纯手工建视图"场景准备的——SARibbon 该场景只出现在测试（S7 骨架里显式传 engine），库内**不设全局引擎单例**。
- **C++ 调 QML 函数/读 QML 值**（叶子有交互逻辑时才需要，P0 预计仅测试可达性用）：`QMetaObject::invokeMethod(m_qmlItem, "fn", Q_RETURN_ARG(QVariant, ret), ...)`（KDDW `TabBar.cpp:110-112/195-196` 调叶子的 `getTabIndexAtPosition/getTabAtIndex`）；`m_qmlItem->property("xxx")` 读叶子自报值（`Group.cpp:191-194` 读 `nonContentsHeight`）。**约束**：凡进入引擎计算的数值必须来自 core metrics，不得来自 QML 自报属性（铁律）；QML 自报值只允许做纯视觉常量（KDDW `TitleBarBase.qml:41` `heightWhenVisible` 模式）。
- **约束传播**（RibbonCategory/Bar 宿主会用到）：KDDW 把 min/max 尺寸存成动态属性再手动发失效信号（`Group.cpp:119-129` `setProperty("kddockwidgets_min_size", ...)` + `layoutInvalidated.emit()`；读回在 `View.cpp:449-460`）——SARibbon 对应物是宿主 `setImplicitSize/setImplicitWidth` + `polish()` 上溯，不需要动态属性 hack（KDDW 那样做是因为其 core layouting 引擎要跨前端读约束）。

**Instantiator 模式评估结论（第2轮，设计附注）**：SARibbon P0 **不需要** Instantiator 式包装类。KDDW 用它的唯一理由写在注释里：真实对象（Core::DockWidget 控制器）构造需要 `uniqueName`，而该值要到 QML 解析结束才齐——故用 `DockWidgetInstantiator : QQuickItem` 先接住声明、`componentComplete()` 时才经 ViewFactory 构造实体（`DockWidgetInstantiator.h:27-35` 注释 + `.cpp:180-238`；`MainWindowInstantiator.h:30,71-72` 同构）。SARibbon 宿主**本身就是那个 QQuickItem**，没有"构造参数必须先于对象存在"的问题，属性写进成员、`componentComplete` 时向父宿主登记即可（上骨架）。两个顺带实证：①KDDW 收集单个声明式子内容也直接用 `childItems()`（`DockWidgetInstantiator.cpp:197-202` 校验 size==1）——QQuickItem 天然子项树够用，SARibbon 的显式列表是为了**布局序稳定**（z 值/装饰项扰动，上节要点4），不是子项发现；②"属性先缓冲、实体后建"时 KDDW 用 `std::optional` 缓冲（`DockWidgetInstantiator.h:113` + `.cpp:234-235`）——若 Tier 2（D7）引入 core controller，再照此上 Instantiator 族。

**操作**：
1. `src/qml/panel/RibbonPanel.h/.cpp`：C++ `QQuickItem` 子类；`updatePolish()` 中取显式子项列表的 sizeHint → 构造 `QVector<SARibbonAbstractLayoutItem*>`（QML 侧适配器实现契约接口，或直接让 RibbonToolButton 实现契约接口）→ 调 `SARibbon::Core::SARibbonPanelLayoutEngine::layout(...)` → 按结果对每个子项 `setPosition(QPointF)` + `setSize(QSizeF)`，宿主自身 `setImplicitSize`（类内，protected 合法）。**引擎一个字都不改**（v2 §5.1）。
2. `panelTitle`/`optionButton` 作伪项传入（引擎不感知）。
3. `src/qml/qml/RibbonPanel.qml`（视觉叶子，标题条/边框/背景色绑定 RibbonTheme）+ `RibbonPanelBase.qml`(契约层)：宿主经 `QQmlComponent` 加载为自身子项（三部曲见上骨架注记；KDDW `View::createItem`，View.cpp:163-173/:840-873）。叶子只做渲染，**不写任何几何计算**；Base/视觉两层拆分与配对规则见下"QML 叶子组织规范"。
4. 输入参数（`PanelLayoutMode`、`RowProportion` 等）的 QML 枚举暴露：**不用 `QML_ENUM`——该宏在 Qt 5.14.2 / 6.7.3 / 6.10.1 公共头树中均不存在（grep `define QML_ENUM` 无命中，第1轮核实，第2轮未回退），KDDW 也未使用**。可行做法：在已注册的 QObject 子类内定义镜像枚举 + `Q_ENUM`（`qmlRegisterType` 注册路径会把 Q_ENUM 带进 QML；若 3.1+ 启用声明式轨，`QML_ELEMENT` 路径同样携带），如 `RibbonToolButton` 内 `enum RowProportion { None, Large, Medium, Small }; Q_ENUM(RowProportion)` + 同名 Q_PROPERTY。core 侧枚举现状（第2轮复核）：`RowProportion` 是 `SARibbonPanelItem`（非 QObject）的类内普通 enum（`SARibbonPanelItem.h:36`），全局的 `SARibbonAlignment/SARibbonTheme` 是全局 enum class（`SARibbonGlobal.h:197/220`，全文件无 Q_NAMESPACE/namespace 包裹）——KDDW 的 `qmlRegisterUncreatableMetaObject` 方案（前提：`namespace KDDockWidgets` 挂 `Q_NAMESPACE`（KDDW `src/KDDockWidgets.h:51-53`）+ 枚举 `Q_ENUM_NS`（:65-71），注册见 QmlTypes.cpp:28）对 SARibbon 不可直接用；且计划 02:67 已决策枚举**维持全局命名空间**（避免 3.0 用户改名），Q_NAMESPACE 路线在 3.0 被封死。故跨类型共享的枚举放一个 uncreatable 注册类（暂名 `Ribbon`，对应 v2 §5.1 示例的 `Ribbon.Large` 写法）统一挂 Q_ENUM；若未来 core 枚举命名空间化（计划 02:77 预留了 Q_NAMESPACE+Q_ENUM_NS 选项），可切回 KDDW 的 metaObject 注册方式（记 NOTES 观察项）。镜像枚举与 core 枚举间的转换函数集中在一个 `_p` 头，static_assert 值一致。

**QML 叶子组织规范（S3–S5 共用，第2轮评审补，KDDW `src/qtquick/views/qml/` 15 个叶子实证）**：
- **两层拆分**：`XxxBase.qml`（契约层）+ `Xxx.qml`（默认视觉层，继承 Base）。Base 声明与 C++ 宿主的全部交互面；视觉层只管外观。实证：KDDW `TitleBarBase.qml:14-23` 头注释（"要自定义外观请直接派生 TitleBarBase.qml 而非 TitleBar.qml"）+ `TitleBar.qml:17-20` 派生关系；`TabBarBase.qml:75-88` 在 Base 里放"必须由派生叶子实现"的抽象 JS 函数（`console.warn` 占位），`TabBar.qml:31-73` 实现之。SARibbon P0 至少 RibbonPanel/RibbonToolButton 走两层（自定义需求最高），其余类型可先单文件、留 Base 化重构余地。
- **宿主配对 = 父链属性查找 + C++ 注入，不用 required property、不用 context property**：宿主创建叶子后 `setProperty("panelCpp", QVariant::fromValue(this))`（Group.cpp:114）；Base 叶子首行 `readonly property QtObject panelCpp: parent.panelCpp`（KDDW 同款：`TabBarBase.qml:20-21` `parent.groupCpp`、`TitleBarBase.qml:27` `parent.titleBarCpp // It's set in the loader`）。所有状态读取带空守卫：`panelCpp ? panelCpp.panelTitle : ""`（TitleBarBase.qml:28-34 全组同款）。**注意**：KDDW 因经 Loader 加载叶子，parent 是 Loader，需 Loader 上声明转发属性（Group.qml:171-175）；SARibbon P0 叶子直接 `setParentItem(宿主)`，parent 即宿主，**不引入 Loader 层**（少一层转发，见 findings §四）。context property 只用于全局服务对象（KDDW 仅 3 个：`_kddwHelpers/_kddwDockRegistry/_kddw_widgetFactory`，Platform.cpp:182-186）——SARibbon 对应物是 RibbonTheme/RibbonMetrics 单例（已走类型注册，不占 context property）。
- **QML→C++ 事件回传**：Base 叶子声明 signal（TitleBarBase.qml:47-58 `closeButtonClicked` 等），视觉层触发，Base 的 handler 调宿主 Q_INVOKABLE（TitleBarBase.qml:86-101 → TitleBar.h:80-90 `onCloseClicked/onFloatClicked...`）。SARibbon 对应：叶子里按钮 `onClicked: panelCpp.optionButtonClicked()`，宿主 Q_INVOKABLE 转 Q_SIGNALS。
- **交互事件重定向**（叶子内 MouseArea 事件转给 C++ 宿主处理）：KDDW 用 `redirectMouseEvents(mouseArea)` Q_INVOKABLE + 事件过滤器（TabBarBase.qml:65-68 握手、View.h:115、View.cpp:39-104 MouseEventRedirector/:175-185）。SARibbon P0 按钮交互直接在叶子的 MouseArea/AbstractButton 里发 signal 即可，**不做通用重定向器**（KDDW 需要它是因为拖拽等逻辑在 C++ controller；SARibbon 交互留前端，D7）。
- **主题绑定规则（SARibbon 特有增强，KDDW 无先例）**：KDDW qtquick 叶子颜色全部硬编码（TitleBar.qml:27 `"#eff0f1"`、Group.qml:33-38 border `"#b8b8b8"`），换肤 = 换叶子文件；SARibbon 改为叶子内**一切颜色/尺寸绑定 RibbonTheme/RibbonMetrics 单例属性**，禁止散落颜色常量（评审检查点：叶子里出现字面量色值 = 打回）。绑定点集中在 Base 层，视觉层引用 Base 的 readonly 属性。
- **换叶子定制（Tier 2 预留缝）**：KDDW 机制 = 工厂虚函数返回叶子 URL（ViewFactory.h:75-83 `Q_INVOKABLE titleBarFilename()/tabbarFilename()` "Called by QML"，实现 ViewFactory.cpp:174-202 qrc URL 表）+ 叶子内 `Loader { source: _kddw_widgetFactory.titleBarFilename() }`（Group.qml:171-185）+ 应用侧子类工厂换 URL（examples/qtquick/customtabbar/main.cpp:27-34 `CustomViewFactory::tabbarFilename() → "qrc:/MyTabBar.qml"`，:47 `config.setViewFactory(...)`）；用户自定义叶子直接目录 import 库内 qml（MyTabBar.qml:15 `import "qrc:/kddockwidgets/qtquick/views/qml/" as KDDW`——qrc 目录 import 无需 qmldir）。SARibbon P0：叶子 URL 集中在库内一个 `namespace SARibbonQmlLeafUrls`（或 _p 头内联函数），**不建工厂类**；Tier 2 若开放定制再把该函数表升为可替换工厂（记 NOTES 观察项）。
- **资源路线**：叶子 .qml 全部进 `saribbon_qml.qrc` 编入库二进制，**不安装 .qml 文件、无 qmldir**——KDDW/QWK 安装侧都只有头文件+库（KDDW `src/CMakeLists.txt:664-666` 仅 install headers；QWK `src/CMakeLists.txt` qwk_add_library 宏仅 install TARGETS+头同步），两家在 Qt6 下亦如此（结论已写 findings §三.2；round2 整合已同步：v2 §6.1 批注②、03 S6-3 安装清单终态结论、01 S11 Config 的 Qml find_dependency 分支）。

**验证**：`tests/qml/tst_ribbonPanel.cpp`：给定一组固定 sizeHint 的按钮 + 固定 metrics，断言每项 `QQuickItem` 的 `x/y/width/height` 与 `tests/core` fixture 的黄金几何**逐项相等**（这是 v2 §7.2 "双前端各自把引擎几何正确应用到了真实 item 上"的 Panel 部分）。注意 item 必须挂窗曝光后 polish 才跑，见 S7 的测试要点。

**提交**：`feat：RibbonPanel QML 结构宿主复用 PanelLayoutEngine`

### S4 结构宿主：RibbonBar / RibbonCategory / RibbonTab

**操作**：
1. `src/qml/bar/RibbonBar.h/.cpp`：C++ 宿主——tab 行排布调 `SARibbonBarGeometryEngine`（若计划 02 S7 走了 D6 降级，则此处只用 metrics 部分，tab 几何在宿主内用**从 widgets 语义移植**的规则，**移植前先把该规则补进 core 引擎**——铁律检查点）；`RibbonBar` 高度从 `RibbonMetrics` 取。tab 条本身作为 RibbonBar 的内部子宿主实现（暂名 `RibbonTabBar`）：它**不在 v2 §5.2 的 P0 用户类型清单内**，注册为 uncreatable 或干脆不注册（仅 C++ 内部类），避免公共 API 面膨胀。
2. `src/qml/category/RibbonCategory.h/.cpp`：panel 排布调 `SARibbonCategoryLayoutEngine`；`clampScrollOffset` 用引擎值，滚动动画用 QML `Behavior on x`（前端动画，v2 §3.4.3）。
3. `src/qml/tab/RibbonTab.h/.cpp`：上下文标签页颜色规则从 core theme 取（context 色进 theme，计划 02 S2 已保证；若缺，回补 core）。
4. 对应 `.qml` 视觉叶子；各宿主的子项收集/重 polish 机制复用 S3 骨架与要点（显式登记列表 + `itemChange` + `polish()` + 叶子创建三部曲 + 析构 deleteLater）；叶子组织走 S3"QML 叶子组织规范"（RibbonTab 视觉简单可单文件，RibbonTabBar 的 tab 条叶子若含交互按钮建议 Base+视觉两层）。
5. **tab 条不用 QML Repeater/model 路线（设计附注，第2轮）**：KDDW 的 tab 条是"C++ QAbstractListModel（DockWidgetModel，TabBar.h:114-151）+ QML Repeater 生成 delegate"（TabBar.qml:100-107 `model: root.groupCpp.tabBar.dockWidgetModel`），因为 tab 位置由 QML 控件自排。SARibbon 的 tab 几何出自 core 引擎（S4.1），每个 RibbonTab 是 C++ 宿主子项、由 `updatePolish` 统一 `setPosition/setSize`——Repeater 会把定位权交回 QML，违反铁律。**P0 不引入 model**；若 Tier 2 定制场景（tab 条重内容化）出现，DockWidgetModel 是现成先例（含 titleChanged→dataChanged 转发，TabBar.cpp:478-499 insert 时的信号接线）。

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

**参考样板（第2轮逐行核实）**：QWK `examples/qml/`——CMake 为普通可执行 + qml 文件进 qrc（`CMakeLists.txt:1-13`：`qwk_add_example(... SOURCES ${_src} QT_LINKS Core Gui Qml Quick LINKS QWKQuick)`，宏体 `examples/CMakeLists.txt:3-12` = `add_executable` + AUTOMOC/AUTORCC；`qml.qrc:2-5` 两个 .qml）；`main.cpp:24-28`：`QGuiApplication` + `QQmlApplicationEngine` + `QWK::registerTypes(&engine)` + `engine.load(QUrl("qrc:///main.qml"))`；`main.qml:5` `import QWindowKit 1.0`。KDDW `examples/qtquick/dockwidgets/CMakeLists.txt:12-37` 同构（cmake_minimum_required + AUTOMOC/AUTORCC + `find_package(KDDockWidgets)` 独立构建 fallback（:21-28）+ `add_executable(main.cpp qrc)`（:35）+ 链库（:37），无 qml module）。**示例侧不用 `qt_add_qml_module`**（两家在 Qt6 下也不用），qrc 方式两版 Qt 通吃。

**操作**：`examples/qml/QmlMainWindowExample/`：
1. CMakeLists 骨架（照抄级，出处见行内注）：

   ```cmake
   # examples/qml/QmlMainWindowExample/CMakeLists.txt
   project(QmlMainWindowExample VERSION 0.1)
   set(CMAKE_AUTOMOC ON)                       # KDDW 示例 CMakeLists.txt:16-17
   set(CMAKE_AUTORCC ON)
   find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)
   add_executable(QmlMainWindowExample main.cpp qml.qrc)   # main.qml/子页全进 qml.qrc（QWK qml.qrc:2-5；KDDW resources_qtquick_example.qrc:2-8）
   if(WIN32)
       set_target_properties(QmlMainWindowExample PROPERTIES WIN32_EXECUTABLE TRUE)  # 照 example/MainWindowExample/CMakeLists.txt:21-25 现例
   endif()
   if(NOT TARGET SARibbonQml)                  # 独立构建 fallback（KDDW 示例 :21-28 同型）
       find_package(SARibbon REQUIRED)         # 组件名以计划 03 的 SARibbonConfig 定稿为准
   endif()
   target_link_libraries(QmlMainWindowExample PRIVATE SARibbon::Qml Qt${QT_VERSION_MAJOR}::QuickControls2)
   ```

   `SARIBBON_BUILD_QML=ON` 时才 add_subdirectory（根 example/CMakeLists.txt 加条件）。
2. `main.cpp` 骨架（单轨：Qt5/Qt6 同码，无版本分支的注册调用）：

   ```cpp
   int main(int argc, char* argv[])
   {
   #if defined(Q_OS_WIN)
       QGuiApplication::setAttribute(Qt::AA_UseOpenGLES);        // KDDW 示例 main.cpp:28-30：Windows 用 GLES，规避桌面 GL 驱动差异
   #endif
   #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
       QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // KDDW 示例 main.cpp:31-34（Qt6 默认开启，无需设）
       QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
   #endif
       qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");              // QWK main.cpp:12-16：Qt6 用 Basic、Qt5 用 Default（按其版本分支设）；固定样式保证截图可对比
       QGuiApplication app(argc, argv);
       QQmlApplicationEngine engine;
       saRibbonRegisterQmlTypes(&engine);                        // QWK main.cpp:26-27 同款：load 之前；单轨下 Qt5/Qt6 都要调（S1.3）
       engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
       if (engine.rootObjects().isEmpty()) return -1;            // 加载失败显式退出（QML 错误已打印）
       return app.exec();
   }
   ```

3. QML：`ApplicationWindow + RibbonBar/Category/Panel/ToolButton` 声明式搭建（`import SARibbon 3.0`），对应 widgets 版 `MainWindowExample` 的主场景（几个 category、大中小按钮混合、主题切换按钮、3 行/最小模式切换）。

**验证**：运行并与 widgets 版**同屏对比**：同字体下条高、间距、装箱一致（v2 M3 判据）；截图存 `docs/3.0/screenshots/`（png 不入 mkdocs nav 无告警）；`SARIBBON_BUILD_STATIC_LIBS=ON` 组合补跑一次，验证 S1.3 的 `Q_INIT_RESOURCE` 静态资源路径（叶子能加载 = qrc 已编入且被 init）。

**提交**：`示例：新增 examples/qml/QmlMainWindowExample`

### S7 跨前端一致性套件（§7.2）

**框架选型（对照 KDDW 已核，第2轮补行号）**：两侧都用 **QtTest C++**，QML 侧配 QQmlEngine/QQmlApplicationEngine，**不用 QtQuickTest**——KDDW `tests/qtquick/tst_qtquick.cpp` 是纯 C++ QTest 类（include `<QtTest/QTest>` + `QQmlApplicationEngine/QQmlContext`，:29-31；测试类 :37-58），仓库内无 QtQuickTest 式 .qml 测试文件（tests/ 下的 main2.qml 等是**场景 fixture**，进 `test_resources.qrc`，`tests/CMakeLists.txt:28`）；CMake 两个前端各一个可执行、共享源码编入两侧（`tests/CMakeLists.txt:28-29` `TESTING_SRCS utils.cpp` + `TESTING_RESOURCES`，:55-64 tst_qtwidgets、:66-76 tst_qtquick，均链 `Qt::Test`）。测试环境要点（全部 KDDW 实证）：
- **QGuiApplication 即可**，无需 QApplication（`src/qtquick/TestHelpers.cpp:54-58` `createCoreApplication`；QTEST_MAIN 在不链 Widgets 时自动展开为 QGuiApplication）；
- **offscreen QPA 默认化**：`maybeSetOffscreenQPA`——未显式传 `-platform` 且未设 `KDDW_NO_OFFSCREEN` 时 `qputenv("QT_QPA_PLATFORM", "offscreen")`（`src/qtcommon/TestHelpers_qt.cpp:278-293`）；SARibbon 在 `tests/qml/` 内放同款 5 行 helper，CI 环境变量双保险（操作6）；
- **固定 Quick Controls 样式**防系统样式插件干扰：`QQuickStyle::setStyle("Basic")`（KDDW 用 Material，`TestHelpers.cpp:73` 注释 "so we don't load KDE plugins"——重点是显式 set，不随系统）；
- **窗口/根对象尺寸是延迟设置的**：KDDW 在 QQuickView 建根后固定 `QTest::qWait(100)`（`TestHelpers.cpp:103/145` 注释 "the root object gets sized delayed"）——SARibbon 用 `QTRY_*` 轮询替代硬等待（下骨架）。

**QML 侧几何断言的关键坑**：`updatePolish()` 只对挂进 QQuickWindow 并进入 polish 阶段的 item 执行（S3 要点）——裸堆 item 断言几何必失败。测试统一模式：`QQuickView`（offscreen 下合法）承载场景 + `show()` + `QTest::qWaitForWindowExposed(&view)` + `QTRY_COMPARE(item->x()..., 黄金值)`；或（更快的白盒路径）直接调用宿主的内部布局入口函数后同步断言。二选一在 `tests/qml/` 内统一，写进测试 README 注释。**测试规范（round2 整合，kddw-qtquick-findings.md §三.5）**：场景 qml 内被测项**必须设 `objectName`**（findChild 定位的前提，KDDW tst_qtquick.cpp:325-329 模式）；叶子内部项经**握手属性**暴露为测试可达性通道（KDDW 明确惯例，Base 叶子内注释原话 "Setting just so the unit-tests can access the buttons"——TabBarBase.qml:70-71、TitleBar.qml:23-25）。这两条约定与上述"二选一"的模式选择一并写进 `tests/qml/` README 注释。

**tests/qml C++ 测试骨架（照抄级，出处见行内注）**：

```cpp
// tests/qml/tst_conformance_qml.cpp —— QQuickView 白盒路线（几何断言主形态）
// 实证对照：KDDW TestHelpers.cpp:89-107（QQuickView + contentItem 挂接 + qWait 延迟尺寸）、
//           tst_qtquick.cpp:29-31/61-75（纯 C++ QTest + 引擎）、:325-329（objectName+findChild 取内部项）
#include <QtTest/QTest>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQuickStyle>                        // 需链 Qt::QuickControls2；若 S5 决策叶子不用 Controls，则本 include 与 initTestCase 那行一并省去
#include <SARibbonQml/SARibbonQmlGlobal.h>
#include "../common/RibbonConformance.h"      // 场景数据（纯头，两侧共享，S7 操作1）

class TestConformanceQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase() { QQuickStyle::setStyle(QStringLiteral("Basic")); }   // TestHelpers.cpp:73 同型（KDDW 库 PUBLIC 链 Controls2 故测试可用，src/CMakeLists.txt:495-496）
    void panelThreeRowMixed()
    {
        QQmlEngine engine;
        saRibbonRegisterQmlTypes(&engine);                 // 与应用同一注册入口（S1.3；static-once 多引擎安全）
        QQuickView view(&engine, nullptr);                 // TestHelpers.cpp:95 同型
        view.setResizeMode(QQuickView::SizeRootObjectToView); // TestHelpers.cpp:138
        view.resize(800, 300);                             // 场景固定尺寸（TestHelpers.cpp:96）
        view.setSource(QUrl("qrc:/scenes/panel_three_row.qml")); // 场景进 tests/qml/test_resources.qrc（KDDW tests/CMakeLists.txt:28 同型）
        view.show();                                       // offscreen 下合法（maybeSetOffscreenQPA，TestHelpers_qt.cpp:278-293）
        QVERIFY(QTest::qWaitForWindowExposed(&view));      // 曝光后 polish 才会跑（S3/S7 关键坑）

        auto* root = view.rootObject();
        QVERIFY(root);
        // 场景 qml 里给每个被测子项设 objectName，C++ 用 findChild 取（KDDW tst_qtquick.cpp:325-329 field1/field2 模式）
        auto* btn0 = root->findChild<QQuickItem*>(QStringLiteral("btn0"));
        QVERIFY(btn0);
        const RibbonGoldenRect g = conformance::panelThreeRowGolden().item(0);  // tests/core/layout_fixtures.h 黄金值
        QTRY_COMPARE(qRound(btn0->x()), g.x());            // QTRY_* 轮询替代 KDDW 的 qWait(100) 硬等待
        QTRY_COMPARE(qRound(btn0->y()), g.y());
        QTRY_COMPARE(qRound(btn0->width()), g.width());
        QTRY_COMPARE(qRound(btn0->height()), g.height());
    }
};
QTEST_MAIN(TestConformanceQml)   // 不链 Widgets → 展开为 QGuiApplication（KDDW 实证 QGuiApplication 足够，TestHelpers.cpp:54-58）
#include "tst_conformance_qml.moc"
```

  ApplicationWindow 级场景（主题桥/示例冒烟）改用 `QQmlApplicationEngine engine; engine.load(QUrl("qrc:/main2.qml 同型场景")); QVERIFY(!engine.rootObjects().isEmpty())`（KDDW tst_qtquick.cpp:75-80/:108-113 模式），断言经 `engine.rootObjects().constFirst()` 下钻。C++ 纯手工建宿主（无 QML 声明）的白盒路径：宿主构造后 `setParentItem(view.contentItem())` + 手动传 engine 建叶子（S3 骨架"引擎获取"注记；KDDW TestHelpers.cpp:94-104 同款挂接）。

**操作**：
1. `tests/common/RibbonConformance.h`：场景数据结构（**纯头、零依赖**，两侧各自 include 编译——对应 KDDW 共享 utils.cpp 的做法）：场景 = 一组 (sizeHint, rowProportion) + metrics 输入 + 期望（黄金值引用 `tests/core/layout_fixtures.h`）。
2. widgets 侧：`tests/widgets/tst_conformance_widgets.cpp` 构造真实 `SARibbonPanel` 场景，断言 `QWidget::geometry()` == 黄金。
3. qml 侧：`tests/qml/tst_conformance_qml.cpp` 构造同场景 QML 树（内联组件串或 test 资源 qrc 中的 .qml），断言 `QQuickItem` 的 `x/y/width/height` == 黄金。
4. 主题切换断言：双端各注册监听 `SARibbonThemeData`，切换后各自重渲染标记变化。
5. CMake 接线：`tests/CMakeLists.txt` 增加 `if(SARIBBON_BUILD_QML) add_subdirectory(qml) endif()`；`tests/qml/CMakeLists.txt` 仿 `tests/widgets/` 的 `add_saribbon_test()` 模式建 tst_* 可执行 + `add_test`（链 `SARibbon::Qml`、`Qt::Test`、`Qt::Quick`；需要 QApplication 的混合场景才链 Widgets——一致性套件 qml 侧**不应**链 Widgets，主题联动断言经 core 信号完成；QTEST_MAIN 因此自动落在 QGuiApplication 分支）。KDDW 组织对照：每前端一个测试可执行、`TESTING_SRCS/TESTING_RESOURCES` 共享编入两侧（tests/CMakeLists.txt:28-29/:55-76）——SARibbon 对应物是 `tests/common/RibbonConformance.h`（纯头）+ 各侧自己的场景资源；qml 侧场景 .qml 进 `tests/qml/test_resources.qrc`（AUTORCC 编入测试可执行，与库叶子 qrc 分离，避免测试场景污染库资源）。
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
1. `docs/zh/build-guide/`、`docs/zh/dev-guide/`（及 docs/en 对应页）：三模块架构总览、core 引擎与契约、QML 模块（类型清单/注册/主题桥）、贡献者指南更新（目录、规范、`dev-3.0` 流程）。dev-guide 增补 **QML 编码规范**（round2 整合，kddw-qtquick-findings.md §三.4）：叶子颜色/尺寸一律绑定 RibbonTheme/RibbonMetrics 单例、绑定点集中在 Base 层、**叶子出现字面量色值 = 评审打回**（S3"QML 叶子组织规范"的主题绑定规则；此为 SARibbon 特有增强、无 KDDW 先例——KDDW 叶子颜色硬编码属反面参照，TitleBar.qml:27、Group.qml:33-38）。
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
| QML 类型注册踩坑 | **已单轨化（S1，第2轮设计级变更）**：命令式注册 Qt5/Qt6 同码同签名（本机双版本 qqml.h 复核），声明式轨整类坑（plugin 加载路径、RESOURCE_PREFIX、静态 plugin_init、`QML_*` 宏 Qt5 不存在）不再存在于 3.0 路径；残余风险=应用漏调 `saRibbonRegisterQmlTypes()` → `import SARibbon 3.0` 报"module not installed"——缓解：S1 导入专项检查、示例/迁移指南模板固定该行（S6.2/S9.2）、错误信息排障说明进 QML 文档；CI 两版本都跑（Qt5.15 走 CI） |
| 静态库组合下叶子 qrc 未加载（`QFile::exists("qrc:/SARibbon/...")` 全失败，叶子静默为 null） | S1.3 `Q_INIT_RESOURCE(saribbon_qml)` 静态守卫（KDDW Platform.cpp:40-46/:82-84 同款）+ S1/S6 各做一次 `SARIBBON_BUILD_STATIC_LIBS=ON` 专项检查；叶子创建处带 errorString/exists 告警（S3 骨架注记，KDDW View.cpp:167-170/:857-860 同款） |
| QML 单例所有权（引擎析构连带 delete 包装对象，多引擎测试悬空） | 注册前统一 `QQmlEngine::setObjectOwnership(CppOwnership)`（S1.3/S2.1）；tst_themeBridge 双引擎重复断言兜底 |
| 结构宿主需要 widgets 语义但 core 没有对应引擎（铁律触发） | 停下，回计划 02 补引擎（记 NOTES），禁止 QML 内重写 |
| 发布流程需要维护者权限（tag/PyPI/Release 附件） | agent 完成到"可触发"状态并列 checklist 交维护者确认，不阻塞其余验收；触发器状态在 S10.4/S10.5 双重复核，防止意外发 PyPI |

## 8. 已知偏差

见 [NOTES.md](NOTES.md)。

## 附录A QML 侧标题栏集成预案（Tier 2 参考，第2轮评审补，QWK 实证；P0 不实现）

背景：3.0 QML 侧不做无边框（v2 留守 widgets/frameless 组合），但 M3 之后"RibbonBar 放进 QML `ApplicationWindow` 并兼任标题栏拖动区"的诉求几乎必然出现（widgets 侧 `SARibbonMainWindow` 已有对应形态）。为避免执行 agent 现场发明，按 QWK quick 模块的实证做法预留方案：

- **QWK 的暴露形态不是 attached property，而是普通注册类型 + 显式 setup**：`WindowAgent` 经 `qmlRegisterType<QuickWindowAgent>("QWindowKit", 1, 0, "WindowAgent")` 注册（`src/quick/qwkquickglobal.cpp:23`），QML 侧声明 `WindowAgent { id: windowAgent }`（`examples/qml/main.qml:38-40`），在 `Component.onCompleted` 里 `windowAgent.setup(window)`（main.qml:14-17；C++ 侧 `Q_INVOKABLE bool setup(QQuickWindow*)`，`quickwindowagent.h:25`，实现 `quickwindowagent.cpp:36-54`——把窗口和一个 `QuickItemDelegate` 交给 core 的 WindowAgentBase）。
- **标题栏 = QML 里随便一个 item，交给 agent**：`windowAgent.setTitleBar(titleBar)`（main.qml:57 在 titleBar Rectangle 的 `Component.onCompleted` 里调用；`quickwindowagent.h:28` `Q_INVOKABLE void setTitleBar(QQuickItem*)`）；标题栏内的可交互子项逐个豁免命中测试：`setHitTestVisible(item, true)`（`quickwindowagent.h:33-34`）。
- **前端适配层**：`QuickItemDelegate : WindowItemDelegate`（`quickitemdelegate_p.h:25-48`）把 QQuickItem/QQuickWindow 映射到 core 的窗口语义（window/isVisible/mapGeometryToScene/setWindowState...）——与 SARibbon"QML 侧实现 core 契约接口"（S3 骨架 PrivateData 适配器）同构，可直接当模板抄。
- **SARibbon 预案**：RibbonBar 宿主暴露只读属性（如 `titleBarDragArea`）指向其 tab 条空白区 item，按钮/交互子项列表供 `setHitTestVisible` 逐个登记；组合代码写在**示例层**（`examples/qml` 增加一个 frameless 变体 main_frameless.qml），SARibbonQml 库本体不链接 QWK（依赖矩阵红线，v2 §5.5 精神）；仅当 `SARIBBON_USE_FRAMELESS_LIB=ON` 且 QML 用户实际反馈需要时才升格为库内适配（Tier 2 gate，D7 同款流程，记 NOTES）。
- 决策点：做与不做由 3.1 QML 用户反馈触发；P0 交付物不含任何 frameless/标题栏代码。
