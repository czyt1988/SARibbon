# 第2轮评审 findings：KDDW qtquick / QWK quick 深读（视角=参考项目深度学习）

> 评审对象：`plans/3.0/04-qml-and-release.md`（第1轮修订后版本）
> 参考副本：`F:\src\3rdparty\KDDockWidgets`（**2.0.1**，`CMakeLists.txt:81-84`，HEAD f041992a）、`F:\src\3rdparty\qwindowkit`（**1.0.1**，`CMakeLists.txt:3`，HEAD 1fb3ec7）
> 本轮全部结论基于第2轮独立逐文件核实的 文件:行 证据；引用第1轮结论处均显式标注"第1轮核实"。
> 04 的修改已落盘（本轮直接修订），修改清单见 §二。

---

## 一、深读笔记（按任务 7 项）

### 1. C++ 宿主类的真实骨架（KDDW qtquick/views）

**继承链**：`QtQuick::View : public QQuickItem, public QtCommon::View_qt`（`views/View.h:52`）。第二基类是 controller 架构的 view 接口适配（View_qt 再实现 Core::View 纯虚接口）；具体宿主再加第三层接口，如 `TabBar : public QtQuick::View, public Core::TabBarViewInterface`（`views/TabBar.h:44`）、`Group : public QtQuick::View, public Core::GroupViewInterface`（`views/Group.h:37`）。**SARibbon 无 controller 层（v2 D7），对应形态 = 直接继承 QQuickItem + 契约适配器做成内嵌类/成员**（QWK `QuickItemDelegate` 同构先例见第 7 项）。

**View 基类做什么**（对 SARibbon 有直接借鉴价值的部分）：
- 几何应用：`setGeometry(QRect)` = `setSize(w,h)` + `move(topLeft())`（`View.cpp:157-161`）；`move()` = `setX/setY`（`View.cpp:232-244`）；`setSize` 尊重 minSize（`View.cpp:367-371`）。
- 尺寸变化→布局失效：构造函数里 connect `widthChanged/heightChanged` → `onResize` + `updateGeometry()`（发 `geometryUpdated` 信号，`View.cpp:139-151`、`:522-525`）；`QQUICKITEMgeometryChanged` 手动补发 Resize/Move 事件 + `itemGeometryChanged` 信号（`View.cpp:303-321`，因 QQuickItem::geometryChanged 不是信号，View.h:161-164 注释说明）。
- `itemChange` 只用于补 QQuickItem 不发的 Visible 事件（`View.cpp:202-209`，ItemVisibleHasChanged → sendVisibleChangeEvent）。
- **不用 updatePolish**：KDDW 几何全部由 core controller 驱动（layoutInvalidated 信号链，`Group.cpp:84-85/:128`），无 QML 侧布局挂点——这正是 SARibbon 与 KDDW 的架构差异点，SARibbon 用 `updatePolish()` 做布局入口是对的（04 S3 要点，第1轮已核 Qt 头行号）。
- QML 叶子创建工具：两个 `createItem` 静态函数——`QQmlComponent(engine, filename)` + `create(context)` + 失败打 `errorString()`（`View.cpp:163-173`）；带 parent 版本先沿 parentItem 链找 `qmlEngine(p)`、fallback 到 `Platform::instance()->qmlEngine()` 全局引擎，且创建前 `QFile::exists(cleanQRCFilename(...))` 检查（`View.cpp:840-873`，cleanQRCFilename 处理 `qrc:/`→`:/` :828-838），创建后 `setParentItem(parent)` + `QObject::setParent(parent)` **双 set**（:869-870）。
- 约束传播 hack：min/max 尺寸存成**动态属性** `kddockwidgets_min_size/max_size`（写：`Group.cpp:125-126`、`View.cpp:486/:782`；读：`View.cpp:449-460`）——因其 core layouting 引擎要跨前端读约束。SARibbon 用 `setImplicitSize/polish()` 即可，不必抄这个 hack（04 S3 骨架注记已写明）。
- ⚠️ 用了 Qt **私有 API**：`#include <QtQuick/private/qquickitem_p.h>`（`View.cpp:22`）、`QQuickItemPrivate::get(this)->explicitVisible`（`View.cpp:335-336/:363-364`），CMake 链 `QuickPrivate`（`src/CMakeLists.txt:497`）。**SARibbon 不抄**（进"不抄清单"，§四）。

**子项管理**：逻辑顺序用**显式 model**而非 childItems()——`DockWidgetModel : QAbstractListModel`（`TabBar.h:114-151`），controller 经 `insertDockWidget/removeDockWidget` 显式增删（`TabBar.cpp:251-263`），model insert 时接 `titleChanged`→`dataChanged`、`destroyed`→`remove`（`TabBar.cpp:478-499`）。childItems() 只用于"单子内容项"场景（`DockWidgetInstantiator.cpp:197-202`，校验 size==1）。→ 印证 04 S3"显式登记列表"设计。

**与 core 接线（viewInterface 实现）**：接口方法多为"转发给 controller/model"（`TabBar.cpp:227-238` setCurrentIndex/closeAtIndex；`:240-244` renameTab 空实现——注释"the .qml has a binding to DockWidget::title, so it works automatically"，**数据经 model 绑定自动流动，C++ 不做 setter 转发**）。controller→view 的通知经 KDBindings 信号转 Q_EMIT（`TabBar.cpp:90-94` init() 里 connect）。SARibbon 无 controller，此层省去。

