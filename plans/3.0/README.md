# SARibbon 3.0 重构执行计划（总索引）

> 目标读者：执行重构的 AI agent（以及复核的人员）。
> 设计依据：[SARibbon-3.0-plan-v2.md](../../SARibbon-3.0-plan-v2.md)（"算法与契约下沉"方案，下称 **v2 计划**）。
> 基线：SARibbon 2.9.5（分支 `dev`，单 target `SARibbonBar`）。截至 2026-09-29，`dev`、`v3`、
> `origin/dev` 三者同指 commit `7a617fc`，工作区当前 checkout 在 `v3` 分支（见文末事实快照与
> [NOTES.md](NOTES.md) B7）。v1 计划文件从未随仓库归档（见下文"计划文档自包含性说明"）。

本目录把 v2 计划的 M0–M4 里程碑拆成 **4 份可独立执行的重构计划**，每份计划都是
自包含的：包含前置条件、逐步操作、验证命令、提交点与验收门。执行时**严格按顺序**，
不跳步、不在"红"状态下跨步。另有一份**参考资料**（非执行计划）：
[appendix-reference-architecture.md](appendix-reference-architecture.md)（参考架构学习手册，
QWindowKit/KDDockWidgets 的已裁决借鉴模式总表与"明确不抄清单"——执行 01~04 时遇到
"参考项目怎么做的"类问题先查它，不必重读参考项目源码）。

## 计划清单与执行顺序

```
01 仓库重排与现代化构建骨架 ──► 02 Core 下沉与布局引擎 ──► 03 构建生态收尾 ──► 04 QML 首版与发布
      (M0)                        (M1, 关键路径)              (M2)                (M3 + M4)
```

| # | 文档 | 对应里程碑 | 一句话目标 | 出口判据（摘要） |
|---|------|-----------|-----------|-----------------|
| 01 | [01-infra-restructure.md](01-infra-restructure.md) | M0 | 目录重排为 3.0 终态布局（`src/core|widgets|qml`、顶层 `3rdparty/`、`examples/`、`tests/` 重组），CMake 现代化，三模块真实 target，core 纯净性门禁就位 | 全部代码仍为 2.9.5 逻辑但构建全绿（ctest == N₀）；core-only 可独立编译；CI 含纯净扫描；新旧 include 路径均可被外部 `find_package` 消费；示例截图与基线一致 |
| 02 | [02-core-sinking.md](02-core-sinking.md) | M1 | theme/metrics/contract/layout/data/factory 六子系统下沉 core；三个布局类退化为适配器；黄金几何测试证明行为零变化 | v2 M1 全部判据：黄金测试 100% 绿、纯净扫描绿、现有测试全绿（ctest == N₀）、6 张截图与度量对照表逐项相等 |
| 03 | [03-build-ecosystem.md](03-build-ecosystem.md) | M2 | amalgamate 按模块改造且产物不入库；sip/PyQt6/PySide6 绑定适配；CI 全矩阵；安装细节 | CI 绿；单文件产物可编译 StaticExample；Python 轮子可构建；回归 ctest == N₀ 且 tests/core 全绿 |
| 04 | [04-qml-and-release.md](04-qml-and-release.md) | M3+M4 | SARibbonQml P0 类型（C++ 结构宿主 + QML 叶子）；跨前端一致性套件；文档、迁移指南、3.0.0 发布 | QML 与 widgets 同屏视觉一致；一致性测试绿；发布物料齐备（tag `v3.0.0` + GitHub Release + 迁移指南） |
| 附录 | [appendix-reference-architecture.md](appendix-reference-architecture.md) | —（**参考资料，非执行计划**） | 参考架构学习手册：QWK/KDDW 机制借鉴总表（机制｜实证 文件:行｜SARibbon 落点｜抄/不抄/改造）、两家共同点提炼、明确不抄清单 | —（无出口判据；随评审轮次由整合 agent 维护，见其 §6） |

## 通用执行规则（所有计划共用，执行前必读）

### R1. 项目规范优先

[AGENTS.md](../../AGENTS.md) 的全部禁令在 3.0 重构期间继续有效并同等适用于新目录：

- `Q_SLOTS` / `Q_SIGNALS` / `Q_EMIT`（禁 `slots`/`signals`/`emit`）。
- .h public 函数仅单行英文注释；Q_PROPERTY 上不加注释；类注释仅用
  `@brief`/`@details`/`@note`/`@see`，双语 `\if ENGLISH/\if CHINESE` 块照 v2 计划 §3.2 样例。
