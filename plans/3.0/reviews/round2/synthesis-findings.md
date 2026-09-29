# Round2 评审记录：整合（设计级裁决 + 跨文件一致性收口）

> 日期：2026-09-29
> 评审 agent 视角：**round2 整合**（第 2 轮第 4 个 agent，收口 3 个并行深读 agent 的产出）
> 本 agent owns：`SARibbon-3.0-plan-v2.md`（设计级修订）、`plans/3.0/appendix-reference-architecture.md`（新建）、
> `plans/3.0/README.md`、`plans/3.0/NOTES.md`、01~04（仅跨文件一致性小修）、本文件
> 素材：round2 三份 findings——[qwk-build-findings.md](qwk-build-findings.md)（修订了 01/03）、
> [kddw-core-findings.md](kddw-core-findings.md)（修订了 02）、
> [kddw-qtquick-findings.md](kddw-qtquick-findings.md)（修订了 04）
> 纪律：未编译/未构建、未 git add/commit；不重排既有 S 编号与 v2 章节号（新增小节只追加：v2 §6.4、§7.4、§9"3.1+ 候选项清单"）；
> 01~04 只做一致性小修，未改动其步骤结构与既有决策正文。

---

## 一、设计级建议逐项裁决表

### 1.1 QWK 构建体系 agent（qwk-build-findings.md §三 D1–D8）

| # | 建议 | 裁决 | 理由 | 落点 |
|---|------|------|------|------|
| D1 | 新增 `SARIBBON_INSTALL`（默认 ON）守卫全部 install/export/包配置规则 | **采纳** | QWK `QWINDOWKIT_INSTALL` 完整实证（QWK:CMakeLists.txt:13,36-39；src/:29,100,181）；SARibbon 2.9.5 install 规则无条件生效，`add_subdirectory` 嵌入场景污染宿主工程；01 S4 第 7 条顶层守卫覆盖不到 install；改动小（一个 option + if 包裹）但属新公共选项，正是应上达 v2 的级别 | v2 新增 §6.4 + §4.4/§6.1 批注；01 S4.3 选项行、S4.1 表、S4.2"不抄 1"翻转为"已采纳"、S5.3/S5.4/S6.7/S11 install 规则加守卫、S12.2 文档表；NOTES B10；README R4 |
| D2 | test-find-package 升级为 ctest 注册的 consumer 测试 | **维持可选，不升格验收门** | 01 S11.4 手动冒烟基线未被推翻、足以覆盖 3.0 验收；consumer ctest 注册价值真实但属增强，升格会扩大 03 验收面且引入"install 后才能跑"的 CI 时序复杂度；03 S5-5 已按"可选增强（做不做记 NOTES）"写入，维持 | 03 S5-5（不改）；附录 §2 表"改造为可选"行 |
| D3 | CI 多编译器轴 + MinGW/msys2 独立 job | **降级 3.1+ 候选（B-2）** | 价值真实（QWK 注释明言 win32 context 在 MinGW 下才暴露 SDK 头差异），但 3.0 CI 范围已由 01 S10/03 S5 锁定，扩轴成本与 runner 时间显著；与 findings 原建议一致 | v2 §9"3.1+ 候选项清单"B-2 |
| D4 | MinGW 去 lib 前缀 | **驳回（不做）** | 产物名行为变化，违反 01"行为零变化"纪律；未来对齐属单独决策 | 附录不抄清单 #8 |
| D5 | qt_add_qml_module 轨无 QWK 先例的预警（供 04） | **已消解**（被单轨化吸收） | 04 S1 已定型命令式单轨，声明式轨降 3.1+——预警转化为候选项 B-3 的启用条件（"届时 qmldir/plugin 安装布局无参考实现，需 Qt6.5/6.8 两版实测"已并入 B-3 说明） | v2 §5.3/§9 B-3；04 S1 附注 |
| D6 | 版本兼容策略差异（QWK=AnyNewerVersion） | **记录，无需动作** | SARibbon 维持 SameMajorVersion（更严格、与 2.9.5 现状一致），findings 已核、01 S11 附注已写 | 附录不抄清单 #15 |
| D7 | 安装 include 布局差异（QWK 有伞目录） | **记录，无需动作** | 维持无伞 `include/<模块>/`（与 2.9.5 同层级，消费端写法两者相同），01 S5.5 已写 | 附录不抄清单 #16 |
| D8 | 不得写"参考 QWK 的 ccache 配置" | **记录为禁令** | QWK 快照与上游 ci.yml 均无 ccache/sccache（仅 jurplel action 自带 Qt 下载缓存）——无实证依据的说法必须禁止 | 附录不抄清单 #7（显式"禁止引用"措辞） |

