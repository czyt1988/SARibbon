# Round 1 评审记录：跨文档一致性与事实准确性

> 日期：2026-09-29
> 评审 agent 视角：跨文档一致性与事实准确性（第 1 轮，共 5 个并行评审之一）
> 本 agent owns：`plans/3.0/README.md`、`plans/3.0/NOTES.md`、`SARibbon-3.0-plan-v2.md`、本文件
> 核实基线：HEAD `7a617fc`（dev/v3/origin/dev 同指）；参考副本 `F:\src\3rdparty\qwindowkit`、`F:\src\3rdparty\KDDockWidgets`
> 约定：所有修正均有命令输出或 文件:行 证据；不确定处标"待核实"。01~04 由并行 agent 修订，本文件只记录、不改动它们。

## 一、本 agent 修改清单（README.md / NOTES.md / plan-v2.md）

### README.md

| 位置 | 问题类型 | 证据 | 处理 |
|------|---------|------|------|
| 头部基线行 | 信息不全 | `git branch --show-current`→`v3`；`git rev-parse dev v3 origin/dev` 同为 `7a617fca...` | 注明 dev/v3/origin/dev 同指 7a617fc、当前 checkout 在 v3、v1 计划未归档 |
| 计划清单表"出口判据" | 与验收门不完全对齐 | 01 §6 含 find_package 冒烟/截图；02 §6 含度量对照表/6 截图；03 §6 含 ctest==N₀；04 §6 含 tag/Release | 四行摘要补齐相应判据（以四份文档为准，只增不改语义） |
| R2 构建验证命令 | 信息不全（执行 agent 会踩坑） | `scripts/build.ps1` param 块 :14-34（action 位置参数 + `-Tests/-Examples` 等 ValidateSet ON/OFF）、:280-304（clean 时 exe 被锁 `Read-Host` 交互）、:67-69（Qt 探测按目录名字符串降序）、:332（传旧选项名 `-DBUILD_TESTS`）；本机 cmake/ctest 3.31.11（`--test-dir` 需 ≥3.20） | 补"已核实的脚本行为与注意事项"6 条：参数形态合法、rebuild 含 install、锁定时交互挂起风险、Qt 探测字符串排序巧合选中 6.7.3、build.ps1 仍传旧选项名、ctest 用法正确 + N₀=26 |
| R4 已知偏差 | 信息不全 | v1 缺失（`git log --all -- SARibbon-3.0-plan.md` 空）；分支 v3；编码扫描（全仓仅 `tools/Amalgamate.sh` 非 UTF-8）；tests=26 | 追加 4 条已知偏差（对应 NOTES B6/B7/B4/B8） |
| R5 术语表"黄金几何测试" | 交叉引用错误 | 02 文档：黄金测试在 S5.0（录制）/S5.2（tst_panelLayoutEngine）/S8（补全），S6 是 Category 引擎 | "见计划 02 S6"改为"S5.0 录制、S5.2/S6/S7 三引擎测试、S8 补全" |
| R6 双分支 | 信息不全 | 02 S5/S6/S7 复用 Step A/B 术语；四文档均用"计划 0X S<n>"引用格式 | 补术语一致性说明（Step A/B 定义以 v2 §3.4.2 为准；S 编号引用约定） |
| 新增"计划文档自包含性说明"节 | 缺失（任务 A4） | v1 文件不存在（ls + git log 双重核实）；各计划 v1 引用清单（grep） | 新节：v1 缺失事实与证据、悬空引用三条处理约定、修订历史（round1 → reviews/round1/） |
| 快照表"测试"行 | 事实错误 | `ls tests/*.cpp \| wc -l`=25；`tests/auto/SARibbonThemePalette/tst_themepalette.cpp`；`tests/CMakeLists.txt:47-72` 共 26 个 `add_saribbon_test()` | "约 24 个"改为"顶层 25 + auto/ 1 = 26 项注册（N₀=26）"，并补机制细节（链接/include/POST_BUILD 仅动态库/TIMEOUT 120/输出目录） |
| 快照表"i18n/资源"行 | 事实错误 | `find . -name "*.qrc"`：qrc 在 `src/SARibbonBar/SARibbonResource.qrc`，resource/ 内无 qrc（含 png/svg/qss/palettes/templates）；lupdate/lrelease 在 `src/SARibbonBar/CMakeLists.txt:253/260` | 修正 qrc 位置并补 resource/ 实际内容与 lrelease 位置 |
| 快照表"amalgamate"行 | 信息不全 | `tools/Amalgamate.sh:4` OPTS 全串（含 `-w "*.cpp;*.h;*.hpp" -s`）、:5-6 用 `./Amalgamate.exe`、:8-20 CRLF 转换；`file`/python decode：该脚本 GBK；`ls tools/amalgamate/` 4 个模板 | 补全 OPTS、运行方式、模板目录、**脚本自身 GBK 编码**（关联 R1 禁转码规则） |
| 快照表"构建脚本"行 | 信息不全 | build.ps1 全文（395 行） | 补行数并指向 R2 详注 |
| 快照表"现有 CMake"行 | 信息不全/精度不足 | 根 `CMakeLists.txt`（246 行）:5 floor 3.15、:7-13 版本 2.9.5、:47 MIN_QT 5.12、:93/:100/:124 flags 手术、:127 `_HAS_AUTO_PTR_ETC=1`、选项实名清单（含无前缀 `BUILD_TESTS`）；`cmake/SARibbonUtils.cmake` 16 行（`wc -l`=15，末行无换行符）、宏无调用者（grep 全仓仅定义处）、`endmacro(damacro_set_bin_name)` 名不符 | 逐项补行号与实名；标注死代码宏（对 01 S5"保留备用"提出事实修正） |
| 快照表"qwindowkit"行 | 信息不全 | `git submodule status`→`-f93657fa82bdd37dba68ea962d5e9b2cf4fd4d60`；`ls -A` 目录空；QWK 副本 `src/CMakeLists.txt:38` `macro(qwk_add_library ...)` | 补全 SHA、参考副本路径、QWK 建库宏实名 |
| 快照表"Python 绑定"行 | 信息不全 | `ls sip pyqt6 pyside6 tools`；`grep name pyproject*.toml`→PyQtSARibbon/PyQtSARibbon/PySideSARibbon；`pyexamples/*/*.py` import 行 | 补 sip 文件已单 n、发行包名、import 形式、pyexamples 结构、project.py/MANIFEST.in |
| 快照表"示例"行 | 信息不全 | `ls example/`：11 个子目录实名 | 列全 11 个目录名 |
| 快照表新增"git 基线状态/本机环境/布局三巨头/全局枚举/编码状况"行 | 信息不全 | git status/rev-parse；D:\Qt 目录；`grep -n` 度量/枚举/PIMPL 宏行号（66/89/92/96/108/114、197/220/245、84/104/123/138/153/168/183）；关键函数行号（updateGeomArray:795、recalcExpandGeomArray:1204、setGeometry 守卫:1831、updateGeometryArr:431、categoryContentSize:408、scrollByAnimate:971、layoutTitleRect:1242）；编码扫描 | 新增/扩充相应行，PIMPL 宏族标注完整范围 :83-184（供 01 S6 修正） |
| 快照表核实无误的行 | — | 三巨头 1863/1425/1993（wc -l）；BarLayout.h 66/89/92/96/108/114；PanelItem.h:36/:59-60；Global.h:197/:220/:245/:84/:104；Panel.h:99；makeColorVibrant 位置与使用者；CI 8 文件清单 | 保持原样（已逐项复核为准确） |

