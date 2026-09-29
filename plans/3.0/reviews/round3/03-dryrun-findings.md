# 计划 03（构建生态收尾）第 3 轮评审记录：首次执行者压力测试

> 评审视角：**假设本人就是被派去执行 `plans/3.0/03-build-ecosystem.md` 的 AI agent**（只有该文档 + plans/3.0/README.md + 仓库访问权，假设计划 01/02 已执行完），对全文做逐节 dry-run：只读命令真跑、脚本 `bash -n`/沙箱实测、sed 无 `-i` 模拟、与 2.9.5 基线真实文件逐一对照、与计划 01/02 正文交叉核对。
> 评审基线：分支 `v3` @ `eba8ebd`（2.9.5 逻辑基线 + 两轮评审修订后的计划文档）；仓库现实路径按"01/02 已执行"假设推演，现实核对用当前路径并注明映射（`src/SARibbonBar`→`src/widgets`、`example`→`examples/widgets`、`tests`→`tests/widgets`）。
> 结论：已直接修订 03 文档（353 行 → 424 行，S 编号未重排，新增均为子条目/S3.0 前置块）；下表为全部发现。级别统计：**阻塞 6，缺口 11，歧义 8，瑕疵 3**（合计 28 条，R3-01～R3-28）。
> round1/round2 findings 仅用于理解意图（如 F13/F14/F24、QWK ctest 纪律的来源），本轮全部结论以当前文档 + 仓库现实独立验证为准。

## 一、逐条发现（位置 | 级别 | 现象与证据 | 处理）

### 阻塞级

