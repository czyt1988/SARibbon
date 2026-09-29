# 计划 03（构建生态收尾）第 1 轮评审记录

> 评审视角：事实准确性与可执行信息完备度
> 评审对象：`plans/3.0/03-build-ecosystem.md`（修订前版本，154 行）
> 基线：分支 `v3` 工作区（2.9.5 逻辑基线；`src/core`、`tools/check_core_purity.py`、`tools/test-find-package/` 等均为计划 01/02 的未来产物，本评审以"03 执行时点 = 01/02 完成后"为参照，同时对 2.9.5 现状逐文件核实）
> 结论：已直接修订 03 文档（154 行 → 349 行，S 编号未重排，新增均为子条目）；下表为全部发现。类型统计：**错误 10，缺失 25，存疑 5**（合计 40 条，F1–F40）。

## 一、头部 / 目标 / 前置条件 / 纪律

| # | 位置 | 问题类型 | 证据 | 处理 |
|---|------|---------|------|------|
| F1 | 头部"设计依据：…v1 计划 §5.4、§7.1–§7.4"及正文 4 处 `v1 §7.2/§7.3` | 错误（悬空引用） | v1 计划文件不存在于仓库与 git 历史（`git log --all` 无该文件；仅 `SARibbon-3.0-plan-v2.md`，其 §6.1 只说"维持 v1 设计，不赘述"） | 头部新增"v1 引用内联说明"块，把 §7.2=产物出库、§7.3=Python 包名不变、§7.2/§7.4=QML 不合并、§5.4=绑定适配范围、§7.1/§7.4/§7.5 的落点（01-S11/01-S10/04/README R6）内联为【内联-1..5】；正文改引内联条目 |
| F2 | §4 "README.md R1–R4 全部适用" | 错误 | `plans/3.0/README.md` 实际有 **R1–R6**（R5 术语、R6 双分支与同步） | 改为 R1–R6，并点名 R5/R6 与本计划的关联 |
| F3 | P4 "publish workflow 已暂停：仅 workflow_dispatch 触发" | 缺失（未注明状态来源） | `.github/workflows/publish-python-bindings.yml` 现状（2.9.5）触发器为 `release: types:[published]` + `workflow_dispatch`；该文件唯一提交 6f32beb 起从未有 tag 触发。"仅 dispatch"是计划 01 S10-4 执行**后**的状态 | P4 补注来源（01-S10-4）与基线原状，避免执行 agent 在 01 未完成时误判 |
| F4 | P3/P5 | 缺失（同上） | `src/SARibbon.h/.cpp` 现被 git 跟踪（`git ls-files` 命中）；`tools/check_core_purity.py` 现不存在（01-S9 创建） | P3 补 01-S8 的 OPTS 细节；P5 补"脚本由 01-S9 创建" |
| F5 | §3 前置条件 | 缺失 | 三套绑定现状：均从源码树分散文件构建（非单文件）、版本停 2.8.0、sip 系 sources 清单缺 2.9.x 新文件（见 F14） | 新增 P6"绑定基线认知"，防止执行 agent 按"绑定=单文件消费者"的错误模型操作 |

## 二、S1 amalgamate

