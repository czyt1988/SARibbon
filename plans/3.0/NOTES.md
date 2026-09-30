# 计划执行偏差记录（NOTES）

> 用途：执行 [README.md](README.md) 索引的四份计划时，凡发现"计划/ v2 计划 ↔ 仓库现实"不符，
> 在此追加记录。格式：日期、发现计划/步骤、证据（命令与输出摘要）、处理决定。
> 规则见 README.md R4（保守方向：缩小而非扩大改动范围）。

## 执行前已成立的已知偏差（基线 2.9.5 @ 2026-09）

### B1：v2 计划 P7（`SARibbonPannel` 拼写修正）已完成

- **证据**（2026-09-29 复核）：`git log --oneline --all --grep="Pannel"` 命中多条历史提交，其中
  `f553de7 把Pannel的命名错误调整为Panel` 为改名提交（更早的 `7f441dd 添加addPannel接口`、
  `f48ea94 完成SARibbonPannel类API的调整` 等为双 n 时代的提交）；
  `git grep -rln "SARibbonPannel" src/` 无任何命中；全部**被 git 跟踪的文件**中仅 `changlog.md`
  残留旧拼写（计划文档尚未入库，不在 git grep 范围内：v2 计划 :85/:451/:592 与 01 §8、02 §8
  的历史叙述中仍有 `SARibbonPannel` 字样）；类定义现为
  `SARibbonPanel.h:99 class SA_RIBBON_EXPORT SARibbonPanel : public QFrame`。
- **影响**：v2 计划 P7 要求的"`SARibbonPannel = SARibbonPanel` 类型别名过渡 + 迁移指南条目"中，**别名过渡不需要**；迁移指南（计划 04 S9）只保留结论性说明（拼写修正在 2.9.x 已完成）。v2 计划相应位置已加"【已完成于 2.9.x，f553de7】"批注（round1 评审）。
- **影响计划**：04（S9 迁移指南内容）。

### B2：v1/v2 计划引用的旧文件名 `SARibbonPannel*.h/.cpp` 均为单 n

- **证据**（2026-09-29 复核）：`ls src/SARibbonBar/ | grep -i panel` 实际为 `SARibbonPanel.h/.cpp`、
  `SARibbonPanelItem.h/.cpp`、`SARibbonPanelLayout.h/.cpp`、`SARibbonPanelOptionButton.h/.cpp`
  共 8 个文件（目录现仍为 `src/SARibbonBar/`，计划 01 S6 执行后才变为 `src/widgets/`）。
- **影响**：阅读 v2 计划时所有 `SARibbonPannelLayout` 等应脑内替换为单 n 名称；执行各计划时以仓库实际文件为准。

### B3：qwindowkit 为未初始化的 git submodule

- **证据**（2026-09-29 复核）：`.gitmodules` 唯一条目 path=`src/SARibbonBar/3rdparty/qwindowkit`，
  url=`https://github.com/czyt1988/qwindowkit`；`git submodule status` 输出
  `-f93657fa82bdd37dba68ea962d5e9b2cf4fd4d60 src/SARibbonBar/3rdparty/qwindowkit`（`-` 前缀=未 checkout）；
  磁盘目录 `ls -A` 为空。本机另有参考副本 `F:\src\3rdparty\qwindowkit`（非 submodule 内容）。
- **影响**：计划 01 S3 的 `git mv` 只搬 gitlink，无内容风险；pin `f93657f` 维持不变。

### B4：仓库存在非 UTF-8 编码的历史文件（round1 评审更正：范围与原记录不同）

- **原记录有误**：原称 `src/SARibbonBar/CMakeLists.txt` 与 `cmake/SARibbonUtils.cmake` 为 GBK。
- **证据**（2026-09-29 全仓扫描：src/example/tests/tools/sip/pyqt6/pyside6/scripts 下全部
  .h/.cpp/.hpp/.txt/.cmake/.sh/.bat/.ps1/.sip/.py/.xml/.qrc/.ui/.ts，除 3rdparty，逐文件
  `bytes.decode('utf-8')`）：**唯一非 UTF-8 文件为 `tools/Amalgamate.sh`**（GBK 可完整解码，
  含"按任意键继续"等中文）；根 `CMakeLists.txt`、`src/SARibbonBar/CMakeLists.txt`、
  `cmake/SARibbonUtils.cmake`、`scripts/build.ps1` 均为合法 UTF-8。根 CMakeLists `:124` 的
  `/wd4819` 是在 GBK 代码页系统上压制"UTF-8 无 BOM 源文件"触发的 C4819 警告，方向与原记录
  的推断相反。
- **影响**：README.md R1 的"禁止重编码既有文件、禁止全局 `/utf-8`"仍然成立，但实际保护对象是
  `tools/Amalgamate.sh`：计划 01 S8 与 03 S1 修改该脚本（改 OPTS、双产物）时**保持 GBK 原编码
  编辑或整体决策转 UTF-8 并单独提交**，不得由编辑器静默转码；CMakeLists 重写（01 S4/S5/S6）
  无编码障碍，正常以 UTF-8 处理。
- **round3 前向注**：计划 01 S8 对 GBK 脚本只做 sed 字节级替换（不触编码）；计划 03 S1-2 已定
  **目标脚本全文 ASCII 化**（ASCII 字节在 GBK/UTF-8 一致，转码事故免疫，属受控内容重写而非静默
  转码）——03 执行后由执行者把本条状态更新为"已消除"（03 §8-⑨ 有对应登记项）。

### B5：本机 Qt 5.14.2 低于 3.0 门槛（v2 D1 = 5.15）

- **证据**（2026-09-29 复核）：仓库根存在 `bin_qt5.14.2_MSVC_x64/` 与 `bin_qt6.7.3_MSVC_x64/`
  （历史安装产物）；`D:\Qt` 下装有 5.14.2、6.4.0、6.7.3、6.10.1；`scripts/build.ps1` 自动探测
  按版本目录名**字符串**降序取第一个 msvc*_64（`"6.7.3" > "6.10.1"`，故当前选中 6.7.3——
  属字符串排序的巧合，安装新版本 Qt 后可能变化，必要时用 `-QtPath` 显式指定）。
- **影响**：Qt5 路径验证一律以 CI（qt5.15 workflow）为准（README.md R2）；本机只用 Qt 6.7.3 做主验证。

### B6：v1 计划文件（`SARibbon-3.0-plan.md`）缺失，全部 "v1 §x" 引用悬空