- PIMPL 惯用式（`SA_RIBBON_DECLARE_PRIVATE` + cpp 内 `PrivateData` + `SA_D/SA_DC`）。
- 涉及 `src/SARibbonBar/` 的旧规则，目录搬移后自动映射到 `src/widgets/`（计划 01 S11 会同步更新 AGENTS.md）。
- **新增文件一律 UTF-8 无 BOM；禁止重编码任何既有文件，禁止全局加 `/utf-8` 编译选项**（仓库内存在非 UTF-8 编码的历史文件，重编码会产生海量噪音 diff）。

### R2. 构建与测试验证命令

Windows 本机（本仓库的开发机，Git Bash + PowerShell）。以下命令形态已对照
`scripts/build.ps1` 的 param 块与本机工具版本核实（2026-09-29）：

```bash
# 全量重建 + 单元测试（脚本自动探测 Qt；本机主验证用 Qt 6.7.3）
pwsh -NoProfile -File scripts/build.ps1 rebuild -Tests ON -Examples ON

# 跑测试（脚本构建目录为 build/，VS 多配置生成器）
ctest --test-dir build -C Release --output-on-failure
```

已核实的脚本行为与注意事项：

- `build.ps1` 的 action 为位置参数（`configure|build|install|clean|rebuild|full|help`），
  选项为 `-QtPath/-VSVersion/-Config/-Examples/-Tests/-StaticLibs/-Frameless/-SnapLayout`
  （均 `ON|OFF`），`rebuild -Tests ON -Examples ON` 形态合法；`rebuild` = clean + configure +
  build + **install**（会顺带安装到 `bin_qt*` 目录）。
- **`rebuild`/`clean` 在检测到 `build/bin` 下有被占用的 exe 时会 `Read-Host` 交互询问**——
  非交互 agent 执行前先关闭运行中的示例/测试程序，否则脚本会挂起（AGENTS.md 已有同样提示）。
- Qt 自动探测在 `D:\Qt`、`C:\Qt` 等根下搜索 `msvc2019_64|msvc2022_64` 目录并按**版本目录名
  字符串降序**取第一个：本机（`D:\Qt` 有 5.14.2/6.4.0/6.7.3/6.10.1）字符串序 `6.7.3 > 6.10.1`，
  恰好选中 6.7.3；若日后安装更高版本 Qt 导致选中结果变化，用 `-QtPath` 显式指定。
- 脚本 configure 时传的是**旧选项名 `-DBUILD_TESTS`**（`build.ps1:332`），与现根 CMakeLists 的
  `option(BUILD_TESTS ...)` 匹配；计划 01 S4 改名为 `SARIBBON_BUILD_TESTS` 后靠兼容 shim 继续可用。
- `ctest --test-dir` 需 CMake ≥ 3.20（本机 cmake/ctest 3.31.11，满足）；VS 多配置生成器下
  `-C Release` 选择配置的方式正确。测试可执行文件输出在 `build/tests/<Config>/`，DLL 已由
  `tests/CMakeLists.txt` 的 POST_BUILD 复制到同目录（仅动态库构建时）。
- 当前基线的 ctest 注册项为 **26**（见文末事实快照"测试"行），计划 01 P3 记录的 N₀ 应以此对照。

core-only / 纯净性（计划 01 引入后的标准门禁，原始命令形式）：

```bash
cmake -S . -B build-3.0-coreonly -DCMAKE_PREFIX_PATH=<Qt路径> \
      -DSARIBBON_BUILD_WIDGETS=OFF -DSARIBBON_BUILD_EXAMPLES=OFF
cmake --build build-3.0-coreonly --config Release
python tools/check_core_purity.py src/core   # 期望退出码 0
```

Qt5 验证：**本机 Qt 5.14.2 低于 3.0 的 5.15 门槛（v2 D1），Qt5 路径一律以 CI
（`cmake-win-qt5.15.yml` 等）为准**，本机不得以 5.14.2 的结果下结论。

### R3. 绿点纪律：一步一提交一验证

- 每个步骤（S1、S2…）结束时必须处于"可构建、测试全绿"状态，然后**立即提交**。
- 提交信息按 AGENTS.md：conventional commits 中文，例如
  `重构：迁移 SARibbonPanelLayout 几何算法至 core 布局引擎`。
