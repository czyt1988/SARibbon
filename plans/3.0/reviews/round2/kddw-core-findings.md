# 评审轮2：KDDockWidgets core 深读发现（视角：参考项目深度学习）

> 评审对象：`plans/3.0/02-core-sinking.md`（轮1修订后版本）
> 参考项目：`F:\src\3rdparty\KDDockWidgets`，版本 **2.0.1**（顶层 CMakeLists.txt:81-84），git `f041992a`（2023-12-20）
> 方法：逐文件真读 KDDW core 源码，全部结论带 `文件:行` 证据；SARibbon 侧行号以 2.9.5 基线 `src/SARibbonBar/` 为准，本文引用的 SARibbon 行号均经本轮独立抽查核实。
> 本轮独立于轮1：所有对照结论基于本轮核实的证据。

---

## 一、KDDW core 深读笔记（按 7 项）

### 1. LayoutingGuest/Item 契约的极简性

**KDDW 事实**：

- 契约全集在 `src/core/layouting/LayoutingGuest_p.h:31-70`，一个类三层内容：
  - **8 个纯虚**（:36-43）：`minSize()` / `maxSizeHint()` / `setGeometry(Rect)` / `setVisible(bool)` / `geometry()` / `setHost(LayoutingHost*)` / `host()` / `id()`；
  - **2 个带默认实现的虚函数**（:45-53）：`freed()`（默认 false，Flutter 用）、`debugName()`（默认空串，诊断用）；
  - **引擎回指 + 3 个 KDBindings 信号**（:55-63）：非虚 `layoutItem()/setLayoutItem()`（存于 PIMPL，`Item.cpp:4122-4147`，内部是 `ObjectGuard<Core::Item>` 弱引用 + ref/unref 引用计数），信号 `hostChanged` / `beingDestroyed` / `layoutInvalidated`。
- **引擎如何消费契约**（`Item.cpp`）：
  - 装配时一次性拉取约束：`setGuest()` 里 `setMinSize(guest->minSize()); setMaxSizeHint(guest->maxSizeHint())`（Item.cpp:203-207）；
  - 约束变化经 `layoutInvalidated` 信号触发 `onWidgetLayoutRequested()` 重拉 min/max（Item.cpp:212-213, 913-931）；
  - 几何回写单点收口：`updateWidgetGeometries()` → `m_guest->setGeometry(mapToRoot(rect()))`（Item.cpp:229-234）；
  - 可见性回写：`setIsVisible()` → `m_guest->setVisible(true)`（Item.cpp:712-725），引擎自持 `m_isVisible`（Item_p.h:350）；
  - 生命周期握手：guest 实现类在自己的析构里 `beingDestroyed.emit()`（示例见 tests/tst_multisplitter.cpp:47-50、Group.cpp:1024、View.cpp:61），引擎侧 `onGuestDestroyed()` 置空指针并转 placeholder 或移除（Item.cpp:209-210, 900-911）。
- **identity 方案**：`id()` 返回 QString（LayoutingGuest_p.h:43），**只用于 JSON 序列化/恢复的关联**——`to_json` 写 `guestId`（Item.cpp:242-243），`fillFromJson` 用 `std::unordered_map<QString, LayoutingGuest*>` 找回（Item.cpp:246-262，Item_p.h:294-299）。运行期标识仍是指针；悬垂防护靠 `ObjectGuard`（Qt 前端即 `QPointer` 别名，ObjectGuard_p.h:18-25）+ `beingDestroyed` 信号 + Item 引用计数（Item.cpp:283-298, 4133-4147）。
- **setVisible 为什么是引擎回写**：KDDW 引擎是**常驻的**（Item 树跨多次布局调用存活），且有 placeholder 语义——dock 关闭后 Item 保留为不可见占位以便原位恢复（`turnIntoPlaceholder()` Item.cpp:874-882，`restore()` :313 起，回归测试 tst_multisplitter.cpp:1617-1655）。可见性状态的**权威在引擎**，guest 只是镜像，所以必须回写。
- **"引擎不持有 QObject"的真相（纠偏）**：KDDW 的引擎节点 `Item` 本身就是 QObject（`class Item : public Core::Object` + `Q_OBJECT`，Item_p.h:190-192；Qt 前端 `Core::Object = QObject`，QtCompat_p.h:80-83）。真正 QObject-free 的是**契约面**：`LayoutingGuest/LayoutingHost/LayoutingSeparator` 三个接口均非 QObject，信号用 KDBindings（非 Qt 信号）以便 Flutter 前端实现（LayoutingGuest_p.h:17,61-63）。引擎与 GUI 的解耦点是"窄接口 + 非 Qt 信号"，不是"引擎无 QObject"。
- Host 契约更小：`LayoutingHost_p.h:33-53` 仅 1 个纯虚 `supportsHonouringLayoutMinSize()` + 公有 `m_rootItem` 指针 + 2 个便利插入方法。margins/spacing/RTL 一概没有——KDDW 引擎用 separator 厚度表达间距（`Item::separatorThickness` 静态量，Item_p.h:213），无 contentsMargins 概念，无 RTL（见第 5 项）。
- Separator 契约：2 个纯虚 `geometry()/setGeometry(Rect)`（LayoutingSeparator_p.h:33-34）；**引擎不 include 任何 GUI 头来造 separator**，靠函数指针注入 `CreateSeparatorFunc`（Item_p.h:37,302,353-354），由 Platform 构造时注入 KDDW 实现（Platform.cpp:73-81，注释明言"Our layouting engine can be used without KDDW"）。同理 `DumpScreenInfoFunc`（Platform.cpp:57-64）把"屏幕信息打印"这种平台能力注入引擎。
- 引擎级可调常量是**静态字段**：`hardcodedMinimumSize/hardcodedMaximumSize/separatorThickness`（Item_p.h:211-213），由 `Config` 单例做门面读写（Config.cpp:156-174, 235-256），文档要求"仅在建任何 DockWidget/MainWindow 之前设置"（Config.h:60-62）。