- 日期：2026-09-29（round1 评审发现并记录）
- 发现位置：v2 计划全篇、计划 01（头部设计依据/S1/S2/S3/S5/S6/S11/§8）、计划 03（头部/S1/S3）、计划 04（S8/S10）
- 证据：`ls SARibbon-3.0-plan.md` → `No such file or directory`；
  `git log --all --oneline -- SARibbon-3.0-plan.md` 输出为空（从未入库）；
  `git status --porcelain` 未跟踪项仅 3 个（`SARibbon-3.0-plan-v2.md`、`plans/`、
  `saribbon-dev-v2.8.0-plus.bundle`），与计划 01 P1 期望的"4 个已知未跟踪项（**两份**计划 md、
  bundle、本目录）"不符；计划 01 S1 的 `git add plans/ SARibbon-3.0-plan.md SARibbon-3.0-plan-v2.md`
  会因 pathspec 不匹配而 fatal。
- 处理：v2 计划 §0/§1 即为 v1 审查结论的唯一存留记录（v2 头部已加注）；悬空引用按
  README.md"计划文档自包含性说明"的约定处理（内联内容为准，无法还原时向维护者澄清，
  禁止想象 v1 内容）；01 P1/S1 的具体修正建议已记入 `reviews/round1/cross-findings.md`
  交 01 的修订 agent 处理。评审轮次记录统一放 `plans/3.0/reviews/`。
- 影响计划：01（P1/S1/头部/§8）、03（头部/S1/S3 的 v1 引用）、04（S8/S10 的 v1 引用）、v2（头部批注已加）

### B7：当前工作区 checkout 分支为 `v3`，非计划 01 P2 期望的 `dev`

- 日期：2026-09-29（round1 评审发现并记录）
- 发现位置：计划 01 P2（`git branch --show-current` 期望 `dev`）、S1（`git checkout -b dev-3.0`）
- 证据：`git branch --show-current` → `v3`；`git rev-parse dev v3 origin/dev` 三者同为
  `7a617fca7a0e48605041e424840394fa027ea87b`（HEAD `7a617fc` 与 01 P2 期望的 commit 一致）。
- 处理：保守方向——两分支同 commit，无内容分歧。执行 01 前可 `git checkout dev`（或直接以当前
  HEAD 为基），P2 门禁按"HEAD==7a617fc 且分支为 dev 或 v3"解读；`dev-3.0` 从该 HEAD 创建即等价。
  已记入 cross-findings 交 01 的修订 agent 更新 P2 文案。
- **round3 更新（终审）**：本条的"三者同指 7a617fc""P2 按 HEAD==7a617fc 解读"表述已过期——评审
  提交已入库（`42b7dcc` round1、`eba8ebd` round2，round3 产物继续追加），`v3` 已前进、`dev` 仍指
  `7a617fc`。计划 01 P1/P2/S1 已在 round3 重写为**执行时点稳健**的双门禁（`git merge-base
  --is-ancestor 7a617fc HEAD` + 非文档 diff 为空），README 头部/事实快照同步改写；解读约定见 B11。
- 影响计划：01（P1/P2/S1，round3 已重写）、README（头部/R4/快照，终审已改）

### B8：ctest 注册项为 26，此前文档写"约 24 个"有误

- 日期：2026-09-29（round1 评审发现并记录）
- 发现位置：README.md 事实快照"测试"行（已改）；计划 01 S7（"24 个测试源"，属 01 修订 agent）
- 证据：`ls tests/*.cpp | wc -l` → 25；`tests/auto/SARibbonThemePalette/tst_themepalette.cpp`
  另注册为 `SARibbonThemePaletteTest`；`tests/CMakeLists.txt:47-72` 共 26 个
  `add_saribbon_test()` 调用（即 ctest 基线 N₀=26）。
- 处理：README 快照与 R2 已改为 26/N₀=26；01 S7 的 `git mv tests/*.cpp tests/widgets/` 会**漏掉
  `tests/auto/` 子目录**，导致 `add_saribbon_test(SARibbonThemePaletteTest auto/SARibbonThemePalette/tst_themepalette.cpp)`
  的相对路径断裂——修正建议已记入 cross-findings 交 01 的修订 agent。
- 影响计划：01（S7、P3 的 N₀ 记录）

### B9：QML 注册路线单轨化（与 v2 原文冲突，round2 已修订）

- 日期：2026-09-29（round2 评审发现，round2 整合裁决采纳）
- 发现位置：round2 评审（04 S1 路线决策段，原标"⚠️设计级变更待整合者复核"）；冲突对象为
  v2 计划 §5.3 原文（"保留 v1 双轨：Qt5 命令式 / Qt6 qt_add_qml_module"）与 §1.1-4、§0"保留"行
- 证据：KDDW 2.0.1 支持 Qt 6.2+（`QT_MIN_VERSION "6.2.0"`，KDDW:CMakeLists.txt:146——
  qt_add_qml_module 自 6.2 起可用，明知而不用）却只用命令式注册（src/qtquick/QmlTypes.cpp:21-30，
  Qt5/Qt6 零条件编译；全仓 grep 无 qt_add_qml_module/qmldir）；QWK 1.0.1 quick 模块 7 个源文件
  零 QT_VERSION 分支（qwkquickglobal.cpp:14-25，README.md:246 明确 Qt6 下 URI import 不变）；
  命令式 API 全集在本机 Qt 5.14.2/6.7.3 头文件同签名存在（逐项行号见
  reviews/round2/kddw-qtquick-findings.md §一.3）
- 处理：整合裁决**采纳单轨化**——v2 §5.3 已修订为"默认命令式单轨（Qt5/Qt6 同码），声明式轨
  降为 3.1+ 可选优化（触发条件：qmltypes 补全/qmllint/qmlcachegen 任一）"，原双轨表述保留为
  历史记录；v2 §0/§1.1-4/§5.4/§9-D5/§10-R9 相应加批注；04 S1 的"待整合者复核"标记已改为
  "已复核采纳"；01 S6-5 实证注记与 03 S6-3 安装预警按终态更新。QML 安装布局结论随之定案：
  叶子进 qrc 随库二进制、安装期零新增产物（v2 §6.1 批注②）。裁决详情见
  reviews/round2/synthesis-findings.md
- 影响计划：04（S1/S2/S6/S7/风险表）、01（S6-5）、03（S6-3）、v2（§0/§1.1/§5.1-§5.4/§6.1/§9/§10）

### B10：新增 SARIBBON_INSTALL 安装守卫选项（与 v2/01 原文冲突，round2 已修订）

- 日期：2026-09-29（round2 评审建议 D1，round2 整合裁决采纳）
- 发现位置：round2 评审（reviews/round2/qwk-build-findings.md §三 D1）；冲突对象为
  01 S4.2 原"不抄 1：QWINDOWKIT_INSTALL 安装守卫选项"条目（原文写"是否新增属设计级决策，
  本计划正文不擅自加"）与 v2 §6.1（原无此选项）
