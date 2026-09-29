# 第1轮评审记录：计划01

> 评审对象：`plans/3.0/01-infra-restructure.md`（修订前 271 行版本）
> 评审视角：事实准确性与可执行信息完备度
> 评审日期：2026-09-29
> 结论概览：共 67 条记录（错误 25 / 缺失 36 / 存疑 5 / 核实通过无需改动 1），其中 #65~#67 为修订过程中的自审追加发现。所有"错误"与"缺失"已直接修订进 01 文档；"存疑"已在文内标注核实方法或按 NOTES 流程处理。本轮**未运行任何构建**（遵守只读约束），涉及构建行为的判断均以源码/配置实证 + 参考项目（F:\src\3rdparty\qwindowkit、KDDockWidgets）为据。

## 一、v1 悬空引用（1 条总项 + 各处内联）

| # | 位置 | 类型 | 证据 | 处理 |
|---|------|------|------|------|
| 1 | 文档头"设计依据"及 S2/S3/S5/S6.2/S6.3/S11 共 7 处 v1 引用 | 错误（悬空引用） | `git log --all --oneline -- SARibbon-3.0-plan.md` 为空；工作区/历史均无该文件（任务已核实事实） | 已修正：文档头改为 v2 章节引用 + 自包含声明；S2（v1 §7.4→v2 §8-M3 examples/qml）、S3（v1 §3→用户要求+[悬空]标注）、S5（v1 §5.2→新 S5.1~S5.4 实证规格）、S6.2/S6.3（v1 §5.3→S5.2 三段式模板 + S6.9 代码块）、S11（v1 §7.1→v2 §4.4 + 转发头生成代码）全部内联 |

## 二、前置条件与 S1（分支/未跟踪项/测试基线）

| # | 位置 | 类型 | 证据 | 处理 |
|---|------|------|------|------|
| 2 | S1 `git add ... SARibbon-3.0-plan.md` | 错误 | 文件不存在（同上）；`git status --porcelain` 实测未跟踪仅 `SARibbon-3.0-plan-v2.md`、`plans/`、`saribbon-dev-v2.8.0-plus.bundle` | 已修正：add 命令去掉 v1 文件，并注明"不要 add 不存在的 v1 计划"与 bundle 不入库 |
| 3 | §3-P1"仅 4 个已知未跟踪项（两份计划 md…）" | 错误 | 同上，实测 3 项（v1 md 不存在） | 已修正为 3 项并逐项列名 |
| 4 | §3-P2"在 dev 分支" | 错误 | `git branch --show-current` = `v3`；`git rev-parse dev v3` 同为 `7a617fca7a0e...` | 已修正：期望改为"分支 dev 或 v3（同指 7a617fc）"，文档头分支说明同步 |
| 5 | §3-P3 测试基线 | 缺失 | `ls tests/*.cpp \| wc -l` = 25；`grep -c "add_saribbon_test(" tests/CMakeLists.txt` = 26（含 `tests/auto/SARibbonThemePalette/tst_themepalette.cpp`）；`ctest --test-dir` 为 CMake 3.20+ 特性 | 已补充：N₀ 预期 26、auto/ 子目录存在、ctest 版本要求、build.ps1 `-Tests ON`→`-DBUILD_TESTS=ON` 映射（scripts/build.ps1:332） |
| 6 | §3-P4 行数 1863/1425/1993 | 核实通过 | `wc -l` 实测三文件完全一致 | 保留原值，加"2026-09 已核实"注 |

## 三、S2/S3（目录与 submodule 搬移）

