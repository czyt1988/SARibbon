# 计划 03：构建生态收尾（对应 v2 里程碑 M2）

> 前置计划：[02-core-sinking.md](02-core-sinking.md) 验收门全部通过
> 后续计划：[04-qml-and-release.md](04-qml-and-release.md)（M3 依赖 M1，可与本计划并行；对 agent 建议仍线性执行，避免目录/CI 同时变动互相干扰）
> 设计依据：v2 计划 §6（构建体系）、§8-M2；原 v1 计划相关决策已内联为下方【内联-1..5】（v1 文档不在仓库，见说明）
> 分支：`dev-3.0`（延续，由计划 01 S1 创建）

> **v1 引用内联说明**：本文原引用的"v1 计划"（§5、§5.4、§7.1–§7.4）在仓库与 git 历史中均不存在，
> 仅 v2 计划（[SARibbon-3.0-plan-v2.md](../../SARibbon-3.0-plan-v2.md)）§6.1 有"维持 v1 设计，不赘述"的转述。
> 相关决策内联如下，正文不再出现 v1 章节号：
>
> - **【内联-1】amalgamate 产物出库**（原 v1 §7.2）：单文件产物由 CI/本地脚本生成，**不再提交进 git**，消除"生成物入库 + 禁止手改"的长期痛点；发行渠道改为 CI 产物/Release 附件。
> - **【内联-2】Python 包名不变**（原 v1 §7.3）：三套绑定的发行名与导入名在 3.0 维持现状不改名——PyQt5=`PyQtSARibbon`、PyQt6 双轨=`PyQtSARibbon`（根目录轨）/`PyQt6SARibbon`（pyqt6/ 轨）、PySide6=`PySideSARibbon`，扩展模块名均为 `saribbon`。
> - **【内联-3】QML 不参与合并**（原 v1 §7.2/§7.4 范围界定）：单文件发行只有两种形态——`SARibbonCore`（仅 core）与 `SARibbonWidgets`（core+widgets）。
> - **【内联-4】绑定适配范围**（原 v1 §5.4）：sip（PyQt5）/PyQt6/PySide6 三套绑定做路径与清单迁移，保持"轮子可构建 + import 冒烟"，不重写绑定架构。
> - **【内联-5】关联决策的落点**：旧路径转发头 `include/SARibbonBar/`（原 v1 §7.1）由计划 01 S11 落地，本计划只复核；组合构建（`Widgets=OFF`、`Widgets=OFF+Qml=ON`，原 v1 §7.4）分别在计划 01 S10 与计划 04 落地；2.x support 分支策略见 [README.md](README.md) R6。

## 1. 目标

把 3.0 的"周边工程"全部收口，使其达到可发布 3.0.0-alpha 的工程完备度：

1. **amalgamate 按模块改造**：产出 `SARibbonCore.h/.cpp`（仅 Core）与 `SARibbonWidgets.h/.cpp`（Core+Widgets 合并），且产物**不再提交进仓库**（由 CI/脚本生成，【内联-1】）。
2. **Python 绑定适配**【内联-4】：`sip/`（PyQt5）、`pyqt6/sip/`、`pyside6/` 的路径与源清单全部切到三模块形态，轮子可构建，publish workflow 适配到位（触发保持现状 + 新增 dry-run，见 S4）。
3. **CI 全矩阵**：6 平台 workflow + 纯净性矩阵项 + amalgamation job + core 黄金测试前置（计划 02 S9 已加，复核）。
4. **安装细节**：i18n qm 安装、静态库样例、`find_package` 组件消费文档化验证。

## 2. 范围与非目标

**非目标**：QML 模块实现与示例（计划 04）；面向用户的文档重写与迁移指南（计划 04，本计划只交工程侧说明，含 `docs/{zh,en}/python-guide/` 的路径同步）；正式 3.0.0 发布与 tag（计划 04）。

## 3. 前置条件（逐项验证）

