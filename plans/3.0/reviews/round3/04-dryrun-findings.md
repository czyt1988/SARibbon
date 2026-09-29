# 计划 04 第3轮评审 findings（视角＝首次执行者压力测试 / dry-run）

> 评审对象：`plans/3.0/04-qml-and-release.md`（第2轮修订后版本）
> 评审日期：2026-09-29
> 角色设定：假设本 agent 就是将被派去执行计划 04 的执行者，只有 04 文档 + `plans/3.0/README.md` 通用规则 + 仓库访问权（并假设计划 01/02/03 已执行完）。对文档做完整 dry-run，找出会卡住/猜错/做错的地方并**直接修订 04**（本轮独立于前两轮，以当前文档 + 仓库现实 + 本机 Qt 头/源码为准自行验证；round1/round2 仅用于理解意图）。
> 核实手段：本机 Qt 头文件与 **Qt 源码**（`D:/Qt/6.7.3/msvc2019_64/include` + `D:/Qt/6.7.3/Src/qtdeclarative`、`D:/Qt/5.14.2/msvc2017_64/include` + `D:/Qt/5.14.2/Src/qtdeclarative`）、参考项目真实代码（`F:/src/3rdparty/KDDockWidgets`、`F:/src/3rdparty/qwindowkit`）、SARibbon 仓库现状（版本文件/workflow/readme/changlog/tag/头文件行号）、跨计划文档（01/02/03/README/NOTES/v2）。
> 处理结果：除标注"供终审 agent 对齐"（04 无法改 01/02/03/README）外，全部已直接改进 04 文档正文。

---

## 一、逐条发现（位置 | 缺陷级别 | 现象与证据 | 处理）

### 阻塞级（3 条）

**#1 | S1.3 代码块 + S1.3"所有权坑" + S2.1 + S2 验证 + S7 骨架 + 风险表 | 阻塞**
- **现象与证据**：原文用 `qmlRegisterSingletonInstance` 注册 RibbonTheme/RibbonMetrics，并声称"注册的单例对象默认归引擎所有（引擎析构即 delete），注册前设 CppOwnership，否则多引擎第二次即悬空"，据此设计"连续建两个引擎重复断言"（S2 验证）与"每个测试用例各自 `QQmlEngine engine;`"（S7 骨架）。**对本机 Qt 源码实测：这两条论断都错，且多引擎策略在 instance 式下必崩**：
  - `qmlRegisterSingletonInstance` 内部用 `SingletonInstanceFunctor`，**首次访问即把实例硬绑到该引擎**（`m_engine = qeng`）并**自动** `setObjectOwnership(CppOwnership)`；第二个不同引擎访问直接 `return nullptr` + 告警 `"Singleton registered by registerSingletonInstance must only be accessed from one engine"`。证据：6.7.3 `Src/qtdeclarative/src/qml/qml/qqml.cpp:397/402-418`（`SingletonInstanceFunctor::operator()` 在 :402，CppOwnership 在 :409，告警在 :412）+ `qqmlprivate.h:788-798`（`m_engine` 是**裸指针**，注释明言"Not a QPointer, so that you cannot assign it to a different engine when the first one is deleted"）；5.14.2 `qqml.cpp:80-104`（`RegisterSingletonFunctor` 的 `alreadyCalled` 守卫 + :102 自动 CppOwnership，行为同款）。
  - 后果：①原"所有权坑"方向反了——instance 式**自动**设 CppOwnership，真正的坑是**单引擎绑定**；②S2"连续建两个引擎"、S7 每个用例各建引擎，在 instance 式下**第二个引擎拿 nullptr**，叶子主题绑定失效、断言崩，属**执行必挂**的阻塞。
- **处理（已改 04）**：S1.3 代码块、S1.3"单例注册坑"、S2.1、S2 验证、风险表全部改为**回调式 `qmlRegisterSingletonType<T>(uri,maj,min,name,callback)`**（callback 内 `return instance()` 并 `setObjectOwnership(CppOwnership)`）。回调式按引擎逐个调用（6.7.3 `qqmlengine.cpp:1834-1835` `siinfo->qobjectCallback(q,q)`）、**无单引擎绑定**，支持多引擎；但回调式**不自动设所有权**（无父默认 JavaScriptOwnership，引擎析构会 GC 掉 core 单例），故 CppOwnership 必须在 callback 内设——这才是原"所有权坑"的真正落点。两版回调式签名齐备（`std::function<QObject*(QQmlEngine*,QJSEngine*)>`：6.7.3 qqml.h:701/706、5.14.2 :645/:663）。S2 验证"连续建两个引擎"改写为"能成立的前提就是用回调式"，兼作"未误用 instance 式"的回归哨兵。

