# 计划 01 第 3 轮评审：首次执行者压力测试（dry-run）findings

> 评审对象：`plans/3.0/01-infra-restructure.md`（round2 修订后版本，921 行）
> 评审方式：以"只有本文档 + README.md + 仓库访问权的首次执行 agent"身份，对 P1~P5、S1~S12、§6 验收门逐节 dry-run——只读命令**真跑**核对，状态变更命令**桌面推演**，代码块逐行桌面编译。
> 评审时点仓库状态：分支 `v3` @ `eba8ebd`（round2 评审提交），基线 `7a617fc` 之后已有 `42b7dcc`、`eba8ebd` 两个计划文档提交；未跟踪项仅 `saribbon-dev-v2.8.0-plus.bundle`。
> 处理原则：所有"已修文档"均直接落在 01-infra-restructure.md 正文（以 "round3" 字样标注）；本文件只做记录。不重排 S 编号。

## 逐条 findings（按文档顺序）

| # | 位置 | 级别 | 现象与证据 | 处理 |
|---|------|------|-----------|------|
| 1 | 头部注（分支行） | 歧义（过时） | "v3 与 dev 均指向基线 7a617fc"已不成立：`git rev-parse dev v3` → dev=`7a617fc`、v3=`eba8ebd`（评审提交链） | 已修：改为"v3 指向评审提交链顶端，dev 仍指 7a617fc" |
| 2 | §1 目录树 `src/CMakeLists.txt` 注释 | 瑕疵 | 注释写"生成 config 头"，但 S6.2 的代码块把 `configure_file` 放在 `src/core/CMakeLists.txt`，前后不一 | 已修：树注释改为"包配置生成（S11）；core config 头由 src/core/CMakeLists.txt 生成（S6.2）" |
| 3 | §3-P1 | **阻塞** | 期望"仅 3 个未跟踪项（v2 计划、plans/、bundle）"——实测 `git status --porcelain` 仅 `?? saribbon-dev-v2.8.0-plus.bundle`：plans/ 与 v2 计划已随 `42b7dcc`/`eba8ebd` 入库。执行者按原文会判定前置条件失败或困惑 | 已修：P1 改为"仅 1 个未跟踪项（bundle）"，并注明校准依据 |
| 4 | §3-P2 | **阻塞** | 期望"HEAD 为 7a617fc"——实测 HEAD=`eba8ebd`。原门禁必然不过 | 已修：重写为执行时点稳健的双门禁：`git merge-base --is-ancestor 7a617fc HEAD`（输出 OK）+ `git diff 7a617fc HEAD --stat -- ':!plans' ':!SARibbon-3.0-plan-v2.md'`（输出为空 = 基线后无代码改动） |
| 5 | §3-P3 | 通过 | `scripts/build.ps1:322` `$cmakeArgs = @(`、`:332` `-DBUILD_TESTS=$Tests` 精确吻合；`tests/CMakeLists.txt` 26 个 `add_saribbon_test(` 实测吻合。构建本身属状态变更未跑（见遗留风险 3） | 记录 |
| 6 | §3-P4 | 通过 | `wc -l` 实测 1863/1425/1993，与文档一致 | 记录 |
| 7 | §3-P5 | 通过 | `build.ps1:81` 输出串 `[OK] Qt path (auto-detected): $candidate ($qtVer)` 与期望格式一致（脚本未真跑，属状态变更） | 记录 |
| 8 | S1 | **阻塞** | `git add plans/ SARibbon-3.0-plan-v2.md` + commit 在执行时点会报 "nothing to commit"（两路径已跟踪且无改动），执行者会卡住或误判 | 已修：S1 改为"建分支为主，commit 仅当 plans/ 有未提交修订（如 round3 产物）时执行；nothing to commit 属正常" |
| 9 | S2 | 通过 | 11 个示例目录与 `example/CMakeLists.txt` 11 个 `add_subdirectory` 一一对应；根 `:224` `add_subdirectory(example)`；`StaticExample/CMakeLists.txt:55` `SET(SARIBBON_DIR .../../../src)`；`git grep -n '\.\./\.\./' example` 唯一命中即 :55——全部吻合。`../../../src` 新路径推演正确（examples/widgets/StaticExample 上三级=仓库根） | 记录 |
| 10 | S2 验证 | 瑕疵 | `git log --follow <新路径>` 在本步提交前无输出（rename 未入历史），原文把它放在提交前验证里，执行者会误判搬移失败 | 已修：注明"须在提交之后执行" |
| 11 | S3 | 通过 | `.gitmodules` path/url（czyt1988 fork）、`git submodule status` `-f93657fa...`、3rdparty 三内容（qwindowkit/CMakeLists.txt/cmd-build-example.sh）、`3rdparty/CMakeLists.txt:46` `../../../${SARIBBON_BIN_NAME}` 全部吻合；`../${SARIBBON_BIN_NAME}` 修正推演正确；回退方案合法 | 记录 |
| 12 | S4/S4.1 | 通过 | 根 CMakeLists 246 行；处置表全部行号逐块实测吻合（L5/7-13/15-27/32-43/45-56(:47=5.12)/58-77/79-103(:93,:100)/105-121/123-128(:124,:127)/131-148(:141-142,:145-148)/151-194(:163,:169)/198/200-206/208-218/220/222-225/227-231/235-244）；`SARIBBON_DOC_FILES` 全仓仅 1 处定义（死变量）证实；`SARibbonBarVersionInfo.h.in` 消费 `@SARIBBON_VERSION_*@`+`#cmakedefine SARIBBON_VERSION` 证实；模块 `:2` `project(SARibbonBar ... VERSION ${SARIBBON_VERSION})` 证实。shim/派生 set/过渡 CXX_STANDARD 桌面推演均成立（include() 幂等、GNUInstallDirs 重复无害） | 记录 |
| 13 | S4.2 | 通过（抽查） | QWK 顶层 floor 3.19（:1）、选项命名（:8-10+）、`QWINDOWKIT_INSTALL` 守卫（src/CMakeLists.txt:100/:181）与本地副本一致 | 记录 |
| 14 | S5.1 | 通过 | `cmake/SARibbonUtils.cmake` wc -l=15（末行无换行，实 16 行）、`saribbon_set_bin_name` 零调用、宏体 `DA_MIN_QT_VERSION`/`endmacro(damacro_set_bin_name)` 残留证实；QWK 侧 `qwk_add_library` @ src/CMakeLists.txt:38-126、参数集（AUTOGEN/NO_SYNC_INCLUDE/NO_WIN_RC + SYNC_INCLUDE_PREFIX/PREFIX，:39-42）、目录级 AUTOMOC（:44-48）、STATIC 判定（:50-54）、win rc（:58-64）、`qm_export_defines`（:73）、`^QWK(.+)` EXPORT_NAME+ALIAS（:83-90）、源目录 `.` PRIVATE（:80）、qmsetup pin `99ca80fd...`（`git submodule status` @ QWK 副本）全部实证吻合 | 记录 |
| 15 | S5.2 | 瑕疵 | `qwkglobal.h` 三段式实际 :12-22（末尾 #endif 在 22），正文引 :12-21（round2 补注处已是 12-22，两处不一致） | 已修：统一 :12-22 |
| 16 | S5.2 旧宏映射方向 | 通过 | `SARibbonGlobal.h:9-18`（NO_EXPORT 外层→空宏）与 `SAColorWidgetsGlobal.h:97-111`（NO_DLL 外层优先）实测与文档描述一致，映射方向未写反 | 记录 |
| 17 | S5.3 骨架 | 通过（桌面编译） | `cmake_parse_arguments` 用法、`add_library+target_sources`（3.11+）、生成器表达式、`^SARibbon(.+)$` 正则、EXPORT_NAME/ALIAS（3.18+ 非 IMPORTED 含 :: 别名，floor 3.21 满足）、`foreach IN LISTS`、`install(TARGETS ... OPTIONAL)`、`create_win32_resource_version` 实参形态（对照 `cmake/WinResource.cmake:1-103` 的 TARGET/FILENAME/EXT/DESCRIPTION 关键字解析，RC 按 `${CMAKE_CURRENT_BINARY_DIR}/${_target}.rc` 生成、core/widgets 各自目录不冲突）均合法。`CMAKE_INSTALL_LIBDIR/BINDIR` 依赖根 `include(GNUInstallDirs)`（S4 第 8 条先行）✓ | 记录 |
| 18 | S5.4 骨架 | 通过 + 补充 | `file(GLOB_RECURSE ... RELATIVE)`、`continue()`（3.2+）、`file(COPY ... DESTINATION dir/)`（_sub 为空时尾斜杠合法）、`install(DIRECTORY)`（配置期已复制，安装期存在）均合法；同步目录不会引入 qwindowkit 头（S3 已把 3rdparty 移出 src/widgets，步骤顺序保证）。**两处未言明的差异**：① GLOB 机制会把 `SARibbonMdiControlsStyle.h` 新带进同步/安装集合（实测：目录 42 个顶层头 = 清单 41 + 该文件；colorWidgets 两侧一致）；② 旧 `install(FILES ... COMPONENT headers)` 的 COMPONENT 标签在新机制丢失（无 cpack/组件消费方，良性） | 已修：S5.4 说明补两条已知良性差异（记 NOTES） |
| 19 | S6.1 骨架 | 歧义（偏阻塞） | 三个"现场发明"点：(a) frameless 只有注释"LINKS_PRIVATE 化"，无机制——`sa_add_library` 调用里没有对应参数，执行者需自创条件传参；(b) 随迁保留块（L203-219/L225-229/L234-288）全部引用 `${SARIBBON_LIB_NAME}`，骨架未说明处置——若按"原样保留"连 L3 `set(SARIBBON_LIB_NAME SARibbonBar)` 一起保留，`target_sources(SARibbonBar ...)`/`add_custom_command(TARGET SARibbonBar ...)` 将作用于 S6.6 的 ALIAS target → 配置期直接报错；(c) 新公共头 `SARibbonWidgetsGlobal.h` 未指示加入 `SARIBBON_HEADER_FILES`（影响 S11.3 转发头循环与 IDE 清单） | 已修：骨架补 `_SA_WIDGETS_PRIVATE_LINKS` 具体代码 + `LINKS_PRIVATE` 参数；补"两条机械替换规则"（LIB_NAME→SARibbonWidgets、qm install 进 SARIBBON_INSTALL 守卫）；补清单唯一新增行说明 |
| 20 | S6.1 清单核对注意 | 歧义（内部矛盾） | 注意说 MdiControlsStyle.h"现状就不随头安装——维持现状"，但 S5.4 GLOB 机制必然把它带进安装树，两处矛盾，执行者无所适从 | 已修：改为"清单变量维持现状（编译面零变化）；同步机制下该头新进入安装集合属良性差异，记 NOTES，勿在 GLOB 里排除" |
| 21 | S6.2 Qt5Compat 引用计数 | 歧义（事实错误） | "widgets 内 **38 个文件**平铺 include"——实测仅 **5 个 .cpp**（SAFramelessHelper.cpp:10、SARibbonBar.cpp:26、SARibbonGallery.cpp:12、SARibbonPanelLayout.cpp:11、SARibbonToolButton.cpp:18；`git grep -rn "SARibbonQt5Compat" src/SARibbonBar`），38 系误抄 SARibbonGlobal.h 的数字。转发头决策不受影响（清单行 L36/L143 指向搬走的文件会配置失败 + amalgamate 模板 1 处 + 安装树形态），但理由需重写 | 已修：计数、清单行、模板、python 打包四类引用面逐项列明 |
| 22 | S6.2 转发头/Config.h.in 内容 | 缺口 | 转发头只有"内容仅 `#include <SARibbonCore/...>`"一句；`SARibbonCoreConfig.h.in` 只有"版本+feature 占位"一句——两者都需现场发明；且 Config 生成归属误写 `src/CMakeLists.txt`（S6.2 自己的代码块在 `src/core/CMakeLists.txt`） | 已修：补转发头全文（pragma once + 注释 + include）、补 .h.in 全文（版本四宏 + SA_RIBBON_CONFIG 占位段）、归属更正、补 `@PROJECT_VERSION*@` 作用域说明（S6 后模块层无 project()，取根 3.0.0） |
| 23 | S6.3 | 歧义 | ① 验证命令 `git grep -rl "SARibbonGlobal.h" src/SARibbonBar \| wc -l` 实测 **39**（含 CMakeLists.txt），与文中"38"不符，执行者会疑心仓库漂移；② `SARibbonGlobal.h` 实为 **UTF-8 带 BOM + CRLF**（`file` 实测），重写为兼容转发头时未声明保持 BOM/行尾——按常规 UTF-8 无 BOM+LF 写回即整文件 diff、违反 R1 | 已修：命令加 pathspec（`-- 'src/SARibbonBar/*.h' '*.cpp' '*.hpp'` → 38）并注明 39 的成因；补编码警示 |
| 24 | S6.4 | 歧义（边界语义丢失） | 替换块把 `SA_COLOR_WIDGETS_API` 直接等价 `SA_RIBBON_WIDGETS_EXPORT`，丢掉了 2.9.5 的 **NO_DLL 最外层优先**结构（实测 L97-111）：外部旧脚本只定义 `SA_COLOR_WIDGETS_NO_DLL`（不定义 BAR_NO_EXPORT）时，2.9.5 得空宏、新结构得 Q_DECL_IMPORT——行为变化 | 已修：替换块恢复 `#ifdef SA_COLOR_WIDGETS_NO_DLL → 空` 优先分支；补 SAColorWidgetsGlobal.h 编码注（UTF-8 无 BOM + CRLF）；尾段"无副作用"表述同步更正 |
| 25 | S6.5 | 缺口 + 瑕疵 | ① v2 §8-M0 判据"三空 target 可配置安装"与"qml 不建 target"的字面偏差未登记，执行者对照 v2 时会困惑；② QWK quick CMakeLists "全文 38 行"实测 `wc -l` = 37 | 已修：补"有意决策"注记（占位 .cpp 会新增 2.9.5 不存在的安装产物、违反行为零变化；M0 判据按三模块可配置解读）；行数更正 |
| 26 | S6.6 | 瑕疵 | "tests 链裸 SARibbonBar（tests/CMakeLists.txt:11）"——实际 `target_link_libraries` 的 `SARibbonBar` 在 **:10** | 已修 |
| 27 | S6.7 | 缺口 + 瑕疵 | ① S6.1 删除旧 install 块（L289-352）后，**`share/SARibbonBar_amalgamate` 的安装规则无人重建**——S5/S6.7/S11 都没有，而 S11.5 安装树清单期望该项存在（2.9.5 行为丢失，验收必挂）；② `src/CMakeLists.txt` 现文件带 UTF-8 BOM（`file` 实测），重写未声明保持 | 已修：S6.7 代码块补 `install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/SARibbon.h ... SARibbon.cpp DESTINATION share/SARibbonBar_amalgamate)`（SARIBBON_INSTALL 守卫内）；补 BOM 注 |
| 28 | S6.8 | 瑕疵（命中面不完整） | "命中仅在 docs/（8 个文件）与 AGENTS.md"——实测 `git grep -ln "SARibbonBar/i18n\|SARibbonBar/resource"` 还命中 `src/SARibbon.cpp`、`tests/ThemeCoverageTest.cpp`、`tools/qrc_SARibbonResource_Datas.cpp`、plans/ 自引用（"CMake 构建系统内零命中"仍成立） | 已修：命中面全列并逐项标注归属（S7.3/S8/门禁豁免/计划 04） |
| 29 | S6.9 代码块 A | 通过 | PIMPL 宏区边界 L20（首个 `/**`）-L184（SA_QC 的 `#endif`）实测精确；宏本体行号（83-88/103-109/122-124/137-139/152-154/167-169/182-184）全对；sa_as_const L274-283 对；**纯净性预扫**：L20-184 与 L274-283 区段对 S9 全部禁词零命中（`sed+grep` 实测），"必绿"声明成立 | 已修（增强）：补边界复核说明与新文件编码（UTF-8 无 BOM） |
| 30 | S6.9 代码块 B | 歧义 | 代码块中枚举/PROP 宏以 `// ==== ... ====` 占位注释呈现，照抄字面内容会**删除三个公共枚举 + Q_DECLARE_FLAGS/Q_DECLARE_OPERATORS_FOR_FLAGS + PROP 宏**（编译期能爆，但属可避免的大返工）；且该文件重写涉及 BOM+CRLF 保持（同 #23） | 已修：块前加"占位说明非文件内容，须置入原 L186-272 真实代码"显式警示 + 编码标注 |
| 31 | S6.9 代码块 C | **阻塞** | 兼容映射放在三段式**之后**，配套论证"C 预处理器是惰性展开…顺序安全"**错误**：惰性的只是对象宏 `SA_RIBBON_EXPORT` 的展开；三段式内的 `#ifdef SA_RIBBON_WIDGETS_STATIC` 是**指令，在定义处即时求值**。旧宏场景（amalgamate 模板头部定义 `SA_RIBBON_BAR_NO_EXPORT` 后 include，Template.h:4-6/Template.cpp:2-4 实测）下求值点看不到映射结果，`SA_RIBBON_WIDGETS_EXPORT` 被外层 `#ifndef` 永久固化为 `Q_DECL_IMPORT` → 单文件产物所有类带 dllimport 编译，MSVC 下"dllimport 类不可定义"必失败，StaticExample 验收门必挂 | 已修：映射块移到三段式之前；重写整段论证（含 Template 头部定义位置证据）；STATIC 优先次序保持（与 round2 补注的双保险说法兼容） |
| 32 | S6 末尾验证（删全局 CXX_STANDARD） | 通过（推演） | 删除安全性论证：tests/examples 链接库 target，`target_compile_features(PUBLIC cxx_std_17)` 经 INTERFACE_COMPILE_FEATURES 传播给消费者；StaticExample 不链接库但自带目录级 `CMAKE_CXX_STANDARD 14`（实测其 CMakeLists:42-43）+ Qt6 target 的 cxx_std_17 接口特性托底（与 2.9.5 现状同机制，非新增风险）；uiform 自设 17（:10-11） | 记录 |
| 33 | S7 现状核实 | 通过 + 瑕疵 | 25 顶层 .cpp + auto/1 + 26 注册实测吻合；函数定义实际 **:7-44**（原文 :7-45）；POST_BUILD/TIMEOUT 120/RUNTIME_OUTPUT_DIRECTORY 全对 | 已修：行号 :7-44、链接行 :10 |
| 34 | S7.3 | 瑕疵（口径） | "tests 内唯一 src/SARibbonBar 硬编码，`git grep -n "src/SARibbonBar" tests` 可复核"——该命令实测命中 **2** 处（CMakeLists.txt:17 + ThemeCoverageTest.cpp:48），执行者会以为漏改；新相对路径 `../../src/widgets/resource`（SOURCEDIR=tests/widgets/）推演正确 | 已修：澄清 2 处命中各自归属（S7.2 删 / S7.3 改），S7 完成后归零 |
| 35 | S7.1/S7.4 | 通过 | `git mv tests/*.cpp tests/widgets/`（Git Bash glob 展开 25 文件 + mkdir 先行）合法；10 个 example 全部 `SARibbonBar::SARibbonBar` + `if(NOT TARGET SARibbonBar)` fallback 实测（10 文件逐一命中）；QWK 树内裸名链接对照与本地副本一致 | 记录 |
| 36 | S8 现状核实 | 通过 | `DEST=../src`、OPTS、两次 `./Amalgamate.exe` 调用、awk LF→CRLF、`read -n 1`、GBK（`file` 报 ISO-8859=GBK 可解码）全对；模板计数 46（PublicHeaders.h）+43（Template.cpp）=89 精确；`src/SARibbon.cpp` 32 处、`qrc_SARibbonResource_Datas.cpp` 32 处（version2/3 为 0）精确；两模板头部先定义 NO_EXPORT/NO_DLL 实测（Template.h:4-10、Template.cpp:2-8） | 已修（增强）：补模板宏定义行号、qrc 计数分布（32 全在 Datas） |
| 37 | S8.2 | 瑕疵（编码保护缺注） | 模板 4 文件中 PublicHeaders.h/Template.h 为 **UTF-8 BOM+CRLF**、Template.cpp 为 UTF-8 无 BOM+CRLF（`file` 实测）；sed 按字节替换不触 BOM/行尾（安全），但执行者若改用编辑器整文件重写会转码违反 R1 | 已修：补编码注 + "只许 sed、改后 git diff --stat 核行数"守则 |
| 38 | S8.3 | 通过（推演）+ 小缺口 | 镜像逻辑桌面推演成立：模板 sed 后全部经 `src/widgets/` 转发链 → `<SARibbonCore/...>` 由 `_amalg_include` 镜像解析；Config.h 不被任何头引用（块 A 设计）故镜像无需带它；脚本中途失败会残留 `tools/_amalg_include/`（未跟踪、不影响 git grep 门禁）但原文未提清理 | 已修：补残留手动清理说明 |
| 39 | S9 | 歧义（清单与 v2 未对齐） | 默认禁词清单用 `QQmlEngine` 单名——v2 §3.7/§6.2 是 `QQml*` 前缀族（漏 QQmlContext 等）；且 v2 §3.7 明写"QLayout（**及 Q*Layout 族**）"，前缀 `QLayout` 拦不住 QGridLayout/QVBoxLayout/QBoxLayout/QFormLayout/QStackedLayout（QAction 原文已含，无需补） | 已修：`QQmlEngine`→`QQml` 前缀族；新增家族正则 `^Q[A-Za-z]*Layout$`；补"合法名零误伤"验证说明（QGuiApplication 不以 QApplication 为前缀等，逐项核过） |
| 40 | S9 调用名 | 通过 + 关联修正 | "Linux CI runner 无 python 命令"属实；但 S10.1 把 `python3` 写进 **windows** workflow 会踩 Store stub（见 #41）；本地验证命令 `python3` 在 Windows 也可能不可用 | 已修：S9 补"例外：windows workflow 用 python"；§6 门禁补 Windows 等价说明 |
| 41 | S10.1 | **阻塞**（win CI 必红） | purity step `run: python3 ...` 加进全部 6 个 workflow——GitHub **windows runner 无 `python3` 命令**（python.org 发行版不生成 python3.exe，PATH 落到 Microsoft Store 占位 stub，退出码 9009/Store 提示），win-qt5.15 与 win-qt6.8 两个 job 在该 step 直接失败 | 已修：按平台拆分——linux/mac 用 `python3`，windows 两个 workflow 用 `python`，并写明原因 |
| 42 | S10.2 × S10.7 | **阻塞**（组合缺陷） | S10.2 让 linux-qt6.8 的 widgets=OFF 组合 `-DSARIBBON_BUILD_TESTS=OFF`（0 个测试注册），S10.7 又给全部 workflow 的 ctest 加 `--no-tests=error`（0 测试即非零退出）→ core-only 矩阵项**必红**，S10 验证门"6 workflow 全绿"不可达成 | 已修：S10.2 补硬性要求——该 workflow Test 步骤加 `if: matrix.widgets == 'ON'`（purity step 不受影响，两组合都跑） |
| 43 | S10 现状核实 | 通过 + 瑕疵 | 6 workflow/8 文件清单、BUILD_TESTS 恰好 6 处（行号 36/37/35/37/33/35 实测）、matrix 三轴、无 submodule checkout、publish 触发器 :3-6、presets version 6+3.25（:2-7）、5 个 configure preset、vcpkg frameless feature 全部吻合。**瑕疵**：S10.7 称 6 个 workflow "均已有 cache:'true'"——实测 mac-qt5.15 与 win-qt6.8 为 `'false'` | 已修：cache 现状逐个列明，声明不统一不改动 |
| 44 | S11 现状盘点 | 通过 | 旧安装规则行号（L292-302/L306-313/L273-276/L316-352）、`SARibbonBarConfig.cmake.in:12` find_dependency、`SARibbonConfig.cmake.in` 与 `tools/test-find-package/` 不存在——全部实测吻合 | 记录 |
| 45 | S11 开头 | 缺口（时序未声明） | S6 删旧 Config 生成、S11 才建新包——S6~S10 各提交点安装树无任何可用 Config（find_package 两种包名均失败）。R3 验证（build+ctest 走构建树）不受影响，但执行者若在中途做安装态验证会误判 | 已修：S11 开头补时序说明（计划内中间态 + 期间不做安装态消费验证） |
| 46 | S11.1 Config 骨架 | 歧义 | 组件支持表只有 `Widgets Qml`——core-only 安装（v2 §6.2 组合构建的产物形态）下 `find_package(SARibbon COMPONENTS Core)` 会被判"Unsupported component"直接失败；且存在性检查硬编码 `STREQUAL "Widgets"`，Qml 组件漏检 | 已修：supported 加 `Core`；检查一般化为 `NOT TARGET SARibbon::${_comp}`。其余（@OPTION@ 代入 if()、IN_LIST、check_required_components 不带 NO_CHECK_REQUIRED_COMPONENTS_MACRO 的坑位提示、baked-in Qt 版本）桌面推演均合法 |
| 47 | S11.2 薄壳 | 缺口 | 只有一句话描述（include Targets + INTERFACE IMPORTED 转发），未给：Targets 相对路径、find_dependency(Qt)（缺了会在消费端 target 解析期才爆）、ConfigVersion 同装（2.9.5 有，缺了 `find_package(SARibbonBar 2.9)` 行为变化）、2.x 变量（INCLUDE_DIR/LIBRARIES）——全部需现场发明 | 已修：补薄壳 .in 全文 + 生成/安装命令归属（src/CMakeLists.txt、SARIBBON_INSTALL 守卫内）+ 三条要点 |
| 48 | S11.4 冒烟命令 | 歧义（必失败） | `-DCMAKE_PREFIX_PATH=<install目录>` 只给安装前缀——Config 的 `find_dependency(Qt6 ...)` 找不到 Qt，configure 直接失败；另"分别以两种 include 各编一个 TU"未强调不可合并（同一 TU 内两种路径经 include guard 只生效一份，测不出双路径） | 已修：命令改 `"<install目录>;<Qt6路径>"`（含 Windows 分号列表引号说明）+ 补 build 命令 + 补"两 TU 不可合并"原因 |
| 49 | S12 | 通过 | AGENTS.md 恰好 6 处（L5/L15/L17/L18/L19/L36 逐行实测）；build.md 选项表 L120-127（BUILD_TESTS 行在 L127）、L11 Qt 门槛行、L13 frameless 注；submodule.md L20/L29 + stdware url 不符；changlog.md 文件名拼写——全部吻合。docs 窄口径 8 文件吻合；全口径实为 22 文件 | 已修（增强）：S12.5 补"NOTES 登记以 22 个全量清单为准（8 个是子集）" |
| 50 | §6 残留引用门禁 | **阻塞**（门禁必败） | 真跑门禁命令并按归属逐项分析：完整执行 S1~S12 后仍会命中 ① `src/SARibbon.cpp` 32 处——全部来自内联的 `qrc_SARibbonResource_Datas.cpp` rcc 生成注释（位于 "Start/End of inlined file" 标记间，实测），而 S8 明确"qrc 无需再生"→ 重生成产物后 32 处**依然存在**，原文"必须归零"不可达成；② `SARibbon-3.0-plan-v2.md:9` 1 处（对 2.9.5 submodule 旧路径的历史叙述），不在排除名单 | 已修：门禁拆 A+B——A 增加 `':!src/SARibbon.cpp'` `':!SARibbon-3.0-plan-v2.md'` 排除；B 对 src/SARibbon.cpp 定向检查（剔除 `// C:/src/Qt/SARibbon/src/SARibbonBar/` 注释行后须零命中）；并把"必须归零"文件的逐一对账清单（amalgamate 89=43+46、.gitmodules 2、AGENTS.md 6、submodule.md 2、根 CMakeLists 2、tests 2、Amalgamate.sh 1）写入期望说明 |
| 51 | §6 其余验收门 | 通过（推演） | --follow 三门（S6 git mv 后成立）；ctest==N₀；core-only 配置推演（widgets/qml/examples/tests 全关，install(EXPORT) 只含 core 合法）；purity 退出码 0（#29 预扫）；submodule status 路径；截图对比可操作 | 记录 |
| 52 | §8 已知偏差 | 过时 | "当前检出分支为 v3（与 dev 同指 7a617fc）""未跟踪项实为 3 个"均已被评审提交推翻 | 已修：按 round1/round3 两时点分述 + 追加 round3 确认清单（8 条） |
| 53 | v2 M0 完整性对照 | 通过 | v2 §8-M0 四项（dev-3.0 分支/SARibbonUtils/三模块骨架/纯净扫描脚本+CI）分别对应 S1/S5/S6/S9+S10；§6.1 保留项（sa_add_library、sa_sync_include、组件化 Config、命名空间 target、静态导出宏、BUILD_QML=OFF、SARIBBON_INSTALL 守卫）对应 S5/S11/S4/S6 全覆盖；§6.2 第一层清单经 #39 修正后一致，第二层归 02 S8-3（01 已声明）；§6.4 守卫在 S4.3/S5.3/S5.4/S6.7/S11 全落位；§2.1 global/ 三件（导出宏/PIMPL/Qt5Compat/Config）= S6.2 骨架，子目录下沉归 02。**一处字面偏差**：M0 判据"三空 target 可配置安装" vs qml 无 target——已在 #25 登记为有意决策；**一处延后**：v2 §6.2 "Widgets=OFF Qml=ON" 矩阵项在 01 无意义（qml 无 target，ON/OFF 等价），归 04 建 target 后补——建议终审确认 04 已认领 | 记录（见第四节建议 5） |