**C++↔QML 双向通道（重要模式）**：
- 握手属性：`Q_PROPERTY(QQuickItem* tabBarQmlItem READ tabBarQmlItem WRITE setTabBarQmlItem NOTIFY ...)`（`TabBar.h:47-48`），叶子 QML 在 `onTabBarCppChanged` 里把自己赋回（`TabBarBase.qml:65-73`，注释明言"Setting just so the unit-tests can access the buttons"——**测试可达性通道**）；TitleBar 同款（`TitleBar.h:33-34` + `TitleBarBase.qml:79-84`）。
- C++ 调叶子 JS 函数：`QMetaObject::invokeMethod(m_tabBarQmlItem, "getTabIndexAtPosition", Q_RETURN_ARG(QVariant, index), Q_ARG(QVariant, globalPos))`（`TabBar.cpp:109-112`、`getTabAtIndex` :194-196）；叶子侧函数契约写在 Base（`TabBarBase.qml:75-88` 抽象占位 console.warn，`TabBar.qml:31-73` 实现）。
- C++ 读叶子自报值：`m_visualItem->property("nonContentsHeight").toInt()`（`Group.cpp:191-194`；QML 侧 `Group.qml:22` readonly property 计算）。
- C++ 写叶子属性：鼠标按下时 `d->m_tabBarQmlItem->setProperty("currentTabIndex", idx)`（`TabBar.cpp:168-172`）。

**叶子生命周期**：宿主析构时对 QML 叶子 `setParent(nullptr)` + `deleteLater()`，**禁止直接 delete**——`Group.cpp:63-72` 及注释（叶子可能正处于其鼠标处理调用栈中，QML 不支持此时删除）。

**QWK 对照**：QWK quick 无"宿主 item"模式（其 QuickWindowAgent 是纯 QObject 服务型，见第 7 项），无 updatePolish 类似物可对照。取舍结论：宿主骨架全部师从 KDDW，QWK 贡献的是适配层形态（delegate）与注册/工程组织。

### 2. QML 叶子的组织（views/qml/，15 个文件）

- **Base/视觉两层**：`TitleBarBase.qml`（契约）→ `TitleBar.qml`（默认视觉，`TitleBar.qml:17-20` 注释"自定义请派生 TitleBarBase 而非 TitleBar"）；`TabBarBase.qml` → `TabBar.qml`。Base 层声明：C++ 宿主引用属性（`TabBarBase.qml:20-21` `readonly property QtObject groupCpp: parent.groupCpp`；`TitleBarBase.qml:27` `parent.titleBarCpp // It's set in the loader`）、从宿主属性派生的只读状态（全部带空守卫，`TitleBarBase.qml:28-34`）、对外 signal（`TitleBarBase.qml:47-58`）、必须由派生实现的 JS 抽象函数（`TabBarBase.qml:75-88`）。
- **宿主绑定方式 = 普通属性 + parent 链查找 + C++ setProperty 注入**：不是 required property、不是 context property。注入点 `Group.cpp:114` `m_visualItem->setProperty("groupCpp", QVariant::fromValue(this))`；叶子经 `parent.xxxCpp` 取。Loader 场景需在 Loader 上声明转发属性（`Group.qml:171-175` Loader 带 `readonly property QtObject titleBarCpp`，叶子的 parent 是 Loader）。
- **叶子内再加载叶子经 Loader + 工厂 URL**：`Group.qml:171-185/:187-204` `Loader { source: _kddw_widgetFactory.titleBarFilename() }`——`_kddw_widgetFactory` 是 context property（`Platform.cpp:185-186`），URL 由工厂虚函数给出（`ViewFactory.h:75-83`，`Q_INVOKABLE` 标注 "Called by QML"；实现 `ViewFactory.cpp:174-202` 全是 `qrc:/kddockwidgets/qtquick/views/qml/*.qml`）。
- **定制 = 子类工厂换叶子 URL**：`examples/qtquick/customtabbar/main.cpp:27-34` `CustomViewFactory::tabbarFilename() → QUrl("qrc:/MyTabBar.qml")` + `:47` `config.setViewFactory(...)`（引擎创建前）；用户自定义叶子直接**目录 import 库内 qml**：`MyTabBar.qml:15` `import "qrc:/kddockwidgets/qtquick/views/qml/" as KDDW`（qrc 目录 import，无需 qmldir）→ `KDDW.TabBarBase {}` 派生（:20）。
- **主题机制：qtquick 侧没有**。颜色全硬编码（`TitleBar.qml:27` `"#eff0f1"`、`Group.qml:33-38` border `"#b8b8b8"`、`TabBar.qml` 用 Controls 默认样式），换肤=换叶子文件。SARibbon 的 RibbonTheme 绑定是**超出 KDDW 的增强**，无先例可抄——04 已按"颜色/尺寸一律绑定单例、集中在 Base 层"规则写死（S3 叶子组织规范）。
- **数据驱动子项**：tab 条用 `Repeater { model: root.groupCpp.tabBar.dockWidgetModel }`（`TabBar.qml:100-107`）——model 驱动仅适用于"QML 自排"的子项；SARibbon 子项几何出自引擎，不采用（04 S4.5 设计附注）。
- **C++→QML 通知**：叶子用 `Connections { target: root.groupCpp; function onCurrentIndexChanged() {...} }`（`TabBar.qml:93-98`）；QML→C++ 经信号 handler 调宿主 Q_INVOKABLE（`TitleBarBase.qml:86-101` → `TitleBar.h:80-90`）。
- **qrc 进库方式**：`kddockwidgets_qtquick.qrc`（前缀 `/kddockwidgets/qtquick/`，15 个 .qml，`:2-18`）**直接列在库源清单**（`src/CMakeLists.txt:158`，KDDW_FRONTEND_QTQUICK_SRCS 尾部）由 AUTORCC 编进库二进制；静态构建显式 `Q_INIT_RESOURCE(kddockwidgets_qtquick)`（`Platform.cpp:40-46` 守卫宏 `KDDOCKWIDGETS_STATICLIB || QT_STATIC`，`:82-84` init() 内调用）。**baseUrl 处理：无**——叶子全部用绝对 `qrc:/` URL（ViewFactory.cpp:174-202），不依赖引擎 baseUrl。
- **全局 context property 只有 3 个**（`Platform.cpp:182-186`）：`_kddwHelpers`（工具）、`_kddwDockRegistry`（注册表）、`_kddw_widgetFactory`（工厂）——宿主/叶子配对不走 context property。

