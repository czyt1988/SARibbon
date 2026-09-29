# SARibbon 3.0 改造计划 第 3 轮终审报告（final-audit）

> 角色：第 3 轮评审的**终审 agent**（在 4 个并行 dry-run agent 之后收口）。
> 日期：2026-09-29。
> 审计范围：`SARibbon-3.0-plan-v2.md` + `plans/3.0/{README.md, NOTES.md, 01~04, appendix-reference-architecture.md}` 共 7 份文档两两交叉；四份 dry-run findings（本目录 `01~04-dryrun-findings.md`）的"对整体计划体系的建议"与"遗留风险"节逐条裁决。
> 审计时点仓库状态：分支 `v3` @ `eba8ebd`（round2 评审提交），工作区含 round3 四份计划的**未提交修订**与未跟踪的 `reviews/round3/`；代码基线 `7a617fc`（2.9.5）后零代码改动。
> 修订权属：终审直接修订 README/NOTES/v2 计划/appendix；01~04 仅做裁决项与跨文档一致性问题的小修（未重排任何 S 编号，未改动步骤结构）。
> 任务边界遵守：未编译构建、未 git add/commit、未执行任何状态变更命令。

---

## 一、跨计划裁决记录

每项按"冲突 | 裁决 | 理由 | 落实位置"记录。裁决编号 F3-A<n>。

### F3-A1 v2 计划与 02 终版冲突的旧文（含终审扩围）——采纳并扩围

- **冲突**（02-dryrun 遗留风险⑥/建议 1，终审 grep 扩围）：
  ① v2 代码块 **7 处**误名宏 `SARIBBON_CORE_EXPORT`（实名 `SA_RIBBON_CORE_EXPORT`，01 S5.2/S6.3 宏映射表；02 S4.1-5 已正名）——不止 findings 点名的 §3.4.1/§3.6，实查 §3.3/§3.4.2/§3.4.3 同样中招；
  ② §3.4.2 `Input` 含已作废的 `enableExpanding` 字段（2.x 无此开关，触发式属算法本体）；
  ③ §4.2 适配器示意 `it->applyGeometry(it->geometry)` 用已废弃字段名（现定名 `resultGeometry`）；
  ④（终审新发现，同族）§7.1 黄金测试示意 `QCOMPARE(items[0].geometry, ...)` 与 `RowProportion::Large` 未随契约改名；
  ⑤（02 正文已标注、v2 未同步）§3.4.3 `clampScrollOffset` 签名缺 `isRTL`，Result 缺 sizeHint/minSizeHint/newXBase/滚动标志与 `scrollButtonFlags` 单实现收敛；
  ⑥（02 §8 已登记、v2 未同步）§3.3 示例 `spacing = 1` 与 2.x 实测默认 `setSpacing(2)` 不符，且输入字段集不完整（缺 pmTabBar* 系列/userDef*）。
- **裁决**：全部按 02 S4.1-5 / S5.2-1 / S6-3 / S3.1 终版对齐修订，各处带【round3】标注；v2 头部新增"round3 修订"说明块。
- **理由**：02 dry-run 已按全仓实测锁定终版并声明"落地以 02 为准"；v2 是执行者对照的设计依据，冲突旧文会误导（02-dryrun 遗留风险⑥："风险缓解但未消除"）。终审职责即消除之。
- **落实位置**：v2 头部、§3.3（spacing + 输入完整性注）、§3.4.2（字段删除 + "以 02 S5.2-1 为准"注）、§3.4.3（签名 + Result/scrollButtonFlags 注）、§3.4.1/§3.3/§3.4.2/§3.4.3/§3.6 宏名 ×7、§4.2、§7.1、§9-D1（单例 API 名，见 F3-A20）。

### F3-A2 02 S1.1-4 的 sip 冒烟在 01→03 窗口不可行——采纳方案(a)降级，否决方案(b)提前

- **冲突**（03-dryrun 体系建议 2，02-dryrun 建议 4 关联）：02 S1.1-4 要求枚举下沉后"跑通 sip/PyQt 构建冒烟"；但 01 S6 目录搬移后、03 S3 绑定适配完成前，三轨绑定的 include-dirs/源清单指向已不存在的 `src/SARibbonBar`，且转发头的 `<SARibbonCore/...>` 在绑定构建场景无解析（03 S3.0 镜像未建）——绑定整体不可构建，冒烟必然失败且无法归因。
- **裁决**：02 S1.1-4 验证**降级为 C++ 侧**（widgets 全量编译 + `QMetaEnum|fromType<` 消费者 grep + ctest == N₀），并在条文明写"绑定侧验证延迟至计划 03 S3"；03 S3.2-1 新增"**枚举别名化适配**"承接条（RowProportion 三文件 `sip/SARibbonPanelItem.sip:10`、`pyqt6/sip/SARibbonPanelItem.sip:10`、`pyside6/typesystem_saribbon.xml:46` + S1.1 四枚举对应条目，按 02 实际执行结果——提升成功或降级留 widgets——分支处理）。
- **理由**：findings 给出的备选方案(b)"把 03 S3 纯路径前缀迁移提前到 01 S6 之后"**否决**——即便提前迁移路径，S3.0 镜像与 S3.2-2 陈旧清单补齐完成前绑定仍不可构建，提前不能使冒烟可行，反而破坏"01/02 不动绑定文件"的计划边界与提交归属，违反 R4 保守原则。方案(a)边界清晰、承接点唯一。
- **落实位置**：02 S1.1-4（重写）、02 S1 RowProportion 绑定波及段（改指向 03 承接清单）、02 §7 风险表对应行、03 S3.2-1（新增承接条）、NOTES B13、01 S6.4（互认注，见 F3-A7）。

### F3-A3 README/NOTES git 基线行过时——采纳，并升格为体系级约定

- **冲突**（01-dryrun 建议 1/4、02-dryrun 遗留风险⑦）：README 头部/事实快照/R4 与 NOTES B7 仍写"dev/v3/origin/dev 三者同指 7a617fc、未跟踪项 3 个"；实际评审提交已入库（`42b7dcc` round1、`eba8ebd` round2，round3 产物待提交），v3 已前进、未跟踪项仅剩 bundle。且该问题**每轮评审提交后都会复发**（round3 的 3 条阻塞级缺陷 P1/P2/S1 均源于此）。
- **裁决**：① README 头部、事实快照"git 基线状态"行、R4 B7 条目全部改为**执行时点稳健表述**：代码基线 = `7a617fc`，门禁 = 01 P2 双检查（`git merge-base --is-ancestor 7a617fc HEAD` + `git diff 7a617fc HEAD -- ':!plans' ':!SARibbon-3.0-plan-v2.md'` 为空），评审提交链"以执行时 `git log` 为准"；② 体系级约定入 NOTES 新条目 **B11**：计划文档中一切 git 状态断言用相对表述，绝对 commit 号只作评审轮次时点备注；③ NOTES B7 追加 round3 更新段；④ v2 头部基线行、appendix §1 `SR:` 行同步改写。
- **理由**：01-dryrun 建议 4 的"体系级约定"是根治方案——快照表加时点声明列治标，相对表述治本。
- **落实位置**：README 头部/R4/事实快照、NOTES B7/B11、v2 头部、appendix §1。

### F3-A4 v2 §6.2 组合构建矩阵的 Qml 轴分工——确认 04 已认领，分工表写入 v2