### NOTES.md

| 位置 | 问题类型 | 证据 | 处理 |
|------|---------|------|------|
| B1 | 精度不足 | `git log --all --grep="Pannel"` 命中多条（f553de7 及更早双 n 时代提交）；`git grep -rln "SARibbonPannel" -- .` 跟踪文件中仅 `changlog.md`（计划文档未入库，不在 git grep 范围） | 证据表述精确化：注明多命中、跟踪文件仅 changlog.md、计划文档中旧拼写的实际位置（v2 :85/:451/:592、01 §8、02 §8） |
| B2 | 事实错误 | `ls src/SARibbonBar/`：目录**现仍为** `src/SARibbonBar/`（01 S6 后才改 `src/widgets/`）；Panel 系 8 文件实名核实 | 删去"（现 `src/widgets/`）"误导表述，改为"现仍为 src/SARibbonBar/，计划 01 S6 后变为 src/widgets/"，补 8 文件清单 |
| B3 | 信息不全 | `git submodule status` 全 SHA `f93657fa82bdd37dba68ea962d5e9b2cf4fd4d60`；`ls -A` 空 | 补全 SHA 与本机参考副本说明 |
| B4 | **事实错误（原记录结论反了）** | 全仓编码扫描（python `decode('utf-8')`，src/example/tests/tools/sip/pyqt6/pyside6/scripts，除 3rdparty）：唯一非 UTF-8 为 `tools/Amalgamate.sh`（GBK 可解码）；三个 CMakeLists 与 build.ps1 均 UTF-8；根 CMakeLists:124 `/wd4819` 实为压 GBK 代码页下 UTF-8 无 BOM 源的 C4819 | 整条重写：更正对象名单，影响改为"01 S8 / 03 S1 编辑 Amalgamate.sh 时防转码"，R1 禁令保护对象修正 |
| B5 | 信息不全 | `ls /d/Qt`→5.14.2/6.4.0/6.7.3/6.10.1；仓库根同时存在 `bin_qt6.7.3_MSVC_x64/`；build.ps1 :67-69 字符串降序排序 | 补 D:\Qt 清单、字符串排序巧合（6.7.3 > 6.10.1）、`-QtPath` 兜底建议 |
| B6（新增） | 新发现（任务 B6） | `ls SARibbon-3.0-plan.md` 不存在；`git log --all -- SARibbon-3.0-plan.md` 空；`git status --porcelain` 3 项未跟踪 ≠ 01 P1 期望 4 项；01 S1 `git add SARibbon-3.0-plan.md` 必 fatal | 按模板新增：v1 缺失、悬空引用处理约定（指向 README 新节）、01 P1/S1 修正转 cross-findings |
| B7（新增） | 新发现 | `git branch --show-current`→`v3`；dev/v3/origin/dev 同 commit | 按模板新增：01 P2 门禁按"HEAD==7a617fc 且分支 dev 或 v3"解读；执行前可 checkout dev |
| B8（新增） | 新发现 | `ls tests/*.cpp \| wc -l`=25 + auto/ 1 = 26 注册；01 S7 写"24 个测试源"且 `git mv tests/*.cpp` 漏 `tests/auto/` | 按模板新增：N₀=26 基线事实 + 01 S7 搬运遗漏风险（详见 cross-findings） |