### 3. 注册机制终局评估（QmlTypes.cpp + qwkquickglobal.cpp）

**KDDW `QmlTypes.cpp:21-30` 全文**（Qt5/Qt6 无任何条件编译）：
```cpp
void KDDockWidgets::registerQmlTypes()
{
    qmlRegisterType<MainWindowInstantiator>("com.kdab.dockwidgets", 2, 0, "DockingArea");
    qmlRegisterType<MainWindowMDIInstantiator>("com.kdab.dockwidgets", 2, 0, "MDIDockingArea");
    qmlRegisterType<DockWidgetInstantiator>("com.kdab.dockwidgets", 2, 0, "DockWidget");
    qmlRegisterType<LayoutSaverInstantiator>("com.kdab.dockwidgets", 2, 0, "LayoutSaver");
    qmlRegisterUncreatableMetaObject(KDDockWidgets::staticMetaObject, "com.kdab.dockwidgets", 2, 0,
                                     "KDDockWidgets", QStringLiteral("Enum access only"));
}
```
要点：①注册的只有 **Instantiator 族**（用户声明式入口）——C++ 宿主视图类（TabBar/Group/TitleBar...）**一概不注册**，它们只经 ViewFactory 由 C++ 创建；②枚举暴露走 `qmlRegisterUncreatableMetaObject`，前提是 `namespace KDDockWidgets` 挂 `Q_NAMESPACE`（`src/KDDockWidgets.h:51-53`）+ 枚举逐个 `Q_ENUM_NS`（如 Location，`:65-71`）；③调用点：库内 `Platform::init()`（`Platform.cpp:80-92`，:86），由应用显式 `initFrontend(FrontendType::QtQuick)` 触发（`src/KDDockWidgets.cpp:29`；示例 main.cpp:36）；④无 qmlRegisterModule。

**QWK `qwkquickglobal.cpp:14-25`**：`registerTypes(QQmlEngine*)` = `Q_UNUSED(engine)` + `static bool once` 防重入 + `qmlRegisterType<QuickWindowAgent>("QWindowKit", 1, 0, "WindowAgent")`（:23）+ **`qmlRegisterModule("QWindowKit", 1, 0)`**（:24，KDDW 没有的一步：保证无 qmldir 时模块 import 完整成立）。应用侧 `engine.load` 前显式调用（`examples/qml/main.cpp:25-27`）。`src/quick/` 7 个源文件零 `QT_VERSION` 分支（grep 无命中）。

**Qt6 下两家都没用 qt_add_qml_module 的实证**：KDDW 支持 Qt 6.2+（`CMakeLists.txt:146` `QT_MIN_VERSION "6.2.0"`——qt_add_qml_module 自 6.2 起可用，KDDW 明知而不用）；全仓 grep `qt_add_qml_module`/qmldir 双双零命中（第2轮复核）。QWK README.md:246："You can omit the version number or use 'auto' instead of '1.0' for the module URI if you are using Qt6"——即 Qt6 下依旧命令式 URI import。本机 Qt 头复核命令式 API 全集在两版同签名存在：qmlRegisterType（6.7.3 qqml.h:300/336；5.14.2 :291/:322）、qmlRegisterSingletonInstance（6.7.3 :727；5.14.2 :681）、qmlRegisterSingletonType 回调式（6.7.3 :674-706；5.14.2 :629/:645）、qmlRegisterModule（6.7.3 :645；5.14.2 :616）、qmlRegisterUncreatableMetaObject（6.7.3 :297；5.14.2 :288）、qmlRegisterUncreatableType（6.7.3 :148；5.14.2 :147）。