## 一、dry-run 中真跑过的命令与结果摘要

全部为只读命令（未执行任何 git mv/add/commit/checkout、cmake、build.ps1、sed -i、pip、文件写入——除本 findings 与计划文档修订本身）：

| 类别 | 命令（摘要） | 结果 |
|------|-------------|------|
| git 状态 | `git status --porcelain`；`git branch --show-current`；`git log --oneline -5`；`git rev-parse dev v3 origin/dev HEAD`；`git show --stat 42b7dcc eba8ebd` | v3@eba8ebd、dev@7a617fc；未跟踪仅 bundle；两评审提交只动 plans/+v2 计划（finding 3/4/8 依据） |
| 前置 | `wc -l` 三巨头；`grep -n "auto-detected" scripts/build.ps1`；`sed -n '315,340p' scripts/build.ps1` | 1863/1425/1993；:81 输出串；:322/:332 参数表（P3/P4/P5 通过） |
| S2/S3 | `ls example/`；`grep add_subdirectory`；`git grep '\.\./\.\./' example`；`cat .gitmodules`；`git submodule status`；`ls src/SARibbonBar/3rdparty/`；`grep SARIBBON_BIN_DIR 3rdparty/CMakeLists.txt` | 11 目录/11 子目录；StaticExample:55 唯一；fork url；`-f93657fa`；三内容；:46 上三级路径（全部吻合） |
| S4 | 通读根 `CMakeLists.txt`（246 行）与 `src/SARibbonBar/CMakeLists.txt`（364 行）；`git grep SARIBBON_DOC_FILES`；`cat SARibbonBarVersionInfo.h.in` | S4.1 处置表全部行号吻合；死变量证实；.in 消费 SARIBBON_VERSION_* 证实 |
| S5 | `cat cmake/SARibbonUtils.cmake`+`wc -l`；`git grep saribbon_set_bin_name`；QWK 副本：`sed -n '38,98p;180,215p' src/CMakeLists.txt`、`cat .gitmodules`、`git submodule status`、`sed -n '10,24p' qwkglobal.h`、`cat QWindowKitConfig.cmake.in` | 15(16) 行/零调用；qwk_add_library :38-126 及全部行号引用吻合；qmsetup pin 99ca80fd；三段式 :12-22；Config.in 8 行含两段式 find_dependency |
| S6 | `file` 各关键头/CMakeLists；`sed/awk` 取 SARibbonGlobal.h L17-22/L83-88/L102-125/L184-200/L253-290；`git grep -rl SARibbonGlobal.h`（39=38+清单）；`git grep -rn SARibbonQt5Compat`（5 个 .cpp）；Qt5Compat include 清单与禁词扫描；`sed -n '90,115p' SAColorWidgetsGlobal.h`；目录头 vs 清单差集（comm，修 CRLF 干扰后=MdiControlsStyle.h 唯一）；`git grep "SARibbonBar/i18n\|resource"`；`cat src/CMakeLists.txt`；`sed -n '40,103p' cmake/WinResource.cmake` | BOM/CRLF 事实（Global.h 带 BOM、src/CMakeLists 带 BOM、SAColorWidgetsGlobal 无 BOM）；宏区边界 L20-184/L274-283 精确；move 区段纯净性禁词零命中；WinResource 宏参数签名吻合 |
| S7 | `ls tests/`+`ls tests/*.cpp \| wc -l`+`find tests/auto`+`grep -c add_saribbon_test`；通读 `tests/CMakeLists.txt`；`sed -n '45,50p' tests/ThemeCoverageTest.cpp`；`git grep "src/SARibbonBar" tests`；examples：`git grep SARibbonBar::SARibbonBar`、`git grep "if(NOT TARGET SARibbonBar)"`、MainWindowExample :25-30 | 25+1=26；函数 :7-44、链接 :10、include :17；tests 硬编码 2 处；10 example 全带 fallback |
| S8 | `cat tools/Amalgamate.sh`+`file`；`ls tools/amalgamate/`+`file` ×4；`head` 三模板；`grep -c "src/SARibbonBar/"` 两模板（46/43）；`grep -c` qrc 三件（32/0/0）与 src/SARibbon.cpp（32）/SARibbon.h（0） | 计数全部精确；32 处命中确系内联 rcc 注释（`/*** Start of inlined file: qrc_SARibbonResource_Datas.cpp ***/` 区段内 `// C:/...` 行）；模板头部宏定义证实 |
| S10 | `ls .github/workflows/`；`git grep BUILD_TESTS .github/`（6 处+行号）；`grep -rn submodules`（零）；`head publish-python-bindings.yml`；通读 cmake-linux-qt6.8.yml / cmake-win-qt6.8.yml；`grep cache:` 其余 4 个；`head -40 CMakePresets.json`+`grep -c '"name"'`；`cat vcpkg.json` | 与文档一致；cache 实测 4 true/2 false（finding 43）；win runner python3 问题为平台事实推定（finding 41） |
| S11/S12 | `cat SARibbonBarConfig.cmake.in`（:12 find_dependency）；`ls cmake/SARibbonConfig.cmake.in tools/test-find-package`（均不存在）；`grep -n src/SARibbonBar AGENTS.md`（6 处 L5/15/17/18/19/36）；`sed build.md`（选项表 L120-127、L11/L13）；`grep submodule.md`（L20/L29 stdware）；`ls changlog.md`；`git grep -l src/SARibbonBar docs`（22） | 全部吻合 |
| §6 门禁 | 真跑门禁 grep（含全部 pathspec）+ 按文件 `uniq -c` 分账；`grep -n src/SARibbonBar src/SARibbon.cpp \| head/tail` | 命中分账：.gitmodules 2/AGENTS 6/根 CMakeLists 2/v2 计划 1/src/SARibbon.cpp 32/submodule.md 2/tests 2/Amalgamate.sh 1/amalgamate 89——其中 32+1 两类在完整执行后仍残留（finding 50 依据） |
| include 形态 | `git grep '#include "SA' tests example \| wc -l`（=190）；colorWidgets 引用形态（顶层文件用 `colorWidgets/` 前缀、colorWidgets 内部平铺同目录） | S5.5"190 处平铺 include"精确；零改动传播方案经形态核查成立（同目录 quoted include 在三态下均可解析） |