| # | 位置 | 级别 | 现象与证据 | 处理 |
|---|------|------|-----------|------|
| R3-01 | S1-2 目标脚本 + S1 现状 + P3 | **阻塞** | 旧稿新 Amalgamate.sh **丢弃了 01-S8 引入的 `_amalg_include` 镜像机制**。证据链：① 01-S8 操作 1 的 OPTS 实为 `-i "../src/widgets" -i "../src/widgets/colorWidgets" -i "../src/core" -i "_amalg_include"`，操作 3 明文"脚本在调用 Amalgamate.exe 前构建镜像、结束后清理"，且解释了原理——`<SARibbonCore/...>` 形态在源码树**物理不存在**，Amalgamate 对解析不到的 include **原样保留**，产物带着无法解析的行 → StaticExample 编译失败；② 01-S6.9 代码块 B/C：widgets 转发头 `SARibbonGlobal.h`/`SARibbonWidgetsGlobal.h` 内容就是 `#include <SARibbonCore/SARibbonCoreGlobal.h>`；③ 02-S1.5：core 头物理在子目录（`src/core/global/` 等）、对外平铺为 `SARibbonCore/<名>.h`——旧稿 `OPTS_CORE='-i "../src/core"'` 连 core 子目录间的平铺 include 都覆盖不了。执行者照旧稿写出的脚本产出的 4 个产物**全部是坏的**，且脚本本身"正常退出"，问题延迟到 S2 编译才爆。03 的 P3 与 S1 现状对 01-S8 的转述也漏了镜像（只列 3 个 `-i`） | 重写 S1-2 目标脚本：内置镜像段（全部 core 头按"保留子目录 + 平铺"双形态复制到 `_amalg_include/SARibbonCore/`；OPTS_CORE/OPTS_WIDGETS 均加 `-i "_amalg_include" -i "_amalg_include/SARibbonCore"`；可选从 `../build*/include/` 拾取 configure_file 产物 SARibbonCoreConfig.h；结束 `rm -rf`）；P3/S1 现状补全 01-S8 事实与"本步必须继承镜像"的明文；镜像循环逻辑已在 `$TEMP` 沙箱真跑验证（双形态复制正确、无 build 树时 `set -e` 下安全跳过） |
| R3-02 | S1 编码警示 + S1-2 | **阻塞** | R1 禁转码令无可执行通道：`tools/Amalgamate.sh` 实测 851 字节、GBK 可解码、UTF-8 解码失败（`0xd3` @771）、LF、无 BOM——而 **AI 执行者的写文件工具一律输出 UTF-8**，旧稿只说"编辑时必须保持 GBK 原编码保存"，没给任何"怎么做到"的办法；且旧稿目标脚本自身含中文注释与 `echo 按任意键继续`，照抄即触发转码事故（整文件 diff + 违反 R1/B4），不抄又无从下手——首次执行者必卡死 | 目标脚本全文 **ASCII 化**（注释/提示改英文；ASCII 字节在 GBK 与 UTF-8 完全一致，任何工具整文件重写都无转码风险，属受控内容重写而非静默转码）；新增"编码操作规程"段：写入后 `file` + `git diff --stat` 复核、NOTES 记录、B4 状态更新；若维护者坚持中文提示给出唯一安全通道（UTF-8 临时文件 + `iconv -f UTF-8 -t GBK` 覆盖），明文禁止编辑工具直接写 UTF-8 中文进该文件；§8 新增 ⑨ |
| R3-03 | P4 + S4 现状尾句 + S4-1 | **阻塞** | 与计划 01 S10-4 **直接矛盾**：01-S10-4 原文"publish-python-bindings.yml：**不改**（原稿'删除 push/tag 触发、改 dispatch-only'与事实不符——本就没有 push/tag 触发，且 dev-3.0 分支上的修改对 master 的 release 触发无效）"；而 03 旧稿 P4 把"仅 workflow_dispatch + TODO 注释（计划 01 S10-4 完成后的状态）"设为前置条件，S4 现状尾句与 S4-1"恢复触发：还原 release"沿用同一错误前提。执行 agent 在 P4 门禁处核对 yml 发现仍是 release+dispatch → 前置条件"失败"→ 按纪律不得继续，卡死；或误以为 01 漏做而去"补做暂停"再"恢复"，白改两轮 | P4 重写为"现状确认"（01 前后该 yml 均保持 release+dispatch 基线原状，本计划 S4 直接适配）；S4 现状尾句重写并给出"若真发现 dispatch-only 态 = 01 执行偏离其正文，按 R4 记 NOTES"的兜底；S4-1 改"触发保持现状"（保留对 tag 触发方案的否决及理由）；§1 目标 2 与 S4 标题的"恢复"措辞同步改；§8 新增 ⑥。注意：`04-qml-and-release.md:408` 引用了 03 S4.1 旧措辞（"属新增行为而非'恢复'"），本轮改为"而非'保持现状'"——结论完全一致，仅引文措辞需终审时同步 04（见 §四.c） |
| R3-04 | S3 全节（S3.2/S3.3/S3.4） | **阻塞** | 三轨绑定的 `<SARibbonCore/...>` 命名空间 include 解析缺失（与 R3-01 同根，但落在绑定侧）：三轨均**直接编译源码树文件**（S3 头部自己写明），include 路径只有 `src/core|widgets|colorWidgets` 类源目录；01/02 之后 widgets 转发头（`SARibbonGlobal.h` 等）内容是 `<SARibbonCore/xxx.h>`，该布局只在主工程 build 树同步目录/安装树存在。01-S6 实测 **38 个文件** include `SARibbonGlobal.h` → sip 编译 sources 清单第一个翻译单元即失败；pyside6 的 `saribbon_lib` 与 shiboken 解析同死。旧稿 S3.2-1 只改 include-dirs 前缀、S3.4-2 只加 `${SARIBBON_CORE_DIR}`，均解析不了命名空间形态；S3.2-1 的"可选方案"（指向 `${CMAKE_BINARY_DIR}/include`）又因破坏轮子自包含被默认否决——即默认路径**没有任何一条能编过**。01-S6.2 只论证了源码树/同步目录/安装树/amalgamate 四种消费场景，**绑定构建是第五种，两份计划都没接住** | 新增 **S3.0 公共前置**（三轨必做）：sip 系 = 仓库根 `build-binding-include/SARibbonCore/` 镜像（复制逻辑同 S1-2 镜像段），include-dirs 追加（pyqt6/ 轨写 `../build-binding-include`），镜像生成挂 `tools/build_python_bindings.bat` 与 publish/dry-run CI wheel 步骤两处，目录进 `.gitignore`；pyside6 = CMakeLists 内 `file(COPY)` 到 `${CMAKE_CURRENT_BINARY_DIR}/_sync_include/SARibbonCore/` 并进 `target_include_directories` + shiboken `-I`（自包含，推荐落点）；project.py 钩子/builder-settings `system()` 列为备选（project.py 是否被加载取决于 `module-name` 键，现 toml 未设置、大概率死配置，与既有"待核实"项并验）；S3.2-1/S3.3 轨B/S3.4-2/S4-2/S4-3 全部联动补镜像引用；风险表加行 |
| R3-05 | S6-5 复核命令 | **阻塞**（验证假绿） | `git grep -n "SARibbonBar)" tests/` 与真实代码形态不符：实测链接目标是 `target_link_libraries(${TEST_NAME} PRIVATE` 后**独立一行的裸 `SARibbonBar`**（tests/CMakeLists.txt:10，行内无右括号），另一处是 `$<TARGET_FILE:SARibbonBar>`（:28，带 `>` 不带 `)`）——该 pattern **改动前后都零命中**，"复核清零"恒成立 = 假绿；且 `SARibbonBarLayoutRTLTest` 等测试名合法含 "SARibbonBar"，宽 grep 也不能作清零判据。执行者会以为切换完成而实际一行没改 | S6-5 重写：列明两处真实位置与改法（裸名 → `SARibbon::Widgets`；`TARGET_FILE:SARibbonBar` → `TARGET_FILE:SARibbonWidgets`；include 路径行已由 01-S7 删除无需处理）；复核命令改 `git grep -nE "^[[:space:]]+SARibbonBar[[:space:]]*$\|TARGET_FILE:SARibbonBar" tests/` 期望零命中，并写明旧命令为何无效 |
| R3-06 | S5-1 core 黄金测试行 | **阻塞**（假绿回归） | 旧稿复核口径写 `ctest -R core`，而 02-S9 原文明确"**用 LABELS 过滤而非 `-R "core"`**：现有测试以文件名注册、名称不含 core 字样，`-R core` 将匹配不到任何测试而'假绿'"并要求 `ctest --test-dir <build> -C Release -L core`。执行 agent 按 03 复核时会把 02 落地的 `-L core` "纠正"回 `-R core`，重新引入 02 专门消除的假绿 | 改为 `ctest -L core`（LABELS 过滤），并写明"不要写成/改回 -R core"及理由，与 02-S9 口径显式对齐 |