**对 04 S1 的评估结论：应把命令式单轨升为默认路线（已直接修订 04，设计级变更待整合者复核）**。理由：
1. 两家生产级库在 Qt6 下的实证选择（上）；qt_add_qml_module 带来的收益（qmltypes IDE 补全、qmllint、qmlcachegen AOT、qmldir 声明式 import）均不在 3.0 验收项内。
2. 单轨化把 04 原风险表第 3 行的**整类**声明式轨风险（plugin 加载、RESOURCE_PREFIX、静态 plugin_init、QML_ELEMENT 宏 Qt5 包裹）从"缓解"变成"不存在"；`SARIBBON_BUILD_STATIC_LIBS=ON` 组合验证从"插件路径"简化为"Q_INIT_RESOURCE 是否生效"。
3. Qt5/Qt6 完全同码（QWK quick 模块零版本分支实证），消除双轨漂移面；应用侧代价 = load 前一行 `saRibbonRegisterQmlTypes()`（QWK 用户已在这么用）。
4. 保留原第1轮对 Qt6QmlMacros 的硬约束核实成果为 S1"附注：声明式轨预案"（3.1+ 启用时照抄），知识不丢。
5. 与 v2 的关系：v2 §5.3 写死"Qt6：qt_add_qml_module"——需上达修订（§三.1）；v2 §9-D5（混合模式）不受影响且被强化（第 1/2 项深读把 D5 的宿主↔叶子配对机制完全实证）。

**安装布局对比（结论供计划 03 整合，不改 03）**：
- KDDW（单库拼合模式）：qtquick 前端源码经 `KDDW_FRONTEND_QTQUICK_SRCS` 列表（`src/CMakeLists.txt:132-158`）条件拼进**同一个** `kddockwidgets` 库（:307-308 → :327 add_library），Quick/QuickControls2 PUBLIC 链接（:495-496）+ GuiPrivate/QuickPrivate PRIVATE（:497）；安装 = 头文件 + 库 + cmake 包（qtquick 头 :634-666），**无任何 .qml/qmldir/plugin 安装项**（叶子在 qrc 里随库二进制走）。
- QWK（三库拆分模式，SARibbon 采用的模式）：`QWKQuick` 独立小库（`src/quick/CMakeLists.txt` 全文 = qwk_add_library + QT_LINKS Core Gui Quick + LINKS QWKCore），独立导出宏三态（`qwkquickglobal.h:9-19`），安装同样只有 lib+头+cmake export（`src/CMakeLists.txt` qwk_add_library 宏内 install(TARGETS) + qm_sync_include INSTALL_DIR）。
- **对 SARibbon 的适用性判断：QWK 模式（三库拆分 + 独立导出宏 + 头/库安装）完全适用**，且因单轨化后 QML 侧无安装期产物（叶子进 qrc），计划 03 的安装/打包矩阵**不需要为 QML 增加任何 qmldir/qmltypes 安装规则**——这是单轨化的第二个安装侧红利（若走声明式轨，则必须决定 qmldir+qmltypes 装到 `<prefix>/qml/SARibbon/` 还是依赖 RESOURCE_PREFIX qrc 内嵌，KDDW/QWK 都没有先例可抄）。

### 4. Instantiator 模式评估（DockWidgetInstantiator）

- **存在理由**（`DockWidgetInstantiator.h:27-34` 注释原文）："DockWidget {} in QML won't create a KDDockWidget::DockWidget directly, but instead an DockWidgetInstantiator. DockWidgetInstantiator will then create the DockWidget instance only when the QML parsing ends (and all properties are set). This allows to pass the correct uniqueName to DockWidget's ctor."——**真实对象的构造函数需要 QML 解析完才齐的属性**（uniqueName），才需要包装类。
- 实现：`DockWidgetInstantiator : public QQuickItem`（:35），QQmlParserStatus 的 `classBegin()`（空，`.cpp:175-178`）/`componentComplete()`（`.cpp:180-238`）——后者做：校验 uniqueName 非空/未注册（:182-191）、**childItems() 校验单子内容项**（:197-202）、经 ViewFactory 构造真身（:204-206）、批量接 controller 信号转发为自身信号（:208-222）、guest item 挂接（:224-229）、**缓冲属性延迟应用**（`std::optional<bool> m_isFloating`，.h:113；`.cpp:234-235` 有值才 set——"属性先写进包装、实体建好再应用"模式）。同族：MainWindowInstantiator（.h:30,:71-72 同构）、LayoutSaverInstantiator、MainWindowMDIInstantiator（QmlTypes.cpp:23-26 注册的四件套）。
- **对 SARibbon 的结论（已写入 04 S3 设计附注）：P0 不需要 Instantiator**。SARibbon 宿主本身就是 QQuickItem（无"真身构造需要后到属性"问题），声明式嵌套 `RibbonPanel { RibbonToolButton {} }` 用 QQuickItem 天然子项树 + 各类型自己的 `componentComplete()` 向父宿主登记即可（登记序=声明序，04 S3 要点）。两个顺带实证支持"天然子项树够用"：KDDW 收集声明式内容子项也直接 `childItems()`（DockWidgetInstantiator.cpp:197-202）；`Group.qml` 的内容区就是个裸 `Item { id: stackLayout }`，C++ 经 `setStackLayout(stackLayout)` Q_INVOKABLE 收编（`Group.h:70`、`Group.qml:40-44`、`Group.cpp:165-173`）。SARibbon 用显式登记列表而非 childItems 的动机是**布局序稳定**（z 扰动/装饰项混入），不是子项发现能力。
- 触发重新评估的条件：Tier 2（D7）引入 core controller、宿主变成"薄包装 + 延迟构造真身"时，Instantiator 族照 KDDW 抄（连同 std::optional 缓冲模式）。