- **冲突**（01-dryrun #53/建议 5）：v2 §6.2 写"组合构建：Widgets=OFF Qml=ON、Widgets=OFF Qml=OFF 两个矩阵项"但未指明分工；Qml 在 01/02 窗口无 target（01 S6-5 占位 message，ON/OFF 等价），存在"三不管"风险。
- **裁决**：**确认计划 04 S8 已显式认领** Qml 轴——S8-2（`SARIBBON_BUILD_QML=ON SARIBBON_BUILD_WIDGETS=OFF` 组合构建 job）+ S8-3（"v2 §6.2 组合矩阵终态复核：两项齐备"）+ S7-6（linux-qt6.8 QML=ON 测试 job）；core-only（Widgets=OFF Qml=OFF）在 01 S10-2；static/amalgamation/ctest 纪律复核在 03 S5。把 7 行**门禁/矩阵项分工表**写入 v2 §6.2（脚本→01 S9、purity step→01 S10-1、core-only→01 S10-2、`--no-tests=error`→01 S10-7、core 前置→02 S9、static/amalgamation→03 S5、Qml 轴→04 S8），并注明"Qml 轴在 01/02 窗口无意义"的原因。
- **理由**：消除"谁负责哪个矩阵项"的模糊；04-dryrun 已核实 S8 文本认领成立，无需改 04。
- **落实位置**：v2 §6.2 分工表。

### F3-A5 publish 触发链终态三方对齐——确认已互洽，04 引文同步

- **冲突**（04-dryrun #2/建议 1、03-dryrun R3-03/建议 4）：round3 之前 01 S10-4（"不改文件"）、03 P4/S4.1（"暂停为 dispatch-only 后还原"）、04 S10.4（"03 恢复为 tag 触发"）三方互相矛盾。round3 各 dry-run 已分别修订 03（P4→现状确认、S4.1→"触发保持现状"）与 04（→运行时实测 + 三分支决策表），但 04 S10.4 对 03 S4.1 的**引文**仍是旧措辞（"还原 release…而非'恢复'"），且"⚠️01↔03 表述冲突供终审对齐"注记已过期。
- **裁决**：确认终态全链一致：**触发器 = `release:[published]` + `workflow_dispatch`，01 不改、03 保持现状、无任何"暂停/恢复"动作**；触发 publish 的是"创建 GitHub Release"（04 S10.5：Release 创建 = 真实发布，agent 只备料、维护者确认后执行）。04 S10.4 引文按 03 现文同步，冲突注记改为"已消除（03 round3 已更正）"，防御性三分支决策表**保留**（运行时实测纪律不因文档对齐而放松）。
- **理由**：三处 dry-run 修订后事实上已互洽，剩余仅引文措辞滞后；决策表是低成本高收益的防御设计。
- **落实位置**：04 S10.4（两段）。

### F3-A6 python vs python3 口径——统一为 01 S9 口径

- **冲突**（04-dryrun 建议 2/#13、03-dryrun R3-19）：01 S9/§6 与 04 S8 用 `python3`；README R2、02 P5/S8-3/验收门、03 P5 用 `python`。
- **裁决**：统一规则（以 01 S9 为口径源）：**文档/本地验证命令一律写 `python3`**，附注"Windows 本机若无 `python3` 用 `python` 或 `py -3`"；**CI step：linux/mac 用 `python3`，windows workflow 用 `python`**（GitHub windows runner 无 python3 命令，01 S10-1 阻塞级教训）。修正 README R2、02 P5/S8-3/验收门、03 P5 共 5 处；01 S10-1 的 windows workflow `python` step 是规则内例外，保留。
- **理由**：纯净扫描 step 在 Linux CI 上 `python` 不存在会直接挂；本地 Windows 上 `python3` 可能落到 Store stub——双向例外都必须在文本中显式。
- **落实位置**：README R2、02 P5/S8-3/§6 验收门、03 P5。

### F3-A7 绑定构建 = 第五种消费场景（文档互认）——采纳

- **冲突**（03-dryrun 建议 1）：01 S6.2/S6.4 论证 include 形态只覆盖四态（源码树/同步目录/安装树/amalgamate）；三轨绑定"直接编译源码树"是第五态，01/02 都没接住，03 S3.0 补上后缺 01 侧互认。
- **裁决**：03 S3.0 的绑定侧自建镜像（sip 系 `build-binding-include/SARibbonCore/`、pyside6 CMakeLists 内 `_sync_include/`，与 01 S8 `_amalg_include` 同构）确认为第五场景**唯一解**；01 S6.4 补交叉引用注；NOTES 新增 **B13** 记录裁决全貌（含 F3-A2）。
- **落实位置**：01 S6.4、NOTES B13。

### F3-A8 01 S5.4 缺 FLATTEN 前向注记——采纳

- **冲突**（02-dryrun 建议 2）：02 S1-操作5 将给 01 交付的 `sa_sync_include` 增加 `FLATTEN` 选项（core 平铺同步），属"计划01 交付物被计划02 修改"，01 侧无据可查。
- **裁决**：01 S5.4 说明区补前向注记（计划内小改、随 02 S1 提交、01 验收对账不视为交付物破坏）。
- **落实位置**：01 S5.4 说明第 3 条。

### F3-A9 README R2 core-only 标准命令升级注——采纳

- **冲突**（02-dryrun 建议 3）：README R2 的 core-only 命令是 01 期形态（无 TESTS=ON、无 ctest -L core）；02 S8-3 修订后的完整形态未在 README 挂接。
- **裁决**：README R2 补"计划 02 执行完后升级为 02 S8-3 形态"注（`-DSARIBBON_BUILD_TESTS=ON` + `ctest -L core --no-tests=error`，依赖 tests/widgets 的 `if(TARGET SARibbonWidgets)` 守卫）。
- **落实位置**：README R2。

### F3-A10 NOTES 预登记三条仓库事实——采纳

- **裁决**（02-dryrun 建议 5）：NOTES 新增 **B12**：① Category 滚动按钮标志双实现（:469-505 与 :1053-1091）及 02 S6-3 收敛决策（M1 唯一"二合一"，双验要求）；② `SARibbonBar.cpp:3810-3854` 注释旧代码（02 S8-2 门误报源，可 S8 清扫时删除）；③ `SARibbonBarLayout::init()` 空桩、子控件由 SARibbonBar 创建持有。
- **落实位置**：NOTES B12。

### F3-A11 03 绑定清单显式收录 RowProportion 波及——采纳（并入 F3-A2 落实）

- **裁决**（02-dryrun 建议 4）：见 F3-A2 的 03 S3.2-1 承接条（三文件实名 + 四枚举条目 + 按 02 执行结果分支）。
- **落实位置**：03 S3.2-1。

### F3-A12 02 命名空间 include 形态"两个答案"——驳回（无需操作）

- **冲突**（03-dryrun 建议 3）：称 02 S1 第 4 条（`<SARibbonCore/global/...>`）与第 5 条（平铺）矛盾。
- **裁决**：**驳回，记理由**——实查 02 现文，S1-操作4 已在其 round3 dry-run 中统一为平铺并带显式警示（"勿写 `<SARibbonCore/global/SARibbonEnums.h>`：同步目录无 global/ 层级"），02 §8 偏差清单亦有登记；矛盾在 03-dryrun 撰写时已被 02-dryrun 闭合。无需再修。
- **落实位置**：无（确认关闭）。

### F3-A13 01 S11 Config COMPONENTS 缺 Qml 分支——驳回（已闭合）

- **冲突**（04-dryrun 建议 3/#17 附注）：称 Qml 组件落 `else()` 无条件 `FOUND TRUE`。
- **裁决**：**驳回，记理由**——实查 01 S11.1 现文，其 round3 dry-run 已把存在性检查一般化为 `elseif(NOT TARGET SARibbon::${_comp}) set(SARibbon_${_comp}_FOUND FALSE)` 且 supported 清单含 `Core Widgets Qml`，Qml/Core 均被覆盖。无需再修。
- **落实位置**：无（确认关闭）。

### F3-A14 一致性套件的字体事件约定——采纳