### 缺口级

| # | 位置 | 级别 | 现象与证据 | 处理 |
|---|------|------|-----------|------|
| R3-07 | S1-2 脚本错误处理 | 缺口 | 旧稿脚本无 `set -e`、无产物校验：Amalgamate.exe 缺失/失败、模板名打错时静默继续（convert_to_crlf 内部 `-f` 守卫跳过），末尾退出码 0 → S5-2 amalgamation job 假绿；2.9.5 原脚本反而有 Warning 分支。单文件产物是计划 04 的 Release 物料，静默半残（如只出 2/4 个文件）无人发现 | 目标脚本加 `set -e` + 4 产物存在性显式检查（缺失即 `exit 1` 并清理镜像）；S5-2 步骤说明同步提及"依赖 S1-2 的非交互**与 set -e** 改造" |
| R3-08 | S2-3 | 缺口 | 实测 `example/StaticExample/MainWindow.cpp:2` 也有 `#include "SARibbon.h"`，旧稿只列 `MainWindow.h:5`。漏改后**开发机因工作区残留旧产物可能照常编过**（S1-3-1 明文保留工作区文件），fresh clone/CI 才暴露——恰是最难排查的形态。S1-5 表写 "MainWindow.*" 但执行者按 S2 操作条文干活 | S2-3 改为"MainWindow.h:5 **与 MainWindow.cpp:2** 两处"，写明残留掩盖风险；S1-5 表 StaticExample 行同步加粗两处 include |
| R3-09 | S1-3 与 S2 的提交时序（R3 纪律） | 缺口 | S1-3 出库动作若先于 S2 单独提交：旧 `src/SARibbon.h/.cpp` 取消跟踪，StaticExample 仍无守卫引用它们 → **fresh clone 默认配置（Examples=ON）构建必失败**，违反 R3"每步结束可构建"（6 个 CI workflow 均 Examples=OFF 所以 CI 绿，恰恰掩盖问题；子模块消费者/新环境直接踩雷）。S1 验证只测 configure+install 不编 examples，测不出来 | S1-3 后新增"提交时序"段：推荐两笔提交序——提交① = S1-1/S1-2 + S1-3-2（.gitignore 先行）+ S1-4/S1-5；提交② = S2 全部 + S1-3-1/3/4；逐点自洽性论证写入；S2 提交行加交叉引用 |
| R3-10 | S3.1 盘点命令 | 缺口 | 宽 pattern `SARibbonBar\|src/SARibbon` 实测 **313 行**命中，其中真路径引用仅 **260 行**（86×3 份 toml + MANIFEST.in 1 + pyside6/CMakeLists.txt 1），其余 ~53 行全是类名噪音（.sip `%TypeHeaderCode #include <SARibbonBar.h>`、`%Include SARibbonBar.sip`、typesystem `<object-type name="SARibbonBar">`、docs `saribbon.SARibbonBar`）——`SARibbonBar` 类名与头文件名 3.0 **不变**。无预期清单时执行者要么误改类名（破坏绑定），要么无法判断盘点是否完整 | S3.1 改窄/宽双命令（窄 `src/SARibbonBar` = 必改项）+ 逐文件预期命中表（含构成拆解与"噪音均不改"结论）；NOTES 贴盘点结果的要求保留 |
| R3-11 | S3.2-2 diff 复核命令 | 缺口 | 命令 bug 两处：① git pathspec 的 `*` **跨目录**匹配——实测 `git ls-files 'src/SARibbonBar/*.cpp'` 命中 colorWidgets 下 6 个文件，故 `'src/widgets/*.cpp' 'src/widgets/colorWidgets/*.cpp'` 会把 colorWidgets 计两次，diff 左侧重复 → 恒有噪音；② 左侧缺 `src/core/*.cpp`——02 下沉后 core 源全部要编进绑定（旧稿只在括号里说"core 的 .cpp 若被 widgets 头引用也必须进 sources"，实际是**全部**必须进，缺一即链接期未定义符号） | 命令改为 `diff <(git ls-files 'src/widgets/*.cpp' 'src/core/*.cpp' \| xargs -n1 basename \| sort -u) <(grep -o '[A-Za-z0-9_]*\.cpp' pyproject.toml \| sort -u)`，附 pathspec 跨目录实测证据；§8 新增 ⑧ |
| R3-12 | S1 验证 + §6 验收门第 1 条 | 缺口 | `git ls-files src/ \| grep SARibbon` 形态的判定不可用：01/02 之后 `src/widgets/SARibbonBar.h`、`src/core/SARibbon*.h` 等**数十个正常类头**都会命中，"无单文件"永远判不过（或诱导执行者误删类头）。验收门"`git ls-files src/` 不含任何 `SARibbon*.h/.cpp` 单文件"同病 | 两处均改顶层 pathspec：`git ls-files 'src/SARibbon*.h' 'src/SARibbon*.cpp'` 期望**空输出**（`src/SARibbon*` 前缀只匹配顶层产物，不会命中 `src/widgets/...`），并写明旧形态为何歧义 |
| R3-13 | S1-5 盘点表 | 缺口 | 真跑 S1-5 grep（排除产物自身）后，表外命中 3 类无处置指引：`changlog.md:54/:268/:426`（历史条目——需明说**不改**，否则执行者面对"逐项核对"要求无所适从）、`AGENTS.md:5/:16`（应指向本节步骤 4）、`tools/Amalgamate.md:82/:86`（应指向步骤 2 的同步更新）及脚本/模板本体自引用；另 readme 行号错位：实测 `readme.md:114`、`readme-cn.md:115`（旧稿写 "readme.md:115"） | 表补 4 行（changlog 不改/AGENTS/Amalgamate.md/脚本模板本体）+ 行号修正 + pyproject 行注明"本步 grep 对其零命中属正常（它们引用的是目录形式，归 S3.1）" |
| R3-14 | S4-3 dry-run job | 缺口 | 三连 import 冒烟意味着同一 Python 环境同时装 PyQt5+PyQt6+PySide6：pip 包名互不冲突，但 Qt5/Qt6 运行库与平台插件路径存在互相干扰可能（尤其 Linux；wheel 构建步骤也未提示需要 S3.0 镜像前置——sip 两轨在 CI 里 `python -m build` 前无人生成 `build-binding-include/`） | S4-3 加"每轨独立 venv 或拆 3 个 job"建议与排查顺序（先拆环境再怀疑绑定）；S4-3 步骤与 S4-2 各加镜像前置步说明（build-pyqt5/build-pyqt6 两 job 在 Build wheel 前加复制 step，build-pyside6 无需） |
| R3-15 | S2-2 EXISTS 守卫 | 缺口 | standalone 场景行为差异未说明：`cmake -S examples/widgets/StaticExample` 时该 CMakeLists 是**顶层**文件，`return()` 直接终止 configure 且不生成构建系统，后续 `cmake --build` 报"无构建文件"类错误——与 add_subdirectory 场景的"WARNING 跳过"完全不同。S2 验证条文"删除生成的单文件再 configure，得到 WARNING 跳过"若被理解为 standalone 验证，执行者会误判守卫写错 | 补"standalone 场景行为差异"说明：验证以常规 examples 构建为准；CI amalgamation job 总是先生成单文件、守卫不触发 |
| R3-16 | S1-1 模板枚举指令 | 缺口 | "枚举全部公共头"与现模板集合不符：实测现 PublicHeaders（46 条 include）**有意不含** `SARibbonMdiControlsStyle.h`（其内容经 .cpp include 链内联进 .cpp 产物侧；.cpp 模板 43 个源文件则齐全）。机械按 `git ls-files` 补齐会把内部头加进 .h 产物、改变产物结构；另外 02 后 core 头在子目录，模板须写**真实子目录路径**（`../../src/core/global/...`），旧稿"枚举 `../../src/core/` 全部公共头"暗示平铺形态，照写即找不到文件 | Widgets 侧改"以现模板既有条目集合为基础调整（下沉 core 的条目删除、其余只改前缀、**不机械新增**）"，给出增补判据（是否进 install 公共头清单）；Core 侧写明子目录真实路径 + `git ls-files src/core` 为准 + `SARibbonCoreConfig.h`（build 树产物）例外核对命令；Widgets .cpp 侧同样注明子目录路径与"与模块 CMakeLists 源清单交叉核对" |
| R3-17 | S3.4 现状/冒烟 | 缺口 | 两处与实测不符：① "生成 26 个 wrapper"——实测 typesystem 为 **25** 个 object/value 类型（23 `<object-type>` + 2 `<value-type>`，另 15 `<enum-type>`）；② "期望末段提示 `python -c ...` 通过"——实测 `build_pyside6_bindings.bat` 末段只**打印** `Verify: python -c "from PySideSARibbon import saribbon; print('OK')"` 提示行，**不自动执行**，执行者等不到"通过"信号 | 数字修正（§8 新增 ⑦）；冒烟表述改"需手动运行该命令确认输出 OK"；顺带补 `SKBUILD_PLATLIB_DIR` 实测行号 :424-425 |