## 二、修订统计

- 对 `01-infra-restructure.md` 共执行 **38 次编辑**（另 1 次因原文与首读渲染差异未匹配、重试成功），覆盖：头部注、P1/P2、S1、§1 树注、S2 验证、S5.2、S5.4 说明（差集+COMPONENT 两条）、S6.1（骨架+清单注意）、S6.2（Qt5Compat/Config.h.in/转发头）、S6.3、S6.4（代码块+尾段）、S6.5（判据+行数）、S6.6、S6.7、S6.8、S6.9（块 A 引言与注释、块 B 警示、块 C 重排+论证改写）、S7（现状/第 3 条/验证）、S8（现状两条+编码注+残留注）、S9（清单+调用名）、S10（第 1/2/7 条）、S11（开头/骨架/薄壳/冒烟）、§6（purity 门注+残留门禁重写）、§8。
- 缺陷合计 **33 条**（上表 #1~#53 中判为缺陷的行，按主级别计；"通过"行不计）：
  - **阻塞 7**：#3 P1 过时、#4 P2 过时、#8 S1 nothing-to-commit、#31 S6.9 块 C 宏顺序、#41 S10.1 win python3、#42 S10.2×S10.7 ctest 组合、#50 §6 门禁必败；
  - **歧义 11**：#1 头部分支注、#19 S6.1 LIB_NAME/LINKS_PRIVATE 机制、#20 S6.1 清单矛盾、#21 S6.2 Qt5Compat 计数、#23 S6.3 命令口径+编码、#24 S6.4 NO_DLL 语义、#30 S6.9B 占位照抄风险、#39 S9 清单未对齐 v2、#46 S11.1 缺 Core 组件、#48 S11.4 命令必失败、#52 §8 过时；
  - **缺口 5**：#22 S6.2 转发头与 Config.h.in 内容（两处合一）、#25 S6.5 v2-M0 判据登记、#27 S6.7 share 安装规则、#45 S11 时序说明、#47 S11.2 薄壳内容；
  - **瑕疵 10**：#2 §1 树注、#10 S2 --follow 时机、#15 S5.2 行号、#26 S6.6 行号、#28 S6.8 命中面、#33 S7 函数行号、#34 S7.3 grep 口径、#37 S8 模板编码注、#38 S8.3 残留清理、#43 S10.7 cache 声明。