### SARibbon-3.0-plan-v2.md（只修事实，未动任何决策/取舍表结论/里程碑）

| 位置 | 问题类型 | 证据 | 处理 |
|------|---------|------|------|
| 文档头部 | 缺失声明（任务 C1） | v1 从未入库（ls + git log 核实） | 加"存留说明"：v1 未归档、§0/§1 为唯一存留记录、悬空引用以本文与 plans/3.0 为准；QWK 条目补 submodule 未初始化 + 参考副本 + `qwk_add_library` 实名（副本 src/CMakeLists.txt:38）；基线行补三分支同指 7a617fc |
| §1.1-2 | 行号精度 | KDDW `src/CMakeLists.txt`：`KDDW_FRONTEND_QTWIDGETS_SRCS` 并入 `DOCKSLIBS_SRCS` 在 :304，`add_library(kddockwidgets ...)` 在 :327 | 原"src/CMakeLists.txt:327 把 ... 条件拼进单一 target"细化为 :304 并入 + :327 单 target；补注 sa_add_library 为 SARibbon 新命名 |
| §1.2-P1 | 数量级不准 | `find src/core -name "*.h" \| wc -l`=69、`*.cpp`=45，合计 114 | "60+ 文件"→"114 个文件（69 .h + 45 .cpp，2026-09 实测）"（附表同改） |
| §1.2-P2 表 | 行号/精度补充 | `updateGeomArray(QRect)`:795-1203（409 行）、`recalcExpandGeomArray`:1204-1387（184 行）、`updateGeometryArr`:431、`categoryContentSize`:408、`scrollByAnimate`:971（QPropertyAnimation :992）、`layoutTitleRect`:1242、度量函数 .h:66-114 | 三行表格补实测行号；"约 170 行"→"约 184 行"；三巨头总行数 5281（"约5300"成立，未改） |
| §1.2-P2 LayoutingGuest 引用 | 行号/清单不准 | `LayoutingGuest_p.h`：class :31，纯虚 :36-43（minSize/maxSizeHint/setGeometry/setVisible/geometry/**setHost/host**/id 共 8 个）；"reused by non-KDDW projects" 注释在 :29 | ":29-46"→":31-43"，方法清单补 setHost/host，注明注释出处 |
| §1.2-P3 | 数字不准 | KDDW `src/core/View.h`（275 行）`= 0;` 计 60 处 | "~70 个纯虚"→"约 60 个纯虚（View.h 275 行、60 处 = 0）"；TabBarViewInterface 各方法补行号（text :42、tabAt :44、moveTabTo :45、rectForTab :46、insertDockWidget :53） |
| §1.2-P5 | 数字过时 | tests 实测 26 项（见 NOTES B8） | v1 引文"25 个单测"保留（属 v1 原文引用），括号补注现仓库实际 26 项及出处；Platform.h tests_* 补行号 :207-263 |
| §1.2-P7 | 现实批注（任务 C2） | f553de7；`git grep SARibbonPannel src/` 无命中 | 行尾加【已完成于 2.9.x，f553de7…别名过渡不再需要，见 NOTES B1】 |
| §2.3 取舍表 View 行 | 数字不准 | 同上（60 处 `= 0`） | "~70 纯虚"→"约 60 纯虚"（仅改数字，**不采用**结论未动） |
| §3.5 | 笔误 + 补充 | 原文"（Qt6/GuiGui）"；`SA_ActionPropertyName_*` 宏 :59-66 | "GuiGui"→"Qt6 属 QtGui"；补属性名宏行号范围；`SARibbonPanelItem.h:36-42` 核实为准确（加"已核实"） |
| §3.7 | 核实无误 | Qt6 QAction 属 QtGui、Qt5 属 QtWidgets 为 Qt 官方事实 | 不改 |
| §4.4 兼容层 Pannel 条目 | 现实批注 | 同 P7 | 加【现实更新：2.9.x 已完成（f553de7），别名过渡不再需要】，原方案文字存档不删 |
| §5.1 KDDW 引用 | 核实无误 | `qtquick/views/TabBar.h:44` = `class DOCKS_EXPORT TabBar : public QtQuick::View, public Core::TabBarViewInterface`（持 DockWidgetModel 属性）；`QmlTypes.cpp:23-26` = 4 个 `qmlRegisterType`（含 DockWidgetInstantiator :25）；`qtquick/views/qml/` 恰 15 个 .qml | 不改（全部属实） |
| §6.1 | 补充注释 | QWK 宏实名 `qwk_add_library`；SARibbonUtils.cmake 现状 | 括号注：sa_* 为 SARibbon 新命名、01 S5 实现、现仓库宏为死代码 |
| §7.3 | 数字过时 | tests 26 项注册 | 标题"25 个"→"26 个"，正文补构成说明与 NOTES B8 出处 |
| §8 M4 行 | 现实批注 | 同 P7 | "含 Pannel→Panel 拼写"后加【修正已在 2.9.x 完成，指南只作结论性说明】 |
| §10 R2 | 核实无误 | `SARibbonPanelLayout.cpp:1831` 正是 `setGeometry(const QRect&)` 定义行（守卫 `mInDoLayout` :1837） | 不改 |
| §1.2-P8 | 核实无误 | `wc -l SARibbonActionsManager.cpp`=792 | 不改 |