- **裁决**（04-dryrun 建议 5）：套件级约定"字体变更测试统一改**应用字体**（`QGuiApplication::setFont`）"写入 04 S7 测试规范段（widgets=per-widget FontChange、QML=ApplicationFontChange，只改单控件字体无法同时触发两端），落点 = `tests/common/RibbonConformance.h` 头注释 + `tests/qml/` README。
- **落实位置**：04 S7 测试规范段。

### F3-A15 单例注册范式写入 dev-guide——采纳

- **裁决**（04-dryrun 建议 6）：04 S9.1 的 dev-guide QML 编码规范增补既定范式："回调式 `qmlRegisterSingletonType` + callback 内 `setObjectOwnership(CppOwnership)`，禁止 `qmlRegisterSingletonInstance`"，防后续贡献者退回 instance 式。
- **落实位置**：04 S9.1。

### F3-A16 AGENTS.md 过时窗口（S12.1 提前）——驳回（记录为执行期可选项）

- **冲突**（01-dryrun 建议 6）：S6 搬目录→S12 改 AGENTS.md 之间约 6 个提交点的文档过时窗口，可考虑把 S12.1 提前到 S6 同提交。
- **裁决**：**驳回强制提前，记理由**——① dry-run 自评"可选优化、非缺陷"；② README R1"旧规则自动映射到 src/widgets/"条款已兜底；③ 提前会扩大 S6 提交改动面、破坏"一步一提交"粒度（R3），违反 R4 保守原则。作为执行期可选项记入本报告 §五-6（多 agent 并行或执行期拉长时由执行者自行决定）。
- **落实位置**：本报告 §五-6（计划文本不动）。

### F3-A17 01 §6 门禁 B 豁免 pattern 依赖 rcc 注释格式——采纳（轻量）

- **裁决**（01-dryrun 建议 7）：03 S1-1 的 qrc bullet 补注"若日后资源变更需重生成 qrc，其生成注释内嵌路径会变化，须同步复查 01 §6 门禁 B 的豁免 pattern"。03 现设计不重生成 qrc（"资源不变则无需重生成"）、门禁 B 只在 01 期间使用，风险已受控，轻量提示即可。
- **落实位置**：03 S1-1。

### F3-A18 跨文档数字传染扫描——已执行

- **裁决**（01-dryrun 建议 3）：对四份计划共享的计数/行号/"已核实"类断言做全库 grep 交叉扫描（重点 01 S8 ↔ 03 S1 的 amalgamate 共享事实：模板 4 文件、46/43/89 计数、GBK 编码、镜像机制、sed 表达式——03-dryrun 已实测双边一致）。新发现并修复的同模式残留：CMake floor "3.16"（03 S7-1、04 S9.2，均已改 3.21）、子系统计数"六/七"（README 计划清单行与 02 S10-1，已统一为"七子系统（六+1）"）、v2 §7.1 契约字段名（并入 F3-A1）。
- **落实位置**：03 S7-1、04 S9.2、README 清单表 02 行、02 S10-1。

### F3-A19 02-dryrun 建议 6（执行顺序）——采纳进本报告 §五

- **裁决**：不改计划文本，作为派单参考写入 §五-4（spike 与 S5.0 录制并行、S4.1 契约头先于 S5.1 独立提交 + FakeItem 冒烟）。
- **落实位置**：本报告 §五-4。

### F3-A20 QML 单例注册 API 全链同步（终审扫描新发现）——采纳

- **冲突**：04 round3 已按 Qt 源码实证把单例注册从 `qmlRegisterSingletonInstance` 翻转为**回调式 `qmlRegisterSingletonType`**（instance 式硬绑首个引擎、多引擎取 nullptr），但 **v2 §5.3（"3.0 唯一路线"实现清单）、§5.4（"注册实现定为 instance 式"+ round2 批注整段方向已反）、§9-D1（理由引用该 API 名）、README R5"命令式单轨"术语行、appendix §3.3 :97 落点栏**共 5 处仍写 instance 式为定案——任何执行者对照 v2/README 都会被引回被否决的 API。
- **裁决**：5 处全部同步为回调式（含 CppOwnership 落点、否决理由与 Qt 源码证据引用），保留 round2→round3 修订痕迹；04 S1 :48 的 API 齐备清单（事实枚举）与 :70 的"不用 instance 式"警示为正当引用，不改。
- **理由**：属"注册单轨化"口径的组成部分（终审任务 B"注册单轨化一致"项），且是四份 dry-run 都未覆盖的 v2/README/appendix 侧残留（04 只 owns 自己）。
- **落实位置**：v2 §5.3/§5.4 正文与批注/§9-D1、README R5、appendix §3.3 :97 行。

---

## 二、一致性扫描结果表

扫描方式：7 份文档全量通读 + 定向 grep 交叉（每项给出全库命中核对）。"修复数"指终审本轮落实的修改点。