### 歧义级

| # | 位置 | 级别 | 现象与证据 | 处理 |
|---|------|------|-----------|------|
| R3-18 | S1-3-3 .gitattributes | 歧义 | 实测第二行原文为 `src/SARibbon.h   -text`（三个对齐空格，旧稿引用无空格），且两行上方有段落注释 `# --- amalgamate 工具生成的合并文件：字节冻结...---`；按旧稿"删除两行"会留孤儿注释，Edit 类工具按旧稿文本精确匹配会失败 | 写明两行实测原文（含对齐空格）+"整段注释一并删除" |
| R3-19 | P5 | 歧义 | P5 命令用 `python`，01-S9/S10-1 统一 `python3`（Linux runner 无 `python` 命令）——本机 Windows 都能跑，但照 P5 抄进 CI step 会挂 | P5 注明"本机 Windows 用 python；CI step 一律 python3，与 01 调用名统一" |
| R3-20 | P6 | 歧义 | "sip **两套** pyproject 的 sources 清单缺..."——实测缺失存在于**三处**（根 pyproject.toml、pyproject-pyqt6.toml、pyqt6/pyproject.toml，三份对 ThemeManager/ThemePalette/MdiControlsStyle 的 grep 均零命中），与 S3.2-2 自己的"三个 sip 系 pyproject"口径打架 | 改"三处"并列实名 |
| R3-21 | S5-3 preset 清单 | 歧义 | 漏抽象基础 preset `vcpkg-base`（实测 CMakePresets.json 共 6 条：base + release/debug/release-static/debug-static/debug-frameless；version 6 + cmakeMinimumRequired 3.25 与文档一致） | 补 vcpkg-base 与总数 |
| R3-22 | S5-4 ctest 范围 | 歧义 | "6 个平台 workflow **与 amalgamation job** 的所有 ctest 调用"——S5-2 的 amalgamation job 设计里**没有 ctest 步骤**（只构建 StaticExample + 可选冒烟），执行者会白找；且 02-S9 新增的 core step 才是本步最该核对的新增调用点 | 改为"6 个平台 workflow 的全部 ctest 调用（含 02-S9 core step 与本步新增矩阵项）；amalgamation/dry-run job 现设计无 ctest，日后若加也须带 --no-tests=error" |
| R3-23 | S5-2 `<Qt>` 占位符 | 歧义 | `-DCMAKE_PREFIX_PATH=<Qt>` 未说取值来源，首次执行者需自己猜 | 补：`$QT_ROOT_DIR`（jurplel action 设置）或 `${{ github.workspace }}/Qt/6.8.x/msvc2022_64`；也可不传——action 已把 Qt bin 加入 PATH，CMake 沿 PATH 定位 Qt6Config（现有 6 workflow 即此机制，实测其 configure 行均未传 PREFIX_PATH） |
| R3-24 | S6-3 转发头/兼容包 | 歧义 | "复核**每个公共头**都有转发"强于 01-S11.3 实际机制（按 `SARIBBON_HEADER_FILES` 逐头生成，**colorWidgets 子目录转发是可选分支**"如也需要…同法生成"）；且清单漏了 `lib/cmake/SARibbonBar/` 兼容薄壳（01-S11.2/11.5 有、03 复核清单无） | 改为"以 01 执行时实际取舍为准"；补兼容薄壳复核项 |
| R3-25 | S6-4 i18n.md | 歧义 | 只提 `:/i18n/` 示例（实测 zh 版 :32）；同文件 :21/:83/:85 还有 `build-dir/src/SARibbonBar/i18n`、`src/SARibbonBar/i18n` 旧路径叙述（en 版同构），按旧稿改完仍留旧路径 | 补第 ② 项旧路径同步（含实测行号） |