## 二、发现于 01~04 但无权修改的问题（供主 agent 整合）

按文档分组；每条含证据与建议修法。

### 01-infra-restructure.md

1. **P1 未跟踪项清单不符**：期望"仅 4 个已知未跟踪项（两份计划 md、bundle、本目录）"；实际 `git status --porcelain` 仅 3 项（`SARibbon-3.0-plan-v2.md`、`plans/`、`saribbon-dev-v2.8.0-plus.bundle`）——v1 计划 md 不存在（NOTES B6）。建议改为"3 个已知未跟踪项（v2 计划 md、plans/ 目录、bundle）"。
2. **P2 分支门禁会挡住执行**：期望 `git branch --show-current` = `dev`，实际为 `v3`（dev/v3/origin/dev 同指 `7a617fc`，HEAD 期望值本身正确）。建议改为"HEAD==7a617fc 且分支为 dev 或 v3；若在 v3，先 `git checkout dev` 或直接以当前 HEAD 继续"（NOTES B7）。
3. **S1 命令必失败**：`git add plans/ SARibbon-3.0-plan.md SARibbon-3.0-plan-v2.md` 中 `SARibbon-3.0-plan.md` 不存在，git add 对不匹配 pathspec 报 fatal。应删去该文件名（NOTES B6）。
4. **S6-2 PIMPL 宏搬移范围不完整**："从 `SARibbonGlobal.h:84-105` 整段 move"只覆盖 `SA_RIBBON_DECLARE_PRIVATE`(:84) 与 `SA_RIBBON_DECLARE_PUBLIC`(:104)；宏族实际延续到 **:184**——`SA_RIBBON_IMPL_CONSTRUCT` :123、`SA_D` :138、`SA_DC` :153、`SA_Q` :168、`SA_QC` :183（AGENTS.md 明确这些宏都属 PIMPL 惯用式）。照 :84-105 搬会漏掉 SA_D 系。建议改为":83-184 整段宏族（含 IMPL_CONSTRUCT/SA_D/SA_DC/SA_Q/SA_QC）"。
5. **S5 对现存宏的描述失准**："现有 16 行 `saribbon_set_bin_name` 宏保留备用"——该宏**无任何调用者**（grep 全仓仅定义处命中；根 CMakeLists :141 自行计算 `SARIBBON_LOCAL_INSTALL_BIN_NAME`），且宏体尾 `endmacro(damacro_set_bin_name)` 与宏名不一致；`wc -l` 报 15（末行无换行符，实为 16 行）。"保留备用"是决策可不变，但建议加事实批注（死代码 + 名称不一致），由执行者决定是否顺手删除并记 NOTES。
6. **S7 测试数量与搬运遗漏**："24 个测试源"应为 **25 个顶层 .cpp**；且 `git mv tests/*.cpp tests/widgets/` 通配不含子目录，会漏掉 `tests/auto/`（`auto/SARibbonThemePalette/tst_themepalette.cpp` 注册为 `SARibbonThemePaletteTest`，CMakeLists 以相对路径 `auto/...` 引用），漏搬即断链。建议补 `git mv tests/auto tests/widgets/auto`（NOTES B8）。
7. **S8/编码风险**：`tools/Amalgamate.sh` 是全仓唯一 GBK 文件（NOTES B4 更正后），S8 改 OPTS 时须防编辑器静默转 UTF-8 造成整文件 diff；建议 S8 加注"该脚本为 GBK，编辑时保持原编码"。
8. **build.ps1 旧选项名无步骤收口**：S4 把选项改名 `SARIBBON_BUILD_TESTS`（带 BUILD_TESTS shim），S10 只改 CI；`scripts/build.ps1:332` 仍传 `-DBUILD_TESTS`，四份计划无任何步骤更新它——虽经 shim 可用，但每次 configure 都会触发"改名提示"，且 R2 的标准验证命令即走此脚本。建议在 S4 或 S12 增加"同步 build.ps1 传参为新选项名"。
9. **§8 已知偏差引用 v1 目录图**："v1 计划目录图中的 `SARibbonPannel*` 文件名"——v1 不存在，此句无所指；建议改为"v2 计划与历史叙述中的 `SARibbonPannel*`"。头部设计依据"v1 计划 §3、§5、§7.1"为悬空引用（见本文件三-1 全局项）。
10. **（低优先）锚点链接**：02 P1 链 `01-infra-restructure.md#6-完成验收门全部满足才算完成本计划` 与 01 标题一致 ✓；但 03 P1 链 02 的锚点 `#6-完成验收门v2-m1-交付判据全部满足`、04 P1 链 03 的 `#6-完成验收门v2-m2-交付判据` 均丢掉了标题中"(= "对应的连字符，GitHub 渲染下大概率失效。建议统一把三个"完成验收门"标题改为无括号形式或修正锚点（属 02/03/04 文件，一并列出）。