### 5. qtquick 测试实践（TestHelpers.cpp + tests/）

- **形态**：纯 C++ QTest（`tests/qtquick/tst_qtquick.cpp:29-31` include QTest/QQmlApplicationEngine/QQmlContext；:37-58 测试类），**无 QtQuickTest、无 .qml 测试逻辑文件**；tests/ 下的 main2.qml 等是场景 fixture，经 `test_resources.qrc` 编入（`tests/CMakeLists.txt:28` `TESTING_RESOURCES`）。
- **场景加载**：`QQmlApplicationEngine engine(":/main2.qml")`（tst_qtquick.cpp:75/:108/:138/:166/:210）——构造即 load；随后从 C++ 侧全局注册表（DockRegistry）拿对象断言（:77-79）。带 context property 的场景：`engine.rootContext()->setContextProperty(...)` 后再 load（:301-303）。
- **应用对象**：QGuiApplication 足够（`src/qtquick/TestHelpers.cpp:54-58` `createCoreApplication`，无 Widgets）；offscreen QPA 默认：`maybeSetOffscreenQPA`——未传 `-platform` 且未设 `KDDW_NO_OFFSCREEN` 时 `qputenv("QT_QPA_PLATFORM","offscreen")`（`src/qtcommon/TestHelpers_qt.cpp:278-293`）；`QQuickStyle::setStyle("Material")` 防加载系统样式插件（TestHelpers.cpp:73 注释 "so we don't load KDE plugins"）。
- **挂窗与等待**：`tests_createView`——`new QQuickView(m_qmlEngine, nullptr)` + `resize(800,800)` + item `setParentItem(view->contentItem())` 双 set + `QTest::qWait(100)`（TestHelpers.cpp:94-104，注释 "the root object gets sized delayed"）；MainWindow 场景用 `setResizeMode(SizeRootObjectToView)` + `setSource("qrc:/main.qml")` + show + `tests_wait(100)`（:135-145）。**无 qWaitForWindowExposed、无 QTRY_***（grep 零命中）——KDDW 全靠固定 qWait；SARibbon 几何断言应升级为 `qWaitForWindowExposed` + `QTRY_COMPARE`（QtTest 标准轮询，语义同 KDDW 的 qWait 但不靠拍脑袋时长）。
- **item 定位**：`findChild<QQuickItem*>("field1")` 按 objectName 下钻（tst_qtquick.cpp:325-329）；叶子内部项靠握手属性暴露（TabBarBase.qml:70-71 "Setting just so the unit-tests can access the buttons"、TitleBar.qml:23-25 "These two are just for unit-tests"）——**QML 侧特意为测试留可达性通道是 KDDW 的明确惯例**。
- **几何断言写法**：KDDW 的 qtquick 测试不直接断言 x/y/w/h（其几何正确性由 core 测试覆盖，视图侧断言的是信号/状态，如 tst_titlebarNumDockWidgetsChanged :135-160 数信号次数）。SARibbon 的一致性套件要断言真实 item 几何（v2 §7.2），骨架 = KDDW 的加载/挂窗/定位模式 + QTRY_COMPARE 几何比对（已写入 04 S7 骨架代码块）。
- **CMake 组织**：每前端一个可执行（tst_qtwidgets :55-64 / tst_qtquick :66-76），共享 `TESTING_SRCS utils.cpp` + `TESTING_RESOURCES` 编入两侧，都链同一个 kddockwidgets 库 + `Qt::Test`（单库拼合使两侧共享库；SARibbon 三库拆分 → widgets 测试链 SARibbon::Widgets、qml 测试链 SARibbon::Qml，场景数据经 `tests/common/RibbonConformance.h` 纯头共享——04 S7 操作1/5 已如此设计，本轮确认与 KDDW 组织等价）。

### 6. 示例工程组织（examples）

- **KDDW qtquick 示例**（`examples/qtquick/dockwidgets/`）：CMakeLists（:12-37）= 独立可构建工程（cmake_minimum_required + project + AUTOMOC/AUTORCC + C++17）+ `find_package(KDDockWidgets QUIET)`/`KDDockWidgets-qt6` fallback（:21-28，支持脱离源码树按安装包构建）+ `add_executable(qtquick_dockwidgets main.cpp ${RESOURCES_EXAMPLE_SRC})`（:35，qrc 两个：自身 + 共享资源）+ `target_link_libraries(... KDAB::kddockwidgets)`（:37）。main.cpp：QGuiApplication（Windows 先 `AA_UseOpenGLES` :28-30；Qt5 补 HighDpi 属性 :31-34）→ `initFrontend(QtQuick)`（:36）→ Config/flags → `QQmlApplicationEngine` + `setQmlEngine(&appEngine)`（:137-139）→ `load("qrc:/main.qml")`（:140）→ C++ 命令式 API 与 QML 声明式混用演示（:142-155）。main.qml：`ApplicationWindow` + `import com.kdab.dockwidgets 2.0 as KDDW`（:14）+ `KDDW.DockingArea { uniqueName; Repeater { KDDW.DockWidget {...} } }`（:86-118）——**声明式子项嵌套 + Repeater 动态生成 Instantiator 都直接可用**。
- **QWK qml 示例**（`examples/qml/`）：CMakeLists（:1-13）= `qwk_add_example`（宏体 `examples/CMakeLists.txt:3-12`：add_executable + AUTOMOC/AUTORCC + win rc/manifest）+ `QT_LINKS Core Gui Qml Quick` + `LINKS QWKQuick`；main.cpp（:10-29）= QT_QUICK_CONTROLS_STYLE 按 Qt 版本设 Basic/Default（:12-16）→ QGuiApplication → QQmlApplicationEngine → `QWK::registerTypes(&engine)`（:26）→ `load("qrc:///main.qml")`（:27）；main.qml = `Window` + `import QWindowKit 1.0`（:5）。
- **共同点（→ 04 S6 骨架的依据）**：都是"普通可执行 + qml 进 qrc + 链库"，**示例侧零 qt_add_qml_module**（两家在 Qt6 下也是）；注册调用都在 load 之前；都设 Quick Controls 样式。差异点：KDDW 有独立构建 fallback（SARibbon 示例现例 `example/MainWindowExample/CMakeLists.txt:26-29` 已有同款 `if(NOT TARGET SARibbonBar) find_package(...)`，沿用）。