### 瑕疵级

| # | 位置 | 级别 | 现象与证据 | 处理 |
|---|------|------|-----------|------|
| R3-26 | S3.6 行号 | 瑕疵 | "build-python-bindings.md:120-171"——实测替换流程描述在 zh 版 :147（"脚本会自动：1. 将 pyproject-pyqt6.toml 临时替换…"）与 :171-177（手动 copy 三步） | 行号修正，并补"这 4 篇对 src/SARibbonBar 路径引用为零（S3.1 窄版实测），docs 命中全是类名"的事实，避免执行者在 docs 里白找路径 |
| R3-27 | S3.3 "近乎同文" | 瑕疵 | 实测 `diff -u sip/SARibbonBar.sip pyqt6/sip/SARibbonBar.sip` **零差异**（完全同文，非"近乎"） | 措辞改"同文（抽查实测零差异）" |
| R3-28 | S7-1 草稿格式 | 瑕疵 | `## 未发布 -> 3.0.0` 与现有 `## YYYY-MM-DD -> X.Y.Z` 格式的偏差未说明何时收敛 | 补"计划 04 定稿时替换为实际发布日期" |

### 核对通过、无需修改的关键声明（抽样列举，全部实测）

- S1 现状：4 模板文件名/职责表 ✓；模板编码 UTF-8（.h 与 PublicHeaders 带 BOM，.cpp/Glue 无）；qrc 三文件在 `tools/` 下、以 `../qrc_*.cpp` 被模板引用 ✓；`src/SARibbon.h/.cpp` 被 git 跟踪 ✓；`.gitignore` 无对应条目 ✓；amalgamate install 规则位于 `src/SARibbonBar/CMakeLists.txt:305-313`（文档写 :304-314，容差内）✓；产物无生成横幅（首行 BOM+`#ifndef SA_RIBBON_H`）✓；01-S8 的 sed 表达式对真实模板有效（无 `-i` 模拟：PublicHeaders 46 处、cpp 43 处，与 01-S8 数字一致）✓。
- S2 现状：`SARIBBON_DIR=../../src`、`SARIBBON_SIMPLE`、`MainWindow.h:5`、`example/CMakeLists.txt:13 add_subdirectory(StaticExample)`、QWindowKit 段与 `SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0` ✓；01-S2/S8 已负责深度改 `../../../src` ✓。
- S3 头部表格：三轨模块声明（`sip/SARibbon.sip:1`、`pyqt6/sip/SARibbon.sip:1`、`typesystem package="PySideSARibbon.saribbon"` :2）✓；14+14 个 .sip ✓；`pyexamples/pyqt6/ribbon_demo.py:43` ✓；bat 脚本流程（toml 替换/恢复、sip-build 参数、copy 清单、末行冒烟）✓；headers 44/sources 40、pyside6 44 头/43 源（:102-103、:343）✓；三处 toml 缺 Theme*/MdiControlsStyle ✓；`project.py` 无路径引用 ✓。
- S4 现状：触发器/3×9 矩阵/Qt 版本（aqtinstall 6.8.3、5.15.2）/ilammy/OIDC `environment: pypi`/三轨 wheel 路径 ✓；workflow 正文零 `src/SARibbonBar` 引用 ✓。
- S5 现状：6 workflow 同构、matrix 维度、mac-qt5.15 `[13]`、win-qt5.15 无 arch、apt 包差异（qt6.8 多 libxcb-cursor0）、configure 四选项、ctest offscreen、不用 vcpkg ✓；`SARIBBON_BUILD_STATIC_LIBS` 决定库类型（:130-136）、`BUILD_SHARED_LIBS` 无消费点 ✓。
- S6 现状：翻译块机制全描述（option/LinguistTools QUIET/create vs add_translation/QM target_sources/install bin/translations/POST_BUILD）✓；库内无 QTranslator（零命中）✓；`SARibbonBarConfig.cmake.in` 实名 ✓；`SARIBBON_DOC_FILES` 只 set（:240）未 install ✓；`tools/test-find-package/` 不存在、归 01-S11 ✓；`.gitattributes` 有 `*.ts text eol=lf` ✓。
- S7 现状：`changlog.md` 实名与格式 ✓；版本唯一来源 :7-13（2.9.5）✓；VersionInfo `configure_file` :203-206 且 .h/.h.in 均被跟踪 ✓；6 处 2.8.0（4 toml + pyside6/CMakeLists :3 + vcpkg.json）✓ = 8 处收口清单成立；docs build-guide 两语言同名 5 篇 ✓。
- 验收门 grep：`git grep -rn "src/SARibbonBar" -- ':!plans' ':!docs' ':!changlog.md' ':!*.md' ':!tools/qrc_SARibbonResource*'` 实测可运行；当前命中 13 文件，其中 `.gitmodules`（01-S3 迁 `3rdparty/` 后归零）、`src/SARibbon.cpp`（S1 出库后不被索引）、tests/CMakeLists 与 ThemeCoverageTest（01-S7）、根 CMakeLists（01-S4/S6）、tools 脚本与模板（本计划 S1）、MANIFEST/pyprojects/pyside6（本计划 S3）——**排除清单闭合，无未分配归属的命中** ✓。