- 未改动任何 S 编号/步骤结构；未触碰 01 与本 findings 之外的任何仓库文件（README/NOTES 的过时项记录于第四节建议 1/2，交终审处理）。

## 三、遗留风险（执行期才能验证）

1. **Amalgamate.exe 的 include 解析行为**（S8 设计的核心假设）：`-i` 目录是否对 `<尖括号>` 形式生效、找不到时是否"原样保留"、按路径去重——均无法离线验证（运行脚本=改状态，被禁）。缓解已内置：S8.5 diff 核对 + StaticExample 编译验收门；若 exe 行为不符，回退方案是给模板顶部手工插入 core 头实体路径（`../../src/core/...`）替代镜像目录。
2. **`git mv` 对未初始化 submodule gitlink 的行为**随 git 版本有差异；S3.3 已备手动回退（rm --cached + 改 .gitmodules + add），风险低。
3. **P3/P5 未真跑**（构建属状态变更）：N₀=26 为注册数推定，实际通过数以执行时为准；build.ps1 的 rebuild 交互挂起风险 README R2 已警示。
4. **空导出 DLL 告警面**：SARibbonCore 占位 .cpp 方案在 MSVC 下的 LNK4088/无导出警告是否干扰 CI（警告非错误，预期可过），执行时确认。
5. **`target_link_libraries(<tgt> PUBLIC <空列表>)`**：sa_add_library 骨架在 QT_LINKS 为空时展开为关键字后零项——三个模块调用均传非空 QT_LINKS，理论不触发；若计划 02+ 出现空参调用需补 if 守卫。
6. **qmsetup 远端行号引用**（`qmsetup:Preprocess.cmake:xx` 等）：本地副本 submodule 为空，无法复核，依赖 round2 的 GitHub API 抓取记录；均为论证性引用，不影响执行动作。
7. **Qt5.15 路径**：本机无 5.15（B5），S4 的 C++17 统一、5.15 门槛、shim 行为在 Qt5 下只由 CI 验证；S6 提交点若 CI 未跑，风险延后到 S10 暴露。
8. **configure_package_config_file 对 `@SARIBBON_BUILD_WIDGETS@` 的代入**：option 变量在调用时点已定义（桌面推演成立），但若执行者把生成逻辑放进 `if(SARIBBON_INSTALL)` 之外的异常作用域导致变量未定义，会代入空串使 `if()` 报错——按 S11.1 指定位置（src/CMakeLists.txt 顶层）写即无此问题。
9. **build.ps1 `rebuild` 附带 install**：S6~S10 期间每次验证都会刷新 `bin_qt6.7.3_MSVC_x64/` 安装树（中间态无 Config，见 S11 时序说明），执行者勿据此树做消费验证。