**对照 SARibbon 契约（02 S4.1，轮1修订版）评估**：

| 维度 | KDDW | SARibbon 02 现状 | 结论 |
|---|---|---|---|
| 输入纯虚 | minSize/maxSizeHint/geometry | sizeHint/minimumSizeHint/isHidden/expandingDirections/maximumWidth/stretchFactor | SARibbon 输入面更宽是**算法实况决定**的（2.x 逐点依赖已核实：maximumWidth cpp:1238、gallery stretchFactor cpp:1244-1246），不是过度设计 |
| 输出回写 | setGeometry+setVisible（引擎常驻、有 placeholder） | applyGeometry + 契约字段 resultGeometry/rowIndex/columnIndex；可见性只读 isHidden，隐藏批处理留适配器 doLayout | **合理分叉**：SARibbon 引擎是"每调用一次算一次"的瞬态引擎（layout() 返回 Result），无 placeholder 语义，可见性权威在 widget/action 侧；KDDW 的 setVisible 回写解决的是"引擎比 guest 更早知道该不该显示"，SARibbon 里该信息经 Result 标志（separator 显隐、show/hide 批处理清单）流向适配器，信息流等价 |
| identity | QString id()（仅序列化）+ ObjectGuard/信号（悬垂防护） | item 指针作缓存 key | SARibbon 3.0 无布局序列化需求（CustomizeData 的 keyValue 是数据层字符串，不是布局 id），**不需要 id()**；悬垂防护靠既有不变量——2.x takeAt 已做"移除缓存条目防陈旧"（SARibbonPanelLayout.cpp:302-303 `mButtonSizeHintCache.remove(item->widget())`，注释 "Remove from cache to prevent stale entries"）+ invalidate() 全清（cpp:367-368）。Step A 把 key 换成 item 指针后，这两处清理必须同步换成引擎缓存 API（02 附录 C takeAt 行原未提此副作用，本轮已补） |
| 诊断 | debugName() 虚函数（默认空）→ Item::updateObjectName → dumpLayout 打印名字而非地址（Item.cpp:884-898, 821-842） | 无 | **建议补**：契约加 `virtual QString debugName() const { return {}; }` 默认实现，widgets 侧返回 widget objectName。成本≈0，黄金测试失败诊断从"item=0x7ff…"变成可读名字 |
| 生命周期信号 | beingDestroyed/layoutInvalidated/hostChanged（KDBindings） | 无（Qt 侧 QLayout::invalidate 即约束变化通知；引擎缓存以 largeHeight 锚定失效，cpp:826-828） | 不需要：SARibbon 引擎瞬态调用 + 适配器持缓存，约束变化由 QLayout 系统驱动重入 layout()，无跨调用握手需求 |
| 常量注入 | 引擎静态量 + Config 门面 + 函数指针注入 separator 工厂 | Metrics 实例字段（每 layout 实例一份）+ 引擎不造任何 widget | SARibbon 方案**更优**：实例字段天然多窗口隔离，无"启动前设置"的全局时序约束；引擎无 separator 构造需求（SARibbonSeparatorWidget 由工厂在 widgets 侧创建），不需要 CreateSeparatorFunc 注入 |

**新发现（轮1未覆盖）**：2.x `SARibbonPanelItem::rowIndex` 的类型是 **`short`**（SARibbonPanelItem.h:51），而 v2 §3.4.1 契约草案写 `int rowIndex = -1`。轮1的 S4.1-3 说"同名同型……搬入基类后删除派生类字段即可保持源码兼容"——**同型不成立**。读/赋值场景 int 隐式转换兼容，仅 `short*`/`short&` 取址绑定会断（全仓 grep 已验证无此用法，见本文四-3）。处置：契约用 `int`（避免 short/int 混用提升警告），在 NOTES.md 记为 3.0 允许的微小源码差异。

### 2. SizingInfo/布局算法的组织

**KDDW 事实**：

- `SizingInfo` 是**显式的"单项布局状态"结构体**（Item_p.h:93-188）：数据仅 5 个字段 `Rect geometry; Size minSize; Size maxSizeHint; double percentageWithinParent; bool isBeingInserted;`（:183-187），其余全是方向感知访问器（`length/setLength/incrementLength/pos/setPos/availableToGrow/neededToShrink`，:107-180）+ `typedef Vector<SizingInfo> List`（:182）。
- 算法三段式（以 growItem 为例，Item.cpp:3021-3049）：
  1. **快照**：`SizingInfo::List sizes = this->sizes()`——从子项收集值拷贝（:3051-3067）；
  2. **纯函数计算**：`growItem(index, sizes, ...)` / `shrinkNeighbours(index, sizes, ...)`（:3138-3169）/ `calculateSqueezes(begin, end, needed, strategy, reversed)`（:3069-3136，**迭代器进、Vector<int> 出，零成员写入**，是最纯粹的分配算法）在快照上迭代；
  3. **统一回写**：`applyGeometries(sizes)` 逐项 `setSize_recursive` 后再 `positionItems()` 排位置（:3036-3049）。