## 二、真跑命令与结果摘要

（全部只读/沙箱，未写仓库文件、未执行任何状态变更命令）

1. **编码/字节事实**：python 读 `tools/Amalgamate.sh` → 851B、GBK 可解码、UTF-8 失败（0xd3@771）、LF×26、无 BOM；4 个模板 → .h/PublicHeaders 带 BOM、全部 UTF-8 可解码。
2. **`bash -n` 语法校验**：旧稿目标脚本（herestring 方式）通过；**修订后目标脚本**（awk 从文档提取 66 行）通过 + 纯 ASCII 校验通过。
3. **镜像循环沙箱实测**（`$TEMP/amalgtest` 伪造 `src/core/{global,metrics}` 结构）：`find|while read` 复制段输出"子目录形态 + 平铺形态"双份正确；`../build*` 通配无匹配时在 `set -e` 下安全跳过（exit 0）。
4. **sed 无 `-i` 模拟**（01-S8 表达式 `s|src/SARibbonBar/|src/widgets/|g`）：PublicHeaders 46 处、模板 cpp 43 处，与 01-S8 声明一致。
5. **模板 vs 源码清单比对**（basename sort/comm，注意模板 CRLF 需 `tr -d '\r'`）：PublicHeaders 缺且仅缺 `SARibbonMdiControlsStyle.h`；模板 cpp 与 src 全部 43 个 .cpp 一一对应 + qrc 3 件。
6. **git 事实**：`git ls-files src/SARibbon.h src/SARibbon.cpp` 命中；`.gitignore` 无 SARibbon 条目、`*.qm` 在 :121；`.gitattributes` 冻结两行（.h 行含对齐空格）；`git tag -l` 全 v2.x（最新 v2.9.5）；VersionInfo .h/.h.in 均被跟踪。
7. **盘点 grep 真跑**：S1-5 命令 → 命中清单见 R3-13；S3.1 窄版 260 行（86×3+1+1）/宽版 313 行；验收门命令 → 13 文件（归属闭合）；`git grep -n "SARibbonBar)" tests/` → **0 命中**（R3-05 证据）；`git grep -c add_saribbon_test tests/CMakeLists.txt` → 27（26 注册 + 1 函数定义，与 N₀=26 口径一致）。
8. **pathspec 跨目录实验**：`git ls-files 'src/SARibbonBar/*.cpp'` 命中 colorWidgets 6 文件（R3-11 证据）。
9. **绑定文件逐个读**：3 份 sip toml 全量、project.py、pyside6/{pyproject.toml,CMakeLists.txt 关键段,typesystem 计数,PySideSARibbon/}、两个 bat 全文、14+14 .sip 清单与 `%Module`/`%TypeHeaderCode`/`%Include` 形态、`diff -u` 抽查零差异。
10. **CI/构建文件逐个读**：publish yml 全文、6 个 cmake-*.yml 的 matrix/arch/apt/configure/ctest 行、CMakePresets.json（python json 解析）、vcpkg.json、根 CMakeLists 关键行（:5/:7-13/:203-206/:240）、src/SARibbonBar/CMakeLists :125-360、example/CMakeLists、StaticExample 全文、tests/CMakeLists :1-45、scripts 无涉。
11. **docs/pyexamples**：python-guide 4×2 与 build-guide 5×2 存在性；i18n.md :21/:32/:83/:85；build-python-bindings.md :147/:171-177；readme.md:114/readme-cn.md:115；`pyexamples/{pyqt5,pyqt6}/ribbon_demo.py`、`pyside6/{ribbon_demo,test_binding}.py`。
12. **跨计划文本核对**：01-S6.2/S6.9（代码块 A/B/C 与转发头 include 形态）、01-S8 全文、01-S10-1/2/4/7、01-S11.1-11.5、01-S12、01-S7（tests 搬迁与函数迁移）、02-S1.4/1.5（子目录+平铺决策）、02-S9（-L core）、NOTES B4、round1/03-findings（F1-F40 意图）、round2 qwk/synthesis（S5-4/S5-5/S6-3 来源）。