### 1.2 KDDW core agent（kddw-core-findings.md §三 ①–⑦）

| # | 建议 | 裁决 | 理由 | 落点 |
|---|------|------|------|------|
| ① | v2 §3.4.1 契约增补 `debugName()` 默认虚函数 | **采纳** | KDDW 实证诊断名是引擎可测性一等公民（LayoutingGuest_p.h:50-53、Item.cpp:884-898）；成本≈0；02 S4.1-5 最终代码块已含，v2 按"以 02 为准"同步 | v2 §3.4.1（代码块整体替换 + 修订说明②） |
| ② | v2 §3.7 禁区清单两层化（模块纯净 vs 引擎确定性），明确 QtGui 类合法 | **采纳** | 这是门禁语义问题：单层清单会误杀 S1/S2 合法下沉的 QGuiApplication 依赖，或放过引擎偷调 `QGuiApplication::layoutDirection()`；02 S8-3 已定执行细则，v2 层面必须同调以防后续 agent 以 v2 原文为准 | v2 §3.7 重写（两层）+ §6.2 扫描示例更新 + §10-R8 批注；01 S9 清单扩充（`QStyledItemDelegate`、`<QtWidgets/`、`<QtQuick/` + QtGui 合法豁免注） |
| ③ | 黄金 fixture 录制介质 JSON 化（回放仍 C++） | **采纳** | KDDW tests/layouts/*.json 全套实践（可 diff 审查、可重录对账、畸形场景可手工构造）；不推翻 02 的 C++ 回放介质（编译期类型安全、录制工具已按此设计）——双介质对账是增强不是替换；采纳后 02 S5.0-3 的工具 JSON 输出升为正式交付物 | v2 §7.1 新增"fixture 双介质制"bullet（02 S5.0-2 第 4 项已落地，v2 与之对齐） |
| ④ | "错误零容忍"（fatal_logger 轻量版）记入 v2 §7 前瞻条款 | **采纳为前瞻条款（3.x 候选，不进 3.0 验收）** | 3.0 core 无日志设施（调试打印不随算法迁移，v2 §3.4.2），M1 零成本；政策价值在 3.1+ core 引入日志时兑现；按任务边界明确标注不进 3.0 验收 | v2 新增 §7.4 + §9 候选清单 B-4 |
| ⑤ | rowIndex 类型定案（int vs short 二选一） | **采纳 int 定案；驳回"改契约为 short"备选** | 引擎内部计算全 int，契约用 short 会引入 short/int 混用提升警告；3.0 是大版本，`short→int` 的取址绑定断裂面已全仓预验证为零（git grep 全命中为赋值/读取，sip/pyside6 未以 short 暴露）；兼容处理与执行期复核命令已在 02 S4.1-3 | v2 §3.4.1 修订说明④（含 2.x short 事实批注）；02 §8 偏差已记（不改） |
| ⑥ | SizingInfo 式显式中间态 + 加权分配段纯函数化立为 3.1 引擎优化项 | **降级 3.1+ 候选（B-1）** | 结构重构违反 M1 纯 move 纪律，3.0 严禁执行；02 S5.2-2 已写"M1 禁止顺手结构体化"注记与 SizeHintCollection 保护条款，v2 层面收口为候选项 | v2 §9 候选清单 B-1 |
| ⑦ | 维持 v2 §2.3 两项"不采用"（单库拼合、Platform 单例） | **采纳（维持，v2 无改动）** | round2 深读后无翻案证据：单库拼合代价=core 内前端宏散布（DragController.cpp:30-37 实证）；Platform/DelayedCall/Screen 抽象的存在理由（跨 GUI 框架前端、Flutter 无 QTimer）在 SARibbon 均不成立 | v2 §2.3 不改；附录不抄清单 #2/#3 + §3.1 表记录实证 |

### 1.3 KDDW qtquick / QWK quick agent（kddw-qtquick-findings.md §三 1–5、§四 未决项）

| # | 建议 | 裁决 | 理由 | 落点 |
|---|------|------|------|------|
| 1 | 注册路线单轨化上达 v2 §5.3（04 S1 已直接修订，待整合复核） | **采纳（复核通过）** | 两家生产级库在 Qt6 下的实证选择（KDDW 明知 6.2+ 可用 qt_add_qml_module 而不用；QWK quick 零 QT_VERSION 分支）；单轨化把整类声明式轨风险从"缓解"变"不存在"；损失项（qmltypes 补全/qmllint/qmlcachegen）均非 3.0 验收项且保留 3.1+ 启用通道；round1 的 Qt6QmlMacros 核实成果压缩保留在 04 S1 附注，知识不丢 | v2 §5.3 重写 + §0/§1.1-4 历史批注 + §5.2/§5.4 批注 + §9-D5 批注 + §9-D1 理由更新 + §10-R9 新增；04 S1 两处"待整合者复核"标记改为"已复核采纳"；01 S6-5、03 S6-3 同步终态；README R5 术语"命令式单轨"；NOTES B9 |
| 2 | QML 安装布局结论（叶子进 qrc、安装期零新增产物、Config Qml 组件只需 find_dependency(Qt Quick/Qml)） | **采纳** | KDDW src/CMakeLists.txt:634-666 与 QWK src/quick 双双实证安装侧只有头+库；与 03 S6-3 的 round2 预警衔接（预警解除为终态结论）；01 S11 Config 骨架原缺 Qml 分支 | v2 §6.1 批注②；01 S11.1 Config 骨架补 `if(@SARIBBON_BUILD_QML@) find_dependency(... Qml Quick)` 分支 + S11.5 清单注；03 S6-3 预警改写为终态结论；04 S3 资源路线 bullet 更新指向 |
| 3 | 契约适配器以 QWK `QuickItemDelegate` 为模板 | **采纳（确认既有落点，无新增修订）** | 04 S3 骨架注记已写明（纯虚窄接口逐个映射、放 .cpp 私有）；属"已落地的设计确认"，整合层只需固化进附录防散失 | 附录 §3.3 表（QuickItemDelegate 行） |
| 4 | 主题绑定规则（叶子禁字面量色值）同步进 dev-guide QML 编码规范 | **采纳** | 该规则是 SARibbon 特有增强、无 KDDW 先例（KDDW 叶子颜色硬编码是反面参照），只存在于 04 S3 规范段——不进 dev-guide 则贡献者不可见；落点为 04 S9.1 文档任务顺带，零新增执行成本 | 04 S9.1 补"QML 编码规范"条目 |
| 5 | objectName 约定 + 握手属性测试可达性写进 tests/qml README | **采纳** | KDDW 明确惯例（Base 叶子注释原话 "just so the unit-tests can access the buttons"）；04 S7 原文只写"二选一模式写进 README"，测试可达性约定是缺口 | 04 S7"关键坑"段补测试规范两句 |
| §四.1/2 | 多引擎注册时序、Q_INIT_RESOURCE 符号名 × AUTORCC 交互 | **收口为 3.1+ 候选 B-5（含执行期核实动作）** | 两项都是"执行时以实测关闭"的核实项而非设计缺口（04 S1/S2 验证步骤已含静态组合检查）；候选清单保留使其不被遗忘 | v2 §9 候选清单 B-5 |
| §四.3 | Loader 宿主引用转发 | **收口为 3.1+ 候选 B-6** | P0 已定"直接 setParentItem、不引 Loader"（04 S3 叶子规范）；风险在 3.1+ 弹出类场景引入 Loader 时兑现 | v2 §9 候选清单 B-6；附录不抄清单 #21 |
| §四.4 | KDDW 私有 API 不抄 | **采纳为清单禁令** | 04 全程公共 API 已核；固化进附录不抄清单供执行期"遇到诱惑时否决" | 附录不抄清单 #10 |
| §四.5 | QuickControls2 是否进库链接 | **收口为候选/观察项 B-7** | 定案权在 04 S5 执行期（取决于菜单实现走 Controls 还是自绘），预期 3.0 内关闭；按任务要求列入候选清单作观察项，防止 3.0 未定案时遗失 | v2 §9 候选清单 B-7（注明"预期 3.0 执行期定案"） |
| §四.6 | Repeater/model 路线的 Tier 2 触发条件 | **记录（无新增动作）** | 04 S4.5 已写死 P0 不采用 + Tier 2 先例指引；附录表行固化 | 附录 §3.3"数据驱动子项"行 |

---

## 二、v2 计划（SARibbon-3.0-plan-v2.md）修改清单

所有修订处均以**【round2 修订】**或**【round2 精确化】**标注并给证据出处；未重排任何既有章节号（新增小节只追加）。

| # | 位置 | 修改 | 对应裁决 |
|---|------|------|---------|
| 1 | 文档头部 | 加"round2 修订"声明（标注约定、findings/附录/synthesis 路径） | — |
| 2 | §0"保留"行 | "QML 双轨注册"加单轨化批注（指向 §5.3） | qtquick-1 |
| 3 | §1.1-4 | 双轨保留条目加【round2 修订】批注，标注"本条保留为 v1 审查历史记录" | qtquick-1 |
| 4 | §1.2-P2 段后 | 新增【round2 精确化】批注三点：① layouting/"零 GUI"精确化（Item 本身是 QObject，Item_p.h:190-192；QObject-free 的是契约面）② KDDW core 整体非绝对零 GUI（前端宏裁剪，DragController.cpp:30-37）③ KDDW 引擎无 RTL（grep 零命中），SARibbon RTL 入参化系无先例自主设计。**不改结论** | core-⑦ 相关独立发现（任务 A7） |
| 5 | §2.3"纯几何 layouting 引擎"行 | 采用理由列加精确化批注（SARibbon 引擎为无 QObject 瞬态纯函数，更彻底的有意分叉） | 同上 |
| 6 | §3.4.1 | **代码块整体替换为 02 S4.1-5 最终契约代码块**（resultGeometry 定名、debugName/maximumWidth/stretchFactor 默认虚、rowIndex int + short 事实批注、rowProportion 默认 Large、SARibbonAbstractCategoryItem 扩展、Host 注释同步）+ 六点修订说明 + 设计要点更新（比 KDDW 更窄的纯虚计数；缓存生命周期不变量指向 02 S5.1-2） | core-①⑤；任务 A2 |
| 7 | §3.7 | **两层化重写**：第一层模块纯净（清单扩 `QStyledItemDelegate`、`<QtWidgets/`、`<QtQuick/`；QGuiApplication/QScreen/QFontMetrics/QColor/QIcon 属 QtGui 明确合法）+ 第二层引擎确定性（layout/ 禁调 QGuiApplication 动态状态；采集器函数豁免；KDDW Item.cpp:1114-1122/Screen_p.h:41 佐证） | core-② |
| 8 | §4.4 | 补 bullet：转发头/兼容包安装规则收进 SARIBBON_INSTALL 守卫 | QWK-D1 |
| 9 | §5.1 | Instantiator bullet 后加【round2 修订】批注：混合模式配对机制被全面实证；P0 不需要独立 Instantiator（DockWidgetInstantiator.h:27-34 存在理由不成立），Tier 2 触发再抄 | qtquick-1 相关（04 S3 结论上达） |
| 10 | §5.2 表后 | 批注：RibbonMetrics 定为 singleton（04 S2 定案，attached 降 3.1+）；核对确认表内无 qt_add_qml_module/QML_ELEMENT 表述 | 任务 A1（§5.2 核对项） |
| 11 | §5.3 | **重写为"命令式单轨"**：原双轨方案保留为历史记录；单轨实现要素（注册函数签名、static-once、Q_INIT_RESOURCE、qmlRegisterModule、load 前显式调用）；两家实证；声明式轨触发条件与 3.1+ 通道（B-3）；"qmldir/plugin 随 qml 模块安装"作废 | qtquick-1；QWK-D5 |
| 12 | §5.4 | qmlRegisterSingletonType → qmlRegisterSingletonInstance + CppOwnership 批注（多引擎悬空防护；回调式保留为声明式轨预案件） | qtquick-1 配套 |
| 13 | §6 标题 | 加"【round2 修订】另增 §6.4"字样（"增强两点"计数时效性） | — |
| 14 | §6.1 | 加【round2 修订】两点增补：① SARIBBON_INSTALL 指针（→§6.4）② QML 安装布局结论（零新增产物 + Config Qml 组件依赖，KDDW:634-666/QWK src/quick 实证） | QWK-D1；qtquick-2 |
| 15 | §6.2 | 扫描示例两层化（python3 + 扩充清单含路径形式；layout/ 定向 grep 命令行；组合构建保留）+ 第一/二层执行方式说明 | core-② |
| 16 | §6.4（新增） | SARIBBON_INSTALL 安装守卫选项：语义、QWK 实证行号、01 落地点清单、证据出处 | QWK-D1 |
| 17 | §7.1 | 新增两 bullet：fixture 双介质制（JSON 录制/C++ 回放，KDDW tests/layouts 实践）；断言双轨（黄金值+关系不变量，tst_multisplitter.cpp:284-285,352-364）+ 失败诊断 dump | core-③（+02 S8-1 既有结论上达） |
| 18 | §7.4（新增） | 前瞻条款"错误零容忍"（fatal_logger 实证 + 3.x 候选标注，不进 3.0 验收） | core-④ |
| 19 | §9-D1 行 | 理由更新：单轨化后"双轨注册"理由失效但 5.15 结论不变（KDDW Qt5 下限 5.15，CMakeLists.txt:149；qmlRegisterSingletonInstance 需 5.14+） | qtquick-1 配套 |
| 20 | §9-D5 行 | 批注：混合模式被全面实证（配对机制三件套）；注册配套从双轨改单轨；D5 结论不变且被强化 | qtquick-1（任务 A1 明确要求） |
| 21 | §9-D8 行 | 补证：KDDW Core::Action ≈406 行服务 2 消费点的量化（支撑"推迟"） | core-⑦/02 S4.3 |
| 22 | §9 表后（新增小节） | **"3.1+ 候选项清单"B-1~B-7**：SizingInfo 中间态、CI 多编译器轴/MinGW、声明式轨、fatal_logger、多引擎时序/Q_INIT_RESOURCE×AUTORCC、Loader 转发、QuickControls2 链接——每项一句话+出处文件名 | core-⑥；QWK-D3；qtquick §四.1/2/3/5（任务 A8） |
| 23 | §10-R8 行 | 补两层化扫描批注 | core-② |
| 24 | §10-R9（新增行） | 单轨化残余风险（漏调注册函数）与缓解（模板固定调用/导入专项检查/排障文档） | qtquick-1 配套 |
| 25 | 附表两行 | layouting 行（零 GUI 精确化 + 无 RTL）、Instantiator 行（P0 不需要）加精确化/修订批注 | core-⑦ 相关；qtquick §一.4 |

## 三、附录文档（plans/3.0/appendix-reference-architecture.md）结构概览

新建，角色=**参考资料（非执行计划）**，目标读者=执行 01~04 的 agent 与未来维护者：

1. **用途与证据约定**：五个引用前缀（QWK:/qmsetup:/QWK-upstream:/KDDW:/SR:）的版本与时效警示（本地副本 commit、fork 差异、上游 ci.yml 比快照新）；行号漂移以符号名检索。
2. **QWindowKit 借鉴总表**（18 行，机制｜实证 文件:行｜SARibbon 落点｜抄/不抄/改造）：qwk_add_library 宏契约、STATIC/SHARED 判定、导出宏三段式（含 qmsetup 非互斥细节与头文件优先级铁律）、install OPTIONAL×3、单 export set、Config.cmake.in（含 NO_CHECK_REQUIRED_COMPONENTS_MACRO 陷阱警示）、QWINDOWKIT_INSTALL 守卫、选项命名、quick 命令式注册、示例四件套链接、ctest --no-tests=error、consumer 测试（可选）、qwk_add_example、裸 target 名、win rc、include 三件套、qm_sync_include、惰性 Qt 探测（不抄）、debug 后缀、qmake/msbuild 物料、CI 组织（不抄降候选）、ccache（禁止引用）。
3. **KDDockWidgets 借鉴总表**（三张分表）：§3.1 core/引擎侧 17 行（契约极简性+debugName、identity/悬垂防护、SizingInfo 三段式、Config 门面、平台数值三通道、确定性净室、Meyers 单例、Registry/Controller、ViewInterface 族、Core::Action 量化、DelayedCall、fatal_logger、导出宏、Logging、前端宏裁剪、单库拼合、序列化、无 RTL）；§3.2 测试侧 9 行（JSON fixture、关系不变量、checkSanity/dump、退化输入、dpr、白名单 RAII、引擎级测试形态、tests_*/Platform、qtquick 测试实践）；§3.3 qtquick 前端侧 17 行（命令式注册、枚举暴露前提、不用 qt_add_qml_module、宿主类模式、叶子三部曲、析构 deleteLater、双向通道、Base-视觉两层、换叶子定制、qrc/Q_INIT_RESOURCE、Instantiator、Repeater/model、无主题机制、动态属性 hack、私有 API、QWK 窗口集成/QuickItemDelegate、示例组织、安装布局、QuickControls2）。
4. **两家共同点提炼**（4 条最有约束力结论）：命令式注册是双 Qt5/Qt6 现实解；C++ 结构类+QML 视觉层混合模式被两家同时验证；边界/纯净靠机制（扫描+组合构建）不靠自觉；黄金值/一致性套件是一致性承诺的唯一可执行保障。每条附"约束力"说明（对执行 agent 的强制含义）。
5. **明确不抄清单**（22 项，每项一句理由）：含 QWK D4/D6/D7/D8 与 core ⑦ 的"记录即可"项（MinGW 前缀、AnyNewerVersion、伞目录、ccache 禁令、单库拼合、Platform 单例），以及 qmsetup、Core::View、Core::Action（3.0）、Instantiator（P0）、私有 API、前端宏裁剪、动态属性 hack、Q_NAMESPACE 路线、两段式 find_dependency、qm_init_directories、惰性 Qt 探测、FOR_UNIT_TESTS 宏、Loader 中转层（P0）、qWait 硬等待。
6. **维护约定**：后续轮次裁决翻转由整合 agent 同步更新；行号漂移先符号名复核；不进 mkdocs nav。