**#2 | S10.4 + S10.5 | 阻塞**
- **现象与证据**：原文称"计划 03 S4 恢复为 **tag `v3.0.*`** 触发""打 tag `v3.0.0` 并 push → publish workflow 自动跑"，S10.5 又把"触发器仍是旧版 `release:published`"当作"03 S4 未生效"的异常分支。**与 03 S4.1 终态直接冲突**：03 S4.1 原文明确"**还原 `release: types:[published]`**"，且"若改用 `push: tags:['v3.0.*']` 属新增行为而非'恢复'……**默认不采用**"。现库 `publish-python-bindings.yml:3-6` 实测 = `release: types:[published]` + `workflow_dispatch`（无 tag 触发）。→ 在真实终态下 **push tag 根本不触发 publish**，触发 publish 的是"创建 GitHub Release"（S10.5）；S10.5 的条件判断也反了（release:published 正是 03 恢复的目标态，不是"未生效"）。另发现**01↔03 冲突**：01 S10.4 实为"**不改此文件**"（维持 release:published+dispatch），而 03 P4 与 04 早稿都称"01 暂停为仅 workflow_dispatch"——三处对触发器演变链的说法不自洽。
- **处理（已改 04）**：S10.4 重写为"发布触发链核对"——写清终态触发器 = `release:[published]`，**触发 publish 的是创建 Release 而非 push tag**，tag 只是 Release 指向的 ref；不依赖任何声称的演变链，一律以运行时 `grep -A5 "^on:"` 实测为准，给出三分支决策表（release:published / 仅 dispatch / 含 push:tags）。S10.5 顺序警告按 release:published 终态改写：`gh release create v3.0.0` 会立即触发 publish→真实上传 PyPI，agent 只备料、由维护者确认后再建 Release。01↔03 表述冲突**显式标注供终审 agent 对齐**（04 无法改 01/03）。

**#3 | 验收门 §6 铁律判据 2 | 阻塞**
- **现象与证据**：判据 2 `git grep -nE "SARibbonPanelLayout|SARibbonCategoryLayout|SARibbonBarLayout" src/qml/` **为空**（widgets 布局类名不得出现）。但 core 引擎实名 `SARibbonPanelLayoutEngine`（02 S5.2）/`SARibbonCategoryLayoutEngine`（02 S6）**含 `SARibbonPanelLayout`/`SARibbonCategoryLayout` 子串**，而 QML 宿主**合法调用**它们（04 S3 操作1 `SARibbonPanelLayoutEngine::layout(...)`、S4.2 `SARibbonCategoryLayoutEngine`）。→ 正确实现的 src/qml 里 grep **必然非空**，"为空"永远不成立，执行者会被自己的验收门卡死。（`SARibbonBarGeometryEngine` 不含 `SARibbonBarLayout` 子串，Bar 侧本无误报。）
- **处理（已改 04）**：判据 2 改为 `... src/qml/ | grep -v "Engine"` 为空，并加注理由（引擎名含布局类名子串、QML 合法调用）。不用 `\b` 是因 `git grep -E` 的 POSIX ERE 对 `\b` 支持不稳，`grep -v Engine` 管道最稳。判据 1（算法函数名）与判据 3（几何应用点列举）经核实成立：判据 1 的源出行号 `SARibbonPanelLayout.h:61/153/155`、`SARibbonCategoryLayout.h:74`、`SARibbonBarLayout.h:66` 逐行核实确为对应函数声明，且这些是 widgets 专属壳（core 引擎入口叫 `layout()`），qml 不应出现；判据 3 顺带把笔误的"契约 item 的 `geometry()`"改为 `resultGeometry`（与 02 S4.1 契约字段名一致）。

### 歧义级（4 条）

