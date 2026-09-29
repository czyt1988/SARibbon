# 计划 01：仓库重排与现代化构建骨架（对应 v2 里程碑 M0）

> 前置计划：无（本计划是 3.0 重构的起点）
> 后续计划：[02-core-sinking.md](02-core-sinking.md)
> 设计依据：v2 计划 §2.1、§3.1、§4.4、§6.1、§6.2、§8-M0、§9-D1/D2/D4。
> **v1 引用说明**：原稿引用的"v1 计划 §3/§5/§7.1/§7.4"已核实为悬空引用（`SARibbon-3.0-plan.md` 在仓库与 git 历史中从未存在）。v2 §6.1 明文"构建体系维持 v1 设计"，涉及的 v1 内容（sa_add_library 设计、导出宏三段式模板、转发头/安装布局、examples/qml 预留、目录设计）已全部按 v2 与仓库现实内联到本文对应章节（S2/S3/S5/S6/S11），本文档自包含，执行时无需 v1。核查记录见 [reviews/round1/01-findings.md](reviews/round1/01-findings.md)。
> 分支：`dev-3.0`（本计划 S1 创建）。当前检出分支为 `v3`：`v3` 指向 plans/3.0 评审提交链顶端（基线 `7a617fc` 之后仅有计划文档提交；round3 修订时点为 `eba8ebd`），`dev` 仍指向 `7a617fc`。P1/P2/S1 已按"执行时点"校准（round3）。

## 1. 目标

把 2.9.5 的"单 target + 平铺目录"物理重组为 3.0 终态布局，并把构建系统现代化：

1. **目录终态**（3.0 完全重构允许目录变更，本步一次性到位）：

   ```
   SARibbon/
   ├── CMakeLists.txt              # 现代化根（详见 S4）
   ├── CMakePresets.json           # 保留 vcpkg presets + 新增常规 presets
   ├── cmake/SARibbonUtils.cmake   # 重写：sa_add_library / sa_sync_include
   ├── 3rdparty/                   # 从 src/SARibbonBar/3rdparty 整体上移：
   │   ├── qwindowkit/             #   submodule（gitlink，pin f93657f，未初始化）
   │   ├── CMakeLists.txt          #   QWK 辅助构建脚本（S3 需改其内部相对路径）
   │   └── cmd-build-example.sh
   ├── src/
   │   ├── core/                   # SARibbonCore（本计划只放 Global/Qt5Compat/Config 骨架）
   │   ├── widgets/                # SARibbonWidgets（2.9.5 全部控件，原 src/SARibbonBar/）
   │   │   ├── colorWidgets/ i18n/ resource/
   │   │   └── SARibbonGlobal.h    # 变为兼容转发头
   │   ├── qml/                    # SARibbonQml 空骨架（SARIBBON_BUILD_QML=OFF 默认不构建）
   │   ├── CMakeLists.txt          # 按 option 进子模块、install(EXPORT)、包配置生成（S11）；core config 头由 src/core/CMakeLists.txt 生成（S6.2）
   │   ├── SARibbon.h/.cpp         # amalgamate 产物暂维持原位原名（计划 03 改造）
   │   └── (不再有 SARibbonBar/ 目录)
   ├── examples/widgets/           # 原 example/ 11 个示例（qml/ 留计划 04）
   ├── tests/widgets/              # 原测试 26 个注册项（25 个顶层 .cpp + tests/auto/ 下 1 个）
   └── tools/check_core_purity.py  # 新增：core 纯净性门禁
   ```

2. **现代化 CMake**（v2 §6.1 保留的构建设计，本计划落实）：floor **3.21**（理由见 S4）、
   `target_*` 全套、`GNUInstallDirs`、`write_basic_package_version_file`、
   组件化 `SARibbonConfig.cmake`、删除 `CMAKE_CXX_FLAGS` 手术、C++17 统一。
3. **core 纯净性门禁**就位（v2 §6.2，提前到 M0）。
4. 全程**行为零变化**：所有代码仍是 2.9.5 逻辑，现有测试/示例全部通过。

## 2. 范围与非目标

**非目标**（明确不做，归属后续计划）：
- 任何算法/枚举/主题数据向 core 的下沉 → 计划 02
- amalgamate 按模块改造、产物出库 → 计划 03（本计划仅修 Amalgamate.sh 与合并模板的路径使其继续可用）
- Python 绑定（sip/pyqt6/pyside6）路径适配 → 计划 03（本计划不动其文件；publish workflow 现状核实见 S10.4）
- QML 模块任何实现 → 计划 04
- tests/examples 的 190 处平铺 include 命名空间化（`"SARibbonBar.h"` → `<SARibbonWidgets/...>`）→ 计划 03/04（本计划靠 S5.5 的 include 传播决策保证零源码改动）

## 3. 前置条件（开始前逐项验证，全部满足才继续）

| # | 检查 | 命令 | 期望 |
|---|------|------|------|
| P1 | 工作区干净 | `git status --porcelain` | 仅 **1** 个已知未跟踪项：`saribbon-dev-v2.8.0-plus.bundle`（历史产物，不入库；v1 计划文件不存在）。**round3 校准**：`SARibbon-3.0-plan-v2.md` 与 `plans/` 已随评审提交（`42b7dcc` round1、`eba8ebd` round2 及其后 round3 提交）入库，不再是未跟踪项；若 plans/ 尚有未提交修订，按 S1 处理 |
| P2 | 代码基线未变 | `git branch --show-current && git log --oneline -1 && git merge-base --is-ancestor 7a617fc HEAD && echo OK && git diff 7a617fc HEAD --stat -- ':!plans' ':!SARibbon-3.0-plan-v2.md'` | 分支为 `v3`；HEAD 为评审提交链最新一条（round3 修订时点为 `eba8ebd 文档：3.0 改造计划第2轮评审修订（参考项目深度学习）`，其后可能追加评审提交，以执行时 `git log -1` 为准）；`merge-base --is-ancestor` 后输出 `OK`（基线 `7a617fc` 是 HEAD 祖先）；末条 diff **输出为空**——基线之后除计划文档外无任何代码改动。`dev` 分支仍指 `7a617fc`（原"HEAD==7a617fc 且 v3/dev 同指"的期望已因评审提交入库而过时，round3 校准为祖先+空 diff 双门禁） |
| P3 | 测试基线 | `pwsh -NoProfile -File scripts/build.ps1 rebuild -Tests ON -Examples ON` 后 `ctest --test-dir build -C Release --output-on-failure` | 全部通过；**记录通过的测试数量 N₀ 到 NOTES.md**（后续各步以 N₀ 对照）。当前注册测试数为 **26**（tests/CMakeLists.txt 有 26 个 `add_saribbon_test()` 调用），N₀ 预期 = 26，以实测为准。注意：`ctest --test-dir` 需 CMake ≥ 3.20；`-Tests ON` 当前映射为 `-DBUILD_TESTS=ON`（scripts/build.ps1:332，S4 第 11 条同步改名） |
| P4 | 布局三巨头行数 | `wc -l src/SARibbonBar/SARibbon{Panel,Category,Bar}Layout.cpp` | 1863 / 1425 / 1993（2026-09 已逐项核实一致；量级一致即可，显著偏离则记偏差） |
| P5 | 本机 Qt | `pwsh -NoProfile -File scripts/build.ps1` 输出的 Qt 探测行 | 形如 `[OK] Qt path (auto-detected): <dir> (Qt6)`，应为 Qt 6.x（P3 已隐含验证） |

## 4. 全程纪律

- [README.md](README.md) R1–R6 全部适用；特别注意 R1（AGENTS.md 规范、**不动文件编码**）与 R3（一步一提交）。
- 所有目录搬移必须用 `git mv`（保留 `--follow` 历史）。
- 本计划**不改任何 .cpp/.h 的逻辑**；唯一例外是导出宏/头文件路径的机械适配与 `SAColorWidgetsGlobal.h` 的宏合并（D4），以及 S7 列出的 tests 内 2 处路径字符串。

## 5. 执行步骤

### S1 创建 dev-3.0 分支（计划文档已随评审提交入库）

```bash
git checkout -b dev-3.0          # 从当前 HEAD（v3 评审提交链顶端，见 P2）创建
git status --porcelain           # 若 plans/ 或 SARibbon-3.0-plan-v2.md 尚有未提交修订（如 round3 评审产物），则：
# git add plans/ SARibbon-3.0-plan-v2.md
# git commit -m "文档：3.0 重构计划评审修订补充"
```

注意（round3 按执行时点校准）：
- 计划文档**已经入库**（`42b7dcc` round1、`eba8ebd` round2 及后续评审提交），S1 通常只剩建分支一步；`git commit` 若报 "nothing to commit" 属正常，跳过即可，**不是错误**。原稿"`git add plans/ SARibbon-3.0-plan-v2.md` + 提交"在评审提交入库后已不适用。
- **不要** `git add SARibbon-3.0-plan.md` —— 该文件（v1 计划）从未存在（`git log --all --oneline -- SARibbon-3.0-plan.md` 为空）。
- `saribbon-dev-v2.8.0-plus.bundle` 保持未跟踪，不入库。

### S2 顶层目录搬移：example → examples/widgets

**目的**：为 QML 示例预留 `examples/qml/`（计划 04 交付物，v2 §8-M3；原 v1 §7.4 引用悬空，动机即此）。

**现状核实**：`example/` 下共 11 个示例目录（与 `example/CMakeLists.txt` 的 11 个 `add_subdirectory` 一一对应）：
`MainWindowExample`、`UseNativeFrameExample`、`MultiScreenDpiExample`、`Qt3DWindowExample`、`WidgetWithRibbon`、`NormalMenuBarExample`、`MdiAreaWindowExample`、`MatlabUI`、`uiform`、`StaticExample`、`ThemeDesignerExample`。

**操作**：
1. 搬移（`git mv` 到不存在的目录会报错，先建 `examples/widgets/`）：

   ```bash
   git mv example examples
   mkdir examples/widgets
   for d in MainWindowExample UseNativeFrameExample MultiScreenDpiExample Qt3DWindowExample \
            WidgetWithRibbon NormalMenuBarExample MdiAreaWindowExample MatlabUI uiform \
            StaticExample ThemeDesignerExample; do
       git mv "examples/$d" examples/widgets/
   done
   git mv examples/CMakeLists.txt examples/widgets/CMakeLists.txt
   ```

2. 新建 `examples/CMakeLists.txt`：`add_subdirectory(widgets)`（qml 子目录留计划 04）。
3. 根 CMakeLists 的 `add_subdirectory(example)`（现 CMakeLists.txt:224）改为 `add_subdirectory(examples)`。
4. **修复 StaticExample 的相对路径**（原稿遗漏，不改必断）：`example/StaticExample/CMakeLists.txt:55` 有
   `SET(SARIBBON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)`——搬移后 `../../src` 会解析成
   `examples/src`（不存在）。改为 `${CMAKE_CURRENT_SOURCE_DIR}/../../../src`。
   盘点命令（实测整个 example/ 只有这一处上两级引用）：`git grep -n '\.\./\.\./' example`。

**验证**：全量构建绿（此时示例 target 名与链接 `SARibbonBar::SARibbonBar` 均未变）；`git log --follow examples/widgets/MainWindowExample/CMakeLists.txt` 可见历史——**须在本步提交之后执行**（rename 未入历史前 `--follow` 无输出，属 git 行为而非搬移失败；round3 注）。

**提交**：`重构：example 目录迁移至 examples/widgets`

### S3 submodule 搬移：src/SARibbonBar/3rdparty → 3rdparty

**目的**：第三方依赖上移顶层（用户明确要求；原 v1 §3 目录设计引用悬空，v2 未涉及此点，按用户要求执行）。

**现状核实**：
- `.gitmodules`：path=`src/SARibbonBar/3rdparty/qwindowkit`，url=`https://github.com/czyt1988/qwindowkit`（**fork**，非 stdware 上游；submodule.md 写的 stdware 与实际不符，S12 一并修正）。
- `git submodule status` 输出 `-f93657fa82bdd37dba68ea962d5e9b2cf4fd4d60 src/SARibbonBar/3rdparty/qwindowkit`（`-` 前缀 = 未初始化，目录为空，`git mv` 只搬 gitlink）。
- **`src/SARibbonBar/3rdparty/` 不是只有 qwindowkit**：还有 `CMakeLists.txt`（QWK 辅助构建脚本，`add_subdirectory(qwindowkit)` 并安装到仓库根 `bin_qtX_编译器_x架构/`）与 `cmd-build-example.sh`（Windows 命令行编译示例）。原稿"删除空目录"不成立。

**操作**：
1. 三项整体搬移：

   ```bash
   mkdir -p 3rdparty
   git mv src/SARibbonBar/3rdparty/qwindowkit 3rdparty/qwindowkit
   git mv src/SARibbonBar/3rdparty/CMakeLists.txt 3rdparty/CMakeLists.txt
   git mv src/SARibbonBar/3rdparty/cmd-build-example.sh 3rdparty/cmd-build-example.sh
   rmdir src/SARibbonBar/3rdparty
   ```

   （较新版本 git 的 `git mv` 会自动更新 `.gitmodules` 的 path；**必须验证** `git diff .gitmodules` 中 path 已变为 `3rdparty/qwindowkit`，若未变则手动改并 `git add .gitmodules`。）
2. **修 3rdparty/CMakeLists.txt 内部相对路径**（原稿遗漏）：该文件有
   `set(SARIBBON_BIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../${SARIBBON_BIN_NAME})`——原位置在
   `src/SARibbonBar/3rdparty/`（上三级 = 仓库根），搬到顶层 `3rdparty/` 后上三级会越出仓库。改为
   `${CMAKE_CURRENT_LIST_DIR}/../${SARIBBON_BIN_NAME}`。
3. 若 `git mv` gitlink 报错，回退方案：`git rm --cached src/SARibbonBar/3rdparty/qwindowkit` → 手动改 `.gitmodules` → `git add 3rdparty/qwindowkit`（保持 gitlink 模式，pin 仍为 f93657f）。

**验证**：`git submodule status` 显示 `-f93657fa82bdd37dba68ea962d5e9b2cf4fd4d60 3rdparty/qwindowkit`（前缀 `-` 仍在，因未初始化，属正常）；`git status` 无脏项；`ls src/SARibbonBar/` 不再含 3rdparty。

**提交**：`重构：qwindowkit submodule 及 3rdparty 辅助脚本迁移至顶层`

### S4 根 CMakeLists.txt 现代化重写

**目的**：现代 CMake 基座（用户明确要求"cmake 写得更现代化"）。

**操作**（重写 `CMakeLists.txt`，逐块处置见 S4.1 表；保留 vcpkg 联动与 frameless 探测逻辑语义）：
1. `cmake_minimum_required(VERSION 3.21)`；`project(SARibbon VERSION 3.0.0 LANGUAGES CXX)`。
   floor 取 3.21 而非原稿 3.16，理由（均有实据）：
   - 本计划与 README R2 的验证命令 `ctest --test-dir` 需 CMake ≥ 3.20；
   - 非 IMPORTED target 的含 `::` ALIAS（现有 `src/SARibbonBar/CMakeLists.txt:146` 的 `SARibbonBar::SARibbonBar`，以及 S5 新增的 `SARibbon::Core/Widgets`）在旧版 CMake 不被支持（3.18 起明确放开；现仓库 floor 3.15 却已如此使用，说明实际构建环境早已高于 floor）；
   - 参考项目 QWindowKit 自身 floor 为 3.19（其 CMakeLists.txt:1）；
   - `CMakePresets.json` 为 version 6 + `cmakeMinimumRequired` 3.25——仅在使用 presets 时生效，不作为 floor，但 floor 不应低于生态现状。
2. 版本三段升为 `3.0.0`（唯一来源在根，`src/core/SARibbonCoreConfig.h.in` 从 `PROJECT_VERSION` 生成）。**project() 之后必须立即派生旧变量**（下游大量消费，漏掉即配置失败）：

   ```cmake
   set(SARIBBON_VERSION       ${PROJECT_VERSION})
   set(SARIBBON_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
   set(SARIBBON_VERSION_MINOR ${PROJECT_VERSION_MINOR})
   set(SARIBBON_VERSION_PATCH ${PROJECT_VERSION_PATCH})
   ```

   消费者证据：`src/SARibbonBar/SARibbonBarVersionInfo.h.in` 用 `@SARIBBON_VERSION_MAJOR@` 等三宏与 `#cmakedefine SARIBBON_VERSION "@SARIBBON_VERSION@"`；`src/SARibbonBar/CMakeLists.txt:2` `project(SARibbonBar ... VERSION ${SARIBBON_VERSION})`；10 个 example 的 `set_target_properties(... VERSION ${SARIBBON_VERSION})`。
   注：本条只改 CMake 工程版本；`vcpkg.json`（现写 2.8.0）、`pyproject*.toml`、`changlog.md` 的版本元数据归计划 03/04，此处不动、记入 NOTES。