- 出现红灯：优先回滚 `git reset --hard HEAD~1`（未推送）或 `git revert`（已推送），
  然后缩小改动面重做。**禁止带病进入下一步。**

### R4. 偏差处理

执行中发现"计划/ v2 计划 ↔ 仓库现实"不符时：

1. 在 [NOTES.md](NOTES.md) 追加一条记录（现象、证据命令输出、处理决定）。
2. 采取**保守**方向：优先缩小而不是扩大改动范围。
3. 已知偏差（执行前就成立，见 NOTES.md）：
   - v2 计划 P7（`SARibbonPannel` 拼写修正）**已由 2.9.x 的 commit f553de7 完成**，
     现仓库类名/文件名均为单 n 的 `SARibbonPanel`，迁移指南只需说明结论，无需别名过渡（B1/B2）。
   - v2 计划中引用的旧文件名 `SARibbonPannelLayout.h` 等实为 `SARibbonPanelLayout.h`（B2）。
   - **v1 计划文件 `SARibbon-3.0-plan.md` 从未存在于仓库与 git 历史**，各计划中的
     "v1 §x" 引用全部悬空，处理约定见下文"计划文档自包含性说明"（B6）。
   - 当前工作区 checkout 分支为 `v3` 而非 `dev`（两分支与 `origin/dev` 同指 `7a617fc`），
     计划 01 P1/P2/S1 的门禁与命令须按此事实解读（B7）。
   - 编码事实更正：全仓（除 3rdparty）**唯一非 UTF-8 代码文件是 `tools/Amalgamate.sh`（GBK）**；
     根/模块 `CMakeLists.txt`、`cmake/SARibbonUtils.cmake` 均为 UTF-8（B4，原记录有误已更正）。
   - ctest 注册项为 26（`tests/` 顶层 25 个 .cpp + `tests/auto/` 1 个），此前文档写"24 个"
     有误（B8）。
   - **QML 注册路线已单轨化**（B9，round2）：v2 §5.3 原"Qt5 命令式 / Qt6 qt_add_qml_module"
     双轨表述已修订为命令式单轨（Qt5/Qt6 同码），04 S1 已落地；声明式轨为 3.1+ 候选。
   - **新增 `SARIBBON_INSTALL` 选项**（B10，round2）：默认 ON，全部 install/export/包配置规则
     收进守卫（v2 §6.4）；01 S4 选项清单已补行、S5/S6/S11 的 install 规则已标注守卫。

### R5. 术语

| 术语 | 含义 |
|------|------|
| Step A | 布局算法"接口化原地重构"：仍在 2.x 文件内，仅把算法的输入输出改经契约接口 |
| Step B | "纯搬移"：算法函数体整体 move 进 core 引擎，不改一行逻辑，diff 可 review |
| 黄金几何测试 | 以 fixture 数据锁定布局引擎输入→输出映射的确定性测试（fixture 录制见计划 02 S5.0；三引擎测试分别在 S5.2/S6/S7；覆盖矩阵补全在 S8） |
| 契约接口 | `SARibbon::Core::SARibbonAbstractLayoutItem/Host` 等窄接口（v2 §3.4.1，最终代码块以计划 02 S4.1-5 为准） |
| 适配器 | widgets 侧退化的 QLayout 子类：收集 items → 调引擎 → 应用几何 |
| 命令式单轨 | 3.0 的 QML 类型注册唯一路线（round2 修订，v2 §5.3）：Qt5/Qt6 同码的 `saRibbonRegisterQmlTypes()`（`qmlRegisterType`/`qmlRegisterSingletonInstance`/`qmlRegisterUncreatableType` + `qmlRegisterModule`，static-once + 静态构建 `Q_INIT_RESOURCE` 守卫），应用在 `engine.load()` 前显式调用；QML 叶子进 qrc 随库二进制、安装期零新增产物；`qt_add_qml_module` 声明式轨降为 3.1+ 候选（04 S1 附注预案、v2 §9 候选清单 B-3） |

### R6. 双分支与同步

- 3.0 开发在 `dev-3.0` 长期分支（计划 01 S1 创建），**合并进 master 前不与 `dev` 交互**。
- 期间 2.x 的 bugfix：普通控件 fix 直接 cherry-pick 到 `dev-3.0` 对应文件；
  **布局引擎内的 fix 必须手动同步 core 版并跑黄金测试**（v2 R7）。