- 方向泛化用自由函数：`inline int pos(Point, Qt::Orientation)` / `length(Size, Orientation)`（Item_p.h:83-91），全套算法对横竖两个方向零分支复制。
- **中间状态可序列化**：`to_json/from_json(SizingInfo)`（Item_p.h:618-619），Item 整体也可序列化（Item.cpp:236-271）。测试里有专门的往返测试 `tst_sizingInfoSerialization`（tst_multisplitter.cpp:2080-2103）与 `tst_itemSerialization`（:2105-2130）。
- 嵌套 host：容器也是 Item（`ItemContainer : Item`、`ItemBoxContainer : ItemContainer`，Item_p.h:364-588），min/max 尺寸递归聚合（`minSize()/maxSizeHint()` 虚覆写 :436-437），无独立"嵌套引擎"。

**对照 02 S5/S6 评估**：

- 02 的 `Input/Result` 结构体设计与 KDDW 同构度判断：SARibbon 的 `Input`（mode/margins/spacing/isRTL/titleTextWidth…）≈ KDDW 的"引擎外部条件"，`Result`（sizeHint/columnCount/largeHeight/titleGeometry…）≈ KDDW 的"applyGeometries 之后的整体状态"。**同构，但 SARibbon 缺 KDDW 的中间层**：KDDW 在 Input 与最终几何之间有一个可命名、可拷贝、可序列化、可单测的 `SizingInfo::List` 中间态；SARibbon 2.x 的对应物是散落局部量（Panel 侧 `columMaxWidth`/`yMediumRow` 等局部数组；Category 侧已有半显式的 `SizeHintCollection`，SARibbonCategoryLayout.cpp:30-35）。
- **M1 不得引入该重构**（违反纯 move 纪律）。落点：① 02 S5.2 加一句"M1 保持 2.x 局部量原样搬移；显式中间态结构体化（SizingInfo 式）列为 3.1 引擎优化项"，防止执行 agent 顺手重构；② Category 引擎的 `SizeHintCollection` 搬移时**保留结构体形态**（它已经是显式中间态，是 SARibbon 里最接近 SizingInfo 的东西，值得在 NOTES.md 点名保护）；③ 测试侧可以先行受益：黄金 fixture 的"期望输出"就是 Result+每项字段的显式快照，与 KDDW 用 JSON 存 SizingInfo 快照同思路（见第 5 项）。
- `calculateSqueezes` 的"迭代器+纯函数"形态提示：SARibbon `recalcExpandGeomArray` 的加权分配段（cpp:1275-1329）与余数补偿（cpp:1303-1328）在 3.1 可提为静态纯函数单独测；M1 不动。

### 3. core 的纯净性保障

**KDDW 事实**：

- **KDDW core 并非绝对零 GUI**。grep 实证：`src/core/DragController.cpp:30-37` 在 `#ifdef KDDW_FRONTEND_QTWIDGETS` 内 include `<QWidget>/<QApplication>`；`src/core/FloatingWindow.cpp:40-46` 在 `#ifdef KDDW_FRONTEND_QT + Q_OS_WIN` 内 include `<QGuiApplication>`；`src/core/WidgetResizeHandler.cpp:29-40` 同类。纯净性靠**前端宏**（`target_compile_definitions(kddockwidgets PUBLIC KDDW_FRONTEND_QTWIDGETS KDDW_FRONTEND_QT)`，src/CMakeLists.txt:492/498/509）在编译期裁剪，而非目录级绝对禁令。
- 但 **`src/core/layouting/` 子目录是事实上的零 GUI 净室**：全部 include 仅为 docks_export/KDDockWidgets.h/kdbindings/nlohmann/std 头 + `core/Platform_p.h` + `<QTimer>`（QtCore）+ `<iostream>`（Item.cpp:13-30）；唯一 Platform 触点在 `checkSanity()` 的析构期守卫（Item.cpp:1114-1122），且被 `#ifdef KDDW_FRONTEND_QT` 包裹。
- **"平台数值"进引擎的三条通道**（对照 SARibbon 的 QStyle::pixelMetric 问题）：
  1. Config 门面 → 引擎静态量（separatorThickness/min/max，Config.cpp:156-174, 235-256 → Item_p.h:211-213）；
  2. 抽象类：core 自带 `Screen` 抽象（含 `devicePixelRatio()` 纯虚，Screen_p.h:41）与 `Platform::screenSizeFor/primaryScreen`（Platform.h:80,153），前端实现；
  3. 函数指针注入（CreateSeparatorFunc/DumpScreenInfoFunc，Item_p.h:36-37）。
  **KDDW core 全程无 QStyle**——它没有 style 概念，所有"平台像素值"都由前端算好经上述通道变成纯数据。这实证了 02 S3.0 的处理（适配器采集 `style()->pixelMetric(...)` 填 Metrics 字段）与 KDDW 同构且更简单（SARibbon 用实例字段，连静态量时序约束都没有）。
- 导出宏组织（src/docks_export.h:14-60）：Qt 前端 `BUILDING_DOCKS_LIBRARY → Q_DECL_EXPORT / 否则 Q_DECL_IMPORT`；`KDDOCKWIDGETS_STATICLIB` → 全部置空；非 Qt 前端用 CMake 生成头；另有 `DOCKS_EXPORT_FOR_UNIT_TESTS`（`DOCKS_DEVELOPER_MODE` 门控，:24-28）供测试访问内部符号。与 SARibbon 现行 `SA_RIBBON_EXPORT`/`SA_RIBBON_BAR_NO_EXPORT`（静态库）机制同型，无需新增；`*_FOR_UNIT_TESTS` 宏 SARibbon 不需要（ctest 正常链接导入库）。
- Logging（Logging_p.h:19-127）：spdlog 可选依赖；无 spdlog 时 trace/debug/info/warn 全为 no-op，仅 error 落到 qWarning（Qt 前端）或 no-op（Flutter）——**core 日志的下限是"错误必达、噪声全灭"**。另提供 `operator<<(ostream, Size/Rect/Vector<double>)`（:105-127），诊断输出打值不打地址。Logging.cpp 仅 12 行（注册 logger 名）。