**#4 | S3.4 枚举暴露 | 歧义**
- **现象与证据**：S3.4 先举例"在 `RibbonToolButton` 内 `enum RowProportion {...}; Q_ENUM(RowProportion)`"（→ 用户写 `RibbonToolButton.Large`），后又说"跨类型共享枚举放 uncreatable 注册类 `Ribbon`……对应 v2 §5.1 的 `Ribbon.Large` 写法"。而 v2 §5.1 的 QML 示例正是 `rowProportion: Ribbon.Large`。同一枚举行（RowProportion）被同时说成放 RibbonToolButton 和放 Ribbon，执行者无法判断用户侧写 `Ribbon.Large` 还是 `RibbonToolButton.Large`、canonical Q_ENUM 放哪，直接影响公共 API 面 + 示例 + 测试 QML。
- **处理（已改 04）**：S3.4 增"归属规则"——凡以 `Ribbon.Xxx` 形式出现的枚举（RowProportion/Alignment/Theme/PanelLayoutMode 等）**统一挂 uncreatable 类 `Ribbon` 做镜像 Q_ENUM**，宿主 Q_PROPERTY 用 `Ribbon::RowProportion` 作类型（跨类枚举属性合法）；只有确属单一类型私有、用户不以 `Ribbon.` 访问的才挂宿主内。原"放进 RibbonToolButton"示例明确**作废**。

**#5 | S3 操作1 + S3 骨架 updatePolish | 歧义**
- **现象与证据**：①S3 操作1 把"QML 侧适配器实现契约接口，**或**直接让 RibbonToolButton 实现契约接口"留成开放二选一，无判据——但该决策决定 S3–S5 整个结构（多继承 vs 每 item 适配器对象、谁实现 sizeHint/isHidden/applyGeometry/rowProportion）。②S3 骨架/操作1 说宿主"逐项 setPosition+setSize"，却**从不点名** 02 S4.1 契约的 `resultGeometry`（几何从哪来）、`applyGeometry`（契约要求前端实现的纯虚）、`debugName`（诊断）——与 02 契约字段脱节。③骨架 `registerChildItem(RibbonToolButton*)` 参数写死按钮类型，P1 的 Separator/Line 异构子项进不来。
- **处理（已改 04）**：S3 操作1 补"契约项形态决策（round3 定案）"——按钮类子项**直接多继承** `RibbonToolButton : QQuickItem + SARibbon::Core::SARibbonAbstractLayoutItem`（与 widgets `SARibbonPanelItem : QWidgetItem + 契约` 同构，02 S5.1-1；契约基类非 QObject，多继承不引入 moc 二义），实现五纯虚 + `rowProportion`，`debugName()` 返回 objectName；独立适配器仅用于"纯 QML 叶子无 C++ 宿主"的子项（P1 leaf-only）。updatePolish 注释与操作1 补明引擎回写 `resultGeometry`→逐项 `applyGeometry(resultGeometry)`（QML 侧实现＝setPosition+setSize）。登记列表参数泛化为 `SARibbonAbstractLayoutItem*`。

**#6 | S3"QML 叶子组织规范"宿主配对 | 歧义**
- **现象与证据**：宿主配对既说"C++ `setProperty("panelCpp", this)` 注入"（骨架 ensureQmlItem），又说"Base 叶子首行 `readonly property QtObject panelCpp: parent.panelCpp`"（KDDW 经 Loader 的模式，parent 是 Loader）。SARibbon 明确"不引 Loader、parent 即宿主"，则宿主没有 panelCpp 属性，`parent.panelCpp` 取空——两种机制混用，执行者不知叶子根到底该 `property QtObject panelCpp`（被 C++ 注入）还是 `readonly ...: parent`。且任务要求的 **panelQmlItem 握手的 QML 侧配对代码（onXxxChanged 把自己赋回）全文缺失**，只在注释里引 KDDW `onTabBarCppChanged`。
- **处理（已改 04）**：宿主配对条重写为"C++ setProperty 注入 + 叶子把自己赋回"，明确无 Loader 下两种等价取法（①根声明可写 `property QtObject panelCpp` 由 C++ 注入；②`readonly property QtObject panelCpp: parent`），二选一勿混用，并指出 KDDW 的 `parent.panelCpp` 是其 Loader 转发模式、SARibbon 不照搬。补 **RibbonPanelBase.qml 握手代码块**（`property QtObject panelCpp: null` + `onPanelCppChanged: if (panelCpp) panelCpp.panelQmlItem = root` + 空守卫派生属性）。