| # | 位置 | 问题类型 | 证据 | 处理 |
|---|------|---------|------|------|
| F6 | S1-1 "新增模板 SARibbonCoreAmalgamTemplate.h/.cpp" | 缺失（模板机制描述不足） | 现有模板是 **4 文件一套**：`tools/amalgamate/SARibbonAmalgamTemplate{.h,.cpp}` + `SARibbonAmalgamTemplateHeaderGlue.h` + `SARibbonAmalgamTemplatePublicHeaders.h`；机制=PublicHeaders 枚举全部头 + cpp 模板 `/*@remap ... */` 防重复合并 + HeaderGlue 间接层（`tools/Amalgamate.md:69-103`）；只新增 .h/.cpp 两个文件不可执行 | S1 增"现状"小节（4 文件表格）+ 操作 1 细化为 Core/Widgets 各 4 文件的内容要点（guard、宏、PublicHeaders 枚举序、@remap 行、qrc 段仅 Widgets 保留、嵌套 include 使单条 @remap 覆盖 core） |
| F7 | S1-2 "Amalgamate.sh 改为双产物…保留 CRLF 转换逻辑" | 缺失（脚本关键事实未提） | 实读 `tools/Amalgamate.sh`：① 现 OPTS=`-i "../src/SARibbonBar" -i "../src/SARibbonBar/colorWidgets" -w "*.cpp;*.h;*.hpp" -s`，`DEST=../src`，cwd 必须是 `tools/`；② CRLF 转换存在（awk `sub(/$/,"\r")`，仅当产物存在时执行）；③ **末尾无条件 `read -n 1` 交互等待，CI 必挂死**；④ `./Amalgamate.exe` 为 Windows 二进制（仓库无其源码，重编方法在 Amalgamate.md） | S1-2 给出完整新脚本代码块（双产物 4 次调用 + CRLF 循环 4 文件 + `if [ -t 0 ]` 守卫 read）；exe 平台限制写入 S1 现状与 S5-2（amalgamation job 定 windows-latest） |
| F8 | S1-3 产物出库（git rm --cached + .gitignore） | 缺失（两处联动遗漏） | ① `.gitattributes` 有 `src/SARibbon.cpp -text`、`src/SARibbon.h -text` 字节冻结行（出库后成僵尸条目）；② `src/SARibbonBar/CMakeLists.txt:304-314` `install(FILES ${SARIBBON_AMALGAMATE_FILES} DESTINATION share/SARibbonBar_amalgamate)`——出库后 fresh clone `cmake --install` 因文件缺失直接失败 | S1-3 扩为 4 个动作（rm --cached / .gitignore 6 行含旧 2 文件 / 删 .gitattributes 冻结行 / 移除或 EXISTS 守卫 share 安装规则），并把 install 通过纳入验证与验收门 |
| F9 | S1-5 盘点（"MANIFEST.in、pyproject、CI"） | 缺失（盘点清单不全） | 实测 `git grep` 单文件/旧路径引用点还有：`readme.md:115`、`readme-cn.md`（"引入 src 下 SARibbon.h/.cpp 即可使用"）、`example/StaticExample/{CMakeLists.txt,MainWindow.h,README.md}`、`tools/Amalgamate.md`、`.gitattributes`、`tools/qrc_SARibbonResource_Datas.cpp`（rcc 注释内旧绝对路径）、`tests/CMakeLists.txt:17`、根 `CMakeLists.txt:204-205`（VersionInfo 路径）；`MANIFEST.in` 实际不含单文件引用（`recursive-include src/SARibbonBar *.h *.hpp *.cpp *.qrc`，需随目录迁移改） | S1-5 改为逐点处理表（含各点归属：本步/S2/S3/01/02）；MANIFEST.in 处理方式具体化 |
| F10 | S1 验证 "git status 不显示它们" | 缺失（验证不充分） | `git status` 不显示还可能是文件根本没生成 | 验证补 `git ls-files src/`、`bash Amalgamate.sh < /dev/null` 非交互退出、fresh-clone install |
| F11 | S1（整体） | 存疑（生成物标识） | 现产物 `src/SARibbon.h` 头部无"generated"横幅（首行即 `#ifndef SA_RIBBON_H`，来自模板本体） | 模板要点中加"可选：横幅注释"（出库后无字节约束） |

## 三、S2 StaticExample