3. 选项统一（全部 `option()` 带英文描述）：`SARIBBON_BUILD_WIDGETS`(ON)、`SARIBBON_BUILD_QML`(OFF)、`SARIBBON_BUILD_STATIC_LIBS`(OFF)、`SARIBBON_BUILD_EXAMPLES`(ON)、`SARIBBON_BUILD_TESTS`(OFF)、`SARIBBON_USE_FRAMELESS_LIB`(OFF)、`SARIBBON_ENABLE_SNAPLAYOUT`(OFF)、`SARIBBON_INSTALL_IN_CURRENT_DIR`(win ON/其他 OFF，现 L23-27 写法保留)、**`SARIBBON_INSTALL`(ON)**（round2 D1 采纳，v2 §6.4/NOTES B10：全部 install/export/包配置规则收进守卫，保护 `add_subdirectory` 嵌入场景；**与 `SARIBBON_INSTALL_IN_CURRENT_DIR`（安装目的地选项）是两个不同选项，勿混淆**——前者管"装不装"，后者管"装到哪"）。
   兼容旧名一版：`if(DEFINED BUILD_TESTS) set(SARIBBON_BUILD_TESTS "${BUILD_TESTS}" CACHE BOOL "" FORCE) endif()` 并 `message(DEPRECATION ...)` 提示改名（shim 只兜底 CI/脚本过渡期，正式改名见第 11 条与 S10）。
4. `set(SARIBBON_MIN_QT_VERSION 5.15)`（v2 D1 落地，现值 5.12 在 L47），`find_package(QT NAMES Qt6 Qt5 ...)` 两段式（L48-55）保留。
5. **删除 L79-103 的 `set(CMAKE_CXX_FLAGS ... /std:c++xx)` 手术**；但 S4~S6 之间存在过渡窗口（旧 `src/SARibbonBar/CMakeLists.txt` 尚未走 `sa_add_library`，无任何 target 级标准设定，Qt6 下会掉回编译器默认标准而失败），因此**过渡期在根保留**：

   ```cmake
   set(CMAKE_CXX_STANDARD 17)
   set(CMAKE_CXX_STANDARD_REQUIRED ON)
   ```

   （目录级变量，非 FLAGS 手术）。S6 三个 target 全部经 `sa_add_library` 的 `target_compile_features(... PUBLIC cxx_std_17)` 覆盖后，在 S6 末尾删除这两行全局设定（3.0 决策：全模块 C++17，Qt5.15/Qt6 均支持，frameless 本就要求 17；写入迁移指南属计划 04）。
6. vcpkg `frameless` feature 自动开启逻辑（L32-43）、frameless 的 Qt 版本门槛块（L58-77，产出内部变量 `_SARIBBON_USE_FRAMELESS_LIB`，被 `src/SARibbonBar/CMakeLists.txt:165/203` 引用，**变量名不可改**）、QWindowKit `find_package` 探测逻辑（L151-194，含本地 `bin_qtX/lib/cmake/QWindowKit` 路径常量）原样保留。
7. 顶层守卫：`if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)` 内才 `add_subdirectory(examples / tests)`（`add_subdirectory(src)` 不放守卫内，嵌入场景也要建库）。
8. 引入 `include(GNUInstallDirs)` 与 `include(CMakePackageConfigHelpers)`（模块里现有 `include(GNUInstallDirs)`（src/SARibbonBar/CMakeLists.txt:171）可随之删除，避免重复）。
9. `list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")` 后 `include(SARibbonUtils)`。
10. `CMAKE_INCLUDE_CURRENT_DIR` 与 `include_directories()` 全局污染：**根文件里并不存在**（原稿表述不准）；实际位置是 `src/SARibbonBar/CMakeLists.txt:5` 的 `set(CMAKE_INCLUDE_CURRENT_DIR ON)`，在 S6 改写模块 CMakeLists 时删除。
11. **同步 `scripts/build.ps1`**（原稿遗漏）：其 configure 参数表（L322-333）传 `-DBUILD_TESTS=$Tests`，改为 `-DSARIBBON_BUILD_TESTS=$Tests`；其余 4 个 `-DSARIBBON_*` 参数名不变（与新选项一致，已核实）。

#### S4.1 现有根 CMakeLists.txt（246 行）逐块处置表

| 行号 | 内容 | 处置 |
|------|------|------|
| L2-3 | 中文风格注释（命令小写/变量大写） | 保留（转写为正确编码的注释，遵守 NOTES B4：不做全仓编码清洗，重写文件用 UTF-8） |
| L5 | `cmake_minimum_required(VERSION 3.15)` | 改 3.21（见上） |
| L7-13 | `SARIBBON_VERSION_*` 三变量 + `project()` | 改为 `project(SARibbon VERSION 3.0.0)` + 第 2 条的派生 set |
| L15-27 | 6 个 option（实名核实：`SARIBBON_BUILD_STATIC_LIBS`、`SARIBBON_BUILD_EXAMPLES`、`BUILD_TESTS`、`SARIBBON_USE_FRAMELESS_LIB`、`SARIBBON_ENABLE_SNAPLAYOUT`、`SARIBBON_INSTALL_IN_CURRENT_DIR` win ON/else OFF） | 保留语义；`BUILD_TESTS`→`SARIBBON_BUILD_TESTS` + shim；新增 `SARIBBON_BUILD_WIDGETS`(ON)/`SARIBBON_BUILD_QML`(OFF)/`SARIBBON_INSTALL`(ON，round2 D1→v2 §6.4) |
| L32-43 | vcpkg 检测 + frameless feature 联动 | 原样保留 |
| L45-56 | Qt 探测（`SARIBBON_MIN_QT_VERSION 5.12`） | 保留，5.12→5.15 |
| L58-77 | frameless Qt 版本门槛 → `_SARIBBON_USE_FRAMELESS_LIB` | 原样保留（内部变量名不可改，模块 CMakeLists 引用它） |
| L79-103 | C++ 标准三分支 + MSVC `/std:c++xx` FLAGS 手术 | **删除**，换第 5 条的两行过渡设定 |
| L105-121 | `CMAKE_*_POSTFIX` + `CMAKE_BUILD_POSTFIX` | 保留（库/示例的 `DEBUG_POSTFIX` 依赖 `CMAKE_DEBUG_POSTFIX`） |
| L123-128 | MSVC `/wd4819` + `add_compile_definitions(_HAS_AUTO_PTR_ETC=1)` | 保留（Qt 6.5.3 qvarlengtharray.h 与新 MSVC STL 的已知问题，注释一并保留） |
| L131-148 | 平台判断 + `SARIBBON_LOCAL_INSTALL_BIN_NAME/DIR`（`bin_qt${QT_VERSION}_${CMAKE_CXX_COMPILER_ID}_${SARIBBON_PLATFORM}`）+ `SARIBBON_INSTALL_IN_CURRENT_DIR` 改 `CMAKE_INSTALL_PREFIX` | 原样保留（QWindowKit 探测块 L169 也引用 `SARIBBON_LOCAL_INSTALL_BIN_DIR`） |
| L151-194 | QWindowKit find_package 探测（用户指定 DIR → 本地 bin → 系统路径，找不到降级 OFF） | 原样保留 |
| L198 | `include(cmake/WinResource.cmake)` | 保留（`create_win32_resource_version` 被 src/SARibbonBar/CMakeLists.txt:357 调用，S5 的 `sa_add_library` 也要用） |
| L200-206 | `configure_file` 生成 `SARibbonBarVersionInfo.h`（**生成到源码树**，文件已被 git 跟踪） | S4 阶段路径不动（此时目录还没搬，改了会破坏 S4 提交点的可构建性）；S6 搬目录时同步改路径为 `src/widgets/...`；"生成到 build 树"的现代化归计划 02（VersionInfo 收编时） |
| L208-218 | static/shared 提示 message | 保留（可并入 sa_add_library 的日志） |
| L220 | `add_subdirectory(src)` | 保留 |
| L222-225 | `if(SARIBBON_BUILD_EXAMPLES) add_subdirectory(example)` | 改 `examples`（S2 已搬）并纳入第 7 条守卫 |
| L227-231 | `if(BUILD_TESTS) enable_testing() add_subdirectory(tests)` | 开关改 `SARIBBON_BUILD_TESTS`；**`enable_testing()` 必须保留**（ctest 依赖）；纳入守卫 |
| L235-244 | `set(SARIBBON_DOC_FILES readme.md readme-cn.md LICENSE)` | **死变量**（全仓无任何 `install()` 消费，`git grep SARIBBON_DOC_FILES` 仅此一处）→ 删除；若想真正安装文档，另加 `install(FILES ${SARIBBON_DOC_FILES} DESTINATION .)` 属行为变化，本计划不做，记 NOTES |
| （新增） | `include(GNUInstallDirs)`、`include(CMakePackageConfigHelpers)`、`CMAKE_MODULE_PATH` + `include(SARibbonUtils)` | 第 8/9 条 |

#### S4.2 附注：QWK 顶层 CMake 组织对照（round2 补充，非执行步骤）

QWK 顶层 `CMakeLists.txt`（83 行，floor 3.19）逐项对照结论，行号指 `qwindowkit/CMakeLists.txt`：

- **抄（已体现在 S4 正文）**：选项命名 `<项目>_BUILD_<对象>` 全大写模式（:8-13，`QWINDOWKIT_BUILD_STATIC/WIDGETS/QUICK/EXAMPLES/DOCUMENTATIONS/INSTALL`）——SARibbon 新选项 `SARIBBON_BUILD_WIDGETS/QML/TESTS` 与之同构，命名一致性有实证先例；`GNUInstallDirs`+`CMakePackageConfigHelpers` 顶层统一 include（:36-39，S4 第 8 条）。
- **抄（S4.1 表 L105-121 行的补充说明）**：QWK 对 debug 后缀用 `if(NOT DEFINED CMAKE_DEBUG_POSTFIX)` 守卫再设默认 "d"（:28-30）。SARibbon 现状用 `set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING ...)`（根 `CMakeLists.txt:105`），CACHE 变量同样尊重用户预设，**等价、不需要改**。
- **已采纳（round2 整合裁决，原"不抄 1"翻转；NOTES.md B10）：`SARIBBON_INSTALL` 安装守卫选项**（QWK 实证 `QWINDOWKIT_INSTALL`，:13,36-39；`src/CMakeLists.txt:29,100,181` 所有 install/包配置规则都在其内）。新增 `SARIBBON_INSTALL`（默认 ON，S4 第 3 条已补行），把全部 `install()`/`install(EXPORT)`/Config 生成/转发头安装规则（S5.3/S5.4 骨架、S6.7、S11）收进守卫——`add_subdirectory` 嵌入场景（OFF）不产生任何安装物、不污染宿主工程；S4 第 7 条的顶层守卫只覆盖 examples/tests，与此互补。决策与证据：v2 §6.4、reviews/round2/qwk-build-findings.md §三 D1、reviews/round2/synthesis-findings.md。
- **不抄 2：qmsetup 全家桶**（:50-75 `find_package(qmsetup QUIET)` + submodule 就地构建回退 + `qm_import`/`qm_init_directories`）。理由：引入构建期外部工具链（corecmd 二进制）与其就地编译回退逻辑，对 SARibbon 收益不成比例；`sa_*` 自实现已覆盖所需能力（S5.1 决策）。
- **不抄 3：MSVC `/utf-8` 与 `/manifest:no`**（:22-26）。SARibbon 已有自己的编码警告处置（`/wd4819`，S4.1 表 L123-128 保留）且不玩 manifest 手术；`/utf-8` 会改变既有 GBK 源文件的解析行为，违反"行为零变化"。
- **不抄 4：MinGW 去 lib 前缀**（:31-33，`CMAKE_STATIC/SHARED_LIBRARY_PREFIX ""`）。会改变 MinGW 产物名（libSARibbonBar.dll→SARibbonBar.dll），属行为变化；若 3.x 想对齐 Windows 命名惯例，记 NOTES 供后续版本单独决策。
- **不抄 5：惰性 Qt 探测**。QWK 顶层不 find_package(Qt)，由各模块链接时经 `qm_find_qt` 按需探测（`qmsetup:QMSetupAPI.cmake:106-116`，`find_package(QT NAMES Qt6 Qt5 ...)` 顺序由 `QMSETUP_FIND_QT_ORDER` 控制 :37-38）。SARibbon 顶层两段式探测**必须保留**——frameless 的 Qt 版本门槛块（现 L58-77）与 `SARIBBON_MIN_QT_VERSION` 检查依赖顶层已知 `QT_VERSION_MAJOR`（S4 第 4/6 条）。
- **不抄 6：统一输出目录 `out-<arch>-<config>/{bin,lib}`**（`qmsetup:Filesystem.cmake:12-24` `qm_init_directories`）。SARibbon 维持现状 `${CMAKE_BINARY_DIR}/{bin,lib}`（S5.3 骨架照抄现有属性集），改布局会破坏 build.ps1/CI 的既有路径假设。
- **注意：QWK 顶层没有 examples/tests 的 `CMAKE_SOURCE_DIR` 守卫**（:82-84 仅靠选项开关）；SARibbon S4 第 7 条的守卫是** stricter 的自有做法**，保留（QWK 无先例不构成反证，嵌入场景守卫是 SARibbon 既有需求）。

**验证**：`cmake -S . -B build-tmp -DCMAKE_PREFIX_PATH=<Qt6路径>` 配置通过，且 `cmake --build build-tmp --config Release` 可编译（S4 提交点必须全绿：新根 + 旧 src 结构是兼容的——旧模块消费的 `SARIBBON_VERSION*`、`_SARIBBON_USE_FRAMELESS_LIB`、`CMAKE_DEBUG_POSTFIX`、`SARIBBON_MIN_QT_VERSION` 均已保留；`build-tmp` 验证后删除，避免与 build.ps1 的 `build/` 混淆）。

**提交**：`重构：根 CMakeLists 现代化（floor 3.21/选项统一/C++17 过渡设定/清理死变量）`

### S5 重写 cmake/SARibbonUtils.cmake（sa_add_library / sa_sync_include）

**目的**：QWK 式统一建库函数（v2 §6.1"保留 v1 §5 设计"；v1 原文悬空，规格以下列实证为准，自包含）。

#### S5.1 现状与参考实现证据