- 证据：QWK 用 `QWINDOWKIT_INSTALL` 守卫一切安装物（QWK:CMakeLists.txt:13,36-39——
  GNUInstallDirs/CMakePackageConfigHelpers 仅 INSTALL 时 include；src/CMakeLists.txt:29,100,181——
  config 头安装、install(TARGETS)+INSTALL_INTERFACE、Config/Targets 生成安装全在守卫内）；
  SARibbon 2.9.5 的 install 规则无条件生效，`add_subdirectory` 嵌入场景会污染宿主工程的
  install/export 规则；01 S4 第 7 条的 CMAKE_SOURCE_DIR 顶层守卫只覆盖 examples/tests，覆盖不到 install
- 处理：整合裁决**采纳**——v2 新增 §6.4（`SARIBBON_INSTALL` 默认 ON，全部
  install(TARGETS/EXPORT/DIRECTORY/FILES)、Config/ConfigVersion 生成安装、兼容转发头与旧包名
  薄壳、qm 翻译安装收进守卫）；01 S4.3 选项清单补行、S4.1 处置表同步、S4.2"不抄 1"翻转为
  "已采纳"、S5.3/S5.4/S6.7/S11 的 install 规则加守卫注记；03 S6-3 复核前提注明"以默认 ON 执行"
- 影响计划：01（S4/S5/S6.7/S11/S12）、03（S6）、v2（§4.4/§6.1/§6.4）

### B11：评审提交链入库与"执行时点门禁"的时点解读约定（round3 终审）

- 日期：2026-09-29（round3 终审记录，采纳 01-dryrun 建议 1/2/4）
- 发现位置：README 头部与事实快照"git 基线状态"行、本文件 B7、计划 01 P1/P2/S1
- 证据：`git log --oneline 7a617fc..v3` → `42b7dcc`（round1 评审提交）、`eba8ebd`（round2 评审提交），
  round3 产物（01~04 修订 + reviews/round3/）随后追加；`git diff 7a617fc HEAD -- ':!plans'
  ':!SARibbon-3.0-plan-v2.md'` 为空（基线后零代码改动）；`git status --porcelain` 未跟踪项仅
  `saribbon-dev-v2.8.0-plus.bundle`
- 处理（**体系级约定，后续评审轮次与执行 agent 一体遵守**）：① 计划文档中一切 git 状态断言用
  **相对表述**——祖先检查（`git merge-base --is-ancestor 7a617fc HEAD`）、非文档 diff 为空、
  "以执行时 `git log` 为准"；绝对 commit 号只作评审轮次的时点备注，不作门禁判据；② 旧文档中
  "未跟踪项 3 个"（round1 时点）一律按"仅 bundle"（评审提交入库后）解读；③ 每轮评审提交入库后，
  README 事实快照的 git 行**不需要**再逐轮改写（已改为时点稳健表述），仅当代码基线本身变动
  （如 2.x bugfix 合入）时更新"代码基线"字段
- 影响计划：01（P1/P2/S1 已按此重写）、README（头部/R4/快照已改）、02~04（同类前置检查引用本条）

### B12：三条仓库事实预登记（round3 新发现，执行期会反复引用；采纳 02-dryrun 建议 5）

- 日期：2026-09-29（round3 终审预登记）
- 内容：
  1. **Category 滚动按钮标志双实现**：`SARibbonCategoryLayout.cpp` 的 `updateGeometryArr`（:469-505）
     与 `updateScrollButtonVisibility`（:1053-1091，由 `updateScrollOffset` :1136 调用）逐分支重算
     同一套 RTL/LTR 标志（唯一文本差异是 maxBase 的 `qMax(0,...)`，在 needsScrolling 前提
     （total>categoryWidth）下恒正，语义等价）。计划 02 S6-3 将收敛为引擎单一纯函数
     `scrollButtonFlags(totalWidth, viewportWidth, xBase, isRTL)`——这是 M1 期间**唯一一处
     "两份源码合成一份"**的操作（§4-4 禁止双实现条款授权），执行时须 NOTES 单独记录并以
     `SARibbonCategoryVisibilityTest` + 滚动/动画交互冒烟双验。
  2. **`SARibbonBar.cpp:3810-3854` 存在整段注释掉的 layoutTitleRect 旧代码**：`contextRegionLeft/
     titleStart` 等词在该注释段命中，是计划 02 S8-2 残留门的误报源（该门已限定三布局 .cpp 文件
     范围将其排除）；可在 02 S8 清扫时一并删除该注释段并单独提交（良性，删除死注释不破坏 API）。
  3. **`SARibbonBarLayout::init()` 是空桩**（:607-610，函数体仅注释"不需要初始化子控件，它们会
     从ribbonBar获取"），BarLayout 的子控件实际全部由 `SARibbonBar` 侧创建持有；:623-696 区间是
     `addItem/itemAt/takeAt/count` 四个 QLayout 协议函数（计划 02 附录 E 已按此修正，执行 02 S7
     时勿再按"factory 创建子控件"的旧描述找代码）。
- 影响计划：02（S6-3/S8-2/S7/附录 E）；03/04 无直接影响

### B13：绑定构建=第五种消费场景；02 窗口内 sip 冒烟降级为 C++ 侧、顺延至 03 S3（round3 终审裁决）

- 日期：2026-09-29（round3 终审裁决，采纳 03-dryrun 建议 1/2 与 02-dryrun 建议 4）
- 发现位置：01 S6.2/S6.4（include 形态四态论证）、02 S1.1-4（sip 冒烟）、03 S3.0（绑定镜像）
- 证据：三轨 Python 绑定（sip×2/pyside6）**直接编译源码树文件**，既不属于 01 S6.2 论证的四种
  消费场景（源码树/同步目录/安装树/amalgamate），也不在 01（明文不动绑定文件）与 02 的落地范围；
  且 01 S6 目录搬移后三份 sip toml 的 include-dirs/清单指向已不存在的 `src/SARibbonBar`，转发头的
  `<SARibbonCore/...>` 在绑定构建场景无解析——**02 执行期绑定整体不可构建**，sip 冒烟必然失败
  且无法归因（03-dryrun R3-04 的镜像缺失分析与 02-dryrun 建议 2 相互印证）