| # | 位置 | 问题类型 | 证据 | 处理 |
|---|------|---------|------|------|
| F12 | S2 全部 | 错误（现状描述与路径深度缺失）+ 缺失 | 实读 `example/StaticExample/CMakeLists.txt`：独立工程（`project(StaticExample)`）且被 `example/CMakeLists.txt` `add_subdirectory` 纳入常规构建；`SARIBBON_DIR=${CMAKE_CURRENT_SOURCE_DIR}/../../src`，`SARIBBON_SIMPLE=SARibbon.h+SARibbon.cpp` 直接编进 target；`MainWindow.h:5 #include "SARibbon.h"`；01-S2 搬到 `examples/widgets/` 后深度变 `../../../src`（01-S8 声称已验证 StaticExample，深度是否已改需复核） | S2 重写：现状引用真实 CMake 变量 + 4 条操作（深度复核、SARibbonWidgets 双文件、`if(NOT EXISTS...) return()` 守卫代码块、MainWindow.h include 改名、CI standalone configure 命令），验证补"删文件后 WARNING 跳过" |

## 四、S3 Python 绑定

| # | 位置 | 问题类型 | 证据 | 处理 |
|---|------|---------|------|------|
| F13 | S3.2 ".sip 的 %Include/%Import 与头引用改为 `<SARibbonWidgets/...>`；优先改为指向安装后的 include，构建脚本先 install 再跑 sip" | 错误（与现状机制不符） | 14 个 .sip 的 `%TypeHeaderCode` 全部用**平面角括号 include**（`#include <SARibbonBar.h>` 等），由根 `pyproject.toml` `[tool.sip.bindings.saribbon] include-dirs=["src/SARibbonBar","src/SARibbonBar/colorWidgets"]` 解析；`%Include` 只引同目录 .sip；`tools/build_python_bindings.bat` **无任何 cmake/install 步骤**，直接 `sip-build.exe --build-dir build-python --qmake ...`（源码树构建），产物手工 copy 到 `site-packages\PyQtSARibbon\` | S3.2 重写为最小改动方案：include-dirs 改三目录、平面 include 保留、子目录限定形式列为可选并记 NOTES；删除"先 install"建议；S3 头部补三轨构建脚本真实流程 |
| F14 | S3.2（隐含假设：清单是新的） | 缺失（存量缺陷未识别） | `pyproject.toml`/`pyproject-pyqt6.toml`/`pyqt6/pyproject.toml` 的 `sources` 均缺 `SARibbonThemeManager.cpp`、`SARibbonThemePalette.cpp`、`SARibbonMdiControlsStyle.cpp`（`grep -c` 三文件=0；文件真实存在于 `src/SARibbonBar/`；`pyside6/CMakeLists.txt` 清单含它们=3 处命中）——绑定版本停 2.8.0，2.9.x 主题重构后未同步 | S3.2-2 新增"陈旧清单补齐"+ diff 对齐复核命令；进验收门与风险表 |
| F15 | S3.2 "pip install 后 `import SARibbonBar`（保持 Python 包名不变）" | 错误（冒烟名不对） | `sip/SARibbon.sip:1 %Module(name=PyQtSARibbon.saribbon...)`；bat 末行冒烟 `from PyQtSARibbon import saribbon`；`pyexamples/pyqt5/ribbon_demo.py:44` 同名 | 修正为 `from PyQtSARibbon import saribbon`；S3 头部给出三轨包名/模块名/冒烟命令总表（【内联-2】的"包名不变"具体化为该表） |
| F16 | S3.3 "PyQt6（pyqt6/sip/）：同 S3.2，配置文件 pyproject-pyqt6.toml" | 缺失（双轨并存未识别） | PyQt6 实际两套：轨 A=根 `pyproject-pyqt6.toml`+根 `sip/`（bat `--pyqt6` 临时替换根 pyproject 使用；模块名 `PyQtSARibbon`；`pyexamples/pyqt6/ribbon_demo.py:43` import 此名）；轨 B=`pyqt6/pyproject.toml`+`pyqt6/sip/`（publish CI `python -m build ... pyqt6/` 使用；模块名 `PyQt6SARibbon`，`pyqt6/sip/SARibbon.sip:1`）；`diff` 两 toml 仅名称/路径前缀/描述差异 | S3.3 拆为轨 A/轨 B 各自的操作与验证命令；"不合并双轨"记 NOTES |
| F17 | S3.4 "目标链接 `SARibbon::Widgets`" | 错误（与现机制冲突且不可自洽） | `pyside6/CMakeLists.txt` 是 standalone 工程：`SARIBBON_SOURCE_DIR=../src/SARibbonBar`（:102）自编 `saribbon_lib STATIC`（44 头/43 源），无 `find_package(SARibbon)`；wheel 经 scikit-build-core（`pyside6/pyproject.toml`）自包含构建，publish CI 直接 `python -m build --wheel pyside6/`——链接已安装库会破坏轮子自包含性 | S3.4 默认改为"维持自编译，迁移路径 + 并入 core 源清单 + shiboken -I 同步"；`SARibbon::Widgets` 链接列为备选并记 NOTES（R4 保守原则） |
| F18 | S3.4 "typesystem_saribbon.xml + glue 代码的 include 路径改三模块形态" | 存疑（大概率零改动） | `typesystem_saribbon.xml` inject-code 用平面 include（`#include "SARibbonGlobal.h"`），依赖 `-I` 解析；glue=`pyside6/saribbon_python_glue.h`（只 include Qt/shiboken 头） | S3.4 注明两者默认零改动，冲突时再改限定形式；实名核对无误（typesystem_saribbon.xml ✓） |
| F19 | S3.4 版本 | 缺失 | `pyside6/pyproject.toml version="2.8.0"` 与 `pyside6/CMakeLists.txt project(... VERSION 2.8.0)` 两处 | S3.4-4 明确两处都升 3.0.0 |
| F20 | S3.1 grep 清单 | 缺失（小） | `project.py` **存在**（根目录，旧式 sipbuild API：自定义 `saribbon_incdir/libdir/lib` 选项，无路径引用）；清单缺 `MANIFEST.in`、publish workflow、`docs/{zh,en}/python-guide`；命令本身可运行（实测 exit 0） | S3.1 命令扩列；补 project.py 说明与"双配置源优先级"待核实项（`sip-build --verbose` 验证） |
| F21 | S3.2 "builder-settings RESOURCES += ../../src/SARibbonBar/SARibbonResource.qrc" 路径基准 | 存疑 | qrc 实名 `src/SARibbonBar/SARibbonResource.qrc` ✓；但该相对路径以 sipbuild 生成工程目录为基准，`../../` 在根配置与 pyqt6/ 配置下基准不同（pyqt6/pyproject.toml 同样写 `../../src/...`） | S3.2 注明改后必须实测资源注册（冒烟时验证主题 QSS/图标可加载），标"待核实" |
| F22 | S3.5 pyexamples | 缺失（实名与轨差） | 实名：`pyexamples/{pyqt5,pyqt6,pyside6}/ribbon_demo.py` + `pyexamples/pyside6/test_binding.py`（现成 PySideSARibbon 冒烟脚本）；pyqt6 示例 import `PyQtSARibbon`（轨 A），与轨 B 轮子（`PyQt6SARibbon`）不匹配 | S3.5 补实名、test_binding.py、轨差注意事项 |
| F23 | S3（整体） | 缺失（文档同步无处安放） | `docs/{zh,en}/python-guide/` 各 4 篇（build-python-bindings.md:120-171 直接描述 bat 参数与 pyproject-pyqt6.toml 替换流程） | 新增 S3.6 文档同步子步 + 独立提交 |