| # | 扫描项 | 统一口径 | 扫描结果 | 修复 |
|---|--------|----------|----------|------|
| 1 | N₀=26 | 26（25 顶层 .cpp + auto/ 1） | README/NOTES B8/01 P3/S7/§6/§8/02 P2/S5.0-3/v2 §1.2-P5/§7.3 全一致；"24/25"仅存于历史叙述 | 1（01 §8 历史括注补"终审确认 README 已改 26"） |
| 2 | ctest 过滤口径 | `ctest -L core`（LABELS），禁 `-R core` | 02 S9/03 S5-1/README R2 升级注一致；**02 S5.0-1 原"core_ 前缀或 LABELS"二选一未闭合**（只加前缀会使 S9 的 -L 零匹配假绿） | 1（02 S5.0-1 LABELS 改硬性要求，前缀降为可选增强） |
| 3 | `--no-tests=error` 适用位置 | 全部 ctest 调用统一带；amalgamation/dry-run job 现无 ctest | 01 S10-7（既有 Test step + `if: matrix.widgets=='ON'` 联动）✓、03 S5-4 ✓；**02 S9 新增 core step 原未带**（03 S5-4 却假定其已带） | 1（02 S9 补参数 + 补 linux-qt6.8 widgets=OFF 组合下与 Test step 同条件跳过的联动说明） |
| 4 | python vs python3 | 见 F3-A6 | 5 处不一致 | 5（README R2、02 P5/S8-3/验收门、03 P5） |
| 5 | publish 触发链终态 | `release:[published]`+`workflow_dispatch`，无暂停/恢复；Release 创建=真实发布 | 01 S10-4/03 P4+S4.1/04 S10.4-5 本体已互洽；04 引文与冲突注记过期 | 2（04 S10.4 引文对齐 + 注记改"已消除"） |
| 6 | 注册单轨化 | 命令式单轨；`qt_add_qml_module` 仅以历史记录/3.1+ 候选 B-3 身份出现 | 全库 grep `qt_add_qml_module` 18 处全部合规（历史/候选/预案语境）✓；**单例 API 5 处残留 instance 式**（F3-A20） | 6（v2 §5.3/§5.4 正文/§5.4 批注/§9-D1、README R5、appendix :97） |
| 7 | 契约字段/方法名 | `resultGeometry`/`debugName`/`applyGeometry`/`rowIndex(int)`/`maximumWidth`/`stretchFactor` | v2 §3.4.1 ↔ 02 S4.1-5 ↔ 04 S3/S5 逐字段比对一致（round2/round3 已同步）；v2 §4.2/§7.1 示意残留旧名 `geometry` | 3（§4.2、§7.1 字段名、§7.1 枚举实名 `SARibbonRowProportion`） |
| 8 | v2 其余与 02 终版冲突旧文 | 以 02 为准 | 误名宏 ×7、enableExpanding、clampScrollOffset 缺 isRTL、spacing=1、Metrics 输入不全、CategoryEngine Result 缺项 | 12（F3-A1 全部） |
| 9 | 选项名 | `SARIBBON_BUILD_{WIDGETS,QML,TESTS,EXAMPLES,STATIC_LIBS}`/`SARIBBON_USE_FRAMELESS_LIB`/`SARIBBON_ENABLE_SNAPLAYOUT`/`SARIBBON_INSTALL`/`SARIBBON_INSTALL_IN_CURRENT_DIR` | 全库 grep 无拼写漂移；01 S4.3 清单 ↔ README ↔ v2 §6.4 一致；旧名 `BUILD_TESTS` 仅以"现状/shim"身份出现 ✓ | 0 |
| 10 | 目录名/include 形态 | `src/core|widgets|qml`；同步目录 `build/include/SARibbonCore/` **平铺**；`<SARibbonCore/X.h>`/`<SARibbonWidgets/X.h>` | 全库一致；02 S1-4/S1-5 平铺统一 ✓；03 模板"物理子目录路径 vs include 形态"区分正确 ✓；04 `<SARibbonQml/...>` ✓ | 0 |
| 11 | CMake floor | 3.21（01 S4） | **03 S7-1 changelog 草稿与 04 S9.2 迁移指南残留 "3.16"** | 2 |
| 12 | 交叉引用锚点 | — | 02 P1→01 §6 锚点、03 P1→02 §6 锚点均有效；README 清单表 ↔ 四份文档标题一致；各文档→appendix/reviews 链接有效 | 0 |
| 13 | 版本收口清单 | 8 处（03 S7-2）+ 04 特有 2 处 | 03 S7-2 ↔ 04 S10.2 一致（04 round3 已补 pyqt6/pyproject.toml 等） | 0 |
| 14 | amalgamate 共享事实（01 S8 ↔ 03 S1） | 模板 4 文件、46/43/89 计数、GBK、`_amalg_include` 镜像、sed 表达式 | 03-dryrun 实测双边一致（含镜像继承与 set -e/ASCII 化演进链） | 0（另补 1 处 qrc 再生提示，F3-A17） |
| 15 | git 基线表述 | 见 F3-A3 | README×3、NOTES B7、v2 头部、appendix SR 行过时 | 6 |
| 16 | 术语表覆盖（README R5） | 新术语应可检索 | 缺"结果几何 resultGeometry/握手属性/叶子创建三部曲/双形态镜像"；"命令式单轨"行含被否决 API | 5（4 新词条 + 1 行更新） |
| 17 | 修订历史 | 每轮有条目 | 缺 round3 | 1 |
| 18 | 子系统计数 | 七（global/theme/metrics/contract/layout/data/factory，"六+1"） | README 清单行"六子系统"、02 S10-1"六子系统"与 02 §1"七个子系统"不一致 | 2 |
| 19 | sip 冒烟窗口 | 见 F3-A2 | 02 S1.1-4/S1 波及段/§7 风险表与 01→03 窗口事实冲突；03 无承接条 | 5 |
| 20 | 第五消费场景互认 | 见 F3-A7 | 01 S6.4 四态论证无绑定态指引 | 2（01 注 + NOTES B13） |

**合计：一致性扫描发现并修复 28 类问题、约 60 个修改点**（含裁决项落实）；另确认 9 项扫描**通过无需修改**（#9/#10/#12/#13/#14 等）。

---

## 三、完整性终审清单（对照用户目标）

用户目标："形成一个完整的改造计划，能帮我实现 SARibbon3.0（支持 qml 和 widget）；发现错误直接修改；信息不全补充；可补充文档"。

### 3.1 v2 要素 → 计划落点映射表

覆盖 v2 §2~§10 全部设计要素（55 项）。状态：✔=有明确执行落点；✔(3.1+)=设计上即为候选/前瞻条款，无 3.0 执行动作（合规）。