- `cmake/SARibbonUtils.cmake` 现为 **16 行**、仅含 `saribbon_set_bin_name` 宏（已核实）。该宏**全仓库零调用**（`git grep saribbon_set_bin_name` 仅命中定义处），且宏体是外来项目残留（内部设 `DA_MIN_QT_VERSION`、`endmacro(damacro_set_bin_name)` 名称与宏不符）——属死代码，可保留不调用或直接删除（建议删除并在提交信息注明；若保留，"备用"二字写明无人使用）。
- 参考实现 QWindowKit 的建库封装 `qwk_add_library` **定义在 `F:\src\3rdparty\qwindowkit\src\CMakeLists.txt:38-126`**（不在 qmsetup/ 里；qmsetup 是 QWK 的 submodule，本地副本未初始化、为空目录，但**其实现已从上游核实**：`.gitmodules` pin 的是 `stdware/qmsetup@99ca80fd63e34f4bb54dcc48db09a66e86f5517d`（QWK `.gitmodules:2-4` + `git submodule status`），round2 评审已通过 GitHub API 抓取该 commit 的 `cmake/QMSetupAPI.cmake`、`cmake/modules/Preprocess.cmake`、`cmake/modules/Filesystem.cmake` 全文核对，下文引用格式 `qmsetup:QMSetupAPI.cmake:行号`）。`sa_*` 函数仍**全部自实现、不依赖 qmsetup**——qmsetup 的 `qm_sync_include` 依赖其自带的外部二进制工具 `corecmd`（`qmsetup:Preprocess.cmake:49-51`，工具缺失直接 FATAL_ERROR），SARibbon 不引入构建期外部工具依赖，此决策不变。
- `qwk_add_library` 的关键机制（**round2 已全部按源码实证**，未注前缀的行号指 `qwindowkit/src/CMakeLists.txt`）：
  - `macro(qwk_add_library _target)`（:38-126），参数 `AUTOGEN / NO_SYNC_INCLUDE / NO_WIN_RC` 选项 + `SYNC_INCLUDE_PREFIX / PREFIX` 单值 + 其余透传 `qm_configure_target`（SOURCES/LINKS/LINKS_PRIVATE/QT_LINKS/QT_INCLUDE_PRIVATE/INCLUDE_PRIVATE 等，签名实证 `qmsetup:QMSetupAPI.cmake:164-196`；实际调用见 `src/core/CMakeLists.txt:88-97`）；
  - `AUTOGEN` 关键字触发的是**目录级** `CMAKE_AUTOMOC/AUTOUIC/AUTORCC ON`（:44-48），非 target 属性——SARibbon 骨架用 target 属性（AUTOMOC/AUTORCC/AUTOUIC ON），等价且作用域更窄，维持；
  - `QWINDOWKIT_BUILD_STATIC` 决定 STATIC/SHARED（:50-54）；WIN32 且 SHARED 且非 NO_WIN_RC 时打版本资源 `qm_add_win_rc`（:58-64）；
  - `qm_export_defines(${_target} PREFIX ${FUNC_PREFIX})` 统一打导出宏（:73；实现实证 `qmsetup:QMSetupAPI.cmake:200-230`：STATIC 库时 `PUBLIC <PREFIX>_STATIC`，而 `<PREFIX>_LIBRARY` **恒 PRIVATE 定义（静态构建也定义）**——两者不互斥，优先级由头文件"先 `ifdef STATIC`"裁决；PREFIX 缺省 = target 名 TOUPPER，QWK 各模块显式传 `QWK_CORE/QWK_WIDGETS/QWK_QUICK`）；
  - C++ 标准**不在宏内**：各模块自行 `set_target_properties(... CXX_STANDARD 17 CXX_STANDARD_REQUIRED TRUE)`（`src/core/CMakeLists.txt:99-102`，widgets/quick 同构）——SARibbon 把 `target_compile_features(PUBLIC cxx_std_17)` 集中进 `sa_add_library`，少一处重复且 PUBLIC 传播给消费者，属有意优于 QWK 的差异，维持；
  - 由 target 名正则 `^QWK(.+)` 推导 `EXPORT_NAME`（QWKCore→Core），并 `add_library(QWindowKit::Core ALIAS QWKCore)`（:83-90）；
  - `install(TARGETS ... EXPORT QWindowKitTargets RUNTIME/LIBRARY/ARCHIVE 按 GNUInstallDirs，三个 DESTINATION 均带 OPTIONAL)`（:100-106），且整段在 `QWINDOWKIT_INSTALL` 守卫内；`install(EXPORT ... NAMESPACE QWindowKit::)` 与 Config/ConfigVersion 生成只在 src 层写一次（:181-212）；
  - include 传播三件套：模块源目录 `.` **PRIVATE**（:80）、构建期生成头目录 PUBLIC `$<BUILD_INTERFACE:.../include>`（:117-124）、安装树 PUBLIC `$<INSTALL_INTERFACE:include/QWindowKit>`（:108-110）。`qm_sync_include` 的机制是调外部工具 `corecmd incsync` 生成**转发头**（一行间接 include 的引用文件，非内容复制；`qmsetup:Preprocess.cmake:27-41` 注释 "Generate indirect reference files"），默认 STANDARD public-private 模式（`qmsetup:Preprocess.cmake:20-22`），安装期经 `install(CODE)` 在安装前缀下重新生成（`qmsetup:Preprocess.cmake:107-140`），支持 `EXCLUDE` 正则（用例：style agent 关闭时排除 `style/` 全部头，`src/core/CMakeLists.txt:85`）。消费端因此可写 `#include <QWKCore/qwkglobal.h>`。

#### S5.2 导出宏三段式准确形态（原 v1 §5.3 模板，此处内联）

QWK 真实头文件 `F:\src\3rdparty\qwindowkit\src\core\qwkglobal.h:12-22`（round3 复核行号；widgets 版 `qwkwidgetsglobal.h` 同构；KDDockWidgets `src/docks_export.h` 亦为同款三段式）：

```cpp
#ifndef QWK_CORE_EXPORT
#  ifdef QWK_CORE_STATIC
#    define QWK_CORE_EXPORT            // 静态：空
#  else
#    ifdef QWK_CORE_LIBRARY
#      define QWK_CORE_EXPORT Q_DECL_EXPORT   // 构建本库：导出
#    else
#      define QWK_CORE_EXPORT Q_DECL_IMPORT   // 消费者：导入
#    endif
#  endif
#endif
```

SARibbon 映射（PREFIX 由 `sa_add_library` 传入）：

| 模块 | PREFIX | 静态宏（PUBLIC） | 构建宏（PRIVATE，动态） | 导出宏 |
|------|--------|------------------|------------------------|--------|
| SARibbonCore | `SA_RIBBON_CORE` | `SA_RIBBON_CORE_STATIC` | `SA_RIBBON_CORE_LIBRARY` | `SA_RIBBON_CORE_EXPORT` |
| SARibbonWidgets | `SA_RIBBON_WIDGETS` | `SA_RIBBON_WIDGETS_STATIC` | `SA_RIBBON_WIDGETS_LIBRARY` | `SA_RIBBON_WIDGETS_EXPORT` |
| SARibbonQml | `SA_RIBBON_QML` | `SA_RIBBON_QML_STATIC` | `SA_RIBBON_QML_LIBRARY` | `SA_RIBBON_QML_EXPORT` |

对应现状（`src/SARibbonBar/CMakeLists.txt:186-199`）：动态定义 PRIVATE `SA_RIBBON_BAR_MAKE_LIB` + PRIVATE `SA_COLOR_WIDGETS_MAKE_LIB`；静态定义 PRIVATE+INTERFACE（≈PUBLIC）`SA_RIBBON_BAR_NO_EXPORT` 与 `SA_COLOR_WIDGETS_NO_DLL`。旧宏名的兼容映射在 S6.3/S6.4 的头文件里做，CMake 侧只发新宏。

**round2 实证补注**：
- **互斥性**：QWK/qmsetup 并**不**在 CMake 层做 `<PREFIX>_LIBRARY` 与 `<PREFIX>_STATIC` 的互斥——`qm_export_defines` 恒 PRIVATE 定义 `_LIBRARY`、静态时再 PUBLIC 定义 `_STATIC`（`qmsetup:QMSetupAPI.cmake:222-228`），完全靠三段式头文件"先 `ifdef STATIC` 走空宏"的顺序裁决（`qwkglobal.h:12-22`）。S5.3 骨架选择的 if/else 互斥发宏与之**行为等价**（且与 2.9.5 现状"静态只发 NO_EXPORT、动态只发 MAKE_LIB"一致，`src/SARibbonBar/CMakeLists.txt:186-199`），同时 S6.9 头文件保持同样优先级顺序 = 双保险，维持骨架写法。
- **旧宏映射方向已核实未写反**：`src/SARibbonBar/SARibbonGlobal.h:9-18` 实证 `SA_RIBBON_BAR_NO_EXPORT` 被定义时 `SA_RIBBON_EXPORT` 为空（静态语义），未定义时按 `SA_RIBBON_BAR_MAKE_LIB` 分 EXPORT/IMPORT；`colorWidgets/SAColorWidgetsGlobal.h:97-111` 同构（`NO_DLL` 外层优先）。故 S6.9 代码块 C 的 `SA_RIBBON_BAR_NO_EXPORT → SA_RIBBON_WIDGETS_STATIC`、`SA_RIBBON_BAR_MAKE_LIB → SA_RIBBON_WIDGETS_LIBRARY` 方向正确。

#### S5.3 sa_add_library 规格与骨架

签名：`sa_add_library(<target> SOURCES <files...> PREFIX <前缀> QT_LINKS <Qt组件...> [LINKS <内部库...>] [LINKS_PRIVATE <私有依赖...>] [NO_WIN_RC])`

行为（与 qwk_add_library 对齐 + SARibbon 现状兼容）：
- STATIC/SHARED 由 `SARIBBON_BUILD_STATIC_LIBS` 决定；导出宏按 S5.2（动态 PRIVATE `<PREFIX>_LIBRARY`，静态 PUBLIC `<PREFIX>_STATIC`）。
- `AUTOMOC/AUTORCC/AUTOUIC ON`、`CXX_EXTENSIONS OFF`、`DEBUG_POSTFIX`、`VERSION`、输出目录三件套——照抄现有 `src/SARibbonBar/CMakeLists.txt:172-183` 的属性集。
- `target_compile_features(PUBLIC cxx_std_17)`。
- `QT_LINKS Core Gui ...` → `Qt${QT_VERSION_MAJOR}::Core` 等（Qt5 的 imported target 也叫 `Qt5::Core`，用 `Qt${QT_VERSION_MAJOR}::` 前缀拼接即可，与现有 L152-156 写法一致）。
- `LINKS SARibbonCore` → 正则改写为 `SARibbon::Core` 再 PUBLIC 链接；`LINKS_PRIVATE` 给 frameless 的 `QWindowKit::Widgets` 这类私有依赖（现 L167）。
- 别名：`add_library(SARibbon::<Comp> ALIAS <target>)`，`<Comp>` 由 `^SARibbon(.+)` 剥离（SARibbonCore→Core，与 QWK 的 `^QWK(.+)` 同款）；`EXPORT_NAME` 同设为 `<Comp>`，使导出的 Targets 文件里出现 `SARibbon::Core/Widgets`（v2 D2）。
- include 传播：**见 S5.5 决策**（源目录与同步目录双 PUBLIC）。
- install：`install(TARGETS <target> EXPORT SARibbonTargets ...)`（RUNTIME/LIBRARY/ARCHIVE 按 GNUInstallDirs，**三个 DESTINATION 均带 `OPTIONAL`**——照抄 QWK `src/CMakeLists.txt:101-106`，静态/动态与平台产物类型差异下容忍某类产物缺席，install 不报错），**整段包在 `if(SARIBBON_INSTALL)` 守卫内**（round2 整合：v2 §6.4，QWK 同段亦在 `QWINDOWKIT_INSTALL` 守卫内，src/CMakeLists.txt:100-106）。**三个模块共用一个 export set `SARibbonTargets`，`install(EXPORT ...)` 只在 `src/CMakeLists.txt` 写一次**（S6.7/S11；QWK 同构：per-module 只 `install(TARGETS ... EXPORT QWindowKitTargets)`，`install(EXPORT)` 唯一一处在其 `src/CMakeLists.txt:208-212`）。Windows 且 `SARIBBON_INSTALL_IN_CURRENT_DIR=ON` 时根 CMakeLists 已把 `CMAKE_INSTALL_PREFIX` 整体指到 `bin_qtX_编译器_x架构/`（现 L144-148），DESTINATION 无需特判，旧行为自动维持。私有头（`*_p.h`）不安装（现仓库尚无 _p.h，规则先立）。
- WIN32 且 SHARED 且未给 NO_WIN_RC 时调 `create_win32_resource_version`（现 L356-363 的行为并入）。

可直接落地的骨架（写入 `cmake/SARibbonUtils.cmake`）：

```cmake
# sa_add_library: unified library creation for SARibbon modules (QWK-style)
function(sa_add_library _target)
    cmake_parse_arguments(FUNC "NO_WIN_RC" "PREFIX"
        "SOURCES;QT_LINKS;LINKS;LINKS_PRIVATE" ${ARGN})

    if(NOT FUNC_PREFIX)
        message(FATAL_ERROR "sa_add_library(${_target}): PREFIX is required")
    endif()

    if(SARIBBON_BUILD_STATIC_LIBS)
        set(_type STATIC)
    else()
        set(_type SHARED)
    endif()

    add_library(${_target} ${_type})
    target_sources(${_target} PRIVATE ${FUNC_SOURCES})

    # Export macros, see plan-01 S5.2 (QWK qwkglobal.h three-way pattern).
    # qm_export_defines always defines <PREFIX>_LIBRARY PRIVATE and adds
    # <PREFIX>_STATIC PUBLIC for static builds (qmsetup:QMSetupAPI.cmake:222-228);
    # the if/else below is the stricter mutually-exclusive variant, matching
    # 2.9.5 behavior. Headers must keep STATIC-first precedence either way.
    if(_type STREQUAL "STATIC")
        target_compile_definitions(${_target} PUBLIC ${FUNC_PREFIX}_STATIC)
    else()
        target_compile_definitions(${_target} PRIVATE ${FUNC_PREFIX}_LIBRARY)
    endif()

    set_target_properties(${_target} PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
        CXX_EXTENSIONS OFF
        DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
        VERSION "${SARIBBON_VERSION}"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
    )
    target_compile_features(${_target} PUBLIC cxx_std_17)

    # SARibbonCore -> Core ; alias SARibbon::Core ; EXPORT_NAME Core
    string(REGEX REPLACE "^SARibbon(.+)$" "\\1" _comp "${_target}")
    set_target_properties(${_target} PROPERTIES EXPORT_NAME ${_comp})
    add_library(SARibbon::${_comp} ALIAS ${_target})

    # Qt components: QT_LINKS Core Gui -> Qt${QT_VERSION_MAJOR}::Core ...
    set(_qt_links)
    foreach(_c IN LISTS FUNC_QT_LINKS)
        list(APPEND _qt_links Qt${QT_VERSION_MAJOR}::${_c})
    endforeach()
    target_link_libraries(${_target} PUBLIC ${_qt_links})

    # Internal modules: LINKS SARibbonCore -> SARibbon::Core
    set(_sa_links)
    foreach(_l IN LISTS FUNC_LINKS)
        string(REGEX REPLACE "^SARibbon(.+)$" "SARibbon::\\1" _a "${_l}")
        list(APPEND _sa_links ${_a})
    endforeach()
    if(_sa_links)
        target_link_libraries(${_target} PUBLIC ${_sa_links})
    endif()
    if(FUNC_LINKS_PRIVATE)
        target_link_libraries(${_target} PRIVATE ${FUNC_LINKS_PRIVATE})
    endif()

    # Include propagation, see plan-01 S5.5 (source dir kept PUBLIC for the
    # 190 flat includes in tests/examples; sync dir gives <SARibbonXxx/...>)
    target_include_directories(${_target} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    # OPTIONAL on every artifact kind, same as QWK (src/CMakeLists.txt:101-106);
    # whole block under SARIBBON_INSTALL guard (v2 §6.4, round2 D1; QWK's
    # install rules live inside QWINDOWKIT_INSTALL the same way)
    if(SARIBBON_INSTALL)
        install(TARGETS ${_target} EXPORT SARibbonTargets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" OPTIONAL
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
        )
    endif()

    if(WIN32 AND NOT FUNC_NO_WIN_RC AND _type STREQUAL "SHARED")
        create_win32_resource_version(
            TARGET ${_target}
            FILENAME ${_target}
            EXT "dll"
            DESCRIPTION "Ribbon control library for Qt"
        )
    endif()
endfunction()
```

#### S5.4 sa_sync_include 规格与骨架

签名：`sa_sync_include(<target> <模块名>)`。把模块源树的公共头（排除 `*_p.h`）在**配置期**复制到 `${CMAKE_BINARY_DIR}/include/<模块名>/`（保持相对子目录，如 `SARibbonWidgets/colorWidgets/SAColorToolButton.h`），清单存入 `<target>_SYNC_FILES`（PARENT_SCOPE），并生成 install 规则。与 QWK 的 `qm_sync_include(. "${QWINDOWKIT_GENERATED_INCLUDE_DIR}/QWKCore" INSTALL_DIR ...)`（qwindowkit/src/CMakeLists.txt:117-125）**语义层一致**（构建期命名空间头目录 + PUBLIC BUILD_INTERFACE + 安装期同布局再生），**机制层不同且有意为之**：QWK 经外部工具 `corecmd incsync` 生成一行式**转发头**（`qmsetup:Preprocess.cmake:27-41,92-104`，每次 configure 带 FORCE 重刷；`_p.h` 排除靠其 STANDARD public-private 默认模式 `:20-22`；安装期 `install(CODE)` 在目标前缀下重跑工具 `:107-140`），SARibbon 用纯 CMake `file(COPY)` 复制内容、`_p\\.h$` 文件名正则显式排除——**不引入构建期外部二进制依赖**（qmsetup 找不到 corecmd 即 FATAL_ERROR，`qmsetup:Preprocess.cmake:49-51`），代价是产物体积略大（真实内容 vs 一行转发），可接受。

```cmake
# sa_sync_include: mirror public headers into ${CMAKE_BINARY_DIR}/include/<module>/
function(sa_sync_include _target _module)
    set(_src_root "${CMAKE_CURRENT_SOURCE_DIR}")
    set(_dst_root "${CMAKE_BINARY_DIR}/include/${_module}")
    file(GLOB_RECURSE _headers RELATIVE "${_src_root}"
        "${_src_root}/*.h" "${_src_root}/*.hpp")
    set(_synced)
    foreach(_h IN LISTS _headers)
        if(_h MATCHES "_p\\.h$")
            continue()   # private headers are neither synced nor installed
        endif()
        get_filename_component(_sub "${_h}" DIRECTORY)   # e.g. colorWidgets
        file(COPY "${_src_root}/${_h}" DESTINATION "${_dst_root}/${_sub}")
        list(APPEND _synced "${_dst_root}/${_h}")
    endforeach()
    set(${_target}_SYNC_FILES "${_synced}" PARENT_SCOPE)
    if(SARIBBON_INSTALL)   # v2 §6.4 (round2 D1)
        install(DIRECTORY "${_dst_root}/"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${_module}"
            FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
    endif()
endfunction()
```