| # | 位置 | 类型 | 证据 | 处理 |
|---|------|------|------|------|
| 7 | S2 搬移后 StaticExample 断链 | 错误（遗漏必改点） | `example/StaticExample/CMakeLists.txt:55` `SET(SARIBBON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)`；搬到 `examples/widgets/` 后 `../../src` 解析为 `examples/src`（不存在）。`git grep -n '\.\./\.\./' example` 仅此 1 处 | 已补充为 S2 操作第 4 条：改 `../../../src`，附盘点命令 |
| 8 | S2 操作顺序 | 缺失 | `git mv` 目标目录不存在会报错（需先 `mkdir examples/widgets`）；11 个目录实名需与 `example/CMakeLists.txt` 的 11 个 `add_subdirectory` 对齐 | 已补充：完整 bash 循环（11 个目录实名枚举）+ CMakeLists 两级改建顺序 |
| 9 | S3"删除空目录 src/SARibbonBar/3rdparty/" | 错误 | `ls src/SARibbonBar/3rdparty` = `CMakeLists.txt`、`cmd-build-example.sh`、`qwindowkit`（非空！前两者是 QWK 辅助构建脚本） | 已修正：三项整体 git mv + `rmdir`；新增第 2 条修 `3rdparty/CMakeLists.txt` 内 `SARIBBON_BIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../` → `../`（否则 bin 路径越出仓库） |
| 10 | S3 submodule 细节 | 缺失 | `.gitmodules` url=`https://github.com/czyt1988/qwindowkit`（fork，submodule.md 却写 stdware）；`git submodule status` 全 hash `-f93657fa82bdd37dba68ea962d5e9b2cf4fd4d60`（`-`=未初始化） | 已补充：全 hash、fork 说明、`.gitmodules` diff 验证步骤、submodule.md 修正挂到 S12 |

## 四、S4（根 CMakeLists 重写）

| # | 位置 | 类型 | 证据 | 处理 |
|---|------|------|------|------|
| 11 | S4.1/§1.2/S12.2 floor 3.16 | 错误 | 本计划与 README R2 的验证命令 `ctest --test-dir` 需 CMake ≥ 3.20；`src/SARibbonBar/CMakeLists.txt:146` 已用非 IMPORTED 的 `SARibbonBar::SARibbonBar` ALIAS（`::` 别名旧版不支持）；QWK 自身 floor 3.19（其 CMakeLists.txt:1）；`CMakePresets.json` version 6 + cmakeMinimumRequired 3.25 | 已修正：floor 改 3.21，四条理由写入 S4 第 1 条 |
| 12 | S4.2 版本升 3.0.0 的下游断裂 | 缺失 | `SARibbonBarVersionInfo.h.in` 用 `@SARIBBON_VERSION_MAJOR/MINOR/PATCH@` 与 `#cmakedefine SARIBBON_VERSION`；`src/SARibbonBar/CMakeLists.txt:2` 与 10 个 example 消费 `${SARIBBON_VERSION}` | 已补充：project() 后必须派生 4 个旧变量（代码块），vcpkg.json(2.8.0)/pyproject/changlog 元数据归计划 03/04 的注记 |
| 13 | S4.5 删标准手术后的过渡窗口 | 缺失（会破坏 R3 绿点） | 删除 L79-103 后，S4~S6 期间旧模块 CMakeLists 无任何 C++ 标准设定（标准只由根提供），Qt6 下掉回编译器默认 → S4 提交点无法构建 | 已补充：S4 保留全局 `CMAKE_CXX_STANDARD 17` 两行过渡，S6 末尾（sa_add_library 全覆盖后）删除；风险表加对应行 |
| 14 | S4.10"删除 CMAKE_INCLUDE_CURRENT_DIR" | 错误 | 根 CMakeLists.txt 全文（246 行）无此项；实际在 `src/SARibbonBar/CMakeLists.txt:5` | 已修正：表述改为"S6 改写模块 CMakeLists 时删除" |
| 15 | S4 未给出逐块处置 | 缺失 | 根文件实存但原稿未提的逻辑块：vcpkg 检测 L32-43、frameless Qt 门槛块 L58-77（内部变量 `_SARIBBON_USE_FRAMELESS_LIB` 被模块 L165/L203 消费，不可改名）、POSTFIX 块 L105-121、MSVC `/wd4819`+`_HAS_AUTO_PTR_ETC=1` L123-128、bin_qtX 逻辑 L131-148、QWindowKit 探测 L151-194、`include(cmake/WinResource.cmake)` L198（`create_win32_resource_version` 被模块 L357 调用）、configure_file VersionInfo L200-206（生成到源码树且被 git 跟踪）、`enable_testing()` L229、死变量 `SARIBBON_DOC_FILES` L240-244（全仓无 install 消费） | 已补充：新增 S4.1 逐块处置表（保留/改/删，全部带行号与理由） |
| 16 | S4 与 build.ps1 选项名耦合 | 缺失 | `scripts/build.ps1:322-333` configure 传 `-DBUILD_TESTS=$Tests`（其余 4 个 `-DSARIBBON_*` 与新选项名一致） | 已补充为 S4 第 11 条：脚本同步改 `-DSARIBBON_BUILD_TESTS` |
| 17 | S4 configure_file 路径与 S6 时序 | 缺失 | S4 时目录尚未搬移，先改 VersionInfo 路径会破坏 S4 提交点 | 已在 S4.1 表明确"S4 不动路径、S6 搬目录时同步改"，S6 第 1 条呼应 |
| 18 | S4 验证命令 | 补充 | 原稿"编译可暂缓"与 R3 绿点纪律矛盾 | 已修正：S4 提交点要求配置+编译全绿（新根与旧 src 兼容性已逐变量核对），build-tmp 用后即删 |