**#7 | S2.2 RibbonMetrics 字体事件 | 歧义（论断不准）**
- **现象与证据**：S2.2 称在 qApp 上监听 `QEvent::ApplicationFontChange`"（与计划 02 S3.2 widgets 侧**同一事件源**）"。实测 widgets 侧用的是**各控件自身的 `QEvent::FontChange`**（`SARibbonBar.cpp:4112` 及 Category/Panel/ToolButton 各自 case，grep 确认），非应用级 `ApplicationFontChange`——两者是**不同事件**。QML metrics 单例无控件、只能收 `ApplicationFontChange`（`QGuiApplication::setFont` 触发），方案本身对，但"同一事件源"的说法会误导执行者去用够不到的 widget FontChange，且埋下一致性测试隐患（只改某控件字体时两端不同步）。
- **处理（已改 04）**：S2.2 更正为"这不是 widgets 侧的同一事件源"，写清 widgets＝per-widget FontChange、QML＝应用级 ApplicationFontChange，目的相同事件不同，一致性测试须设**应用字体**才能同时触发两端，记 NOTES。

### 缺口级（5 条）

**#8 | S5 RibbonToolButton sizeHint | 缺口（round1 §三.4 遗留未决）**
- **现象与证据**：S5 验证断言"sizeHint 三态与黄金值一致"，但 S5/S3 全文**未说 QML 按钮的契约 `sizeHint()` 从哪来**。round1 findings §三.4 已点此遗留项，第2轮未决。铁律禁止把文字测量写成 QML 第二实现，round1 提示"公式应源自 core metrics"。
- **处理（已改 04）**：S5 操作1 补"sizeHint 传导链（round3 补，锁定 round1 §三.4）"——契约 `sizeHint()` **必须在 C++ 侧由 core `SARibbonMetrics` 算出**（图标尺寸 + 字体度量公式），**不得取叶子 implicitWidth/Height 反推**（那是 QML 自报几何＝第二实现，违铁律）；完整链 core metrics→宿主 C++ sizeHint()→layout()→resultGeometry→applyGeometry；文字测量用 `QFontMetrics` 不用 QML `TextMetrics`；三态公式若 core metrics 未提供＝core 缺口，回计划 02 补。

**#9 | S7 一致性套件字体 | 缺口（round1 §三.3 遗留未决）**
- **现象与证据**：S7 操作2/3 构造**真实** widgets 控件与 QML 树，其 sizeHint 经 core metrics 由字体推导；要跨平台复现黄金几何，必须用固定部署字体。但 S7 全文**未提字体固定**，linux-qt6.8 的 QML CI job（操作6）会因系统字体差异得到不同 sizeHint→几何断言必挂。round1 §三.3 已点，未决。
- **处理（已改 04）**：S7 操作3 补"固定字体（round3 补，落地 round1 §三.3）"——两端复用 tests/core 固定部署字体（02 S5.0-3：`tests/core/fonts/` + `QFontDatabase::addApplicationFont` + `QFontInfo` 断言防 fallback），`RibbonConformance.h` 的 metrics 输入以该字体构造，录制/回放字体严格一致（v2 R5）。

**#10 | S10 仓库现实块 + S10.2 | 缺口（版本落点少计）**
- **现象与证据**：S10 现实块只列 `pyproject.toml:9`/`pyproject-pyqt6.toml:9`/`pyside6/pyproject.toml:11` 三个 pyproject，**漏了 `pyqt6/pyproject.toml:12`**（实测 `find . -name 'pyproject*.toml'` 得 **4 个**，均 2.8.0）；S10.2 称"版本号核对**四处**……pyproject×3"，与 03 S7.2 的**8 处**清单（含 `pyside6/CMakeLists.txt:3` project VERSION、`vcpkg.json:4`，均实测 2.8.0）不符，核对清单不完整会漏 bump。
- **处理（已改 04）**：S10 现实块补全 4 个 pyproject（含 pyqt6/pyproject.toml:12）+ `pyside6/CMakeLists.txt:3` + `vcpkg.json:4`，注明"8 处全部由 03 升 3.0.0，本步核对，清单以 03 S7.2 为准"。S10.2 改为"先复核 03 S7.2 全部 8 处 + 04 特有 2 处（QML module VERSION 3.0、readme badge 两处）"。

**#11 | S3 骨架 ensureQmlItem | 缺口**
- **现象与证据**：骨架 `ensureQmlItem()` 内 `QQmlComponent component(qmlEngine(this), leafUrl)`，注记说"测试纯手工建宿主时显式传 engine"，但 `ensureQmlItem()` **无 engine 参数**——S7 白盒路径（C++ 手工建宿主 `setParentItem(view.contentItem())`）下 `qmlEngine(this)` 在挂窗前后可能为空，签名接不住"显式传 engine"。
- **处理（已改 04）**：骨架签名改 `ensureQmlItem(QQmlEngine* engine = nullptr)`，体内 `QQmlEngine* e = engine ? engine : qmlEngine(this)`，注明测试白盒须显式传、QML 声明创建时 qmlEngine(this) 有效。