| v2 要素 | 计划落点 | 状态 |
|---------|----------|------|
| §2.1 分层图（三库单向依赖） | 01 S5.3（LINKS→SARibbon:: 单向）/S6（三 target）/S6.7（单 export set 自动携带依赖） | ✔ |
| §2.2 归属六问 + 铁律 | 02 §1 最高纪律/§4-4 禁双实现；02 S10-1（六问入 dev-guide checklist）；04 §1 铁律/S3（叶子不写几何）/S5（sizeHint 传导链锁定 core metrics） | ✔ |
| §2.3 KDDW 取舍表 | appendix §3/§5（裁决汇编 + 不抄清单 22 项）；02 附录 F（Tier2 预案） | ✔ |
| §3.1 core 目录结构 | 01 S6-2（骨架三件 + CMakeLists 全文）；02 S1~S4（七子目录实体化）；03 S1-1（模板按真实子目录路径） | ✔ |
| §3.2 theme（ThemeData/信号/明暗/色板） | 02 S2（S2.0 实况修正 + S2.1 数据层 + Meyers 单例定案 + QSS 留 widgets） | ✔ |
| §3.3 metrics | 02 S3（S3.0 实况/S3.1 建类含常量表/S3.2 接线含 resetSize 九触发点/度量对照表验收） | ✔ |
| §3.4.1 契约接口 | 02 S4.1（最终代码块，含 Category 扩展/头文件自足性）；04 S3 操作1（QML 侧多继承实现 + 登记列表泛化） | ✔ |
| §3.4.2 PanelLayoutEngine | 02 S5（S5.0 录制含 dump 工具规格/S5.1 StepA 含缓存不变量/S5.2 StepB 含 Input/Result 终版） | ✔ |
| §3.4.3 CategoryLayoutEngine | 02 S6（双实现收敛 scrollButtonFlags、clampScrollOffset+isRTL、Result 补项、壳全量写回） | ✔ |
| §3.4.4 BarGeometryEngine | 02 S7（下沉/留守表 + TitleRectInput 采集方式 + D6 降级 S7-3） | ✔ |
| §3.5 data（枚举 + CustomizeData） | 02 S1（附录 A 16 项盘点 + RowProportion 兼容机制）/S4.2（公有继承拆分 + simplify 纯性定案） | ✔ |
| §3.6 factory | 02 S4.3（占位头 + 降级定案 + KDDW 成本实证；D7 gate 衔接附录 F） | ✔ |
| §3.7 禁区两层化 | 01 S9（脚本 + 家族正则 + 合法名零误伤）；02 S8-3（两层执行细则 + 定向 grep）；CI 落点 = v2 §6.2 分工表 | ✔ |
| §4.1 widgets 角色转变表 | 02 全文（S5~S7 适配器化）+ 02 附录 C/D/E（函数级去向 100% 覆盖三布局头全部方法） | ✔ |
| §4.2 QLayout 适配器模式 | 02 S5.2-3（壳形态 + 外部调用者锚点）/S6-1（壳=计算+回写+hide）/S7-2 | ✔ |
| §4.3 留守清单 | 01 S6（整目录迁 widgets）；02 附录 A（widgets 枚举 7 项）/S4.2（manager/apply/make*/isCanCustomize 留守） | ✔ |
| §4.4 兼容层 | 01 S6.3（SARibbonGlobal 转发）/S6.4（colorWidgets 宏 + NO_DLL 优先）/S6.9（三全局头全文）/S6-6（双别名）/S11.2（旧包薄壳全文）/S11.3（include/SARibbonBar 转发头）；03 S6-3（复核 + 薄壳清单）；04 S9.2（迁移指南）；守卫=v2 §6.4 | ✔ |
| §5.1 混合模式（宿主+叶子） | 04 S3（宿主骨架 + 叶子组织规范 + 握手/三部曲/析构纪律 + Instantiator 评估） | ✔ |
| §5.2 类型清单 P0/P1/P2 | 04 S2（Theme/Metrics 单例）/S3（Panel）/S4（Bar/Category/Tab）/S5（ToolButton + P1 三项）；P2=04 §2 非目标（D8 触发） | ✔ |
| §5.3 注册命令式单轨 | 04 S1（骨架全文 + 附注声明式轨预案）；v2/README/appendix 口径终审已同步（F3-A20） | ✔ |
| §5.4 主题桥 | 04 S2.1（回调式单例 + CppOwnership + 双引擎哨兵） | ✔ |
| §5.5 qml 不依赖 widgets | 04 §4 红线 + S8-2（Widgets=OFF Qml=ON 组合验证）+ S8-1（纯净扫描 qml 清单） | ✔ |
| §6.1 保留项（sa_add_library/sa_sync_include/组件化 Config/命名空间 target/静态导出宏/BUILD_QML 默认 OFF/amalgamate 按模块/绑定路径） | 01 S5（两函数全文骨架）/S11（Config 全文）/S5.2（宏三段式）/S4.3；03 S1（双产物 + 出库）/S3（三轨迁移）；QML 安装布局结论 → 01 S11.5/03 S6-3 | ✔ |
| §6.2 纯净门禁 + 组合构建 | v2 §6.2 分工表（终审新增）：01 S9/S10-1/S10-2/S10-7、02 S8-3/S9、03 S5-1~5-5、04 S8 | ✔ |
| §6.3 tests/core 升格 | 02 S5.0-1（挂接点 + widgets 守卫 + LABELS）/S9（CI 前置 merge blocking） | ✔ |
| §6.4 SARIBBON_INSTALL | 01 S4.3/S5.3/S5.4/S6.7/S11（全 install 入守卫）；03 S6-3（默认 ON 复核 + OFF 无产物） | ✔ |
| §7.1 黄金几何测试（含双介质/断言双轨/诊断 dump） | 02 S5.0-2（fixture 四项补强 + JSON 入库）/S5.0-3（录制 + 固定字体）/S5.2-4/S6-4/S7-4/S8-1（矩阵四类补强） | ✔ |
| §7.2 跨前端一致性套件 | 04 S7（RibbonConformance.h + 双端测试 + 固定字体 + offscreen/QTRY 规范 + CI） | ✔ |
| §7.3 现有 26 单测迁移 | 01 S7（搬迁含 auto/ + N₀）；02（全程 ctest==N₀）；03 S6-5（链接切 SARibbon::Widgets + 有效复核命令） | ✔ |
| §7.4 错误零容忍 | 前瞻条款 → §9 候选 B-4，无 3.0 执行动作（设计自洽） | ✔(3.1+) |
| §8-M0（四项判据） | 01 S1（分支）/S5（Utils）/S6（三模块骨架，"三空 target"字面偏差已登记 01 S6-5）/S9+S10（扫描 + CI） | ✔ |
| §8-M1（六项判据①~⑥） | 02：①S1 ②S2 ③S3 ④S5~S7 ⑤S5.0+S8 ⑥S4.2；验收门=v2 M1 判据逐条对应 | ✔ |
| §8-M2 | 03（验收门=M2 判据：CI 绿/StaticExample/轮子） | ✔ |
| §8-M3 | 04 S1~S8（验收门首三条=M3 判据：同屏一致/一致性测试绿/examples/qml） | ✔ |
| §8-M4 | 04 S9（文档三模块 + QML API + 迁移指南含度量对照表链接）/S10（版本/tag/Release/轮子） | ✔ |
| §8-Tier 分级 | 02 §2 非目标 + 附录 F；04 §2 非目标 + 附录 A（Tier2 预案两份） | ✔ |
| §8-依赖序（M0→M1→M2→M4；M3 依赖 M1 可并行 M2） | 03 头部（04 可并行、建议线性）；04 头部（M3 只依赖 M1） | ✔ |
| §9-D1（Qt 5.15） | 01 S4-4（MIN_QT_VERSION）；04 S9.4（badge 5.14+→5.15+ 双文件） | ✔ |
| §9-D2（target 命名 + 别名） | 01 S5.3（SARibbon::）/S6-6（SARibbonBar 双别名）；03 S6-5（内部切换、别名留一周期） | ✔ |
| §9-D3（GalleryItem 留 widgets） | 02 附录 A（Gallery 枚举留守）；01 S9（core 禁 QAction） | ✔ |
| §9-D4（colorWidgets 宏并入） | 01 S6.4（SA_COLOR_WIDGETS_API→WIDGETS_EXPORT + NO_DLL 优先分支保留） | ✔ |
| §9-D5（混合模式） | 04 S3（全面实证落地，round2/round3 两轮加固） | ✔ |
| §9-D6（Bar 引擎降级） | 02 S7-3（S7 开始时评估 + NOTES；Panel/Category 不许降） | ✔ |
| §9-D7（controller gate Tier2） | 02 §2/附录 F（gate 写死）；04 S3/S4（不引 controller）+ 附录 A | ✔ |
| §9-D8（core Action 推迟） | 02 S4.3（406 行成本实证）；04 S5.2（QAction 不桥接 + 不学 KDDW 理由） | ✔ |
| §9-B-1（SizingInfo 结构体化） | 02 S5.2-2"M1 严禁顺手结构体化"+ SizeHintCollection 唯一例外 | ✔(3.1+) |
| §9-B-2（CI 多编译器轴） | 03 S5（3.0 范围锁定，不扩轴） | ✔(3.1+) |
| §9-B-3（声明式轨） | 04 S1 附注（Qt6QmlMacros 硬约束预案完整保留） | ✔(3.1+) |
| §9-B-4（错误零容忍） | v2 §7.4（记录在案） | ✔(3.1+) |
| §9-B-5（多引擎时序 × Q_INIT_RESOURCE） | 04 S1 验证（static 组合 Q_INIT_RESOURCE 检查）/S2 验证（双引擎哨兵）；残余列 04 遗留风险 1/2 | ✔（观察项） |
| §9-B-6（Loader 转发） | 04 S3 叶子规范（P0 不引 Loader；取宿主二选一勿混用） | ✔(3.1+) |
| §9-B-7（Controls2 链接） | 04 S1 操作1（条件链接）+ S5（执行期定案 + NOTES）；01 S11.1（Config 注） | ✔（执行期定案） |
| §10-R1（提取回归） | 02 §1 纪律/§7 风险表行1（两步走 + 先录基线 + 禁改算法） | ✔ |
| §10-R2（重入守卫） | 02 S5.2-3/S6-3（Panel 与 Category 两套守卫明示留适配器）；04 无 QLayout 天然免疫 | ✔ |
| §10-R3（度量默认值漂移） | 02 S3 验证（metrics-comparison.md 逐项相等 + 同字体/style/明暗条件） | ✔ |
| §10-R4（QML 宿主性能） | 04 §7 风险表行1（引擎缓存 + updatePolish 合帧 + resize 压测） | ✔ |
| §10-R5（字体差异） | 02 S5.0-3（部署字体 + QFontInfo 断言）；04 S7 操作3（双端复用同一字体） | ✔ |
| §10-R6（Tier2 拖延漂移） | 04 S7（一致性套件持续 CI 对拍） | ✔ |
| §10-R7（双分支同步） | README R6 + 04 S10.6（support/2.x；引擎 fix 手动同步 + 黄金测试） | ✔ |
| §10-R8（纯净性悄坏） | 01 S9/S10（CI 前置 merge blocking）+ 02 S8-3 两层 | ✔ |
| §10-R9（漏调注册函数） | 04 S1（导入专项检查）/S6.2（示例模板固定调用）/S9.2（迁移指南）+ §7 风险表 | ✔ |