| # | 检查 | 期望 |
|---|------|------|
| P1 | 计划 02 验收门全绿 | 复核 [02 验收门](02-core-sinking.md#6-完成验收门-v2-m1-交付判据全部满足) 全勾 |
| P2 | 测试基线 | `ctest` 通过数 == N₀ + `tests/core` 黄金测试全绿 |
| P3 | 单文件现状 | `src/SARibbon.h/.cpp` 存在且由计划 01 S8 的脚本生成（含 core 骨架；01-S8 已把 OPTS 的 `-i` 指向 `../src/widgets`、`../src/widgets/colorWidgets`、`../src/core` **与 `_amalg_include`**，脚本内在调用 exe 前构建 `tools/_amalg_include/SARibbonCore/` 镜像以解析转发头的 `<SARibbonCore/...>` include、结束后清理，并已把模板内 `src/SARibbonBar/` 前缀 sed 为 `src/widgets/`——镜像机制本计划 S1 必须继承，见 S1 现状与操作 2） |
| P4 | publish workflow 现状确认 | `publish-python-bindings.yml` 触发器仍为 `release: types:[published]` + `workflow_dispatch`（**计划 01 S10-4 明确"不改此文件"**：本就无 push/tag 触发，且 dev-3.0 分支上的修改不影响 master 的 release 触发，故 01 执行前后该 yml 保持 2.9.5 基线原状；自唯一提交 6f32beb 起从未有过 tag 触发。旧稿"01-S10-4 完成后为 dispatch-only 暂停态"与 01 正文矛盾，已更正）。本计划 S4 直接在现状上适配 |
| P5 | 纯净门禁 | `python3 tools/check_core_purity.py src/core` 退出码 0（脚本由计划 01 S9 创建；2.9.5 基线不存在。**调用名口径（round3 终审统一，与 01-S9/S10-1 一致）**：文档/本地命令一律写 `python3`，Windows 本机若无 `python3` 命令则用 `python` 或 `py -3`；CI step 中 linux/mac 用 `python3`、windows workflow 用 `python`（GitHub windows runner 无 python3）） |
| P6 | 绑定基线认知 | 三套绑定当前均直接从**源码树分散头/源文件**构建（不是从单文件构建），版本字段全部停在 `2.8.0`，且 sip 系**三处** pyproject（根 `pyproject.toml`、`pyproject-pyqt6.toml`、`pyqt6/pyproject.toml`）的 sources 清单均缺 2.9.x 新增源文件（见 S3.2-2）——迁移时一并修复 |

## 4. 全程纪律

[README.md](README.md) R1–R6 全部适用（R5 术语表、R6 双分支同步与本计划相关：布局引擎 fix 同步规则、dev-3.0 分支纪律）。本计划涉及**删除 git 跟踪的生成物**（`src/SARibbon.h/.cpp`）与**改动 AGENTS.md 禁令**，属预期内的大变更，逐小步提交并同步文档。

## 5. 执行步骤

### S1 amalgamate 按模块改造

**现状（已核实，2.9.5 基线 + 计划 01 S8 之后）**：

- `tools/Amalgamate.sh` 必须**以 `tools/` 为工作目录**运行（内部全是相对路径），核心行：
  ```bash
  DEST=../src
  OPTS='-i "../src/SARibbonBar" -i "../src/SARibbonBar/colorWidgets" -w "*.cpp;*.h;*.hpp" -s'
  ./Amalgamate.exe $OPTS ./amalgamate/SARibbonAmalgamTemplate.h   $DEST/SARibbon.h
  ./Amalgamate.exe $OPTS ./amalgamate/SARibbonAmalgamTemplate.cpp $DEST/SARibbon.cpp
  ```
  计划 01 S8 已做应急适配：`-i` 改为 `../src/widgets`、`../src/widgets/colorWidgets`、`../src/core` **与 `_amalg_include`**；两个模板的 `src/SARibbonBar/` 前缀已 sed 为 `src/widgets/`；脚本在调用 exe 前构建 `tools/_amalg_include/SARibbonCore/` 镜像（复制 core 头）、结束后 `rm -rf` 清理——用于解析 widgets 兼容转发头（`SARibbonGlobal.h`、`SARibbonWidgetsGlobal.h`、`SARibbonQt5Compat.hpp`、`SAColorWidgetsGlobal.h` 等，01-S6.9 代码块 B/C 与 S6-2/S6-4）里的 `<SARibbonCore/...>` 命名空间 include：该 `SARibbonCore/` 布局只存在于主工程 build 树同步目录与安装树，**源码树中物理不存在**，Amalgamate 对解析不到的 include 会原样保留、产物必坏。**本步的双产物改造必须继承并扩展此镜像机制**（02 之后 core 头分散在 `src/core/global|metrics|contract|layout|...` 子目录且对外形态平铺为 `SARibbonCore/<名>.h`，镜像需同时提供两种形态，见操作 2 目标脚本）。
- 脚本用 awk 把产物 LF→CRLF（`convert_to_crlf` 函数，保留）；**末尾有无条件 `read -n 1` 交互等待（"按任意键继续"），CI 调用会挂死，本步必须处理**。
- **编码警示（NOTES B4）**：`tools/Amalgamate.sh` 是全仓唯一 GBK 编码文件（实测 851 字节、GBK 可解码、UTF-8 解码失败、LF 换行）。**AI 执行者的常规写文件工具一律输出 UTF-8，无法直接"保持 GBK 保存"**——旧稿只说"保持原编码"而未给操作通道，是执行死点。本步的解法：**新脚本全文 ASCII 化**（注释与提示改英文，ASCII 字节在 GBK/UTF-8 完全一致，任何工具整文件重写都无转码风险），这是对 R1"禁转码令"的合规落地而非违反（无中文内容被静默重编码；属受控内容重写，记 NOTES.md 并同步更新 B4"全仓唯一非 UTF-8 文件"条目为已消除）。详细写入规程与备选 iconv 通道见操作 2。
- `Amalgamate.exe` 是 Windows 二进制（vinniefalco/Amalgamate 的编译产物，仓库内无其源码；重编译方法见 `tools/Amalgamate.md` "编译amalgamate"节）——**amalgamation CI job 因此需 windows runner**（见 S5-2）。
- 模板机制（`tools/Amalgamate.md` 有完整说明）是 **4 文件一套**，不是 2 个：
  | 文件（tools/amalgamate/） | 作用 |
  |---|---|
  | `SARibbonAmalgamTemplate.h` | 产物 .h 的种子：`SA_RIBBON_H` guard + 定义 `SA_RIBBON_BAR_NO_EXPORT`/`SA_COLOR_WIDGETS_NO_DLL` + include PublicHeaders |
  | `SARibbonAmalgamTemplatePublicHeaders.h` | 枚举全部公共头（`../../src/SARibbonBar/...` 相对路径，顺序 Global→colorWidgets→各类） |
  | `SARibbonAmalgamTemplateHeaderGlue.h` | 一行转发 include PublicHeaders，给 @remap 提供间接层 |
  | `SARibbonAmalgamTemplate.cpp` | 产物 .cpp 的种子：定义宏 + `/*@remap "SARibbonAmalgamTemplatePublicHeaders.h" "SARibbon.h" */` + include Glue + MSVC 4996 抑制段 + include `../qrc_SARibbonResource_Datas.cpp`、`../qrc_SARibbonResource_version2/3.cpp`（按 `QT_VERSION>=5.14` 选择）+ **逐个显式枚举全部 .cpp**（`../../src/SARibbonBar/...`） |
- 产物 `src/SARibbon.h/.cpp` 当前被 git 跟踪（`git ls-files src/SARibbon.h src/SARibbon.cpp` 命中），`.gitignore` 无对应条目；`.gitattributes` 有 `src/SARibbon.cpp -text`、`src/SARibbon.h -text`（"字节冻结"行）；`src/SARibbonBar/CMakeLists.txt:304-314` 有 `install(FILES ${SARIBBON_AMALGAMATE_FILES} DESTINATION share/SARibbonBar_amalgamate)`。
- 产物头部无"生成文件"横幅注释（首行即模板内容 `#ifndef SA_RIBBON_H`），识别完全靠 AGENTS.md/.gitattributes 约定。

**操作**：

1. **模板双套化**（`tools/amalgamate/`，Core 一套 + Widgets 一套，各 4 文件；旧 4 文件 `git mv` 改造成 Widgets 套以保留历史）：
   - `SARibbonCoreAmalgamTemplate.h`：guard `SA_RIBBON_CORE_H`；定义 core 的静态导出宏（宏名以计划 01 S6 落地的 `SA_RIBBON_CORE_*` 三段式为准，**待核实**：`git grep -n "SA_RIBBON_CORE_STATIC\|SA_RIBBON_CORE_LIBRARY" src/core/`）；include `SARibbonCoreAmalgamTemplatePublicHeaders.h`。
   - `SARibbonCoreAmalgamTemplatePublicHeaders.h`：枚举 core 全部公共头，**以 `git ls-files src/core` 实际输出为准**。注意 core 头物理位于**子目录**（`src/core/global/`、`metrics/`、`contract/`、`layout/` 等；同步目录/安装树才平铺为 `SARibbonCore/<名>.h`，02-S1.5 决策），模板必须写**真实子目录路径**（如 `../../src/core/global/SARibbonEnums.h`、`../../src/core/metrics/SARibbonMetrics.h`），不要按同步目录形态平铺。范围 = 01 骨架（`SARibbonCoreGlobal.h`、`SARibbonQt5Compat.hpp`）+ 02 下沉的 theme/metrics/contract/layout/data/factory/global 各头。**执行时核对例外**：`SARibbonCoreConfig.h` 是 build 树 `configure_file` 产物（01-S4/S6.2，源码树不存在），若 `git grep -n "SARibbonCoreConfig" src/core` 命中任何 include 它的翻译单元，模板无法直接枚举它——由操作 2 的镜像段从 build 树拷贝兜底，并在 NOTES 记录实际命中情况。
   - `SARibbonCoreAmalgamTemplateHeaderGlue.h`：一行 include CorePublicHeaders（照抄现 Glue 写法）。
   - `SARibbonCoreAmalgamTemplate.cpp`：定义宏 + `/*@remap "SARibbonCoreAmalgamTemplatePublicHeaders.h" "SARibbonCore.h" */` + include Glue + MSVC 4996 抑制段 + 枚举 `../../src/core/` 全部 .cpp（**真实子目录路径**，如 `../../src/core/layout/SARibbonPanelLayoutEngine.cpp`，以 `git ls-files src/core` 实测为准；01 骨架的占位 `SARibbonCoreGlobal.cpp` 内容仅一行 include，按 01-S8 结论**不进模板**；02 下沉后以真实源清单为准）；**不含 qrc 资源段**（QSS/图标资源属 widgets）。
   - `SARibbonWidgetsAmalgamTemplate.h`：guard `SA_RIBBON_WIDGETS_H`；定义 `SA_RIBBON_WIDGETS_STATIC` 与 `SA_RIBBON_CORE_STATIC`（旧宏 `SA_RIBBON_BAR_NO_EXPORT`/`SA_COLOR_WIDGETS_NO_DLL` 经 01-S6 的兼容转发已等价映射，模板内不必再定义——**待核实**：以 01-S6 落地后的 `src/widgets/SARibbonWidgetsGlobal.h` 实际映射为准）。
   - `SARibbonWidgetsAmalgamTemplatePublicHeaders.h`：首行 `#include "SARibbonCoreAmalgamTemplatePublicHeaders.h"`（core 段整体并入），再在**现模板（01-S8 sed 后）既有条目集合**的基础上调整：已下沉 core 的条目（如 Global/VersionInfo/Qt5Compat 与 02 下沉的主题/度量等文件——**具体哪些以 `git ls-files src/core src/widgets` 实测位置为准**，勿按本文示例硬套）从本段**删除**（已由嵌套的 CorePublicHeaders 覆盖，避免同内容经两条路径重复内联）；其余条目保持原顺序、路径前缀改 `../../src/widgets/...`/`../../src/widgets/colorWidgets/...`。**不要按"目录下全部头文件"机械新增条目**——实测现模板有意不含个别内部头（`SARibbonMdiControlsStyle.h` 不在 PublicHeaders，其内容经 .cpp 的 include 链内联进 .cpp 产物侧），机械补齐会改变产物结构；01/02 新增的公共头以"是否进入 install 公共头清单（sa_sync_include/安装树）"为增补判据，逐个记 NOTES。
   - `SARibbonWidgetsAmalgamTemplateHeaderGlue.h`：一行 include WidgetsPublicHeaders。
   - `SARibbonWidgetsAmalgamTemplate.cpp`：`/*@remap "SARibbonWidgetsAmalgamTemplatePublicHeaders.h" "SARibbonWidgets.h" */`（core 头经由 WidgetsPublicHeaders 的嵌套 include 一并被 remap 吸收，无需第二条 @remap）；**保留 qrc 三个资源文件 include**（`../qrc_SARibbonResource_Datas.cpp` + version2/version3 按 QT_VERSION 选择，路径不变——这三个文件在 `tools/` 下，是 rcc 预生成物，资源不变则无需重生成；**若日后资源变更需重生成 qrc，其生成注释内嵌的机器绝对路径会变化，须同步复查 01 §6 门禁 B 的豁免 pattern——round3 终审注，01-dryrun 建议 7**）；枚举 `../../src/core/` 全部 .cpp（真实子目录路径，同 Core 模板 .cpp 的说明）+ `../../src/widgets/`（含 colorWidgets）全部 .cpp——两侧清单均以 `git ls-files` 实测为准，与对应模块 CMakeLists 的源清单交叉核对（漏一个 .cpp = 产物链接期未定义符号）。
   - 可选改进：两套 .h 模板顶部各加一行 `// Generated by tools/Amalgamate.sh — DO NOT EDIT` 横幅（产物出库后无字节冻结约束）。
2. **`tools/Amalgamate.sh` 改为双产物 + 镜像继承 + 失败即停 + 非交互安全**，目标全文（**纯 ASCII**，理由与写入规程见代码块后"编码操作规程"；已按本形态通过 `bash -n` 语法校验）：
   ```bash
   #!/bin/bash
   # Must run with tools/ as cwd:  bash Amalgamate.sh
   # NOTE: this file is intentionally ASCII-only. It used to be the repo's
   # single GBK-encoded file (NOTES B4); ASCII bytes are identical in GBK and
   # UTF-8, so no editor/agent can corrupt it by encoding any more.
   set -e
   DEST=../src

   # --- namespaced-include mirror (inherited & extended from plan 01 S8) ---
   # Forwarding headers in src/widgets include <SARibbonCore/xxx.h>; that layout
   # exists only in the build-tree sync dir / install tree, so mirror it here.
   # Both the real subdir form (<SARibbonCore/global/X.h>) and the flattened
   # form (<SARibbonCore/X.h> and flat "X.h") are made resolvable.
   rm -rf _amalg_include
   mkdir -p _amalg_include/SARibbonCore
   find ../src/core -type f \( -name '*.h' -o -name '*.hpp' \) | while read -r f; do
       rel="${f#../src/core/}"
       mkdir -p "_amalg_include/SARibbonCore/$(dirname "$rel")"
       cp "$f" "_amalg_include/SARibbonCore/$rel"      # keep subdir layout
       cp "$f" "_amalg_include/SARibbonCore/"          # flattened copy
   done
   # Optional: SARibbonCoreConfig.h is generated into the build tree (plan 01
   # S4/S6.2). Pick it up if a configured build tree is present.
   for cfg in ../build*/include/SARibbonCore/SARibbonCoreConfig.h; do
       if [ -f "$cfg" ]; then cp "$cfg" _amalg_include/SARibbonCore/; break; fi
   done

   # --- SARibbonCore single file (core only) ---
   OPTS_CORE='-i "../src/core" -i "_amalg_include" -i "_amalg_include/SARibbonCore" -w "*.cpp;*.h;*.hpp" -s'
   ./Amalgamate.exe $OPTS_CORE ./amalgamate/SARibbonCoreAmalgamTemplate.h   $DEST/SARibbonCore.h
   ./Amalgamate.exe $OPTS_CORE ./amalgamate/SARibbonCoreAmalgamTemplate.cpp $DEST/SARibbonCore.cpp

   # --- SARibbonWidgets single file (core + widgets) ---
   OPTS_WIDGETS='-i "../src/core" -i "../src/widgets" -i "../src/widgets/colorWidgets" -i "_amalg_include" -i "_amalg_include/SARibbonCore" -w "*.cpp;*.h;*.hpp" -s'
   ./Amalgamate.exe $OPTS_WIDGETS ./amalgamate/SARibbonWidgetsAmalgamTemplate.h   $DEST/SARibbonWidgets.h
   ./Amalgamate.exe $OPTS_WIDGETS ./amalgamate/SARibbonWidgetsAmalgamTemplate.cpp $DEST/SARibbonWidgets.cpp

   # --- artifact sanity: all 4 files must exist (set -e + explicit check) ---
   for f in SARibbonCore.h SARibbonCore.cpp SARibbonWidgets.h SARibbonWidgets.cpp; do
       if [ ! -f "$DEST/$f" ]; then
           echo "ERROR: missing artifact $DEST/$f" >&2
           rm -rf _amalg_include
           exit 1
       fi
   done

   # LF -> CRLF (same awk logic as the 2.9.5 script, extended to 4 artifacts)
   convert_to_crlf() {
       local file="$1"
       if [ -f "$file" ]; then
           awk '{sub(/$/, "\r"); print}' "$file" > "${file}.tmp"
           mv "${file}.tmp" "$file"
           echo "Converted line endings to CRLF for $file"
       fi
   }
   for f in SARibbonCore.h SARibbonCore.cpp SARibbonWidgets.h SARibbonWidgets.cpp; do
       convert_to_crlf "$DEST/$f"
   done

   rm -rf _amalg_include   # transient; never committed (same as plan 01 S8)

   # Wait for a keypress only on an interactive terminal (the 2.9.5 script had
   # an unconditional 'read -n 1' which hangs CI).
   if [ -t 0 ]; then
       echo "Press any key to continue"
       read -r -n 1
   fi
   ```
   设计要点（对照 2.9.5 原脚本与 01-S8 应急版）：① `set -e` + 产物存在性检查——原脚本 exe 失败时静默继续（CI 假绿风险），现在失败即非零退出；② 镜像段从 01-S8 的"2 个文件"扩展为"全部 core 头 × 两种形态"（02 之后 core 头在子目录且对外平铺，`-i _amalg_include` 解析 `<SARibbonCore/...>`、`-i _amalg_include/SARibbonCore` 解析跨子目录平铺 include；镜像副本与实体是不同解析路径，同一头可能按两条路径各内联一次，include guard 保证编译安全、仅产物略冗余——01-S8 已注记，接受）；③ `[ -t 0 ]` 守卫交互等待；④ 原脚本的 `echo 继续运行` 尾行与 else-Warning 分支不再保留（`set -e` 与存在性检查取代）。
   **编码操作规程（R1 禁转码令的落地办法，实测可行）**：上述目标全文为纯 ASCII，直接用任何编辑工具整文件重写即可（写入后 `file tools/Amalgamate.sh` 应报 ASCII text，`git diff --stat tools/Amalgamate.sh` 应只含预期行；`.gitattributes` 的 `*.sh text eol=lf` 保证 LF 换行）。写入完成后在 NOTES.md 记一条："Amalgamate.sh 已 ASCII 化，全仓唯一非 UTF-8 代码文件消除，B4 状态更新"。**若维护者坚持保留中文提示**，唯一安全通道是：先把新内容以 UTF-8 写入临时文件，再 `iconv -f UTF-8 -t GBK tmp.sh > tools/Amalgamate.sh` 覆盖；**禁止**让编辑工具把 UTF-8 中文直接写进该文件（= 静默转码，违反 R1）。
   QML 不参与合并（【内联-3】）。同步更新 `tools/Amalgamate.md`（模板文件名、双产物、镜像机制、"产物不入库，由 CI/脚本生成"）。
3. **产物出库**（【内联-1】），四个动作缺一不可：
   1. `git rm --cached src/SARibbon.h src/SARibbon.cpp`（工作区文件保留，作为 01-S8 遗产由 StaticExample 继续用到 S2 切换为止）；
   2. `.gitignore` 追加 6 行：`src/SARibbon.h`、`src/SARibbon.cpp`（防本地残留误提交）+ `src/SARibbonCore.h`、`src/SARibbonCore.cpp`、`src/SARibbonWidgets.h`、`src/SARibbonWidgets.cpp`；
   3. `.gitattributes` **删除**字节冻结条目（文件已不跟踪，留着会误导；新产物不入库故无需新增冻结行）。实测两行原文为 `src/SARibbon.cpp -text` 与 `src/SARibbon.h   -text`（后者含对齐空格，删除时按整行匹配），其上方段落注释行 `# --- amalgamate 工具生成的合并文件：字节冻结，永不转换（该文件禁止手工修改）---` 一并删除，不留孤儿注释；
   4. 处理 `src/widgets/CMakeLists.txt`（原 `src/SARibbonBar/CMakeLists.txt:304-314`）的 `install(FILES ${SARIBBON_AMALGAMATE_FILES} DESTINATION share/SARibbonBar_amalgamate)`：**移除该规则**（单文件发行改由 CI 产物/Release 附件提供，计划 04 的发布物料清单接管），或降级为 `if(EXISTS ...)` 守卫 + `share/SARibbonWidgets_amalgamate`——二选一，记 NOTES.md；不处理则**fresh clone 后 `cmake --install` 必失败**（文件不存在）。

   **提交时序（R3 逐点可构建的落地办法，round3 补）**：若把本节 4 个出库动作在 S2 之前单独提交，会出现一个红灯窗口——旧 `src/SARibbon.h/.cpp` 已取消跟踪，而 StaticExample 仍无守卫地引用它们，fresh clone 默认配置（`SARIBBON_BUILD_EXAMPLES=ON`）构建必失败（本机不受影响：工作区文件保留；CI 不受影响：6 个 workflow 均 Examples=OFF，但 R3 口径是 fresh clone 可构建）。**推荐两笔提交序**：提交① = S1-1（模板双套化）+ S1-2（新脚本）+ S1-3-2（.gitignore 6 行，须先于本地跑新脚本，避免新产物成为未跟踪噪音）+ S1-4/S1-5（AGENTS.md 与引用点同步，其中 StaticExample 行留待 S2 兑现）——此时旧产物仍被跟踪、StaticExample 仍用旧文件，fresh clone 自洽；提交② = S2 全部（StaticExample 切新产物 + EXISTS 守卫）+ S1-3-1/3/4（`git rm --cached`、.gitattributes、install 规则）——切换完成后旧产物无消费者，出库安全。两节的提交信息约定见各自末尾（可按此时序微调措辞，记 NOTES）。
4. **AGENTS.md 同步**（重要，避免后续 agent 违规；01-S12 已更新过结构段，本步叠加）：
   - 禁令从"禁止读取或修改 `src/SARibbon.cpp/.h`"改为"**`src/SARibbon*.h/.cpp` 是 amalgamate 生成物，不入库不手改**；改动一律在 `src/widgets/` 与 `src/core/` 进行，需要单文件时在 `tools/` 目录运行 `bash Amalgamate.sh` 生成到本地（已被 .gitignore 排除）"。
5. **引用点盘点与同步**（`git grep -n "src/SARibbon\.\|SARibbon\.h\|SARibbon\.cpp" -- ':!plans' ':!docs'` + 下列实测清单逐项核对）：
   | 引用点 | 处理 |
   |---|---|
   | `MANIFEST.in` | `recursive-include src/SARibbonBar *.h *.hpp *.cpp *.qrc` → 改为 `src/core` 与 `src/widgets` 两行（sdist 打的是分散源码，不含单文件，无需为出库改动其他行） |
   | 根 `readme.md:114` / `readme-cn.md:115`（"引入 src 下的 SARibbon.h/SARibbon.cpp 即可使用"；行号为 2.9.5 基线实测，执行时以 grep 为准） | 改为"运行 `tools/Amalgamate.sh` 本地生成，或从 Release 附件下载单文件"（面向用户的完整措辞可留计划 04，本步先保证不误导） |
   | `examples/widgets/StaticExample/`（CMakeLists、README、**MainWindow.h 与 MainWindow.cpp 两处 include**） | 见 S2 |
   | `pyproject.toml`、`pyproject-pyqt6.toml`、`pyqt6/pyproject.toml`、`pyside6/CMakeLists.txt` | 见 S3（这三套绑定**不消费单文件**，改的是分散源码路径；本步 grep 对它们零命中属正常——它们引用的是 `src/SARibbonBar` 目录形式，由 S3.1 的盘点命令覆盖） |
   | `.gitattributes` / `.gitignore` / `src/widgets/CMakeLists.txt` | 本步 3 已处理 |
   | `AGENTS.md`（基线 :5/:16 两处禁令与结构图；01-S12 改过后行号会变） | 本步 4 已处理 |
   | `changlog.md`（基线 :54/:268/:426 历史条目提及 SARibbon.h/.cpp） | **不改**——历史更新记录保持原文；3.0 的双产物变更由 S7 的新草稿段记载 |
   | `tools/Amalgamate.md`（基线 :82/:86 等提及旧模板名与 "SARibbon.h"） | 本步 2 末尾的同步更新覆盖 |
   | `tools/Amalgamate.sh` / `tools/amalgamate/*`（脚本与模板本体自引用） | 本步 1/2 重写后自然消除 |
   | `tools/qrc_SARibbonResource_Datas.cpp`（rcc 生成物注释里嵌着旧机器绝对路径 `C:/src/Qt/SARibbon/src/SARibbonBar/resource/...`） | 仅注释、不影响构建，可不动；验收门 grep 对它做排除（见 §6） |

**验证**：在 `tools/` 目录 `bash Amalgamate.sh` 产出 4 个新单文件且脚本**不等待按键**（`bash Amalgamate.sh < /dev/null` 能正常退出、退出码 0）；`git status` 不显示它们；出库生效核对用 `git ls-files 'src/SARibbon*.h' 'src/SARibbon*.cpp'` 期望**空输出**（**必须用此顶层 pathspec 形式**——`git ls-files src/ | grep SARibbon` 会把 `src/widgets/SARibbonBar.h` 等数十个正常类头误报为"单文件残留"，旧稿此命令有歧义已更正）；**产物内容健全性**：`grep -n '#include <SARibbonCore/\|#include "SARibbon\|#include "SAColor' src/SARibbonCore.h src/SARibbonCore.cpp src/SARibbonWidgets.h src/SARibbonWidgets.cpp` 期望仅命中 2 行——`SARibbonCore.cpp` 顶部的 `#include "SARibbonCore.h"` 与 `SARibbonWidgets.cpp` 顶部的 `#include "SARibbonWidgets.h"`（两者都是 @remap 的正常产物），**其余任何命中 = 镜像失效或模板漏枚举，产物必坏**（Qt/系统头 `<QString>` 等不在此 pattern 内，属正常保留）；`cmake --install` 在 fresh clone 配置的构建树上不再因缺单文件报错。

**提交**：`重构：amalgamate 按模块双产物且生成物出库`

### S2 StaticExample 适配新单文件

**现状（已核实）**：`example/StaticExample/`（计划 01 S2 后位于 `examples/widgets/StaticExample/`）是**独立 CMake 工程**（自带 `project(StaticExample)`，可单独 configure），同时被 `example/CMakeLists.txt` `add_subdirectory(StaticExample)` 纳入常规 examples 构建。它把单文件直接编进目标：
```cmake
SET(SARIBBON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
set(SARIBBON_SIMPLE ${SARIBBON_DIR}/SARibbon.h ${SARIBBON_DIR}/SARibbon.cpp)
set(PROJECT_SOURCES main.cpp MainWindow.cpp MainWindow.h icon.qrc ${SARIBBON_SIMPLE})
```
`MainWindow.h:5` 为 `#include "SARibbon.h"`；另有 `target_include_directories(... $<BUILD_INTERFACE:${SARIBBON_DIR}>)` 与可选 QWindowKit 段（`SARIBBON_USE_FRAMELESS_LIB`，单文件场景恒以 `SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0` 编译）。

**操作**：
1. `SARIBBON_DIR` 指向 `<repo根>/src`——目录搬移后深度由 `../../src` 变 `../../../src`（若 01-S8 验证 StaticExample 时已改，复核即可）；`SARIBBON_SIMPLE` 换成 `SARibbonWidgets.h/.cpp`。
2. 加存在性守卫（单文件已出库，常规构建默认没有它）：
   ```cmake
   if(NOT (EXISTS "${SARIBBON_DIR}/SARibbonWidgets.h" AND EXISTS "${SARIBBON_DIR}/SARibbonWidgets.cpp"))
       message(WARNING "SARibbonWidgets.h/.cpp 未生成，跳过 StaticExample；请先在 tools/ 目录运行 bash Amalgamate.sh")
       return()
   endif()
   ```
   （子目录 CMakeLists 里 `return()` 合法；`examples/widgets/CMakeLists.txt` 无需改，跳过后其余示例照常。**standalone 场景行为差异**：单独 `cmake -S examples/widgets/StaticExample` 时本文件是顶层 CMakeLists，`return()` 会直接终止 configure 且不生成构建系统，后续 `cmake --build` 报"无构建文件"类错误——属预期而非 bug；"删除产物再 configure 得 WARNING 跳过"的验证**以常规 examples 构建（add_subdirectory 场景）为准**；CI amalgamation job（S5-2）总是先生成单文件再 standalone configure，守卫不触发。）
3. `MainWindow.h:5` **与 `MainWindow.cpp:2`** 两处 `#include "SARibbon.h"` → `#include "SARibbonWidgets.h"`（实测两文件各有一处，旧稿只列 .h 会漏改——本机因残留旧 `SARibbon.h` 可能照常编过，fresh clone/CI 才暴露）；`README.md` 同步单文件获取方式。
4. CI 联动：S5-2 的 amalgamation job 先生成单文件再 configure 本工程（standalone：`cmake -S examples/widgets/StaticExample -B build-static -DCMAKE_PREFIX_PATH=<Qt>`，不必开全量 examples）。

**验证**：本地 `bash tools/Amalgamate.sh` 后 StaticExample configure+编译+运行通过；删除生成的单文件再 configure，得到 WARNING 跳过且其余 examples 构建不受影响。

**提交**：`重构：StaticExample 适配模块化单文件`（与 S1 出库动作的提交合并次序见 S1-3"提交时序"：推荐本步全部改动与 S1-3-1/3/4 同笔提交，保证每个提交点 fresh clone 可构建）

### S3 Python 绑定适配（sip / PyQt6 / PySide6）【内联-2】【内联-4】

> 三套绑定**均直接从源码树的分散头/源文件构建，不消费单文件**（已核实），因此 S1 的出库对绑定无直接影响；本步的实质是"路径迁移 + 陈旧清单补齐 + 版本对齐"。每子步以"构建出轮子（或装入 site-packages）并 import 冒烟"为终点。
>
> **包名/模块名现状**（【内联-2】要求全部保持不变）：
> | 轨 | 配置 | sip/shiboken 模块声明 | import 冒烟 |
> |---|---|---|---|
> | PyQt5 | 根 `pyproject.toml` + `sip/`（14 个 .sip）+ 根 `project.py` | `%Module(name=PyQtSARibbon.saribbon ...)`（`sip/SARibbon.sip:1`） | `from PyQtSARibbon import saribbon` |
> | PyQt6-A（bat 脚本轨） | 根 `pyproject-pyqt6.toml`（构建时临时替换根 pyproject.toml）+ 复用根 `sip/` | 同上（PyQtSARibbon.saribbon） | `from PyQtSARibbon import saribbon`（`pyexamples/pyqt6/ribbon_demo.py:43` 用的就是这个名字） |
> | PyQt6-B（独立目录轨，publish CI 用） | `pyqt6/pyproject.toml` + `pyqt6/sip/`（14 个 .sip，与根 sip/ 近乎同文） | `%Module(name=PyQt6SARibbon.saribbon ...)`（`pyqt6/sip/SARibbon.sip:1`） | `from PyQt6SARibbon import saribbon` |
> | PySide6 | `pyside6/pyproject.toml` + `pyside6/CMakeLists.txt` + `typesystem_saribbon.xml` + `saribbon_python_glue.h` + `PySideSARibbon/__init__.py` | `<typesystem package="PySideSARibbon.saribbon">` | `from PySideSARibbon import saribbon`（现成冒烟脚本 `pyexamples/pyside6/test_binding.py`） |
>
> **构建脚本现状**：
> - `tools/build_python_bindings.bat [PYTHON_DIR] [QMAKE_PATH] [--pyqt6]`：自动探测 Python/qmake/vcvarsall → （--pyqt6 时把 `pyproject-pyqt6.toml` 临时覆盖根 `pyproject.toml`，完毕恢复）→ `sip-build.exe --build-dir build-python[6] --qmake ... --verbose` → **手工 copy 产物到 `site-packages\PyQtSARibbon\`（saribbon.pyd/.pyi + 生成的 `__init__.py`），不产轮子、不跑 cmake install** → 末尾冒烟 `from PyQtSARibbon import saribbon`。
> - `tools/build_pyside6_bindings.bat [PYTHON_DIR] [QT_DIR]`：`cmake -S pyside6 -B build-pyside6 -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="<Qt6>;shiboken6_generator;shiboken6;PySide6 的 lib/cmake"` → `cmake --build` → `cmake --install`（装进 site-packages/PySideSARibbon）。
> - 轮子路径（publish CI）：PyQt5=根目录 `python -m build --wheel --no-isolation` → `dist/*.whl`；PyQt6-B=`... pyqt6/` → `pyqt6/dist/*.whl`；PySide6=`python -m build --wheel pyside6/`（scikit-build-core）→ `pyside6/dist/*.whl`。

**S3.0 公共前置：命名空间 include 的解析（三轨必做，round3 新增——这是"轮子可构建"的硬前提）**：

01-S6.9/02-S1 之后，`src/widgets/` 里的**兼容转发头**（`SARibbonGlobal.h`、`SARibbonWidgetsGlobal.h`、`SARibbonQt5Compat.hpp`、`SAColorWidgetsGlobal.h` 等）内容是 `#include <SARibbonCore/xxx.h>` 形态的命名空间 include，而 `SARibbonCore/` 目录布局**只存在于主工程 build 树同步目录与安装树**，源码树中没有。三轨绑定全部**直接编译源码树文件**，其 include 路径（sip `include-dirs`、pyside6 `target_include_directories` 与 shiboken `-I`）若只指向 `src/core|widgets|widgets/colorWidgets`，`<SARibbonCore/...>` 无从解析——而 widgets 源几乎全部 include `SARibbonGlobal.h`（01-S6 实测 38 个文件），**第一个编译单元就会失败**。处理办法与 S1 amalgamate 的 `_amalg_include` 镜像同构，各轨落点：

1. **sip 两轨（根目录轨 + pyqt6/ 轨）**：仓库根准备构建期镜像目录 `build-binding-include/SARibbonCore/`（复制 core 公共头，"保留子目录 + 平铺"双形态，复制逻辑同 S1-2 脚本镜像段），三处 pyproject 的 `include-dirs` 追加该目录（pyqt6/ 轨写 `../build-binding-include`）；镜像生成挂在**两处**：`tools/build_python_bindings.bat`（sip-build 之前加复制段）与 publish/dry-run CI 的 wheel 步骤（`python -m build` 之前加一步 bash 复制命令）；目录加入 `.gitignore`。（备选：`project.py` 钩子或 `builder-settings` 的 qmake `system()` 现场复制——`project.py` 是否被 sipbuild 加载取决于 `[tool.sip.project] module-name` 键，现 toml 未设置该键、大概率是死配置，与 S3.1 的"待核实"项一并验证后再定，默认不依赖它。）
2. **pyside6 轨**：`pyside6/CMakeLists.txt` 内自足解决——`file(COPY ...)` 把 core 公共头镜像到 `${CMAKE_CURRENT_BINARY_DIR}/_sync_include/SARibbonCore/`，加进 `saribbon_lib` 的 `target_include_directories` **与 shiboken 的 include 参数**（wheel 构建无外部前置步骤，自包含，推荐落点）。
3. **备选（默认不采用）**：include 路径指向主工程 build 树 `${CMAKE_BINARY_DIR}/include` 或安装目录——引入"先构建主库"前置依赖，破坏轮子自包含性（R4 保守原则），记 NOTES。

**S3.1 路径盘点**（命令已实测，窄/宽两版都跑；行数以 2.9.5 基线实测为准，01/02 执行后重跑核对分布）：
```bash
P="sip pyqt6 pyside6 pyproject.toml pyproject-pyqt6.toml project.py MANIFEST.in tools/build_python_bindings.bat tools/build_pyside6_bindings.bat .github/workflows/publish-python-bindings.yml docs/zh/python-guide docs/en/python-guide"
# 窄版：真正的"路径引用"（必改项），实测 260 行
git grep -n "src/SARibbonBar" -- $P
# 宽版：含类名/头文件名噪音（实测 313 行，多出的 ~53 行全部不改），仅作参照
git grep -n "SARibbonBar\|src/SARibbon" -- $P
```
宽版噪音的构成（**均不改**——`SARibbonBar` 类名与 `SARibbonBar.h` 头文件名在 3.0 保持不变，文件仍在 `src/widgets/`）：.sip 的 `%TypeHeaderCode #include <SARibbonBar.h>`、`%Include SARibbonBar.sip`、typesystem 的 `<object-type name="SARibbonBar">`/`#include "SARibbonBar.h"`、docs 里的 `saribbon.SARibbonBar` Python 类名与类层次说明。窄版（必改路径引用）预期命中清单（实测）：

| 文件 | 命中行数 | 构成 |
|---|---|---|
| `pyproject.toml` | 86 | include-dirs 1 行（含 2 处）+ builder-settings 1 行 + headers 44 + sources 40 |
| `pyproject-pyqt6.toml` | 86 | 与上同构 |
| `pyqt6/pyproject.toml` | 86 | 与上同构（前缀 `../src/`，builder-settings 为 `../../src/`） |
| `MANIFEST.in` | 1 | `recursive-include src/SARibbonBar ...`（处理方式在 S1-5 表） |
| `pyside6/CMakeLists.txt` | 1 | :102 `SARIBBON_SOURCE_DIR`（其余清单行全部经 `${SARIBBON_SOURCE_DIR}` 变量引用，改一处即全改） |
| `sip/*.sip`、`pyqt6/sip/*.sip`、`typesystem_saribbon.xml`、`project.py`、两个 .bat、publish yml、docs/python-guide | 0 | 无路径引用（docs 的 9+9 处命中全是类名噪音） |

把盘点结果（含噪音项的处置结论）贴进 NOTES.md。（根 `project.py` 已核实存在：旧式 sipbuild API 自定义工程类，只声明 `saribbon_incdir/libdir/lib` 三个用户选项，**本身无路径引用，预计零改动**；它与根 pyproject.toml 的 `[tool.sip]` 段并存，sipbuild 实际优先采用哪套**待核实**——跑一次 `sip-build --verbose` 看输出引用的配置源；保守做法是两套同步改、不删任何一个。）

**S3.2 PyQt5（根 pyproject.toml + sip/ + project.py）**：
1. **路径迁移**（`[tool.sip.bindings.saribbon]` 段）：
   - `include-dirs = ["src/SARibbonBar", "src/SARibbonBar/colorWidgets"]` → `["src/core", "src/widgets", "src/widgets/colorWidgets", "build-binding-include"]`（末项为 S3.0 的命名空间镜像目录，缺它则转发头 `<SARibbonCore/...>` 解析失败）；
   - `headers`（44 项）/`sources`（40 项）两个显式清单逐条改前缀：Global/Qt5Compat/VersionInfo 等 02 下沉到 core 的文件指 `src/core/`，其余指 `src/widgets/`（colorWidgets 4+1 项指 `src/widgets/colorWidgets/`）——**以计划 02 完成后的实际文件位置为准**（`git ls-files src/core src/widgets` 输出对照改）；
   - `builder-settings = ["RESOURCES += ../../src/SARibbonBar/SARibbonResource.qrc"]` → `.../src/widgets/SARibbonResource.qrc`（该相对路径以 sipbuild 生成工程目录为基准，改后必须实测 qrc 注册生效，**待核实**：构建后跑冒烟时确认主题 QSS/图标可加载）；
   - `.sip` 文件本体**无需改 include 风格**：14 个 .sip 的 `%TypeHeaderCode` 全部用平面角括号 include（如 `#include <SARibbonBar.h>`、`#include <SARibbonGlobal.h>`），由 include-dirs 解析；`%Include`/`%Import` 只引同目录 .sip 与 Qt 模块 sip。（可选方案：改成 `<SARibbonWidgets/...>` 子目录限定形式并把 include-dirs 指向 `sa_sync_include` 的 `${CMAKE_BINARY_DIR}/include` 或安装目录——改动面大，默认不采用，记 NOTES。）
   - **枚举别名化适配（round3 终审新增——计划 02 S1/S1.1 的绑定侧承接点，sip 冒烟验证自 02 S1.1-4 顺延至本步）**：先读计划 02 在 NOTES.md 的枚举下沉记录（成功提升别名化，还是走了"四枚举留 widgets"降级路径），再核对/适配三处绑定文件的枚举条目：`sip/SARibbonPanelItem.sip:10` 与 `pyqt6/sip/SARibbonPanelItem.sip:10`（类内 `enum RowProportion` 声明）、`pyside6/typesystem_saribbon.xml:46`（`<enum-type name="RowProportion"/>`），以及 02 S1.1 四个 Q_ENUM 枚举（`PanelLayoutMode/RibbonMode/RibbonStyleFlag/RibbonButtonType`）的对应条目。02 若走降级路径（枚举留 widgets）则绑定文件零改动、只复核编译；若提升别名化成功，则以"绑定可编译 + Python 侧枚举值可访问"为准调整声明形式（sip/shiboken 对 using 别名与枚举符 using 声明的解析行为是 02 遗留风险 1 的工具链灰区，此处实测并记 NOTES）。验证并入 S3.2-4/S3.3/S3.4 各轨构建与 import 冒烟，另在 Python 侧抽查枚举拼写（如 `saribbon.SARibbonPanelItem.Large`）。
   - `define-macros`：现值 `SA_RIBBON_BAR_NO_EXPORT`、`SA_COLOR_WIDGETS_NO_DLL`、`SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0`、`NOMINMAX`。01-S6 的兼容转发使旧宏仍有效，可不动；如切新宏（`SA_RIBBON_CORE_STATIC`/`SA_RIBBON_WIDGETS_STATIC`）以 01-S6 落地名为准。
2. **陈旧清单补齐**（已核实的存量缺陷，与 3.0 迁移一并修）：三个 sip 系 pyproject 的 `sources` 均缺 `SARibbonThemeManager.cpp`、`SARibbonThemePalette.cpp`、`SARibbonMdiControlsStyle.cpp`（2.9.x 主题重构新增，`pyside6/CMakeLists.txt` 的清单里有，sip 侧漏更，版本停在 2.8.0 的直接后果）；`headers` 相应缺 `SARibbonThemeManager.h`、`SARibbonThemePalette.h`。补齐后与 widgets+core 源清单对齐复核：
   ```bash
   diff <(git ls-files 'src/widgets/*.cpp' 'src/core/*.cpp' | xargs -n1 basename | sort -u) \
        <(grep -o '[A-Za-z0-9_]*\.cpp' pyproject.toml | sort -u)
   ```
   （**命令已按实测修正**：git pathspec 的 `*` 会**跨目录**匹配——`src/widgets/*.cpp` 本身已含 `colorWidgets/` 子目录的 .cpp（2.9.5 基线用 `src/SARibbonBar/*.cpp` 实测命中其中 6 个 colorWidgets 文件），旧稿再列一条 colorWidgets 路径会重复计数造成 diff 噪音；左侧必须并入 `src/core/*.cpp`——02 下沉后 core 源全部要编进绑定，缺一即链接期未定义符号；`sort -u` 兜底去重。）期望差集只剩绑定刻意不编的文件，逐个在 NOTES 说明理由。
3. **版本**：`pyproject.toml` `version = "2.8.0"` → `"3.0.0"`（发行名 `PyQtSARibbon` 不变，【内联-2】）。
4. **构建冒烟**：`tools\build_python_bindings.bat`（无参自动探测；或显式 `tools\build_python_bindings.bat "C:\Python311" "<Qt>\bin\qmake.exe"`），期望末行输出 `Import OK: <class '...SARibbonMainWindow'>`；轮子形态以 S4 的 dry-run job（`python -m build --wheel --no-isolation` + `pip install dist/*.whl` + `python -c "from PyQtSARibbon import saribbon"`）为准。

**S3.3 PyQt6（双轨都要改）**：
- **轨 A**：根 `pyproject-pyqt6.toml`——与根 pyproject.toml 逐字段同构（仅 name 描述、依赖 PyQt6 不同），S3.2 的 1/2/3 全部同样执行一遍（version → 3.0.0）。构建验证：`tools\build_python_bindings.bat --pyqt6`（脚本自动做 toml 替换与恢复），冒烟 `from PyQtSARibbon import saribbon`。
- **轨 B**：`pyqt6/pyproject.toml` + `pyqt6/sip/`——path 前缀是 `../src/SARibbonBar/...`（相对 pyqt6/ 目录），改成 `../src/core|widgets|widgets/colorWidgets`，include-dirs 追加 `../build-binding-include`（S3.0 镜像，相对 pyqt6/）；**注意两种相对深度并存（实测）**：headers/sources/include-dirs 用 `../src/...`，而 `builder-settings` 的 RESOURCES 是 `../../src/SARibbonBar/SARibbonResource.qrc`（上两级，以 sipbuild 生成工程目录为基准，与根轨同值）——改成 `../../src/widgets/SARibbonResource.qrc` 时**勿顺手减成 `../src/`**；`sip-file = "sip/SARibbon.sip"` 不变；version → 3.0.0；`pyqt6/sip/` 的 14 个 .sip 与根 `sip/` 同文（抽查 `diff -u sip/SARibbonBar.sip pyqt6/sip/SARibbonBar.sip` 实测零差异），若轨 A 侧 .sip 有内容性修改需同步过来。轮子验证：`python -m build --wheel --no-isolation pyqt6/` + `pip install pyqt6/dist/*.whl` + `python -c "from PyQt6SARibbon import saribbon"`（**wheel 构建前确认 `build-binding-include/` 镜像已生成**——publish/dry-run CI 里由 S3.0-1 的前置步骤保证）。
- 双轨并存是现状（AGENTS.md："pyqt6/sip 独立维护"），本计划**不合并双轨**（超范围），只在 NOTES 记录"两套 .sip 内容重复，3.x 期间建议合一"。

**S3.4 PySide6（pyside6/）**：
- 现状机制：`pyside6/CMakeLists.txt` 是 standalone 工程（`project(PySideSARibbon VERSION 2.8.0)`），把 `SARIBBON_SOURCE_DIR = ../src/SARibbonBar`（:102-103）的 44 头/43 源（**含 ThemeManager/ThemePalette/MdiControlsStyle**，清单比 sip 侧新）编成 `saribbon_lib STATIC`，shiboken6 按 `typesystem_saribbon.xml` 生成 wrapper（实测 25 个 object/value 类型 = 23 `<object-type>` + 2 `<value-type>`，另 15 个 `<enum-type>`；旧文"26 个 wrapper"不确），qrc 直接编进 .pyd（`qt_add_resources`，:343，注释解释了为何不进静态库），install 目标 `SKBUILD_PLATLIB_DIR`（:424-425，scikit-build-core 提供）或 site-packages/PySideSARibbon。
- **不采用"链接 `SARibbon::Widgets`"**：轮子构建（scikit-build-core + publish CI）必须自包含，不能依赖先安装主库——保守方向（README R4）是维持"自编译源码"模式，只迁移路径；该偏差记 NOTES.md。
- 迁移操作：
  1. `SARIBBON_SOURCE_DIR` → `${CMAKE_CURRENT_SOURCE_DIR}/../src/widgets`；新增 `SARIBBON_CORE_DIR = .../src/core`；
  2. `SARIBBON_HEADERS`/`SARIBBON_SOURCES` 清单按 02 后实际位置改前缀，并**并入 core 的源文件**（02 下沉的引擎/主题数据 .cpp 必须编进 `saribbon_lib`，否则链接失败；shiboken 的 `-I` 参数与 `target_include_directories` 同步加 `${SARIBBON_CORE_DIR}`，**以及 S3.0-2 的 `_sync_include` 镜像目录**——`saribbon_lib` 编译与 shiboken 解析都会经转发头的 `<SARibbonCore/...>`，缺镜像即第一个翻译单元失败）；
  3. `typesystem_saribbon.xml` 的 inject-code 平面 include（`#include "SARibbonGlobal.h"` 等）依赖 include 目录解析，**默认零改动**（若 02 后出现同名头冲突再改限定形式）；`saribbon_python_glue.h` 与 `PySideSARibbon/__init__.py` 不涉及路径，零改动；
  4. 版本两处：`pyside6/pyproject.toml` `version = "2.8.0"` → `"3.0.0"`；`pyside6/CMakeLists.txt` `project(... VERSION 2.8.0)` → `3.0.0`。
- 构建冒烟：`tools\build_pyside6_bindings.bat`（实测脚本末段只**打印**提示行 `Verify: python -c "from PySideSARibbon import saribbon; print('OK')"`，**并不自动执行冒烟**——需手动运行该命令确认输出 OK）；轮子：`python -m build --wheel pyside6/` + `pip install pyside6/dist/*.whl` + `python pyexamples/pyside6/test_binding.py`。

（本机若无对应 Python 绑定构建环境，以 S4 的 CI dry-run job 为验证，记 NOTES.md 说明哪些子步交由 CI 验证。）

**S3.5 pyexamples 冒烟**：`pyexamples/pyqt5/ribbon_demo.py`、`pyexamples/pyqt6/ribbon_demo.py`、`pyexamples/pyside6/ribbon_demo.py` 各跑一次（offscreen 或本机 GUI）；pyside6 另跑 `test_binding.py`。注意 pyqt6 示例 import 的是 `PyQtSARibbon`（轨 A 产物）——若只装了轨 B 轮子（`PyQt6SARibbon`），需临时改 import 或记 NOTES 说明。

**S3.6 绑定文档同步**：`docs/{zh,en}/python-guide/` 各 4 篇（`build-python-bindings.md`、`build-pyside6-bindings.md`、`use-python-bindings.md`、`publish-to-pypi.md`）内含 bat 用法与 toml 替换流程描述（实测 zh 版 `build-python-bindings.md:147` 与 `:171-177` 直接描述 `pyproject-pyqt6.toml` 替换步骤），按 S3.2–S3.4 的最终形态同步（**含 S3.0 新增的镜像前置步骤说明**）；注意这 4 篇对 `src/SARibbonBar` 的**路径引用为零**（S3.1 窄版实测，命中全是 `saribbon.SARibbonBar` 类名——类名不变、不改），只改工程事实，不做面向用户的重写（那属计划 04）。

**提交**（按绑定分笔）：`适配：PyQt5 绑定迁移三模块头与清单补齐` / `适配：PyQt6 绑定双轨迁移` / `适配：PySide6 绑定迁移三模块源码清单` / `文档：python-guide 构建路径同步`

### S4 Python 发布 CI 适配与强化

**现状（已核实）**：`publish-python-bindings.yml` 自唯一提交（6f32beb）起的触发器就是 `release: types:[published]` + `workflow_dispatch`，**从未有过 tag 触发**；仓库既有 tag 全部是 `v2.x.y`（最新 `v2.9.5`）。jobs：`build-pyside6` / `build-pyqt5` / `build-pyqt6` 各为 `{windows,ubuntu,macos}-latest × py{3.10,3.11,3.12}` 9 格矩阵（Qt：win 用 aqtinstall 6.8.3/5.15.2，linux 用 apt，mac 用 brew；MSVC 经 ilammy/msvc-dev-cmd），产物 `actions/upload-artifact`；`publish` job 汇总后经 PyPI Trusted Publishing（OIDC，`environment: pypi`）上传。**计划 01 S10-4 明确"不改此文件"**（本就无 push/tag 触发可删，且 GitHub Actions 的 release 触发执行的是**默认分支 master** 上的 workflow 版本，dev-3.0 分支改它不影响 master 的 2.x 发布行为），因此 01 执行前后该 yml 均保持上述现状——旧稿"01-S10-4 已把触发删剩 dispatch-only + TODO 注释"与 01 正文矛盾，round3 已更正（P4 同步改）。若执行本步时实际发现文件已是 dispatch-only 暂停态，说明 01 的执行偏离了其正文，按 R4 记 NOTES 后仍按本节操作。

**操作**：
1. **触发保持现状**：`release: types: [published]` + `workflow_dispatch` 原样保留（01 未删触发，**无"恢复"动作**；且计划 04 的发布流程是"打 v3.0.0 tag + 创建 GitHub Release"，Release 发布事件天然触发；dev-3.0 上的修改合入 master 后才对 release 生效，开发期间 2.x 发布走 master 版本互不干扰）。若改用 `push: tags: ['v3.0.*']` 属**新增行为**而非"保持现状"，如坚持需在 NOTES.md 记录理由并与计划 04 的发布步骤对齐——默认不采用。
2. **步骤适配核查**：三个 build job 的命令只引用 `pyside6/`、`pyqt6/`、仓库根（不直接引用 `src/SARibbonBar` 路径），S3 改完 toml/CMakeLists 后 workflow 正文**零路径改动**；但 `build-pyqt5` 与 `build-pyqt6` 两个 job 需在 Build wheel 步骤**之前各加一步镜像生成**（`bash` 复制 core 头到 `build-binding-include/SARibbonCore/`，复制逻辑同 S1-2 脚本镜像段，S3.0-1 的落点之一；`build-pyside6` 无需——其镜像在 CMakeLists 内自足）；逐项复核 Qt 版本满足 3.0 门槛（Qt5 轨 = 5.15.2 ✓，Qt6 轨 = 6.8.3 ✓）。
3. **新增 dry-run job**（供本计划验收，不发布）：
   - `if: github.event_name == 'workflow_dispatch'`（或加 `inputs.dry_run` 布尔输入）；矩阵裁剪为 `windows-latest + ubuntu-latest × py3.12`；
   - 步骤：三轨各 `python -m build --wheel ...`（**wheel 构建前先生成 S3.0-1 的 `build-binding-include/` 镜像**）→ `pip install <wheel>` → import 冒烟三连：`from PyQtSARibbon import saribbon`、`from PyQt6SARibbon import saribbon`、`from PySideSARibbon import saribbon`（Windows/Linux 各跑，offscreen：`QT_QPA_PLATFORM=offscreen`）→ `actions/upload-artifact` 留存轮子；**不依赖 publish job、不触发 PyPI 上传**。
   - **同环境并存风险提示（round3 补）**：三连冒烟意味着同一 Python 环境同时装 PyQt5、PyQt6、PySide6——pip 包名互不冲突，但 Qt5/Qt6 运行库与平台插件路径存在互相干扰的可能（尤其 Linux）。稳妥实现：**每轨用独立 venv**（`python -m venv` → install → 冒烟），或直接拆成三个 job；若首次实现图省事用同一环境且冒烟失败，先拆环境再排查，不要误判为绑定问题（记 NOTES）。

**验证**：手动 `workflow_dispatch` 触发，dry-run job 全绿且 artifact 可下载；`release` 触发路径以 workflow 语法审查为准（3.0.0 发布时实跑，计划 04 复核）。

**提交**：`CI：适配 Python 绑定发布流水线并新增 dry-run`

### S5 CI 全矩阵与纯净性矩阵（v2 §6.2）

**现状（已核实，6 个 `cmake-{win,linux,mac}-qt{5.15,6.8}.yml` 结构完全同构）**：`on: [pull_request, push]`；单 job，matrix 仅 `{os_version:[latest], qt_version:[5.15.*|6.8.*], shared:[ON]}`（mac-qt5.15 特例 `macos_version:[13]`，Intel runner）；`actions/checkout@v4` **不带 submodules**（qwindowkit submodule 未初始化也不影响——CI 不开 frameless）；Qt 一律 `jurplel/install-qt-action@v4`（win-qt6.8：`arch: win64_msvc2022_64`；win-qt5.15：未指定 arch，用 action 默认；mac 两个：`host: mac, arch: clang_64`；linux 两个：无 host/arch，另有 apt 附加包 `libgl1-mesa-dev libxcb1-dev libgtk-3-dev`，qt6.8 多 `libxcb-cursor0`）；configure 统一为：
```
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DSARIBBON_BUILD_EXAMPLES=OFF -DBUILD_TESTS=ON -B build
```
ctest 带 `QT_QPA_PLATFORM=offscreen`。**不用 vcpkg**。计划 01 S10 已改：选项名（`BUILD_TESTS`→`SARIBBON_BUILD_TESTS`）、purity step 前置、linux-qt6.8 加 core-only 项；计划 02 S9 已加 core 黄金测试 step。本步在其上复核补齐：

**操作**：
1. 矩阵目标状态（6 workflow 全部达成）：

   | 维度 | 取值 | 落点与已核实注意事项 |
   |------|------|------|
   | 平台 × Qt | win/linux/mac × Qt5.15/Qt6.8（现有） | 保持 |
   | 组合构建 | linux-qt6.8 有 `SARIBBON_BUILD_WIDGETS=OFF` 项（core-only） | 01-S10-2 已加，复核存在 |
   | 静态 | 至少一个 `SARIBBON_BUILD_STATIC_LIBS=ON` 项（建议 win-qt6.8 加 `static: [OFF, ON]` 维度或独立 job） | **必须用 `SARIBBON_BUILD_STATIC_LIBS`**：库类型由它决定（`src/widgets/CMakeLists.txt` 原 :130-136 `if(SARIBBON_BUILD_STATIC_LIBS) STATIC else SHARED`）；现有 `-DBUILD_SHARED_LIBS=ON` 对本项目**无实效**（无处消费），static 项写成 `BUILD_SHARED_LIBS=OFF` 不会得到静态库。顺带评估删除无效的 shared 维度，记 NOTES |
   | 纯净扫描 | 6 workflow 全部 step 级前置 | 01-S10-1 已加，复核 `python3 tools/check_core_purity.py src/core`（CI 调用名与 01-S9/S10-1 统一为 python3，Linux runner 无 `python` 命令）在 configure 之前 |
   | core 黄金测试 | 6 workflow 全部前置且 merge blocking | 02-S9 已加，复核其 `ctest -L core`（**LABELS 过滤**）step 失败即 job 失败——**不要写成/改回 `-R core`**：现有测试以文件名注册、名称不含 "core" 字样，`-R core` 匹配不到任何测试且 ctest 照样退出 0（假绿），02-S9 已明文纠正，本表与其保持一致 |

2. 新增独立 workflow `amalgamation.yml`：**`runs-on: windows-latest`**（`tools/Amalgamate.exe` 是 Windows 二进制且仓库无其源码；若日后要 linux 跑，需按 `tools/Amalgamate.md` 从 vinniefalco/Amalgamate 源码现编，本计划不做）。步骤：checkout → `jurplel/install-qt-action`（6.8.*，win64_msvc2022_64）→ `working-directory: tools` 跑 `bash Amalgamate.sh`（windows runner 自带 Git Bash；依赖 S1-2 的非交互与 `set -e` 改造，否则挂死/假绿）→ `cmake -S examples/widgets/StaticExample -B build-static -DCMAKE_PREFIX_PATH=<Qt>` + build（standalone 工程，不必开全量 examples；`<Qt>` 用 install-qt-action 设置的 `$QT_ROOT_DIR` 环境变量或 `${{ github.workspace }}/Qt/6.8.x/msvc2022_64`——也可不传：jurplel action 已把 Qt bin 加入 PATH，CMake 沿 PATH 即可定位 Qt6Config，现有 6 个 workflow 即此机制）→（可选）offscreen 运行冒烟（`QT_QPA_PLATFORM=offscreen`）。本 job **无 ctest 步骤**（StaticExample 不含测试）。触发：push 到 dev-3.0 + `workflow_dispatch`。
3. vcpkg preset 验证（手动一次并记 NOTES，或加 dispatch-only job）：`CMakePresets.json` 已核实存在 preset `vcpkg-msvc-x64-release`（另有 `-debug`、`-debug-static`、`-release-static`、`-debug-frameless`，以及它们共同继承的抽象基础 preset `vcpkg-base`——共 6 条，version 6 + `cmakeMinimumRequired 3.25`），presets version 6 → **需 CMake ≥ 3.25 且设置 `VCPKG_ROOT` 环境变量**；`vcpkg.json`（name=`saribbonbar`, version=2.8.0→S7 升 3.0.0）含 `frameless`（qwindowkit）与 `svg`（qtsvg）feature。**注意：release preset 不含 frameless 联动**——"frameless feature 联动"验证要么用 `vcpkg-msvc-x64-debug-frameless`（它设 `VCPKG_MANIFEST_FEATURES=frameless`，根 CMakeLists 检测到后自动 `SARIBBON_USE_FRAMELESS_LIB=ON`），要么新增 `vcpkg-msvc-x64-release-frameless` preset（一行 inherits + features，推荐，顺带交付）。frameless=ON 需 C++17 与 QWindowKit 可解析（vcpkg 装），配置通过即可，不要求编 qwindowkit submodule。
4. **ctest 防空转纪律复核**（round2 从 QWK 上游 CI 抄入，承接 01-S10-7）：本步矩阵扩项后，6 个平台 workflow 的**全部** ctest 调用（含 02-S9 新增的 core 黄金测试 step 与本步新增的矩阵项）统一带 **`--no-tests=error`**（01-S10-7 已给既有 Test step 加过，本步核对新增 step 不遗漏；S5-2 amalgamation job 与 S4 dry-run job 现设计均无 ctest 调用，日后若加也须带此参数）——ctest 在"0 个测试"时退出码为 0，矩阵/选项改动一旦让测试静默不注册（如 static 项把 `SARIBBON_BUILD_TESTS` 联动关掉却仍跑 ctest），CI 会假绿。证据：QWK 上游 main `.github/workflows/ci.yml` Test 步骤 `ctest --test-dir build --output-on-failure --no-tests=error`（本地 QWK 快照 1fb3ec7 无 `.github/`，经 GitHub API 从上游抓取，详见 reviews/round2/qwk-build-findings.md §一.6）。
5. **可选增强：安装包消费测试注册进 ctest**（QWK 上游同款模式，建议随 S6-3 的 test-find-package 复核一并做，做不做记 NOTES）：QWK 上游把"安装后以 cmake/qmake/msbuild 三种构建系统消费安装包"注册为 ctest 测试（`buildsystems.cmake/qmake/msbuild`），并在 CI 里用专门步骤核对测试已注册（`ctest -N -R` 逐项检查，防"工具缺失→测试静默跳过"）。SARibbon 对应做法：`tools/test-find-package/` 由手动冒烟升级为 `add_test(NAME consumer.cmake COMMAND ${CMAKE_COMMAND} ...)`（在 install 后 configure+build 该最小工程），纳入 01-S11.4/本计划 S6-3 的验收链。此条超出 01-S11.4 的"手动冒烟"基线，属增强项，不阻塞验收。

**验证**：push 后全部 workflow 绿（含 dry-run、amalgamation、static 项）。

**提交**：`CI：构建矩阵补齐（静态/core-only/纯净扫描/合并单文件）`

### S6 i18n 与安装细节收尾

**现状（已核实）**：
- i18n 源：`src/SARibbonBar/i18n/{SARibbon_zh_CN.ts, SARibbon_en_US.ts}`（01-S6 随目录整体搬到 `src/widgets/i18n/`）；`.qm` 不入库（`.gitignore` 有 `*.qm`）。
- qm 生成机制在 `src/SARibbonBar/CMakeLists.txt:233-290`（01 后为 `src/widgets/CMakeLists.txt`）翻译块：`find_package(Qt{5|6}LinguistTools QUIET)`（找不到则整段静默跳过）；`SARIBBON_UPDATE_TRANSLATIONS=ON` 走 `qt5/6_create_translation`（lupdate+lrelease，会改 .ts），默认 OFF 走 `qt5/6_add_translation`（仅 lrelease）；`QM_FILES` 挂 `target_sources`；`install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_BINDIR}/translations COMPONENT translations)`；POST_BUILD 复制到 `build/bin/translations/`。
- **运行时加载：库自身没有任何 QTranslator/qm 加载代码**（`git grep QTranslator src/SARibbonBar/` 零命中；`SARibbonResource.qrc` 只含图片/QSS/palette，不含 qm）——加载是宿主应用的事，因此"2.x 用户查找路径"= 安装产物 `bin/translations/SARibbon_*.qm`，**该 install 目的地保持不变**即可。
- install 规则现状（`src/SARibbonBar/CMakeLists.txt:289-355`）：头文件→`include/SARibbonBar/`（+`colorWidgets/`，COMPONENT headers）；单文件→`share/SARibbonBar_amalgamate`（S1-3 已处理）；`SARibbonBarConfig.cmake.in`（**现名是 SARibbonBarConfig，不是 SARibbonConfig**；01-S5/S6 之后才变为组件化 `SARibbonConfig.cmake` + `SARibbonTargets.cmake` + `SARibbon::` 命名空间 + 转发头 `include/SARibbonBar/`）→ `lib/cmake/SARibbonBar/`。根 CMakeLists 的 `SARIBBON_DOC_FILES`（readme×2+LICENSE）只 `set` 未 `install`，是死变量（readme/LICENSE 现不随安装分发；01 重写 CMake 时顺手补 install 或删除，记 NOTES）。
- `tools/test-find-package/` 当前**不存在**，由计划 01 S11 创建（旧路径 `<SARibbonBar/SARibbonBar.h>` + 新路径 `<SARibbonWidgets/SARibbonBar.h>` 双 TU、`find_package(SARibbon 3.0 REQUIRED COMPONENTS Widgets)`）。

**操作**：
1. qm 编译进构建：机制已在（上述翻译块），本步只做**迁移复核**——01-S6 重写 `src/widgets/CMakeLists.txt` 后确认翻译块存活、`TS_SOURCES` 引用的源清单变量与拆分后清单一致（core 骨架无 `tr()` 字符串，i18n 归属 widgets；若 02 下沉的代码引入 `tr()`，评估是否给 core 单独 TS，记 NOTES）；Qt5/Qt6 分支（`qt5_*`/`qt6_*`）保持。install 目的地 `bin/translations` 不动（运行时路径逻辑不存在"库内加载函数"，无需兼容层）。
2. lupdate 维护入口：无独立脚本，就是 `-DSARIBBON_UPDATE_TRANSLATIONS=ON` 的 CMake 选项（01 重写后若选项被更名/遗漏，在此恢复）；`.ts` 内相对路径由 lupdate 扫描源生成，目录搬移后跑一次 ON 构建核对 `git diff *.ts` 只有位置类变化（.gitattributes 已固定 `*.ts text eol=lf`，不会出现换行噪音）。
3. 安装树复核（`cmake --install build --config Release` 后逐项打勾；round2 整合注：全部 install 规则在 `SARIBBON_INSTALL`（默认 ON）守卫内——v2 §6.4/01 S4，本复核以默认 ON 执行）：
   - `include/SARibbonCore/`、`include/SARibbonWidgets/`（含 `colorWidgets/` 子目录）
   - 旧路径转发头 `include/SARibbonBar/`（01-S11.3 产物；其机制是按 `SARIBBON_HEADER_FILES` 逐头生成 `#include <SARibbonWidgets/<名>>` 转发，colorWidgets 子目录转发在 01-S11.3 是**可选分支**——复核以 01 执行时的实际取舍为准，不在此追加要求）
   - `lib/cmake/SARibbon/{SARibbonConfig,SARibbonConfigVersion,SARibbonTargets}.cmake`，`find_package(SARibbon COMPONENTS Core|Widgets)` 可用，target 为 `SARibbon::Core`/`SARibbon::Widgets`；另复核 `lib/cmake/SARibbonBar/` 兼容薄壳仍在（01-S11.2，`SARibbonBarConfig.cmake` + `SARibbonBar::SARibbonBar` INTERFACE 转发，保留一个版本周期）
   - `bin/translations/SARibbon_{zh_CN,en_US}.qm`（Qt Linguist Tools 在位时）
   - `share/*_amalgamate` 的处置结果与 S1-3-4 的决定一致
   - `tools/test-find-package/` 在 01-S11 版本上**补一个 `COMPONENTS Core` 最小消费 TU**（只 include core 头、只链 `SARibbon::Core`，证明 core 可独立消费；可选升级为 ctest 注册测试，见 S5-5）
   - **QML 安装布局结论（round2 补预警，round2 整合已解除为终态）**：本清单**没有** qml 相关项即为**终态正确状态**——计划 04 S1 已定型**命令式单轨**（v2 §5.3 round2 修订、NOTES B9），QML 叶子全部进 qrc 编入 SARibbonQml 库二进制，**安装期零新增产物**（无 qmldir/qmltypes/plugin 安装项），SARibbonQml 的安装与普通 C++ 库完全同构。实证：QWK 1.0.1 quick 库安装无任何 qml 项（`qwindowkit/src/quick/CMakeLists.txt` 全文 38 行、`src/CMakeLists.txt:100-115` 统一 install）；KDDW qtquick 前端安装仅头+库+cmake 包（`src/CMakeLists.txt:634-666`）。仅当 3.1+ 启用 `qt_add_qml_module` 声明式轨（v2 §9 候选清单 B-3、04 S1 附注预案）时，安装树才会新增 `lib/qt6/qml/SARibbon/` 一类条目（布局随 Qt 版本变化、两家均无先例），届时本清单必须增补对应复核项。`SARibbonConfig` 的 Qml 组件依赖只需 `find_dependency(Qt Quick/Qml)`（01 S11 骨架已补，QuickControls2 视 04 S5 决策）
4. 文档：根 `build.md` 与 `docs/{zh,en}/build-guide/build-SARibbon.md` 增加"单文件发行（生成方法：tools/ 下 bash Amalgamate.sh；产物不入库）"与"vcpkg/组件化 find_package"两节的 3.0 说明；`docs/{zh,en}/build-guide/i18n.md` 顺手修正两处——① 示例代码用 `translator.load("SARibbon_zh_CN.qm", ":/i18n/")`（Qt 资源路径，实测 zh 版 :32），与实际交付方式（安装树/构建目录 `bin/translations/` 下的文件路径加载）不符；② 旧路径叙述 `build-dir/src/SARibbonBar/i18n`、`src/SARibbonBar/i18n`（实测 zh 版 :21/:83/:85，en 版同构）随目录迁移同步为 `src/widgets/i18n`。
5. **tests 链接目标切换**（承接计划 01 S6-6 留给本计划的义务）：01-S7 后 `add_saribbon_test()` 函数体位于 `tests/widgets/CMakeLists.txt`，其中 `SARibbonBar` 目标引用实测有**两处形态**（2.9.5 基线 tests/CMakeLists.txt:9-10 与 :28）：① `target_link_libraries(${TEST_NAME} PRIVATE` 之后**独立一行的裸 `SARibbonBar`**——改为 `SARibbon::Widgets`；② POST_BUILD 的 `copy_if_different $<TARGET_FILE:SARibbonBar> ...`——改为 `$<TARGET_FILE:SARibbonWidgets>`（ALIAS 也能用于 TARGET_FILE 生成器表达式，但既然内部用法统一切换，直接写实名）。指向 `../src/SARibbonBar` 的 include 路径行已由 01-S7 删除，无需处理。兼容别名 `add_library(SARibbonBar ALIAS SARibbonWidgets)` **保留**（对外过渡一个版本周期，01-S6 决定），仅切换仓库内部用法。复核命令：`git grep -nE "^[[:space:]]+SARibbonBar[[:space:]]*$|TARGET_FILE:SARibbonBar" tests/` 期望**零命中**（**旧稿的 `git grep -n "SARibbonBar)" tests/` 无效**——裸目标名独占一行、不带右括号，该 pattern 在改动前后都命中不了任何东西；而 `SARibbonBarLayoutRTLTest`/`SARibbonBarEventFilterTest` 等**测试名**合法含 "SARibbonBar" 字样，不能作为清零判据）。

**提交**：`重构：i18n 与安装规则三模块化收尾`（tests 链接切换可并入或单独 `重构：tests 链接目标切换为 SARibbon::Widgets`）

### S7 版本与变更日志预演

**现状（已核实）**：
- 变更日志实名**根目录 `changlog.md`**（拼写确实少个 e），条目格式 `## YYYY-MM-DD -> X.Y.Z`，新在上。
- 版本号唯一来源：根 `CMakeLists.txt` 的 `SARIBBON_VERSION_{MAJOR,MINOR,PATCH}`（2.9.5 基线；**计划 01 S4 已升 3.0.0**，本步复核即可）；`src/SARibbonBar/SARibbonBarVersionInfo.h` 是由根 CMakeLists `configure_file`（:204-205）从 `.h.in` 生成**且被 git 跟踪**——改版本后需重新 configure 让生成物刷新并随提交入库（01-S6 后 core 另有 `SARibbonCoreConfig.h.in`，VersionInfo 暂保留等价内容，02 收编）。
- **绑定/打包侧版本全部停在 2.8.0（落后于库 2.9.5，存量不同步）**：`pyproject.toml`、`pyproject-pyqt6.toml`、`pyqt6/pyproject.toml`、`pyside6/pyproject.toml`、`pyside6/CMakeLists.txt`（project VERSION）、`vcpkg.json` 共 6 处。

**操作**：
1. `changlog.md` 顶部新增草稿段（沿用现有格式）：
   ```markdown
   ## 未发布 -> 3.0.0

   - 变更：目录重组为 src/core、src/widgets、src/qml 三模块（原 src/SARibbonBar）；
   - 新增：SARibbonCore 纯算法库（主题数据/度量/布局引擎），可脱离 Widgets 独立编译消费；
   - 变更：单文件发行改为 SARibbonCore.h/.cpp 与 SARibbonWidgets.h/.cpp 双产物，由 tools/Amalgamate.sh 生成，不再随仓库提交；
   - 变更：Python 绑定（PyQtSARibbon/PyQt6SARibbon/PySideSARibbon）适配三模块源码树，包名与导入名不变；
   - 变更：最低要求 CMake 3.21 / Qt 5.15 / C++17（round3 终审更正：原草稿写 3.16 系旧稿残留，计划 01 S4 已定 floor 3.21）；
   - …（面向用户的完整变更清单留给计划 04 定稿，本步先落工程性条目）
   ```
   （最低要求数值以计划 01 S4 实际落地的 `cmake_minimum_required`/`SARIBBON_MIN_QT_VERSION` 为准，发布前复核；"未发布"占位符由计划 04 定稿时替换为实际发布日期，以对齐现有 `## YYYY-MM-DD -> X.Y.Z` 格式。）
2. **版本收口清单**（本步逐项核对 = 3.0.0，S3 各子步已 bump 的只复核）：根 `CMakeLists.txt`（01 已改）、重新生成的 `SARibbonBarVersionInfo.h`（或 02 收编后的等价物）、`pyproject.toml`、`pyproject-pyqt6.toml`、`pyqt6/pyproject.toml`、`pyside6/pyproject.toml`、`pyside6/CMakeLists.txt`、`vcpkg.json`。
3. `docs/zh/build-guide/` 与 `docs/en/build-guide/`（**已核实两语言目录同名 5 篇**：`build-3rdparty.md`、`build-SARibbon.md`、`build-instructions.md`、`common-build-errors.md`、`i18n.md`）按三模块更新构建章节；python-guide 的同步已归 S3.6，此处只复核链接不断。

**提交**：`文档：构建指南按三模块重写；changelog 3.0 草稿`

## 6. 完成验收门（= v2 M2 交付判据）

- [ ] `bash tools/Amalgamate.sh`（cwd=tools，`< /dev/null` 非交互）产出 `src/SARibbonCore.h/.cpp` 与 `src/SARibbonWidgets.h/.cpp`，且**产物内容健全性 grep 通过**（S1 验证末段：除两行 @remap 产物 `#include "SARibbonCore.h"`/`#include "SARibbonWidgets.h"` 外，无任何 `<SARibbonCore/` 或项目头 include 残留）；`git ls-files 'src/SARibbon*.h' 'src/SARibbon*.cpp'` 空输出（顶层 pathspec，避免 `src/widgets/SARibbon*.h` 类头误报）且 `.gitignore` 生效；`.gitattributes` 冻结行已删；`cmake --install` 在 fresh clone 上通过（无 share/amalgamate 缺文件错误）
- [ ] CI 全矩阵绿：6 平台 workflow + core-only 项 + `SARIBBON_BUILD_STATIC_LIBS=ON` 静态项 + purity step + core 黄金前置 + amalgamation job（windows）+ Python dry-run
- [ ] StaticExample 以新单文件编译运行通过（CI amalgamation job 中完成）；删除单文件后常规 examples 构建仅 WARNING 跳过
- [ ] 至少一个 Python 绑定轮子可构建并 import 冒烟通过（本机或 CI dry-run；冒烟名以 S3 表格为准：`PyQtSARibbon`/`PyQt6SARibbon`/`PySideSARibbon` 的 `saribbon` 模块）
- [ ] 三套绑定的源清单与 `git ls-files src/core src/widgets` 对齐（S3.2-2 的 diff 复核无未解释缺失，ThemeManager/ThemePalette/MdiControlsStyle 已补入 sip 侧）
- [ ] 版本一致：§S7-2 清单 8 处全部 3.0.0
- [ ] `pyexamples` 至少一个示例运行（或 NOTES.md 记录环境受限、以 CI 为准）
- [ ] `cmake --preset=vcpkg-msvc-x64-release` 配置通过（`VCPKG_ROOT` 已设、CMake≥3.25）；frameless 联动经 `vcpkg-msvc-x64-debug-frameless`（或新增 release-frameless preset）验证
- [ ] 安装树复核清单（S6-3）全过；`tools/test-find-package/` 支持 `COMPONENTS Core` 与 `COMPONENTS Widgets`
- [ ] `git grep -rn "src/SARibbonBar" -- ':!plans' ':!docs' ':!changlog.md' ':!*.md' ':!tools/qrc_SARibbonResource*'` 无残留（命令已实测可运行；`tools/qrc_SARibbonResource_Datas.cpp` 的 rcc 注释嵌有旧机器绝对路径属生成物噪音，显式排除；出库后的 `src/SARibbon*.cpp` 不被 git grep 索引，天然不命中）
- [ ] `MANIFEST.in`（已核实存在）的 `recursive-include` 指向 `src/core`/`src/widgets`，sdist 可打全绑定所需源码
- [ ] tests 内部链接已全部 `SARibbon::Widgets`（兼容别名保留），ctest == N₀ + `tests/core` 全绿（回归未破坏）

## 7. 风险与回滚

| 风险 | 缓解 |
|------|------|
| 删除跟踪的生成物后下游脚本找不到单文件 | S1-5 盘点全部引用点（含 .gitattributes、share/ 安装规则、readme）；构建脚本里先跑 Amalgamate.sh 再编译；StaticExample 加 EXISTS 守卫 |
| Amalgamate.sh 的交互 `read` 或 Windows-only exe 挂死/挡住 CI | S1-2 已做 `[ -t 0 ]` 守卫 + `set -e` + 产物存在性检查（exe 失败不再静默假绿）；amalgamation job 固定 windows-latest；exe 损坏/缺失时按 tools/Amalgamate.md 从上游源码重编 |
| 新脚本丢弃 01-S8 的 `_amalg_include` 镜像机制，产物残留 `<SARibbonCore/...>` 行（编译必坏） | S1-2 目标脚本已内置镜像段（子目录 + 平铺双形态）；S1 验证与验收门的"产物内容健全性 grep"零残留是硬门 |
| Amalgamate.sh 被编辑工具误以 UTF-8 写入中文（静默转码违反 R1/B4） | S1-2 目标全文已 ASCII 化（编码事故基本免疫）；若维护者坚持中文提示，仅允许 iconv 通道；写后 `file` + `git diff --stat` 复核 |
| 绑定构建在第一个翻译单元即失败（widgets 转发头 `<SARibbonCore/...>` 在源码树 include 路径下无解析） | S3.0 公共前置：sip 系 `build-binding-include/` 镜像（bat 与 publish/dry-run CI 两处前置步骤），pyside6 在 CMakeLists 内 `_sync_include` 镜像自足；失败时先查镜像目录是否生成、是否已进 include 路径 |
| dry-run job 同一环境并存 PyQt5/PyQt6/PySide6 时 import 互相干扰 | S4-3 已注明：每轨独立 venv 或拆 job；先拆环境再排查绑定问题 |
| 绑定构建环境本机缺失 | 子步允许"CI dry-run 验证"，但必须在 NOTES.md 显式记录，不许静默跳过 |
| sip 侧源清单陈旧（2.8.0 时代清单缺 2.9.x 新源文件）导致轮子链接失败或功能缺失 | S3.2-2 强制与 `git ls-files` 对齐 + diff 复核进验收门 |
| sipbuild 配置双源（project.py 与 pyproject.toml 并存）行为不明 | 待核实项：先跑 `sip-build --verbose` 确认生效配置；两套同步改，不删任何一个（保守） |
| Qt5/Qt6 sip 工具链行为差异 | 每绑定独立提交；失败回滚该笔不影响主线 |
| CI 矩阵超时 | amalgamation 与 python dry-run 独立 workflow/job，不挂进平台构建的关键路径 |

## 8. 已知偏差

见 [NOTES.md](NOTES.md)。本计划评审时新增待记录项：① publish workflow 历史触发器为 release 而非 tag（S4）；② `-DBUILD_SHARED_LIBS` 对库类型无实效（S5）；③ sip 绑定清单陈旧（S3）；④ 根 CMakeLists `SARIBBON_DOC_FILES` 死变量（S6）；⑤ `docs/*/build-guide/i18n.md` 的 `:/i18n/` 示例路径与实际交付不符（S6-4）；⑥（round3）旧稿 P4/S4 称"01-S10-4 已把 publish yml 暂停为 dispatch-only"，与 01-S10-4 正文"不改此文件"矛盾，已更正为"01 不动、现状即 release+dispatch、本计划直接适配"（P4/S4）；⑦（round3）pyside6 typesystem 实测 25 个 object/value 类型（23 object-type + 2 value-type，另 15 个 enum-type），旧文"26 个 wrapper"不确（S3.4）；⑧（round3）两处命令级实测更正：git pathspec 的 `*` 跨目录匹配——S3.2-2 清单命令不再重复列 colorWidgets；`git ls-files src/ | grep SARibbon` 会误报 `src/widgets/` 类头——S1 验证与验收门改用顶层 pathspec `git ls-files 'src/SARibbon*.h' 'src/SARibbon*.cpp'`（S1/§6）；⑨（round3）Amalgamate.sh 目标全文 ASCII 化后全仓唯一 GBK 文件消除，NOTES B4 状态待执行后更新（S1-2）。