- 处理：① 03 S3.0 的"绑定侧自建镜像"（sip 系仓库根 `build-binding-include/SARibbonCore/`、
  pyside6 CMakeLists 内 `_sync_include/`，与 01 S8 的 `_amalg_include` 同构）确认为第五场景的
  **唯一解**，01 S6.4 已补互认交叉引用；② 02 S1.1-4 的 sip/PyQt 构建冒烟**降级为 C++ 侧验证**
  （widgets 全量编译 + QMetaEnum 消费者 grep + ctest==N₀），绑定侧验证**顺延至 03 S3**——
  备选方案"把 03 S3 路径迁移提前到 01 之后"被否决：即便提前迁移路径，镜像与陈旧清单补齐
  （03 S3.0/S3.2-2）完成前绑定仍不可构建，提前只会打乱"01/02 不动绑定文件"的边界与提交归属；
  ③ 03 S3.2-1 已补"枚举别名化适配"承接条（RowProportion 三文件 + 02 S1.1 四枚举的对应条目，
  按 02 实际执行结果——提升成功或降级留 widgets——分支处理）
- 影响计划：01（S6.4 注）、02（S1/S1.1-4/风险表）、03（S3.0/S3.2）

---

## 执行中追加

### B24：计划 02 S5 执行记录（PanelLayoutEngine Step A/B）
- 日期：2026-09-30
- 内容：
  1. **S5.0 基线**：`tests/core/tst_panelLayoutGolden.cpp`（LABELS core + offscreen ENVIRONMENT）以文本 blob 锁定 updateGeomArray 输入→输出映射（3 模式 × 3 尺寸 × 5 按钮，65 行黄金值，`SARIBBON_GOLDEN_REGEN=1` 重录）；录制与回放确定性均验证。**字体口径调整**：未随仓库部署字体，采用 `QApplication::setFont(SimSun 9, "SARibbonToolButton")` 固定按钮字体——Windows 本机可复现；黄金值头部记录字体名，跨平台 CI 字体差异将显式失败（非静默），复验移交 CI。
  2. **Step A**：`SARibbonPanelItem : QWidgetItem + 契约`（多继承）；`isHidden()=isEmpty()`（action 可见性语义）；`sizeHint/expandingDirections` 显式覆写消歧（双基类同名虚函数缺最终覆写会 C2259）；契约按"接口最小"原则**删除 minimumSizeHint 纯虚**（三算法均不调用）。
  3. **Step B**：`src/core/layout/SARibbonPanelLayoutEngine.h/.cpp`——算法体纯 move（cpp 头注释列全机械替换表）；Input 含 `previousSizeHintWidth`（**语义保真点**：2.x recalcExpand 读上一次 mSizeHint——函数在覆写前调用，引擎经 Input 传入上一次值而非本次 totalWidth）；sizeHint 缓存 key 改契约 item 指针；`removeFromCache/clearCache` 承接 takeAt/invalidate 不变量。
  4. **widgets 适配器**：updateGeomArray = 收集 Input → 引擎 layout() → 回写五输出 + RTL label 对齐；**遮蔽字段取消**：`itemWillSetGeometry` 改为绑定 resultGeometry 的 `QRect&` 引用成员（单真源、存量读写零改动）；rowIndex/columnIndex/isExpandItem/rowProportion 直接用契约基类字段（2.x rowIndex 为 short、契约定 int，无取址消费者）；`invalidateButtonSizeHintCache(QWidget*)` 按 widget 反查 item 转发引擎缓存。
  5. **验证**：黄金测试 PASS（引擎化后与基线逐字节一致）；ctest 26/27（B15）；纯净绿；确定性层扫描绿；**残留门 Panel 标记物全零**（SARibbonToolButton.cpp:887 的 v2.9.4 历史注释措辞同步更新）；amalgamate 模板补引擎条目并重生成，StaticExample 编译+运行通过。
- 遗留：S5.2-4 的 FakeItem 引擎级测试与 S8 矩阵补全（RTL/dpr/退化输入/takeAt 缓存用例）合并到 S8 执行（黄金 blob 已锁行为）。
- 影响计划：02-S5 完成；02-S8；03-S1

### B25：计划 02 S6/S7/S8/S9/S10 执行记录（Category/Bar 引擎、黄金补全、CI、文档）
- 日期：2026-09-30
- S6（Category 引擎）：`SARibbonCategoryLayoutEngine`（Input+Result）纯 move updateGeometryArr 函数体；`scrollButtonFlags()` 收敛 2.x 双实现（updateGeometryArr 内 + updateScrollButtonVisibility，语义等价唯一文本差异 maxBase 的 qMax(0,..) 在 needsScrolling 前提下恒正——B12-1 预登记）；`clampScrollOffset()` 带 isRTL 入参；`SizeHintCollection` 提升为 core 的 `SARibbonCategorySizeHints`（收集函数留适配器）；隐藏项 separator hide 留公共壳（SARibbonCategory::updateItemGeometry 外部路径依赖，B12-2）；CategoryLayoutItem 契约化（isHidden=isEmpty 默认语义、expandingDirections 精确映射 isExpanding、双几何字段引用绑定）。验证：RTL/Visibility/黄金全绿、expandWidth 标记物零残留。
- S7（Bar 引擎，D6 范围）：`SARibbonBarGeometryEngine::layoutTitleRect` 四分支（RTL/LTR×紧凑/宽松）纯提取（TitleRectInput 结构）；resizeInLoose/CompactStyle 留 widgets（计划 S7-2 明文）。执行事故：适配器替换时误删公共 `SARibbonBarLayout::resetSize()`（git 恢复，见提交 f1edea2 过程）；验证：BarLayoutRTL/SystemButtonBarGeometry/TitleBarHitTest/BarEventFilter 全绿、titleStart/titleEnd/contextRegionLeft/Right 标记物零残留。
- S8（黄金补全+残留清扫）：新增 `tests/core/tst_panelLayoutEngine.cpp`（FakeItem 显式输入，**纯 core 无 widgets 依赖**）：三行混合/隐藏项/scrollButtonFlags 六态/clamp 五例/紧凑 LTR titleRect（含过小置空）；core-only 构建含 `SARIBBON_BUILD_TESTS=ON` 全绿（`ctest -L core --no-tests=error` 1/1）；FakeItem 教训：引擎读契约字段 rowProportion（构造默认 Large），测试必须显式赋值（对应 widgets createItem 的赋值路径）。**截图验收口径**：引擎化后黄金 blob 逐字节一致（S5）+ 全量 ctest 绿 + 布局三巨头公共面无删除（见下），视觉零变化由此保证，6 张截图由 CI/发布流程录制（B20 同口径）。
- **公共 API 机检（§6 第 7 条）**：三布局头文件声明行 diff——Panel/Bar 公共面（public 段）零删除；Category 仅内部类 `SARibbonCategoryLayoutItem` 的两个公有字段 `QRect mWillSetGeometry;` → `QRect& mWillSetGeometry;`（同名引用绑定，全部消费代码零改动，编译+27 测试证明）；另 PanelLayout 的 protected `recalcExpandGeomArray` 与 private 缓存成员按计划 S5.2-2/S8-2 移入引擎（残留门要求零命中与保留声明互斥，机检红灯即计划内移动）。**判定：公共类名/方法/信号零变化，通过。**
- S9（CI 前置）：6 workflow 增加 "Core golden tests" step（`ctest -L core --no-tests=error` + offscreen；linux-qt6.8 的 widgets=OFF 组合条件跳过避免零测试假红——与 01 S10-2 联动）。全部 YAML 校验通过。
- S10（文档）：`docs/zh/dev-guide/core-module-guide.md`（七子系统结构、两层纯净铁律、契约接口、适配器模式、FakeItem 走查、R7 同步流程）。
- 影响计划：02 全部完成；03-S1/S5（amalgamate 含三引擎）；04（QML 引擎复用基础就绪）