## 五、S5（SARibbonUtils.cmake）

| # | 位置 | 类型 | 证据 | 处理 |
|---|------|------|------|------|
| 19 | S5"现有 16 行宏保留备用" | 核实通过 + 补充 | `cmake/SARibbonUtils.cmake` 实测 16 行、宏名 `saribbon_set_bin_name` 属实；但全仓零调用（死代码），宏体含外来残留（`DA_MIN_QT_VERSION`、`endmacro(damacro_set_bin_name)` 名不符） | 已补充 S5.1：死代码定性 + 处置建议（删除或保留不调用） |
| 20 | S5 参考实现定位 | 错误（定位偏差）+ 缺失 | `qwk_add_library` 定义在 `F:\src\3rdparty\qwindowkit\src\CMakeLists.txt:38-124`（**不在 qmsetup/**）；qmsetup 是 QWK 的未初始化 submodule（`ls qmsetup` 为空），`qm_export_defines/qm_configure_target/qm_sync_include` 实现本地不可读；网络获取超时 | 已补充 S5.1：以调用点语义 + QWK 头文件宏形态反推规格，声明 sa_* 全自实现、不依赖 qmsetup；存疑项见 #47 |
| 21 | S5 导出宏三段式无准确形态（v1 §5.3 悬空） | 缺失 | `qwindowkit/src/core/qwkglobal.h:12-21`（`QWK_CORE_STATIC`→空 / `QWK_CORE_LIBRARY`→`Q_DECL_EXPORT` / else→`Q_DECL_IMPORT`）；`qwkwidgetsglobal.h:9-19` 同构；KDDockWidgets `src/docks_export.h` 同款 | 已补充 S5.2：真实宏代码引用 + SARibbon 三模块 PREFIX/宏名映射表 + CMake 侧定义方式 |
| 22 | S5 静态导出宏语义 | 错误（与现状不符） | 现静态构建为 PRIVATE+INTERFACE `SA_RIBBON_BAR_NO_EXPORT`（`src/SARibbonBar/CMakeLists.txt:186-192`，≈PUBLIC）；原稿"静态时 PUBLIC <PREFIX>_STATIC"方向对但未给旧宏衔接 | 已修正/补充：规格明确 PUBLIC，旧宏兼容全部放头文件层（S6.9 代码块 C） |
| 23 | S5 include 传播设计 | 错误（与仓库现状冲突，影响面最大） | 现状 `src/SARibbonBar/CMakeLists.txt:221-224` 为 PUBLIC `$<BUILD_INTERFACE:源目录>`；`git grep` 实测 tests+example 平铺 include（`"SARibbonBar.h"` 形式）共 **190 处**；原稿"PRIVATE 模块源目录"会全部打断，违背"行为零变化" | 已修正：新增 S5.5 决策——源目录 PUBLIC（平铺兼容）+ 同步目录 PUBLIC（`<SARibbonWidgets/...>`）双通道；命名空间化归计划 03/04 |
| 24 | S5 无可落地代码 | 缺失 | — | 已补充 S5.3 `sa_add_library` 完整骨架（含 QT_LINKS/LINKS 正则改写、EXPORT_NAME 剥离、别名、install EXPORT SARibbonTargets、win32 rc 并入）与 S5.4 `sa_sync_include` 骨架（GLOB 仅限头同步的豁免说明、配置期复制的局限说明） |

## 六、S6（三模块落地）

| # | 位置 | 类型 | 证据 | 处理 |
|---|------|------|------|------|
| 25 | S6.2 PIMPL 宏行号"84-105" | 错误 | `SARibbonGlobal.h` 实测：PIMPL 区 L20-184（含双语注释），宏本体 83-88（DECLARE_PRIVATE）、103-109（DECLARE_PUBLIC）、122-124（IMPL_CONSTRUCT）、137-139/152-154/167-169/182-184（SA_D/DC/Q/QC）；84-105 只覆盖到一半 | 已修正：给出完整行号；明确"宏名不变、只换定义文件"；`sa_as_const`(L274-283) 一并 move，`SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE`(L270-272) 留 widgets |
| 26 | S6.3 转发头方案无内容 | 缺失 | 38 个文件 include SARibbonGlobal.h（`git grep -rl` 实测）；原 Global.h:6 include VersionInfo、:7 前置声明 QWidget | 已补充 S6.9 代码块 B/C：转发头全文（含 VersionInfo include 保持、枚举留守声明）与 SARibbonWidgetsGlobal.h 全文（三段式 + 旧宏映射 + `SA_RIBBON_EXPORT` 等价定义 + 宏展开顺序说明） |
| 27 | S6.4"SA_COLOR_WIDGETS_EXPORT" | 错误（宏名不实） | `colorWidgets/SAColorWidgetsGlobal.h:97-111` 实名 `SA_COLOR_WIDGETS_API`（守卫 `SA_COLOR_WIDGETS_NO_DLL`/`SA_COLOR_WIDGETS_MAKE_LIB`）；API 宏 5 文件在用、`SA_COLOR_WIDGETS_DECLARE_PRIVATE/PUBLIC`（QScopedPointer 版）5 文件在用 | 已修正：实名 + 替换代码块 + 保留范围（版本宏/PIMPL 宏/sacolor_as_const 不动） |
| 28 | S6.6 兼容别名不全 | 错误 | tests 链裸 `SARibbonBar`（tests/CMakeLists.txt:11）；10 个 example 链 `SARibbonBar::SARibbonBar` 且 `if(NOT TARGET SARibbonBar)` 探测（MainWindowExample/CMakeLists.txt:25-30 等，grep 实证 10 处） | 已修正：两个别名都必须加 |
| 29 | S6.1 源清单变量名 | 核实通过 + 补充 | `SACOLOR_DIR/SACOLOR_HEADER_FILES/SACOLOR_SOURCE_FILES`(L11-24)、`SARIBBON_HEADER_FILES`(L33-75)、`SARIBBON_SOURCE_FILES`(L79-119)、`SARIBBON_RESOURCE_FILES`(L123-125)、`SARibbonQt5Compat.hpp` 存在（且在清单外重复列于 L143）；另发现 `SARibbonMdiControlsStyle.h` 在目录且有 .cpp（L83）但**不在**头文件清单（现状即不安装） | 已补充：骨架代码块逐变量引用 + 行号对照；MdiControlsStyle.h 差异按"维持现状记 NOTES"处理；原文"井 qrc"错字修正 |
| 30 | S6.2 core 纯头 target 无法建 SHARED 库 | 缺失 | add_library 无源文件在 MSVC 下报错/空 DLL 告警（CMake 通识 + 原稿 S6.5 对 qml 已有同类担忧） | 已补充：core 加 1 行占位 .cpp，计划 02 被真实源取代 |
| 31 | S6.2 Qt5Compat 搬移后平铺 include 解析 | 缺失 | 38 个文件平铺 `#include "SARibbonQt5Compat.hpp"`；搬入 core 后同步目录形态是 `include/SARibbonCore/SARibbonQt5Compat.hpp`，平铺名无法从同步根解析 | 已补充：widgets 侧保留同名转发头 `#include <SARibbonCore/SARibbonQt5Compat.hpp>` |
| 32 | S6.1 frameless/翻译/win32-rc/install 块随迁 | 缺失 | 模块 CMakeLists L165-169（QWindowKit find+link）、L203-219（FRAMELESSHELPER/SNAP_LAYOUT PUBLIC 宏）、L225-229（NOMINMAX）、L234-288（LinguistTools 全套 + qm 安装/复制）、L289-352（install/包配置）、L356-363（win32 rc） | 已补充：骨架注释逐块给出行号与去向（rc 并入 sa_add_library、install 归 S5/S11、旧包配置移 src 层） |
| 33 | S6.5 qml 空骨架 | 补充 | 无源文件 target 必然报错，原稿"报错才降级"顺序反了 | 已修正：默认路径 = 只建目录 + Global 头 + message 占位 CMakeLists，不建 target |
| 34 | S6.7 src/CMakeLists.txt | 缺失 | 现内容仅 `add_subdirectory(SARibbonBar)` + 注释掉的 DesignerPlugin；三模块共用 export set 时 `install(EXPORT)` 只能写一次 | 已补充：完整代码块（core/widgets/qml gate + install(EXPORT SARibbonTargets)） |
| 35 | S6.8 i18n 盘点结论 | 核实通过 | `git grep "SARibbonBar/i18n\|SARibbonBar/resource"` 构建系统内零命中（.ts 用模块相对路径），命中仅 docs/ 8 文件 + AGENTS.md | 已补充实测结论，docs 归计划 04 |

## 七、S7（tests/examples 适配）

| # | 位置 | 类型 | 证据 | 处理 |
|---|------|------|------|------|
| 36 | S7"24 个测试源" | 错误 | 实测 25 个顶层 .cpp + `tests/auto/SARibbonThemePalette/tst_themepalette.cpp`，注册 26 个（v2 §7.3 也写"25 个单测"，与 24 均不符） | 已修正为 25+1=26，并给出复核命令 |
| 37 | S7 搬移命令漏 tests/auto | 错误 | `add_saribbon_test(SARibbonThemePaletteTest auto/SARibbonThemePalette/tst_themepalette.cpp)`（tests/CMakeLists.txt:61） | 已补充 `git mv tests/auto tests/widgets/auto` |
| 38 | S7"include 路径改 ../src/widgets 临时" | 错误 | 从 `tests/widgets/` 出发 `../src` = `tests/src`（少一级） | 已修正：首选删除 target_include_directories（靠 S5.5 PUBLIC 传播）；临时路径为 `../../src/widgets` |
| 39 | S7 测试源内硬编码路径 | 错误（遗漏必改点） | `tests/ThemeCoverageTest.cpp:47-48` `absoluteFilePath("../src/SARibbonBar/resource")`；`git grep -n "src/SARibbonBar" tests` 唯一命中 | 已补充为操作第 3 条：改 `"../../src/widgets/resource"`（QT_TESTCASE_SOURCEDIR 随 CMakeLists 位置自动变化，相对段需加一级） |
| 40 | S7 add_saribbon_test 函数迁移方式 | 缺失 | 函数定义在 tests/CMakeLists.txt:7-45（链接裸 SARibbonBar、POST_BUILD 复制 `$<TARGET_FILE:SARibbonBar>`、`RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests`、TIMEOUT 120） | 已补充：函数整体移入 tests/widgets/CMakeLists.txt、tests/CMakeLists.txt 重建内容、函数体内 3 处适配点 |
| 41 | S7 examples"二选一后统一" | 缺失（决策未定） | 10 个 example 链接 `SARibbonBar::SARibbonBar` + find_package fallback；190 处平铺 include | 已修正：明确本计划零改动（靠 S6.6 别名 + S5.5 传播 + S11.2 兼容包），切换归计划 03 |

## 八、S8（amalgamate）

| # | 位置 | 类型 | 证据 | 处理 |
|---|------|------|------|------|
| 42 | S8 只改 OPTS | 错误（重大遗漏） | `tools/amalgamate/SARibbonAmalgamTemplatePublicHeaders.h` 含 **46 处**、`SARibbonAmalgamTemplate.cpp` 含 **43 处** `../../src/SARibbonBar/...` 硬编码 include（git grep 计数实证）；OPTS 现值 `-i "../src/SARibbonBar" -i "../src/SARibbonBar/colorWidgets" -w "*.cpp;*.h;*.hpp" -s` 核实属实 | 已补充：通用 sed 批量替换（转发链自动拉入 core 头，无需手工插入）+ `_amalg_include/SARibbonCore` 命名空间镜像步骤（见 #65） |
| 43 | S8 运行方式 | 缺失 | `Amalgamate.sh` 用 `./Amalgamate.exe`、`DEST=../src`（须在 tools/ 下运行）；末尾 `read -n 1` 交互等待会挂起自动化；CRLF awk 转换属实 | 已补充：`cd tools && bash Amalgamate.sh < /dev/null`；生成后 diff 预期说明 |
| 44 | S8 模板头部兼容宏 | 核实通过 | 两模板均先定义 `SA_RIBBON_BAR_NO_EXPORT`、`SA_COLOR_WIDGETS_NO_DLL` 再 include——与 S6.9 代码块 C 的旧宏映射衔接 | 已补充衔接说明（保留不动） |
| 45 | tools/qrc_*.cpp 旧路径 | 缺失（影响验收门） | `tools/qrc_SARibbonResource_Datas.cpp` 等含 32 处 `src/SARibbonBar`，实测全部位于 rcc 生成注释（"Created by: The Resource Compiler for Qt version 5.14.2"），资源字节与路径无关 | 已补充：无需再生的结论 + §6 grep 门对其豁免 |

## 九、S9/S10（纯净性与 CI）

| # | 位置 | 类型 | 证据 | 处理 |
|---|------|------|------|------|
| 46 | S9 清单与"必绿" | 核实通过 + 补充 | 禁 include 清单与 v2 §6.2 一致；`grep -n "QWidget\|qApp\|QApplication\|QLayout\|QStyle\|QAction" src/SARibbonBar/SARibbonQt5Compat.hpp` 零命中（仅 QtCore/QtGui include） | 已补充：预核实证据、`python3` 命名统一（Linux runner 无 `python`）、前缀匹配规则细化 |
| 47 | S10.1 选项名 | 补充（实证行号） | 6 个 workflow configure 行同式：`-DBUILD_SHARED_LIBS=${{matrix.shared}} -DSARIBBON_BUILD_EXAMPLES=OFF -DBUILD_TESTS=ON`（cmake-linux-qt6.8.yml:37、cmake-win-qt6.8.yml:35 等）；`BUILD_SHARED_LIBS` 不被根 CMakeLists 消费（库型由 SARIBBON_BUILD_STATIC_LIBS 决定） | 已补充：只改 BUILD_TESTS 一处×6、purity step 插入位置（Configure 前）、BUILD_SHARED_LIBS 无效传参记 NOTES |
| 48 | S10.2"若无 matrix 则复制新文件" | 错误（前提不实） | `cmake-linux-qt6.8.yml:13-19` 实有 matrix（ubuntu_version/qt_version/shared）；win/mac 同款 | 已修正：删备选路径，直接加 `widgets: [ON, OFF]` 轴并联动 `-DSARIBBON_BUILD_TESTS=${{matrix.widgets}}`（core-only 时 tests 无法链接 widgets） |
| 49 | S10.3 submodule checkout | 错误（前提不实） | 6 个 yml 的 `actions/checkout@v4` 均无 `submodules:` 参数，CI 从不初始化 submodule、从不构建 frameless | 已修正：改为事实陈述 + vcpkg preset 本机验证路径（preset `vcpkg-msvc-x64-release` 现成） |
| 50 | S10.4 publish-python-bindings.yml | 错误（触发器不实） | 该文件 :3-6 实际触发器 = `release: types:[published]` + `workflow_dispatch`，**无 push/tag**；且 release 触发执行默认分支（master）版本，dev-3.0 上修改无效果 | 已修正：本计划不改该文件，NOTES 记录核实结论（防止执行 agent 照原稿"删除 push/tag"空转或误改） |

## 十、S11/S12、验收门与风险表

| # | 位置 | 类型 | 证据 | 处理 |
|---|------|------|------|------|
| 51 | S11 现状未盘点 | 缺失 | 现有安装规则全部在 `src/SARibbonBar/CMakeLists.txt:289-352`（headers→include/SARibbonBar、amalgam→share/SARibbonBar_amalgamate、包 SARibbonBarConfig/Version/Targets→lib/cmake/SARibbonBar、namespace SARibbonBar::）；`SARibbonBarConfig.cmake.in` 存在，`SARibbonConfig.cmake.in` 不存在；`tools/test-find-package/` 不存在 | 已补充：现状盘点 + 新组件化 SARibbonConfig.cmake.in 骨架（含 COMPONENTS Widgets/Qml 映射与 check_required_components）+ install(EXPORT) 归位说明 |
| 52 | S11 旧包兼容 | 缺失 | 10 个 example 独立构建 fallback `find_package(SARibbonBar REQUIRED)` | 已补充 S11.2：旧 SARibbonBarConfig 保留为薄壳（INTERFACE IMPORTED 转发到 SARibbon::Widgets）一个版本周期 |
| 53 | S11 转发头无生成方案（v1 §7.1 悬空） | 缺失 | v2 §4.4 有转发头示例（include/SARibbonBar/SARibbonBar.h → include/SARibbonWidgets/SARibbonBar.h） | 已补充 S11.3：按 `SARIBBON_HEADER_FILES` foreach file(WRITE) 生成 + install(DIRECTORY) 代码块（含 colorWidgets 子目录说明） |
| 54 | S12 文档清单不全 | 缺失 | AGENTS.md 6 处旧路径（L5/L15/L17/L18/L19/L36）；build.md 选项表 L120-127（`BUILD_TESTS` 在 L127）、L11 frameless Qt 版本注；`changlog.md` 拼写确认（sic，非 changelog）；`submodule.md` L20/L29 旧路径 + url 与 .gitmodules 不符；docs/ 8 文件旧路径 | 已补充：逐文件行号级清单；docs/ 明确 defer 计划 04 并记 NOTES |
| 55 | §6 验收门 `--follow src/widgets/SARibbonBar.cpp` | 存疑→澄清 | `src/SARibbonBar/SARibbonBar.cpp` 是普通类实现源文件（在 SARIBBON_SOURCE_FILES L85），**不是**禁改的 amalgamate 产物（产物为 `src/SARibbon.cpp`）——用它验证 --follow 合法有效 | 已补充澄清注（防止执行 agent 因 AGENTS.md 禁改条款误判该文件），并加 2 个抽查文件 |
| 56 | §6 grep 门无排除清单 | 错误 | `git grep -n "src/SARibbonBar" -- ':!plans' ':!docs'` 实测残留：pyproject.toml/pyproject-pyqt6.toml/pyqt6/pyproject.toml（各 86 处）、MANIFEST.in(1)、pyside6/CMakeLists.txt(1)、tools/qrc_*.cpp(32，注释)、AGENTS.md(6)、submodule.md(2)——按原稿命令永不归零 | 已修正：完整排除清单（Python 打包归计划 03、qrc 快照注释豁免）+ 期望零输出 + "src/SARibbon.cpp 与 tools/amalgamate 不在豁免、S8 后必须归零"的责任界定 |
| 57 | §6 core-only 命令 | 补充 | 原命令缺 `CMAKE_PREFIX_PATH` 与 `-DSARIBBON_BUILD_TESTS=OFF` | 已修正命令 |
| 58 | §7 风险表"R5 私有 include 方案" | 错误（引用不实） | README R5 是术语表（Step A/Step B/黄金测试等），无"私有 include 方案" | 已修正：改引本计划 S5.5 决策；另补 3 行新风险（过渡窗口标准缺失、build.ps1/CI 选项名耦合、版本号外溢） |
| 59 | §4"R1–R4 全部适用" | 错误（不全） | README 实有 R1–R6；R6（dev-3.0 不与 dev 交互）与本计划直接相关 | 已修正为 R1–R6 |
| 60 | §8 已知偏差 | 缺失 | 本轮新确认的基线事实（v1 不存在、分支 v3、未跟踪 3 项、测试 26、SA_COLOR_WIDGETS_API 实名、README 快照"约 24 个"不确） | 已补充 5 条（README 非本轮修改权限，差异登记于此） |

## 十一、存疑项（文内已标注核实方法）

| # | 位置 | 类型 | 说明 | 处理 |
|---|------|------|------|------|
| 61 | S4 floor 依据之一"`::` 别名需 3.18+" | 存疑 | 本轮禁跑 cmake，无法实测最低支持版本（文档记忆为 3.18 放开非 IMPORTED `::` ALIAS）；但 floor 3.21 由 `ctest --test-dir`≥3.20 硬支撑，结论不受此存疑影响 | 文中以"3.18 起明确放开；现仓库 floor 3.15 却已使用，说明实际环境远高于 floor"表述，不依赖精确版本号 |
| 62 | S4.2 版本号 3.0.0 提升时机 | 存疑 | v2 §8 到 M4 才发布 3.0.0；M0 即提 CMake 工程版本会使 `SA_RIBBON_BAR_VERSION_MAJ=3` 提前生效（运行时可见） | 维持原稿决策（dev-3.0 分支上先行），风险表 + NOTES 跟踪；若后续轮次认为应在 M4 才提版，仅需改 S4 第 2 条 |
| 63 | S10 CI 的 `BUILD_SHARED_LIBS` 传参 | 存疑 | CI 传但根 CMakeLists 从不消费（历史无效参数） | 保留不动（保守），NOTES 登记；是否删除/映射归 S10 执行者按 R4 决断 |
| 64 | qmsetup 实现细节（qm_export_defines/qm_sync_include 源码） | 存疑 | 参考机 `F:\src\3rdparty\qwindowkit\qmsetup` 为未初始化 submodule（空目录），网络获取超时；规格由 qwk_add_library 调用点（src/CMakeLists.txt:38-124）与 qwkglobal.h/qwkwidgetsglobal.h 宏形态反推 | S5.1 已声明"sa_* 全自实现、不依赖 qmsetup"；如需逐行对照，核实命令：`git -C F:\src\3rdparty\qwindowkit submodule update --init qmsetup` |

## 十二、修订过程中进一步发现的问题（round-1 自审，已一并修入 01 文档）

| # | 位置 | 类型 | 证据 | 处理 |
|---|------|------|------|------|
| 65 | S8 × S6.9 转发头：`<SARibbonCore/...>` 在 amalgamate 场景不可解析 | 缺失 | Amalgamate 工具语义（tools/Amalgamate.md）：include 只在"同目录或 -i 路径"命中时内联，否则**原样保留**；`SARibbonCore/` 目录布局只存在于 build 树同步目录与安装树，源码树目录名是 `src/core` → 产物将残留无法编译的 `#include <SARibbonCore/...>`，StaticExample 断 | 已修正 S8：新增 `_amalg_include/SARibbonCore` 镜像步骤 + OPTS 加 `-i "_amalg_include"` + 运行后清理；并说明 include guard 对重复内联的兜底 |
| 66 | S6.9 代码块 A：`SARibbonCoreGlobal.h` include 了 `SARibbonCoreConfig.h` | 缺失（会在源码树/amalgamate 场景断裂） | config 头由 `configure_file` 生成到 `${CMAKE_BINARY_DIR}/include/SARibbonCore/`（S6.2），源码树 `src/core/` 下无此文件；QWK 同款做法是 qwkconfig.h 不被 qwkglobal.h include（qwkglobal.h:1-21 无 config include，实证） | 已修正代码块 A：删除该 include，改为注释说明"需要 feature 开关的 TU 显式 include `<SARibbonCore/SARibbonCoreConfig.h>`（计划 02 起）" |
| 67 | S6.4 D4 代码块：colorWidgets 引 `"SARibbonWidgetsGlobal.h"` 平铺形式 | 缺失（安装树断裂） | 安装树中该头位于 `include/SARibbonWidgets/colorWidgets/`，消费者的 include 根是 `include/`——平铺名无法从根解析（`<SARibbonWidgets/...>` 形式则又需给 amalgamate 加第二层镜像） | 已修正为 `#include "../SARibbonWidgetsGlobal.h"`，并在文中列出源码树/同步目录/安装树/amalgamate 四场景的解析验证 |

## 本轮未解决、建议后续轮次关注

1. **plans/3.0/README.md 快照表数据不确**："tests/*.cpp 约 24 个"实为 25 个顶层 + auto/ 1 个（注册 26）；"SARibbonGlobal.h PIMPL 宏在 :84/:104"实为 L20-184 宏区（本体 83-184）。README 非本轮修改权限，建议下轮统一修订。
2. **计划 02/03/04 可能存在同类问题**：本轮仅核查 01。已知线索：03 的 P3 引用"计划 01 S8 的脚本生成（含 core 骨架）"与新 S8 一致（无需改）；但 02/03/04 中的 v1 引用、测试计数、宏实名（SA_COLOR_WIDGETS_API）、include 传播假设（190 处平铺）需按本轮同标准复核。
3. **S11 兼容薄壳包的 IMPORTED 转发写法**（`add_library(SARibbonBar::SARibbonBar INTERFACE IMPORTED)` + 链接 `SARibbon::Widgets`）与 `install(EXPORT)` 生成的 Targets 文件共存时的导出/安装组合，本轮禁跑构建未实证，S11 执行时需按 R4 验证并记录。
4. **S10.2 core-only 矩阵的 tests 联动**（`-DSARIBBON_BUILD_TESTS=${{matrix.widgets}}`）是简化处理；若希望 widgets=OFF 时仍跑 core 自身测试（计划 02 引入 tests/core 后），需在计划 02 的 CI 步骤重新设计矩阵轴。
5. **tools/qrc_*.cpp 的再生流程无文档**：rcc 生成参数（Qt 版本、qrc 路径、输出名）未记录在任何文档；计划 03 改造 amalgamate 时建议补"资源快照再生说明"，并顺带清掉注释里的旧路径。
6. **版本号元数据多处漂移**：vcpkg.json 写 2.8.0、CMake 将写 3.0.0、pyproject/changlog 停在 2.9.5——计划 03/04 需要一张"版本单一来源"处置表。