### 7. QWK quick 的窗口集成（次要项）

- **形态：普通注册类型 + 显式 setup，不是 attached property**（全模块 grep 无 qmlRegisterAttachedType/QML_ATTACHED）。`WindowAgent` = `QuickWindowAgent : WindowAgentBase`（QObject，`quickwindowagent.h:17`），QML 声明 `WindowAgent { id: windowAgent }`（examples/qml/main.qml:38-40），`Component.onCompleted: windowAgent.setup(window)`（main.qml:14-17；`Q_INVOKABLE bool setup(QQuickWindow*)` quickwindowagent.h:25，实现 quickwindowagent.cpp:36-54）。
- **标题栏/命中测试**：`setTitleBar(QQuickItem*)`（quickwindowagent.h:28；QML 侧 main.qml:57 在 titleBar Rectangle 的 onCompleted 里交出去）、`setSystemButton(button, item)`（:30-31）、`setHitTestVisible(item, true)` 豁免交互子项（:33-34）。
- **前端适配层**：`setup` 内部 `d->setup(window, new QuickItemDelegate())`（quickwindowagent.cpp:47）；`QuickItemDelegate : WindowItemDelegate` 纯虚窄接口逐个映射 QQuickItem/QQuickWindow 语义（quickitemdelegate_p.h:25-48：window/isVisible/mapGeometryToScene/hostWindow/setWindowState/setCursorShape...）。**这与 SARibbon"QML 侧实现 core 契约接口"的适配器是同一模式**，S3 骨架的 PrivateData 契约适配器可直接以它为模板。
- → 已写成 04 附录A"QML 侧标题栏集成预案"（Tier 2；RibbonBar 暴露拖拽区 item + 示例层组合 + 库本体不链 QWK）。

---

## 二、对 04 的修改清单（本轮已落盘）