**对照 02 S8-3 纯净门禁与 v2 §3.7 禁区清单的结论**：

- v2 §3.7 清单（QWidget/QLayout/QSS/QStyle/QQuickItem/QQml*/QApplication）**方向正确但不完整且有一处易误伤**：
  - 模块层扫描应补：`QStyledItemDelegate`、`<QtWidgets/`、`<QtQuick/` 模块化 include 路径形式（`#include <QtWidgets/QWidget>` 可绕过裸类名扫描）；`QScreen`/`primaryScreen` 属确定性层（仅禁入 layout/ 引擎文件，见下），不进模块层清单；
  - **易误伤点**：`QGuiApplication`/`QScreen`/`QFontMetrics` 属 **QtGui**，core 允许链接 Qt::Gui——S1/S2 已把 `isOperatingSystemInDarkMode()`（QGuiApplication::styleHints）等合法下沉。若把 QGuiApplication 一刀切进禁区会误杀；正确做法是**分两层门禁**（此为本轮对 02 S8-3 的核心修订）：
    - **模块层（硬门，CI 扫描 + 链接检查）**：禁 QtWidgets/QtQuick 的头与链接。扫描目标：QWidget、QApplication、QStyle、QLayout*、QQuickItem、QQml*、`<QtWidgets/`、`<QtQuick/`；
    - **确定性层（引擎专属，评审门 + 定向 grep）**：`src/core/layout/` 内不得调用 QGuiApplication 动态状态（layoutDirection/styleHints/primaryScreen/screen 遍历）——引擎要确定性可测，这些值只能经 Input/Metrics 进来。KDDW 佐证：layouting/ 对 Platform 的唯一触碰是析构期守卫且被宏隔离（Item.cpp:1114-1122）；DPI 经 Screen 抽象成数据（Screen_p.h:41）。global/theme 工具函数（saIsRTL core 版、isOperatingSystemInDarkMode）不受此层约束（它们本来就是"采集器"）。
- KDDW 的前端宏裁剪模式（core 内 `#ifdef KDDW_FRONTEND_QTWIDGETS` 包 widget 代码）SARibbon **不应抄**：SARibbon core 是独立库、物理隔离比宏隔离强，02 现行方案（链接级分离 + 扫描）已优于 KDDW。此结论写进 findings 即可，02 不改。

### 4. 单例与生命周期

**KDDW 事实**（三个单例三种模式，无一挂 QCoreApplication，无一用 Q_GLOBAL_STATIC）：

| 单例 | 实现 | 证据 |
|---|---|---|
| `Config` | Meyers 函数内静态**对象**：`static Config& self() { static Config config; return config; }`，非 QObject，PIMPL | Config.cpp:76-78；Config.h:63-70,382-387 |
| `Platform` | 裸静态指针 `static Platform* s_platform`，**构造函数自注册** + `assert(!s_platform)` 防双例，析构自注销；`instance()` 带"仅一个前端时自动初始化"的便利逻辑；测试收尾显式 `delete` | Platform.cpp:47-71, 84-100, 232-236 |
| `DockRegistry` | 函数内 `static ObjectGuard<DockRegistry>` + 懒 new；QObject（无 parent）；**空时自删**（`maybeDelete()`，"just to make LSAN happy"），源码注释明言"**please don't change this to be deleted at static dtor time with Q_GLOBAL_STATIC**" | DockRegistry.cpp:80-87, 307-316 |

- 析构顺序处理：DockRegistry 靠 ObjectGuard（=QPointer）容忍对象先亡（DockRegistry.cpp:309）；引擎侧 checkSanity 对"Platform 已析构"做守卫（Item.cpp:1115-1122）；Config 静态对象在 static-dtor 期析构，其 Private 析构 `delete m_viewFactory`（Config.cpp:45-48）。**单例间有隐式依赖序**：Config::Private 构造即调 `Platform::instance()->createDefaultViewFactory()`（Config.cpp:40-43）——Config 首次使用要求 Platform 已存在，KDDW 靠"Platform 在前端初始化时先建"保证；SARibbon ThemeData 无此类跨单例依赖，选型时保持这一点（不得在 ThemeData ctor 里触碰其他单例）。
- 线程：三单例均主线程假定，无锁；Config 文档要求启动期设置（Config.h:60-62）。

**对 02 S2.1-5 ThemeData 单例选型的决策建议（已写入 02）**：