内容全部取材于三份 findings（压缩提炼，每个实证保留 文件:行），未发明新证据、未新增源码复核。

## 四、跨文件一致性小修清单（01~04，未动步骤结构与既有决策）

### 01-infra-restructure.md

| 位置 | 修改 | 依据 |
|------|------|------|
| S4 第 3 条选项清单 | 补 `SARIBBON_INSTALL`(ON) 一行 + 与 `SARIBBON_INSTALL_IN_CURRENT_DIR` 的区分注（"装不装" vs "装到哪"） | QWK-D1 采纳（任务 A5 明示允许） |
| S4.1 表 L15-27 行 | 新增选项串补 `SARIBBON_INSTALL`(ON) | 同上 |
| S4.2"不抄 1"条目 | 翻转为"已采纳（round2 整合裁决）"，指向 v2 §6.4/NOTES B10/synthesis | 消除与 v2 新决策的直接矛盾 |
| S5.3 install bullet + 骨架代码 | install(TARGETS) 包进 `if(SARIBBON_INSTALL)` | v2 §6.4 |
| S5.4 骨架代码 | install(DIRECTORY) 包进 `if(SARIBBON_INSTALL)` | v2 §6.4 |
| S6-5 QWK quick 实证注记末条 | "03-S6 届时需增补 qml 项"更新为"04 已单轨化，03-S6 无 qml 项即终态" | qtquick-1/2（消除与 04 S1 的过时交叉引用） |
| S6.7 代码块 | install(EXPORT) 包进 `if(SARIBBON_INSTALL)` | v2 §6.4 |
| S9 扫描清单 bullet | 清单补 `QStyledItemDelegate` + `<QtWidgets/`、`<QtQuick/` 路径形式 + QtGui 合法豁免注（与 v2 §3.7/02 S8-3 对齐） | core-② |
| S11.1 Config 骨架 | 补 `if(@SARIBBON_BUILD_QML@) find_dependency(... Qml Quick REQUIRED)` 分支 + 注释；生成/安装规则加 SARIBBON_INSTALL 守卫说明 | qtquick-2；QWK-D1 |
| S11.3 转发头代码块 | install(DIRECTORY) 包进 `if(SARIBBON_INSTALL)` | v2 §6.4 |
| S11.5 安装树清单 | 加注：以 SARIBBON_INSTALL=ON 执行；"无 qml 项即终态正确状态" | QWK-D1；qtquick-2 |
| S12.2 build.md 条目 | 新增选项名串补 `SARIBBON_INSTALL` | QWK-D1 |