| 位置 | 修改 | 依据（本轮核实证据） |
|------|------|---------------------|
| S1 标题+路线决策段 | **设计级变更**：命令式单轨（Qt5/Qt6 同码）升为 3.0 唯一默认路线；qt_add_qml_module 降为"附注：声明式轨预案（3.1+）"，第1轮的 Qt6QmlMacros 硬约束知识压缩保留在附注内 | QmlTypes.cpp:21-30、qwkquickglobal.cpp:14-25、KDDW CMakeLists.txt:146/:149、QWK README.md:246、全仓 grep 无 qt_add_qml_module/qmldir、本机双版本 qqml.h 行号复核（§一.3） |
| S1 操作1 | qrc 列入库 SOURCES（AUTORCC）；QuickControls2 库侧链接改为条件化（叶子用 Controls 才链） | KDDW src/CMakeLists.txt:132-158（:158 qrc 入清单）、:495-496 + views/qml/TabBar.qml:12-13 |
| S1 操作2 | qrc 路径定为 `src/qml/qml/saribbon_qml.qrc`（与叶子同目录）；明确"无 qmldir/plugin/RESOURCE_PREFIX 事项" | kddockwidgets_qtquick.qrc:2-18、ViewFactory.cpp:174-202（绝对 qrc URL，无 baseUrl 依赖） |
| S1 操作3 | 新增照抄级注册函数代码块：static-once + **Q_INIT_RESOURCE 静态守卫（新增，原稿缺失）** + qmlRegisterType/UncreatableType/SingletonInstance + **qmlRegisterModule（新增，原稿缺失）**；调用时机模型定为 QWK 显式调用（不抄 KDDW Platform 自动注册） | qwkquickglobal.cpp:15-24、Platform.cpp:40-46/:80-92、KDDockWidgets.cpp:29、QWK examples/qml/main.cpp:25-27 |
| S1 操作4 | 导出宏三态直接照抄 QWK | qwkquickglobal.h:9-19 |
| S1 验证 | 导入专项检查改为"注册后无 qmldir import 成功"；新增静态组合 Q_INIT_RESOURCE 检查 | QWK README:246、View.cpp:857-860（exists 检查先例） |
| S2.1/S2 验证 | 注册改单轨 qmlRegisterSingletonInstance（保留 static create() 作 3.1+ 预案）；测试统一经 saRibbonRegisterQmlTypes | qqml.h 双版本行号（§一.3）、qwkquickglobal.cpp:17-21（once 多引擎安全） |
| S3 新增"宿主类骨架"代码块 | class 声明级骨架：QQuickItem 直接继承、Q_PROPERTY 面（panelTitle/layoutMode/panelQmlItem 握手属性）、registerChildItem 显式登记、componentComplete 双职责、updatePolish 布局入口、itemChange、ensureQmlItem 三部曲；4 条补充注记（析构 deleteLater、引擎获取、C++↔QML 通道及铁律约束、约束传播不抄动态属性 hack） | View.h:52、TabBar.h:44-51、TitleBar.h:33-48、Group.cpp:105-116/:63-72/:119-129/:191-194、View.cpp:157-161/:202-209/:840-873、TabBar.cpp:110-112/:195-196、TabBarBase.qml:65-73、DockWidgetInstantiator.h:27-34（§一.1） |
| S3 新增"Instantiator 评估结论" | P0 不需要 Instantiator；天然子项树+componentComplete 登记够用；std::optional 缓冲模式留作 Tier 2 参考 | DockWidgetInstantiator.h:27-35、.cpp:180-238（:197-202 childItems、:234-235 optional）、Group.h:70 + Group.qml:40-44（§一.4） |
| S3 操作3 | 叶子拆 Base+视觉两层；指向新"叶子组织规范" | TitleBarBase.qml:14-23、TitleBar.qml:17-20（§一.2） |
| S3 操作4（枚举） | 补 KDDW Q_NAMESPACE 方案前提证据 + 计划 02:67"维持全局命名空间"决策交叉引用 + 02:77 未来切换路径 | KDDockWidgets.h:51-53/:65-71、QmlTypes.cpp:28、SARibbonGlobal.h:197/:220（无 namespace 包裹，本轮复核）、计划 02:67/:77 |
| S3 新增"QML 叶子组织规范" | 6 条：两层拆分、父链属性配对（不用 required/context property、P0 不引 Loader 层）、signal→Q_INVOKABLE 事件回传、不做通用鼠标重定向、主题绑定集中 Base 层（禁字面量色值）、换叶子定制 Tier 2 预留缝 + qrc 资源路线与安装结论 | TabBarBase.qml:20-21/:75-88、TitleBarBase.qml:27-34/:47-58/:86-101、Group.cpp:114、Group.qml:171-185、Platform.cpp:182-186、ViewFactory.h:75-83、customtabbar/main.cpp:27-34/:47、MyTabBar.qml:15/:20、TitleBar.qml:27、Group.qml:33-38、TabBarBase.qml:65-68 + View.cpp:39-104（§一.2） |
| S4 操作4/新增操作5 | 叶子机制复用 S3 规范；tab 条**不用 Repeater/model 路线**的设计附注（几何权在引擎）+ Tier 2 model 先例指引 | TabBar.qml:100-107、TabBar.h:114-151、TabBar.cpp:478-499（§一.2/.4） |
| S6 | 补照抄级 CMakeLists 骨架 + main.cpp 骨架（AA_UseOpenGLES、HighDpi、QT_QUICK_CONTROLS_STYLE、注册调用、rootObjects 空检查）；静态组合验证目标从"插件路径"改"Q_INIT_RESOURCE"；参考样板段补精确行号 | QWK examples/qml/CMakeLists.txt:1-13 + examples/CMakeLists.txt:3-12、KDDW examples/qtquick/dockwidgets/CMakeLists.txt:12-37、QWK main.cpp:12-28、KDDW main.cpp:28-40/:137-140（§一.6） |
| S7 | 框架选型段补第2轮行号与 4 条测试环境要点（QGuiApplication、offscreen 默认化 helper、固定 Controls 样式、延迟尺寸）；新增照抄级 tests/qml C++ 测试骨架（QQuickView 白盒主形态 + QQmlApplicationEngine 场景形态 + findChild/objectName + QTRY_COMPARE）；操作5 补测试资源 qrc 组织与 KDDW 对照 | TestHelpers.cpp:54-58/:73/:89-107/:135-145、TestHelpers_qt.cpp:278-293、tst_qtquick.cpp:29-31/:75-80/:301-303/:325-329、tests/CMakeLists.txt:28-29/:55-76（§一.5） |
| 风险表 | 注册风险行按单轨化重写（残余风险=漏调注册函数）；**新增**"静态库 qrc 未加载"风险行 | §一.3、Platform.cpp:40-46、View.cpp:167-170/:857-860 |
| 新增附录A | QML 侧标题栏集成预案（Tier 2）：QWK WindowAgent 模式全记录 + SARibbon 预留方案（示例层组合、库不链 QWK） | quickwindowagent.h:17/:25-34、quickwindowagent.cpp:36-54、quickitemdelegate_p.h:25-48、examples/qml/main.qml:14-17/:38-40/:57（§一.7） |

未改动：S5（其 Action.h:17 引文本轮复核无误：`src/core/Action.h:17` "Class to abstract QAction, so code still works with QtQuick and Flutter"，QtQuick 端纯 QObject `src/qtquick/Action.h:20`）、S8、S9、S10、验收门、前置条件表。

---

## 三、设计级建议