**决策：Meyers 函数内静态对象**（`static SARibbonThemeData s_themeData; return &s_themeData;`），不挂 QCoreApplication，不用 Q_GLOBAL_STATIC。理由：
1. KDDW 三个单例全部避开 Q_GLOBAL_STATIC 与 app 父子挂载，DockRegistry.cpp:82-84 甚至留下针对 Q_GLOBAL_STATIC 的显式反面注释——业界成熟项目实证过这两条路的坑（静态析构期顺序不可控 / LSAN 报告）；
2. 挂 `QCoreApplication::instance()` 有硬伤：`instance()` 可能在 app 创建前被调（单测、静态初始化期），此时 parent 为 null 且**永不补挂**，行为随调用时序漂移；
3. ThemeData 需要的恰是 Config 式语义：进程唯一、启动即可用、析构在静态期但**析构函数不发射信号、不触碰 QCoreApplication**（写入 02 的注意事项）；
4. C++11 magic static 保证线程安全初始化；对象跨线程使用仍需主线程亲和（信号槽连接自动按接收者线程投递，QObject 析构自动断连——widgets 监听者先亡无风险）。
- 与 KDDW 的刻意差异：不需要 DockRegistry 式"空时自删"（ThemeData 常驻且极小）；不需要 Platform 式前端注入（SARibbon 无跨 GUI 框架前端）。

### 5. tests_* 抽象与无 GUI 测试

**KDDW 事实**：

- `Platform.h` 的 tests_* 族分两个宏门：`DOCKS_DEVELOPER_MODE`（:197-244）——`tests_wait/tests_waitForResize/tests_waitForDeleted/tests_waitForWindowActive/tests_waitForEvent/tests_doubleClickOn/tests_pressOn/tests_releaseOn/tests_mouseMove/tests_createWindow/tests_initPlatform/tests_deinitPlatform` + 成员 `m_expectedWarning`（:243）；`DOCKS_TESTING_METHODS`（:246-297）——`tests_createView(CreateViewOptions)/tests_createFocusableView/tests_createNonClosableView/installMessageHandler/pauseForDebugger`，配套 `SetExpectedWarning` RAII（:315-329）与 `CreateViewOptions` 结构（:337-360，**isVisible/sizeHint/minSize/maxSize/size 全显式数值**——测试视图的尺寸约束是喂进去的，不来自平台）。
- **fatal_logger 机制**（tests/fatal_logger.h:19-32、fatal_logger.cpp:21-58）：自定义 spdlog sink，任何 ≥err 级日志直接 `std::terminate()`——政策注释原话 "KDDW should be error free"（fatal_logger.h:20-21）；唯一豁免通道是 `m_expectedWarning` 白名单（子串匹配则降级为 warn，fatal_logger.cpp:29-37）；`main()` 第一件事安装（tests_main.h:40-42）。
- **引擎级测试范本 `tests/tst_multisplitter.cpp`**（2229 行，55 个用例）：
  - `Guest` 假实现（:35-101）：包一个 `Platform::tests_createView(CreateViewOptions)` 造出的测试 View，逐项转发契约方法；
  - 断言风格：**关系不变量为主**（`CHECK(item3->x() > item2->x())` :284、`CHECK_EQ(container3->height(), item3->height() + st + item31->height())` :364），绝对值断言只针对显式喂入的常量；
  - **每个操作后 `CHECK(root->checkSanity())`**，且 checkSanity 失败前自动 `root()->dumpLayout()` 打全树诊断（Item.cpp:738-741, 746-747, 760-763；dumpLayout 打 geometry/min/max/hidden/objectName，:821-842）；
  - **每个用例做序列化往返**（`serializeDeserializeTest` :115-135：to_json → 新根 fillFromJson → checkSanity）；
  - 期望告警用 RAII 白名单（`SetExpectedWarning w("Size constraints not honoured")` :2158）；引擎开关用 `ScopedValueRollback`（:1966）；用例注释带 ASCII 布局图（:1071-1072, 1538-1541）；
  - 测试注册是 `std::vector<KDDWTest>` + 共享 `tests_main.h`（:2170-2228），非 QTest——为 Flutter 协程事件循环设计。
- **fixture 形态**：`tests/layouts/*.json` 是**真实录制的布局序列化文件**，文件名即 bug 场景（stuck-separator.json、overlapping-item.json、layoutEquallyCrash.json、invalid*.json 族）；回放方式 `LayoutSaver::restoreFromFile(resourceFileName("layouts/stuck-separator.json"))` 后断言引擎状态（tst_docks.cpp:3464-3485）；Qt 前端经 qrc 访问、非 Qt 前端经 `KDDW_SRC_DIR` 编译期路径（tests/utils.h:301-308、tests/CMakeLists.txt:96）。invalid 族专门测**畸形输入的优雅处理**。
- **组织方式**：tests/CMakeLists.txt 每次构建只注册当前前端的测试可执行文件（`add_kddw_test` :82-107，条件注册 qtwidgets/qtquick 主套件 :54-75）；"同一套测试跑双前端"靠 **CI 按前端各构建一遍**，不是运行期切换。tests/core/tst_*.cpp 是 controller 级测试（仍需前端），真正的引擎测试是 tst_multisplitter（仍需 tests_createView 造真视图）。
- **RTL/DPI**：`grep RightToLeft|isRTL|layoutDirection src/core/` **零命中**——KDDW 引擎完全无 RTL 概念；layouting 与引擎测试中无 devicePixelRatio 触碰（DPI 只在 Screen 抽象与 WidgetResizeHandler 出现）。**SARibbon 的 RTL 需求无 KDDW 先例可抄，02 自行设计的 RTL 入参化 + RTL/LTR 双份 fixture（S6-2）是唯一路径，本轮确认其必要性。**

**对照 02 S5.0/S8 的差距清单（FakeLayoutItem 方案还缺什么）**：