### 02-core-sinking.md

11. **S1-1 / S3.1 的 git grep 命令语法错误（实测 fatal）**：`git grep -n "^enum class\|	enum class" src/widgets --include="*.h"` → `fatal: option '--include=*.h' must come before non-option arguments`。git grep 的 `--include` 必须置于路径参数**之前**（或改用 pathspec：`git grep -n "pattern" -- 'src/widgets/*.h'`）。S3.1 的 `git grep -n "setTabBarHeight\|..." src/widgets --include="*.cpp" --include="*.h"` 同病。执行 agent 会在此浪费回合，建议修正命令写法。
12. **S1-1 与 v2 §3.1 的命名空间张力（记录，不裁决）**：02 决策"枚举维持全局命名空间"，而 v2 §3.4.1 契约接口在 `SARibbon::Core` 内直接引用 `RowProportion`（§3.4.1 代码块注释"枚举移入 core/global"未指明命名空间）。两者可共存（全局枚举 + Core 命名空间接口引用它），但建议 02 S1 明示"契约接口引用的枚举为全局命名空间类型"，避免执行时来回改。属设计澄清，留给后续轮次/02 owner。
13. **核实无误项（无需修改，供 owner 参考）**：P4 期望"命中 6 处"✓（.h :66/89/92/96/108/114）；引用的测试名 `SARibbonThemeAutoSwitchTest`、`ThemeCoverageTest`、`SARibbonCategoryLayoutRTLTest`、`SARibbonCategoryVisibilityTest`、`SARibbonBarLayoutRTLTest`、`SARibbonSystemButtonBarGeometryTest`、`SARibbonTitleBarHitTestTest` 全部真实存在于 `tests/CMakeLists.txt`；`mButtonSizeHintCache`、`SARibbonPanelLayout_DEBUG_PRINT`、`updateGeomArray`、`recalcExpandGeomArray`、`updateGeometryArr`、`categoryContentSize`、`scrollByAnimate`、`layoutTitleRect`、`PanelLayoutMode`(SARibbonPanel.h:115)、`SARibbonThemePalette.*`、`SARibbonCustomizeData.*`、`SARibbonElementFactory.*`、`SARibbonQt5Compat.hpp`、`SARibbonBarVersionInfo.h(.in)` 均核实存在。
14. **（低优先）标题锚点**：本文档 §6 标题"完成验收门（= v2 M1 交付判据，全部满足）"被 03 P1 以简化锚点引用，见第 10 条。