说明：
- 这里的 `file(GLOB_RECURSE)` 只用于**头文件同步清单**（QWK 同款机制），不违反"源文件清单禁止 GLOB"（那条只约束参与编译的 SOURCES）。
- 配置期复制意味着**新增头文件后要重跑 cmake**；本计划接受（QWK 亦然），计划 03 若嫌烦可升级为 custom command。
- **前向注记（round3 终审补，02-dryrun 建议 2）**：计划 02 S1-操作5 将给本函数**增加 `FLATTEN` 选项**（core 头物理在 `global/` 等子目录、对外同步形态平铺为 `include/SARibbonCore/<名>.h`；仅 core 调用传入 FLATTEN，widgets 调用不变以保留 `colorWidgets/` 层级）。这是对 01 交付物的**计划内**构建脚本小改、随 02 S1 提交——01 的验收对账不应把它当作交付物被破坏，此处先行登记以免执行 02 时无据可查。
- **已知良性差异（round3 登记）**：2.9.5 旧头文件安装带 `COMPONENT headers` 标签（现 L292-302），新 `install(DIRECTORY)` 不带 COMPONENT——仓库无 cpack、无 `--component` 消费方，影响仅限组件化安装元数据；qm 翻译的 `COMPONENT translations` 随翻译块原样保留（S6.1 规则②）。记 NOTES，不为此给骨架加参数。
- widgets 模块同步后 `${CMAKE_BINARY_DIR}/include/SARibbonWidgets/` 下含 `SARibbonBar.h` 等全部公共头 + `colorWidgets/` 子目录 + 生成到源码树的 `SARibbonBarVersionInfo.h`（它也在模块目录里，会被同步，正好满足其"随头安装"的现状）。
- **同步/安装集合与 2.9.5 旧 `install(FILES ${SARIBBON_HEADER_FILES})` 的差集（round3 实测）**：唯一新增项是 `SARibbonMdiControlsStyle.h`（在目录、不在旧清单）——GLOB 机制会把它带进同步与安装目录，属良性差异，记 NOTES，勿为对齐旧集合而排除它（详见 S6.1 清单核对注意）；colorWidgets 5 个头两侧集合完全一致，其余 41 个顶层头也一致。

#### S5.5 include 传播决策（修正原稿，关键）

- 现状 2.9.5：`src/SARibbonBar/CMakeLists.txt:221-224` 是 `PUBLIC $<INSTALL_INTERFACE:include/SARibbonBar> + $<BUILD_INTERFACE:源目录>`；tests/examples 共 **190 处**平铺 include（`#include "SARibbonBar.h"` 形式，`git grep -c` 实测），依赖源目录 PUBLIC 传播或显式 include 路径。
- QWK 模式是**源目录 PRIVATE** + 同步目录 PUBLIC，消费端一律 `<QWKCore/qwkglobal.h>` 命名空间形式（实证：`qwindowkit/src/CMakeLists.txt:80` 源目录 `.` PRIVATE、`:122-124` 同步目录 `$<BUILD_INTERFACE:...>` PUBLIC、`:108-110` `$<INSTALL_INTERFACE:include/QWindowKit>` PUBLIC）。安装树布局差异注意：QWK 在 include 下多一层伞目录（`include/QWindowKit/QWKCore/...`，INSTALL_INTERFACE 指向伞目录），SARibbon 计划为**无伞目录**（`include/SARibbonCore/...`，INSTALL_INTERFACE 指向 `include/`）——两种布局下消费端 include 写法相同（`<模块名/头名>`），SARibbon 与 2.9.5 的 `include/SARibbonBar/` 现状同层级，维持无伞目录。
- **本计划取零改动方案**：源目录 PUBLIC（平铺兼容，build 树内）+ 同步目录 PUBLIC（`<SARibbonWidgets/...>` 新形式同时可用）+ INSTALL_INTERFACE 只暴露同步目录形态（安装树里只有 `include/SARibbonCore|SARibbonWidgets|SARibbonBar(转发)`，见 S11）。原稿"PRIVATE 模块源目录"**作废**——它会把 190 处平铺 include 全部打断，违背"行为零变化"。命名空间化改造归计划 03/04。

**验证**：`cmake -P` 级自测不可行，直接以 S6 的三个 target 使用作为验证（S6 构建绿 = S5 正确）。

**提交**：`重构：SARibbonUtils.cmake 实现 sa_add_library/sa_sync_include`

### S6 三模块 target 落地（核心步骤）

**目的**：`src/SARibbonBar` → `src/widgets`，并建出真实的三模块。

**操作**：
1. **搬移**：`git mv src/SARibbonBar src/widgets`（整个目录一次搬，含 colorWidgets/i18n/resource 与其 CMakeLists.txt）。同时把根 CMakeLists 的 `configure_file` VersionInfo 路径（S4.1 表 L200-206 行）改为 `src/widgets/SARibbonBarVersionInfo.h.in` → `src/widgets/SARibbonBarVersionInfo.h`（生成到源码树的旧行为暂保持，计划 02 收编）。

   `src/widgets/CMakeLists.txt` 改写骨架（源文件清单**沿用现有变量与内容**，仅整理为现代命令；禁止对编译源用 file(GLOB)）：

   ```cmake
   # SARibbonWidgets module (migrated from src/SARibbonBar, 2.9.5 file lists kept verbatim)
   find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED
       COMPONENTS Core Gui Widgets Svg)

   # ---- 原有清单变量原样保留（现 CMakeLists.txt 行号供比对）----
   # SACOLOR_DIR/SACOLOR_HEADER_FILES/SACOLOR_SOURCE_FILES   (L11-24)
   # SARIBBON_HEADER_FILES                                   (L33-75)
   #   唯一新增行：SARibbonWidgetsGlobal.h（S6.3 新建的公共头，必须进清单——
   #   IDE 可见性、S11.3 转发头生成循环都按此清单走；round3 补）
   # SARIBBON_SOURCE_FILES                                   (L79-119)
   # SARIBBON_RESOURCE_FILES (SARibbonResource.qrc)          (L123-125)
   # 现 L3 的 set(SARIBBON_LIB_NAME SARibbonBar) **不保留**（见下方替换规则①）

   # ---- frameless（现 L165-169）：find_package 留在本文件，
   #      链接改经 sa_add_library 的 LINKS_PRIVATE（round3 补具体机制）----
   set(_SA_WIDGETS_PRIVATE_LINKS)
   if(_SARIBBON_USE_FRAMELESS_LIB)
       find_package(QWindowKit REQUIRED)
       list(APPEND _SA_WIDGETS_PRIVATE_LINKS QWindowKit::Widgets)
   endif()

   sa_add_library(SARibbonWidgets
       SOURCES
           ${SARIBBON_HEADER_FILES}
           ${SARIBBON_SOURCE_FILES}
           ${SACOLOR_HEADER_FILES}
           ${SACOLOR_SOURCE_FILES}
           ${SARIBBON_RESOURCE_FILES}
           SARibbonQt5Compat.hpp    # git mv 后此位置是 S6.2 新建的转发头（实体在 core），行保留
       PREFIX SA_RIBBON_WIDGETS
       QT_LINKS Core Gui Widgets Svg
       LINKS SARibbonCore
       LINKS_PRIVATE ${_SA_WIDGETS_PRIVATE_LINKS}
   )
   sa_sync_include(SARibbonWidgets SARibbonWidgets)

   # ---- 保留块的两条机械替换规则（round3 补充，逐字执行）----
   # ① 所有随迁保留块——frameless PUBLIC 宏定义块（现 L203-219）、MSVC NOMINMAX
   #    （现 L225-229）、翻译块（现 L234-288：SARIBBON_UPDATE_TRANSLATIONS 选项、
   #    LinguistTools、TS_FILES i18n/*.ts、qt5/qt6_add_translation、POST_BUILD 复制）
   #    ——中的 ${SARIBBON_LIB_NAME} 一律替换为字面量 SARibbonWidgets。
   #    **不得保留现 L3 的 set(SARIBBON_LIB_NAME SARibbonBar)**：SARibbonBar 在本文件
   #    末尾变成 ALIAS target，target_sources/add_custom_command 作用于 ALIAS 会直接
   #    配置报错（"ALIAS target ... may not be used as ..."）。
   # ② 翻译块中 install(FILES ${QM_FILES} DESTINATION .../translations) 一条包进
   #    if(SARIBBON_INSTALL) 守卫（v2 §6.4/NOTES B10 明列 qm 安装规则进守卫）；
   #    POST_BUILD 复制是构建树行为，不属安装规则，保持在守卫外。

   # ---- 旧宏兼容（amalgamate 产物与外部旧构建脚本仍会定义它们）----
   # SA_RIBBON_BAR_MAKE_LIB / SA_RIBBON_BAR_NO_EXPORT / SA_COLOR_WIDGETS_*
   # 的映射全部在头文件层做（S6.3/S6.4），CMake 侧不再发旧宏。

   # ---- 旧 install/包配置块（现 L289-352）从本文件删除，统一由
   #      sa_add_library/sa_sync_include（S5）+ src/CMakeLists.txt（S6.7/S11）承担；
   #      单文件产物 share/SARibbonBar_amalgamate 的安装规则移到 src/CMakeLists.txt
   #      （S6.7 已补，round3——否则 S11.5 安装树清单的 share/ 项会缺失）；
   #      旧 SARibbonBarConfig 兼容包的生成移到 src/CMakeLists.txt（S11.2）----

   # ---- 旧 target 名兼容（D2），两条都必须加 ----
   add_library(SARibbonBar ALIAS SARibbonWidgets)
   add_library(SARibbonBar::SARibbonBar ALIAS SARibbonWidgets)
   ```

   清单核对注意：`SARibbonMdiControlsStyle.h` 存在于目录且其 .cpp 在 `SARIBBON_SOURCE_FILES`（L83），但 **.h 不在 `SARIBBON_HEADER_FILES`**——清单变量维持现状，勿顺手"修复"（编译面零变化）。**但与 S5.4 的关系要说清（round3 修正原稿的内部矛盾）**：`sa_sync_include` 按目录 GLOB 同步，该头**会**因此新进入同步/安装目录（这是新旧安装集合的唯一差异，round3 实测确认；它本就是参与编译的公共头，被安装属良性）——记 NOTES 即可，不要为对齐 2.9.5 旧集合而在 GLOB 里排除它。

2. **core 骨架**：新建 `src/core/`，内容（本计划仅这三项 + CMakeLists，其余计划 02 下沉；文件名与 v2 §3.1 目录结构一致）：
   - `SARibbonCoreGlobal.h`：三段式 `SA_RIBBON_CORE_EXPORT`（S5.2 模板）+ **PIMPL 宏整段 move**。
     **行号修正**：PIMPL 宏区实际在 `SARibbonGlobal.h` 的 **L20-184**（含双语 Doxygen 注释块；宏本体：`SA_RIBBON_DECLARE_PRIVATE` L83-88、`SA_RIBBON_DECLARE_PUBLIC` L103-109、`SA_RIBBON_IMPL_CONSTRUCT` L122-124、`SA_D` L137-139、`SA_DC` L152-154、`SA_Q` L167-169、`SA_QC` L182-184）——原稿"84-105"只截到一半，照抄会丢宏。
     **宏名一律不变**（仍叫 `SA_RIBBON_DECLARE_PRIVATE` 等，不加 CORE 前缀），只换定义文件，全仓库使用处零改动。
     另把 `sa_as_const`（原 Global.h L274-283，纯 std/Qt 版本分支）一并 move 到 core。
     完整内容见 S6.9 代码块 A。
   - `SARibbonQt5Compat.hpp`：`git mv src/widgets/SARibbonQt5Compat.hpp src/core/`（已核实其 include 仅 QtCore/QtGui：QtGlobal/QObject/QMouseEvent/QKeyEvent/QWheelEvent/QFontMetrics(F)，无 QWidget/QAction，纯净性门禁可过；round3 复扫 `grep -n "QWidget\|qApp\|QAction"` 零命中）。**引用面（round3 实测更正：原稿"38 个文件"系误抄 SARibbonGlobal.h 的数字）**：widgets 内仅 **5 个 .cpp** 平铺 include 它（SAFramelessHelper.cpp:10、SARibbonBar.cpp:26、SARibbonGallery.cpp:12、SARibbonPanelLayout.cpp:11、SARibbonToolButton.cpp:18，复核命令 `git grep -rn "SARibbonQt5Compat" src/SARibbonBar`）；另有模块 CMakeLists 清单 2 行（现 L36/L143）、amalgamate 模板 PublicHeaders.h 1 处、python 打包配置（pyproject*/pyside6，归计划 03）。构建树内平铺名可经 `SARibbon::Core` 的 PUBLIC 源目录传播解析；但**同步目录/安装树**中平铺名不在 include 根下（实体位于 `include/SARibbonCore/`），且搬走后清单行 L36/L143 指向的文件不存在会直接配置失败。因此 widgets 侧保留同名**转发头** `src/widgets/SARibbonQt5Compat.hpp`（新文件，UTF-8 无 BOM），全文（round3 补）：

     ```cpp
     #pragma once
     // 3.0 compatibility forwarding header: the real file moved to SARibbonCore (plan-01 S6.2).
     #include <SARibbonCore/SARibbonQt5Compat.hpp>
     ```

     ——构建树、同步/安装树、amalgamate 镜像（S8 第 3 条）三态均可解析，5 处 .cpp 引用与清单行零改动。
   - `SARibbonCoreConfig.h.in`：版本（`@PROJECT_VERSION@` 派生）+ feature 开关占位（`SA_RIBBON_CONFIG` 段，v2 §3.1），由 **`src/core/CMakeLists.txt`**（见下方代码块；原稿误写 `src/CMakeLists.txt`，round3 更正）的 `configure_file` 生成到 **build 树** `${CMAKE_BINARY_DIR}/include/SARibbonCore/SARibbonCoreConfig.h`（新文件从一开始就进同步目录形态）。**.in 全文（round3 补具体内容，新文件 UTF-8 无 BOM）**：

     ```cpp
     #ifndef SARIBBONCORECONFIG_H
     #define SARIBBONCORECONFIG_H
     /* Generated from src/core/SARibbonCoreConfig.h.in by cmake. Do not edit. */
     #define SARIBBON_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
     #define SARIBBON_VERSION_MINOR @PROJECT_VERSION_MINOR@
     #define SARIBBON_VERSION_PATCH @PROJECT_VERSION_PATCH@
     #define SARIBBON_VERSION "@PROJECT_VERSION@"
     /* == SA_RIBBON_CONFIG feature switches (v2 §3.1) ==
      * 计划 02 起在此追加 #cmakedefine01 SA_RIBBON_CONFIG_xxx 行 */
     #endif  // SARIBBONCORECONFIG_H
     ```

     `@PROJECT_VERSION*@` 在 src/core 目录作用域取值：S6 重写后模块层**不再有** `project()` 调用，最近的 project 是根 `project(SARibbon VERSION 3.0.0)`，故三宏 = 3/0/0，语义正确。旧 `src/widgets/SARibbonBarVersionInfo.h(.in)` 暂保留生成等价内容（计划 02 收编）。
   - `src/core/CMakeLists.txt`：

     ```cmake
     find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED COMPONENTS Core Gui)

     configure_file(
         "${CMAKE_CURRENT_SOURCE_DIR}/SARibbonCoreConfig.h.in"
         "${CMAKE_BINARY_DIR}/include/SARibbonCore/SARibbonCoreConfig.h"
     )

     sa_add_library(SARibbonCore
         SOURCES
             SARibbonCoreGlobal.h
             SARibbonCoreGlobal.cpp   # 1 行占位源（仅 include 本头），使纯头模块可成库，计划 02 被真实源取代
             SARibbonQt5Compat.hpp
             "${CMAKE_BINARY_DIR}/include/SARibbonCore/SARibbonCoreConfig.h"
         PREFIX SA_RIBBON_CORE
         QT_LINKS Core Gui
     )
     sa_sync_include(SARibbonCore SARibbonCore)
     ```

     （纯头 target 需至少一个可编源文件才能成 SHARED 库；MSVC 下空 DLL 会告警。处理：给 core 加一个最小 `SARibbonCoreGlobal.cpp`（仅 `#include "SARibbonCoreGlobal.h"`），或将 SOURCES 中任一未来 .cpp 提前——**推荐新建 1 行占位 .cpp**，计划 02 下沉时自然被真实源文件取代。此细节原稿未提，执行时按此办理并记 NOTES。）