1. **失败诊断输出**（KDDW：checkSanity 失败自动 dumpLayout）：SARibbon 黄金测试失败时 QCOMPARE 只打单值。补：tests/core 提供 dump helper，首个断言失败即打印整个 Result + 全部 item 的输入/输出字段（契约字段全 public，测试侧可直接打印，core 无需日志设施）——已写入 02 S5.0-2。
2. **退化输入 fixture 族**（KDDW：invalid*.json）：补空 items、零宽/负宽 availableRect、全隐藏项、min 总和超可用宽（溢出）、maxWidth<min 的矛盾输入等场景，先对 2.9.5 录制"不崩溃 + 实际行为"作黄金值——已写入 02 S5.0-2。
3. **关系不变量断言**（KDDW：x 序关系、尺寸加和恒等式）：作为绝对黄金值的补充（黄金值管"与 2.x 一致"，不变量管"跨平台/跨字体也恒真"）——已写入 02 S8-1。
4. **dpr 变体 fixture**：Metrics 已含 devicePixelRatio 输入（02 S3.1），fixture 加 dpr=1.0/2.0 两份断言取整确定性——已写入 02 S5.0-2。
5. **错误零容忍政策的轻量版**（KDDW：fatal_logger）：SARibbon core 无日志框架，等价物 = 引擎测试中若 2.x 代码路径产生 qWarning 应视为失败信号；M1 不强上（core 不打日志即无此问题），在 NOTES.md 记录该政策供 3.1 评估——写入 findings 设计级建议。
6. **fixture 序列化格式**：KDDW 用 JSON 存引擎状态（可 diff、可重录、可回放畸形数据）。02 S5.0-2 现定为 C++ 结构体 layout_fixtures.h。**不推翻**（C++ 结构体编译期类型安全、录制工具 dump_panel_geometry.py 已按此设计），但建议：dump 工具产出的 JSON 保留在 `tests/core/fixtures/recorded/*.json` 作为黄金值的**可审查源**，layout_fixtures.h 由脚本从 JSON 生成或人工誊录时对账——JSON 是"录制介质"，C++ 是"回放介质"。列为设计级建议供整合者裁决。

### 6. ViewInterface 族的接口粒度

**KDDW 事实**：

- 每类 controller 配一个窄接口，规模（虚函数计数，含默认实现）：DockWidgetViewInterface 2、SideBarViewInterface 3、MainWindowViewInterface 4、StackViewInterface 4、TitleBarViewInterface 4、GroupViewInterface 5（仅 `nonContentsHeight()` 纯虚，其余带默认，GroupViewInterface.h:29-41）、TabBarViewInterface 11、ClassicIndicatorWindowViewInterface 11。
- 形态约定（以 TabBarViewInterface.h:30-60 为范）：
  - 构造函数收 controller 指针并存 protected const 成员（:33, :57 `TabBar* const m_tabBar`）；
  - 可选能力给默认实现（:36-38 `setTabsAreMovable` 注释"目前仅 QtWidgets 前端支持"）；
  - **测试专用方法进接口但注明**（:40-42 `text(int)` 注释 "This is only used by tests"）；
  - 禁拷贝（:58-59）。
- Controller 消费方式：controller 持通用 `View`（god-interface，约 60 纯虚——v2 §2.3 已明确不采用），用时 `dynamic_cast<Core::TabBarViewInterface*>(view())` 再调窄接口（TabBar.cpp:70, 102, 134, 158, 276, 282, 290, 335, 344）；Controller 基类只做 view 生命周期与几何转发（Controller.h:48-103，Controller.cpp:34-35, 55-131）。
- 视图创建统一走 `ViewFactory` 纯虚族（ViewFactory.h:68-136：createGroup/createTitleBar/createStack/createTabBar/createSeparator/…/createAction），Config::setViewFactory 可整体替换（Config.h:225-228）——core 定义接口、前端实现、用户可换。

**对照 SARibbon（Tier2 预案，写入 02 附录 F）**：SARibbon 3.0 只给布局引擎用契约（engine↔前端），KDDW 窄接口给 controller↔view。若 3.1 做结构控制器（D7 gate 触发），接口应按 KDDW 形态：每 controller 一个 `SARibbon*ViewInterface`（只含 controller 需要命令 view 的方法）、ctor 存 controller const 指针、可选能力默认实现、测试专用方法注明、创建走 `SARibbonElementFactoryInterface`（与 S4.3 占位接口衔接——KDDW ViewFactory 实证了"factory 接口放 core、实现留前端"正是 S4.3 降级路径的完全体）。细节见 02 附录 F。

### 7. Action/DelayedCall 等 core 基础设施的成本实证

**KDDW 事实（量化）**：