### 03-build-ecosystem.md

15. **S3.2 Python 冒烟语句错误**："`pip install` 后 `import SARibbonBar`（保持 Python 包名不变）"——实际发行包名为 `PyQtSARibbon`（pyproject.toml:8、pyproject-pyqt6.toml:8）/`PySideSARibbon`（pyside6/pyproject.toml:10），pyexamples 的实际导入形式是 `from PyQtSARibbon import saribbon` / `from PySideSARibbon import saribbon`；不存在 `import SARibbonBar` 的包。"包名不变"结论本身成立（迁移前后都是 PyQtSARibbon），但冒烟命令须改，否则 S3.2/S3.3/S3.4 的验收步骤执行必失败。
16. **S6-1 命令同 --include 语法问题**：`git grep -rn "lrelease\|\.ts" CMakeLists.txt src cmake scripts --include="*" | head -30`——`--include` 位置错误（同第 11 条），且 `--include="*"` 无意义可直接删。另核实：lupdate/lrelease 现有机制在 `src/SARibbonBar/CMakeLists.txt:253/260` 附近，"现有机制在哪就改哪"可落地。
17. **S1 对 Amalgamate.sh 的编码风险**：同第 7 条（03 S1 也要改该脚本：双产物 + CRLF 逻辑保留），建议加注 GBK 提示。
18. **核实无误项**：`MANIFEST.in`、`project.py`、`pyproject.toml`、`pyproject-pyqt6.toml`、`tools/build_python_bindings.bat`、`tools/build_pyside6_bindings.bat`、`tools/amalgamate/`（4 模板）、`pyside6/{CMakeLists.txt,typesystem_saribbon.xml,saribbon_python_glue.h,pyproject.toml}`、`pyexamples/{pyqt5,pyqt6,pyside6}` 全部存在；S1-3 拟 ignore 的 `src/SARibbon.h/.cpp` 现确为 git 跟踪产物；`vcpkg-msvc-x64-release` preset 存在于 CMakePresets.json:40。
19. **（低优先）标题锚点**：§6 标题被 04 P1 以简化锚点引用，见第 10 条。