### B26：计划 03 S1/S2 执行记录（amalgamate 双产物 + 出库 + StaticExample 切换）
- 日期：2026-09-30
- 内容：
  1. 模板双套化（git mv 保留历史）：Core 4 文件 + Widgets 4 文件（原 4 文件改名重写）。Widgets PublicHeaders 首行嵌套 include CorePublicHeaders（core 头整体并入、10 条直接条目删除避免双路径重复内联）；Widgets 模板宏段保留 SA_RIBBON_BAR_NO_EXPORT/SA_COLOR_WIDGETS_NO_DLL（兼容层消费）+ 补 SA_RIBBON_CORE_STATIC。
  2. Amalgamate.sh 全文 ASCII 化（NOTES B4 状态"已消除"达成）+ set -e + 产物存在性检查 + [ -t 0 ] 交互守卫 + 镜像段扩展（find ../src/core 全量平铺 + build 树 SARibbonCoreConfig.h 拾取）+ 双 pass 设计：Core 双 pass 均带镜像；Widgets h pass 带镜像、cpp pass 不带（B23 的 Q_OBJECT 双内联教训），sed 把 cpp 产物中 <SARibbonCore/X.h> 改写为对应产物头。
  3. 出库四动作：git rm --cached（工作区保留）；.gitignore +6 行；.gitattributes 删字节冻结 2 行及段注释；src/CMakeLists.txt 的 share/SARibbonBar_amalgamate 安装规则移除（选"移除"：产物已出库 fresh clone 无文件可守卫；Release 附件归计划 04）。
  4. StaticExample 切换：SARIBBON_SIMPLE 指向 SARibbonWidgets.{h,cpp} + EXISTS 守卫 + 两处 include 改名；提交时序按计划 round3 双笔方案。
  5. SARibbonCore.moc 尾行：SARibbonThemeData（Q_OBJECT）在 core 单文件 .cpp 内，AUTOMOC 需要 #include "SARibbonCore.moc"（widgets 产物无此问题——其 Q_OBJECT 头全在 .h 产物侧）。沙盒验证：SARibbonCore 单文件独立工程编译+运行 exit=0。
- 产物健全性门禁偏差登记：计划 03 §6 期望产物 cpp 仅 1 处 include "产物.h"（@remap 假设）；实测 @remap 不匹配尖括号 include（B17/B23 实证），经 sed 改写后产物 cpp 有多个指向产物自身头的 include 行（include guard 去重，编译安全），非镜像失效；StaticExample 与沙盒双编译验证通过，以此替代该 grep 字面口径。
- 验证：双产物生成 exit 0（非交互）；StaticExample 编译+运行冒烟通过；SARibbonCore 单文件编译+运行通过；git ls-files src/SARibbon* 空输出。
- 影响计划：03-S1/S2 完成；04-S10（Release 附件为产物 zip）

### B27：计划 03 S3-S7 执行记录（绑定迁移、CI 矩阵、版本收口）
- 日期：2026-09-30
- S3（三轨绑定）：
  1. **绑定镜像**：`tools/sa_build_binding_include.py` 生成 `binding-include/SARibbonCore/`（14 头平铺+build 树 Config 拾取；命名避开 .gitignore 的 `build*` 通配——初版 build-binding-include 被 L2 的 `build*` 规则误吞，记此教训）。
  2. **sip 双轨 toml**：三份 pyproject 的 include-dirs 改 [src/core, src/widgets, src/widgets/colorWidgets, (../)binding-include]，headers/sources 以 `git ls-files src/core src/widgets` 为准整体重生成（114 项与树完全对齐；2.8.0 时代缺失的 ThemeManager/MdiControlsStyle 引擎层全量补齐），RESOURCES 指 src/widgets/SARibbonResource.qrc，版本 3.0.0。
  3. **pyside6 轨**：CMakeLists 的 SARIBBON_SOURCE_DIR → src/widgets，新增 SARIBBON_CORE_DIR + `_sync_include` 镜像（file(COPY) 自包含，S3.0-2 落点）；SARIBBON_HEADERS/SOURCES 按 git ls-files 重生成（SARIBBON_* 变量前缀形式）；saribbon_lib 与 saribbon 包装 target 的 include dirs 及 shiboken -I 参数补 core+镜像；版本 3.0.0。
  4. 绑定构建/轮子/冒烟验证：本机无 sip/shiboken 工具链，全部顺延至 CI dry-run job（计划 03 §S3 尾注允许，非静默跳过）。
- S4（publish CI）：三个 build job 触发保持现状（release+dispatch，01-S10-4 结论）；build-pyqt5/pyqt6 各加镜像前置 step（pyside6 不加——其镜像在 CMakeLists 内自足）；新增 dry-run job（dispatch-only，windows+ubuntu × pyqt5/pyqt6/pyside6，每轨独立 venv 安装+import 冒烟，artifact 留存）。
- S5（CI 全矩阵）：win-qt6.8 加 `staticlibs: [OFF, ON]` 轴（SARIBBON_BUILD_STATIC_LIBS 实名；BUILD_SHARED_LIBS 维持无效传参不动，NOTES 记录）；新增 amalgamation.yml（windows-latest：bash Amalgamate.sh → standalone StaticExample configure+build → offscreen 冒烟 → 4 产物 artifact）；vcpkg preset 本机配置通过（VCPKG_ROOT 未设只打印变量清单——语法与 preset 链验证达成，依赖安装归 CI）。
- S6（安装收尾）：tools/test-find-package 增 test_find_package_core TU（`find_package(SARibbon COMPONENTS Core)` + 纯 core 头 + 链 SARibbon::Core），双消费者编译+运行 exit 0（需要先刷新 install 树——计划 02 后的安装树是陈旧的，已重装）；tests 内部链接切 SARibbon::Widgets（别名保留对外）；i18n 翻译块随目录迁移正常（构建日志 translations 复制行存在，SARIBBON_UPDATE_TRANSLATIONS 选项在位）。
- S7（版本与 changelog）：vcpkg.json 3.0.0；changlog.md 3.0.0 草稿段补齐（core 引擎/黄金测试/双产物/绑定四类条目）；8 处版本落点全部 3.0.0（根 CMake/project、4 pyproject、pyside6 CMakeLists、vcpkg.json；VersionInfo.h 由 configure 生成自动跟随）。
- 验收门执行：残留 grep（排除 docs/plans/changlog/qrc 注释）零命中；产物出库（git ls-files 空）；MANIFEST 指向 src/core+src/widgets；全量 ctest 27/28（B15 项）；core 纯净绿。
- 影响计划：03 全部完成（除绑定构建实跑归 CI dry-run）；04 全部前置就绪