**覆盖率结论：55/55 = 100%**。无缺失项（终审未发现"v2 有设计、四份计划无落点"的要素）；其中 6 项为 3.1+ 候选/前瞻条款，按设计无 3.0 执行动作，均有 NOTES/观察项挂接。

### 3.2 四份计划各自闭环

逐份核对"前置条件→步骤（操作/验证/提交）→验收门→风险→偏差登记"五段结构：

| 计划 | 前置门 | 步骤闭环 | 验收门 ↔ 步骤产物 | 结论 |
|------|--------|----------|-------------------|------|
| 01 | P1~P5（P1/P2 已改执行时点双门禁，可机检） | S1~S12 每步有操作/验证/提交信息；S6.9 代码块、S11.2 薄壳等"需发明"点已全文给出 | §6 十条门分别覆盖 S2(--follow)/S6+S7(构建+ctest)/S6(core-only)/S9(purity)/S10(CI)/S6+S7(截图)/S8(StaticExample)/S11(find-package)/S3(submodule)/S12+§6(残留门禁 A+B 含逐文件对账清单) | 闭环 ✔ |
| 02 | P1~P5（N₀=26 已修正，锚点链有效） | S1~S10 每子步独立提交 + ctest==N₀；附录 A~E 函数级去向 100% 覆盖（dry-run 完备性核对） | §6 八条门覆盖 S5.0(fixture 零修改)/S5~S7(黄金 100%)/S8-3(core-only)/S8-3(purity)/§4-2(6 截图)/S3(度量对照表)/S8-2+机检脚本(公共 API 与算法零残留)/全程(NOTES) | 闭环 ✔ |
| 03 | P1~P6（P4 触发器现状确认已改，无死门） | S1~S7；两处提交时序（S1-3 两笔提交序）保证每提交点 fresh clone 可构建 | §6 十一条门覆盖 S1(产物健全性 grep + ls-files 顶层 pathspec)/S5(CI 全矩阵)/S2(StaticExample)/S3+S4(轮子冒烟)/S3.2-2(清单对齐)/S7(版本 8 处)/S3.5(pyexamples)/S5-3(vcpkg)/S6-3(安装树)/§6(残留 grep)/S6-5(tests 链接) | 闭环 ✔ |
| 04 | P1~P6（P4 已改只读文件核对，可执行） | S1~S10；注册/宿主/叶子/测试骨架均照抄级 | §6 九条门覆盖 S6(同屏对比 + 客观基准)/S7(一致性套件)/S8(QML CI + 组合)/S8(纯净)/铁律四判据(含 grep -v Engine 修正)/S9(文档 nav)/S10(tag+Release+publish)/全程(ctest)/NOTES | 闭环 ✔ |

无悬空步骤（每个步骤产物至少被一条验收门或下一步 P 门核对）；round3 修订后所有"验证"条目均为可执行命令或明确人工规程（含期望输出）。

### 3.3 执行者零发明原则抽查（8 个高风险步骤，每计划 ≥1）

| 计划-步骤 | 只凭文档能否执行 | 依据 / 残余缺口 |
|-----------|------------------|-----------------|
| 01 S6.9 代码块 C（兼容映射 + 三段式） | ✔ | 全文给出 + 宏求值顺序论证（round3 重排）+ Template 头部定义位置证据 + BOM/CRLF 编码警示 |
| 01 S11.2 旧包名薄壳 | ✔ | .in 全文 + 生成/安装命令归属（src/CMakeLists.txt、SARIBBON_INSTALL 守卫）+ 三要点（find_dependency/相对路径/ConfigVersion 同装）（round3 补全） |
| 02 S1.1 Q_ENUM 五步策略 | **部分 ✔** | 机制完整（自由枚举 + 类内别名 + 4 条枚举符 using + flags 三件套处置 + :704 锚点 + 降级出口）；但"别名 + Q_PROPERTY 的 moc 行为、sip/shiboken 解析"是工具链灰区，静态推演无法替代实跑——已定义降级出口（不阻塞），spike 建议见 §五-2a |
| 02 S5.0-3 dump 工具 | ✔ | 采集规格逐 API 给出（itemAt+dynamic_cast/panelTitleLabel()->geometry()/findChild 精确类型/两个公式的行号）+ JSON schema 最小集 + 字体方案（round3 补全） |
| 03 S1-2 目标脚本 | ✔ | ASCII 全文（`bash -n` 通过 + 镜像段沙箱真跑）+ set -e + 产物存在性校验 + 编码操作规程（iconv 唯一安全通道）（round3 重写） |
| 03 S3.0 绑定镜像前置 | ✔ | 三轨落点明确（bat + publish/dry-run CI 两处前置 step；pyside6 CMakeLists 自足 file(COPY)）+ 备选方案否决理由 + .gitignore |
| 04 S1.3 注册函数 | ✔ | 骨架全文 + 回调式单例（双版本签名行号）+ 单例注册坑 Qt 源码实证 + static-once/Q_INIT_RESOURCE 守卫 |
| 04 S3 宿主骨架 | ✔ | 类声明级骨架 + 全部 QQuickItem API 访问级别/行号核实 + 契约项形态 round3 定案（多继承，消除"或"二选一）+ updatePolish 数据流注释到字段级 |

结论：8 步中 7 步"只凭文档可执行"；1 步（02 S1.1）存在**已声明、有出口**的工具链灰区（属"只有实跑才能消除"的客观限制，非文档缺陷）。

### 3.4 文档体系自包含

- **不依赖 v1**：README"计划文档自包含性说明"给出悬空引用处理约定（内联为准/无法还原记 NOTES 澄清/禁止想象）；四份计划头部的 v1 引用均已内联或标注（01 头部说明、03【内联-1..5】、04 S8/S10 标注）✔
- **不依赖本机绝对路径**：`F:\src\3rdparty\*`（参考项目）→ README 第 4 点给出 submodule init（pin f93657f）/clone URL + "行号漂移以符号名为锚"约定 ✔；`D:\Qt`（构建环境）→ 属环境要求，README R2/B5 写明 Qt5 以 CI 为准、Qt6.7.3 本机主验证 ✔
- **参考项目证据可追溯**：appendix §1 前缀表（QWK/qmsetup/QWK-upstream/KDDW/SR 五个前缀 + 各自版本/时效警示）✔

### 3.5 AGENTS.md 合规性

- `Q_SLOTS/Q_SIGNALS/Q_EMIT`：02 S2.1 显式强调（"注意拼写 Q_SIGNALS"）；04 S3 骨架"Q_INVOKABLE 转 Q_SIGNALS"；全部计划代码块无裸 `emit/signals/slots` ✔
- 注释规范（.h 单行英文/类注释双语/PIMPL）：01 S6.9 块 A "双语注释随行整段 move"；04 S3 骨架 d_ptr+PrivateData 照仓库规范；02 S10-1/04 S9.1 安排 dev-guide 更新 ✔
- "禁止读取或修改 src/SARibbon.cpp/.h"：01 S8 仅经 `tools/Amalgamate.sh` 重生成（AGENTS.md 认可路径）且明文"产物仍是生成物，禁止手改"；03 S1-4 更新禁令措辞，且按 S1-3 提交时序与出库动作同在提交①，无"禁令与仓库现实脱节"的提交点 ✔
- **过渡窗口时序**（终审确认）：01 S6（目录搬移）→ 01 S12（AGENTS.md 结构图更新）之间约 6 个提交点，README R1"旧规则自动映射到 src/widgets/"条款兜底（F3-A16 裁决维持该窗口）；02 全程在 01 S12 之后执行，面对的是已更新的 AGENTS.md；03 S1-4 的第二次更新（ amalgamate 禁令）与其出库动作同提交。时序讲清 ✔
- 计划中无任何"执行期行为指令"违反 AGENTS.md 其余禁令（类型安全压制/双语 Doxygen 滥用等无涉及）✔