### 04-qml-and-release.md

20. **S5-2 章节引用错误**："`QAction` 在 Qt6 属 QtGui，core 一律不 include（**计划 02 §3.7**）"——§3.7（core 的禁区）是 **v2 计划**的章节号；计划 02 无 §3.7（其结构为 §1-§8 + S1-S10）。应改为"v2 计划 §3.7"。同类表述在 04 §4 纪律中无此问题。
21. **S9-2 迁移指南与 NOTES B1 一致性 ✓**：已按 B1 写明"Pannel 拼写已于 2.9.x（f553de7）修正、无需别名"——与 v2 批注（本轮已加）一致，无需修改。
22. **核实无误项**：`mkdocs.yml`、`readme.md`、`readme-cn.md`、`changlog.md`（仓库实名即此拼写）存在；"版本号三处核对"中 pyproject×3 属实（根/pyqt6/pyside6）；S10-6 "v1 §7.5" 为悬空引用（归入全局项）；S7 的 `tests/common/RibbonConformance.h` 与 v2 §7.2 命名一致；S3 引用的 `tests/core/layout_fixtures.h` 由 02 S5.0-2 定义，链路闭合。
23. **（低优先）P1 锚点**：`03-build-ecosystem.md#6-完成验收门v2-m2-交付判据`，见第 10 条。

### 跨 01~04 的全局项

24. **"v1 §x" 悬空引用全清单**（v1 文件从未存在，NOTES B6；处理约定见 README"计划文档自包含性说明"）：
    - 01：头部设计依据（v1 §3、§5、§7.1）、S2（v1 §7.4）、S3（v1 §3）、S5（v1 §5.2）、S6（v1 §5.3 两处）、S11（v1 §7.1）、§8（v1 目录图）；
    - 03：头部设计依据（v1 §5.4、§7.1–§7.4）、S1（v1 §7.2 两处）、S3.2（v1 §7.3）；
    - 04：S8（v1 §7.4）、S10（v1 §7.5）；
    - 02：无 v1 引用（头部只引 v2，最干净）。
    建议各 owner 将可还原处改指 v2 章节或执行计划自身步骤，不可还原处删除或标"（v1 未归档，内容以本步骤为准）"。