### 02-core-sinking.md

| 位置 | 修改 | 依据 |
|------|------|------|
| S4.1-5 代码块标题句 | 补"round2 整合已把 v2 §3.4.1 同步为本块——两处字段/方法名一致，本块仍是执行依据" | 任务 C2（02 S4.1 ↔ v2 §3.4.1 一致性核对结论：一致） |
| S8-3 标题句 | 补"v2 §3.7 已按本节修订为两层表述、§6.2 扫描示例已同步" | core-② |

（02 其余 round2 修订由 core agent 完成，本轮未动。）

### 03-build-ecosystem.md

| 位置 | 修改 | 依据 |
|------|------|------|
| S6-3 前言 | 补"全部 install 规则在 SARIBBON_INSTALL（默认 ON）守卫内，本复核以默认 ON 执行" | QWK-D1 |
| S6-3"QML 安装布局预警"bullet | 改写为**终态结论**：04 已单轨化 → 无 qml 项即终态正确状态（零新增产物，KDDW:634-666/QWK src/quick 实证）；仅 3.1+ 启用声明式轨（B-3）时才需增补；Config Qml 依赖指向 01 S11 | qtquick-2；QWK-D5 |

（03 其余 round2 修订由 QWK agent 完成，本轮未动。）

### 04-qml-and-release.md