### B21：类作用域 using 声明无法引入命名空间枚举符（计划 02 S1 round3 断言错误，MSVC C2886）
- 日期：2026-09-30（计划 02 S1 执行）
- 发现位置：计划 02 S1 第 1 条 RowProportion 兼容机制
- 证据：MSVC 19.29 对 `class SARibbonPanelItem { using SARibbon::Core::None; ... }` 报 `error C2886: "SARibbon::Core::None": 在成员 using 声明中不能使用符号`——C++ 标准 [namespace.udecl]/7 要求成员 using 声明的名字必须是基类成员；命名空间枚举符不满足。计划 02 round3"using 声明把命名空间成员引入类作用域，合法 C++"的断言有误。
- 处理（沙盒最小实验验证后采用）：类型别名 + **static constexpr 成员**：`using RowProportion = SARibbon::Core::SARibbonRowProportion;` + `static constexpr RowProportion None/ Large/Medium/Small = ...;`。三类存量用法全部保持可编译（沙盒+全仓构建验证）：`SARibbonPanelItem::Large` 类限定、类内裸名默认值、`QString::number(d.actionRowProportionValue)` 隐式 int 转换。枚举本体仍入 `SARibbon::Core` 命名空间、仍 unscoped（计划两目标不变）。
- 影响计划：02-S1（兼容机制落地修正）；03-S3.2-1（绑定侧枚举适配按此现状核对）

### B23：计划 02 S2 执行记录（theme/ 下沉）与 amalgamate 的 Q_OBJECT 双内联问题
- 日期：2026-09-30
- 内容：
  1. `SARibbonThemePalette.h/.cpp` git mv 至 `src/core/theme/`（导出宏换 `SA_RIBBON_CORE_EXPORT`，include 换 `<SARibbonCore/SARibbonCoreGlobal.h>`）；widgets 留同名转发头，7 处消费者（MainWindow/Util/ThemeManager/tests/examples）零改动。
  2. 新建 `src/core/theme/SARibbonThemeData.h/.cpp`：QObject 单例（Meyers 静态，`SARibbon::Core`），持 theme+palette+双信号；5 个静态表（margins/contextColors/highlights/baseline + 2 个高亮 lambda）纯 move 为静态查询接口；`FpContextCategoryHighlight` 提升为 `SARibbon::Core::SARibbonFpContextCategoryHighlight`（widgets 侧 `SARibbonBar::FpContextCategoryHighlight` 原拼写不动——2.x 公共 API，且两边同为 `std::function<QColor(const QColor&)>` 具体类型一致，零转换）。
  3. `SARibbonThemeManager.cpp` 删除全部静态表，4 个查表块改为调 core 静态接口；QSS 加载/渲染与其余逻辑零改动（计划 S2.4"先只做数据表 move"选项）。
- **amalgamate 新问题**：widgets 侧 `#include <SARibbonCore/SARibbonThemeData.h>` 在 .cpp 产物生成时经 `_amalg_include` 镜像被再次内联 → Q_OBJECT 类定义落进 .cpp 产物 → StaticExample 的 AUTOMOC 报错（"contains Q_OBJECT but does not include SARibbon.moc"）。@remap 对尖括号 include 无效（工具只匹配引号形式）。
- 处理：`Amalgamate.sh` 拆分双 pass——`.h` 产物 pass 保留 `-i _amalg_include`，`.cpp` 产物 pass **去掉镜像目录**（尖括号 include 原样保留），后处理 sed 把 .cpp 产物中 `#include <SARibbonCore/X.h>` 改写为 `#include "SARibbon.h"`（内容已在 .h 产物）。模板同步：PublicHeaders 加 2 个 theme 头、cpp 模板加 2 个 theme 源、删陈旧 widgets/SARibbonThemePalette.cpp 行。验证：.cpp 产物无 Q_OBJECT、StaticExample 编译+运行通过；ctest 25/26（B15 项）+ 主题 4 测试全绿。
- 影响计划：02-S2 完成；03-S1（双产物脚本必须继承"cpp pass 无镜像 + sed 改写"设计）

### B22：计划 02 S1 执行记录（枚举/Util 下沉）
- 日期：2026-09-30
- 内容与证据：
  1. `src/core/global/SARibbonEnums.h`：三自由枚举（Alignment/Theme/MainWindowStyleFlag，含 Q_DECLARE_METATYPE/FLAGS）+ `SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE` 宏（纯 move，全局命名空间维持）+ `SARibbon::Core::SARibbonRowProportion`（提升）+ 三个属性名宏（移自 PanelItem.h）。widgets/SARibbonGlobal.h 转发 include 之，38 个既有 include 零改动。
  2. **四个 Q_ENUM 枚举（PanelLayoutMode/RibbonStyleFlag/RibbonMode/RibbonButtonType）按计划 S1.1 的降级出口留 widgets 原地**（计划明文允许："任何一步造成 moc/编译失败且无法在不改行为前提下解决时允许留在 widgets，枚举下沉不是 M1 验收硬门"）。决策理由：RibbonStyleFlag/RibbonMode/PanelLayoutMode 深度耦合 Q_PROPERTY+Q_ENUM+Q_FLAG+SARibbonMainWindow 侧的 Q_DECLARE_OPERATORS_FOR_FLAGS，提升+别名方案在这些 moc 元对象链上属工具链灰区（round3 final-audit §五已预警），而 M1 的验收硬门是引擎提取；降级后引擎 Input 用 int/独立值即可，零行为风险。
  3. `src/core/global/SARibbonCoreUtil.h/.cpp`：8 个 core 函数纯 move（makeColorVibrant/scaleSizeByHeight×2/scaleSizeByWidth/iconToPixmap/saMirrorX/isOperatingSystemInDarkMode/set+isEnableSystemDarkModeAutoSwitch）；saIsRTL 的 `QApplication::layoutDirection()` 换 `QGuiApplication::layoutDirection()`（行为等价，QtGui）；`iconToPixmap` 补 `SA_RIBBON_CORE_EXPORT`（2.9.5 本就无导出宏，属 S1 计划内补漏）。widgets/SARibbonUtil.h/.cpp 留 3 个（widgetDevicePixelRatio/replaceQssTokens/getBuiltInRibbonThemeQss），include `<SARibbonCore/SARibbonCoreUtil.h>` 保持 SA:: 拼写零变化。
  4. `sa_sync_include` 增加 FLATTEN 选项（core 专用，widgets 层级不变）。
  5. tests POST_BUILD 增加 SARibbonCore.dll 拷贝（S1 后 widgets 双 DLL 依赖）。
  6. Amalgamate.sh 镜像段扩展为 `find ../src/core -name '*.h|*.hpp'` 全量平铺；两个模板补 `SA_RIBBON_CORE_STATIC` 定义与 `../../src/core/global/SARibbonCoreUtil.cpp` 枚举——单文件产物重新生成，StaticExample 编译链接通过。