3. **widgets 兼容头**：`src/widgets/SARibbonGlobal.h` 改为兼容转发（保留原文件名；实测 **38 个 C++ 文件** include 它，全部零改动。**round3 校准验证命令**：`git grep -rl "SARibbonGlobal.h" -- 'src/SARibbonBar/*.h' 'src/SARibbonBar/*.cpp' 'src/SARibbonBar/*.hpp' | wc -l` = 38；原稿不带 pathspec 的 `git grep -rl "SARibbonGlobal.h" src/SARibbonBar | wc -l` 会多命中 CMakeLists.txt 得 **39**，勿以 39 为异常）。完整内容见 S6.9 代码块 B。**编码警示（round3 新增，R1 相关）**：该文件现为 **UTF-8 带 BOM + CRLF**（`file` 实测），重写时必须保持 BOM 与 CRLF——丢 BOM/换行尾会产生整文件 diff 并改变 MSVC 对中文注释的解析方式，属违反 R1"不动文件编码"。新增 `src/widgets/SARibbonWidgetsGlobal.h`（新文件：UTF-8 无 BOM、行尾 CRLF 随目录惯例；完整内容见 S6.9 代码块 C）：`SA_RIBBON_WIDGETS_EXPORT` 三段式 + `#define SA_RIBBON_EXPORT SA_RIBBON_WIDGETS_EXPORT` + 旧宏映射（`SA_RIBBON_BAR_MAKE_LIB`/`SA_RIBBON_BAR_NO_EXPORT` → 新三段式等价分支；原 v1 §5.3 引用悬空，映射规则即代码块 C 所示）。
   **枚举不搬**：`SARibbonAlignment`(L197)/`SARibbonTheme`(L220)/`SARibbonMainWindowStyleFlag`(L245) 三个枚举与 `SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE`(L270-272) 留在转发后的 `SARibbonGlobal.h`/`SARibbonWidgetsGlobal.h`（下沉 core 是计划 02 的事，本计划只动宏与 include 结构）。
4. **colorWidgets 宏废除（D4）**：**实名修正**——导出宏是 `SA_COLOR_WIDGETS_API`（不是 `SA_COLOR_WIDGETS_EXPORT`；证据 `colorWidgets/SAColorWidgetsGlobal.h:97-111`，守卫宏 `SA_COLOR_WIDGETS_NO_DLL`/`SA_COLOR_WIDGETS_MAKE_LIB`）。改法：该文件的 API 宏段（L97-111）替换为（**round3 修订：保留 NO_DLL 最外层优先分支**，与 2.9.5 的嵌套结构语义逐项对齐，覆盖"外部旧脚本只定义 NO_DLL"的边界场景；amalgamate 下 NO_DLL 与 BAR_NO_EXPORT 同定义，两条路径结果一致均为空宏）：

   ```cpp
   #include "../SARibbonWidgetsGlobal.h"   // D4: colorWidgets 导出宏并入 widgets
   #ifndef SA_COLOR_WIDGETS_API
   #  ifdef SA_COLOR_WIDGETS_NO_DLL
   #    define SA_COLOR_WIDGETS_API                 // 2.x 语义保留：NO_DLL 最外层优先（round3 补）
   #  else
   #    define SA_COLOR_WIDGETS_API SA_RIBBON_WIDGETS_EXPORT
   #  endif
   #endif
   ```

   **编码注（round3 新增）**：`SAColorWidgetsGlobal.h` 为 UTF-8 无 BOM + CRLF（`file` 实测），编辑时保持原编码与行尾。

   include 形式**必须用 `"../SARibbonWidgetsGlobal.h"`**（相对本文件上一级），四种消费场景才可全部解析：源码树（`src/widgets/colorWidgets/../` = `src/widgets/`）、同步目录（`include/SARibbonWidgets/colorWidgets/../`）、安装树（同前）、amalgamate（相对包含文件目录解析）。平铺 `"SARibbonWidgetsGlobal.h"` 在安装树断裂（消费者 include 根下没有该文件），`<SARibbonWidgets/...>` 则需给 amalgamate 再加一层镜像（S8），都不如相对上一级简单。
   **第五种消费场景（round3 终审补，03-dryrun 建议 1 采纳）**：Python 绑定三轨（sip×2/pyside6）**直接编译源码树**，转发头的 `<SARibbonCore/...>` 在上述四态之外——其解析由计划 03 S3.0 的绑定侧自建镜像承担（sip 系仓库根 `build-binding-include/SARibbonCore/`、pyside6 在其 CMakeLists 内 `_sync_include/`，与 S8 的 `_amalg_include` 同构），本计划不动绑定文件，两侧决策互认（NOTES B13）；02 S1.1-4 的 sip 冒烟也因此顺延至 03 S3。

   其余内容**保留不动**：版本宏 `SA_COLOR_WIDGETS_VERSION_MAJ/MIN/PAT`、`SA_COLOR_WIDGETS_DECLARE_PRIVATE/PUBLIC`（QScopedPointer 版 PIMPL，与 core 的 unique_ptr 版并存，5 个文件在用）、`sacolor_as_const`。`SA_COLOR_WIDGETS_NO_DLL` 宏在新结构下不再被 CMake 定义，但 amalgamate 模板仍会定义它（S8），且上面代码块的 NO_DLL 优先分支会消费之（与 2.9.5 语义一致，round3 修订后不再是"无副作用的悬空符号"），保留以防外部旧脚本。
5. **qml 空骨架**：建 `src/qml/SARibbonQmlGlobal.h`（三段式 `SA_RIBBON_QML_*`，照 S5.2 模板；新文件 UTF-8 无 BOM）+ `src/qml/CMakeLists.txt` **只放占位**：`message(STATUS "SARibbonQml: placeholder, implemented in plan 04")`，**不建 target**（无源文件的 add_library 会直接报错；这是默认路径而非原稿的"报错才降级"）。`src/CMakeLists.txt` 里 `if(SARIBBON_BUILD_QML) add_subdirectory(qml) endif()` 照常写（QWK 同构：`src/CMakeLists.txt:133-139` 的 `if(QWINDOWKIT_BUILD_WIDGETS/QUICK) add_subdirectory(...)`）。
   **与 v2 §8-M0 判据的字面偏差（round3 登记，有意决策）**：v2 M0 交付判据写"三空 target 可配置安装"，而本计划 qml 无 target——理由是占位 .cpp 造出的空 `SARibbonQml.dll` 会新增 2.9.5 不存在的安装产物、违反行为零变化，且无源文件的 add_library 不可构建；S11.5"安装清单无 qml 项即终态正确"与此一致。M0 判据按"**三模块目录可配置，core/widgets 真实 target 可安装，qml 占位 message 可配置**"解读；SARibbonQml 真 target 归计划 04。

   **QWK quick 模块构建实证（round2 补充，供计划 04 的构建/安装侧直接参考；QML 类型实现细节归 04，此处只记构建事实）**：
   - QWK 1.0.1 的 `src/quick/CMakeLists.txt`（全文 37 行，round3 `wc -l` 实测；round2 记录 38 行差 1，不影响结论）建的 QWKQuick 是**纯 C++ 库**：`qwk_add_library(QWKQuick AUTOGEN SOURCES ... LINKS QWKCore QT_LINKS Core Gui Quick PREFIX QWK_QUICK)`（:22-29）——**没有 qt_add_qml_module、没有 qmldir、没有 QML 文件、没有 qml 目录安装规则**，安装与普通库完全同构（走 qwk_add_library 的统一 install）。
   - QML 类型注册是**命令式导出函数**：`QWK_QUICK_EXPORT void registerTypes(QQmlEngine*)`（`qwkquickglobal.h:27`），实现为 `qmlRegisterType<QuickWindowAgent>("QWindowKit", 1, 0, "WindowAgent") + qmlRegisterModule(...)` 带一次性守卫（`qwkquickglobal.cpp:14-25`），由应用 `main()` 在 `engine.load()` 前手动调用（`examples/qml/main.cpp:26`）；qml 文件放**应用侧 qrc**（`examples/qml/qml.qrc`），不进库。
   - 对 SARibbonQml 的含义：v2 §5.3 双轨中 **Qt5 命令式轨有 QWK 完整同款先例**（库 + `SA_RIBBON_QML_EXPORT saRibbonRegisterQmlTypes(QQmlEngine*)`）；**Qt6 `qt_add_qml_module` 轨在 QWK 1.0.1 无任何先例可抄**（其 qmldir/plugin 安装布局、URI/IMPORT_VERSION 设置需计划 04 自行设计）。**【round2 整合更新】计划 04 S1 已定型为命令式单轨（Qt5/Qt6 同码，v2 §5.3 已修订、NOTES B9），`qt_add_qml_module` 降为 3.1+ 预案（04 S1 附注）——"Qt6 轨无先例"的风险已随单轨化消除；03-S6 安装树清单维持"无 qml 项"即为终态**（叶子进 qrc 随库二进制、安装期零新增产物，v2 §6.1 批注②、kddw-qtquick-findings.md §三.2）。
   - 链接细节：QWKQuick 只显式链 `Core Gui Quick` 不链 `Qml`（Qt6 下 Qml 由 Quick 传递；`src/quick/CMakeLists.txt:25`），而 QML **示例**显式链 `Core Gui Qml Quick`（`examples/qml/CMakeLists.txt:7`）——SARibbonQml 建 target 时建议显式 `QT_LINKS Core Gui Qml Quick`（Qt5/Qt6 两轨都稳），示例照 QWK 显式四件。
6. **旧 target 名兼容（D2）**：见第 1 条骨架末尾——`SARibbonBar` 与 `SARibbonBar::SARibbonBar` **两个别名都必须加**（原稿只写了前者）：tests 链裸 `SARibbonBar`（tests/CMakeLists.txt:10，round3 核实行号），10 个 example 链 `SARibbonBar::SARibbonBar` 且以 `if(NOT TARGET SARibbonBar)` 作独立构建探测（如 example/MainWindowExample/CMakeLists.txt:25-30）。过渡一个版本周期，计划 03 统一改 `SARibbon::Widgets`。
7. **src/CMakeLists.txt**：现内容仅 `add_subdirectory(SARibbonBar)` + 注释掉的 DesignerPlugin 块（**现文件带 UTF-8 BOM，round3 `file` 实测；重写时保持 BOM，R1**）。改为：

   ```cmake
   add_subdirectory(core)
   if(SARIBBON_BUILD_WIDGETS)
       add_subdirectory(widgets)
   endif()
   if(SARIBBON_BUILD_QML)
       add_subdirectory(qml)
   endif()
   # 统一导出（三模块共用一个 export set，只在这里 install 一次；
   # SARIBBON_INSTALL 守卫内——v2 §6.4/round2 D1，与 S5.3/S11 的安装规则同守卫）
   if(SARIBBON_INSTALL)
       install(EXPORT SARibbonTargets
           FILE SARibbonTargets.cmake
           NAMESPACE SARibbon::
           DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SARibbon)
       # 2.9.5 行为保持（round3 补缺）：单文件产物安装规则——原模块 CMakeLists
       # L306-313 被 S6.1 删除后由这里承担，否则 S11.5 清单的 share/ 项缺失
       install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/SARibbon.h
                     ${CMAKE_CURRENT_SOURCE_DIR}/SARibbon.cpp
           DESTINATION share/SARibbonBar_amalgamate)
   endif()
   # 组件化 SARibbonConfig 的生成/安装见 S11（同样在 SARIBBON_INSTALL 守卫内）
   ```

   QWK 同构实证（`qwindowkit/src/CMakeLists.txt`）：子目录按选项进（:131-139）；`install(EXPORT QWindowKitTargets FILE QWindowKitTargets.cmake NAMESPACE QWindowKit:: DESTINATION lib/cmake/QWindowKit)` 全项目唯一一处（:208-212）；Config/ConfigVersion 的生成与安装也集中在 src 层（:181-205），模块层只 `install(TARGETS ... EXPORT ...)`。组件间依赖（Widgets→Core）无需在 Config 里手写：`LINKS QWKCore` 以 PUBLIC 进 target 属性（`src/widgets/CMakeLists.txt:23` + `qmsetup:QMSetupAPI.cmake:186`），导出的 Targets 文件自动携带 `INTERFACE_LINK_LIBRARIES QWindowKit::Core`——SARibbon 的 `LINKS SARibbonCore`（S6.1 骨架）同理自动传播。

8. i18n/resource 路径盘点：`git grep -n "SARibbonBar/i18n\|SARibbonBar/resource"` 实测 **CMake 构建系统内零命中**（.ts 在模块 CMakeLists 里是相对模块目录的 `i18n/*.ts`，随目录整体搬移不受影响）。**命中面（round3 更正原稿"仅 docs 与 AGENTS.md"的不完整表述）**：`docs/` 8 个文件（归计划 04）、`AGENTS.md`（S12 改）、`plans/`（评审文档自引用，不计）、`tests/ThemeCoverageTest.cpp:48`（S7.3 改）、`tools/qrc_SARibbonResource_Datas.cpp` 与 `src/SARibbon.cpp`（rcc 生成注释及其内联副本，S8/§6 门禁 B 已豁免）——全部有归属，无遗漏项。

#### S6.9 附注（非执行步骤，供第 2/3 条引用的代码块）：三个全局头的完整内容

**代码块 A：`src/core/SARibbonCoreGlobal.h`**（新文件，UTF-8 无 BOM；PIMPL 宏从 SARibbonGlobal.h L20-184 整段 move，双语注释随行；此处省略注释正文，执行时用 `git mv` 级别的整段剪切保证注释不丢。round3 复核：L20 = 首个 `/**` 文档注释起始、L184 = `SA_QC` 块的 `#endif`，边界精确）：

```cpp
#ifndef SARIBBONCOREGLOBAL_H
#define SARIBBONCOREGLOBAL_H
#include <memory>
#include <QtGlobal>
#include <QObject>
// 注意：不要在此 include SARibbonCoreConfig.h —— 它由 configure_file 生成到 build 树
// （${CMAKE_BINARY_DIR}/include/SARibbonCore/），源码树与 amalgamate 单文件场景都看不到该文件。
// 需要 feature 开关的翻译单元显式 include <SARibbonCore/SARibbonCoreConfig.h>（计划 02 起使用）。

// 三段式导出宏（模板见计划 01 S5.2，QWK qwkglobal.h:12-22 同款）
#ifndef SA_RIBBON_CORE_EXPORT
#  ifdef SA_RIBBON_CORE_STATIC
#    define SA_RIBBON_CORE_EXPORT
#  else
#    ifdef SA_RIBBON_CORE_LIBRARY
#      define SA_RIBBON_CORE_EXPORT Q_DECL_EXPORT
#    else
#      define SA_RIBBON_CORE_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif

// ==== 以下 PIMPL 宏区 = 原 SARibbonGlobal.h L20-184 整段原样 move ====
// SA_RIBBON_DECLARE_PRIVATE / SA_RIBBON_DECLARE_PUBLIC / SA_RIBBON_IMPL_CONSTRUCT
// SA_D / SA_DC / SA_Q / SA_QC（含全部双语 Doxygen 注释，宏名与定义体一字不改）
// ==== move 结束 ====

// sa_as_const：原 SARibbonGlobal.h L274-283 整段 move（C++17 std::as_const / C++14 qAsConst 分支）

#endif  // SARIBBONCOREGLOBAL_H
```

注意：core 版不再 `class QWidget;` 前置声明（原 Global.h L7 属 widgets 需要，留 widgets 侧）。

**代码块 B：`src/widgets/SARibbonGlobal.h`（改后全文骨架；保持 UTF-8 BOM + CRLF，见 S6.3 编码警示）**：

**round3 警示：下面代码块中 `// ==== ... ====` 注释是占位说明，不是文件内容**——执行时必须把原 SARibbonGlobal.h L186-272 的**真实代码**（三个枚举全文 + `Q_DECLARE_FLAGS`(L252) + `Q_DECLARE_OPERATORS_FOR_FLAGS`(L253) + `SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE` 宏及其双语注释）原样置于该位置；照抄占位注释会删除公共 API（编译期即爆，但属可避免返工）。

```cpp
#ifndef SARIBBONGLOBAL_H
#define SARIBBONGLOBAL_H
// 3.0 兼容转发头：原内容拆分至 SARibbonCore/SARibbonCoreGlobal.h（PIMPL/导出宏基座）
// 与 SARibbonWidgetsGlobal.h（widgets 导出宏），本文件保留以兼容既有 include。
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include "SARibbonWidgetsGlobal.h"
#include "SARibbonBarVersionInfo.h"   // 原 Global.h:6 行为保持：版本宏随全局头可见
class QWidget;                        // 原 Global.h:7 前置声明保留（widgets 侧需要）

// ==== 【占位说明，执行时替换为真实代码】三个枚举（SARibbonAlignment/SARibbonTheme/
//      SARibbonMainWindowStyleFlag，原 L186-253 含注释与 Q_DECLARE_FLAGS/
//      Q_DECLARE_OPERATORS_FOR_FLAGS）与 SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE（原 L255-272）
//      原样保留在本文件，计划 02 再下沉 core ====

#endif  // SARIBBONGLOBAL_H
```

**代码块 C：`src/widgets/SARibbonWidgetsGlobal.h`（新文件全文建议；round3 重排——兼容映射必须在三段式之前，原顺序是阻塞级错误，见下方说明）**：