| 位置 | 修改 | 依据 |
|------|------|------|
| S1 路线决策标题句 | "⚠️设计级变更待整合者复核"→"整合者已复核采纳（v2 §5.3 已同步修订，裁决记录见 synthesis-findings.md/NOTES B9）" | qtquick-1 |
| S1 末 bullet | "本变更上达 v2 §5.3 修订"→"round2 整合已完成 v2 §5.3 修订（含 §9 B-3、NOTES B9）" | 同上 |
| S3 叶子组织规范"资源路线"bullet | "供计划 03 整合"→"round2 整合已同步：v2 §6.1 批注②、03 S6-3 终态结论、01 S11 Qml find_dependency 分支" | qtquick-2 |
| S7"关键坑"段 | 补测试规范两句：被测项必须设 objectName（KDDW tst_qtquick.cpp:325-329）；握手属性=测试可达性通道（TabBarBase.qml:70-71、TitleBar.qml:23-25）——写进 tests/qml README | qtquick-5 |
| S9.1 | 补 dev-guide"QML 编码规范"条目（叶子颜色/尺寸一律绑定单例、集中 Base 层、字面量色值=打回） | qtquick-4 |

（04 其余 round2 修订由 qtquick agent 完成，本轮未动。）

### README.md / NOTES.md