25. **选项名一致性 ✓**：四份文档统一使用 `SARIBBON_BUILD_WIDGETS/QML/STATIC_LIBS/EXAMPLES/TESTS`、`SARIBBON_USE_FRAMELESS_LIB`、`SARIBBON_ENABLE_SNAPLAYOUT`，与 01 S4 定义一致；与现仓库的差距（`BUILD_TESTS` 无前缀、无 WIDGETS/QML 选项）已由 01 S4 shim 覆盖。唯一遗漏是 build.ps1（见第 8 条）。
26. **接口/类型名一致性 ✓**：`SARibbonAbstractLayoutItem/Host`、`SARibbonMetrics`、`SARibbonPanelLayoutEngine/CategoryLayoutEngine/BarGeometryEngine`、`SARibbonThemeData`、`SARibbonElementFactoryInterface`、`SARibbonEnums.h`、`check_core_purity.py`、`sa_add_library/sa_sync_include` 在 v2 与 01/02/04 间拼写一致，未发现漂移。
27. **S 编号交叉引用抽查**：02 S9↔03 P2/目标-3（core 黄金测试前置）✓；02 S3 产物 `docs/3.0/metrics-comparison.md`↔04 S9-2 链接 ✓；01 S8↔03 P3（单文件现状）✓；04 S4-1"计划 02 S7 走了 D6 降级"↔02 S7-3 ✓；README R5 原"计划 02 S6"错误已由本 agent 修正。未发现其他编号错引。

## 三、建议后续轮次关注

1. **枚举命名空间决策收口**（见第 12 条）：02 S1"维持全局命名空间"与 v2 §3.1 目录注释（`SARibbon::Core`）之间的表述张力，建议 round2 由设计视角 agent 裁决并同步 v2 §3.1/§3.4.1 措辞。
2. **ThreadData 单例选型**：02 S2.1-3 留给执行时定（`Q_GLOBAL_STATIC` vs 挂 `QCoreApplication`），涉及 core 纯净性（不得 include QApplication）；建议 round2 给出倾向性建议，避免执行 agent 现场发明。
3. **N₀ 基线的实测落账**：26 是静态清点 `add_saribbon_test()` 的结果；01 P3 要求实跑记录 N₀——若某测试在 CI/本机被跳过（如 offscreen 平台插件问题），N₀ 可能与 26 不同，round2 可考虑把"N₀ 定义（注册数 vs 实跑通过数）"写死。
4. **build.ps1 的 Qt 探测字符串排序**（README R2 已注）：`"6.7.3" > "6.10.1"` 是字符串比较的巧合正确；后续轮次可决定是否让 01 S12 顺带把 build.ps1 的版本排序改为 `[version]` 类型比较（超出 3.0 重构范围，默认不动）。
5. **amalgamate 产物出库的下游盘点**：03 S1-5 的 `git grep "src/SARibbon\."` 盘点范围建议明确包含 `pyproject*.toml`（其 `builder-settings` 直接引用 `src/SARibbonBar/SARibbonResource.qrc` 与头文件清单，01 S6 目录改名后同样断裂——sip 配置的 include-dirs 也是 `src/SARibbonBar`）。03 S3.2 有覆盖 sip 侧，但 qrc 路径（`../../src/SARibbonBar/SARibbonResource.qrc`）未见显式盘点项。
6. **KDDW/QWK 参考副本为本机绝对路径**：`F:\src\3rdparty\...` 仅本机有效；若执行 agent 在别的环境跑，需先初始化 qwindowkit submodule（pin f93657f）或另行获取 KDDW 源码。建议 round2 在 README R2 或 01 前置条件中加"参考项目获取方式"说明。
7. **CI workflow 内部结构未逐一核**：本轮只核实了 8 个 workflow 文件的**存在性与名称**；01 S10 假设的"每个 workflow 的构建 step 结构、matrix 有无"未逐个打开核对（如 `cmake-linux-qt6.8.yml` 是否已有 matrix），执行到 S10 时按 R4 处理即可，round2 若有 CI 视角 agent 建议预核。
8. **`tests/CMakeLists.txt` 顶层 `find_package` 与 01 S7 重建**：01 S7 要求"重建 tests/CMakeLists.txt + add_subdirectory(widgets)"，注意保留 `QT_TESTCASE_SOURCEDIR` 编译定义（有测试依赖它定位源码目录）与 TIMEOUT 120 设置，避免重建时丢失。