---

## 四、终审结论

### 判定：**可交付执行**

理由：

1. **阻塞级缺陷已清零**：round3 四个 dry-run 共修复 25 条阻塞级缺陷（01×7、02×9、03×6、04×3），全部附实测证据与修订落点；终审复核其修订文本均已落盘（01~04 工作区修改与 findings 一一对应）。
2. **跨计划冲突已裁决**：任务列出的 5 项待裁决项 + findings 中全部"供终审/跨文档"项（含终审扫描新发现的单例 API 链、CMake floor 残留等）共 20 项裁决完毕（§一），采纳 16、驳回 3（均已闭合或记录为可选项）、执行扫描 1；七份文档两两交叉后口径一致（§二，28 类问题约 60 处修复）。
3. **设计→执行全覆盖**：v2 §2~§10 共 55 项要素 100% 有落点或明确的 3.1+ 候选归属（§3.1），无三不管地带；矩阵项分工表已写入 v2 §6.2。
4. **执行者可零发明运行**：抽查 8 个高风险步骤，7 个可只凭文档执行，1 个（02 S1.1）灰区有降级出口与 spike 建议（§3.3）。
5. **剩余风险全部属"执行期才能验证"类**（真实构建/CI/绑定/Qt Quick 环境），且每项在计划内已内置缓解、回退或 NOTES 记录要求（下表）——不构成文档缺陷，不降级交付判定。

### 剩余执行期风险汇总（四份 dry-run 遗留风险节归并去重，21 条 → 4 类）

**A. 工具链灰区（建议开工前 spike，见 §五-2）**

| # | 风险 | 来源 | 计划内置缓解/回退 |
|---|------|------|-------------------|
| A1 | Amalgamate.exe 对镜像 `-i` 的 `<尖括号>` 解析、"解析不到原样保留"、按路径去重的实际行为 | 01 遗留1 + 03 遗留1（同根合并） | 01 S8.5 diff 核对 + StaticExample 验收门 + 03 S1"产物内容健全性 grep"硬门；回退=模板顶部手工插 core 头实体路径 |
| A2 | Q_ENUM 下沉：类内 using 别名 + Q_PROPERTY 的 moc 行为、Q_ENUM_NS 反射等价性、sip/shiboken 对别名与枚举符 using 声明的解析 | 02 遗留1 | 独立小工程 spike（Qt5.15+Qt6.8 双工具链）；失败→"四枚举留 widgets"降级出口（02 S1.1 末），枚举下沉非 M1 验收硬门 |
| A3 | sipbuild 配置双源（project.py vs pyproject.toml `[tool.sip]`）优先级、`module-name` 键行为 | 03 遗留2 | `sip-build --verbose` 定案；两套同步改、不删任何一个（保守） |
| A4 | builder-settings `RESOURCES` 相对基准与 qrc 主题资源可加载性 | 03 遗留3 | S3.2 冒烟时实测（"待核实"项已登记） |

**B. 构建/CI 环境依赖（需真实环境或 CI 验证）**

| # | 风险 | 来源 | 缓解 |
|---|------|------|------|
| B1 | Qt5.15 路径（本机仅 5.14.2 低于门槛）：C++17 统一、5.15 门槛、shim 行为只由 CI 验证 | 01 遗留7 | qt5.15 双 workflow 覆盖；S6 提交点若 CI 未跑，风险延后到 S10 暴露（已知） |
| B2 | N₀=26 为注册数推定（P3/P5 构建未真跑）；01/02 执行期增删测试会漂移 | 01 遗留3 + 02 遗留5（合并） | 01 P3/02 P2 均已写"以实测为准"口径 |
| B3 | 空导出 DLL 的 MSVC 告警面（LNK4088 等）是否干扰 CI | 01 遗留4 | 警告非错误，预期可过；执行时确认 |
| B4 | `target_link_libraries(<tgt> PUBLIC <空列表>)` 展开（02+ 若出现空参调用） | 01 遗留5 | 三模块调用均传非空 QT_LINKS；出现空参再补 if 守卫 |
| B5 | `configure_package_config_file` 对 `@SARIBBON_BUILD_*@` 的作用域（变量未定义代入空串使 if() 报错） | 01 遗留8 | 按 S11.1 指定位置（src/CMakeLists.txt 顶层）写即无此问题 |
| B6 | build.ps1 `rebuild` 附带 install：S6~S10 期间安装树为中间态（无 Config） | 01 遗留9 | S11 时序说明"期间不做安装态消费验证" |
| B7 | static 矩阵项 × tests POST_BUILD DLL copy / core-only 组合联动 | 03 遗留5 | CI 实跑验证（03 S5-1 static 项） |
| B8 | vcpkg preset 实配需 VCPKG_ROOT + 网络 | 03 遗留6（01 S10-3 同） | 无环境降级为 JSON 语法核查（01 S10-3 已有出口，03 沿用） |
| B9 | lupdate 目录搬移后 .ts diff 噪音需 Linguist Tools 环境 | 03 遗留7 | `-DSARIBBON_UPDATE_TRANSLATIONS=ON` 构建核对 |
| B10 | 三轨轮子同环境并存（PyQt5+PyQt6+PySide6）插件路径干扰 | 03 遗留4 | 每轨独立 venv 或拆 3 job（03 S4-3）；失败先拆环境再怀疑绑定 |
| B11 | ASCII 化脚本在 chcp 936/65001 控制台的显示 | 03 遗留8 | 风险极低（英文提示无编码依赖） |

**C. Qt Quick 运行时行为（需真实 Qt Quick 环境）**

| # | 风险 | 来源 | 缓解 |
|---|------|------|------|
| C1 | 回调式单例多引擎实测（引擎销毁顺序 × core Meyers 单例存活期） | 04 遗留1 | tst_themeBridge 双引擎重复断言 + 先后销毁再断言（04 S2 验证，兼作 instance 式回归哨兵） |
| C2 | `Q_INIT_RESOURCE(saribbon_qml)` × sa_add_library AUTORCC 符号名交互（=v2 候选 B-5） | 04 遗留2 | `STATIC_LIBS=ON` 组合 `QFile::exists("qrc:/SARibbon/...")` 检查（04 S1/S6 验证） |
| C3 | updatePolish 触发时机 × offscreen QPA 的 exposed 语义 | 04 遗留3 | `qWaitForWindowExposed` + `QTRY_COMPARE`（04 S7 骨架）；白盒路径可绕过 |
| C4 | 叶子 implicit 尺寸是否反向干扰宿主/上层排布 | 04 遗留4 | sizeHint 传导链已锁定 core metrics（04 S5 操作1）；示例 resize 压测观察 |
| C5 | QuickControls2 库侧链接决策（=v2 候选 B-7） | 04 遗留5 | 04 S5 执行期定案 + NOTES |
| C6 | 同屏视觉一致性最终人工确认 | 04 遗留6 | 客观基准=一致性套件黄金值双端断言（04 §6 门首条已补），截图为佐证 |

**D. 已登记的受控事项（执行纪律提醒，非缺陷）**

| # | 事项 | 来源 | 约束 |
|---|------|------|------|
| D1 | scrollButtonFlags 收敛是 M1 唯一"两份源码合成一份"操作 | 02 遗留2 | §4-4 授权；NOTES B12-① + CategoryVisibilityTest + 滚动/动画冒烟双验 |
| D2 | 02 §6 public API 机检脚本是近似门（顺序不敏感、宏生成声明不可见） | 02 遗留4 | 三布局头已核实无宏生成声明；语义偷换靠黄金测试兜底 |
| D3 | dump 工具依赖运行时对象树（工厂替换 optionButton 类型会失配） | 02 遗留3 | 录制用默认工厂 + findChild 断言 fail-fast（已入规格） |
| D4 | `git mv` 未初始化 submodule gitlink 的 git 版本差异 | 01 遗留2 | S3.3 手动回退流程 |
| D5 | qmsetup 远端行号引用本地不可复核 | 01 遗留6 | 论证性引用，不影响执行动作 |
| D6 | 03 若重生成 qrc → 01 §6 门禁 B 豁免 pattern 需复查 | 01 建议7 | 03 S1-1 已补注（F3-A17） |
| D7 | v2 三处冲突旧文 / README 快照过期（02 遗留6/7） | 02 遗留6/7 | **本轮终审已消除**（F3-A1/F3-A3），不再遗留 |