## 五、S4 publish CI

| # | 位置 | 问题类型 | 证据 | 处理 |
|---|------|---------|------|------|
| F24 | S4 "恢复触发（tag `v3.0.*`）" | 错误（历史事实不符） | 该 workflow 自唯一提交 6f32beb 起触发器=`release: types:[published]`+`workflow_dispatch`，**从无 tag 触发**；`git tag -l` 全部为 `v2.x.y`（最新 v2.9.5）；计划 04 发布流程=tag+GitHub Release，release 事件天然覆盖 | S4 默认改"恢复 `release: published`"；tag 方案降级为需 NOTES 记录理由的备选；补 jobs 现状结构（3×9 矩阵 + publish OIDC/`environment: pypi`）与 dry-run job 具体规格（dispatch-only、win+ubuntu×3.12、三轨 build+install+import、artifact 留存、不上传） |

## 六、S5 CI 矩阵

| # | 位置 | 问题类型 | 证据 | 处理 |
|---|------|---------|------|------|
| F25 | S5-1 "静态：至少一个 SARIBBON_BUILD_STATIC_LIBS=ON 项"（原文未写明选项） + 现有 CI 的 `-DBUILD_SHARED_LIBS` | 错误（隐患）+缺失 | 库类型由 `SARIBBON_BUILD_STATIC_LIBS` 决定（`src/SARibbonBar/CMakeLists.txt:130-136`）；6 个 workflow 现传的 `-DBUILD_SHARED_LIBS=ON` 无任何消费点（`git grep BUILD_SHARED_LIBS` 仅 workflows 命中）——static 项若写 `BUILD_SHARED_LIBS=OFF` 得不到静态库 | S5 表格"静态"行明确选项名 + 证据行号；无效 shared 维度的清理记 NOTES |
| F26 | S5-2 "新增 amalgamation.yml：checkout → 跑 tools/Amalgamate.sh" | 缺失（平台与阻塞点） | Amalgamate.exe Windows-only（无源码在库）；Amalgamate.sh 末尾 `read -n 1`（F7）；StaticExample 可 standalone configure（F12） | S5-2 定 `runs-on: windows-latest`、注明依赖 S1-2 非交互改造、给出 standalone configure/build 命令 |
| F27 | S5-3 "vcpkg preset 验证…（frameless feature 联动）" | 缺失（preset 事实不全） | `CMakePresets.json` 实存 `vcpkg-msvc-x64-release` ✓（另有 debug、debug-static、release-static、**debug-frameless**）；version 6 → CMake≥3.25、依赖 `VCPKG_ROOT`；**release preset 不含 frameless**——联动逻辑（`VCPKG_MANIFEST_FEATURES` 含 frameless → 自动 `SARIBBON_USE_FRAMELESS_LIB=ON`）在根 CMakeLists:31-45，仅 debug-frameless preset 触发；`vcpkg.json` name=saribbonbar version=2.8.0，features=frameless/svg | S5-3 写明验证用 debug-frameless 或新增 release-frameless preset（推荐后者）+ 环境前提 |
| F28 | S5 矩阵目标表（"每个文件要改什么"粒度不足） | 缺失 | 实测 6 workflow 同构：`on:[pull_request,push]`、单 job、matrix `{os:[latest]（mac-qt5.15=[13]）, qt:[5.15.*|6.8.*], shared:[ON]}`、checkout 无 submodules、Qt 全走 `jurplel/install-qt-action@v4`（win-qt6.8 arch=win64_msvc2022_64；win-qt5.15 无 arch；mac 两个 host=mac arch=clang_64；linux 两个另装 apt 包，qt6.8 多 libxcb-cursor0）、configure 四选项统一、ctest offscreen、无 vcpkg | S5 头部补"现状"段（逐文件差异点）；表格补"落点与注意事项"列，标注哪些项 01-S10/02-S9 已做（复核）哪些 03 新增 |