```cpp
#ifndef SARIBBONWIDGETSGLOBAL_H
#define SARIBBONWIDGETSGLOBAL_H
#include <SARibbonCore/SARibbonCoreGlobal.h>

// 2.x 兼容：旧构建脚本 / amalgamate 产物定义 SA_RIBBON_BAR_MAKE_LIB / SA_RIBBON_BAR_NO_EXPORT。
// 顺序不可调换：#ifdef 指令在定义处即时求值（非惰性），本映射必须先于三段式，
// 否则旧宏场景下 SA_RIBBON_WIDGETS_EXPORT 被固化为 Q_DECL_IMPORT（round3 修正）。
#if defined(SA_RIBBON_BAR_NO_EXPORT) && !defined(SA_RIBBON_WIDGETS_STATIC)
#  define SA_RIBBON_WIDGETS_STATIC
#endif
#if defined(SA_RIBBON_BAR_MAKE_LIB) && !defined(SA_RIBBON_WIDGETS_LIBRARY)
#  define SA_RIBBON_WIDGETS_LIBRARY
#endif

// 三段式导出宏（S5.2 模板；STATIC 优先于 LIBRARY，与 2.9.5 的 NO_EXPORT 外层优先一致）
#ifndef SA_RIBBON_WIDGETS_EXPORT
#  ifdef SA_RIBBON_WIDGETS_STATIC
#    define SA_RIBBON_WIDGETS_EXPORT
#  else
#    ifdef SA_RIBBON_WIDGETS_LIBRARY
#      define SA_RIBBON_WIDGETS_EXPORT Q_DECL_EXPORT
#    else
#      define SA_RIBBON_WIDGETS_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif

// 2.x 公共符号：SA_RIBBON_EXPORT ≡ SA_RIBBON_WIDGETS_EXPORT（v2 §4.4 兼容层；
// 对象宏惰性展开，此定义位置不受上面顺序影响）
#ifndef SA_RIBBON_EXPORT
#  define SA_RIBBON_EXPORT SA_RIBBON_WIDGETS_EXPORT
#endif

#endif  // SARIBBONWIDGETSGLOBAL_H
```

注意宏求值顺序（**round3 修正，原稿论证有误**）：原稿称"C 预处理器是惰性展开，先三段式、后映射的顺序安全"——**错误**。惰性的只有对象宏（`SA_RIBBON_EXPORT → SA_RIBBON_WIDGETS_EXPORT`）的展开；三段式内部的 `#ifdef SA_RIBBON_WIDGETS_STATIC` 是**预处理指令，在其定义处即时求值**。若映射放在三段式之后，用旧宏构建时（amalgamate/StaticExample/外部旧脚本定义 `SA_RIBBON_BAR_NO_EXPORT`）三段式求值点看不到 `SA_RIBBON_WIDGETS_STATIC`，`SA_RIBBON_WIDGETS_EXPORT` 将被永久固化为 `Q_DECL_IMPORT`（外层 `#ifndef` 阻止重定义），单文件产物编译必失败（MSVC 下 dllimport 类不可定义）。故代码块 C 已把映射**置于三段式之前**。**构建系统侧仍须保证**：用旧宏构建时，`SA_RIBBON_BAR_NO_EXPORT` 在 include 本头之前已定义（amalgamate 模板正是这么做的——Template.h:4-6 与 Template.cpp:2-4 都在文件头部先定义，S8 已核实）。

**验证**：
```bash
pwsh -NoProfile -File scripts/build.ps1 rebuild -Tests ON -Examples ON
ctest --test-dir build -C Release --output-on-failure   # 通过数 == N₀（=26）
```
验证后删除 S4 第 5 条的全局 `CMAKE_CXX_STANDARD` 两行（此时所有 target 已被 cxx_std_17 覆盖），再跑一次增量构建确认仍绿。

**提交**：`重构：src/SARibbonBar 拆分为三模块目录（core/widgets/qml 骨架）`

### S7 tests 与 examples 的 include/link 适配

**现状核实**（据此修正原稿数字与路径）：
- tests 顶层 **25 个 .cpp**，另有 `tests/auto/SARibbonThemePalette/tst_themepalette.cpp`；`tests/CMakeLists.txt` 共 **26 个 `add_saribbon_test()` 注册**（其中 `SARibbonThemePaletteTest` 用 `auto/...` 相对路径源）。原稿"24 个"不确。
- `add_saribbon_test()` 函数定义在 `tests/CMakeLists.txt:7-44`（round3 核实行号）：链接裸 `SARibbonBar`（:10）+ Qt Test/Core/Gui/Widgets；`target_include_directories(... ${CMAKE_CURRENT_SOURCE_DIR}/../src/SARibbonBar)`；定义 `QT_TESTCASE_SOURCEDIR="${CMAKE_CURRENT_SOURCE_DIR}"`；非静态构建时 POST_BUILD 复制 `$<TARGET_FILE:SARibbonBar>` 到测试输出目录；`RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests`；`TIMEOUT 120`。
- examples：10 个库消费型示例统一 `target_link_libraries(... SARibbonBar::SARibbonBar)` + `if(NOT TARGET SARibbonBar) find_package(SARibbonBar REQUIRED)` 独立构建 fallback；StaticExample 直接编 `src/SARibbon.h/.cpp` 单文件。

**操作**：
1. tests 搬移（**含 auto/，原稿遗漏**）：

   ```bash
   mkdir tests/widgets
   git mv tests/*.cpp tests/widgets/
   git mv tests/auto tests/widgets/auto
   ```

   `add_saribbon_test()` 函数定义整体移入新的 `tests/widgets/CMakeLists.txt`（26 个注册调用随迁，`auto/SARibbonThemePalette/tst_themepalette.cpp` 相对路径在新位置不变）；`tests/CMakeLists.txt` 重建为：`find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test Core Gui Widgets REQUIRED)` + `add_subdirectory(widgets)`。
2. 函数体内 3 处适配：
   - `target_include_directories(... ../src/SARibbonBar)`：**直接删除**（S5.5 决策下 `SARibbonWidgets` target 的 PUBLIC 源目录传播已覆盖平铺 include）。若需临时显式路径，从 `tests/widgets/` 出发是 `${CMAKE_CURRENT_SOURCE_DIR}/../../src/widgets`——**原稿写的 `../src/widgets` 少一级，是错的**。
   - `target_link_libraries(... SARibbonBar ...)`：靠 D2 别名可零改动；建议本步顺手改为 `SARibbonWidgets`（一处函数定义，26 个测试全部生效）。
   - POST_BUILD 复制 `$<TARGET_FILE:SARibbonBar>` → `$<TARGET_FILE:SARibbonWidgets>`（与上一条同步改）。
3. **测试源内路径硬编码 1 处**（原稿遗漏）：`tests/ThemeCoverageTest.cpp:47-48`
   `QDir(QT_TESTCASE_SOURCEDIR).absoluteFilePath("../src/SARibbonBar/resource")` →
   `"../../src/widgets/resource"`（SOURCEDIR 随 CMakeLists 移到 tests/widgets/ 后自动变为新目录，相对段需同步加一级并改模块名）。复核命令 `git grep -n "src/SARibbonBar" tests` 现命中 **2** 处（round3 澄清口径）：本条的 `ThemeCoverageTest.cpp:48`（测试源代码内唯一硬编码）+ `tests/CMakeLists.txt:17`（即第 2 条删除的 include 路径，非测试源代码）；S7 全部完成后该 grep 应归零。
4. examples：**统一决策为保持现状零改动**（原稿"二选一"作废）——链接继续走 `SARibbonBar::SARibbonBar` 别名（S6.6 保证存在），include 继续平铺（S5.5 保证传播）；`find_package(SARibbonBar REQUIRED)` fallback 由 S11.2 的兼容包继续支撑。切换到 `SARibbon::Widgets` 与 `<SARibbonWidgets/...>` 归计划 03。唯一例外是 StaticExample 的 `SARIBBON_DIR`（已在 S2 第 4 条修复）。
   QWK 对照（round2 实证，支持既有方向、不改决策）：QWK 树内消费者一律链**裸 target 名**（examples 的 `LINKS QWKWidgets/QWKQuick` → `target_link_libraries PUBLIC`，`examples/mainwindow/CMakeLists.txt:5-9`、`examples/qml/CMakeLists.txt:5-9`、`qmsetup:QMSetupAPI.cmake:186`；共享辅助库 WidgetFrame 也是裸名 STATIC 库 `examples/shared/widgetframe/CMakeLists.txt:9-14`），命名空间别名 `QWindowKit::*` 在树内定义（`src/CMakeLists.txt:90`）但主要服务安装导出（`install(EXPORT ... NAMESPACE)` :208-212）与外部消费。SARibbon 的对应终态即：tests 链裸 `SARibbonWidgets`（本 S7 第 2 条已建议）、examples 在 03 切 `SARibbon::Widgets`（安装态消费口径）、别名只作过渡兼容。
5. 根 CMakeLists tests 开关改 `SARIBBON_BUILD_TESTS`（S4 已就位，此处仅确认生效）。

**验证**：构建 + `ctest --test-dir build -C Release --output-on-failure` 通过数 == N₀（26）；`MainWindowExample` 运行冒烟（手动/截图对比 2.9.5，视觉应完全一致）；`git log --follow tests/widgets/SARibbonUtilTest.cpp` 历史可见（同 S2：**提交之后**执行，提交前 `--follow` 无输出属正常）。

**提交**：`重构：tests 与 examples 适配三模块 target`

### S8 amalgamate 应急适配（保持单文件发行可用）

**现状核实**：
- `tools/Amalgamate.sh`：`DEST=../src`；`OPTS='-i "../src/SARibbonBar" -i "../src/SARibbonBar/colorWidgets" -w "*.cpp;*.h;*.hpp" -s'`；调用 `./Amalgamate.exe $OPTS ./amalgamate/SARibbonAmalgamTemplate.{h,cpp} $DEST/SARibbon.{h,cpp}`；产物生成后有 awk LF→CRLF 转换（原稿 CRLF 说法属实）；**脚本末尾有 `read -n 1` 交互等待**；所有相对路径以 `tools/` 为工作目录。
- **模板硬编码旧路径（原稿遗漏，必改）**：`tools/amalgamate/SARibbonAmalgamTemplatePublicHeaders.h` 有 **46 处** `../../src/SARibbonBar/...` include；`SARibbonAmalgamTemplate.cpp` 有 **43 处**（colorWidgets 4 个 .cpp + 各控件 .cpp）。只改 OPTS 不改模板，生成会直接失败。
- 两个模板头部都先定义 `SA_RIBBON_BAR_NO_EXPORT` 与 `SA_COLOR_WIDGETS_NO_DLL` 再 include（round3 核实行号：Template.h:4-10、Template.cpp:2-8）——合并宏后这两个定义仍有效且被消费（S6.9 代码块 C 的兼容映射消费前者；S6.4 修订后的 `SAColorWidgetsGlobal.h` NO_DLL 优先分支消费后者），**保留不动**。
- `tools/qrc_SARibbonResource_{Datas,version2,version3}.cpp`：rcc 快照（Qt 5.14.2 生成），被模板 cpp 以 `#include "../qrc_SARibbonResource_*.cpp"` 引用；`src/SARibbonBar` 字样共 32 处、**全部在 `qrc_SARibbonResource_Datas.cpp` 的生成注释里**（version2/version3 为 0 处，round3 逐文件计数），资源字节与路径无关，**无需再生**（验收门 grep 对其豁免，见 §6；这些注释会随内联进入重生成后的 `src/SARibbon.cpp`，即门禁 B 的豁免对象）。
- **编码警示（NOTES B4）**：`tools/Amalgamate.sh` 是全仓唯一 GBK 编码文件。编辑它必须保持 GBK 原编码保存；编辑器静默转 UTF-8 会造成整文件 diff 并违反 R1 禁转码令。改完用 `git diff --stat tools/Amalgamate.sh` 确认只有预期行变化。

**操作**（第 1~3 条的改动全部写进 `tools/Amalgamate.sh` 脚本本体，不是手工命令）：
1. `OPTS` 改为 `-i "../src/widgets" -i "../src/widgets/colorWidgets" -i "../src/core" -i "_amalg_include" -w "*.cpp;*.h;*.hpp" -s`。
2. 模板路径批量替换（**只需这一条通用 sed，无需手工插入 core 头、无需对 Qt5Compat 特殊处理**）：

   ```bash
   cd tools/amalgamate
   sed -i 's|src/SARibbonBar/|src/widgets/|g' SARibbonAmalgamTemplatePublicHeaders.h SARibbonAmalgamTemplate.cpp
   ```

   原理：sed 后模板引用的 `../../src/widgets/SARibbonQt5Compat.hpp`、`SAColorWidgetsGlobal.h` 等都是**转发/兼容头**，它们经 `#include <SARibbonCore/...>` 转发链自动把 core 实体头拉进单文件（SAColorWidgetsGlobal.h → `../SARibbonWidgetsGlobal.h` → `<SARibbonCore/SARibbonCoreGlobal.h>`），无需再在模板顶部手工插入。
   **编码注（round3 新增）**：`PublicHeaders.h` 与 `SARibbonAmalgamTemplate.h` 为 **UTF-8 带 BOM + CRLF**、`Template.cpp` 为 UTF-8 无 BOM + CRLF（`file` 实测）——上面的 sed 只替换路径字节、不触 BOM/行尾，安全；但**不得用编辑器整文件重写模板**。改完 `git diff --stat tools/amalgamate/` 应只见 89 行以内的路径替换；若出现整文件级 diff 即被转码，回滚重来（R1）。
3. **新增命名空间镜像步骤（关键，原稿与本节早稿都遗漏）**：`<SARibbonCore/SARibbonCoreGlobal.h>` 这种形态在**源码树里物理不存在**（目录名是 `src/core`，`SARibbonCore/` 布局只存在于 build 树同步目录与安装树）。Amalgamate 对找不到的 include 会**原样保留该行**，产物将带着无法解析的 `#include <SARibbonCore/...>` → StaticExample 编译失败。因此脚本在调用 Amalgamate.exe 前构建镜像、结束后清理：

   ```bash
   # Amalgamate.sh 内、运行 Amalgamate.exe 之前：
   rm -rf _amalg_include && mkdir -p _amalg_include/SARibbonCore
   cp ../src/core/SARibbonCoreGlobal.h ../src/core/SARibbonQt5Compat.hpp _amalg_include/SARibbonCore/
   # ... 两次 ./Amalgamate.exe 调用 ...
   rm -rf _amalg_include   # 不入库，无需改 .gitignore
   ```

   配合第 1 条的 `-i "_amalg_include"`，`<SARibbonCore/xxx>` 即解析为 `_amalg_include/SARibbonCore/xxx`。注：镜像副本与实体是不同解析路径，Amalgamate 的"每文件只内联一次"按路径去重，极端情况下同一内容会以不同路径各内联一次——头文件的 include guard（如 `SARIBBONCOREGLOBAL_H`）保证编译安全，仅产物略冗余；模板不直接引 `../../src/core/` 实体路径即可避免（第 2 条已保证）。core 的占位 `SARibbonCoreGlobal.cpp`（S6.2）不需要进模板 cpp（其内容仅一行 include）。若脚本中途失败残留 `tools/_amalg_include/`，手动删除即可（未跟踪目录，不影响 git grep 类门禁；round3 注）。
4. 运行脚本重新生成 `src/SARibbon.h/.cpp` 并提交：`cd tools && bash Amalgamate.sh < /dev/null`（`< /dev/null` 绕过末尾 `read -n 1` 挂起；必须在 tools/ 下运行，脚本用 `./Amalgamate.exe` 与 `../src` 相对路径）。**产物仍是生成物，禁止手改**（AGENTS.md 规则不变）。
5. 生成后 `git diff --stat src/SARibbon.h src/SARibbon.cpp`：预期只有 include 路径替换与 core 头内容新并入（SARibbonCoreGlobal.h 宏区、Qt5Compat 实体）造成的增量，无逻辑 diff。

**验证**：`StaticExample`（`examples/widgets/StaticExample`，其 `SARIBBON_DIR` 已在 S2 改为 `../../../src`）用重新生成的单文件编译通过并运行。

**提交**：`重构：Amalgamate 适配新目录并重新生成单文件`

### S9 core 纯净性门禁（v2 §6.2）