1. **注册路线单轨化（已直接修订 04 S1，⚠️待整合者复核）→ 需上达 v2 §5.3 修订**：v2 §5.3 原文"Qt5：命令式；Qt6：qt_add_qml_module(URI SARibbon VERSION 3.0)"应改为"Qt5/Qt6 统一命令式 `saRibbonRegisterQmlTypes()`（KDDW 2.0.1 / QWK 1.0.1 在 Qt6 下的实证方案）；qt_add_qml_module 列为 3.1+ 可选优化（触发条件：需要 qmltypes IDE 补全 / qmllint / qmlcachegen AOT 任一）"。"qmldir/plugin 资源随 qml 模块安装"一句随之删除（单轨下无此产物）。v2 §9-D5 不需修订——混合模式被本轮深读全面实证（宿主/叶子配对机制见 findings §一.1/.2），只是 D5 的注册配套从双轨变单轨。
2. **安装布局（供计划 03 整合者，本轮未改 03）**：三库拆分采用 QWK 模式已确认适用；**QML 侧安装期零新增产物**——叶子 .qml 进 qrc 随 libSARibbonQml 二进制安装，无 qmldir/qmltypes/plugin 安装项（KDDW src/CMakeLists.txt:634-666 与 QWK qwk_add_library 均只装头+库的实证）。03 若有"QML 模块安装"预留段落，可按此收敛；`SARibbonConfig.cmake` 的 Qml 组件只需 find_dependency(Qt Quick/Qml)（+QuickControls2 视 S5 决策）。
3. **契约适配器模板**：S3 宿主的"契约接口适配器"直接以 QWK `QuickItemDelegate`（quickitemdelegate_p.h:25-48）为模板——纯虚窄接口 + QQuickItem 语义逐个映射，放 .cpp 私有；不必发明新形态。
4. **主题绑定是 SARibbon 特有增强，无 KDDW 先例**：KDDW 叶子颜色硬编码、换肤=换叶子（TitleBar.qml:27、Group.qml:33-38）。04 已定规则"叶子颜色/尺寸一律绑定 RibbonTheme/RibbonMetrics、集中在 Base 层、字面量色值=评审打回"——建议整合者把该规则同步进 dev-guide 的 QML 编码规范（S9.1 文档任务顺带）。
5. **测试可达性通道列为叶子契约的一部分**：KDDW 在 Base 叶子里明确为单测留握手属性（TabBarBase.qml:70-71、TitleBar.qml:23-25 注释）。SARibbon 的 panelQmlItem 握手属性已承担此职责（04 S3 骨架），建议 S7 测试骨架的 objectName 约定（场景 qml 内被测项必须设 objectName）作为测试规范写进 tests/qml README 注释。

## 四、未解决 / 待核实

1. **qmlRegisterType 全局注册表 × 多引擎**：命令式注册进 QQmlMetaType 进程级注册表（QWK 的 static-once + Q_UNUSED(engine) 暗示全局生效），S2 验证"连续建两个引擎"应能覆盖；但"第二个引擎在注册发生**之后**创建/之前创建"的时序组合建议执行时在 tst_themeBridge 里各测一次（QWK/KDDW 测试均为"先注册后建多引擎"路径，未见反例，风险低）。
2. **Q_INIT_RESOURCE 符号名与 sa_add_library 的 AUTORCC 交互**：`Q_INIT_RESOURCE(saribbon_qml)` 要求 qrc 基名为 saribbon_qml 且资源初始化符号未被裁剪；若计划 01 S5 的 sa_add_library 对 qrc 有特殊处理（如预编译成 .cpp），宏名需随之调整——执行 S1 时以实际构建产物核实（本轮未构建，遵守任务约束）。另注意 Q_INIT_RESOURCE 不可在 namespace 内调用（注册函数保持全局命名空间，或按 KDDW Platform.cpp:41-45 用文件级 static 自由函数包一层）。
3. **叶子父链属性查找 × Loader**：KDDW 的 `parent.titleBarCpp` 依赖"叶子的 parent 恰是持有转发属性的 Loader"（Group.qml:171-175）。SARibbon P0 已定"直接 setParentItem(宿主)、不引 Loader"（04 S3 叶子规范），但若 S5 的 RibbonToolButton 菜单弹出等场景引入 Loader/Popup，需逐层转发 panelCpp 或改用显式 setProperty 注入——执行时留意，别混用两种取宿主方式。
4. **KDDW 私有 API 不抄清单**：QQuickItemPrivate（View.cpp:22/:335-336）、QtQuick/private 链接（src/CMakeLists.txt:497）、qhighdpiscaling_p.h（View.cpp:24）——SARibbon 宿主骨架全部走公共 API（04 S3 已核 setPosition/setSize/setImplicitSize 的公共性），执行中如遇"KDDW 这么写"的私有 API 诱惑，以本条否决。
5. **QuickControls2 是否进库链接**：取决于 S5 RibbonToolButton 菜单实现（Controls Menu/Popup vs 自绘叶子）；KDDW 因叶子用 Controls TabBar/TabButton 而 PUBLIC 链接（src/CMakeLists.txt:495-496）。04 S1 操作1 已留条件化表述，S5 执行时定案并记 NOTES。
6. **Repeater/model 路线的 Tier 2 触发条件**：tab 条重度定制（用户在 QML 里自定义 tab delegate）时才有价值；P0 已在 04 S4.5 写死不采用。若 3.1 出现该需求，DockWidgetModel（TabBar.h:114-151 + TabBar.cpp:478-499 信号接线）是完整先例。