## 七、S6 i18n 与安装

| # | 位置 | 问题类型 | 证据 | 处理 |
|---|------|---------|------|------|
| F29 | S6-1 盘点命令 `git grep -rn "lrelease\|\.ts" ... --include="*"` | 错误（命令不可运行） | 实测：`fatal: option '--include=*' must come before non-option arguments`（`--include` 是 GNU grep 选项，git grep 不支持该位置/用法） | S6 改为直接给出已核实的机制事实 + 正确命令 `git grep -n "lrelease\|add_translation\|create_translation" -- "*CMakeLists.txt" "*.cmake"` |
| F30 | S6-1 "qm 编译进构建（Qt5/Qt6 lrelease 命令差异处理，现有机制在哪就改哪）" | 缺失（机制已存在，无需"处理差异"） | `src/SARibbonBar/CMakeLists.txt:233-290`：LinguistTools QUIET 可选、`SARIBBON_UPDATE_TRANSLATIONS` 开关（ON=qt5/6_create_translation lupdate+lrelease；OFF=qt5/6_add_translation 仅 lrelease）、QM 挂 target_sources、`install → ${CMAKE_INSTALL_BINDIR}/translations COMPONENT translations`、POST_BUILD 复制 build/bin/translations | S6 头部补机制全文；S6-1 改为"迁移复核"（翻译块存活、TS_SOURCES 变量对齐、i18n 归属 widgets） |
| F31 | S6-1 "若 2.x 有运行时 qm 加载函数，路径逻辑不动" | 存疑→已核实（无此函数） | `git grep QTranslator src/SARibbonBar/` 零命中；`SARibbonResource.qrc` 不含 qm（仅 image/QSS/palette 32 项）；加载是宿主应用责任 | S6 写明"库无运行时加载，保持 install 目的地 bin/translations 不变"；顺带发现 `docs/{zh,en}/build-guide/i18n.md` 示例用 `:/i18n/` 资源路径与实际交付不符 → S6-4 修正 |
| F32 | S6-3 安装树复核清单（"SARibbonConfig.cmake 组件"） | 缺失（现状与目标态混淆） | 现状 config 模板实名 `src/SARibbonBar/SARibbonBarConfig.cmake.in` → `lib/cmake/SARibbonBar/`（namespace `SARibbonBar::`）；`SARibbonConfig.cmake`/`SARibbon::` 是 01-S5/S6 之后的目标态；`tools/test-find-package/` 现不存在（01-S11 创建）；另发现根 CMakeLists `SARIBBON_DOC_FILES`（:240）只 set 未 install（死变量，readme/LICENSE 现不分发） | S6 头部补现状 install 全清单（headers/share_amalgamate/config/qm）；S6-3 复核清单具体到 6 项；死变量记 NOTES |
| F33 | S6（tests 链接切换义务缺失） | 缺失 | 计划 01 S6-6 明文："tests 的 `target_link_libraries(... SARibbonBar)` 借此继续工作，**计划 03 改为 `SARibbon::Widgets`**"——03 原文无对应步骤 | 新增 S6-5（承接义务）：tests 内部链接切换 + 保留对外兼容别名 + 复核 grep；进验收门 |