- 术语一致性：Step A（接口化原地重构）/ Step B（纯 move 搬移）的定义以 v2 §3.4.2 为准，
  计划 02 S5.1/S5.2 是其在 PanelLayoutEngine 上的执行化；02 S6/S7 对 Category/Bar 引擎复用
  同一对术语。四份计划中出现的 "S<n>" 均指该计划自身的步骤编号，跨计划引用时写全
  "计划 0X S<n>"。

## 计划文档自包含性说明

**v1 计划文件不存在。** `SARibbon-3.0-plan.md`（"v1 计划"）从未提交进仓库，也不在 git
历史中（核实：`ls SARibbon-3.0-plan.md` 报不存在；`git log --all --oneline -- SARibbon-3.0-plan.md`
输出为空，2026-09-29）。因此：

1. **v2 计划 §0（v1→v2 修订摘要）与 §1（对 v1 计划的审查结论）是关于 v1 内容的唯一存留
   记录**；v2 文档头部已加注说明。
2. v2 计划与 01~04 四份执行计划中残留的全部 "v1 §x" 章节引用（如 01 头部的 "v1 计划 §3、§5、
   §7.1"、03 的 "v1 §5.4、§7.1–§7.4"、04 的 "v1 §7.4/§7.5" 等）**均为悬空引用**。处理约定：
   - 若对应设计已在 v2 计划或执行计划正文内联展开（构建体系→01 S4/S5、amalgamate→03 S1、
     Python 绑定→03 S3、发布流程→04 S10 等），**以内联内容为准**，"v1 §x" 仅作出处备注；
   - 若某步骤的依据只剩 "v1 §x" 而无法从 v2/执行计划正文还原，按 R4 记入 NOTES.md 并向
     维护者澄清，**禁止自行想象 v1 内容**；
   - 后续修订这四份文档时，应逐步把悬空的 "v1 §x" 引用改为指向 v2 章节或删除（round1 评审
     已将完整清单记录在 reviews/round1/cross-findings.md）。
3. **修订历史**：本套文档（README/NOTES/v2 计划/01~04/附录）按评审轮次修订，每轮评审的证据与
   修改记录存于 `plans/3.0/reviews/round<N>/`。
   - 第 1 轮（2026-09-29，视角=跨文档一致性与事实准确性）：本 README、NOTES.md、v2 计划由
     评审 agent 直接修订；01~04 由并行 agent 修订，该轮发现的跨文档问题清单见
     [reviews/round1/cross-findings.md](reviews/round1/cross-findings.md)。
   - 第 2 轮（2026-09-29，视角=参考项目深度学习）：3 个并行 agent 分别深读 QWindowKit 构建体系
     （修订 01/03，findings：[reviews/round2/qwk-build-findings.md](reviews/round2/qwk-build-findings.md)）、
     KDDockWidgets core（修订 02，findings：[reviews/round2/kddw-core-findings.md](reviews/round2/kddw-core-findings.md)）、
     KDDW qtquick + QWK quick（修订 04，findings：[reviews/round2/kddw-qtquick-findings.md](reviews/round2/kddw-qtquick-findings.md)）；
     整合 agent 完成设计级修订上达 v2 计划（§5.3 QML 注册命令式单轨化、§3.4.1 契约代码块同步、
     §3.7 禁区两层化、§6.4 新增 SARIBBON_INSTALL 选项、§7.1/§7.4 测试策略增补、§9 新增"3.1+
     候选项清单"），新建 [appendix-reference-architecture.md](appendix-reference-architecture.md)
     与本目录 [reviews/round2/synthesis-findings.md](reviews/round2/synthesis-findings.md)
     （整合裁决记录），NOTES.md 追加 B9/B10，并对 01/02/03/04 做跨文件一致性小修。
4. **参考项目获取方式**：本套文档大量引用两个参考项目的 `文件:行号` 证据，评审时使用的是
   本机副本 `F:\src\3rdparty\qwindowkit` 与 `F:\src\3rdparty\KDDockWidgets`（仓库内的
   qwindowkit submodule 未初始化，为空目录）。在其他环境执行/复核时：QWindowKit 可
   `git submodule update --init src/SARibbonBar/3rdparty/qwindowkit`（pin `f93657f`，注意
   其自身的 `qmsetup` 也是 submodule 需递归初始化）或 clone `https://github.com/qt-labs/QWindowKit`；
   KDDockWidgets clone `https://github.com/KDAB/KDDockWidgets`。行号以评审时各副本的
   checkout 为准，若 upstream 漂移，以文中引用的符号名（类/函数名）为检索锚点。