**#12 | 验收门 §6 首条同屏对比 | 缺口（判定不可度量）**
- **现象与证据**：验收门"`examples/qml/QmlMainWindowExample` 与 widgets 版同屏对比条高/间距/装箱一致（截图存档）"是纯目测判定，无客观基准、无截图规程，执行者难自证"一致"。
- **处理（已改 04）**：补"'一致'的客观基准＝下一条跨前端一致性套件对同一黄金几何双端断言相等，截图为佐证"；补截图规程（同固定字体/同主题/同窗口尺寸，各截三行/最小模式一张）。

### 瑕疵级（5 条）

**#13 | S8.1 | 瑕疵（CLI 口径 + 内部矛盾）**
- **现象与证据**：①S8 命令用 `python tools/check_core_purity.py`，与 01 S9 明定"调用名统一 `python3`（Linux CI runner 无 `python`）"及 01 CI step/验收门（01:778/883 均 `python3`）不一致；且 README R2（:81）与 02（P5/S8-3/验收门）却写 `python`——跨文档不一致。②S8.1 一边标"a（推荐，**零改脚本**）"，一边又说"词法级'类型名出现'扫描同理**需要 qml 白名单**"——若词法层要改就不是零改脚本，自相矛盾。
- **处理（已改 04）**：S8 命令统一 `python3`，注明依据 01 S9；README/02 的 `python` 标为跨文档待对齐项供终审统一（04 不改它们）。"零改脚本"改为"零改脚本**前提见下**"并讲清：`--forbid-include` 只换 include 扫描清单（换成 qml 清单即自动放行 QQuickItem/QQmlEngine/QQuickPaintedItem）；01 S9 的词法清单（qApp/QApplication::/QWidget/QLayout）恰是 qml 也该禁的子集、且不含 QQuickItem/QQmlEngine/QGuiApplication，**不误伤** qml——故若词法清单硬编码且 qml-safe 则 a 真零改，否则退化为 b，以 01 S9 脚本实际实现为准记 NOTES。

**#14 | §4 全程纪律 | 瑕疵**
- **现象与证据**：§4 写"README.md **R1–R4** 适用"，但 04 大量用 **R5 术语**（Step A/B、黄金几何测试、契约接口、适配器、命令式单轨均出自 R5 表），S10.6 依赖 **R6 双分支**；01/02/03 §4 均写"R1–R6"。
- **处理（已改 04）**：改为"R1–R6 全部适用"并点名 R5/R6 的用处，与其他三计划对齐。

**#15 | S6 main.cpp 骨架注释 | 瑕疵**
- **现象与证据**：骨架 `qputenv("QT_QUICK_CONTROLS_STYLE", "Basic")` 是无条件设，但注释写"QWK main.cpp:12-16：Qt6 用 Basic、Qt5 用 Default（按其版本分支设）"，暗示需要版本分支——与骨架的无条件 Basic 不符，易误导执行者去加分支。
- **处理（已改 04）**：注释改为"无条件 Basic（Qt5/Qt6 均内置，比 QWK 版本分支更简；两版统一 Basic 保证截图可对比），须在 QGuiApplication 构造前设"。

**#16 | S9.4 readme badge | 瑕疵**
- **现象与证据**：S9.4 只点 `readme.md:8` 为 `Qt-5.14+`，实测 `readme-cn.md:8` 有同款 badge（grep 两文件均命中 `Qt-5.14+`）；S9.4 标题虽写"readme.md / readme-cn.md"，但具体行引用只提 readme.md，执行者可能漏改中文版。
- **处理（已改 04）**：S9.4 明确"`readme.md:8` 与 `readme-cn.md:8` 两处"都改 `Qt-5.15+`。

**#17 | S6 CMakeLists 独立构建 fallback | 瑕疵**
- **现象与证据**：示例 `target_link_libraries(... SARibbon::Qml ...)` 但独立构建 fallback 写 `find_package(SARibbon REQUIRED)`（无 COMPONENTS），不保证 `SARibbon::Qml` 可用。
- **处理（已改 04）**：改为 `find_package(SARibbon REQUIRED COMPONENTS Qml)`。（附带发现 01 S11 Config 骨架的 COMPONENTS 校验只对 Widgets 做了 `NOT TARGET` 判断、Qml 分支未校验 target 存在性——属 01 侧小缺口，已在下方"四"供终审 agent 参考，04 不改 01。）