- `Core::Action`：接口面 10 个纯虚 + 2 个便利函数（Action.h:24-61，65 行）+ Action.cpp 36 行 + Action_p.h 41 行（PIMPL 内藏 KDBindings `toggled` 信号，Action_p.h:26-38）= **core 侧 142 行**；前端实现 qtwidgets 150 行（Action.cpp 91 + Action_p.h 59）、qtquick 59 行、flutter 55 行 = **前端侧 264 行**；合计 **约 406 行**，还要占 ViewFactory 一个纯虚槽位（ViewFactory.h:136 `createAction`）。
- **收益面**：core 内消费者只有 DockWidget 的 `toggleAction()/floatAction()` 两处（DockWidget.h:151, 157；grep core/*.cpp 仅 DockWidget.cpp 命中 Action）。**约 400 行基础设施服务 2 个用例**——因为它同时服务 3 个前端（含无 QAction 的 Flutter）。
- `DelayedCall`：抽象基类 `call()` + 两个具体延迟操作（DelayedCall_p.h:22-57，59 行 + .cpp 58 行 ≈ 117 行），经 `Platform::runDelayed(ms, DelayedCall*)` 派发（Platform.h:113-115）——存在的唯一理由是 Flutter 没有 QTimer。
- **对 SARibbon D8 的含义**（写入 02 S4.3 引用 + findings 设计级建议）：KDDW 付这 400 行是因为 QAction 在 Flutter 前端不存在。SARibbon 双前端（widgets/qml）**都在 Qt 内**，Qt6 QAction 属 QtGui、Qt5 属 QtWidgets——只有"core 需要持有 action 语义"时才需要抽象，而 M1 的引擎/主题/度量都不需要。若 3.2 Gallery 下沉触发 D8，按 KDDW 单价估算：SARibbon 的 action 面（text/icon/enabled/checked/shortcut/QAction 属性系统/`_sa_RowProportion` 动态属性/ActionsManager 分组语义）远大于 KDDW 的 10 个纯虚，**成本下限即 400+ 行 × 消费面放大**，且 Qt5 兼容还要双轨（Qt5 侧 QAction 在 QtWidgets，core 不能 include）。D8"推迟"决策获得量化支撑。
- DelayedCall 对 SARibbon **零必要**：双前端都有 QTimer::singleShot。原则记录：Qt 已提供的能力不做二次抽象（与 v2 §2.3 不采用 Platform 单例同一逻辑）。

### 附：单库拼合与前端变量（背景项）

src/CMakeLists.txt：`KDDW_FRONTEND_QTWIDGETS_SRCS`（:85-109）/`KDDW_FRONTEND_QTQUICK_SRCS`（:132+）/`KDDW_FRONTEND_FLUTTER_SRCS`（:221+）三组源列表，按前端条件拼进 `DOCKSLIBS_SRCS`（:308, :316），最终**单一 target** `add_library(kddockwidgets ...)`（:327）+ 前端宏 PUBLIC 传播（:492-509）。v2 §2.3 已决策 SARibbon 不采用（三库保留，amalgamate/Python 绑定依赖库边界），本轮读后**维持该决策**：KDDW 单库的代价是 core 内必须散布前端宏 ifdef（第 3 项），SARibbon 三库物理隔离更干净。无需改 02。

---

## 二、对 02-core-sinking.md 的修改清单

| # | 位置 | 修改 | 依据（KDDW 证据 / SARibbon 核实） |
|---|---|---|---|
| 1 | S2.1-5 | 悬而未决的单例选型**定案**：Meyers 函数内静态对象 + 代码示例 + 析构/线程注意事项；记录对 Q_GLOBAL_STATIC 与挂 QCoreApplication 两案的否决理由 | Config.cpp:76-78；DockRegistry.cpp:80-87（反 Q_GLOBAL_STATIC 注释）;Platform.cpp:47-71 |
| 2 | S4.1 新增第 5 条 | "对照 KDDW 的设计说明"附注 + **最终契约接口完整代码块**：定名 `resultGeometry`；新增 `debugName()` 默认虚；契约字段 `rowIndex` 定为 int 并记录与 2.x `short` 的类型差异及验证命令；identity/setVisible/无 id() 三项设计理由 | LayoutingGuest_p.h:31-70；Item.cpp:229-234,712-725,884-898；SARibbonPanelItem.h:51（short）；SARibbonPanelLayout.cpp:302-303 |
| 3 | S3.0 末 | 补一句 KDDW 同构实证：Config 门面→引擎静态量模式对照 Metrics 实例字段模式（SARibbon 更优：多窗口隔离、无启动时序约束） | Config.cpp:156-174,235-256→Item_p.h:211-213 |
| 4 | S5.0-2 | fixture 补四项：失败诊断 dump helper、退化输入 fixture 族、dpr=1.0/2.0 变体、录制 JSON 保留为可审查源（fixtures/recorded/） | Item.cpp:738-741（失败自动 dumpLayout）；tests/layouts/invalid*.json + tst_docks.cpp:3464-3485；Screen_p.h:41 |
| 5 | S5.1-2、S5.2-1、附录 C takeAt 行 | 补引擎缓存生命周期不变量：takeAt 必须调引擎 `removeFromCache(item)`（2.x 现行为 cpp:302-303 注释 "Remove from cache to prevent stale entries"），invalidate() 调 `clearCache()`（cpp:367-368）；防纯 move 丢副作用 | SARibbonPanelLayout.cpp:285-306,356-370；对照 KDDW ObjectGuard/beingDestroyed 重方案（Item.cpp:4122-4147）——SARibbon 用适配器不变量替代 |
| 6 | S5.2-2 末 | 补"M1 禁止顺手结构体化"注记：KDDW SizingInfo 式显式中间态列为 3.1 优化项；Category 的 SizeHintCollection 是现成半显式中间态，搬移时保留结构体形态 | Item_p.h:93-188；Item.cpp:3021-3067；SARibbonCategoryLayout.cpp:30-35 |
| 7 | S8-1 | 覆盖矩阵补"关系不变量断言"（补绝对黄金值）；补退化输入族与 dpr 变体入矩阵 | tst_multisplitter.cpp:284-285,352-364（关系断言）;invalid*.json |
| 8 | S8-3 | 纯净门禁**分两层**：模块层（硬门，扫描清单扩充 `<QtWidgets/`、`<QtQuick/`、QStyledItemDelegate 等；明确 QGuiApplication/QScreen/QFontMetrics 属 QtGui 不在禁区）+ 确定性层（layout/ 引擎禁调 QGuiApplication 动态状态，定向 grep 入 CI） | DragController.cpp:30-37（KDDW 宏裁剪反例）；Item.cpp:1114-1122（layouting 唯一 Platform 触点被隔离）；Screen_p.h:41（DPI 数据化） |
| 9 | S4.3 | 补 KDDW Action 成本量化一句（core 142 行 + 前端 264 行 ≈ 406 行服务 2 个消费点），支撑 D8 推迟 | Action.h/.cpp/Action_p.h、qtwidgets/qtquick/flutter Action 实现行数；DockWidget.h:151,157 |
| 10 | 新增附录 F | Tier2 结构控制器接口预案（标注 3.1 才生效）：窄接口形态五约定 + SARibbon 示例接口 + 与 S4.3 factory 衔接 | TabBarViewInterface.h:30-60；TabBar.cpp:70,158；ViewFactory.h:68-136 |
| 11 | S10-1 | dev-guide 增补一条：仿 KDDW layouting/examples 的 FakeItem 最小示例入开发指引 | src/core/layouting/examples/qtwidgets/main.cpp:14-30 |
| 12 | §8 已知偏差 | 追加轮2偏差记录：rowIndex short/int 类型差异（轮1"同名同型"表述不准确） | SARibbonPanelItem.h:51 vs v2 §3.4.1 草案 |

## 三、设计级建议（供整合者裁决是否上达 v2 计划）

1. **v2 §3.4.1 契约草案增补 `debugName()`**（默认空实现）：KDDW 实证诊断名是引擎可测性的一等公民（Item.cpp:884-898 用它做 objectName，dumpLayout 打名字）。改动极小，建议直接进 v2 契约代码块。
2. **v2 §3.7 禁区清单改为两层表述**（模块纯净 vs 引擎确定性），并明确 QtGui 头（QGuiApplication/QScreen/QFontMetrics/QColor）合法：现清单"禁止 include QSS/QStyle"未区分层次，执行 agent 可能把 S1/S2 合法下沉的 QGuiApplication 依赖误判为违规，或反过来把引擎里偷调 `QGuiApplication::layoutDirection()` 放过。这是**门禁语义问题**，宜在 v2 层面定调。
3. **黄金 fixture 的录制介质 JSON 化**（回放仍用 C++ 结构体）：dump_panel_geometry 工具产出的 JSON 入库（tests/core/fixtures/recorded/），黄金值可 diff 审查、可重录对账、畸形场景可手工构造（KDDW tests/layouts 全套实践）。若采纳，02 S5.0-3 的工具输出即正式交付物而非临时件。
4. **"错误零容忍"测试政策**（fatal_logger 的轻量版）：3.1 起若 core 引入任何日志/警告输出，引擎测试应把非预期 qWarning 视为失败（KDDW: fatal_logger.cpp:21-45 "KDDW should be error free"）。M1 core 无日志，暂零成本，建议记入 v2 §7 作前瞻条款。
5. **rowIndex 类型定案**：契约用 `int`（v2 草案原值），2.x `short` 差异记 NOTES.md；若整合者认为必须严格同型，改契约为 short 亦可（引擎内部计算全是 int，写回时窄化），二选一后同步 v2 §3.4.1。
6. **3.1 引擎优化项立项建议**：Panel 引擎引入 SizingInfo 式显式中间态结构体（收敛 columMaxWidth 等散量）+ 加权分配段提为纯函数（对照 KDDW calculateSqueezes，Item.cpp:3069-3136）。M1 严禁执行，防止纯 move 纪律被"顺手优化"破坏。
7. **维持 v2 §2.3 两项"不采用"**：单库拼合（KDDW 代价=core 内前端宏散布，DragController.cpp:30-37 实证）与 Platform 单例（SARibbon 无跨 GUI 框架前端；DelayedCall/Screen 抽象的存在理由在 SARibbon 均不成立）。本轮深读后无翻案证据。

## 四、未解决/待核实

1. **KDDW 2.0.1 与最新 master 的差异**：本机副本停在 2023-12（f041992a）。KDDW 后续版本若对 LayoutingGuest 契约有增删（如 id() 语义变化），本文结论按 2.0.1 负责。核实路径：github.com/KDAB/KDDockWidgets master 分支 src/core/layouting/ 对 diff。
2. **`QFontMetrics` 在无 QGuiApplication 实例时的行为**：ThemeData/Metrics 若在 app 创建前被调用（Meyers 静态初始化时机），QFontMetrics 依赖默认 QFont——Qt 文档允许无 app 实例构造 QFont/QFontMetrics（返回预置默认值），但 S2/S3 执行时应加一个"无 QApplication 单测"验证 core 库可独立初始化（tests/core 本来就不建 QApplication，天然覆盖，执行时确认不 segfault 即可）。
3. ~~**short→int 的取址用法全仓验证**~~：**本轮已跑并关闭**——`git grep -n "rowIndex" -- src/ tests/ example/ sip/ pyside6/ pyqt6/` 全部命中为赋值/读取（`item->rowIndex = 0` 形态，SARibbon.cpp 合并文件内 14 处 + 声明 2 处），无任何 `short*`/`short&` 取址绑定；sip/pyside6 目录 grep short+rowIndex 零命中（绑定未暴露该字段的 short 类型）。int 定案无损。执行 S4.1 时按 02 中命令复核一次即可。
4. **KDDW examples/qtwidgets（layouting 独立复用示例）未逐行读**：仅核实其 include 与类声明形态（main.cpp:14-30，QWidget 直接子类化三个 layouting 接口）。其 CMakeLists 显示示例独立编译链接 kddockwidgets——如需"引擎可被外部项目复用"的更强证据可读它，对 02 无进一步修订影响。