## 八、S7 版本与 changelog

| # | 位置 | 问题类型 | 证据 | 处理 |
|---|------|---------|------|------|
| F34 | S7-1 "changlog.md 顶部新增草稿段" | 缺失（格式与位置未给） | 实名核实：根目录 `changlog.md`（拼写确实无 e）✓；格式 `## YYYY-MM-DD -> X.Y.Z` 新在上（首条 `## 2026-09-16 -> 2.9.5`） | S7-1 给出格式一致的草稿段代码块（`## 未发布 -> 3.0.0`） |
| F35 | S7（版本文件清单缺失） | 缺失 | 版本现值：根 `CMakeLists.txt` `SARIBBON_VERSION_{MAJOR,MINOR,PATCH}=2.9.5`（01-S4 升 3.0.0）；`SARibbonBarVersionInfo.h` 为 configure_file 生成物（根 :204-205）**且被跟踪**（改版本需重新 configure 刷新入库）；绑定/打包侧 6 处全停 **2.8.0**：pyproject.toml、pyproject-pyqt6.toml、pyqt6/pyproject.toml、pyside6/pyproject.toml、pyside6/CMakeLists.txt（project VERSION）、**vcpkg.json**（原文档漏） | S7 补"现状"与 8 处版本收口清单；验收门加"版本一致"项 |
| F36 | S7-2 "docs/en/（如有对应）" | 缺失（对应关系未核实） | `docs/zh/build-guide/` 与 `docs/en/build-guide/` 同名 5 篇均存在（build-3rdparty/build-SARibbon/build-instructions/common-build-errors/i18n）；根 `build.md` 存在 | S7-3 列明两语言文件实名清单；python-guide 归 S3.6 |

