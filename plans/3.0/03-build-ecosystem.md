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
2. **Python 绑定适配**【内联-4】：`sip/`（PyQt5）、`pyqt6/sip/`、`pyside6/` 的路径与源清单全部切到三模块形态，轮子可构建，恢复 publish workflow。
3. **CI 全矩阵**：6 平台 workflow + 纯净性矩阵项 + amalgamation job + core 黄金测试前置（计划 02 S9 已加，复核）。
4. **安装细节**：i18n qm 安装、静态库样例、`find_package` 组件消费文档化验证。

## 2. 范围与非目标

**非目标**：QML 模块实现与示例（计划 04）；面向用户的文档重写与迁移指南（计划 04，本计划只交工程侧说明，含 `docs/{zh,en}/python-guide/` 的路径同步）；正式 3.0.0 发布与 tag（计划 04）。

## 3. 前置条件（逐项验证）

| # | 检查 | 期望 |
|---|------|------|
| P1 | 计划 02 验收门全绿 | 复核 [02 验收门](02-core-sinking.md#6-完成验收门-v2-m1-交付判据全部满足) 全勾 |
| P2 | 测试基线 | `ctest` 通过数 == N₀ + `tests/core` 黄金测试全绿 |
| P3 | 单文件现状 | `src/SARibbon.h/.cpp` 存在且由计划 01 S8 的脚本生成（含 core 骨架；01-S8 已把 OPTS 的 `-i` 指向 `../src/widgets`、`../src/widgets/colorWidgets`、`../src/core`） |
| P4 | publish workflow 已暂停 | `publish-python-bindings.yml` 仅 `workflow_dispatch` 触发且文件头有 `# TODO(3.0): 计划 03 恢复` 注释（计划 01 S10-4 完成后的状态；**2.9.5 基线原状是 `release: types:[published]` + `workflow_dispatch`，自该文件唯一提交 6f32beb 起从未有过 tag 触发**） |
| P5 | 纯净门禁 | `python tools/check_core_purity.py src/core` 退出码 0（脚本由计划 01 S9 创建；2.9.5 基线不存在） |
| P6 | 绑定基线认知 | 三套绑定当前均直接从**源码树分散头/源文件**构建（不是从单文件构建），版本字段全部停在 `2.8.0`，且 sip 两套 pyproject 的 sources 清单缺 2.9.x 新增源文件（见 S3.2-2）——迁移时一并修复 |

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
  计划 01 S8 已把 `-i` 改为 `../src/widgets`、`../src/widgets/colorWidgets`、`../src/core`。
- 脚本用 awk 把产物 LF→CRLF（`convert_to_crlf` 函数，保留）；**末尾有无条件 `read -n 1` 交互等待（"按任意键继续"），CI 调用会挂死，本步必须处理**。
- **编码警示（NOTES B4）**：`tools/Amalgamate.sh` 是全仓唯一 GBK 编码文件，编辑时必须保持 GBK 原编码保存（静默转 UTF-8 = 整文件 diff + 违反 R1 禁转码令）；改完用 `git diff --stat` 确认只有预期行变化。
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
   - `SARibbonCoreAmalgamTemplatePublicHeaders.h`：枚举 `../../src/core/` 全部公共头（01 骨架 3 头 + 02 下沉的 theme/metrics/contract/layout/data/factory 头）。
   - `SARibbonCoreAmalgamTemplateHeaderGlue.h`：一行 include CorePublicHeaders（照抄现 Glue 写法）。
   - `SARibbonCoreAmalgamTemplate.cpp`：定义宏 + `/*@remap "SARibbonCoreAmalgamTemplatePublicHeaders.h" "SARibbonCore.h" */` + include Glue + MSVC 4996 抑制段 + 枚举 `../../src/core/` 全部 .cpp；**不含 qrc 资源段**（QSS/图标资源属 widgets）。
   - `SARibbonWidgetsAmalgamTemplate.h`：guard `SA_RIBBON_WIDGETS_H`；定义 `SA_RIBBON_WIDGETS_STATIC` 与 `SA_RIBBON_CORE_STATIC`（旧宏 `SA_RIBBON_BAR_NO_EXPORT`/`SA_COLOR_WIDGETS_NO_DLL` 经 01-S6 的兼容转发已等价映射，模板内不必再定义——**待核实**：以 01-S6 落地后的 `src/widgets/SARibbonWidgetsGlobal.h` 实际映射为准）。
   - `SARibbonWidgetsAmalgamTemplatePublicHeaders.h`：首行 `#include "SARibbonCoreAmalgamTemplatePublicHeaders.h"`（core 段整体并入），再按现模板顺序枚举 `../../src/widgets/...` 与 `../../src/widgets/colorWidgets/...` 全部公共头。
   - `SARibbonWidgetsAmalgamTemplateHeaderGlue.h`：一行 include WidgetsPublicHeaders。
   - `SARibbonWidgetsAmalgamTemplate.cpp`：`/*@remap "SARibbonWidgetsAmalgamTemplatePublicHeaders.h" "SARibbonWidgets.h" */`（core 头经由 WidgetsPublicHeaders 的嵌套 include 一并被 remap 吸收，无需第二条 @remap）；**保留 qrc 三个资源文件 include**（`../qrc_SARibbonResource_Datas.cpp` + version2/version3 按 QT_VERSION 选择，路径不变——这三个文件在 `tools/` 下，是 rcc 预生成物，资源不变则无需重生成）；枚举 `../../src/core/` 全部 .cpp + `../../src/widgets/`（含 colorWidgets）全部 .cpp。
   - 可选改进：两套 .h 模板顶部各加一行 `// Generated by tools/Amalgamate.sh — DO NOT EDIT` 横幅（产物出库后无字节冻结约束）。
2. **`tools/Amalgamate.sh` 改为双产物 + 非交互安全**，目标全文：
   ```bash
   #!/bin/bash
   # 必须在 tools/ 目录下运行：bash Amalgamate.sh
   DEST=../src

   # --- SARibbonCore 单文件（仅 core） ---
   OPTS_CORE='-i "../src/core" -w "*.cpp;*.h;*.hpp" -s'
   ./Amalgamate.exe $OPTS_CORE ./amalgamate/SARibbonCoreAmalgamTemplate.h   $DEST/SARibbonCore.h
   ./Amalgamate.exe $OPTS_CORE ./amalgamate/SARibbonCoreAmalgamTemplate.cpp $DEST/SARibbonCore.cpp

   # --- SARibbonWidgets 单文件（core + widgets） ---
   OPTS_WIDGETS='-i "../src/core" -i "../src/widgets" -i "../src/widgets/colorWidgets" -w "*.cpp;*.h;*.hpp" -s'
   ./Amalgamate.exe $OPTS_WIDGETS ./amalgamate/SARibbonWidgetsAmalgamTemplate.h   $DEST/SARibbonWidgets.h
   ./Amalgamate.exe $OPTS_WIDGETS ./amalgamate/SARibbonWidgetsAmalgamTemplate.cpp $DEST/SARibbonWidgets.cpp

   # LF -> CRLF（沿用现有 awk 逻辑，扩展到 4 个产物）
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

   # 仅交互式终端等待按键（原脚本无条件 read -n 1，会挂死 CI）
   if [ -t 0 ]; then
       echo 按任意键继续
       read -n 1
   fi
   ```
   QML 不参与合并（【内联-3】）。同步更新 `tools/Amalgamate.md`（模板文件名、双产物、"产物不入库，由 CI/脚本生成"）。
3. **产物出库**（【内联-1】），四个动作缺一不可：
   1. `git rm --cached src/SARibbon.h src/SARibbon.cpp`（工作区文件保留，作为 01-S8 遗产由 StaticExample 继续用到 S2 切换为止）；
   2. `.gitignore` 追加 6 行：`src/SARibbon.h`、`src/SARibbon.cpp`（防本地残留误提交）+ `src/SARibbonCore.h`、`src/SARibbonCore.cpp`、`src/SARibbonWidgets.h`、`src/SARibbonWidgets.cpp`；
   3. `.gitattributes` **删除** `src/SARibbon.cpp -text`、`src/SARibbon.h -text` 两行字节冻结条目（文件已不跟踪，留着会误导；新产物不入库故无需新增冻结行）；
   4. 处理 `src/widgets/CMakeLists.txt`（原 `src/SARibbonBar/CMakeLists.txt:304-314`）的 `install(FILES ${SARIBBON_AMALGAMATE_FILES} DESTINATION share/SARibbonBar_amalgamate)`：**移除该规则**（单文件发行改由 CI 产物/Release 附件提供，计划 04 的发布物料清单接管），或降级为 `if(EXISTS ...)` 守卫 + `share/SARibbonWidgets_amalgamate`——二选一，记 NOTES.md；不处理则**fresh clone 后 `cmake --install` 必失败**（文件不存在）。
4. **AGENTS.md 同步**（重要，避免后续 agent 违规；01-S12 已更新过结构段，本步叠加）：
   - 禁令从"禁止读取或修改 `src/SARibbon.cpp/.h`"改为"**`src/SARibbon*.h/.cpp` 是 amalgamate 生成物，不入库不手改**；改动一律在 `src/widgets/` 与 `src/core/` 进行，需要单文件时在 `tools/` 目录运行 `bash Amalgamate.sh` 生成到本地（已被 .gitignore 排除）"。
5. **引用点盘点与同步**（`git grep -n "src/SARibbon\.\|SARibbon\.h\|SARibbon\.cpp" -- ':!plans' ':!docs'` + 下列实测清单逐项核对）：
   | 引用点 | 处理 |
   |---|---|
   | `MANIFEST.in` | `recursive-include src/SARibbonBar *.h *.hpp *.cpp *.qrc` → 改为 `src/core` 与 `src/widgets` 两行（sdist 打的是分散源码，不含单文件，无需为出库改动其他行） |
   | 根 `readme.md:115` / `readme-cn.md`（"引入 src 下的 SARibbon.h/SARibbon.cpp 即可使用"） | 改为"运行 `tools/Amalgamate.sh` 本地生成，或从 Release 附件下载单文件"（面向用户的完整措辞可留计划 04，本步先保证不误导） |
   | `examples/widgets/StaticExample/`（CMakeLists/README/MainWindow.*） | 见 S2 |
   | `pyproject.toml`、`pyproject-pyqt6.toml`、`pyqt6/pyproject.toml`、`pyside6/CMakeLists.txt` | 见 S3（这三套绑定**不消费单文件**，改的是分散源码路径） |
   | `.gitattributes` / `.gitignore` / `src/widgets/CMakeLists.txt` | 本步 3 已处理 |
   | `tools/qrc_SARibbonResource_Datas.cpp`（rcc 生成物注释里嵌着旧机器绝对路径 `C:/src/Qt/SARibbon/src/SARibbonBar/resource/...`） | 仅注释、不影响构建，可不动；验收门 grep 对它做排除（见 §6） |

**验证**：在 `tools/` 目录 `bash Amalgamate.sh` 产出 4 个新单文件且脚本**不等待按键**（`bash Amalgamate.sh < /dev/null` 能正常退出）；`git status` 不显示它们、`git ls-files src/ | grep SARibbon` 无单文件（ignore + 出库生效）；`cmake --install` 在 fresh clone 配置的构建树上不再因缺单文件报错。

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
   （子目录 CMakeLists 里 `return()` 合法；`examples/widgets/CMakeLists.txt` 无需改，跳过后其余示例照常。）
3. `MainWindow.h` 的 `#include "SARibbon.h"` → `#include "SARibbonWidgets.h"`；`README.md` 同步单文件获取方式。
4. CI 联动：S5-2 的 amalgamation job 先生成单文件再 configure 本工程（standalone：`cmake -S examples/widgets/StaticExample -B build-static -DCMAKE_PREFIX_PATH=<Qt>`，不必开全量 examples）。

**验证**：本地 `bash tools/Amalgamate.sh` 后 StaticExample configure+编译+运行通过；删除生成的单文件再 configure，得到 WARNING 跳过且其余 examples 构建不受影响。

**提交**：`重构：StaticExample 适配模块化单文件`

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

**S3.1 路径盘点**：
```bash
git grep -n "SARibbonBar\|src/SARibbon" -- sip pyqt6 pyside6 pyproject.toml pyproject-pyqt6.toml project.py MANIFEST.in tools/build_python_bindings.bat tools/build_pyside6_bindings.bat .github/workflows/publish-python-bindings.yml docs/zh/python-guide docs/en/python-guide
```
把所有引用旧目录/旧头的位置列成清单贴进 NOTES.md。（根 `project.py` 已核实存在：旧式 sipbuild API 自定义工程类，只声明 `saribbon_incdir/libdir/lib` 三个用户选项，**本身无路径引用，预计零改动**；它与根 pyproject.toml 的 `[tool.sip]` 段并存，sipbuild 实际优先采用哪套**待核实**——跑一次 `sip-build --verbose` 看输出引用的配置源；保守做法是两套同步改、不删任何一个。）

**S3.2 PyQt5（根 pyproject.toml + sip/ + project.py）**：
1. **路径迁移**（`[tool.sip.bindings.saribbon]` 段）：
   - `include-dirs = ["src/SARibbonBar", "src/SARibbonBar/colorWidgets"]` → `["src/core", "src/widgets", "src/widgets/colorWidgets"]`；
   - `headers`（44 项）/`sources`（40 项）两个显式清单逐条改前缀：Global/Qt5Compat/VersionInfo 等 02 下沉到 core 的文件指 `src/core/`，其余指 `src/widgets/`（colorWidgets 4+1 项指 `src/widgets/colorWidgets/`）——**以计划 02 完成后的实际文件位置为准**（`git ls-files src/core src/widgets` 输出对照改）；
   - `builder-settings = ["RESOURCES += ../../src/SARibbonBar/SARibbonResource.qrc"]` → `.../src/widgets/SARibbonResource.qrc`（该相对路径以 sipbuild 生成工程目录为基准，改后必须实测 qrc 注册生效，**待核实**：构建后跑冒烟时确认主题 QSS/图标可加载）；
   - `.sip` 文件本体**无需改 include 风格**：14 个 .sip 的 `%TypeHeaderCode` 全部用平面角括号 include（如 `#include <SARibbonBar.h>`、`#include <SARibbonGlobal.h>`），由 include-dirs 解析；`%Include`/`%Import` 只引同目录 .sip 与 Qt 模块 sip。（可选方案：改成 `<SARibbonWidgets/...>` 子目录限定形式并把 include-dirs 指向 `sa_sync_include` 的 `${CMAKE_BINARY_DIR}/include` 或安装目录——改动面大，默认不采用，记 NOTES。）
   - `define-macros`：现值 `SA_RIBBON_BAR_NO_EXPORT`、`SA_COLOR_WIDGETS_NO_DLL`、`SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0`、`NOMINMAX`。01-S6 的兼容转发使旧宏仍有效，可不动；如切新宏（`SA_RIBBON_CORE_STATIC`/`SA_RIBBON_WIDGETS_STATIC`）以 01-S6 落地名为准。
2. **陈旧清单补齐**（已核实的存量缺陷，与 3.0 迁移一并修）：三个 sip 系 pyproject 的 `sources` 均缺 `SARibbonThemeManager.cpp`、`SARibbonThemePalette.cpp`、`SARibbonMdiControlsStyle.cpp`（2.9.x 主题重构新增，`pyside6/CMakeLists.txt` 的清单里有，sip 侧漏更，版本停在 2.8.0 的直接后果）；`headers` 相应缺 `SARibbonThemeManager.h`、`SARibbonThemePalette.h`。补齐后与 widgets 源清单对齐复核：
   ```bash
   diff <(git ls-files 'src/widgets/*.cpp' 'src/widgets/colorWidgets/*.cpp' | xargs -n1 basename | sort) \
        <(grep -o '[A-Za-z0-9_]*\.cpp' pyproject.toml | sort -u)
   ```
   （期望差集只剩绑定刻意不编的文件，逐个在 NOTES 说明理由；core 的 .cpp 若被 widgets 头引用也必须进 sources。）
3. **版本**：`pyproject.toml` `version = "2.8.0"` → `"3.0.0"`（发行名 `PyQtSARibbon` 不变，【内联-2】）。
4. **构建冒烟**：`tools\build_python_bindings.bat`（无参自动探测；或显式 `tools\build_python_bindings.bat "C:\Python311" "<Qt>\bin\qmake.exe"`），期望末行输出 `Import OK: <class '...SARibbonMainWindow'>`；轮子形态以 S4 的 dry-run job（`python -m build --wheel --no-isolation` + `pip install dist/*.whl` + `python -c "from PyQtSARibbon import saribbon"`）为准。

**S3.3 PyQt6（双轨都要改）**：
- **轨 A**：根 `pyproject-pyqt6.toml`——与根 pyproject.toml 逐字段同构（仅 name 描述、依赖 PyQt6 不同），S3.2 的 1/2/3 全部同样执行一遍（version → 3.0.0）。构建验证：`tools\build_python_bindings.bat --pyqt6`（脚本自动做 toml 替换与恢复），冒烟 `from PyQtSARibbon import saribbon`。
- **轨 B**：`pyqt6/pyproject.toml` + `pyqt6/sip/`——path 前缀是 `../src/SARibbonBar/...`（相对 pyqt6/ 目录），改成 `../src/core|widgets|widgets/colorWidgets`；`sip-file = "sip/SARibbon.sip"` 不变；version → 3.0.0；`pyqt6/sip/` 的 14 个 .sip 与根 `sip/` 近乎同文（`diff -u sip/SARibbonBar.sip pyqt6/sip/SARibbonBar.sip` 抽查），若轨 A 侧 .sip 有内容性修改需同步过来。轮子验证：`python -m build --wheel --no-isolation pyqt6/` + `pip install pyqt6/dist/*.whl` + `python -c "from PyQt6SARibbon import saribbon"`。
- 双轨并存是现状（AGENTS.md："pyqt6/sip 独立维护"），本计划**不合并双轨**（超范围），只在 NOTES 记录"两套 .sip 内容重复，3.x 期间建议合一"。

**S3.4 PySide6（pyside6/）**：
- 现状机制：`pyside6/CMakeLists.txt` 是 standalone 工程（`project(PySideSARibbon VERSION 2.8.0)`），把 `SARIBBON_SOURCE_DIR = ../src/SARibbonBar`（:102-103）的 44 头/43 源（**含 ThemeManager/ThemePalette/MdiControlsStyle**，清单比 sip 侧新）编成 `saribbon_lib STATIC`，shiboken6 按 `typesystem_saribbon.xml` 生成 26 个 wrapper，qrc 直接编进 .pyd（`qt_add_resources`，:343，注释解释了为何不进静态库），install 目标 `SKBUILD_PLATLIB_DIR` 或 site-packages/PySideSARibbon。
- **不采用"链接 `SARibbon::Widgets`"**：轮子构建（scikit-build-core + publish CI）必须自包含，不能依赖先安装主库——保守方向（README R4）是维持"自编译源码"模式，只迁移路径；该偏差记 NOTES.md。
- 迁移操作：
  1. `SARIBBON_SOURCE_DIR` → `${CMAKE_CURRENT_SOURCE_DIR}/../src/widgets`；新增 `SARIBBON_CORE_DIR = .../src/core`；
  2. `SARIBBON_HEADERS`/`SARIBBON_SOURCES` 清单按 02 后实际位置改前缀，并**并入 core 的源文件**（02 下沉的引擎/主题数据 .cpp 必须编进 `saribbon_lib`，否则链接失败；shiboken 的 `-I` 参数与 `target_include_directories` 同步加 `${SARIBBON_CORE_DIR}`）；
  3. `typesystem_saribbon.xml` 的 inject-code 平面 include（`#include "SARibbonGlobal.h"` 等）依赖 include 目录解析，**默认零改动**（若 02 后出现同名头冲突再改限定形式）；`saribbon_python_glue.h` 与 `PySideSARibbon/__init__.py` 不涉及路径，零改动；
  4. 版本两处：`pyside6/pyproject.toml` `version = "2.8.0"` → `"3.0.0"`；`pyside6/CMakeLists.txt` `project(... VERSION 2.8.0)` → `3.0.0`。
- 构建冒烟：`tools\build_pyside6_bindings.bat`（期望末段提示 `python -c "from PySideSARibbon import saribbon; print('OK')"` 通过）；轮子：`python -m build --wheel pyside6/` + `pip install pyside6/dist/*.whl` + `python pyexamples/pyside6/test_binding.py`。

（本机若无对应 Python 绑定构建环境，以 S4 的 CI dry-run job 为验证，记 NOTES.md 说明哪些子步交由 CI 验证。）

**S3.5 pyexamples 冒烟**：`pyexamples/pyqt5/ribbon_demo.py`、`pyexamples/pyqt6/ribbon_demo.py`、`pyexamples/pyside6/ribbon_demo.py` 各跑一次（offscreen 或本机 GUI）；pyside6 另跑 `test_binding.py`。注意 pyqt6 示例 import 的是 `PyQtSARibbon`（轨 A 产物）——若只装了轨 B 轮子（`PyQt6SARibbon`），需临时改 import 或记 NOTES 说明。

**S3.6 绑定文档同步**：`docs/{zh,en}/python-guide/` 各 4 篇（`build-python-bindings.md`、`build-pyside6-bindings.md`、`use-python-bindings.md`、`publish-to-pypi.md`）内含 bat 用法、toml 替换流程与源码路径引用（实测 `build-python-bindings.md:120-171` 直接描述 `pyproject-pyqt6.toml` 替换步骤），按 S3.2–S3.4 的最终形态同步；只改工程事实，不做面向用户的重写（那属计划 04）。

**提交**（按绑定分笔）：`适配：PyQt5 绑定迁移三模块头与清单补齐` / `适配：PyQt6 绑定双轨迁移` / `适配：PySide6 绑定迁移三模块源码清单` / `文档：python-guide 构建路径同步`

### S4 恢复并强化 Python 发布 CI

**现状（已核实）**：`publish-python-bindings.yml` 自唯一提交（6f32beb）起的触发器就是 `release: types:[published]` + `workflow_dispatch`，**从未有过 tag 触发**；仓库既有 tag 全部是 `v2.x.y`（最新 `v2.9.5`）。jobs：`build-pyside6` / `build-pyqt5` / `build-pyqt6` 各为 `{windows,ubuntu,macos}-latest × py{3.10,3.11,3.12}` 9 格矩阵（Qt：win 用 aqtinstall 6.8.3/5.15.2，linux 用 apt，mac 用 brew；MSVC 经 ilammy/msvc-dev-cmd），产物 `actions/upload-artifact`；`publish` job 汇总后经 PyPI Trusted Publishing（OIDC，`environment: pypi`）上传。计划 01 S10-4 已把触发删剩 `workflow_dispatch` + TODO 注释。

**操作**：
1. **恢复触发**：还原 `release: types: [published]`（与历史一致，且计划 04 的发布流程是"打 v3.0.0 tag + 创建 GitHub Release"，Release 发布事件天然触发）。若改用 `push: tags: ['v3.0.*']` 属**新增行为**而非"恢复"，如坚持需在 NOTES.md 记录理由并与计划 04 的发布步骤对齐——默认不采用。
2. **步骤适配核查**：三个 build job 的命令只引用 `pyside6/`、`pyqt6/`、仓库根（不直接引用 `src/SARibbonBar` 路径），S3 改完 toml/CMakeLists 后 workflow 正文预计**零路径改动**；逐项复核 Qt 版本满足 3.0 门槛（Qt5 轨 = 5.15.2 ✓，Qt6 轨 = 6.8.3 ✓）。
3. **新增 dry-run job**（供本计划验收，不发布）：
   - `if: github.event_name == 'workflow_dispatch'`（或加 `inputs.dry_run` 布尔输入）；矩阵裁剪为 `windows-latest + ubuntu-latest × py3.12`；
   - 步骤：三轨各 `python -m build --wheel ...` → `pip install <wheel>` → import 冒烟三连：`from PyQtSARibbon import saribbon`、`from PyQt6SARibbon import saribbon`、`from PySideSARibbon import saribbon`（Windows/Linux 各跑，offscreen：`QT_QPA_PLATFORM=offscreen`）→ `actions/upload-artifact` 留存轮子；**不依赖 publish job、不触发 PyPI 上传**。

**验证**：手动 `workflow_dispatch` 触发，dry-run job 全绿且 artifact 可下载；`release` 触发路径以 workflow 语法审查为准（3.0.0 发布时实跑，计划 04 复核）。

**提交**：`CI：恢复 Python 绑定发布流水线并新增 dry-run`

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
   | 纯净扫描 | 6 workflow 全部 step 级前置 | 01-S10-1 已加，复核 `python tools/check_core_purity.py src/core` 在 configure 之前 |
   | core 黄金测试 | 6 workflow 全部前置且 merge blocking | 02-S9 已加，复核 `ctest -R core` step 失败即 job 失败 |

2. 新增独立 workflow `amalgamation.yml`：**`runs-on: windows-latest`**（`tools/Amalgamate.exe` 是 Windows 二进制且仓库无其源码；若日后要 linux 跑，需按 `tools/Amalgamate.md` 从 vinniefalco/Amalgamate 源码现编，本计划不做）。步骤：checkout → `jurplel/install-qt-action`（6.8.*，win64_msvc2022_64）→ `working-directory: tools` 跑 `bash Amalgamate.sh`（依赖 S1-2 的非交互改造，否则挂死）→ `cmake -S examples/widgets/StaticExample -B build-static -DCMAKE_PREFIX_PATH=<Qt>` + build（standalone 工程，不必开全量 examples）→（可选）offscreen 运行冒烟。触发：push 到 dev-3.0 + `workflow_dispatch`。
3. vcpkg preset 验证（手动一次并记 NOTES，或加 dispatch-only job）：`CMakePresets.json` 已核实存在 preset `vcpkg-msvc-x64-release`（另有 `-debug`、`-debug-static`、`-release-static`、`-debug-frameless`），presets version 6 → **需 CMake ≥ 3.25 且设置 `VCPKG_ROOT` 环境变量**；`vcpkg.json`（name=`saribbonbar`, version=2.8.0→S7 升 3.0.0）含 `frameless`（qwindowkit）与 `svg`（qtsvg）feature。**注意：release preset 不含 frameless 联动**——"frameless feature 联动"验证要么用 `vcpkg-msvc-x64-debug-frameless`（它设 `VCPKG_MANIFEST_FEATURES=frameless`，根 CMakeLists 检测到后自动 `SARIBBON_USE_FRAMELESS_LIB=ON`），要么新增 `vcpkg-msvc-x64-release-frameless` preset（一行 inherits + features，推荐，顺带交付）。frameless=ON 需 C++17 与 QWindowKit 可解析（vcpkg 装），配置通过即可，不要求编 qwindowkit submodule。

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
3. 安装树复核（`cmake --install build --config Release` 后逐项打勾）：
   - `include/SARibbonCore/`、`include/SARibbonWidgets/`（含 `colorWidgets/` 子目录）
   - 旧路径转发头 `include/SARibbonBar/`（01-S11 产物，复核每个公共头都有转发）
   - `lib/cmake/SARibbon/{SARibbonConfig,SARibbonConfigVersion,SARibbonTargets}.cmake`，`find_package(SARibbon COMPONENTS Core|Widgets)` 可用，target 为 `SARibbon::Core`/`SARibbon::Widgets`
   - `bin/translations/SARibbon_{zh_CN,en_US}.qm`（Qt Linguist Tools 在位时）
   - `share/*_amalgamate` 的处置结果与 S1-3-4 的决定一致
   - `tools/test-find-package/` 在 01-S11 版本上**补一个 `COMPONENTS Core` 最小消费 TU**（只 include core 头、只链 `SARibbon::Core`，证明 core 可独立消费）
4. 文档：根 `build.md` 与 `docs/{zh,en}/build-guide/build-SARibbon.md` 增加"单文件发行（生成方法：tools/ 下 bash Amalgamate.sh；产物不入库）"与"vcpkg/组件化 find_package"两节的 3.0 说明；`docs/{zh,en}/build-guide/i18n.md` 顺手修正示例——现文档用 `translator.load("SARibbon_zh_CN.qm", ":/i18n/")`（Qt 资源路径），与实际交付方式（安装树/构建目录 `bin/translations/` 下的文件路径加载）不符。
5. **tests 链接目标切换**（承接计划 01 S6-6 留给本计划的义务）：`tests/`（01 后 `tests/widgets/`）里 `target_link_libraries(... SARibbonBar)` 全部改为 `SARibbon::Widgets`；兼容别名 `add_library(SARibbonBar ALIAS SARibbonWidgets)` **保留**（对外过渡一个版本周期，01-S6 决定），仅切换仓库内部用法。`git grep -n "SARibbonBar)" tests/` 复核清零（别名定义处除外）。

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
   - 变更：最低要求 CMake 3.16 / Qt 5.15 / C++17；
   - …（面向用户的完整变更清单留给计划 04 定稿，本步先落工程性条目）
   ```
   （最低要求数值以计划 01 S4 实际落地的 `cmake_minimum_required`/`SARIBBON_MIN_QT_VERSION` 为准，发布前复核。）
2. **版本收口清单**（本步逐项核对 = 3.0.0，S3 各子步已 bump 的只复核）：根 `CMakeLists.txt`（01 已改）、重新生成的 `SARibbonBarVersionInfo.h`（或 02 收编后的等价物）、`pyproject.toml`、`pyproject-pyqt6.toml`、`pyqt6/pyproject.toml`、`pyside6/pyproject.toml`、`pyside6/CMakeLists.txt`、`vcpkg.json`。
3. `docs/zh/build-guide/` 与 `docs/en/build-guide/`（**已核实两语言目录同名 5 篇**：`build-3rdparty.md`、`build-SARibbon.md`、`build-instructions.md`、`common-build-errors.md`、`i18n.md`）按三模块更新构建章节；python-guide 的同步已归 S3.6，此处只复核链接不断。

**提交**：`文档：构建指南按三模块重写；changelog 3.0 草稿`

## 6. 完成验收门（= v2 M2 交付判据）

- [ ] `bash tools/Amalgamate.sh`（cwd=tools，`< /dev/null` 非交互）产出 `src/SARibbonCore.h/.cpp` 与 `src/SARibbonWidgets.h/.cpp`；`git ls-files src/` 不含任何 `SARibbon*.h/.cpp` 单文件且 `.gitignore` 生效；`.gitattributes` 冻结行已删；`cmake --install` 在 fresh clone 上通过（无 share/amalgamate 缺文件错误）
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
| Amalgamate.sh 的交互 `read` 或 Windows-only exe 挂死/挡住 CI | S1-2 已做 `[ -t 0 ]` 守卫；amalgamation job 固定 windows-latest；exe 损坏/缺失时按 tools/Amalgamate.md 从上游源码重编 |
| 绑定构建环境本机缺失 | 子步允许"CI dry-run 验证"，但必须在 NOTES.md 显式记录，不许静默跳过 |
| sip 侧源清单陈旧（2.8.0 时代清单缺 2.9.x 新源文件）导致轮子链接失败或功能缺失 | S3.2-2 强制与 `git ls-files` 对齐 + diff 复核进验收门 |
| sipbuild 配置双源（project.py 与 pyproject.toml 并存）行为不明 | 待核实项：先跑 `sip-build --verbose` 确认生效配置；两套同步改，不删任何一个（保守） |
| Qt5/Qt6 sip 工具链行为差异 | 每绑定独立提交；失败回滚该笔不影响主线 |
| CI 矩阵超时 | amalgamation 与 python dry-run 独立 workflow/job，不挂进平台构建的关键路径 |

## 8. 已知偏差

见 [NOTES.md](NOTES.md)。本计划评审时新增待记录项：① publish workflow 历史触发器为 release 而非 tag（S4）；② `-DBUILD_SHARED_LIBS` 对库类型无实效（S5）；③ sip 绑定清单陈旧（S3）；④ 根 CMakeLists `SARIBBON_DOC_FILES` 死变量（S6）；⑤ `docs/*/build-guide/i18n.md` 的 `:/i18n/` 示例路径与实际交付不符（S6-4）。