| 文件 | 修改 |
|------|------|
| README 引言 | 补附录文档一句话介绍（参考资料定位 + "先查它再读源码"用法） |
| README 计划清单表 | 加 appendix 行（对应里程碑=—，角色=参考资料非执行计划，无出口判据） |
| README"修订历史" | 补第 2 轮完整条目（日期、视角、3 个并行 agent 各自修订对象与 findings 路径、整合 agent 产出清单） |
| README R4 已知偏差 | 补 B9/B10 两条摘要 |
| README R5 术语表 | 新增"命令式单轨"词条（定义 + 要素 + 3.1+ 通道）；"契约接口"词条补"最终代码块以计划 02 S4.1-5 为准" |
| NOTES.md | 按模板追加 **B9**（QML 注册单轨化，与 v2 原文冲突已修订）、**B10**（新增 SARIBBON_INSTALL 选项，与 v2/01 原文冲突已修订）——发现位置=round2 评审、证据=findings 文件与 文件:行、处理=v2 已修订 |

## 五、术语一致性核对结果（任务 C2-2）

- **"命令式单轨"**：v2（§0/§1.1/§5.3/§9-D5/§10-R9）、README（R5 词条/R4/修订历史）、NOTES（B9）、01（S6-5）、03（S6-3）、04（S1/风险表既有）、附录（§2/§4）——全部统一为该拼写；grep 复核 16 处命中无变体。
- **`SARIBBON_INSTALL`**：v2 §4.4/§6.1/§6.4、README R4、NOTES B10、01（S4.3/S4.1/S4.2/S5.3/S5.4/S6.7/S11×3/S12.2）、03 S6-3、附录——拼写一致，且 01 S4.3 已加与 `SARIBBON_INSTALL_IN_CURRENT_DIR` 的区分注防混淆。
- **契约字段/方法名**：v2 §3.4.1 代码块与 02 S4.1-5 逐字段核对一致（`resultGeometry`/`resultSeparatorGeometry`/`isSeparatorHidden`/`rowIndex:int`/`debugName()`/`maximumWidth()`/`stretchFactor()`/`SARibbonRowProportion::Large`）；仅注释内交叉引用按宿主文档改写（"见上第 N 条"→"计划 02 S4.1-N"）。
- **01 S6-5/S11 ↔ 04 S1**：QML 构建/注册/安装表述已对齐单轨终态（见 §四 各行）。
- **03 S6-3 ↔ 04/v2**：预警已解除为终态结论，三处（v2 §6.1 批注②、03 S6-3、04 S3 资源路线）互相指向一致。
- 04 中两处"待整合者复核"标记、03 中"供计划 03 整合"类悬置表述已全部清除（grep `待整合者复核|供整合者裁决|供计划 03 整合` 于 01~04 零命中）。