---

## 二、真跑命令与 Qt 头/源码核实摘要

**命令式注册 API（本机双版本 qqml.h，行号逐一核实，与 04 S1 引用完全一致）**：
- `qmlRegisterType`：6.7.3 `qqml.h:300/336`；5.14.2 `:291/:322` ✓
- `qmlRegisterSingletonInstance`：6.7.3 `:727`；5.14.2 `:681` ✓（存在，但 04 已改为不用它做单例）
- `qmlRegisterSingletonType` 回调式 `std::function<QObject*(QQmlEngine*,QJSEngine*)>`：6.7.3 `:674-706`（模板 :701/:706）；5.14.2 `:645/:663`（QJSValue 变体 :629） ✓
- `qmlRegisterModule`：6.7.3 `:645`；5.14.2 `:616` ✓
- `qmlRegisterUncreatableMetaObject`：6.7.3 `:297`；5.14.2 `:288` ✓
- `qmlRegisterUncreatableType`：6.7.3 `:148`；5.14.2 `:147` ✓

**单例所有权/多引擎语义（本机 Qt 源码，finding #1 的硬证据）**：
- 6.7.3 `Src/qtdeclarative/src/qml/qml/qqml.cpp:397/402-418`（`SingletonInstanceFunctor::operator()`：首调设 CppOwnership + 绑 `m_engine`，异引擎 return nullptr + 告警 :412）；`qqmlprivate.h:788-798`（`m_engine` 裸指针 + "cannot assign to a different engine"注释）。
- 5.14.2 `qqml.cpp:80-104`（`RegisterSingletonFunctor`：`alreadyCalled` 守卫 + :102 自动 CppOwnership）。
- 6.7.3 `qqmlengine.cpp:1834-1835`（回调式 `qobjectCallback(q,q)` 按引擎逐个调用，无单引擎绑定）。

**QQuickItem / QQuickWindow / QQuickPaintedItem / QQuickView API（6.7.3，与 04 S3/S7 引用一致）**：
- `setPosition(const QPointF&)` public `qquickitem.h:207`；`setSize(const QSizeF&)` public `:226`；`setImplicitSize` **protected** `:433`（protected 段自 :420）；`childItems()` public `:189`；`polish()` `:322`；`itemChange` protected virtual `:424`；`updatePolish` protected virtual `:468`；`classBegin/componentComplete` `:435/:436`。全部核实一致 ✓
- `QQuickWindow::effectiveDevicePixelRatio()` public `qquickwindow.h:141`；`QQuickPaintedItem : public QQuickItem` `qquickpainteditem.h:13`；`QQuickView(QQmlEngine*,QWindow*)` `qquickview.h:27`、`contentItem()` `qquickwindow.h:95`、`QQuickStyle::setStyle` `qquickstyle.h:17` ✓
- `QTest::qWaitForWindowExposed(QWindow*,int)`：声明在 **QtGui `qtestsupport_gui.h:27`**（namespace QTest），经 `<QtTest/QTest>`→`qtest.h:657`→`qtestsystem.h:11`（`#ifdef QT_GUI_LIB`）传递可见——S7 骨架链接 `Qt::Quick`（→QtGui→定义 QT_GUI_LIB），故 include 清单**足够**，无需额外 include（核实为正确，非缺陷）。

**P4 只读验证（本机 Qt 6.7.3）**：`D:/Qt/6.7.3/msvc2019_64/lib/cmake/` 下 `Qt6Quick`/`Qt6Qml`/`Qt6QuickControls2` 三目录均存在且各含 `*Config.cmake` ✓（P4 已由"find_package 通过"改为文件存在性只读核对）。

**版本落点（实测 grep）**：根 `CMakeLists.txt:7-13`＝2.9.5；`pyproject.toml:9`/`pyproject-pyqt6.toml:9`/`pyqt6/pyproject.toml:12`/`pyside6/pyproject.toml:11`＝2.8.0；`pyside6/CMakeLists.txt:3` project VERSION 2.8.0；`vcpkg.json:4`＝2.8.0。`readme.md:8`/`readme-cn.md:8`＝`Qt-5.14+`。`changlog.md` 格式 `## YYYY-MM-DD -> X.Y.Z`。tag `v2.5.7…v2.9.5`。`.github/workflows/`＝6 cmake-* + page.yml + publish-python-bindings.yml，`publish-python-bindings.yml:3-6`＝`release:[published]`+`workflow_dispatch`（无 tag 触发）。