## 九、验收门与其他

| # | 位置 | 问题类型 | 证据 | 处理 |
|---|------|---------|------|------|
| F37 | 验收门 `git grep -rn "src/SARibbonBar" -- ':!plans' ':!docs' ':!changlog.md' ':!*.md'` | 存疑→已核实（可运行，有一处噪音） | 实测命令可运行（git grep 支持 -r 与 pathspec magic），基线命中 420 处；`tools/qrc_SARibbonResource_Datas.cpp` 的 rcc 生成注释嵌旧机器绝对路径 `C:/src/Qt/SARibbon/src/SARibbonBar/resource/...`，属不可消除噪音（除非重生成 rcc 产物） | 验收门命令追加 `':!tools/qrc_SARibbonResource*'` 并注明理由；出库后的 src/SARibbon*.cpp 不被 git grep 索引（untracked）天然不命中 |
| F38 | P1 链接锚点 `02-core-sinking.md#6-完成验收门v2-m1-交付判据全部满足` | 错误（锚点缺连字符） | 02 实际标题 `## 6. 完成验收门（= v2 M1 交付判据，全部满足）` → GitHub 锚点 `#6-完成验收门-v2-m1-交付判据全部满足` | 已修正链接 |
| F39 | 验收门"MANIFEST.in" | 已核实存在 | 根 `MANIFEST.in` 存在，内容：include project.py/pyproject×2 + recursive-include sip、pyqt6、pyside6、src/SARibbonBar | 验收门补 MANIFEST.in 项（指向 src/core+src/widgets、sdist 完整性） |
| F40 | §8 "已知偏差：见 NOTES.md" | 缺失 | 本轮评审新发现 5 项应记录偏差（release 触发史、BUILD_SHARED_LIBS 无实效、sip 清单陈旧、DOC_FILES 死变量、i18n.md 示例路径） | §8 列出待记录清单（执行时由 agent 按 R4 格式落 NOTES.md；评审不代写 NOTES） |

## 建议后续轮次关注

1. **跨文件锚点**：`04-qml-and-release.md` P1 链接本文件用锚点 `#6-完成验收门v2-m2-交付判据`，而本文件标题 `## 6. 完成验收门（= v2 M2 交付判据）` 的实际锚点为 `#6-完成验收门-v2-m2-交付判据`（缺连字符）——04 不在本轮 owns 范围，需 04 的评审轮修正（本轮刻意未改标题以最小化断链面）。
2. **计划 01 S8 的完整性**：01-S8 只说改 Amalgamate.sh 的 OPTS，但模板 4 文件里的硬编码路径（`../../src/SARibbonBar/...`）与 guard/宏也必须同步改，否则 01-S8 自身跑不通——建议 01 的评审轮核对；本轮已在 03-S1 现状节写明"01-S8 之后"的预期状态以兜底。
3. **sipbuild 双配置源**（project.py vs pyproject.toml 优先级）与 **builder-settings RESOURCES 相对路径基准**：两处"待核实"需在 S3 执行时以 `sip-build --verbose` 实测定案，第 2 轮可复查 NOTES 是否落账。
4. **qrc_*.cpp 预生成物的再生流程**：`tools/qrc_SARibbonResource_{Datas,version2,version3}.cpp` 为 rcc（Qt 5.14.2）产物且仓库无再生脚本/文档；若 3.0 期间资源文件变动，需要先把再生命令文档化（建议 04 或独立小任务）。
5. **amalgamation 产物的发布通道**：产物出库后，"单文件发行"实际交付方式（Release 附件？独立 artifact 包？）由计划 04 发布物料清单接管——04 评审时核对其是否真的接住了【内联-1】。
6. **N₀ 的取值口径**：README 快照说 tests 约 24 个源文件，v2 §7.3 说 25 个单测，实际 `tests/*.cpp` 25 个——02/03 的 P2 均以"N₀=02 录定值"为准，第 2 轮可核对 02 是否显式录定了 N₀。