---

## 五、对维护者的建议（执行前准备事项清单）

1. **先提交 round3 评审产物再启动执行**：当前 01~04 有未提交的 round3 修订、`reviews/round3/`（含本文件）未跟踪。建议按 `42b7dcc`/`eba8ebd` 同款模式提交一笔"文档：3.0 改造计划第3轮评审修订（dry-run 压测与终审）"，然后从 01 S1 开始（S1 的"nothing to commit 属正常"分支即为此设计）。本终审按任务边界未执行任何 git 操作。
2. **三个开工前 spike**（均为状态变更操作，dry-run 被禁未做；每个 30~60 分钟级）：
   - **a. Q_ENUM 下沉工具链 spike**（02 S1 开工前，风险 A2）：独立小工程验证"自由枚举 + 类内 using 别名 + Q_PROPERTY + 枚举符 using 声明"在 Qt5.15 与 Qt6.8 双工具链下的 moc 行为，以及 sip/shiboken 对该形态的解析；失败即启用 02 S1.1 降级出口（四枚举留 widgets），不要在本体上反复试错。
   - **b. Amalgamate.exe 镜像行为 spike**（01 S8 开工前，风险 A1）：按 01 S8 第 3 条镜像机制搭最小样例（一个转发头 + `_amalg_include` 镜像 + 一次 exe 调用），验证 `-i` 目录对 `<SARibbonCore/...>` 生效、解析不到时原样保留、按路径去重的产物冗余可接受；不符即启用回退方案（模板顶部手工插 core 头实体路径）。
   - **c. sip-build 配置双源确认**（03 S3.1 前，风险 A3）：`sip-build --verbose` 一次，确认 project.py 与 pyproject.toml 的实际优先级。
3. **环境要求清单**：
   - PyPI Trusted Publishing（OIDC，`environment: pypi`）在 04 S10 前由维护者就绪；**GitHub Release 创建 = 真实发布**，只能由维护者执行（04 S10.5 顺序警告）。
   - 本机无 Qt5.15：一切 Qt5 结论以 CI 为准（README R2/NOTES B5），不得以本机 5.14.2 下结论。
   - vcpkg（`VCPKG_ROOT` + 网络拉取）供 01 S10-3/03 S5-3 preset 验证；缺失走已登记的降级路径（JSON 语法核查）。
   - Python 绑定构建环境（sip、shiboken6、PySide6、Python 3.10~3.12）；本机缺失时按 03 S3 末注交 CI dry-run 验证并记 NOTES，不许静默跳过。
   - **固定字体**：02 S5.0 录制前下载开源字体（建议 OFL 许可，如 Noto Sans SC / DejaVu Sans）入 `tests/core/fonts/`——录制、回放、一致性套件、截图对比必须同一字体文件（02 S5.0-3/04 S7 操作3）。
   - 跑 `build.ps1 rebuild/clean` 前关闭运行中的示例/测试程序（Read-Host 交互挂起，README R2）。
4. **派单与执行顺序**：
   - 每计划一个执行 agent、线性 01→02→03→04（03/04 技术上可并行——M3 只依赖 M1——但建议线性避免目录/CI 同时变动互踩，03 头部已有同样建议）；交接时下一执行 agent 必须：复核上一计划 §6 验收门全勾（P1）+ 通读 NOTES.md 全部条目 + README R1~R6。
   - 02 内部（02-dryrun 建议 6，不改计划文本）：S1.1 spike 与 S5.0 基线录制是两条可并行的前置长杆；**S4.1 契约头（SA_RIBBON_CORE_EXPORT 正名版代码块）应在 S5.1 之前独立提交**并用 tests/core 的 FakeItem 编译冒烟，避免 Step A 大改与契约设计问题纠缠。
5. **执行期待维护者/执行者定案的决策点**（均有 NOTES 记录要求，无需现在回答）：
   - 02 S4.1-3：`itemWillSetGeometry` 保留引用成员 vs 记录为 3.0 允许源码破坏项（二选一）。
   - 02 S1.1：Q_ENUM 下沉失败时是否触发降级（四枚举留 widgets）。
   - 03 S1-3-4：`share/*_amalgamate` 安装规则删除 vs `if(EXISTS)` 守卫 + 改名（二选一）。
   - 03 S1-2：Amalgamate.sh 是否保留中文提示（默认 ASCII 化；坚持中文只允许 iconv 通道）。
   - 04 S5：QuickControls2 库侧链接（B-7 定案）。
   - 04 S8.1：qml 纯净扫描落地 a（零改脚本）vs b（`--module` 开关），以 01 S9 脚本实际分层为准。
6. **可选优化（终审未采纳为强制项）**：若执行期拉长或多 agent 并行，可把 01 S12.1（AGENTS.md 结构图）提前到 S6 同提交以缩小文档过时窗口（01-dryrun 建议 6；终审驳回强制提前，理由见 F3-A16，由执行者视情形自决）。

---

## 附：终审修订统计

| 文件 | 权属 | 修订内容 | 编辑次数 |
|------|------|----------|----------|
| SARibbon-3.0-plan-v2.md | 终审 owns | 头部基线行 + round3 修订说明；宏名 ×7；§3.3 spacing + 输入注；§3.4.2 enableExpanding + 终版注；§3.4.3 isRTL + Result 注；§4.2/§7.1 字段名；§5.3/§5.4 单例 API + 批注重写；§6.2 分工表；§9-D1 | 12 |
| plans/3.0/README.md | 终审 owns | 头部基线；R2 python3 口径 + core-only 升级注；R4 B7 条 + B11~B13 摘要条；R5 术语（命令式单轨行 + 4 新词条）；修订历史 round3；事实快照 git 行；清单表 02 行计数 | 9 |
| plans/3.0/NOTES.md | 终审 owns | B4 前向注；B7 round3 更新段；新增 B11/B12/B13 | 3 |
| plans/3.0/appendix-reference-architecture.md | 终审 owns | §1 SR 行时点稳健化；§3.3 :97 单例注册回调式 | 2 |
| plans/3.0/01-infra-restructure.md | 小修 | S5.4 FLATTEN 前向注；S6.4 第五消费场景互认注；§8 历史括注 | 3 |
| plans/3.0/02-core-sinking.md | 小修 | S1.1-4 sip 冒烟降级；S1 绑定波及段；§7 风险表行；P5/S8-3/验收门 python3；S5.0-1 LABELS 硬性化；S9 `--no-tests=error` + 矩阵联动；S10-1 七子系统 | 9 |
| plans/3.0/03-build-ecosystem.md | 小修 | S3.2-1 枚举别名化承接条；P5 python3；S7-1 CMake 3.21；S1-1 qrc 再生提示注 | 4 |
| plans/3.0/04-qml-and-release.md | 小修 | S9.2 CMake 3.21；S10.4 引文对齐 + 冲突注记更新；S7 字体事件约定；S9.1 单例范式入 dev-guide | 5 |
| 本文件（final-audit.md） | 新建 | 终审报告 | 1 |

未重排任何 S 编号；未改动四份计划的步骤结构与既有 round3 修订；未执行编译构建与 git 提交。