## 代码库事实快照（2026-09-29 核实，供 agent 交叉核对）

执行任一计划前若发现下列事实明显不符（如文件不存在、行数大幅变化），先按 R4 记录偏差再继续。

| 事实 | 数值/位置（2026-09-29 逐项核实） |
|------|----------|
| git 基线状态 | HEAD `7a617fc 修复：颜色按钮色块遮挡图标及高DPI下色块消失`；`dev`/`v3`/`origin/dev` 同指该 commit；当前 checkout 分支为 `v3`；未跟踪项 3 个：`SARibbon-3.0-plan-v2.md`、`plans/`、`saribbon-dev-v2.8.0-plus.bundle`（**v1 计划 md 不存在**） |
| 本机环境 | cmake/ctest 3.31.11；`D:\Qt` 下有 5.14.2/6.4.0/6.7.3/6.10.1（6.7.3 含 `msvc2019_64`）；仓库根已有安装目录 `bin_qt5.14.2_MSVC_x64/`、`bin_qt6.7.3_MSVC_x64/` |
| 布局三巨头的 .cpp 行数 | `SARibbonPanelLayout.cpp` 1863 行；`SARibbonCategoryLayout.cpp` 1425 行；`SARibbonBarLayout.cpp` 1993 行（合计 5281，v2 "约5300行"成立）。关键函数：`updateGeomArray(QRect)` :795（约 409 行）、`recalcExpandGeomArray` :1204（约 184 行）、`setGeometry` 重入守卫 :1831；`updateGeometryArr` :431、`categoryContentSize` :408、`scrollByAnimate` :971（用 QPropertyAnimation）；`layoutTitleRect` :1242 |
| BarLayout 度量函数 | `SARibbonBarLayout.h:66/89/92/96/108/114`：`calcMinTabBarWidth` / `minimumModeMainBarHeight` / `normalModeMainBarHeight` / `tabBarHeight` / `categoryHeight` / `panelTitleHeight`（对应 setter `setTabBarHeight` :98、`setCategoryHeight` :110、`setPanelTitleHeight` :116） |
| RowProportion 枚举 | `SARibbonPanelItem.h:36-42`（类内普通 enum：None/Large/Medium/Small）；属性名宏 `SA_ActionPropertyName_RowProportion`（`"_sa_RowProportion"`）在 `SARibbonPanelItem.h:59-60`，同族 `_sa_ToolButtonPopupMode` :62-63、`_sa_ToolButtonStyle` :65-66 |
| 全局枚举 | `SARibbonGlobal.h:197` `SARibbonAlignment`、`:220` `SARibbonTheme`、`:245` `SARibbonMainWindowStyleFlag`；PIMPL 宏：`SA_RIBBON_DECLARE_PRIVATE` :84、`SA_RIBBON_DECLARE_PUBLIC` :104、`SA_RIBBON_IMPL_CONSTRUCT` :123、`SA_D` :138、`SA_DC` :153、`SA_Q` :168、`SA_QC` :183（**整段宏族约 :83–184**，计划 01 S6 只写 ":84-105" 不完整） |
| Panel 类 | `SARibbonPanel.h:99`（`class SA_RIBBON_EXPORT SARibbonPanel : public QFrame`，单 n 拼写，P7 已完成）；`PanelLayoutMode` 枚举在 `SARibbonPanel.h:115` |
| 颜色算法 | `makeColorVibrant` 等现位于 `SARibbonUtil.h/.cpp`，被 `SARibbonBar.cpp`、`SARibbonThemeManager.cpp` 使用 |
| 测试 | `tests/` 顶层 25 个 .cpp + `tests/auto/SARibbonThemePalette/tst_themepalette.cpp`，`tests/CMakeLists.txt` 以 `add_saribbon_test(<名> <源>)` 注册 **26 项**（ctest 基线 N₀=26）；机制：链接 `SARibbonBar` + Qt Test/Core/Gui/Widgets，include `../src/SARibbonBar`，非静态构建时 POST_BUILD `copy_if_different $<TARGET_FILE:SARibbonBar>` 到测试目录，`TIMEOUT 120`，输出 `build/tests/<Config>/` |
| CI | `.github/workflows/cmake-{win,linux,mac}-qt{5.15,6.8}.yml` 6 个构建 workflow + `page.yml` + `publish-python-bindings.yml`（共 8 个文件） |
| amalgamate | `tools/Amalgamate.sh`（**自身为 GBK 编码，全仓唯一非 UTF-8 代码文件**）在 `tools/` 下运行 `./Amalgamate.exe`，`OPTS='-i "../src/SARibbonBar" -i "../src/SARibbonBar/colorWidgets" -w "*.cpp;*.h;*.hpp" -s'`，模板在 `tools/amalgamate/`（4 个模板文件），产物 `src/SARibbon.h/.cpp` 末尾做 LF→CRLF 转换 |
| qwindowkit | git submodule，未初始化（目录为空），`.gitmodules` path=`src/SARibbonBar/3rdparty/qwindowkit`，url `https://github.com/czyt1988/qwindowkit`，pin `f93657fa82bdd37dba68ea962d5e9b2cf4fd4d60`；本机参考副本 `F:\src\3rdparty\qwindowkit`（其建库宏实名 `qwk_add_library`，`src/CMakeLists.txt:38`） |
| 构建脚本 | `scripts/build.ps1`（395 行）：构建目录固定 `build/`，自动探测 Qt/VS/CMake，详见 R2 的核实说明 |
| 现有 CMake | 根 `CMakeLists.txt` 246 行：floor 3.15（:5）、版本 2.9.5（:7-13）、`SARIBBON_MIN_QT_VERSION 5.12`（:47）、`CMAKE_CXX_FLAGS` 手术（:93 `/std:c++17`、:100 `/std:c++14`、:124 `/wd4819`，另 :127 `_HAS_AUTO_PTR_ETC=1`）；选项实名 `SARIBBON_BUILD_STATIC_LIBS/EXAMPLES`、`BUILD_TESTS`（无 SARIBBON_ 前缀）、`SARIBBON_USE_FRAMELESS_LIB/ENABLE_SNAPLAYOUT/INSTALL_IN_CURRENT_DIR`；`cmake/SARibbonUtils.cmake` 仅 16 行（末行无换行符，`wc -l` 报 15），唯一宏 `saribbon_set_bin_name` **无任何调用者**（根 :141 自行计算 bin 名，且宏体 `endmacro(damacro_set_bin_name)` 与宏名不一致） |
| i18n / 资源 | `src/SARibbonBar/i18n/`（`SARibbon_en_US.ts`、`SARibbon_zh_CN.ts` + CMakeLists）；图片/qss 在 `src/SARibbonBar/resource/`（png/svg/theme-base.qss + `palettes/`、`templates/` 子目录），**qrc 在 `src/SARibbonBar/SARibbonResource.qrc`（不在 resource/ 内）**；lupdate/lrelease 逻辑在 `src/SARibbonBar/CMakeLists.txt:253/260` 附近 |
| Python 绑定 | `sip/`（PyQt5，.sip 文件已是单 n `SARibbonPanel.sip` 等）、`pyqt6/sip/`、`pyside6/`（CMakeLists.txt、typesystem_saribbon.xml、saribbon_python_glue.h、PySideSARibbon/、pyproject.toml）；构建脚本 `tools/build_python_bindings.bat`、`tools/build_pyside6_bindings.bat`；发行包名 `PyQtSARibbon`（pyproject.toml 与 pyproject-pyqt6.toml）/`PySideSARibbon`，导入形式 `from PyQtSARibbon import saribbon`；示例 `pyexamples/{pyqt5,pyqt6,pyside6}/`；打包配置还引用 `project.py`、`MANIFEST.in` |
| 示例 | `example/` 下 11 个子目录：`MainWindowExample`、`MatlabUI`、`MdiAreaWindowExample`、`MultiScreenDpiExample`、`NormalMenuBarExample`、`Qt3DWindowExample`、`StaticExample`、`ThemeDesignerExample`、`UseNativeFrameExample`、`WidgetWithRibbon`、`uiform`（+ 顶层 CMakeLists.txt） |
| 编码状况 | 全仓代码文件（src/example/tests/tools/sip/pyqt6/pyside6/scripts，除 3rdparty）扫描：**仅 `tools/Amalgamate.sh` 非 UTF-8（GBK 可解码）**；三个 CMakeLists（根/src/SARibbonBar、cmake/SARibbonUtils.cmake）与 build.ps1 均为 UTF-8。`/wd4819` 是压 GBK 代码页下 UTF-8 无 BOM 源文件的 C4819 警告，不代表源码是 GBK |