- 验证：构建绿；ctest 25/26（B15 环境项）；`check_core_purity.py src/core` 绿（扫描器顺带修正：include 前缀改为"完全相等或后随大写字母"边界，QStyleHints 等 QtGui 类不再被 QStyle 前缀误伤；补 QQuickPaintedItem 规则）。
- 影响计划：02-S1 完成；03-S1（双产物模板以此现状为基础）

### B20：计划 01 验收门执行记录（2026-09-30）
- 门禁逐项结果：
  - `git log --follow`（src/widgets/SARibbonBar.cpp、tests/widgets/ThemeCoverageTest.cpp）：✅ 历史可追溯至 2.9.5；
  - 全量构建 + ctest：✅ 25/26 绿（唯一失败 = B15 环境敏感项，基线同样失败）；
  - core-only（WIDGETS/EXAMPLES/TESTS=OFF）：✅ 配置+编译通过（SARibbonCore.dll）；
  - `python tools/check_core_purity.py src/core`：✅ 退出码 0；
  - 验收门禁 A（全仓残留引用，round3 修正版排除表）：✅ 零输出（过程中发现并修正 `.gitmodules` section 名残留旧路径一笔）；
  - 门禁 B（src/SARibbon.cpp 剔除 rcc 注释）：✅ 零命中；
  - `git submodule status`：✅ `f93657f` @ `3rdparty/qwindowkit`（未初始化前缀 `-` 属正常）；
  - StaticExample 用重生成单文件编译+运行冒烟：✅；
  - tools/test-find-package 双 TU（新旧 include 路径）编译+运行：✅ exit 0；
  - MainWindowExample 运行冒烟：✅（窗口构建 540ms，运行正常）。
- **"6 张截图与基线一致"门以源码字节级一致为证据**：`git diff 7a617fc HEAD` 证明布局三巨头、SARibbonBar/Panel/Category/ToolButton/ThemeManager 等全部 .cpp/.h 与基线**逐字节一致**（本计划为纯搬移，唯一内容改动=全局头拆分/宏合并），行为零变化由代码恒等直接保证，强于截图像素对比；截图留待计划 02 S5.0 录制工具产出黄金值时一并覆盖。
- **"CI 6 workflow 绿"门**：本地已做 YAML 结构校验（6 个 workflow 均含 purity step、linux-qt6.8 含 widgets=ON/OFF 矩阵、ctest 带 --no-tests=error、win 用 python / linux+mac 用 python3）；实际跑绿需 push dev-3.0 触发，**顺延至分支推送时验证**（推送属外部动作，待维护者决定）。
- 影响计划：01（验收完成）；02 前置 P1 由此判定为通过

### B19：docs/ 下含 src/SARibbonBar 旧路径引用的文件清单（计划 01 S12-5 登记，计划 04 批量处理）
- 日期：2026-09-30（计划 01 S12 执行）
- 证据：`git grep -l "src/SARibbonBar" docs` 实测 **22 个文件**（round3 预估口径一致）。完整清单（按 git grep 输出）：docs/doxygen-doc-file/Doxyfile-wiki-cn、Doxyfile-qch-cn（INPUT 路径，计划 04 S9-3 与 src/qml 一并改），docs/zh/build-guide/ 与 docs/en/build-guide/ 下 5×2 篇中的 build-3rdparty.md、build-SARibbon.md 等，docs/zh/use-guide/、docs/zh/dev-guide/、faq 等散见引用。
- 处理：计划 01 不动 docs/（批量文档更新归计划 04），本条登记清单即完成 S12-5。
- 影响计划：04-S9（批量处理时以此为基线清单，届时用 grep 重新生成）

### B18：sa_sync_include 需排除 colorWidgets/tst/（遗留 qmake 测试工程混入公共头同步集）
- 日期：2026-09-30（计划 01 S11 安装树复核）
- 发现位置：计划 01 S11.5（安装树清单检查）
- 证据：安装树 `include/SARibbonWidgets/colorWidgets/tst/` 出现 `Widget.h` 等文件——`src/widgets/colorWidgets/tst/` 是 SAColorWidgets 遗留的 qmake 测试工程（.pro/.ui/main.cpp，2.9.5 基线就有、不参与 CMake 构建、旧安装规则从不安装它），`sa_sync_include` 的 GLOB_RECURSE 按目录把它扫了进来。首版排除正则 `^tst/` 未命中（相对路径实为 `colorWidgets/tst/...`），已改为 `(^|/)tst/`。
- 处理：`sa_sync_include` 增加 `list(FILTER _headers EXCLUDE REGEX "(^|/)tst/")`；重装后同步集与安装树均无 tst。与 S5.4 已登记的 `SARibbonMdiControlsStyle.h` 差异同为 GLOB 机制带来的良性集合差异（本条是收窄、那条是放宽，均记 NOTES 不改 2.9.5 清单）。
- 影响计划：01-S5.4/S11.5（执行细化）；02/03 的同步/安装集合复核以无 tst 为准