## 三、修订统计

- 文档 353 → **424 行**；编辑 41 处（含对新增文本的 2 处自我校正）；S 编号未重排（新增 S3.0 为 S3 内前置子块，S3.1~S3.6 原编号不变）。
- 级别分布：阻塞 6（R3-01~06）、缺口 11（R3-07~17）、歧义 8（R3-18~25）、瑕疵 3（R3-26~28）；全部 28 条已在 03 文档内落地修订。
- 新增/重写的成块内容：S1-2 目标脚本全文（镜像+set -e+ASCII+存在性校验）与"编码操作规程"；S1-3"提交时序"；S3.0"公共前置"；S3.1 窄/宽双命令+预期命中表；S6-5 全文重写；风险表 +4 行；§8 +④项（⑥⑦⑧⑨）。

## 四、遗留风险（需真实构建环境才能验证）

1. **Amalgamate.exe 对镜像双形态 `-i` 的实际行为**：同一头经"实体路径 + 镜像路径"各内联一次时的产物膨胀幅度、`-s` 对 `<SARibbonCore/...>` 的解析顺序——只能 01/02 落地后实跑；已给"产物内容健全性 grep"硬门兜底（S1 验证/验收门）。
2. **sipbuild 配置双源**（project.py vs pyproject.toml `[tool.sip]`）的实际优先级与 `module-name` 键行为——S3.1 既有"待核实"项，`sip-build --verbose` 定案；S3.0 的镜像方案不依赖 project.py 生效（默认走 bat/CI 前置步骤）。
3. **builder-settings RESOURCES 相对基准**：`../../src/widgets/SARibbonResource.qrc` 在三轨各自 sipbuild 生成工程目录下的解析、qrc 注册后主题 QSS/图标可加载——S3.2 既有"待核实"项，冒烟时验证。
4. **三轨轮子同环境并存**（S4-3）：venv 隔离建议未实测，PyQt5/PyQt6/PySide6 在 ubuntu-latest 同环境的插件路径冲突与否需 CI 实跑。
5. **static 矩阵项联动**：`SARIBBON_BUILD_STATIC_LIBS=ON` 下 tests 的 POST_BUILD DLL copy 分支、core-only 项与 static 项组合的行为，需 CI 实跑。
6. **vcpkg preset 实配**：需 `VCPKG_ROOT` 环境与网络拉取依赖，本机未验证（01-S10 已有"无环境降级为 JSON 语法核查"的出口，03 沿用）。
7. **lupdate 目录搬移后的 .ts diff 噪音**（S6-2）：需 Qt Linguist Tools 环境跑 `-DSARIBBON_UPDATE_TRANSLATIONS=ON` 构建核对。
8. **GBK 控制台兼容**：ASCII 化脚本在 chcp 936/65001 控制台下的输出显示（风险极低，英文提示无编码依赖）。