**操作**：新建 `tools/check_core_purity.py`：
- 扫描 `src/core/` 全部 `*.h/*.hpp/*.cpp`：禁止 `#include` 清单（**round3 与 v2 §3.7/§6.2 逐项对齐**）`QWidget QLayout QStyle QStyledItemDelegate QQuickItem QQml QApplication QAction QQuickPaintedItem` 以及**模块化 include 路径形式** `<QtWidgets/`、`<QtQuick/`，另加一条**家族正则** `^Q[A-Za-z]*Layout$`（v2 §3.7 写明"QLayout 及 Q*Layout 族"——前缀 `QLayout` 拦不住 QGridLayout/QVBoxLayout/QBoxLayout/QFormLayout/QStackedLayout，须正则补；原稿用 `QQmlEngine` 单名，改为 `QQml` 前缀族以拦下 QQmlEngine/QQmlContext 等，与 v2 §6.2 命令行一致。02 S8-3 为执行细则）。按 `#include <X...>` / `#include "X..."` 两种形式匹配，清单项以**前缀**命中（`QQuickItem` 同时拦下其派生头名；`QAction` 在 Qt6 属 QtGui 但 **Qt5 属 QtWidgets**，core 一律禁止——v2 §3.7/D3）；`<QtWidgets/` 形式防"模块化路径绕过裸类名扫描"。**明确合法不得误伤**：`QGuiApplication`/`QScreen`/`QFontMetrics`/`QColor`/`QIcon` 属 QtGui（v2 §3.7 第一层）；round3 已验证前缀清单对其零误伤（`QGuiApplication` 不以 `QApplication` 为前缀等）。
- 禁止出现 `qApp`、`QApplication::`、`QWidget`、`QLayout` 等类型名使用（词法级 `\b` 词边界匹配即可，误报宁多勿漏）。
- 违规即打印 `文件:行号` 与违规内容，exit 1；`--forbid-include` 参数化（默认值 = 上述清单）。
- python3 标准库（`re/os/sys/argparse/pathlib`），无第三方依赖；shebang `#!/usr/bin/env python3`。
- **脚本 shebang 与本地/文档调用名统一 `python3`**（Linux/mac CI runner 无 `python` 命令；Windows 本地 `python`/`py -3` 等价）。**例外（round3 修正）：S10.1 的两个 windows workflow 里 purity step 必须写 `python`**——GitHub windows runner 无 `python3` 命令（python.org 安装不生成 python3.exe，会落到 Microsoft Store 占位 stub 返回 9009），详见 S10 第 1 条。

**验证**：`python3 tools/check_core_purity.py src/core` 退出码 0。**"必绿"已预先核实**：core 骨架三文件中，`SARibbonQt5Compat.hpp` 仅 include QtCore/QtGui 头（QMouseEvent/QKeyEvent/QWheelEvent/QFontMetrics 等，`grep -n "QWidget\|qApp\|QAction" 零命中`），`SARibbonCoreGlobal.h` 仅 memory/QtGlobal/QObject。

**提交**：`工具：新增 core 纯净性扫描脚本 check_core_purity.py`

### S10 CI 更新

**现状核实**（6 个 `cmake-{linux,mac,win}-qt{5.15,6.8}.yml` 结构高度一致）：
- configure 行全部为（如 `cmake-linux-qt6.8.yml:37`、`cmake-win-qt6.8.yml:35`）：
  `cmake -DCMAKE_BUILD_TYPE=... -DBUILD_SHARED_LIBS=${{matrix.shared}} -DSARIBBON_BUILD_EXAMPLES=OFF -DBUILD_TESTS=ON -B ...`
  ——实际需要改名的只有 `-DBUILD_TESTS=ON`；`BUILD_SHARED_LIBS` 当前**不被根 CMakeLists 消费**（库类型由 `SARIBBON_BUILD_STATIC_LIBS` 决定），属历史无效传参，保留无害，记 NOTES。
- **6 个 workflow 全部已有 matrix**（linux：`ubuntu_version/qt_version/shared`；win：`win_version/...`；mac：`macos_version/...`）。原稿"若无 matrix 则复制新文件"的备选路径不需要。
- **没有任何 workflow checkout submodule**（`actions/checkout@v4` 均未带 `submodules:` 参数），CI 从不构建 frameless——S3 的路径变化对 CI 零影响。
- `publish-python-bindings.yml` 实际触发器是 `release: types:[published]` + `workflow_dispatch`（文件头 :3-6），**不存在 push/tag 触发**；且 GitHub Actions 的 release 触发执行的是**默认分支（master）**上的 workflow 版本，在 dev-3.0 分支改此文件不影响 master 的发布行为。
- `CMakePresets.json` 现状：`"version": 6` + `cmakeMinimumRequired 3.25`（:2-7），5 个 configure preset 均继承 vcpkg-base（toolchain/triplet/installed-dir），含 static 与 frameless 变体；`vcpkg.json` 含 `frameless` feature（依赖 qwindowkit）。

**操作**：
1. 6 个 `cmake-*.yml`：`-DBUILD_TESTS=ON` → `-DSARIBBON_BUILD_TESTS=ON`（盘点命令 `git grep -n "BUILD_TESTS" .github/`，应恰好 6 处，round3 核实行号：linux-qt5.15:36、linux-qt6.8:37、mac-qt5.15:35、mac-qt6.8:37、win-qt5.15:33、win-qt6.8:35）；每个 workflow 在 Configure 步骤**之前**加：

   ```yaml
   # linux / mac 四个 workflow：
   - name: Core purity check
     run: python3 tools/check_core_purity.py src/core
   # windows 两个 workflow（cmake-win-qt5.15.yml / cmake-win-qt6.8.yml）必须改用 python：
   - name: Core purity check
     run: python tools/check_core_purity.py src/core
   ```

   **round3 修正（原稿统一 `python3` 在 windows runner 必失败）**：GitHub windows runner 无 `python3` 命令——python.org 发行版不生成 python3.exe，PATH 落到 Microsoft Store 占位 stub 返回 9009；linux/mac runner 则相反（无 `python` 只有 `python3`）。step 失败即 job 失败，merge blocking（v2 §6.2）。
2. `cmake-linux-qt6.8.yml` 的 matrix 增加一轴：`widgets: [ON, OFF]`，configure 行追加 `-DSARIBBON_BUILD_WIDGETS=${{ matrix.widgets }}`，job name 加 `-widgets-${{ matrix.widgets }}` 后缀（core-only 组合验证，v2 §6.2"组合构建"；该 workflow 现有 `-DSARIBBON_BUILD_EXAMPLES=OFF -DBUILD_TESTS→ON` 需同步：core-only 时测试无法链接 widgets，故把 tests 也纳入矩阵联动——最简做法：`-DSARIBBON_BUILD_TESTS=${{ matrix.widgets }}`，widgets=OFF 时 core-only 编译验证、widgets=ON 时全测试）。**且该 workflow 的 Test 步骤必须加条件 `if: matrix.widgets == 'ON'`（round3 修正，阻塞级组合缺陷）**——否则与第 7 条的 `--no-tests=error` 冲突：widgets=OFF 组合注册 0 个测试，ctest 直接非零退出，core-only 矩阵项必红。purity step 不受影响（两种组合都要跑）。
3. frameless：无 CI job、无 submodule checkout（现状核实如上），无需改 workflow；本地以 vcpkg preset 手动验证一次 `cmake --preset=vcpkg-msvc-x64-release` 配置通过即可（preset 现成，`CMakePresets.json` 的 `vcpkg-msvc-x64-release`）。
4. **publish-python-bindings.yml：不改**（原稿"删除 push/tag 触发、改 dispatch-only"与事实不符——本就没有 push/tag 触发，且 dev-3.0 分支上的修改对 master 的 release 触发无效）。sip/pyqt6/pyside6 的 CI 恢复归计划 03；本计划在 NOTES 记录此核实结论即可。
5. `CMakePresets.json`：保留全部 vcpkg presets 与 `"version": 6`（不降版本字段）；新增 `win-msvc-qt6-debug/release`（generator "Visual Studio 17 2022"，`CMAKE_PREFIX_PATH` 留 `${sourceDir}` 相对占位或文档注明需本地覆盖）与 `linux-ninja-release` 常规 presets，binaryDir 统一 `build-${presetName}`；对应 buildPresets/testPresets 补齐。
6. `page.yml`（mkdocs 文档发布）与构建无关，不动。
7. **ctest 加 `--no-tests=error`**（round2 从 QWK 上游 CI 抄入，6 个 workflow 的 Test 步骤统一改）：现 ctest 行为 `ctest --output-on-failure -C ${{env.BUILD_TYPE}}`（如 `cmake-linux-qt6.8.yml:52-55`），**0 个测试时照样退出 0**——本步第 1 条把 `-DBUILD_TESTS` 改名 `-DSARIBBON_BUILD_TESTS` 后，若哪个 workflow 漏改导致测试静默不注册，CI 会假绿。改为 `ctest --output-on-failure --no-tests=error -C ...` 即可把"零测试"变成硬失败。证据：QWK 上游 main 分支 `.github/workflows/ci.yml` 的 Test 步骤用 `ctest --test-dir build --output-on-failure --no-tests=error`，并配注释 "ctest exits 0 when it finds nothing to run"（**溯源说明**：本地 QWK 快照 1fb3ec7 不含 `.github/`，该证据经 GitHub API 从上游 main 抓取，属比快照新的版本，见 reviews/round2/qwk-build-findings.md §一.6）。QWK 上游的其余 CI 做法（matrix include 组织、多编译器轴、MinGW/msys2 独立 job、把安装包消费测试注册进 ctest）记入 findings §三设计级建议，本步不抄。已核实 SARibbon 6 个 workflow 均已有 `fail-fast: false` 与 `jurplel/install-qt-action@v4`（`cmake-linux-qt6.8.yml:17-37`），无需重复添加；cache 取值现状为 linux×2 / mac-qt6.8 / win-qt5.15 = `'true'`、**mac-qt5.15 / win-qt6.8 = `'false'`**（round3 逐个实测，更正原稿"6 个均为 cache:'true'"的失实表述），本计划不统一、不改动（与构建正确性无关）。

**验证**：push 后 6 个 workflow 全绿（含新增 core-only 矩阵项与 purity step）；`cmake --preset=vcpkg-msvc-x64-release` 本机配置通过（需 vcpkg 环境；无环境则记 NOTES 降级为"仅语法核查 preset JSON"）。

**提交**：`CI：适配三模块选项、新增 core-only 矩阵与纯净性扫描`

### S11 安装与包配置验收

**现状盘点**（全部在 `src/SARibbonBar/CMakeLists.txt`，行号供比对）：
- 头文件 → `include/SARibbonBar/`，colorWidgets 头 → `include/SARibbonBar/colorWidgets/`（L292-302）；
- amalgamate 产物 → `share/SARibbonBar_amalgamate`（L306-313）；
- qm 翻译 → `bin/translations`（L273-276）；
- 包配置：`SARibbonBarConfig.cmake(.in)`（.in 在 `src/SARibbonBar/`，find_dependency Qt + Targets include + `set_and_check INCLUDE_DIR`）、`SARibbonBarConfigVersion.cmake`（SameMajorVersion）、`SARibbonBarTargets.cmake`（namespace `SARibbonBar::`）→ `lib/cmake/SARibbonBar`（L316-352）。
- `SARibbonConfig.cmake.in` **不存在**（新组件化包是本步新增物；`tools/test-find-package/` 亦不存在，新建）。

**操作**（若 S5 已实现 install 则本步只做验证与补件）。**时序说明（round3 新增）**：S6 已删除旧 `SARibbonBarConfig` 生成而本步才建新包，故 S6~S10 各提交点的安装树暂无任何可用 Config（`find_package(SARibbon)`/`find_package(SARibbonBar)` 均失败）——这是计划内中间态，不影响 R3 验证（build.ps1 的 build+ctest 走构建树，不依赖安装树），S11 完成后恢复并升级；期间不做"安装态外部消费"类验证。
1. 新组件化包：新建 `cmake/SARibbonConfig.cmake.in`（骨架）：

   ```cmake
   @PACKAGE_INIT@
   include(CMakeFindDependencyMacro)
   # REQUIRED 必带（QWK QWindowKitConfig.cmake.in:5-6 同款）：否则 Qt 缺失时
   # find_dependency 静默返回，错误延迟到 Targets 引用 Qt6::Core 时才爆出，难排查
   find_dependency(Qt@QT_VERSION_MAJOR@ COMPONENTS Core Gui REQUIRED)
   if(@SARIBBON_BUILD_WIDGETS@)
       find_dependency(Qt@QT_VERSION_MAJOR@ COMPONENTS Widgets Svg REQUIRED)
   endif()
   if(@SARIBBON_BUILD_QML@)
       # round2 整合（kddw-qtquick-findings.md §三.2）：命令式单轨下 Qml 组件
       # 只需 Quick/Qml——无任何 qmldir/qmltypes 安装物；QuickControls2 是否
       # 追加由计划 04 S5 的叶子实现决策定（候选清单 B-7）
       find_dependency(Qt@QT_VERSION_MAJOR@ COMPONENTS Qml Quick REQUIRED)
   endif()
   include("${CMAKE_CURRENT_LIST_DIR}/SARibbonTargets.cmake")
   # 组件语义：COMPONENTS Core/Widgets/Qml 映射到 target 存在性
   # （round3 补 Core：core-only 安装（SARIBBON_BUILD_WIDGETS=OFF）下
   #   find_package(SARibbon COMPONENTS Core) 必须可用——v2 §6.2 组合构建的消费端形态）
   set(_saribbon_supported_components Core Widgets Qml)
   foreach(_comp ${SARibbon_FIND_COMPONENTS})
       if(NOT _comp IN_LIST _saribbon_supported_components)
           set(SARibbon_FOUND FALSE)
           set(SARibbon_NOT_FOUND_MESSAGE "Unsupported component: ${_comp}")
       elseif(NOT TARGET SARibbon::${_comp})
           set(SARibbon_${_comp}_FOUND FALSE)
       else()
           set(SARibbon_${_comp}_FOUND TRUE)
       endif()
   endforeach()
   check_required_components(SARibbon)
   ```

   在 `src/CMakeLists.txt` 用 `configure_package_config_file` + `write_basic_package_version_file`（VERSION ${SARIBBON_VERSION}，COMPATIBILITY SameMajorVersion）生成并安装到 `lib/cmake/SARibbon/`；`install(EXPORT SARibbonTargets NAMESPACE SARibbon:: ...)` 已在 S6.7。**（round2 整合）本条全部生成/安装规则与 S11.2 兼容包、S11.3 转发头的 install 规则均在 `SARIBBON_INSTALL` 守卫内（v2 §6.4/NOTES B10；QWK 同构——Config/Targets 生成安装在 QWINDOWKIT_INSTALL 内，src/CMakeLists.txt:181-215）。**

   **QWK 对照附注（round2 实证，实现时必读）**：
   - **QWK 1.0.1 没有组件化 Config 可抄**：其 `QWindowKitConfig.cmake.in` 全文仅 8 行（find_dependency + include Targets，无 COMPONENTS 逻辑），且 `configure_package_config_file` 传了 **`NO_CHECK_REQUIRED_COMPONENTS_MACRO`**（`src/CMakeLists.txt:193-198`），三模块共用单一 export set `QWindowKitTargets`（:208-212）。SARibbon 上面的组件逻辑是**自主设计**（比 QWK 更进一步）；实现时 `configure_package_config_file` **不得**带 `NO_CHECK_REQUIRED_COMPONENTS_MACRO`，否则骨架里的 `check_required_components(SARibbon)` 未定义、消费端直接报错——照抄 QWK 的调用方式会踩这个坑。
   - 单 export set 恰好支撑组件语义：`SARibbonTargets.cmake` 自动携带 `SARibbon::Widgets → SARibbon::Core` 的 INTERFACE 依赖（PUBLIC 链接被导出，证据见 S6.7 附注），组件间依赖传递无需 Config 手写。
   - 版本兼容策略差异：QWK 用 `AnyNewerVersion`（`src/CMakeLists.txt:186-190`）；SARibbon 维持 `SameMajorVersion`（与 2.9.5 现状一致，`src/SARibbonBar/CMakeLists.txt:325-329`，且更严格——拒绝跨大版本静默消费）。不抄 QWK。
   - **不抄 QWK 的两段式 find_dependency**（`QWindowKitConfig.cmake.in:5-6`：`find_dependency(QT NAMES Qt6 Qt5 ...)` + `find_dependency(Qt${QT_VERSION_MAJOR} ...)`，把 Qt 大版本裁决权交给消费端环境）：SARibbon 的 Targets 文件在生产端已固定 `Qt@QT_VERSION_MAJOR@::Core`，若消费端先探测到另一 Qt 大版本会链接混版；baked-in 写法与导出文件确定一致，且与现状 `SARibbonBarConfig.cmake.in:12` 同款。维持骨架写法。
   - 旧包名 `SARibbonBarConfig` 兼容薄壳（第 2 条）：QWK 无旧包名先例，不构成参照也不构成反证；薄壳的 INTERFACE IMPORTED 转发方案自洽，维持。