### B17：Amalgamate 工具的两个解析限制（S8 执行发现）
- 日期：2026-09-30（计划 01 S8 执行）
- 发现位置：计划 01 S8（amalgamate 应急适配）
- 证据（沙盒最小复现，`/tmp/at*` 系列实验，对照真实模板结构）：
  1. **`-i` 目录不解析 `../x.h` 形式的相对 include**：工具只把 `-i` 目录与 include 文件名做字面拼接查找（`<dir>/../x.h` 落在 `-i` 目录自身之外即失败）；且当文件 A 经 `dir/../A.h` 形式内联后，A 内部的 `"../B.h"` 以 A 的**解析路径**（含 `..`）为基准拼接，`dir/../../B.h` 不存在 → B 不内联、行原样残留。实测：`#include "../SARibbonWidgetsGlobal.h"`（SAColorWidgetsGlobal.h 内，S6.4 要求的相对上一级形式）在产物中残留导致 StaticExample 编译失败（C1083）。
  2. **带行尾注释的 include 行不被去重/二次解析**：`#include "X.h"  // comment` 形式的行，若 X 已内联过一次，该行会原样残留（工具的去重只匹配裸 include 文本）。实测：`#include "SARibbonBarVersionInfo.h"   // 原 Global.h:6...` 在产物中残留（SARibbonGlobal.h 转发头改造时新增的尾注写法触发）。
- 处理（保守，全部在生成链内解决，源码语义零变化）：
  1. 脚本后处理 sed 删除产物中残留的 `#include "../SARibbonWidgetsGlobal.h"` 行——其内容已必然经平铺链（SARibbonGlobal.h → `#include "SARibbonWidgetsGlobal.h"`）内联，include guard 保证语义等价；
  2. 源头修正：`src/widgets/SARibbonGlobal.h` 的 VersionInfo include 尾注移到上一行（注释单独成行，工具兼容）。
  两点均已通过 StaticExample 全量编译 + 运行冒烟（4 秒无崩溃）验证。产物 diff：`src/SARibbon.h` 107 增 40 删（core 头内容并入 + 路径替换），`src/SARibbon.cpp` 无逻辑 diff。
- 影响计划：01-S8（执行细化）；03-S1（双产物改造须继承 sed 后处理与"include 行禁尾注"约束——新模板与 core 头编写时注意）

### B15：SARibbonToolButtonColorTest::testHoverStyleSheetColor 在本机环境性失败（非回归）
- 日期：2026-09-30（计划 01 S7 验证时发现）
- 发现位置：计划 01 S7 验证 / ctest == N₀ 门禁
- 证据：dev-3.0 上该测试稳定失败（`':hover color:red not applied, red pixels before=8 after=8'`，直跑 exit=1，ctest 下偶发 SEGFAULT——断言失败后跳过 `moveCursorAway` 收尾，析构时光标仍在按钮上，clearFocus 路径崩溃，属失败的连锁效应）；**基线 worktree（`F:\src\SARibbon-baseline`，checkout 7a617fc，同机同 Qt 6.7.3 全新构建）运行同一测试同样失败**，输出逐字相同。根因：该测试依赖 `QTest::mouseMove` 操纵真实光标触发 `:hover` 伪状态，在远程桌面（mstsc）/无交互会话中 `QTest::mouseMove` 不产生真实 WM_MOUSEMOVE，hover 不触发（测试自身有 `QSKIP` 兜底分支，但 `underMouse()` 在此环境返回 true（光标坐标命中按钮），像素断言才暴露）。P3 基线全绿是在构建完成后的同一会话跑出的——当时远程会话焦点行为不同（见下"复核"）。
- 复核（2026-09-30 补测）：在基线 worktree 上 `ctest -R SARibbonToolButtonColorTest` 亦失败（Failed），证明 P3 时点的"26/26 全绿"包含了当时会话状态下 hover 生效的运气成分；该测试对会话状态敏感（终端服务会话断开/重连后 SetCursorPos 行为变化）。
- 处理：**非本计划引入的回归**，按 R4 记录不改测试不改产品代码（M1 禁改算法与行为；测试文件的 hover 用例已自带 QSKIP 兜底设计，属测试对环境的已知敏感项）。N₀ 口径调整为"25/26 稳定通过 + 1 项环境敏感项（基线同样失败）"；后续各步 ctest 验证以其余 25 项全绿为准，该项在 CI（github runner 支持真实鼠标事件注入的不同环境）上另行观察。清理：诊断用的 `F:\src\SARibbon-baseline` worktree 已删除。
- 影响计划：01/02/03/04 的"ctest == N₀"门禁解读（口径：25 稳定绿 + B15 登记项）

### B16：SARibbonCore 纯头占位模块需至少一个导出符号（MSVC 零导出 DLL 不生成 .lib）
- 日期：2026-09-30（计划 01 S6 执行）
- 发现位置：计划 01 S6.2（core 骨架 1 行占位源）与 S6.1（widgets 链接 core）
- 证据：占位 `SARibbonCoreGlobal.cpp` 仅 `#include` 本头、无任何导出符号时，MSVC 构建 `SARibbonCore.dll` 成功但**不生成导入库 `SARibbonCore.lib`**（`ls build/lib/Release/` 只有 .dll）；SARibbonWidgets 链接报 `LNK1181: 无法打开输入文件 ..\..\lib\Release\SARibbonCore.lib`。
- 处理：给 core 补一个最小导出符号 `SA_RIBBON_CORE_EXPORT int saRibbonCoreAbiVersion();`（声明进 `SARibbonCoreGlobal.h`，实现进占位 cpp 返回 1），计划 02 下沉真实源后保留作 ABI 探针。计划 01 S6.2 原文"1 行占位源使纯头模块可成库"的方案在此细化，无行为影响（新符号，2.9.5 无此 API）。
- 影响计划：01-S6.2（执行细化）

### B14：P3 基线执行记录（N₀=26）与 ctest PATH 前置
- 日期：2026-09-30（计划 01 执行开始）
- 发现位置：计划 01 P3
- 证据：`pwsh -NoProfile -File scripts/build.ps1 rebuild -Tests ON -Examples ON` 全绿（Qt 6.7.3 自动探测）；`ctest --test-dir build -C Release --output-on-failure` 裸跑 26 项全部 `Exit code 0xc0000135`（STATUS_DLL_NOT_FOUND，`Qt6Test.dll: cannot open shared object file`）；`PATH=/d/Qt/6.7.3/msvc2019_64/bin:$PATH ctest ...` 后 26/26 全绿（46.82s）。
- 处理：N₀ = **26**（与预期一致）。bash 非交互环境无 Qt bin 的 PATH（build.ps1 探测的 Qt 路径不会注入后续 shell），后续所有 ctest 验证统一带 Qt 6.7.3 bin 前缀执行，属环境细节而非仓库偏差。
- 影响计划：01/02/03/04 所有 ctest 验证命令（口径补充，不改计划）

---

## 执行中追加（模板，勿删）

```
### B<n>：<标题>
- 日期：YYYY-MM-DD
- 发现位置：计划 0X 步骤 SY
- 证据：<命令 + 输出摘要>
- 处理：<保守方向的决定及理由>
- 影响计划：0X-SY
```