**铁律判据源出行号（实测）**：`SARibbonPanelLayout.h:61 updateGeomArray()`/`:153 updateGeomArray(const QRect&)`/`:155 recalcExpandGeomArray`；`SARibbonCategoryLayout.h:74 updateGeometryArr()`；`SARibbonBarLayout.h:66 calcMinTabBarWidth()` ✓。core 引擎名 `SARibbonPanelLayoutEngine`/`SARibbonCategoryLayoutEngine`/`SARibbonBarGeometryEngine`（02 S5/S6/S7）——判据 2 子串冲突的证据。

**widgets 字体事件（实测）**：`SARibbonBar.cpp:4112`、`SARibbonCategory.cpp:1494`、`SARibbonPanel.cpp:1756`、`SARibbonToolButton.cpp:1525` 均 `case QEvent::FontChange`（无 ApplicationFontChange）——finding #7 证据。

**QWK 实证（`F:/src/3rdparty/qwindowkit`）**：`src/quick/qwkquickglobal.cpp` `registerTypes`＝`Q_UNUSED(engine)`+`static bool once`+`qmlRegisterType<QuickWindowAgent>`+`qmlRegisterModule`（与 04 S1.3 骨架同款，QWK **不注册任何单例**，故单例多引擎坑无参考先例可循，finding #1 属 SARibbon 自闯区）；`src/quick/CMakeLists.txt`＝`qwk_add_library(... QT_LINKS Core Gui Quick PREFIX QWK_QUICK)`（无 qt_add_qml_module/qmldir，印证命令式单轨）。

## 三、修订统计

- 本轮直接修订 04 文档共 **17 处 finding**（阻塞 3 / 歧义 4 / 缺口 5 / 瑕疵 5），全部落盘到 `04-qml-and-release.md` 正文（P4、§4、S1.3 代码块+单例注册坑、S2.1、S2 验证、S2.2、S3 骨架 ensureQmlItem、S3 操作1、S3.4、S3 叶子组织规范宿主配对、S5 操作1、S6 CMake fallback、S6 main.cpp 注释、S7 操作3、S8.1、S9.4、S10 现实块、S10.2、S10.4、S10.5、验收门 §6 判据2/判据3/首条、风险表单例行）。
- 未改 01/02/03/README（越权），涉及它们的 3 处跨文档不一致已在下方"四"列出供终审 agent 对齐。
- 核实为**正确、未改**的关键表述：S1 全部命令式 API 行号（双版本）、S3 QQuickItem API 访问级别与行号、S5 QAction 模块归属、S7 `qWaitForWindowExposed` include 链、铁律判据 1 源出行号、参考副本路径与 QWK 注册骨架。

## 四、遗留风险（需 Qt Quick 环境才能验证的项）

以下项本机无 SARibbonQml 构建产物（src/qml 尚不存在，且不得执行状态变更命令），只能做桌面/头文件级审查，**需真实 Qt Quick 环境构建后验证**：

1. **回调式单例多引擎实测**：finding #1 的修复（回调式 + CppOwnership）已按 Qt 源码论证，但"连续建两个引擎、第一个销毁后第二个仍能取到 RibbonTheme"需 tst_themeBridge 实跑确认（尤其引擎销毁顺序与 core Meyers 单例存活期）。
2. **Q_INIT_RESOURCE × sa_add_library AUTORCC 交互**：`Q_INIT_RESOURCE(saribbon_qml)` 要求 qrc 基名＝`saribbon_qml` 且资源初始化符号未被裁剪；01 S5 的 `sa_add_library` 对 qrc 的实际处理（AUTORCC 直编 vs 预编译 .cpp）决定符号名，需 `SARIBBON_BUILD_STATIC_LIBS=ON` 组合构建后以叶子 `QFile::exists("qrc:/SARibbon/...")` 核实（round2 §四.2 同列，仍未构建验证）。
3. **updatePolish 触发时机 × 测试挂窗**：S3/S7 依赖"item 挂 QQuickWindow 并曝光后 polish 才跑"，`qWaitForWindowExposed`+`QTRY_COMPARE` 在 offscreen QPA 下的实际收敛行为需真跑确认（offscreen 下 exposed 语义、polish 是否如期触发）。
4. **叶子 implicit 尺寸与宿主契约 sizeHint 的解耦**：finding #8 定了"sizeHint 来自 core metrics 不来自叶子"，但叶子视觉（Rectangle/Text 组合）在真实渲染下是否会反向影响宿主 implicitSize→干扰上层 Category/Bar 排布，需示例运行观察。
5. **QuickControls2 库侧链接决策（B-7）**：S5 菜单弹出用 Controls Menu/Popup 还是自绘叶子，决定库是否 PUBLIC 链 Controls2，需 S5 执行时定案（round2 §四.5 遗留）。
6. **同屏视觉一致性**：验收门首条的"条高/间距/装箱一致"最终仍需 widgets 版与 qml 版并排运行截图人工确认（已补客观基准＝一致性套件黄金值，但视觉佐证仍需环境）。