2. **旧 `SARibbonBarConfig` 兼容包保留一个版本周期**（10 个 example 独立构建的 `find_package(SARibbonBar REQUIRED)` fallback 依赖它）：旧 `SARibbonBarConfig.cmake.in`（随 S6 迁至 `src/widgets/`）改写为薄壳。**全文（round3 补具体内容，消除现场发明）**：

   ```cmake
   @PACKAGE_INIT@
   # 3.0 compatibility shell: forwards the legacy SARibbonBar package name to the
   # component-based SARibbon package. Remove after one release cycle (plan 03).
   include(CMakeFindDependencyMacro)
   find_dependency(Qt@QT_VERSION_MAJOR@ COMPONENTS Core Gui Widgets Svg REQUIRED)
   include("${CMAKE_CURRENT_LIST_DIR}/../SARibbon/SARibbonTargets.cmake")
   if(NOT TARGET SARibbonBar::SARibbonBar)
       add_library(SARibbonBar::SARibbonBar INTERFACE IMPORTED)
       target_link_libraries(SARibbonBar::SARibbonBar INTERFACE SARibbon::Widgets)
   endif()
   # 2.x 变量口径保留（外部旧脚本可能消费；INCLUDE_DIR 指转发头目录，S11.3 生成）
   set(SARibbonBar_INCLUDE_DIR "${PACKAGE_PREFIX_DIR}/@CMAKE_INSTALL_INCLUDEDIR@/SARibbonBar")
   set(SARibbonBar_LIBRARIES SARibbonBar::SARibbonBar)
   ```

   要点：① `find_dependency(Qt...)` 必须保留——SARibbonTargets.cmake 的 INTERFACE 引用 `Qt@QT_VERSION_MAJOR@::Core` 等，消费端未找 Qt 时会在 target 解析期报"target not found"，薄壳先行 find_dependency 与 2.9.5 旧 Config（.in:12）行为一致；② 相对路径 `../SARibbon/SARibbonTargets.cmake` 依据两包同在 `lib/cmake/` 下成立（S6.7 与本条的 DESTINATION）；③ **ConfigVersion 必须同装**：`write_basic_package_version_file(SARibbonBarConfigVersion.cmake VERSION ${SARIBBON_VERSION} COMPATIBILITY SameMajorVersion)`——2.9.5 装有它，缺失会让 `find_package(SARibbonBar 2.9)` 形态的旧脚本行为变化。生成与安装（`configure_package_config_file(... INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SARibbonBar)` + `install(FILES ... DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SARibbonBar)`）写在 `src/CMakeLists.txt`，整体在 `if(SARIBBON_INSTALL)` 守卫内（v2 §6.4）。
3. 转发头目录 `include/SARibbonBar/`（旧路径兼容，v2 §4.4；原 v1 §7.1 引用悬空，规则即"对每个公共头生成同名转发文件"）：在 `src/widgets/CMakeLists.txt` 末尾按清单生成（不手维护）：

   ```cmake
   # 3.0 兼容转发头：include/SARibbonBar/<name>.h -> #include <SARibbonWidgets/<name>.h>
   set(_compat_dir "${CMAKE_BINARY_DIR}/compat-include/SARibbonBar")
   file(MAKE_DIRECTORY "${_compat_dir}")
   foreach(_h IN LISTS SARIBBON_HEADER_FILES)
       get_filename_component(_name "${_h}" NAME)
       file(WRITE "${_compat_dir}/${_name}"
           "#pragma once\n#include <SARibbonWidgets/${_name}>\n")
   endforeach()
   if(SARIBBON_INSTALL)   # v2 §6.4 (round2 D1)
       install(DIRECTORY "${_compat_dir}/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/SARibbonBar")
   endif()
   ```

   （colorWidgets 头如也需要旧路径 `include/SARibbonBar/colorWidgets/`，同法对 `SACOLOR_HEADER_FILES` 生成到 `${_compat_dir}/colorWidgets/`，转发目标 `<SARibbonWidgets/colorWidgets/<name>>`。）
4. 外部消费冒烟：`tools/test-find-package/` 下建最小工程——`main.cpp` 分别以 `#include <SARibbonBar/SARibbonBar.h>`（旧路径转发头）与 `#include <SARibbonWidgets/SARibbonBar.h>`（新路径）各编一个 TU（**两个 TU 不可合并成一个**：两种头经各自 include guard 后同文件只生效一份，合并等于没测）；CMakeLists：`find_package(SARibbon 3.0 REQUIRED COMPONENTS Widgets)` + `target_link_libraries(app PRIVATE SARibbon::Widgets)`；命令（**round3 修正：CMAKE_PREFIX_PATH 必须同时含 Qt 路径**——Config 的 `find_dependency(Qt6 ...)` 在消费端要能找到 Qt；Windows 本机 install 目录即 `bin_qt6.7.3_MSVC_x64/`，分号列表整体加引号）：

   ```bash
   cmake -S tools/test-find-package -B build-fp -DCMAKE_PREFIX_PATH="<install目录>;<Qt6路径>"
   cmake --build build-fp --config Release
   ```

   编译通过即验收。该工程不挂进主构建（独立冒烟用；`build-fp/` 不入库）。
5. `cmake --install build --config Release` 后检查安装树：`include/SARibbonCore/`、`include/SARibbonWidgets/`（含 `colorWidgets/`）、`include/SARibbonBar/`（转发头）、`lib/cmake/SARibbon/{SARibbonConfig,SARibbonConfigVersion,SARibbonTargets}.cmake`、`lib/cmake/SARibbonBar/`（兼容壳）、`share/SARibbonBar_amalgamate/`、`bin/translations/*.qm`、库/DLL 本体（Windows + `SARIBBON_INSTALL_IN_CURRENT_DIR=ON` 时整体落在 `bin_qtX_编译器_x架构/`，行为同 2.9.5）。（round2 整合注：① 本条以 `SARIBBON_INSTALL=ON`（默认）执行，OFF 组合应无任何安装产物——v2 §6.4；② 清单**无 qml 项即为终态正确状态**——命令式单轨下 QML 叶子进 qrc 随库二进制，安装期零新增产物，04 S1/03 S6-3 同结论。）

**提交**：`重构：安装转发头与 find_package 组件化验收`

### S12 文档同步（AGENTS.md / build.md / submodule.md / changlog.md）

**操作**：
1. [AGENTS.md](../../AGENTS.md)：项目结构图改为 3.0 布局（`src/SARibbonBar/` → `src/widgets/`，新增 `src/core`、`src/qml`、顶层 `3rdparty/`、`plans/3.0/`）；"禁止触碰合并文件"段落（L5）改为指向 `src/SARibbon.h/.cpp` 由 `tools/Amalgamate.sh` 从 `src/widgets + src/core` 生成；核心头文件段（L36，`src/SARibbonBar/SARibbonGlobal.h`）改为"宏基座在 `src/core/SARibbonCoreGlobal.h`，`src/widgets/SARibbonGlobal.h` 为兼容转发头"；补充"3.0 开发在 dev-3.0 分支"。实测 AGENTS.md 共 6 处 `src/SARibbonBar` 引用需改（L5/L15/L17/L18/L19/L36）。
2. [build.md](../../build.md)：选项表（L120-127）更新为新选项名（`BUILD_TESTS`→`SARIBBON_BUILD_TESTS` 在 L127，新增 `SARIBBON_BUILD_WIDGETS/QML` 与 `SARIBBON_INSTALL`）、Qt 门槛 5.15（L11 frameless 版本注一并核对）、CMake 门槛 **3.21**。
3. `submodule.md`：2 处旧 submodule 路径（L20/L29）改 `3rdparty/qwindowkit`；其 url 写 `stdware/qwindowkit` 与 `.gitmodules` 实际 `czyt1988/qwindowkit`（fork）不符，一并修正。
4. `changlog.md`（**拼写确认为 changlog，非 changelog，文件名不改**）顶部加"3.0 开发分支启动（目录重组、三模块构建）"条目。
5. `docs/zh|en/` 下 8 个文件引用 `src/SARibbonBar/i18n|resource` 旧路径——**本计划不动 docs/**（批量文档更新归计划 04），在 NOTES 登记清单。**round3 补充口径**：docs/ 下含任意 `src/SARibbonBar` 引用的文件实为 **22 个**（`git grep -l "src/SARibbonBar" docs` 实测），NOTES 登记时以该全量清单为准（8 个 i18n/resource 文件是其子集），计划 04 批量处理。

**提交**：`文档：AGENTS.md/build.md/submodule.md 适配 3.0 目录与构建`

## 6. 完成验收门（全部满足才算完成本计划）

- [ ] `git log --follow src/widgets/SARibbonBar.cpp` 可追溯到 2.9.5 历史（注：`src/widgets/SARibbonBar.cpp` 是普通类实现源文件，**不是** amalgamate 产物——产物是 `src/SARibbon.cpp`，二者勿混淆；`--follow` 对单文件 rename 追踪有效）。另抽查 `git log --follow src/widgets/SARibbonPanelLayout.cpp` 与 `git log --follow tests/widgets/ThemeCoverageTest.cpp`
- [ ] `pwsh -NoProfile -File scripts/build.ps1 rebuild -Tests ON -Examples ON` 全绿，`ctest --test-dir build -C Release` 通过数 == N₀（P3 记录值，预期 26）
- [ ] core-only：`cmake -S . -B build-3.0-coreonly -DCMAKE_PREFIX_PATH=<Qt6路径> -DSARIBBON_BUILD_WIDGETS=OFF -DSARIBBON_BUILD_EXAMPLES=OFF -DSARIBBON_BUILD_TESTS=OFF` 配置 + `cmake --build build-3.0-coreonly --config Release` 编译通过
- [ ] `python3 tools/check_core_purity.py src/core` 退出码 0（Windows 本地若 `python3` 不存在则用 `python`，见 S9 调用名说明）
- [ ] CI 6 workflow 绿（含 purity step 与 linux-qt6.8 的 widgets=OFF 矩阵项）
- [ ] `MainWindowExample` 运行并与 2.9.5 基线截图对比视觉一致（三行/两行/最小模式、明暗主题各一张）
- [ ] `StaticExample` 用重新生成的 `src/SARibbon.h/.cpp` 编译运行通过
- [ ] `tools/test-find-package/` 外部消费工程编译通过（`<SARibbonBar/...>` 转发与 `<SARibbonWidgets/...>` 新路径两种 include）
- [ ] `git submodule status` 显示 `f93657f`、路径 `3rdparty/qwindowkit`
- [ ] AGENTS.md / build.md / submodule.md 已更新，且残留引用门禁（**round3 修正版**——原版对 `src/SARibbon.cpp` 的"归零"要求不可达成，且漏排 v2 计划文件）：

  ```bash
  # 门禁 A：全仓残留引用（期望零输出）
  git grep -n "src/SARibbonBar" -- ':!plans' ':!docs' ':!changlog.md' \
      ':!pyproject*.toml' ':!pyqt6' ':!pyside6' ':!sip' ':!MANIFEST.in' \
      ':!tools/qrc_SARibbonResource*' ':!src/SARibbon.cpp' ':!SARibbon-3.0-plan-v2.md'
  # 门禁 B：src/SARibbon.cpp 定向检查——命中只允许是内联 rcc 注释（期望零输出）
  grep -n "src/SARibbonBar" src/SARibbon.cpp | grep -v "// C:/src/Qt/SARibbon/src/SARibbonBar/"
  ```

  排除项理由：`pyproject*.toml`(3 份、各 86 处)/`pyqt6`/`pyside6`/`sip`/`MANIFEST.in` 是 Python 打包路径，归计划 03；`tools/qrc_*.cpp` 的 32 处命中（全部在 `qrc_SARibbonResource_Datas.cpp`，version2/version3 为 0）全在 rcc 生成注释里（S8 已核实无功能影响）；`docs`/`changlog.md` 归计划 04/历史记录；`SARibbon-3.0-plan-v2.md` 的 1 处（:9）是对 2.9.5 submodule 旧路径的历史叙述（round3 新增排除）。**`src/SARibbon.cpp` 的 32 处命中同样全部来自内联的 `qrc_SARibbonResource_Datas.cpp` rcc 注释（位于 "Start/End of inlined file" 标记之间）——S8 明确不再生 qrc 文件，故重生成产物后这 32 处依然存在，原稿"S8 后归零"必失败（round3 实测确认）；改为门禁 B：剔除 rcc 注释行后必须零命中**。**不在排除名单**、必须在对应步骤后归零的：`tools/amalgamate/*`（89 处 = Template.cpp 43 + PublicHeaders.h 46，S8 sed 负责）、`.gitmodules`（2 处，S3）、`AGENTS.md`（6 处，S12）、`submodule.md`（2 处，S12）、根 `CMakeLists.txt`（2 处，S6.1 configure_file 改路径）、`tests/CMakeLists.txt` + `tests/ThemeCoverageTest.cpp`（各 1 处，S7）、`tools/Amalgamate.sh`（1 处，S8.1）——任一仍有命中即对应步骤未完成。

## 7. 风险与回滚

| 风险 | 缓解 |
|------|------|
| `git mv` submodule 失败 | S3 步骤 3 备选手动流程；submodule 未初始化，风险低 |
| 同步头目录导致 moc/翻译路径错乱 | 内部相对 include 保持不动（S5.5 include 传播决策：源目录维持 PUBLIC，190 处平铺 include 零改动）；每步构建验证 |
| S4~S6 过渡窗口 C++ 标准缺失（Qt6 编译失败） | S4 第 5 条保留全局 `CMAKE_CXX_STANDARD 17` 两行过渡，S6 末尾才删除 |
| CI 矩阵大面积红 | 每步一提交，`git revert` 精确定位；CI 是最后一步（S10），前面已本机全绿 |
| 5.15 门槛导致旧 Qt5 用户 CI 报警 | 仅影响 3.0 分支；迁移指南（计划 04）说明；CI 的 qt5.15 workflow 覆盖下限验证 |
| build.ps1/CI 旧选项名 `BUILD_TESTS` 耦合 | S4 兼容 shim 兜底 + S4.11/S10.1 同步改名双保险 |
| 版本号 3.0.0 外溢（VersionInfo/打包元数据不一致） | S4 只改 CMake 工程版本并派生旧变量；vcpkg.json/pyproject/changlog 归计划 03/04，NOTES 跟踪 |
| publish-python-bindings 误触发 | 已核实其触发器为 release+dispatch 且 release 触发只认默认分支，dev-3.0 上无误触发路径（S10.4），无需改动 |

## 8. 已知偏差（执行前成立）

见 [NOTES.md](NOTES.md)：P7（Pannel 拼写）已在 2.9.x 完成与本计划无交集；旧计划目录图中的 `SARibbonPannel*` 文件名均为 `SARibbonPanel*`。本轮评审（reviews/round1）新增确认：

- **v1 计划文件从未存在**（仓库与 git 历史均无 `SARibbon-3.0-plan.md`），原稿全部 v1 引用已内联或标注；
- 当前检出分支为 `v3`；round1 时点 `v3`/`dev` 同指 `7a617fc`，**round3 时点 `v3` 已前进到评审提交链顶端（`42b7dcc`、`eba8ebd` 及后续），`dev` 仍指 `7a617fc`**——P1/P2/S1 已按"执行时点"重写为祖先检查 + 非文档 diff 为空的双门禁；
- 未跟踪项 round1 时为 3 个（v2 计划、plans/、bundle）；round3 时点前两项已随评审提交入库，**仅剩 bundle**，P1 已再次修订；
- tests 实为 25 个顶层 .cpp + `tests/auto/` 1 个 = 26 个注册测试（round1 时点 README 快照表曾写"约 24 个"、当时不在本计划修订范围故差异登记于此；**终审确认：README 快照与 NOTES B8 已在 round1 同步改为 26，本括号仅为历史记录**）；
- `SAColorWidgets` 导出宏实名为 `SA_COLOR_WIDGETS_API`（原稿误写 `SA_COLOR_WIDGETS_EXPORT`）。

第 3 轮 dry-run 评审（reviews/round3/01-dryrun-findings.md）新增确认（均已在正文修复）：

- **S6.9 代码块 C 的兼容映射与三段式顺序颠倒**为阻塞级错误（`#ifdef` 指令即时求值，原稿"惰性展开安全"论证不成立，旧宏场景产物编译必失败）——已重排并改写论证；
- **§6 残留引用门禁原版必失败**（`src/SARibbon.cpp` 的 32 处命中来自内联 rcc 注释、重生成后仍在；v2 计划文件 1 处历史叙述未排除）——已改为门禁 A+B；
- **S10.2 × S10.7 组合冲突**（core-only 矩阵项 0 测试 × `--no-tests=error` 必红）——Test 步骤加 `if: matrix.widgets == 'ON'`；
- **S10.1 windows runner 无 `python3`**——win 两个 workflow 改用 `python`；
- `SARibbonQt5Compat.hpp` 的 widgets 内引用实为 **5 个 .cpp**（原稿"38 个文件"系误抄）；`SARibbonGlobal.h` 的验证命令带 pathspec 才是 38（不带为 39，含 CMakeLists）；
- **amalgamate 产物 `share/SARibbonBar_amalgamate` 安装规则**在 S6.1 删除旧块后无人承担——已补进 S6.7 的 src/CMakeLists.txt；
- S6.1 保留块的 `${SARIBBON_LIB_NAME}` 必须替换为 `SARibbonWidgets`（原骨架未说明，保留旧 set 会让 target_sources 作用于 ALIAS 而报错）——已补两条机械替换规则；
- 重写既有文件（SARibbonGlobal.h=**UTF-8 BOM+CRLF**、SAColorWidgetsGlobal.h/src/CMakeLists.txt 等）的编码保持要求已在对应步骤显式标注（R1）。