## 六、遗留事项（不在本轮处理范围，交执行期/后续轮次）

1. **执行期实测项（候选 B-5 承载）**：多引擎注册时序组合（注册前/后建第二引擎）在 04 S2 tst_themeBridge 各测一次；`Q_INIT_RESOURCE(saribbon_qml)` 符号名与 sa_add_library AUTORCC 的实际交互以构建产物核实；`install(TARGETS ... OPTIONAL)` 在 static/shared 各跑一次 `cmake --install`（qwk-build-findings.md §四.3）。
2. **QuickControls2 库侧链接**：04 S5 执行期定案并记 NOTES（候选 B-7 观察项）。
3. **01 S11 Config 骨架的组件检查小缺口（本轮未改，避免超出小修范围）**：`foreach` 中仅对 `Widgets` 组件检查 `NOT TARGET SARibbon::Widgets`，`Qml` 组件落入 else 分支恒置 TRUE——`SARIBBON_BUILD_QML=OFF` 时消费端 `find_package(SARibbon COMPONENTS Qml)` 会误报成功。执行 S11 时建议补 `(_comp STREQUAL "Qml" AND NOT TARGET SARibbon::Qml)` 分支并记 NOTES（逻辑推读，非参考项目证据）。
4. **QFontMetrics 在无 QGuiApplication 实例时的行为**（kddw-core-findings.md §四.2）：tests/core 不建 QApplication 天然覆盖，02 S2/S3 执行时确认不 segfault 即可。
5. **QWK-upstream 证据时效**：所有 `QWK-upstream:` 引用仅代表上游 main 新版做法，不代表 SARibbon submodule pin 的 fork（czyt1988/qwindowkit f93657f）；升级 QWK pin 时应重新核对（qwk-build-findings.md §四.2；附录 §1 已载警示）。
6. **v2 §6 标题计数**："增强两点"经 round2 后实为四点（§6.2/§6.3/§6.4 + §6.1 增补），标题已加"另增 §6.4"字样但未按数字重述——遵守"不重排 v2 章节号/不重写历史标题"的纪律，下轮如有 v2 整体翻新再收口。