## 四、对整体计划体系的建议（供终审 agent）

1. **README.md 事实快照已过时**（不在本 agent 权属）："git 基线状态"行仍写 HEAD `7a617fc`、3 个未跟踪项、"dev/v3/origin/dev 三者同指"；"修订历史"停在 round2。建议终审同步更新，并给快照表加"时点声明"列——或改用 P2 式祖先检查表述，避免每轮评审提交后再次过时。
2. **NOTES.md B7 同样停在 round1 时点**，建议追加 B11：记录评审提交链（42b7dcc/eba8ebd/round3）与"执行时点门禁"的解读方式，供 02~04 的同类前置检查引用。
3. **跨文档数字传染风险**：本轮在 01 内修掉的"38 个文件"（Qt5Compat）、"cache:'true'"、":12-21" 类事实错误，可能同样存在于 02/03/04（例如 03 S1 与 01 S8 共享 amalgamate 事实：模板 4 文件、46/43/89 计数、GBK、镜像目录设计——03 若引用需与 01 修订后的口径对齐）。建议终审对四份计划跑一次同模式交叉扫描（重点：计数类、行号类、"已核实"类断言）。
4. **评审提交与计划文档的互踩问题**：本轮 4 条阻塞级缺陷中有 3 条（P1/P2/S1）源于"评审提交入库使文档内的 git 状态断言过时"。建议体系级约定：计划文档中一切 git 状态断言用**相对表述**（祖先检查、diff 为空、"以执行时 git log 为准"），绝对 commit 号只作时点备注。
5. **v2 §6.2 组合构建矩阵的 Qml 轴**：01 只落地 widgets 轴（Qml 在 01 无 target，ON/OFF 等价，做了也白做）；"Widgets=OFF Qml=ON"矩阵项须待 04 建 target 后补——建议终审核对计划 04 是否已显式认领该 CI 增补，否则会成三不管地带。
6. **AGENTS.md 过时窗口**（S6 搬目录 → S12 改文档之间约 6 个提交点）：README R1 的"旧规则自动映射到 src/widgets/"条款已兜底，可接受；若执行期拉长或多 agent 并行，可考虑把 S12.1（AGENTS.md 结构图）提前到 S6 同提交，缩小窗口——属可选优化，非缺陷。
7. **门禁 B 的正则依赖 rcc 注释格式**：`// C:/src/Qt/SARibbon/src/SARibbonBar/` 前缀匹配基于当前 qrc 快照的注释格式；若计划 03 再生 qrc 文件（03 的产物出库改造），注释路径会变化，届时门禁 B 需随之调整——建议 03 执行时复查 01 §6 门禁 B 的豁免模式。