## 五、对整体计划体系的建议（供终审 agent）

1. **【高优先·跨 01/03/04】publish 触发链三方对齐**：01 S10.4（"不改文件"，维持 release:published+dispatch）、03 P4（称 01"暂停为仅 dispatch"）+ 03 S4.1（"还原 release:published、否决 tag"）、04 S10.4（早稿误称 03 改 tag，已由本轮改正为 release:published）——三处对"01 之后/03 之后触发器是什么"的叙述不自洽。04 已改为"不依赖声称的演变链、一律运行时 grep 实测 + 三分支决策表"自保，但建议终审统一 01/03 的表述，明确终态＝`release:[published]`，并让 03 P4 与 01 S10.4 一致（要么 01 真暂停为 dispatch、要么 03 P4 别声称 01 暂停过）。

2. **【中优先·跨 README/01/02/04】`python` vs `python3` 口径统一**：01 S9 明定 `python3`（CI runner 无 `python`），01 的 CI step/验收门亦用 `python3`；但 README R2（:81）与 02（P5/S8-3/验收门）用 `python`。04 已从 `python3`。建议终审把 README/02 也统一为 `python3`，避免 Linux CI 上 `python` 不存在导致纯净扫描 step 失败。

3. **【中优先·01 S11】SARibbonConfig 的 COMPONENTS 校验缺 Qml 分支**：01 S11 Config 骨架的 foreach 里只对 `Widgets` 做了 `NOT TARGET SARibbon::Widgets` 的存在性判断，`Qml` 落到 `else()` 无条件 `set(SARibbon_Qml_FOUND TRUE)`——即 `find_package(SARibbon COMPONENTS Qml)` 在未构建 QML 时也会报 Qml_FOUND=TRUE，消费端到链接 `SARibbon::Qml` 才失败。04 S6 示例已改为 `COMPONENTS Qml`（正确消费姿势），但建议终审在 01 S11 Config 给 Qml 补 `elseif(_comp STREQUAL "Qml" AND NOT TARGET SARibbon::Qml) set(SARibbon_Qml_FOUND FALSE)` 分支。

4. **【中优先·02↔04 契约字段名】`resultGeometry` 全链一致性**：02 S4.1-2 已把契约几何字段从 `geometry` 改名 `resultGeometry`（避 `QLayoutItem::geometry()` 冲突）。04 早稿验收门判据 3 仍写"契约 item 的 `geometry()`"（本轮已改 `resultGeometry`），S3 也补齐了对 `resultGeometry`/`applyGeometry`/`debugName` 的显式引用。建议终审全库 grep 一次 `geometry()` 在契约语境的残留（尤其 02 正文/v2 §3.4.1 代码块与 04 之间），确保字段名单向对齐到 `resultGeometry`。

5. **【低优先·跨前端一致性语义】字体事件源差异的测试约定**：finding #7 暴露 widgets（per-widget FontChange）与 QML（应用级 ApplicationFontChange）监听不同事件。建议终审在 tests/common 一致性套件规约里明确"字体变更测试统一改**应用字体**（QGuiApplication::setFont）"，使两端事件都能触发，避免只改单控件字体导致的双端不同步（04 S2.2/S7 已就地加注，但套件级约定宜写进 tests/common README）。

6. **【低优先·设计观察】单例注册无参考先例**：QWK/KDDW 在 qtquick/quick 模块**都不注册 QML 单例**（QWK 只 `qmlRegisterType<WindowAgent>`，KDDW 只注册 Instantiator 族 + uncreatableMetaObject），故 SARibbon 的 RibbonTheme/RibbonMetrics 单例（尤其"进程级 core 单例 × 多引擎测试"）是**自闯区**，finding #1 的坑正源于此。建议把"回调式 + CppOwnership + 多引擎哨兵测试"作为 SARibbon 的既定范式写进 dev-guide 的 QML 编码规范（S9.1 文档任务顺带），防后续贡献者退回 instance 式。