## 五、对整体计划体系的建议（供终审 agent）

1. **（跨计划缺口，建议裁决）绑定构建是"第五种消费场景"**：01-S6.2 论证 include 形态时只覆盖源码树/同步目录/安装树/amalgamate 四态；sip×2 + pyside6 三轨"直接编译源码树"的绑定构建既不在其中，01（明文不动绑定文件）与 02（只在 S1.1-4 要求"跑通 sip 冒烟"）都没接住。03-S3.0 已按"绑定侧自建镜像"补上，但终审应确认该决策与 01-S5.5/S6.2 的 include 策略在文档层面互认（建议在 01 或 NOTES 加一句指向 03-S3.0）。
2. **（02 需修正）计划 02 S1.1-4 的 sip 冒烟在 01→03 窗口不可行**：01-S6 搬目录后三份 sip toml 的 include-dirs/清单全部指向已不存在的 `src/SARibbonBar`，绑定在 03-S3 之前**不可构建**——02 执行期的"sip/PyQt 构建冒烟"必然失败。建议终审把 02-S1.1-4 的验证降级为 C++ 侧（QMetaEnum grep + ctest），sip 冒烟顺延至 03-S3；或把 03-S3 的"纯路径前缀迁移"（不含清单补齐/镜像）提前到 01-S6 之后立即执行。
3. **（02 内部不一致）命名空间 include 形态两个答案**：02-S1 第 4 条写 `#include <SARibbonCore/global/SARibbonEnums.h>`（带子目录），第 5 条平铺决策写 `<SARibbonCore/SARibbonEnums.h>`——03 的 S1/S3.0 镜像已按"双形态兼容"设计（子目录+平铺都复制），无论 02 选哪种都能跑，但 02 执行 agent 需要唯一答案，建议 02 评审侧统一。
4. **（04 措辞同步）** `04-qml-and-release.md:408` 引用了 03 S4.1 旧措辞（"还原 release…"“属新增行为而非'恢复'"）——本轮已把 03 S4.1 改为"触发保持现状"（结论不变：release+dispatch、否决 tag 触发），04 的引文与结论仍兼容，但引用文字建议终审时顺手对齐。
5. **（正面确认）** 01-S8 的模板 sed 数字（46/43 处）、01-S10 的 workflow 现状描述、02-S9 的 `-L core` 决策、NOTES B4/B8 编码与测试数口径，本轮全部实测复核**成立**——三轮修订后的 01/02 事实层质量较高，03 的问题集中在"对 01/02 已演进机制的转述滞后"（镜像、-L core、publish yml 不改）与"命令未经真跑"（S6-5 grep、S3.2-2 diff、ls-files 判定）两类，终审可按此模式抽查 04。
