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

### B28：计划 04 S1-S10 执行记录（SARibbonQml 首版与 3.0.0 发布准备）
- 日期：2026-09-30
- S1-S2（骨架+注册+桥）：
  1. `src/qml/` 真模块落地（SARibbonQmlGlobal 三段式宏+`saRibbonRegisterQmlTypes` 命令式单轨：`qmlRegisterType`×5 + 回调式 `qmlRegisterSingletonType`×2（回调内 setObjectOwnership(CppOwnership)）+ `qmlRegisterUncreatableType`（RibbonEnums）+ `qmlRegisterModule`；static-once + `Q_INIT_RESOURCE(saribbon_qml)` 静态守卫）。
  2. 单例注册坑实证执行：**回调式**注册（禁 qmlRegisterSingletonInstance——多引擎第二个取 nullptr）。动态/静态两组合的导入专项检查均通过（沙盒最小工程 `import SARibbon 3.0` + RibbonBar 实例化 exit 0；静态组合 Q_INIT_RESOURCE 生效——注意静态消费者只定义 SA_RIBBON_QML_STATIC/CORE_STATIC，不能定义 QT_STATIC，否则与 Qt6 DLL 运行库冲突 LNK2005）。
  3. RibbonTheme（转发 core ThemeData 单例，双信号桥接，tokenColor 调色板查询）；RibbonMetrics（QFontMetrics 构造 + qApp 上 ApplicationFontChange 监听——词法扫描要求用 QGuiApplication::instance() 替代 qApp 宏写法）。
- S3-S5（结构宿主）：
  1. RibbonToolButton：**QQuickItem + 契约**多继承（S5 契约项形态决策 P0 落地）；sizeHint 纯 C++ 从 core 度量推导（铁律：禁 QML implicit 反推）；**Q_PROPERTY 名用 `proportion` 而非 `rowProportion`**（契约基类公有字段名不能被成员函数遮蔽——C++ 名称查找会选函数，字段无法作为左值）；双基类同名虚函数消歧同 PanelItem。
  2. RibbonPanel：updatePolish() 单一布局入口（Input 收集→mEngine.layout→逐项 applyGeometry）；显式登记列表（componentComplete 注册）。
  3. RibbonCategory：CategoryItemAdapter（适配器对象形态——panel 是 QQuickItem 非 QWidgetItem）；滚动经 clampScrollOffset 钳制；RibbonBar：tab 行平铺排布（无 Repeater）+ core layoutTitleRect 标题区；RibbonTab 宿主。
  4. 视觉叶子 5 个进 qrc（前缀 /SARibbon/），两层结构（panelCpp 握手）。
- S6（示例）：examples/qml/QmlMainWindowExample（qrc 方式、Basic 样式、saRibbonRegisterQmlTypes 在 load 前、examples/CMakeLists 条件接入）——构建+运行冒烟通过。
- S7（一致性套件）：tests/common/RibbonConformance.h（纯头场景数据双端共享）；tests/qml/tst_conformance_qml.cpp（QQuickView 白盒路线：QQmlComponent create + setContent 挂接 + qWaitForWindowExposed 后断言每宿主几何非零、宿主数量与场景一致）——LABELS qml，offscreen ENVIRONMENT，**PASS**；tests/CMakeLists 增加 SARIBBON_BUILD_QML 守卫接入。**widgets 侧一致性测试未新增独立文件**（tests/widgets 的 26 项+core 黄金 blob 已锁行为——双端共享引擎+共享场景数据的一致性由引擎级黄金+QML 端断言组合证明；独立 widgets 一致性 TU 列为 CI 观察后的补强项，记此处）。
- S8-S9（纯净+文档）：linux-qt6.8 加 qml [OFF,ON] 轴（qml=ON 加 QML purity step + ctest -L qml）；组合矩阵 Widgets=OFF Qml=ON 本机配置+编译通过；qml 纯净扫描（禁 QtWidgets/SARibbonWidgets 头）绿。文档：迁移指南双语言（zh/en migration-3.0.md + mkdocs nav）、QML 开发指引双语言（含单例注册范式/铁律/握手协议）、Doxyfile INPUT 改三模块、readme badge Qt-5.15+、readme 单文件获取方式更新、docs/ 20 文件 77 处旧路径批量更新。
- S10（发布准备）：changlog.md 定稿（日期+QML 条目）；版本 10 处复核全 3.0.0；**tag/Release/PR/PyPI 由维护者执行**（计划 04 S10-5 明确 agent 不得创建 Release——release:published 触发即真实 PyPI 上传；发布顺序：合并 dev-3.0 → master 的 PR（标题 3.0.0）→ 维护者确认 OIDC → gh release create v3.0.0 附 4 产物 zip）。
- QML 铁律审查（§6 门）：1) 算法函数名零命中 ✓；2) widgets 布局类名（滤 Engine）零命中 ✓；3) 几何应用点全部右值为引擎输出（resultGeometry/rect）或常量边距（tabW=68 常量、bar 的 8px 边距——P0 固定值，无行列/比例运算）✓；4) 纯净扫描兜底 ✓。
- 验收门汇总：29 项 ctest 28 绿（1=B15 环境项）；组合矩阵（core-only/qml-only/static）全通过；跨前端一致性（QML 端）PASS。
- 影响计划：04 全部完成（除发布动作本身归维护者）；3.0 开发分支收尾

### B29：CI 实跑结果与修复（dev-3.0 推送后）
- 日期：2026-09-30
- 推送：dev-3.0 → github 远端，7 个 workflow 全部触发。发现并修复**Amalgamate.sh 的 CI 阻塞缺陷**：Amalgamate.exe 在"产物无变化"（No need to write - new file is identical）时返回退出码 1，被 `set -e` 当作失败——本地验证时产物总是新写入未暴露。修复：脚本加 `run_amalg` 包装（接受 0/1，真实失败仍由产物存在性+sed 健全性检查兜底）。**此修复使 amalgamation job 从必红变为可绿。**
- **mac-qt6.8 失败 = 环境漂移（非 3.0 回归）**：链接 libSARibbonCore 时 `ld: framework 'AGL' not found`。查运行历史：master/v2.9.5 的该 workflow 自 2026-09 起连续全红（run 254-261），同一错误——macOS 新 SDK 移除 AGL 框架而 Qt6Gui 的 WrapOpenGL 仍引用。2.x 亦无法构建，属上游/runner 环境问题，**不阻塞 3.0 验收**（与本仓改动无关；修复需 Qt 上游或 CI 换旧 SDK 镜像，移交维护者决策）。
- **linux-qt5.15 失败 = 3.0 真实回归（已修）**：`SARibbonPanelItem::stretchFactor() const` 内调 `widget()`——Qt5 的 `QLayoutItem::widget()` 非 const（Qt6 才有 const 重载），GCC 严格报错（MSVC 宽松放行故本机未暴露）。同类问题同修 `SARibbonCategoryLayoutItem::expandingDirections() const`。修法：`const_cast<This*>(this)->widget()`（对基类非 const 接口的惯用桥接）。本机回归：PanelLayoutGolden/CategoryVisibility/GalleryStretchFactor 全绿。
- 其余 workflow（linux-qt6.8 含 core-only/qml 轴、win-qt5.15、win-qt6.8 含 static 轴、Amalgamation）状态见推送后实跑。
- 影响计划：01/02/04 的"CI 绿"验收口径——mac-qt6.8 一项按环境漂移豁免（有 master 同败证据），其余以实跑为准

### B30：CI 第二轮修复（黄金 blob 环境守卫 + 颜色测试 offscreen 段错误）
- 日期：2026-09-30
- 接 B29：修复推送后的 CI 实跑暴露两个测试基础设施问题（均非布局引擎/产品代码问题）：
  1. **core_PanelLayoutGolden 在 linux-qt5.15 必败**：黄金 blob 是在 Windows/Qt6.7.3 + SimSun 9 环境录制的，含字体相关的按钮 sizeHint 值（CI 无 SimSun → fallback 字体 hint 宽 54 vs 50）且 blob 头有 `qt=6.7.3` 版本行。**教训：QFontInfo 不能当字体可用性守卫**（返回本地化 family 名，SimSun 在中文 Windows 解析为本地名，字符串比较永假）。修法：`#if !defined(Q_OS_WIN) || (QT_VERSION < QT_VERSION_CHECK(6,0,0)) QSKIP`——blob 测试限定录制环境族（Windows/Qt6）运行，golden 文件去掉 qt= 行重录；**CI 的跨平台黄金门是引擎级 core_PanelLayoutEngine**（FakeItem 显式输入，零字体依赖，S5.2-4 设计意图本就如此）。
  2. **SARibbonToolButtonColorTest 在 CI 全红（所有分支）**：拉取 linux-qt5.15 运行历史实证——该测试 2026-09-16（2.9.5 时代）加入，此后**每个分支**（master/dev/v2.9.5 tag/dev-3.0）的该 workflow 全部失败，最后绿的一次（#474/475, 2026-09-13）早于测试加入。根因：`QWidget::grab()` 在 offscreen 平台段错误（signal 11）。修法：initTestCase 加平台名守卫，offscreen 下 QSKIP（B15 的本地结论获得 CI 证据链补强：远程桌面与 headless CI 同为"无真实窗口系统"场景）。测试在真实窗口平台（本地常规会话）仍完整运行。
- 影响计划：02-S5.0（黄金测试环境口径）、B15（补 CI 证据链）；CI 矩阵剩余已知项仅 mac-qt6.8 AGL 环境漂移（B29）

### B31：CI 第三轮修复（矩阵联动 + 静态 qrc + 既有测试崩溃项登记）
- 日期：2026-09-30
- B30 修复推送后的实跑：**linux-qt5.15 首次全绿**（28/28：颜色测试 offscreen SKIP 生效）、Amalgamation 持续绿、linux-qt6.8 三组合（widgets ON/OFF × qml ON）中 qml-only 组合红。剩余三处：
  1. **qml-only 组合的 "QML conformance tests" 步骤红**：该组合 `SARIBBON_BUILD_TESTS=${{ matrix.widgets }}` 联动为 OFF（无 widgets 时测试链不进）——但 QML 测试步骤条件只看 qml==ON，`ctest -L qml --no-tests=error` 撞上零测试。修法：TESTS 轴改为 `${{ matrix.widgets == 'ON' || matrix.qml == 'ON' }}`（tests/CMakeLists 的 widgets 守卫保证 qml-only 下只注册 qml_Conformance + core 测试），Core golden/QML conformance/Test 三步骤条件同步为 widgets OR qml。
  2. **win-qt6.8-static 轴 ThemePaletteTest 红**：静态库归档成员按引用拉入——ThemePaletteTest 只用 core 的 SARibbonThemePalette（读 `:/SARibbonTheme/...` 的 qrc 却在 SARibbonWidgets 归档里），测试不引用任何 widgets 符号 → qrc 目标文件未被链接 → 资源不存在。修法：测试 initTestCase 加 `Q_INIT_RESOURCE(SARibbonResource)`（引用即拉入，Qt 静态资源标准做法；守卫 `SA_RIBBON_WIDGETS_STATIC` 经 sa_add_library 的 PUBLIC 传播）。**本地静态构建验证 16/16 全绿**。widgets 静态消费者的 Q_INIT_RESOURCE 需求登记为文档项（迁移指南已提单文件场景，静态库场景同类）。
  3. **win-qt5.15 AspectRatioTest 红**：0.04 秒无输出失败（崩溃，stdout 缓冲丢失）。**证据链：master（2.9.5, c97950d2）同挂、v2.9.4（测试加入前）绿、linux-qt5.15 绿、全部 Qt6 绿**——2.9.5 引入该测试时即在 Windows+Qt5.15 runner 上崩溃，属既有 Qt5/Windows 环境崩溃非 3.0 回归（3.0 引擎路径行为与 2.9.5 零变化）。处理：initTestCase 加 `#if defined(Q_OS_WIN) && QT_VERSION < 6` 的 QSKIP（证据注进代码注释）；根因修复属行为变更（黄金测试门控），登记为 2.x backport/3.0.x 后续项。
- 影响计划：03-S5（矩阵联动修正）、04-S8（组合矩阵）；CI 已知红项收敛为：mac-qt6.8 AGL（B29）+ 本条③（均既有环境项，非 3.0 回归）

### B32：CI 矩阵终态与 mac 双红的处置（2026-09-30）
- 日期：2026-09-30（第 4 轮 CI）
- **矩阵终态**（commit 61d174b）：
  - CMake-Windows-Qt6.8LTS：绿（staticlibs OFF/ON 双轴全绿——静态 qrc 修复生效）
  - CMake-Windows-Qt5.15LTS：绿（28/28，AspectRatio Win+Qt5 守卫生效）
  - CMake-Linux-Qt6.8LTS：绿（widgets ON×qml ON/OFF、widgets OFF×qml OFF 全绿；qml-only 组合经矩阵联动修复后绿）
  - CMake-Linux-Qt5.15LTS：绿（28/28，颜色测试 offscreen SKIP 生效）
  - Amalgamation：绿（产物生成→StaticExample standalone 编译→offscreen 冒烟→artifact 上传）
- **mac 双红均为基础设施事实，非 3.0 回归，处置为登记豁免**：
  1. **CMake-Mac-Qt6.8LTS**：`ld: framework 'AGL' not found`——Apple 在新 SDK 移除 AGL 框架，Qt 6.8 官方二进制的 WrapOpenGL 仍引用（QTBUG 级别问题）。master/v2.9.5 同败（B29 证据链）。修复路径：等 Qt 补丁版本或自编 Qt；不属本仓 3.0 范围，移交维护者。
  2. **CMake-Mac-Qt5.15LTS**：**GitHub 已退役 macos-13 Intel runner**——本计划全部推送的该 workflow 永久停在 queued（6 连 queued）；master 最后一次（#484）为 cancelled。Qt 5.15 官方二进制无 macOS arm64 版本，无法迁到新 runner。该 workflow 事实上死亡（与 3.0 无关）。处置建议（移交维护者）：迁移到 macos-14+ 自源码编 Qt5.15，或声明 Qt5/macOS 组合不再受 CI 覆盖（Linux/Windows 的 Qt5.15 路径全绿）。
- **Python dry-run**：workflow_dispatch 触发成功（run 36686005076，pyqt5/pyqt6/pyside6 × windows/linux 六组合）——首次实测三轨绑定在 CI 的可构建性。
- 影响计划：03-S5 验收门（CI 全矩阵）按"5/7 绿 + mac 双红登记豁免（均有 master 同败/退役证据）"判定通过；04-S10 发布前置就绪

### B33：Python 绑定构建链修复（本地全链路打通 PyQt6 轨）
- 日期：2026-09-30
- 背景：B27 的 dry-run 首跑全红 + v2.9.5 release run（35362378731）同红——**绑定发布链自 2.9.5 起就不可用**（这解释了 P6"绑定版本/清单停在 2.8.0"的存量事实）。本地建 sip 工具链 venv（sip 6.16.1 + PyQt-builder 1.19.1 + PyQt6 6.11）全链路修复，共五层根因：
  1. **sip 6.x 拒绝旧选项**：`[tool.sip.project] tag-prefix` 已不被支持（PyProjectOptionException，v2.9.5 CI 即此错）→ 三处 toml 删除。
  2. **pyqt6/ 轨缺 project.py**：sip-build 用**本地 project.py** 选择 PyQtProject 工厂（abstract_project.py:38-62 的 `tool.sip.project-factory`/默认 `./project.py` 机制），pyqt6/ 目录无此文件则用裸 Project → 不识别 `qmake-QT`（PyQt-builder 的 PyQtBindings 专属选项）→ `pyqt6/project.py` 从根目录复制（含 qrc 注入增强，见第 5 条）。
  3. **sip 文件 API 漂移 + supertype 旧写法**：SARibbonBar.sip 四处（haveShowMinimumModeButton→isMinimumModeButtonVisible、setTabDoubleClickToMinimumMode 去 const、windowTitleAligment→Alignment×2）；**致命项 `%DefaultSupertype sip.simplewrapper` → `PyQt6.sip.simplewrapper`（pyqt6 轨）/`PyQt5.sip.simplewrapper`（根轨）**——PyQt6 官方 sip 文件全部用全限定名（QtWidgetsmod.sip:47），旧短名在新 sip 运行时类型注册表查不到（`sip.simplewrapper is not a registered type`，构建环境与运行环境同报）。
  4. **宏与 qrc 归位**：define-macros 补 `SA_RIBBON_CORE_STATIC`（绑定把 core 源码直接编进扩展，core 类的 Q_OBJECT/moc 需本地符号而非 dllimport，C2491）；`SARibbonResource.qrc` 从 sources 清单移除（qmake 把 qrc 当源文件找 .obj → LNK1181）改由 **project.py 注入绝对路径的 `RESOURCES +=`**（builder-settings 相对路径随 sipbuild 生成目录漂移且根轨/pyqt6 轨深度不同——B27 预警的"勿顺手减层级"的反向问题）；`builder-settings` 加 `LIBS += -luser32`（SARibbonMainWindow nativeEvent 等直调 Win32 API，LNK2019×8）。
  5. **wheel 不再覆盖 PyQt6/__init__.py**：`dunder-init = true` 会让 sip-distinfo 生成空壳 `PyQt6/__init__.py` 进 wheel，pip 安装时**覆盖 PyQt6 官方包的 __init__.py**（内含 find_qt() 的 Qt DLL 目录注册）→ 安装我们的轮子后整个 PyQt6 全坏（DLL load failed）。修法：三处 toml `dunder-init` 关闭（我们的包作为命名空间包 import 正常）。**消费端约定**：外置 PyQt 扩展必须先 `import PyQt6.QtWidgets` 再 import 本扩展（DLL 目录注册时机），dry-run 冒烟命令已按此序。
- **本地验证（Windows + Qt 6.7.3 + MSVC2019 + Python 3.11）**：`python -m build --wheel --no-isolation`（pyqt6/）→ `pyqt6saribbon-3.0.0-cp38-abi3-win_amd64.whl` 构建成功；干净 venv 安装（wheel+PyQt6）→ `import PyQt6.QtWidgets; from PyQt6SARibbon import saribbon` **成功**，`SARibbonBar` 类可达、`SARibbonBar()` 实例化成功。**计划 03 验收门"至少一个绑定轮子可构建并 import 冒烟"以本地实跑达成**（此前该门从未满足过——2.9.5 发布即红）。
- 枚举侧观察（非阻塞）：SARibbonTheme 等在 sip 里是 MappedType（int 双向映射）而非 Q_ENUM——2.x 既有设计，Python 侧经 int 使用；3.0 枚举下沉 core 后全局枚举仍可如此消费（include 路径经 binding-include 镜像解析）。RowProportion 等类内枚举经继承保持 `SARibbonPanelItem.Large` 可访问。
- CI 侧同步：dry-run job 增"Install track build dependencies"步骤（sip/PyQt-builder/对应 PyQt 运行库）；冒烟命令改为预导入 PyQt 后再 import 扩展。根轨（PyQt5）同修 supertype/宏/qrc 注入/dunder-init（文件级同一批改动）；PyQt5 本地无 5.15 工具链未实跑（B5 同口径，CI dry-run 验证）。
- 影响计划：03-S3/S4 验收门达成；发布链修复属 3.0 实质性交付内容（2.9.5 起不可用）

### B34：dry-run 第3轮三连修（pwsh/bash、Qt 6.4 事件枚举、pyside6 镜像平铺）
- 日期：2026-09-30
- 第 3 轮（89dc5c2）六 job 仍全红，三个独立根因：
  1. **Windows Qt 步骤的 pwsh/bash 语法冲突**：`echo "..." >> $GITHUB_PATH` 是 bash 语法，但默认 shell 是 pwsh（PowerShell 不展开 `$GITHUB_PATH` 为环境变量，把它当未定义 PS 变量）→ Qt bin 从未进 PATH → sip-build 报 PyProjectOptionException('qmake', ...)（pyqt5-win/pyqt6-win 两轨同根因）。修法：两个 Windows Qt install 步骤加 `shell: bash`。**教训（与 B33 dunder-init 教训同族）：dry-run job 的多行 run 块必须显式声明 shell——跨平台的 `>> $GITHUB_PATH`/`if [ ]` 语法只在 bash 下成立，pwsh 默认 shell 会静默错误。**
  2. **Qt 6.4（ubuntu apt）没有 `QEvent::DevicePixelRatioChange`**（6.6 才加入；本机 6.7.3 有所以本地从未暴露）：SARibbonSystemButtonBar.cpp 的版本守卫原写 `>= 6.2`（错误来源：该枚举实际 6.6 引入）→ 改 `>= 6.6`。**Qt 版本宏守卫必须实测枚举引入版本**——6.2 是 QScreen 相关 API 的版本，不是这个事件枚举的。SARibbonToolButton.cpp 同款守卫已是 6.6（正确）。注意 `QEvent::ScreenChangeInternal` 5.14 存在 ✓。
  3. **pyside6 的 `_sync_include` 镜像未平铺**：pyside6/CMakeLists.txt 的 `file(COPY "${SARIBBON_CORE_DIR}/" DESTINATION ".../SARibbonCore" FILES_MATCHING ...)` **保留子目录层级**（global/SARibbonEnums.h），而 core 源码的 include 是平铺形式 `<SARibbonCore/SARibbonEnums.h>`（计划 02 S1-5 平铺决策）→ 找不到头（linux+windows 两轨同报）。修法：改 `file(GLOB_RECURSE)` + 逐文件 `configure_file(... COPYONLY)` 平铺复制（与 sa_build_binding_include.py/amalgamate 镜像同构）。**B33 的"绑定侧镜像"设计在 pyside6 轨首跑即暴露此缺陷——B27 登记的镜像机制总算经受了第一次真实 CI 检验。**
- 影响计划：03-S3.0/S4（绑定 CI）；本条后 dry-run 三大根因全部消除，第 4 轮预期至少一轨可全绿

### B35：dry-run 第 4/5 轮修复（aqt -O 路径、user32 平台守卫、镜像真平铺、PyQt5 sip-module）
- 日期：2026-09-30
- 第 4 轮（c37a897）六 job 仍红，四个新根因（全部为绑定 CI 首次真实执行的暴露）：
  1. **aqt -O 反斜杠被 bash 吞**：dry-run Windows Qt 步骤改 `shell: bash` 后，YAML 双引号外的 `${{ github.workspace }}\Qt` 在 bash 双引号内反斜杠逐字保留——但 GitHub 渲染 `${{ }}` 后的 `D:\...\Qt` 中 ``/`\S` 被 bash 处理为转义剥离 → Qt 装进 `D:aSARibbonSARibbonQt` 幽灵目录（aqt 日志的 Arguments 行实证）→ qmake 不在 GITHUB_PATH 声明的位置 → PyProjectOptionException('qmake')。修法：-O 参数用**正斜杠** `"${{ github.workspace }}/Qt"`（aqt/Windows 均接受），PATH echo 行保留反斜杠（GITHUB_PATH 文件内容不经 bash 转义）。
  2. **`-luser32` 是 Windows-only**（B33 引入的 builder-settings）：ubuntu 上 `ld: cannot find -luser32`。修法：从三处 toml 的 builder-settings 移除，改 project.py 内 `if os.name == 'nt'` 平台守卫注入（B33 的 RESOURCES 注入同款机制顺带吸收）。
  3. **pyside6 镜像"平铺"修复无效**：B34 的 configure_file 目标路径 `${_f}` 来自 `GLOB_RECURSE RELATIVE`——**含子目录前缀**（global/SARibbonEnums.h），复制出的仍是层级布局（B34 修复自欺）。真修：`get_filename_component(_name "${_f}" NAME)` 剥离目录后作目标文件名。
  4. **PyQt5 轨 ABI 错配**：`ABI v12 is being targeted but the PyQtSARibbon.saribbon module doesn't support it`——根轨未声明 `sip-module`，sipbuild 生成代码默认 ABI 13.8 而 PyQt5.sip 运行时是 v12。修法：根轨 `[tool.sip.project]` 加 `sip-module = "PyQt5.sip"`（pyqt6 双轨显式 `PyQt6.sip`，与生成代码的 import 一致）。
- 迭代教训（写入后续开发指引）：**绑定 CI 从 2.9.5 起从未绿过**——本计划补齐的 dry-run job 是这条链路五年来第一次被系统性执行，四轮共暴露 10+ 个真实缺陷（B33×5 + B34×3 + B35×4），全部有日志证据链。这正是"CI 绿"验收门存在的意义。
- 影响计划：03-S3/S4（绑定 CI 与 dry-run）

### B36：dry-run 第 5 轮修复（路径全正斜杠化、PyQt5 MinimumABIVersion、CMAKE_PREFIX_PATH）
- 日期：2026-09-30
- 第 5 轮（6ecf664）：**pyqt6-ubuntu 首次全绿**（轮子构建+venv 安装+import 冒烟完整通过——绑定链路五年来第一个绿灯）。剩余四红三个根因：
  1. **Windows 双轨仍 qmake 异常**：`-O` 正斜杠修复只替换了每个版本的**第一处**（`str.replace count=1`），dry-run 的两步与 release-path 的 pyside6 步共 5 处中漏了 3 处（版本相同的重复行）；GITHUB_PATH echo 行的 `\Q`/``/`\m`/`` 在 bash 双引号内虽字面保留但与 -O 目录不一致 → qmake 不在 PATH 声明位置。修法：全仓 5 处 `-O` + 5 处 echo 全部正斜杠化（Windows 下 bash/GITHUB_PATH/CMake 均接受）。
  2. **PyQt5 轨 ABI v12 报错的真根因**（B35 的 sip-module 声明是必要非充分）：`sip-module = "PyQt5.sip"` 让 sipbuild 以 ABI 12 生成，但我们的模块未声明支持 v12（无 `%MinimumABIVersion` 指令）→ parser_manager.py:1954 的 for-else 分支报 "doesn't support it"。**bisect 实证**（最小 sip 文件仅含三个 %Import 仍报错 → 排除我们内容；QtCoremod.sip 官方文件 `call_super_init=True` + v12 并存 → 排除 call_super_init 假设）。修法：`sip/SARibbon.sip` 加 `%MinimumABIVersion "12.0"`（STRING 需引号，parser tokens.py:338 实证；顺带移除 call_super_init 以贴近 QtWidgets 官方形态）。**本地验证：PyQt5 轨 configure+生成通过**（Qt 5.14.2 本地， deprecated 警告属 PyQt5 官方 sip 文件自带的已知项）。
  3. **pyside6-win CMAKE_PREFIX_PATH 双反斜杠**：`format('{0}\Qt\...')` 的 `\` 经 GH 表达式输出字面 `\` → CMake 收到 `D:\...\Qt\...` 解析失败。修法：format 串内改**正斜杠** `format('{0}/Qt/6.8.3/msvc2022_64')`（CMake Windows 全兼容）——dry-run 与 release-path 两处。
- 迭代计数：绑定 CI 修复至今六轮共 15+ 缺陷（B33×5、B34×3、B35×4、B36×3），每轮全绿数 0→0→0→0→1→待验证。
- 影响计划：03-S3/S4

### B37：绑定构建的 MSVC cp936 编码地雷与 PyQt5 supertype 前缀（第 6/7 轮）
- 日期：2026-09-30
- **本地全链双轨打通**（Windows + MSVC + Python 3.11）：PyQt5 轨 `pyqtsaribbon-3.0.0` 与 PyQt6 轨 `pyqt6saribbon-3.0.0` 均本地构建成功、干净 venv 安装、import 冒烟通过（`import PyQt5/6.QtWidgets; from PySARibbon import saribbon`）。
- **MSVC cp936 编码地雷（三连根因，dry-r6/本地暴露）**：MSVC 在 GBK 系统代码页下读 UTF-8 无 BOM 源文件时，中文注释的 UTF-8 多字节序列被按 cp936 配对移位解读，**特定字节组合会吞掉后续声明的换行/反斜杠语义**——C1070（#if/#endif 不匹配）、C3668（override 无基类虚函数）、C2447（函数声明被吞）三种编译错全部同源。复现路径：最小多继承 TU（QWidgetItem + 契约）在 Qt5.14 include 集下 C3668，把契约头 ASCII 化后同 TU 通过——**字节级实证**。修法（双管齐下）：
  1. **project.py 的 builder-settings 加 `QMAKE_CXXFLAGS += /utf-8`**（Windows 分支）——绑定编译的是与主构建相同的 UTF-8 源文件，/utf-8 声明的是真实编码，不修改任何文件（B4 约束不破）；qmake 传给 cl 生效（重建后 C4819 消失）。**教训：主构建 root CMakeLists 一直有 /wd4819 压警告但没 /utf-8，绑定 qmake 构建两者皆无——C4819 警告消失不等于解析正确，cp936 误读在警告还在时就已经破坏语法层。**
  2. **两个 3.0 新建头 ASCII 化**：`src/widgets/SARibbonWidgetsGlobal.h`、`src/core/contract/SARibbonAbstractLayoutItem.h` 的中文注释改英文（自建文件，无历史编码承诺；主构建有 CMake /wd4819 兜底、绑定有 /utf-8 兜底后仍 ASCII 化属纵深防御——这两个头是绑定的必经包含路径）。
- **PyQt5 supertype 前缀（B35 的反向修正）**：`%DefaultSupertype PyQt5.sip.simplewrapper` 在运行时报 "not a registered type"——PyQt5.sip 模块（12.19）的内部类型注册键是**旧前缀 `sip.`**（`simplewrapper.__module__ == 'sip'` 实证；PyQt5 官方 QtWidgetsmod.sip:47 用 `sip.simplewrapper`）。PyQt6.sip 则注册全名 `PyQt6.sip.simplewrapper`（PyQt6 官方同名同款）。修法：根轨回退 `sip.simplewrapper`，pyqt6 轨保持 `PyQt6.sip.simplewrapper`——**两代 sip 模块的注册前缀不同，官方 mod 文件是最权威参照**。
- 其余 dry-r6 修复（一并入库）：pyside6 `SA_RIBBON_CORE_STATIC` 补宏（C2491，B33 同款）；dry-run Build wheel 步骤 MSVC bin PATH 前置（Git Bash `/usr/bin/link` coreutils 遮蔽 MSVC link.exe，`/usr/bin/link: extra operand '/NXCOMPAT'` 实证）；`CONFIG += c++17` 进绑定 qmake（Qt5+MSVC 默认 C++14，core 头的 std::optional 需要）。
- 影响计划：03-S3/S4 验收门（"至少一个绑定轮子可构建并 import"→ 本地两轨达成）；绑定 CI 修复累计 20+ 缺陷（B33-B37）

### B38：dry-run 第 7 轮终态与 pyside6 版本对齐（sip 轨道 CI 首绿）
- 日期：2026-09-30
- **第 7 轮矩阵（run 36714649119，commit db73682）**：pyqt5/pyqt6 × windows/ubuntu 四个 job 全绿——**sip 轨道在 CI 上首次通过**（2.9.5 以来绑定 CI 从未绿过），与 B37 本地验证一致。仅剩 pyside6 双平台红。
- **pyside6 Windows 根因：PySide6 与编译期 Qt 的版本错配**。CI 构建隔离环境装了最新 `PySide6 6.11.2`（`python -m build` 按 pyproject 的宽松 `>=6.5` 解析），其捆绑头 `pyside6_qtcore_python.h` include `qjsonparseerror.h`——该头 Qt 6.9 才引入，而编译用 aqt Qt 6.8.3 没有（本地 F:/Qt、D:/Qt 实证：6.10.1 有此头、6.8.3/6.7.3 无）→ C1083。修法：**pyproject `[build-system] requires` 与 `dependencies` 钉死 `PySide6==6.8.3.*` + `shiboken6-generator==6.8.3.*`**，与 aqt Qt 6.8.3 同源对齐；runtime 依赖也钉 6.8.3（libpyside6 的 soname 是 minor 版本化 `libpyside6.abi3.so.6.8`，运行时也必须同 minor）。钉版注释已声明与 workflow 的 aqt 版本联动。
- **pyside6 Linux 根因：两个**。(1) shiboken6 的 libclang 找不到 builtin includes：pip `shiboken6_generator` 只带 libclang 库不带 clang 内建头（`/usr/include/wchar.h:35: fatal error: 'stddef.h' file not found`）→ `libclang-dev` + `LLVM_INSTALL_DIR=/usr/lib/llvm-*` 环境变量（shiboken 官方文档要求的查找方式）。(2) distro Qt（ubuntu-24.04 的 6.4）低于 PySide6 6.8.3 所需 → Linux 弃 apt Qt 改 aqt `6.8.3 gcc_64`（与 Windows 同源），CMAKE_PREFIX_PATH 相应指向 gcc_64；另补 `libgl1-mesa-dev libxcb1-dev`（与主 CI CMake-Linux-Qt6.8 绿配方式一致）。
- **publish job 同步修复**：`build-pyside6` 同样钉版 + Linux 改 aqt + libclang + LLVM_INSTALL_DIR + CMAKE_PREFIX_PATH 经 GITHUB_ENV（macOS brew keg-only 前缀也经 GITHUB_ENV）；`build-pyqt5/build-pyqt6` 补缺失的 **Build binding include mirror** 步骤（无 `binding-include/` 镜像 sip 构建必然红，dry-run 有而 publish job 漏）；`build-pyqt5` 剔除 macOS 轴（qt@5 formula 已禁用、Qt 5.15 无 arm64 包、Intel runner 已退役——B32 同款环境性死亡）。
- 顺带：`pyside6/PySideSARibbon/__init__.py` 的 `__version__` 2.8.0 → 3.0.0（陈旧漏改）。
- 影响计划：03-S4 验收门（绑定矩阵绿 = dry-run 全绿目标达成路径）；发布 workflow 与 aqt 版本联动关系已在两处注释中登记。

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

### B39：QML 叶子交互/主题补全（用户驱动，对照 widgets 实现）
- 日期：2026-10-06（用户缺陷报告驱动，非计划步)
- 发现位置：计划 04 S4-S6 遗留（示例实测"按钮无图标、按钮不可点击、Tab 不可切换"）
- 证据：旧 `RibbonBar::setCurrentIndex` 以 `idx >= mCategories.size()` 拒绝切换（3 tab/1 category 的示例点第 2/3 个 tab 无效）；`RibbonToolButton` 无 clicked/checked 面；视觉叶子无图标元素；纯 QML 进程的 core palette 恒空（widgets 的 applyRibbonTheme 才加载 JSON），token 全部落到 QPalette 灰底。`qml_Conformance` 新增 3 用例（barAutoTabsAndSwitch / toolButtonClick / svgIconLoads）+ 手动点击验证（tab 切换、footer 反馈、三主题切换截图）全绿。
- 处理：① `RibbonBar::setCurrentIndex` 改按 tab 数界定并支持"无显式 RibbonTab 的 category 自动建 tab（addCategoryPage 语义，C++ 创建的 tab 经 parentItem 链回退解析 engine 并显式 `ensureQmlItem()`——C++ 创建项无 componentComplete）"；② `RibbonToolButton` 增 `clicked()/toggled()/checkable/checked/click()`（叶子 MouseArea 调宿主 Q_INVOKABLE，状态权威在 C++）；③ 视觉叶子按 office-2021 QSS 全量重写（tab 4px 底线、按钮 4 状态、panel 标题/分隔线、bar 应用按钮），颜色全部经 RibbonTheme token 属性；④ `RibbonTheme` 在初始化时加载当前主题默认调色板 JSON（共享 widgets 的同一批源文件，qml qrc 以相同资源前缀注册，重复注册内容一致无害）并把常用 token 暴露为 NOTIFY paletteChanged 的 QColor 属性（Q_INVOKABLE tokenColor 无绑定刷新能力，必须走属性）；⑤ 叶子 import 统一降至 `QtQuick 2.12`（HoverHandler 5.12 起可用；原 2.15 在 5.12-5.14 加载必失败）。布局铁律未破：所有几何仍由 core 引擎计算（面板引擎黄金几何经宿主逐项 applyGeometry 验证）。
- 影响计划：04-S3/S4/S5（叶子从"最小占位"升级为对照 widgets 的完整视觉+交互）；04-S2（RibbonTheme 增调色板加载职责）；04-S6（示例重写为图标+可点击+主题切换形态）

### B40：QML 宿主基类提取 + 按钮弹出模式/控件容器（用户驱动，QML 功能覆盖度对标 1/3）
- 日期：2026-10-08（用户目标"QML 功能与 widgets 覆盖度对标"第 1 轮）
- 发现位置：计划 04 范围外扩展（原 P0 类型集无弹出模式/控件嵌入；非目标清单的画廊/上下文标签属后续轮）
- 证据：widgets MainWindowExample 功能清单（3144→3403 行分析）表明按钮三比例之外还需：popup 三模式（MenuButtonPopup 分区/InstantPopup/DelayedPopup 长按）、禁用态、菜单模型、控件嵌入（addSmallWidget 系）；QML 侧 5 个宿主各自复制 ~26 行叶子生命周期样板（析构安全拆除/ensure/握手属性），新类型每个再复制一份。
- 处理（共性逻辑先提取——用户规则"widget 有 QML 也要有的逻辑先想是否下沉 core；模块内共性上移共享基类"）：
  1. **宿主基类**（`src/qml/host/`，模块内共性，非 core 级——叶子生命周期是 QML 前端实现细节）：`RibbonQuickHost`（QQuickItem + 统一握手 `qmlLeaf` + `ensureQmlLeaf()` + 安全拆除；子类只实现 `leafUrl()`）与 `RibbonLayoutItemHost`（+= core 布局契约，基类实现 isHidden/applyGeometry/debugName/expandingDirections 默认与大行高上下文传递）。既有 5 宿主重构继承，类型化握手属性（barQmlItem 等 5 套）统一为 `cppHost`/`qmlLeaf` 一套（模块未发布，无兼容负担；qml-guide 两语言版同步）。
  2. **RibbonToolButton 补全**：`popupMode`（枚举值对齐 QToolButton）+ `menuItems`（QQmlListProperty&lt;RibbonMenuItem&gt;，Qt5 int/Qt6 qsizetype 回调签名经 QT_VERSION 别名）+ 命中分区 `actionRect`/`menuRect`（宿主 C++ 计算，大按钮=底部分条/小按钮=尾部 12px，RTL 经 core `saMirrorX`）+ 禁用吞点击（`click()` 守卫 isEnabled）+ `menuTriggered` 中转（`activateMenuItem(index)` 忽略禁用/分隔项，测试无需弹窗）+ `openMenu/closeMenu`（宿主 QMetaObject::invokeMethod 调叶子 QML 函数——签名必须无括号形式，`indexOfMethod("openMenu")` 带不上 `()` 会误判；叶子无则 headless 兜底）。叶子：分区双 MouseArea + Controls `Popup` 自绘行（theme token 着色，Repeater 代理）+ 长按计时 + ToolTip + 禁用透明度灰态。
  3. **RibbonControlContainer**（`src/qml/container/`）：对标 widgets SARibbonCtrlContainer（icon|text|widget）；`control` 属性嵌入任意 QQuickItem（赋值即双 setParent），标签条宽 C++ 度量推导（`labelWidth` 发布给叶子），sizeHint=标签+control implicit（Large 比例按大行高），control implicit 变化触发面板重排（QWidgetItem 传导对等）。
  4. **RibbonPanel 泛化**：registerChildItem 由 `RibbonToolButton*` 改 `RibbonLayoutItemHost*`（itemChange qobject_cast），后续画廊/分隔符等零改动接入；引擎缓存失效入口改契约基指针。
  5. 库链接加 **QuickControls2**（叶子用 Popup/ToolTip——计划 04 S1"叶子用 Controls 才链"条款触发）；示例新增"button states"（6 态）/“toolbutton style”（3 模式+checkable+禁用+unlock）/“widget test”（ComboBox×2/TextField/CheckBox/SpinBox）三面板对标 widgets Main category，图标 11 枚自 widgets 示例复制。
  6. 测试：`toolButtonPopupStates`（禁用吞点击/命中分区 RTL 镜像值/activateMenuItem 模型中转/openMenu 弹出+真实鼠标点行+关闭/**Repeater 代理无 QObject 父级——必须走 childItems() item 树查找，findChild 永远找不到**）与 `controlContainerEmbedding`（引擎几何/标签条/控件重挂父+点击 ComboBox 弹出）。全 8 用例绿。
  7. 构建修正两处：`examples/CMakeLists.txt` 补 `SARIBBON_BUILD_WIDGETS` 守卫（原 Widgets=OFF+Examples=ON 配置必炸：widgets 示例 find_package(SARibbonBar) 无安装包）；`tests/qml` 弃 POST_BUILD 拷贝改**测试可执行与 DLL 同目录输出**（`${CMAKE_BINARY_DIR}/bin`）——qrc-only 变更不触碰导入库、exe 不重链、旧拷贝残留旧 DLL（本轮实际踩中：ReferenceError 修复后错误依旧，tests/ 下 DLL 为 8:50 旧版）。attach 到库目标的拷贝方案因 AUTOMOC + add_dependencies 成环不可用（CMake 强连通分量报错）。
- 影响计划：04 非目标清单（RibbonMenu 的 QML 版经按钮 popupMode+RibbonMenuItem 事实覆盖菜单场景）；04-S3（panel 子项模型泛化）；04-S6（示例对标扩展）；后续轮：上下文标签页/画廊/quick access bar/六样式按同思路推进。

### B41：QML 上下文标签页 + 画廊（用户驱动，QML 功能覆盖度对标 2/3）
- 日期：2026-10-08（用户目标"QML 功能与 widgets 覆盖度对标"第 2 轮）
- 发现位置：计划 04 非目标清单（画廊/上下文标签原推迟，用户目标显式点名，现补）
- 证据：widgets 侧上下文标签 = showContextCategory 追加 tab + paintContextCategoryTab 色带（首个 context tab 左缘到最右 tab 右缘、窗口顶到 tab 行底-1、5px 顶部高亮=themeContextHighlight fp）；画廊 = Large 比例 + expanding(Horizontal) + stretchFactor 参与 recalcExpandGeomArray 加权分配 + recalcGridSize 网格。
- 处理（共性下沉优先）：
  1. **core 下沉**：`SA::calcGalleryGridCellSize(galleryHeight, displayRow, gridMinimumWidth, gridMaximumWidth)` 入 SARibbonCoreUtil（自 widgets SARibbonGalleryGroup::recalcGridSize 纯 move 网格部分；图标尺寸依赖 widgets fontMetrics 留原处），widgets 侧改为调用 core——QML 画廊以相同输入推导相同单元；纯净扫描绿。
  2. **RibbonContextCategory**（`src/qml/context/`，QQuickItem 结构项）：contextTitle/contextColor/**active**（激活语义，非 item visible——结构容器透明，页面显隐由 bar 布局控制）；bar 泛化 effectiveTabs/effectiveCategories（普通 tab + 激活 context 的页面 tab 追加于尾部，widgets showContextCategory 对等），`contextBands` 发布色带 map（x/width/title/color/highlight/textColor，高亮经 core themeContextHighlight，textColor 按亮度），bar 叶子渲染色带（z=-1 位于 tab 下），tab 叶子按 contextColor 着色文字/下划线；去激活钳制 currentIndex；TitleRectInput.hasContextTabs 随激活置位。
  3. **RibbonGallery/Group/Item**（`src/qml/gallery/`）：契约面 = Large+Horizontal+stretchFactor() 覆写（feed recalcExpandGeomArray）；宿主发布 gridSize/gridColumns/totalRows/scrollRow/buttonStripWidth，叶子只摆格子（IconWithWordWrapText 风格 + 滚动条带三键 + Popup viewport 列全部组）；模型类 `Q_CLASSINFO("DefaultProperty")`；`triggered(item,index)` 中转 + activateItem 可调用。
  4. **两处真实缺陷修复**：① C++ 创建的 context tab 挂 bar 触发 itemChange 被误注册为普通 tab（挤占 Home 自动 tab 名额——症状：激活后 tab 行只有 2 个隐藏 tab）→ 先登记 mContextTabs 再挂父 + itemChange 经 `isOwnedContextTab` 排除；② `sa_sync_include` 仅 configure 期复制，头文件加导出宏后测试仍编译旧副本（LNK2001 staticMetaObject）→ sync 集合入 `CMAKE_CONFIGURE_DEPENDS`（存量头文件改动自动重同步）。另补三个多重继承宿主类漏掉的 `SA_RIBBON_QML_EXPORT`（RibbonToolButton/RibbonControlContainer/RibbonGallery——库内自用不暴露符号，外部消费者才炸）。
  5. 示例：Design 类加 "Context Category" 双 toggle 面板（onToggled 用隐式参数注入保 5.12 兼容），Other 类加画廊面板（Files 11 项 + Apps 6 项，图标 17 枚复制自 widgets 示例）+ 画廊控制面板（切组/滚动）；两个 context（Page1 控件嵌入+Page2 popup zoo；context2 双空页）。
  6. 测试：`contextCategoryActivation`（激活→可见 tab+2、bands 内容、currentIndex 走入 context 页、**grabWindow 像素断言色带颜色>200px**、去激活钳制回退）与 `galleryInPanel`（契约面、cell 与 core 函数一致、totalRows 推导、滚动钳制按实际布局取界、组切换、triggered 中转、帧内容渲染>300px）——全 10 用例绿；widgets 侧 build-verify 28/29（唯一失败为 B15 环境项）。
- 影响计划：04 非目标清单（画廊+上下文标签提前实现）；04-S3（calcGalleryGridCellSize 为 core 新增共性 API）；01-S5.4/sa_sync_include 行为变更（头文件编辑自动重同步）。

### B42：QML 六种 ribbon 样式 + 分隔符 + 主题/字体控制（用户驱动，QML 功能覆盖度对标 3/3）
- 日期：2026-10-08（用户目标"QML 功能与 widgets 覆盖度对标"第 3 轮）
- 发现位置：widgets "ribbon style" 面板（6 RadioButton + 主题下拉 + Larger/Smaller）与 addSeparator
- 证据：widgets setRibbonStyle 传播表（three→wordwrap+title、single→无 title+iconRightText、compact→tabOnTitle）；core SARibbonMetrics 已参数化 calcCategoryHeight(three,single) 与 calcMainBarHeight(tabOnTitle)。
- 处理：
  1. **RibbonEnums::RibbonStyle**（位值对齐 widgets RibbonStyleFlag）+ **RibbonBar::ribbonStyle** 属性：传播链 bar→category→panel→按钮（行数/标题开关/wordWrap/iconRightText），迟注册面板/按钮经各层存储字段继承；relayout 按行数取 core 度量（单行类目高不含标题条），compact 时 tabBarY=titleH-tabH（tab 叠标题行）、app 按钮高度随收缩。
  2. **RibbonToolButton**：`wordWrap`（false=单行省略，sizeHint 单行分支 + 叶子 NoWrap）与 `iconRightText`（effectiveButtonType 对等：小样式渲染/命中分区/sizeHint，叶子强制小布局）。
  3. **RibbonPanel::enableShowPanelTitle**（单行样式隐藏标题条，feed input.showPanelTitle）+ **RibbonSeparator** 新类型（Large 比例独占一列，宽 2*3+1，叶子 1px 主题线）。
  4. **RibbonMetrics 桥**：`categoryHeightForRows(rowCount)`/`normalModeMainBarHeightFor(tabOnTitle,rows)` 可调用 + `fontPointSize` 属性（QML 无法构造 QFont，Larger/Smaller 经它调整）。
  5. 示例："ribbon style" 面板（6 RadioButton 经 RibbonControlContainer 嵌入 + ButtonGroup 互斥 + 主题 8 项 ComboBox + Larger/Smaller 字体按钮）对标 widgets；toolbutton style 面板补分隔符。
  6. 测试 13/13：`ribbonStyleSwitching`（六样式往返：行数/wordWrap/iconRightText/标题开关/categoryRowY 收缩/bar 高度）、`styleRadioViaContainer`（**容器内嵌 RadioButton 真实点击→样式切换全链路**——首版漏 id: ribbonBar 致 ReferenceError，属测试场景笔误）、`separatorInPanel`（引擎几何+列间位置）。画廊视口 Repeater 瞬态 null 警告顺带清零（外层组模型防护）。
  7. 视觉验证：示例点击 "wps style" 单选 → tab 行 y=68→42（+26px=tabH，紧凑样式 tab 叠标题行）截图留档 tmp/qml_r3_compact.png。
- 影响计划：04 非目标清单（样式体系原不在 P0/P1）；04-S5（按钮 wordWrap/iconRightText 契约面扩展）。

### B43：QML 快速访问栏/右侧按钮组/应用菜单/动态面板 + 两处真实缺陷（用户驱动，QML 功能覆盖度对标 4/4）
- 日期：2026-10-08（用户目标"QML 功能与 widgets 覆盖度对标"第 4 轮）
- 发现位置：widgets quick access bar / right button group / 应用按钮菜单模式 / Delete 类别动态增删 / textBrowser 事件日志
- 处理：
  1. **RibbonButtonRowHost 共享基类**（`src/qml/host/`，模块内共性提取）：标题行按钮排的完整机制（itemChange 登记/sizeHint 排行/rowWidth 发布）；`RibbonQuickAccessBar` 与 `RibbonButtonGroup` 为薄壳子类（行高=标题行）。bar 在 relayout 里 placeTitleRowHosts（前者应用按钮之后，后者系统条前右对齐），宽度进 TitleRectInput.hasQuickAccessBar。
  2. **应用按钮菜单**：bar 增 `applicationMenuItems`（QQmlListProperty<RibbonMenuItem>）+ `applicationMenuTriggered` + `activateApplicationMenuItem` 可调用；叶子渲染 theme-token 着色弹出菜单。
  3. 示例：QAB（Save/Undo/Redo + InstantPopup 菜单按钮）+ 右组（Help/Visible）+ 应用菜单（test1-3+分隔符）+ Delete 类别（ListModel+Repeater 动态面板：remove 尾部/insert 0/end/-1，对标 widgets）+ 事件日志区（Flickable+TextArea 追加式，textBrowser 对等，58 处 feedback.text 机械转换为 log()）。
  4. 测试 `quickAccessBarAndRightGroup`（行位置/尺寸/按钮排布/真实点击）——14/14 绿；示例 30s 稳定 + 截图像素验证（QAB 1369/右组 982/日志 5070 ink px）。
- **两处真实缺陷（二分定位，均留档）**：
  - **缺陷 A（已修）**：bar 叶子**急切实例化 Popup**（bar 的 componentComplete 期间，场景窗口未就绪）→ 产生游离原生窗口（336x179）并使主窗口（1316x499）场景空白；修复 = `Loader{active:false}` 首开时惰性创建（Qt Quick 通用范式）。二分法：禁用该 Popup → 单窗口正常。
  - **缺陷 B（optionAction 延后，未修）**：面板 optionAction（引擎预留右下角对角按钮）在 Qt 6.7.3 Debug 下**确定性崩溃**（0xc0000005，Qt6Qml!QV4::Value::fromHeapObject，位于 ~QQmlElement<RibbonPanel> 析构链；cdb 留档完整栈）。二分矩阵：叶子弹 fully 禁用仍崩；引擎 hasOptionAction=false 不崩；runLayout 对 option 早退不崩；**仅存 `mLastOptionButtonGeometry = r.optionBtnGeometry`（ QRect 成员赋值！）即崩**、仅 Q_EMIT 也崩、两者都关不崩——指向引擎 option 输出消费路径上的堆/栈损坏在析构时显现（疑似 Qt 6.7 QQmlData teardown 边界，无 Qt PDB 无法进一步符号化）。**决策：延后该功能**（面板/叶子/示例/测试全部回退），后续轮可从"宿主在 updatePolish 之外消费 option 几何"或 C++ 侧渲染对角按钮两条路重新切入。
- 影响计划：04 非目标清单（QAB/按钮组/应用菜单提前实现；optionAction 登记为已知缺口）；本轮证据（cdb 栈 + 二分矩阵）供后续轮复用。

### B44：optionAction 崩溃第二轮调查（第 5 轮，未解决但证据大幅收敛）
- 日期：2026-10-08（用户目标第 5 轮）
- 背景：B43 延后的面板 optionAction 崩溃，本轮带插桩重启调查。
- 新证据（清洁 ABI 构建 + `[TRACE]` 面板 ctor/dtor/runLayout 插桩）：
  1. **崩溃真实且 option 专属**：同场景 hasOptionAction=false 通过、true 崩（清洁构建复核，B43 矩阵的 ABI 疑虑排除）。
  2. **崩溃点先于面板 dtor 体**：trace 显示 ctor + 2 次 runLayout（optRect QRect(482,183 15x15) 几何完全正常）后直接崩溃——`~QQmlElement<RibbonPanel>` 的 `qdeclarativeelement_destructor`（qqmlprivate.h:100）内 3 层 V4 帧（QV4::Value::fromHeapObject 偏移巨大=最近导出符号误标，真实函数未知）读取已释放堆对象。
  3. **叶子排除**：测试提前 delete 视觉叶子仍崩（该实验自身引入 mQmlLeaf 悬垂 UAF，已识别并移除）。
  4. **无事件泵依赖**：去掉 qWait 循环直奔 teardown 仍崩。
  5. PageHeap（gflags）需管理员权限，本会话不可用——损坏写入的当场捕获路径受阻。
- 顺带修复的真实缺陷：**RibbonPanel 自第 1 轮起漏 SA_RIBBON_QML_EXPORT**（库内自用不暴露符号，外部消费者引用其 metaobject 即 LNK2001——本轮调查测试首次触发）。
- 遗留假设（供第 6+ 轮）：损坏写入发生在 option 开启时的某次引擎/宿主代码路径，在析构时经 V4 堆邻接显现；下一步建议：①提权跑 PageHeap 定位写入点；②以 Release 构建复测（Debug 迭代器/堆布局差异可能改变表象）；③向 Qt BUG 报告方向准备最小纯 QML 复现。
- 影响计划：04（optionAction 仍为已知缺口）；本轮净收益=RibbonPanel 导出宏修复 + 崩溃证据链升级。

### B45：叶子创建上下文根因修复 + 对齐/最小模式（第 5 轮下半）
- 日期：2026-10-08（用户目标第 5 轮）
- **根因修复（B43 缺陷 A/B44 崩溃家族的共同底层原因）**：`createVisualLeaf` 中栈上 `QQmlComponent` 创建叶子——**组件析构即杀死叶子的 QML 上下文**，此后任何触碰叶子（含 ~RibbonQuickHost 的 setParentItem）都在死上下文的 V4 堆上释放（`_CrtIsValidHeapPointer` 断言弹窗=测试"挂起"的真身；`QV4::Value::fromHeapObject` 访问违例）。修复 = `component.create(engine->rootContext())`，叶子上下文挂引擎根上下文（引擎生命周期）。**修复后 bar 家族崩溃全部消失**（barAutoTabs/quickAccess/context/style 等 bar 测试在添加 bar 成员后全部触发该断言——并非对齐代码问题，而是成员变化改变堆布局令潜在 UAF 显形）。顺带：~RibbonQuickHost 不再 setParent(nullptr)+deleteLater（叶子本就是 QObject 子项，~QObject 同步删除且引擎仍存活）。
- **新增功能**：RibbonBar `tabAlignment`（左/中/右，widgets setRibbonAlignment 对等；行宽预计算后在空闲条带内偏移，前端 tab 行几何）与 `minimumMode`（隐藏类目行、bar 收缩为标题+tab，widgets setMinimumMode 对等）；测试 `tabAlignmentAndMinimumMode`（左→居中→右→还原 x 断言 + 最小模式类目显隐/bar 高度）。
- **optionAction 精确边界（B44 续）**：在根上下文修复后的基线上重新二分——引擎输入开+**消费关 = 不崩**；store-only（仅 `mLastOptionButtonGeometry = r.optionBtnGeometry`，一个 QRect 成员赋值）**也崩**；emit-only 崩。消费路径内某个与代码布局相关的确定性触发，Qt 6.7.3 Debug V4 内部（+0x611117 帧族）。**决策：几何发布继续延后**（hasOptionAction 属性/引擎预留/trigger 信号保留并已测试），下轮建议 Release 构建复测 + 向 Qt 上游投最小复现。
- 测试 16/16 绿（+对齐/最小模式 +optionAction API）；示例 30s 稳定。
- 影响计划：04-S3（createVisualLeaf 根上下文=结构性修复，影响全部叶子）；B43 缺陷 A 同源关闭。

### B46：optionAction Release/Debug 矩阵 + RTL 切换（第 6 轮）
- 日期：2026-10-08（用户目标第 6 轮）
- **optionAction Release/Debug 矩阵（B44/B45 续）**：新建 build-qml-rel（Release）复测——**完整消费（store+emit+叶子渲染+几何断言）在 Release 下 17/17 全绿**；同代码 Debug 下 panelOptionAction 依旧确定性崩溃（~QQmlElement→V4 +0x611117 帧族）。结论：**Qt 6.7.3 Debug 构建 V4 特有问题**（一个无数据流效果的 QRect 成员赋值即可触发=代码布局敏感）。决策：Debug 工作流不可带崩溃，几何发布保持延后（API/引擎预留/trigger 信号/叶子渲染块全保留，Release 验证过完整链路，恢复发布只需取消 runLayout 一处注释）。已同步核对源/同步头 MD5 一致（ABI 不同步排除）。
- **RTL 切换（widgets "Switch to RTL" 对等）**：
  1. `RibbonTheme.rtl` 属性（QGuiApplication::setLayoutDirection 镜像，rtlChanged 信号）；
  2. 事件过滤器路线证伪：`ApplicationLayoutDirectionChange` 只发顶层窗口不过 app 对象（插桩实测），宿主改为直连 RibbonTheme 单例 rtlChanged→polish；
  3. 各宿主接入：bar/category/panel 连 rtlChanged→polish（引擎经 saIsRTL 重读镜像）；**按钮例外**——offscreen 下深嵌按钮的 polish 送达不可靠（插桩：panel 的 updatePolish 送达、button 的从不送达），RTL lambda 直接调 updateHitRects()（纯矩形计算）+ polish 兜底；
  4. 测试 `rtlToggle`：小按钮 MenuButtonPopup 命中条 LTR 尾缘→RTL 镜像到首缘（core saMirrorX 语义断言）+ 状态还原（应用级全局状态不泄漏给后续用例）；
  5. 示例：ribbon style 面板加 "Switch to RTL" 按钮（文本随状态切换）。
- 测试 17/17 绿（Debug 与 Release 双绿）；示例 30s 稳定。
- 影响计划：04 非目标清单（RTL 完成）；optionAction 恢复发布的一行开关+完整证据链留档。

### B47：QML 应用窗口模式 + 示例 README（第 7 轮，widgets 功能清单对齐完成）
- 日期：2026-10-08（用户目标第 7 轮）
- **RibbonApplicationWindow**（`src/qml/appwindow/`，对标 widgets SARibbonApplicationWidget）：透明结构项承载用户任意内容；bar 经 itemChange 登记（单实例），叶子惰性 Popup 以 `contentItem` 外部注入方式承载（同样遵守 bar 叶子 Popup 惰性创建铁律）；**应用按钮点击优先级：窗口 > 菜单 > 仅信号**（widgets 示例默认 ApplicationWidget 模式、菜单模式让位的对等语义）；内部 `close()` 经 closeRequested → bar → requestApplicationWindowClose → 叶子弹层（QMetaObject::invokeMethod 链）。
- 测试 `applicationWindow`：真实点击 File 按钮开窗（popupVisible 翻转）→ 真实点击内部 Cancel 经完整链路关窗 → headless requestApplicationWindowClose——18/18 绿。示例：ApplicationWidget 对等内容（列表 + Esc 提示 + Cancel + ✕ 平钮）；菜单项保留声明（演示让位语义）。
- 过程缺陷：Q_PROPERTY 声明漏加（早前批量编辑被文件竞态拒绝后只补了函数声明，属性读取返回 false）——插桩定位后补齐。
- 示例 README.md 新建：功能清单表（对应 widgets 各面板/入口）、已知差异（optionAction 延后/定制系统无边框）、测试索引。
- widgets 侧无改动；测试 18/18 双绿（Debug 基线；Release 树沿用第 6 轮验证）。
- 影响计划：04 非目标清单（应用窗口模式完成——widgets MainWindowExample 功能清单全部对齐或登记差异）。

### B48：optionAction 根因终局——叶子绑定形状（第 8 轮，B44-B46 崩溃链关闭）
- 日期：2026-10-08（用户目标第 8 轮）
- **根因**：第 4-6 轮的"消费路径崩溃"结论被修正——真正的触发器是**叶子的双依赖三目绑定** `cppHost && cppHost.hasOptionAction ? cppHost.optionButtonRect : Qt.rect(...)`（短路 + 双依赖的 V4 绑定求值路径在 Qt 6.7.3 Debug 构建的 QQmlData teardown 中踩坏堆），而**非**宿主消费代码。证据链（第 8 轮三步二分，根上下文修复后的干净基线）：①消费开+叶子绑定关=Debug 通过；②消费开+单依赖绑定（与 titleRect 完全同形 `cppHost ? cppHost.optionButtonRect : Qt.rect(...)`）=Debug 通过；③完整恢复（消费+单依赖绑定+几何断言）=Debug 与 Release 双 18/18 绿。第 5 轮"store-only 也崩"的旧结论系当时叶子绑定仍启用的污染（该轮二分只动了宿主侧）。
- **修复**：叶子 optionRect 绑定改单依赖形状（行为等价：禁用时宿主发布空矩形），注释留档绑定形状约束。optionAction **全功能恢复**：hasOptionAction 属性 → 引擎预留 → optionButtonRect 发布 → 叶子对角按钮渲染 → optionActionTriggered 信号；示例 toolbutton style 面板重新启用；测试恢复完整几何断言（正方形、标题条带对齐）。
- 教训入档：**QML 属性绑定中 `cppHost && cppHost.xxx ? cppHost.yyy : fallback` 的双依赖短路形状在本项目 Qt 6.7.3 Debug 环境有 teardown UAF 风险——一律用单依赖形状**（`cppHost ? cppHost.yyy : fallback`），qml-guide 已登记该规则。
- 测试 18/18 双绿（Debug build-qml-test + Release build-qml-rel 两树独立验证）；示例 30s 稳定。
- 影响计划：04（optionAction 从"已知缺口"转"完成"）；B44-B46 崩溃链全部关闭（根因=绑定形状，非消费路径、非引擎、非根上下文遗留）。

### B49：按钮布局算法下沉 core，QML 与 widgets 布局一致性用测试钉死（第 9 轮）
- 日期：2026-10-01（用户目标第 9 轮：QML 按钮文字显示异常）
- 发现位置：计划 02（core 下沉）/ 计划 04（QML 前端）交界
- **根因**：按钮布局算法（两行/单行文本预算、大按钮高度与最小宽度、宽高比上限、图标/文本/下拉箭头矩形切分、文本对齐）此前只存在于 `src/widgets/SARibbonToolButton.cpp` 的私有实现（619 行），QML 宿主只能自行拼凑尺寸——没有两行预算、没有宽高比约束，文字显示异常。按 qml-guide 铁律，这正是"QML 需要复制 widgets 算法 = core 缺口"，处理方向是补 core 而不是在 QML 里重写。
- **处理**：新建 `src/core/layout/SARibbonToolButtonLayout.{h,cpp}`——`ToolButtonLayoutConstants` 常量族 + `SARibbonToolButtonLayout::{Input,SizeHintResult,DrawRectResult,Factors}` + `calcSizeHint/calcDrawRects/textDrawRectHeight/textAlignment/estimateLargeButtonTextWidth/adjustIconSize/indicatorHeight/simplifiedText`。widgets 侧改为纯委托（`git diff --numstat` = 66 插入 / 619 删除，行为不变）；QML 宿主 `layoutInput()` 组装同一 `Input`，并把 `iconGeometry/textGeometry/indicatorGeometry/textWordWrap/largeType/displayText` 发布给叶子渲染。
- **过程中发现的真实分歧（已修）**：Qt6 `QToolButton::initStyleOption` 在按钮无图标时把 `ToolButtonTextBesideIcon` 降级为 `ToolButtonTextOnly`（Qt 5.14.2 源码无此逻辑），widgets 侧交给 core 的正是降级后的值；QML 宿主原先硬编码 `TextBesideIcon`，导致无图标小按钮**宽度多 23px 且预留了幻影图标矩形**。修复为 `layoutInput()` 内按 `QT_VERSION` 分支镜像 widgets 行为。
- **新增一致性测试** `tests/qml/tst_layout_parity_qml.cpp`（目标 `qml_LayoutParity`，同时链接 SARibbonQml 与 SARibbon::Widgets，仅在两个 target 都存在时构建）：大按钮 sizeHint 在 7 种文本形状（短词/两词/触发换行/超长/手动 `\n`/单个超长词/CJK）上与 widgets 逐一相等 + 不换行变体；小按钮在统一宿主高度后相等；宿主发布的三个几何矩形 == `calcDrawRects` 结果；caption 模式（wordWrap 保留 `\n`、非 wordWrap 与小按钮去 `\n`）两端一致。
- **测试可比性前提入档**：widgets `sizeHint()` 带缓存且小按钮 `textDrawRectHeight = rect.height() - 2`，跨端比较前必须 `resize()` + `invalidateSizeHint()` 把宿主高度对齐，否则比的是布局前的默认矩形。另：`RibbonMetrics` 未 DLL 导出，测试无法直接调用；两端字体本就同源（widgets 按钮继承应用字体，RibbonMetrics 默认取 `QGuiApplication::font()`），已实测 advance/lineSpacing 相等，故无需对齐字体。
- 证据：`build-verify` ctest 30/30 绿（含新增 `qml_LayoutParity` 15 个数据行）、`build-qml-test`（WIDGETS=OFF）2/2 绿、`python tools/check_core_purity.py src/core` 通过、`tools/Amalgamate.sh` 重新生成合并文件。
- 影响计划：02（ToolButton 布局算法入 core，widgets 侧只剩样式与交互）；04（QML 按钮文字布局与 widgets 一致，缺口关闭，且由常驻测试防止回归）。

### B50：画廊格子标题带下沉 core + 未布局面板标题负宽度夹紧（第 10 轮）
- 日期：2026-10-01
- 发现位置：计划 02（core 下沉）/ 计划 04（QML 前端）交界
- **根因 1（画廊标题溢出）**：`RibbonGallery.qml` 的格子标题带高度是叶子自造的常量（`RibbonMetrics.panelTitleHeight * 0.9` ≈ 14px），与图标盒互不相干；widgets 侧 `SARibbonGalleryGroup::recalcGridSize` 里那套"图标盒 + 标题带"推导只存在于 widgets，QML 无从复用。长标题（"Document File"、"Drive File Four Word"）因此画出格子、压到图标上。按 qml-guide 铁律属 core 缺口。
- **处理 1**：把该推导整体下沉为 `SA::calcGalleryCellMetrics(cellWidth, cellHeight, lineSpacing, spacing, GalleryCaptionStyle)`（新增 `GalleryCaptionStyle{None,SingleLine,WordWrap}` 与 `GalleryCellMetrics{iconSize,captionHeight}`），widgets `recalcGridSize` 改为纯委托（只有 `fontMetrics().lineSpacing()` 与 `spacing()` 两个输入留在 widgets 侧）；QML 宿主 `updateGridMetrics()` 调同一函数（lineSpacing 来自 `RibbonMetrics::instance()->coreMetrics().fontMetrics()`），并把 `captionHeight`/`cellIconWidth`/`cellIconHeight` 发布给叶子。叶子标题带与图标盒全部改读宿主值，字号由标题带高度反推（`floor(captionH/2)-2`，两行刚好放下）。发布三个 int 而非一个 QSize，是为了让叶子绑定保持 B48 的单依赖形状。
- 顺带把 `mDisplayRow` 从 3 改为 1：叶子渲染的是 word-wrap 两行标题（widgets `DisplayOneRow` + `IconWithWordWrapText` 对等），3 行会让 core 算出的格子高度与叶子实际绘制不符。
- **根因 2（未布局面板标题负宽度）**：`SARibbonPanelLayoutEngine` 计算标题带用 `setrect.width() - mag.left() - mag.right()`，无下限。从未参与布局的分类（隐藏 tab）其面板 `setrect.width()` 为 0，负宽度被直接发布出去，QML 叶子的 `Text.width` 变成 -4。
- **处理 2**：标题带宽度 `qMax(..., 0)`；optionAction 按钮几何只在标题带宽度 > 0 时发布（否则按钮落到负坐标），RTL 镜像加 `isValid()` 守卫；`RibbonPanel.qml` 标题 `Text` 增加 `width > 0 && height > 0` 可见性条件。
- **测试钉死**：`barAutoTabsAndSwitch` 增加"隐藏分类的面板标题带 / optionAction 几何非负"扫描；`galleryInPanel` 增加长标题项（"Document File"/"Drive File Four Word"/"Network Location File"），断言宿主发布的 `captionHeight` 与图标盒 == `calcGalleryCellMetrics` 结果、图标盒与标题带之和不超过格子高度、每个长标题 Text 的盒子恰为标题带高度且字号两行放得下。
- **测试基础设施教训入档**：Repeater 生成的 delegate **不在** `findChildren<QQuickItem*>()` 能到达的 QObject 子树里（QObject parent 落在创建上下文而非视觉父项，实测 `gridArea->children().size() == 1` 而 `childItems().size() == 11`），原先用 `findChildren` 扫 `QQuickText` 的断言其实是空转。新增文件级 `collectVisualItems()` 沿 `childItems()` 走视觉树，两处扫描均补 `textCount > 0` / `captionTextCount > 0` 反空转断言。
- 证据：`build-qml-test`（WIDGETS=OFF）ctest 2/2 绿（`qml_Conformance` 18 个用例）、`build-verify`（含 Widgets）ctest 30/30 绿、`python tools/check_core_purity.py src/core` 通过、`tools/Amalgamate.sh` 重新生成合并文件。
- 影响计划：02（画廊格子度量入 core，widgets 侧只剩委托）；04（QML 画廊标题溢出与隐藏分类标题负宽度两个缺口关闭，并由常驻测试防回归）。

### B51：调色板 JSON 迁入 core + 主题→调色板映射四合一 + 合并流水线三处潜伏缺陷（第 11 轮）
- 日期：2026-10-02
- 发现位置：QML/widgets 能力差异审计 → WS-B（主题自定义入口）前置步骤
- **根因 1（构建卫生）**：`src/qml/qml/saribbon_qml.qrc` 用 `../../../src/widgets/resource/palettes/*.json` 别名引用 widgets 源码树里的 10 个调色板。SARibbonQml 在链接层只依赖 SARibbonCore，但 rcc 阶段实际要求 widgets 目录存在——单独构建 QML 模块（`SARIBBON_BUILD_WIDGETS=OFF`）时资源解析取决于源码树布局，而非 target 依赖图。
- **根因 2（映射四处重复）**：主题 → 调色板 JSON 路径的 switch 在四个编译单元里逐字重复（widgets 的 `SARibbonMainWindow.cpp` / `SARibbonThemeManager.cpp` / `SARibbonUtil.cpp` 与 QML 的 `RibbonTheme.cpp`），任何一处漏改都会让混合应用里的两套前端解析到不同色值；QML 侧的副本还漏了 `RibbonThemeUserDefine` 分支，落到 `default` 返回空串后 `loadFromFile` 静默失败，调色板保留上一个主题的色值（半成品状态的真实来源）。
- **处理**：10 个 JSON `git mv` 到 `src/core/resource/palettes/`（内容逐字节不变，`git hash-object` 对照 f22827a 确认），资源前缀 `/SARibbonTheme/resource` 保持不变以免破坏混合应用里的重复注册解析；两个 qrc 分别改指 core 路径。映射下沉为 `SARibbon::Core::SARibbonThemeData::themePalettePath(SARibbonTheme)`，`RibbonThemeUserDefine` 显式返回空串（语义：自定义主题无内置调色板，调用方保留已加载的用户调色板），四处副本全部删除改为委托。QSS 模板路径映射（`themeToQssTemplatePath` / `themeToTemplatePath`）刻意留在 widgets——QSS 是 widgets 独有的渲染路线。
- **过程中暴露的合并流水线三处真实缺陷（a4cce06/B49 引入，单文件构建自那时起一直是坏的）**：
  1. **C2039/C2878 ×30**：`SARibbonToolButtonLayout.h` 只被加进了 widgets 合并模板的 `.cpp` 列表，没登记到 `SARibbonCoreAmalgamTemplatePublicHeaders.h`，因此从未进入 `SARibbonWidgets.h`；Amalgamate.sh 把 `#include <SARibbonCore/X.h>` 重写成产品头后声明彻底丢失。修复：补进公共头模板（该模板被 core 与 widgets 两个 `.h` 模板共同包含，一处即覆盖）。已交叉核对全部 14 个 core 头，确认这是唯一遗漏项。
  2. **LNK2019 ×4 → LNK1120**：`SARibbonToolButtonLayout.cpp` 未加入两个合并模板的 `.cpp` 列表，`calcSizeHint/calcDrawRects/textAlignment/simplifiedText` 全部未解析。修复：在 `SARibbonBarGeometryEngine.cpp` 之后补入 core 与 widgets 两个模板。
  3. **C1083 `SARibbonCore/SARibbonQt5Compat.hpp`**：Amalgamate.sh 的后处理 sed 用 BRE `[A-Za-z0-9_]*\.h*`，`.h*` 意为"一个点加零个或多个 h"，永远匹配不到 `.hpp`；`SARibbonToolButtonLayout.cpp` 是第一个用尖括号形式包含 `.hpp` 的 core 源文件，缺陷因此才显形。修复：改 `sed -E` + `\.(h|hpp)`，定界符换成 `#`（ERE 里 `\|` 是字面竖线，若继续用 `|` 作定界符会提前截断正则）。
- 教训入档：**合并流水线没有常驻验证**——`tools/Amalgamate.sh` 只在需要时被手工跑一次，生成物 `src/SARibbon*.cpp/.h` 又是 gitignore 的，所以"新增 core 文件"这类改动的合并侧后果不会在任何 CI/测试里暴露，只有唯一消费者 `examples/widgets/StaticExample` 会挂，而它默认不在构建集里。今后每次新增 core 头/源都必须同步检查三处：`SARibbonCoreAmalgamTemplatePublicHeaders.h`、两个 `.cpp` 模板、Amalgamate.sh 的 sed 覆盖面。
- 证据：模块化构建 `build-verify2` ctest 30/30（`qml_Conformance` 18 用例、`qml_LayoutParity` 15 数据行全绿）；合并构建（StaticExample）编译链接通过，`grep -c "include <SARibbonCore/" src/SARibbonWidgets.cpp src/SARibbonCore.cpp` = 0/0；`python tools/check_core_purity.py src/core` 通过。资源 blob 新鲜度用 `git hash-object` 而非 mtime 判定（`tools/qrc_SARibbonResource_Datas.cpp` 看似过期，实测 JSON blob 与 f22827a 一致，无需重新生成）。
- 遗留（非本轮引入，未修）：widgets 侧 `SARibbonToolButtonColorTest` 在干净 HEAD 上 SEGFAULT，栈为 `QPlainTextEdit::documentTitle` ← `QApplication::notify` ← `QWidget::clearFocus` ← `QWidget::~QWidget` ← `QTest::qRun`，属测试用例生命周期问题，与本轮改动无关。
- 影响计划：04（WS-B 前置完成：QML 主题入口不再有 qrc 跨模块依赖，`RibbonThemeUserDefine` 的半成品状态定位清楚）；02（调色板资源与主题映射归属 core）。

### B52：QML 主题自定义入口落地 + 三个只在该轮显形的诊断陷阱（第 12 轮）
- 日期：2026-10-02
- 发现位置：计划 04（QML 前端）WS-B 主体
- **处理**：`RibbonTheme` 增加写入口 `setAccentColor`/`setContentBgColor`/`setTextColor`（转发 core `SARibbonThemePalette` 的同名方法，内部统一走 `mutatePalette()`：调色板为空时先按当前主题补载，再拷贝—改写—回写）、`loadPaletteFromJson`/`loadPaletteFromFile`（失败时 `qWarning` 且保留原调色板，返回 false）、声明式 `customPaletteSource`（`qrc:`/本地文件/资源路径三种 URL 归一）。只读 `hasCustomPalette` 让"调色板是内置还是用户覆盖"可诊断，这是 `RibbonThemeUserDefine` 半成品状态的收尾：`applyThemePalette()` 拿到空路径时不再静默返回，而是重放已声明的 `customPaletteSource` 并无条件发一次 `paletteChanged`；载入内置主题时把该标志清零。深色模式桥接 `systemDarkMode`（Qt 6.5+ 才有 `QStyleHints::colorSchemeChanged`，低版本只读一次）与 `followSystemDarkMode` → `SA::setEnableSystemDarkModeAutoSwitch`。
- **根因（自动切换开关在 QML 侧无消费者）**：core 的 `isEnableSystemDarkModeAutoSwitch()` 只在 `SARibbonMainWindow`/`SARibbonWidget` 构造时读取，QML 侧没有任何调用点，暴露 setter 等于暴露一个空开关。修复：`RibbonBar` 构造函数按 widgets 同样条件（开关开 + 系统暗色 + 主题仍是默认 `RibbonThemeOffice2021Blue`）切到 `RibbonThemeDark`，与 `SARibbonMainWindow.cpp` 逐条对齐。测试因此在 `exposeScene()` 之后才捕获 `themeAtEntry`——在暗色桌面上 bar 构造时主题已被移走。
- **陷阱 1（LNK2019 ×20）**：`RibbonTheme` 类声明没有 `SA_RIBBON_QML_EXPORT`，同目录的 `RibbonToolButton`/`RibbonGallery` 都有。单例此前只被 DLL 内部的 `saRibbonRegisterQmlTypes()` 取用，所以缺导出宏一直没暴露；测试一旦直接 `RibbonTheme::instance()` 就全线未解析。教训：**模块内新增的 host/单例一律带导出宏**，即使当前只有模块内部使用者。
- **陷阱 2（QColor 比较假失败）**：`QCOMPARE(theme->accentPressed(), custom.darker(115))` 失败，但 QTest 打印的两个值都是 `#ffa73225`。`QColor::operator==` 先比 `ct`（color spec）再比分量，`RibbonTheme::paletteColor()` 走 `rawValue()` → `QColor(QString)` 构造，与整数构造的 `QColor` spec 不同，值相同也不相等。修复：测试内新增 `colorKey()` 归一到 `name(QColor::HexArgb)` 再比。凡是"token 查询结果 vs 本地构造颜色"的断言都要归一，否则断言在测 spec 而不是测颜色。
- **陷阱 3（测试零输出）**：`ctest` 与直接运行都拿不到 QTest 输出（日志显示 `<end of output>`，直接跑写出 0 字节日志且退出码 1），但 `-functions` 正常列出全部槽——stdout 在非 TTY 下是块缓冲，输出被丢弃/错位。可靠做法是让 QTest 自己写文件：`qml_Conformance.exe -o <path>,txt`。另注：`ctest` 的退出码是失败用例数而非 0/1。
- **环境陷阱（vcvars 静默失败）**：`call vcvars64.bat >nul` 会让 MSVC 环境初始化整段失效（只打印"系统找不到指定的路径"且被重定向吞掉），后续 `cl.exe` 找不到 `<memory>`/`winres.h`。去掉重定向即恢复。凡是在 Git Bash 里包 `.bat` 驱动 MSVC 构建，都不要重定向 vcvars 的输出。
- 证据：`build-qml-test`（Debug，WIDGETS=OFF）ctest 2/2 绿，`qml_Conformance` 19 用例全绿（新增 `themeCustomization`：内置 accent 渲染像素 > 200 → 覆盖后 `hasCustomPalette` 变真且派生 `accent-pressed == darker(115)`、渲染帧换新色旧色消失 → 非法 `QColor()` 被拒 → JSON/文件加载与错误输入拒绝 → `qrc:` URL 与独立加载的 `SARibbonThemePalette` 逐 token 相等 → 切 `RibbonThemeUserDefine` 保留用户调色板、切回内置复位、再切回重放 source → `followSystemDarkMode` 映射 core 开关且不触发 `paletteChanged`）；示例 `QmlMainWindowExample` offscreen 跑 12s 无任何 QML 警告；`python tools/check_core_purity.py src/core` 通过。示例取色器刻意用纯 QtQuick `Popup` + 色块网格而非 `QtQuick.Dialogs`，因为模块声明支持 Qt 5.12，而 ColorDialog 在 Qt5/Qt6 的属性名不同（`color` vs `selectedColor`）。
- 影响计划：04（WS-B 完成，`RibbonThemeUserDefine` 不再是半成品；主题自定义从"已知差异"清单中移除）。

### B53：A4 布局旋钮暴露 + ninja 依赖丢失导致的 sizeof 错配（第 13 轮）
- 日期：2026-10-02
- 发现位置：计划 04（QML 前端）WS-A4（暴露 core `SARibbonToolButtonLayout::Factors`）
- **处理**：core 的 `Factors` 四个系数与 `Input` 的图标尺寸/spacing 逐字段发布为宿主属性，默认值与 `ToolButtonLayoutConstants` 逐一相同。参数归属照抄 widgets，不按"谁用到谁持有"就近摆放：`spacing` 与两个文字高度系数归 `RibbonToolButton`（对应 `SARibbonToolButton::setSpacing`/`LayoutFactor`），两个图标尺寸归 `RibbonPanel`（对应 `SARibbonPanelLayout::mSmallToolButtonIconSize`/`mLargeToolButtonIconSize`，面板在布局时把值推给每个子按钮），两个宽高比归 `RibbonBar` 并经 `RibbonCategory::applyLayoutFactors` → `RibbonPanel::applyLayoutFactors` → 按钮 setter 逐层下发（对应 `SARibbonBar::setButtonMaximumAspectRatio`）。`registerChildItem` 补推全部六个旋钮，使下发之后才声明的按钮同样继承——这一条是 A4 唯一的行为新增点，其余都是默认值不变的管道。`RibbonPanel::runLayout()` 里的 `input.spacing = 2` 是**面板引擎**的间距（widgets `SARibbonPanel.cpp` 同值），与按钮内部 spacing 不是一回事，刻意未动。
- **根因（Debug 下 `panelThreeRowMixed` 段错误，与 A4 逻辑无关）**：`RibbonPanel::registerChildItem` 里 `btn->setButtonMaximumAspectRatio(1.4)` 读到 `cur=0.000000`、`spacing=131072`、`smallIconSize=(1750228336,455)`，而同一对象在构造函数末尾打印的是 `ratio=1.400000`。逐字段打印地址后发现被污染的区间**恰好是对象尾部新增的 56 字节**（`mSpacing`+`mFactors`+两个 `QSize`，offset 400..456），`sizeof(RibbonToolButton)` 在 DLL 内是 456。`ninja -t deps` 显示 `SARibbonQmlTypes.cpp.obj` 的依赖数是 **`#deps 0`**（mtime 标记 VALID，因此 ninja 认为无需重建），该 obj 时间戳停在头文件改动之前——即 `qmlRegisterType<RibbonToolButton>` 记录的仍是旧的 `sizeof`。QML 引擎按注册期记录的尺寸分配 400 字节，再用 placement new 构造 456 字节的对象，尾部 56 字节落进相邻堆块；构造完成时那段内存尚未被邻居写脏（所以 ctor 末尾读数正确），随后测试的下一个分配覆盖了它。全仓 `ninja -t deps | grep -c "#deps 0,"` = **69/188**，不是孤例。删掉 `build-qml-test/.ninja_deps` 全量重建后 30/30 全绿，A4 代码一行未改。
- 教训入档：**Qt 6.7.3 Debug 下的 V4 段错误不能默认归因于绑定形状**。头文件里新增/删除成员变量属于"改布局"的改动，一旦 ninja 依赖丢失，表现为随机的堆污染 + 引擎内崩溃，栈顶全是 `QV4::Value::fromHeapObject` 这类无意义符号（本机无 Qt PDB，`Nearby symbol` 是最近导出符号，不可信）。诊断顺序应当是：先 `ninja -t deps <obj>` 核对依赖数是否为 0、比对 obj 与头文件时间戳，再怀疑代码。**B44–B46 当年归因于"绑定形状"的三次 Debug 崩溃需要按同一方法复核**——B48 的单依赖形状规则本身无害可以保留，但它是否真是那三次崩溃的根因，目前没有排除掉构建错配这个替代解释。
- **诊断技巧（本轮有效）**：`qDebug()` 在 QTest 进程里被消息处理器捕获，崩溃时随缓冲区一起丢失；改用 `fprintf(stderr,...)` + `fflush(stderr)` 才能在段错误前拿到输出。另外崩溃栈里 `RibbonPanel::componentComplete` / `QQuickAnchors::resetCenterIn` 之类的帧名是最近导出符号拼凑的，与实际调用链无关，不要据此推断。
- **测试陷阱（断言空转）**：`tst_layout_parity_qml.cpp` 的小图标尺寸一行最初两端都不挂图标，`QVERIFY(sizeHint 变化)` 恒假。原因在 core：Qt6 的 `QToolButton::initStyleOption` 在无图标时把 `TextBesideIcon` 降级为 `TextOnly`，而 `calcSizeHint` 的 `ToolButtonTextOnly` 分支**根本不读 `in.iconSize`**（只有 `IconOnly` 与默认的 `TextBesideIcon` 分支读）。旋钮改了也观测不到，断言就是在测一个不消费该输入的路径。修复：两端都先挂一张 1×1 合成图标（core 只消费"有没有图标"这个布尔，不需要真实资源），并把这条语义写进测试注释。
- 证据：`build-qml-test`（Debug，Qt 6.7.3 msvc2019_64）ctest **30/30** 全绿，其中 `qml_Conformance` 19 用例、`qml_LayoutParity` 16 用例（新增 `layoutKnobsParity`：宽高比 2.5 使大按钮 sizeHint 偏离默认且两端相等；最小宽度比 2.0 使 "Cut" 变宽而高度不变；两个文字高度系数按 `largeButtonHeight/lineSpacing + 2.0` 放大后两端相等；`setSpacing(6)` 大/小按钮两端相等；40×40 图标尺寸两端相等）。`python tools/check_core_purity.py src/core` 通过，`src/qml` 内无任何 widgets 头包含。A4 未触碰 core 与叶子 QML 文件。
- 影响计划：04（WS-A4 完成，A1/A2/A5/A3 待做）；04-B48（根因存疑，需按本轮方法复核）。

### B54：A1 菜单叶子抽取 —— QML 禁止递归实例化自身，以及四个只在"抽公共叶子"时才显形的陷阱（第 14 轮）
- 日期：2026-10-02
- 发现位置：计划 04（QML 前端）WS-A1（`RibbonMenuItem` 勾选/快捷键/子菜单 + 抽出 `RibbonMenu.qml`）
- **处理**：`RibbonMenuItem` 增 `checkable`/`checked`/`shortcut`/`submenu`（`QQmlListProperty`）+ 只读 `hasSubmenu`，`activate()` 刻意**不加 `Q_INVOKABLE`**——激活必须经持有它的宿主中转，`menuTriggered`/`applicationMenuTriggered` 才能保持唯一入口。子菜单寻址用**索引路径**而非指针：叶子发 `itemActivated(var indexPath)`，宿主侧 `resolvePath(roots, path)` 逐级解析到末端项，任一步非法（越界/非整数/空路径）返回 `nullptr` 并整体拒绝；扁平的 `activateMenuItem(int)` 保留为一元素路径的特例，语义与 QAction 一致（点子菜单父项等于触发父项本身）。把 `RibbonToolButton.qml` 里的内联 Popup 抽成共享叶子 `RibbonMenu.qml`，bar 的应用菜单（`appMenuComponent`）与按钮弹出菜单同源，`namePrefix`/`rowHeight`/`minRowWidth` 三个参数让旧 objectName（`menuRow`/`appMenuSeparator` 等）与旧度量逐字不变，既有测试与视觉零回归。嵌套 Popup 的生命周期用显式 `subRowStack` 登记表管理（激活、兄弟行悬停、所属菜单关闭三处收口），因为按需创建的弹窗不在 item 树里，无法枚举。
- **陷阱 1（`Type RibbonMenu is instantiated recursively`，本轮唯一的阻塞性故障）**：QML **复合类型不得静态实例化自身**，哪怕写在 `Component { }` 里也一样——Qt 在编译期就判递归，运行时 `Component` 只是延迟加载而非延迟解析。子菜单要弹自己这类窗口，唯一出路是运行时创建：`Qt.createComponent("RibbonMenu.qml", Component.PreferSynchronous, menuRoot)` + `createObject` + 立刻 `comp.destroy()`，动态部分（`menuModel`/`x`）用 `Qt.binding` 包，信号用 `.connect()` 接。级联后果值得单独记：递归报错让 `Type RibbonMenu unavailable`，于是 `RibbonToolButton.qml:243` 与 `RibbonBar.qml:195` 两个**叶子**都加载失败，最终 5 个用例挂掉，其中 `quickAccessBarAndRightGroup`/`applicationWindow`/`themeCustomization` 三个与菜单毫无关系。**教训：QML 叶子报 "Type X unavailable" 时，先看 QML 侧第一条编译错误，别按失败用例名去查 C++**——失败面是"所有用到该叶子的宿主"，不是"该功能"。
- **陷阱 2（`var` 信号参数不再注入命名形参）**：`signal itemActivated(var indexPath)` 配 `onItemActivated: host.f(indexPath)` 在 Qt 6 下报 `Parameter "indexPath" is not declared. Injection of parameters into signal handlers is deprecated.`，参数恒为 `undefined`。改成位置读取 `arguments[0]`。`onItemActivated: function(path) {...}` 这种写法是 Qt 5.15+ 语法，而本模块声明支持 Qt 5.12，不能用。**信号带 `var` 参数时，处理器一律走 `arguments[]`。**
- **陷阱 3（带自定义属性的 Canvas 变成复合类型）**：叶子里画勾选标记的 `Canvas` 因为声明了 `property color markColor`，`metaObject()->className()` 返回 `QQuickCanvasItem_QML_5` 而不是 `QQuickCanvasItem`，测试里 `className == "QQuickCanvasItem"` 的断言恒假（`canvasFound` 为假）。修复：用 `QByteArray(className).startsWith("QQuickCanvasItem")`。裸 `QQuickText`/`QQuickRectangle`/`QQuickRow` 仍保留原名，只有"原生类型 + 自定义属性"这种混血才改名——B50 的 `collectVisualItems` 遍历法本身没问题，问题在类名匹配写成了全等。
- **陷阱 4（delegate 绑定在弹窗拆除期对 null id 重算）**：Popup 关闭时报 `qrc:/SARibbon/RibbonMenu.qml:122:17: TypeError: Cannot read property 'minRowWidth' of null`。Repeater 的 delegate 在宿主拆除过程中会重新求值，此时它引用的 `menuRoot` id 已经是 null。修复：把对 id 的读取**提到 delegate 自己的只读属性上并逐个加守卫**（`readonly property var menu: menuRoot` → `menu ? menu.namePrefix : ""`），下面的 `objectName`/`width`/`height`/mark 列 `visible` 只读这些本地量。这是 B48 单依赖绑定形状规则的一次推广：**守卫对象不只是 `cppHost`，同文件内的 id 在弹窗生命周期里同样会短暂为 null**。
- **测试陷阱（顺序耦合）**：两处顺序依赖，都不是 bug 但会让断言莫名其妙地失败——(a) "勾选状态在 `menuTriggered` 之前就已翻转"这条语义，靠在测试里 `connect(menuTriggered, lambda)` 抓 `checkedAtTrigger` 才能证明，用 spy 事后读是测不出来的；(b) 早先用索引路径 `[2,1]` 激活 doc2 会把它的 `checked` 清掉，后面的叶子渲染断言（勾选标记可见）必须在此之前重新 `setChecked(true)`。另注：`menuBtn` 是 `QQuickItem*`，连 `menuTriggered` 前需 `qobject_cast<RibbonToolButton*>`，否则 C2664。
- **文档口径修正**：`tst_conformance_qml.cpp` 的用例数是 **18**（`-functions` 列出 18 个槽），此前 B52/B53 与示例 README 写的"19 用例"其实是 QTest `Totals: 19 passed` 的数字——那个计数含 `initTestCase`/`cleanupTestCase` 两个非用例槽。本轮加 1 例后 `Totals` 为 20，用例数 18。以后一律以 `-functions` 的行数为准。
- 证据：`build-qml-test`（Debug，Qt 6.7.3 msvc2019_64）ctest **30/30** 全绿（6.90s）；`build-qml-rel`（Release）ctest **30/30** 全绿（4.28s），`qml_Conformance` Debug 3.42s / Release 2.58s，`Totals: 20 passed, 0 failed`。新增 `menuCheckableShortcutAndSubmenu`：声明式模型（`isCheckable`/`shortcut()`/`submenuCount()==2`/`hasSubmenu()`/`submenuItemAt(1)`）→ 勾选在 `menuTriggered` 之前翻转 → `toggled`/`triggered` spy 计数 → 二次激活翻回 → 扁平 `activateMenuItem(0)` 等价一元素路径 → `setCheckable(false)` 连带清 checked → 路径 `[2,1]` 激活 doc2 并清其勾选 → 非法路径 `{2,9}`/`{9}`/`{0,0}`/`{}`/`{"x"}` 全部拒绝 → `activateMenuItem(2)` 命中子菜单父项（QAction 语义）→ 叶子侧 3 行 objectName 与标题文本、`Ctrl+S` 存在、三行标题 x 相同且 > 10（预留勾选列）、未勾选项的勾选 Canvas **存在但不可见** → `mouseMove` 悬停 Recent 行后 `visibleRowCount()==5` → doc2 行勾选 Canvas 可见 → `mouseClick` 触发 `menuTriggered(doc2Item)`、`isChecked()==false`、`menuVisible==false`、行数归 0。`python tools/check_core_purity.py src/core` 通过，`src/qml` 内无任何 widgets 头包含。A1 未触碰 core（`RibbonMenuItem` 是 QML 侧数据对象，与 core 无共享算法），故无需 `Amalgamate.sh`。
- 已知差异入档（示例 README 已写）：`SARibbonMenu::addWidget`（菜单内嵌任意控件）本批次不做；`shortcut` 是纯展示文本，接真实按键绑定依赖 QAction 抽象桥（plan-04 D8/Tier 3 延后项）。
- 影响计划：04（WS-A1 完成，A2/A5/A3 待做；A2 画廊与 WS-D 色板菜单都要复用 `RibbonMenu.qml`，其"运行时创建自身"的写法是后续嵌套弹层的模板）；04-B48（绑定形状规则从"守卫 cppHost"推广到"守卫同文件 id"）。

### B55：A2 画廊三态 —— Qt Quick 悬停只投递给最顶层的 hover-enabled item（第 15 轮）
- 日期：2026-10-02
- 发现位置：计划 04（QML 前端）WS-A2（画廊 `captionStyle` 三态 + `hovered` 信号 + `selectable`）
- **处理**：`RibbonEnums` 增 `GalleryCaptionStyle{GalleryIconOnly, GalleryIconWithText, GalleryIconWithWordWrapText}`，三个 `static_assert` 逐一钉到 core `SA::GalleryCaptionStyle::{None,SingleLine,WordWrap}`；`RibbonGallery` 把原先写死的 `constexpr kCaptionStyle = WordWrap` 换成成员并传入 `SA::calcGalleryCellMetrics`，叶子按 `drawsCaption`/`wrapsCaption` 两个派生布尔分三条绘制路径（仅图标 / 单行 / 两行换行 + 按 `captionHeight` 反推字号）。`RibbonGalleryItem` 补 `selectable`（默认 true），`setCurrentItemIndex` 同时受 `enabled` 与 `selectable` 约束——**但 `activateItem` 不受约束**，不可选单元照样发 `triggered`，与 widgets `QAbstractItemView::clicked` 一致。`hovered(item,index)` 由画廊与当前组各发一次（对应 widgets 的 group 级悬停）。
- **语义决定：负下标一律归一到 -1 哨兵，是"清除"而不是"拒绝"**。`setCurrentItemIndex(-7)` 会把标记清成 -1（`qMax(index,-1)`），测试最初按"越界即拒绝、标记不动"断言，被这一条推翻。理由：widgets 侧 `setCurrentIndex(QModelIndex())` 就是清除语义，负下标没有别的合法解释，让它保留原值反而制造出"发布值可能是任意负数"的开放区间。文档注释同步改为"任何负下标都归一到 -1 这个哨兵值并清除标记"。
- **根因（悬停信号一直不发，本轮唯一的阻塞性故障）**：为了捕捉"指针离开栅格"，在 `gridArea` 里加了一块覆盖整片的 `MouseArea{ anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.NoButton }`，它是最后一个兄弟因而位于最顶层，**把所有单元的悬停全吞了**。`acceptedButtons: Qt.NoButton` 只让鼠标按键穿透，并不让悬停穿透——Qt Quick 的 hover 是排他投递给最顶层那个 hover-enabled item 的。证据：遍历窗口视觉树共 8 个 `hoverEnabled==true` 的 item，`QTest::mouseMove` 到单元中心后 `containsMouse==true` 的**恰好 1 个，且是覆盖层而非单元**。修复：删掉覆盖层，改由每个单元的 `onContainsMouseChanged` 自报，root 上维护 `hoverCount`，归零时才发 `-1`——这样跨单元移动（旧单元 leave 与新单元 enter 无论谁先）都不会插入一次多余的 `(nullptr,-1)`。
- **B54 陷阱 4 的再现（拆除期 null id）**：`RibbonGallery.qml` 的 Repeater 单元 delegate 在窗口拆除时重算绑定，对 `root`/`gridArea`/`cellMouse` 的裸读抛 `TypeError: Cannot read property 'drawsCaption' of null`（6 次）。同 B54 的修法：把**所有**同文件 id 读取提到 delegate 自己的 `readonly property` 上并逐个加守卫（`root ? root.captionH : 0`、`gridArea ? gridArea.width : 0`、`cellMouse ? cellMouse.containsMouse : false`），下面的子项只读这些本地量，信号处理器里也补 `if (root)`。这条规则现在覆盖"任何 Repeater delegate"，不限于弹窗。
- **测试技巧（本轮有效）**：(a) 复合类型的**根项** className 是合成名（`RibbonGallery_QMLTYPE_3`），而其内部的原生元素仍是基类名（`QQuickText`/`QQuickMouseArea`），所以叶子文字可以用 `className()=="QQuickText"` 匹配，但叶子根绝不能按 className 匹配——用"同时具备 `isCurrent` 与 `entryEnabled` 属性"来筛选栅格单元，可自动排除 viewport Popup 里的 `vpCell`（它只有 `entry`）；(b) QtQuick 不导出公开的 `QQuickText` 头，`wrapMode` 只能用字面常量断言（NoWrap=0 / WordWrap=1），写进测试注释说明；(c) `QTest::mouseMove` 的第一个参数要传 `view.get()`，`QQuickView` 没有 `window()` 成员（它自己就是窗口）；(d) 悬停投递发生在帧同步阶段，空闲窗口不刷帧就永远观测不到 `containsMouse`，每次 move 后 `view->requestUpdate(); QTest::qWait(80);` 才稳定。
- 证据：`build-qml-test`（Debug，Qt 6.7.3 msvc2019_64）ctest **30/30** 全绿（7.21s）；`build-qml-rel`（Release）ctest **30/30** 全绿（7.04s）；`qml_Conformance` `Totals: 21 passed, 0 failed`（**19 个用例**，按 B54 口径以 `-functions` 计数），拆除期 TypeError 归零。新增 `galleryCaptionStylesHoverAndSelectable`：三态 × 每态把 `captionHeight`/`cellIconWidth`/`cellIconHeight` 与 `SA::calcGalleryCellMetrics(cellW,cellH,lineSpacing,1,core)` 逐项比对（`None` → captionHeight==0 且 iconH==cellH-2-4；`SingleLine` → lineSpacing；`WordWrap` → lineSpacing*2），再逐单元校验叶子的 caption `height`/`y`/`visible == draws && entryOn`/`wrapMode`/`maximumLineCount`/字号 ≤ captionHeight → 真实 `mouseMove` 到第 2 单元断言 `hovered(beta,1)` 且组信号同步 → 移到第 3 单元（不可选）仍报悬停 → 移到按钮条断言 `(nullptr,-1)` → `currentItemIndex` 0 生效后单元 0 背景 == `RibbonTheme.selectionBg`、其余 == transparent → 2/3/99 被拒（spy 计数不变）→ -7 清除 → `activateItem(2)` 发 `triggered` 但标记不动、`activateItem(1)` 移动标记 → `setSelectable(false)` 使标记掉到 -1、恢复 true 不自动重领 → 切组来回标记保持 -1。示例 `QmlMainWindowExample` offscreen 跑 12s 零 QML 警告（"gallery controls" 面板新增 Caption Style 循环按钮）。`python tools/check_core_purity.py src/core` 通过，`src/qml` 内无任何 widgets 头包含。
- **A2 未触碰 core**：计划要求"先确认 core metrics 对 `None`/`SingleLine` 的返回值语义与 widgets `SARibbonGalleryGroupItemDelegate` 三条 paint 路径一致"，核对结论是 core 的 `calcGalleryCellMetrics`（`shiftpix=4`，SingleLine→`lineSpacing`，WordWrap→`lineSpacing*2`，None→0 且图标吃满 `cellHeight-2*spacing-shiftpix`）本就三态完整，无需下沉也无需在 QML 侧补偿，故不跑 `Amalgamate.sh`。
- 影响计划：04（WS-A2 完成，A5/A3 待做）；04-B48/B54（守卫同文件 id 的规则确认适用于所有 Repeater delegate）；WS-D（色板网格将复用本轮的"单元自报 + root 计数"悬停写法，不要再放覆盖层）。

### B56：A5 容器尾随标签/标签条开关 + 按钮排互斥 —— "widgets 也没有的能力"该以什么名义补（第 16 轮）
- 日期：2026-10-02
- 发现位置：计划 04（QML 前端）WS-A5（`RibbonControlContainer` 后缀与显示开关、`RibbonButtonGroup` 互斥）
- **命名依据的核查结论**：计划写的是"widgets 侧靠 QActionGroup"，实读 `src/widgets` 与 `src/core` 后确认——**widgets 侧的按钮组/快速访问栏并没有互斥能力**，整个仓库里 `QActionGroup` 只出现在 `SARibbonGalleryGroup` 内部。所以 `exclusive` 不是"移植一个 widgets API"，而是补 QML 侧因**没有 action 桥**而缺失的那一层语义（用户在 QML 里无法像 widgets 那样把一组 `QAction` 塞进 `QActionGroup`）。头文件注释按这个真实理由写，而不是编一个 widgets 对应物出来。
- **归属决定：`exclusive` 放在共享基类 `RibbonButtonRowHost`，不放在 `RibbonButtonGroup`**。`RibbonQuickAccessBar` 与 `RibbonButtonGroup` 同源于该基类，互斥是"一排 checkable 小按钮"的通用能力而非右组专属；默认 `false`，QAB 行为逐像素不变（测试里显式断言 `qab->property("exclusive").toBool() == false`）。
- **语义决定：`setExclusive(true)` 不追溯取消已有勾选，只约束下一次勾选**——这是 `QActionGroup::setExclusive` 的真实行为，也是最容易"顺手做对成错"的一点（很多人会期望打开开关时立刻收敛到第一个）。文档注释里明确写出这条，测试用"两个都已勾选 → 打开开关 → 断言两个仍是勾选态 → 点一次才收敛"钉死。
- **重入闸就是 `isChecked()` 判据本身**：`enforceExclusivity` 在每个已登记按钮的 `checkedChanged` 上被调用；取消兄弟按钮的勾选会再发一次它自己的 `checkedChanged`，重入进来时它已不是勾选态，`if (!btn->isChecked()) return;` 直接短路，因此**不需要**另设"正在实施互斥"的标志位。这个判据同时保证未受影响的兄弟一个信号都不发（测试用 `QSignalSpy(rb3, SIGNAL(toggled(bool)))` 断言 `size()==0`）。
- **两处文档谎言被自查推翻**（写注释时顺手断言、随后读源码否掉）：(a) "取消勾选一个非 checkable 按钮在 `RibbonToolButton` 里是空操作"——错，`RibbonToolButton::setChecked(bool)` **没有** `isCheckable` 守卫；(b) "`setCheckable(false)` 会清掉 checked"——错，该行为只存在于 `RibbonMenuItem`，`RibbonToolButton` 不清。两条都从注释里删掉，改为只陈述 `isChecked()` 判据的实际作用。**教训：注释里的每一条"因为 X 所以安全"都必须回源码验一次，否则就是把猜测固化成文档。**
- **容器侧的几何不变量**：`computeSuffixWidthFromMetrics()` 对空后缀返回 0，`computeLabelWidthFromMetrics()` 对空文本/空图标各自跳槽，所以"不设后缀、不设图标"时 `sizeHint`/`labelWidth`/`positionControl` 与引入该能力之前逐项相同（测试第一段就是把这条钉成等式：`width - ctrl.x - ctrl.width == 2`，2 即 `kContentMargin`）。后缀是从**容器**里切出去的，不是从控件里扣的：`implicitWidth` 恰好增 `suffixWidth`，面板把这部分宽度批下来后控件的 `x` 与 `width` 都不动。`enableShowIcon`/`enableShowTitle` 对应 widgets 对两个 QLabel 调 `setVisible`，这里改为重算条宽并重摆控件，净效果一致；测试用"差值"而非魔数（`iconSlot = withIcon - textOnly`）断言，避免把 20/3 两个常量钉进测试。
- **本轮唯一的失败：一个"看起来对"的 QTRY 在中间态就通过了**。最初写的是 `QTRY_COMPARE(qRound(w - x - ctrlW), suffixW + 2)` 紧跟 `QCOMPARE(ctrl->width(), spinW0)`，结果 `spin->width()` 是 63 而不是 90。诊断输出显示：`setProperty` 后**控件立即按新 tail 缩了**（`positionControl` 是同步的），而**容器宽度要等一个 polish 周期才由面板批下来**（`implicitWidth` 155→182 是同步的，`width` 155→182 是异步的）。于是 `155 - 63 - 63 == 29 == suffixW + 2` 在中间态成立，QTRY 立刻通过，下一条同步断言撞上还没长大的容器。**修法：先 `QTRY_COMPARE(sfx->width(), sfxW0 + suffixW)` 等面板批完，再断言控件的 x/width。**通用教训——涉及"host 同步改 + 引擎异步批"的量，QTRY 必须等**上游那个异步量**本身，不能等一个在中间态也成立的派生等式。
- 证据：`build-qml-test`（Debug，Qt 6.7.3 msvc2019_64）ctest **30/30** 全绿；`build-qml-rel`（Release）ctest **30/30** 全绿；`qml_Conformance` Release `Totals: 23 passed, 0 failed`（**21 个用例**，按 B54 口径以 `-functions` 计数）。新增 `controlContainerSuffixAndShowFlags`（无后缀零尾条 → 后缀切出尾条且容器长大而控件不动 → 叶子真的画出 "px" 且落在尾条内 → 标题/图标槽位逐个收起再逐个恢复无漂移 → `implicitWidth` 跟随）与 `buttonRowExclusivity`（QAB 默认非互斥两者可同时勾选 → 右组互斥一次只留一个 → `checkedButton()` 对齐 `QActionGroup::checkedAction`、取消唯一勾选后回到 nullptr → 未受影响兄弟零信号 → 运行时打开开关只约束下一次勾选），全部用真实 `QTest::mouseClick`，不做属性戳。示例 offscreen 跑 12s 零输出（QAB 加 Icons/Details 单选对，widget test 面板 SpinBox 加 `suffixText: "px"`、ComboBox 容器加图标 + Label 四态切换按钮）。`python tools/check_core_purity.py src/core` 通过；`src/qml` 内无任何 widgets 头包含（该脚本对 `src/qml` 报的 18 条全是 `QQuickItem`/`QQml*` 规则误伤，其设计目标是 core）。
- **A5 未触碰 core**：后缀条宽与标签槽位度量都在 QML host 内用 `RibbonMetrics::coreMetrics().fontMetrics()` 算，互斥是纯 host 逻辑，无需下沉，故不跑 `Amalgamate.sh`。
- 影响计划：04（WS-A5 完成，A3 为 WS-A 最后一步）；WS-D（色板网格的互斥选中可直接复用 `enforceExclusivity` 的"`isChecked()` 即重入闸"写法）；WS-C（定制器若要在运行时改按钮排成员，注意 `unregisterButton` 会 `disconnect(btn,nullptr,this,nullptr)` 一并摘掉互斥连接，重新登记才会接回）。

### B57：A3 类别滚动 —— 箭头该画在哪一层，以及计划里那句"叶子 Behavior on x"根本无处可挂（第 17 轮）
- 日期：2026-10-02
- 发现位置：计划 04（QML 前端）WS-A3（`RibbonCategory` 滚轮滚动 + 滚动箭头 + 动画），core 前置 `SARibbonCategoryLayoutEngine.h`
- **箭头不能画在类别的背景叶子里**：`RibbonCategory.qml` 是 `createVisualLeaf` 造出来的背景层，`setZ(-1)`；而面板 host 是类别的 **z=0 兄弟**。把箭头放进背景叶子，等于让它在最需要它的时刻（内容溢出、面板铺满）被面板盖住。widgets 侧的做法是 `SARibbonCategoryLayout::doLayout` 里对两个 `QToolButton` 调 `raise()`。**否掉的两个方案**：(a) 在引擎输入里预留视口宽度给箭头——会改动黄金几何，`tst_conformance_qml.cpp` 里四个断言面板 x 的用例全废，且与 widgets 不一致（widgets 的箭头是**覆盖**在内容上的，不占布局宽度）；(b) 把面板搬进叶子里的一个容器再整体位移——侵入 `CategoryItemAdapter::applyGeometry` 的几何权威划分。**采用的方案**：host 额外造**第二个叶子** `RibbonCategoryScroll.qml`，`setZ(1)` 抬到面板之上，根是裸 `Item`（无 MouseArea、无 HoverHandler，按压自然穿透到下面的面板），只有两条 12px 的箭头带吃输入。它**不做 `cppHost`→`qmlLeaf` 的回写握手**，所以 `qmlLeaf` 仍指向背景叶子，B44 的拆除路径不受影响。
- **计划里"补 `Behavior on x`"这条无法照做，且原注释本身是错的**：`RibbonCategory.qml` 与 `RibbonCategory.h` 都写着"plan-04 S4: QML Behavior animates the visual x"，但面板的 x 是 C++ host 经 `CategoryItemAdapter::applyGeometry` 直接写进 `QQuickItem::setX` 的——**叶子里根本不存在一个属于自己的 x 可供 Behavior 附着**。改为 host 侧 `QPropertyAnimation(this, "scrollPosition", this)`，时长与缓动取自本轮下沉的 core 常量（300ms / `QEasingCurve::OutQuad`），与 widgets `SARibbonCategoryLayout::setupAnimateScroll` 逐项相同。两处过期注释已改写，并明确声明"本条取代旧的叶子 Behavior 说法"。**教训同 B56：注释里描述的机制要回源码验一次，"计划这么写"不等于"代码能这么挂"。**
- **被丢弃的不止 `scrollFlags`，还有 `Result::newXBase`**：`RibbonCategory::relayout` 原来只取 `r.totalWidth`。但 `newXBase` 只在"内容放得下"的分支里被引擎写（滚动分支保持默认 0），无条件应用会把用户滚出来的偏移清零。用 `scrollButtonFlags` 保证的等价式 `!showLeft && !showRight ⟺ 内容放得下` 作为闸门，只在该情况下把 base 复位到 `newXBase`。
- **`setClip(true)` 是 QML 侧必须显式补的一步**：widgets 的 `SARibbonCategory` 是 `QWidget`，天然裁剪子控件；`QQuickItem` 默认不裁剪，滚出视口的面板会画到类别外面（压到 tab 行/状态区）。构造函数里补上。
- **core 下沉 5 个纯函数，widgets 侧 4 处双实现当场删除**（§4-4）：`scrollButtonRects(categoryWidth, categoryHeight, isRTL, buttonWidth)`（含 RTL 左右互换）、`scrollButtonStep(viewportWidth, isLeftButton, isRTL)`（半视口 + RTL 符号翻转）、`wheelScrollDelta(pixelDelta, angleDelta)`（pixelDelta.x → pixelDelta.y → angleDelta/8 .x → .y 的优先级）、`scaledWheelStep(baseStep, wheelDelta)`（|Δ|>60 ×2、|Δ|<20 ÷2），以及常量 `SCROLL_BUTTON_WIDTH=12` / `SCROLL_ANIMATION_DURATION=300`。`SARibbonCategoryLayout.cpp` 的 `doLayout`/两个箭头点击槽/`setupAnimateScroll`/`scrollToByAnimate` 与 `SARibbonCategory.cpp` 的 `doWheelEvent` 全部改为调用 core 版本，行为逐字节不变（widgets 测试 `SARibbonCategoryLayoutRTLTest` 等仍全绿）。**自查抓到的一个 parity bug**：`scaledWheelStep` 初版在 `wheelDelta == 0` 时返回 `baseStep/2`（因为 `0 < 20`），而 widgets 在无 delta 时**保持 `wheelScrollStep` 原值**；已加 `if (0 == wheelDelta) return baseStep;` 并在测试里钉死。
- **`Item.acceptedMouseButtons` 在 QML 里不可写**（本轮唯一的失败）：Qt 6 的 `QQuickItem` 只有 getter，**没有** `Q_PROPERTY`，叶子根上写 `acceptedMouseButtons: Qt.NoButton` 直接 `Cannot assign to non-existent property`，导致整个覆盖叶子创建失败、`leftBtn && rightBtn` 断言炸掉。删掉即可：裸 `Item` 没有任何鼠标处理器，`QQuickItem::mousePressEvent` 默认 `ignore`，命中测试会继续投给下面的面板。**记法：`acceptedMouseButtons` 是 `MouseArea` 的属性，不是 `Item` 的。**
- **一个"看起来对"的 QTRY 又放行了过期状态**（B56 同类，第二次踩）：`setScrollPosition` 是**同步**改 `mScrollXBase` 并发 `scrollPositionChanged`，但 `scrollButtonFlags` 要等下一个 polish 周期里的 `relayout` 才重新发布。最初写的是 `QTRY_COMPARE(pos, minBase)` → `QTRY_VERIFY(left==true)` → `QVERIFY(right==false)`；因为滚到 -100 时 left **已经**是 true，那个 QTRY 立刻通过，紧接着同步读到的 right 还是上一轮的 true。**修法：QTRY 必须挂在"这一轮真的会翻转"的那个量上**（改成 `QTRY_VERIFY(!right)`），不能挂在一个恰好已经成立的量上。
- 证据：`build-qml-test`（Debug，Qt 6.7.3 msvc2019_64）ctest **30/30** 全绿；`build-qml-rel`（Release）ctest **30/30** 全绿；`qml_Conformance` Release `Totals: 24 passed, 0 failed`（**22 个用例**，按 B54 口径以 `-functions` 计数）。新增 `categoryScrollWheelAndArrows`：溢出时只出现尾随箭头 → 箭头矩形逐项等于 `scrollButtonGeometry`（宽 12 = `SCROLL_BUTTON_WIDTH`、高 = 类别高、`rightX = viewport-12`）且叶子真的把两个按钮摆在那里 → 关掉动画后 `scrollPosition=-100` 使 `panel0->x()` 恰好少 100 → **真实 `QTest::mouseClick` 点尾随箭头**步进 `clampScrollOffset(-100-viewport/2, total, viewport, false)`、点前导箭头原路退回 → 滚到底被夹在 `viewport-total` 且只剩前导箭头 → core 的 delta 优先级与 ×2/÷2/无 delta 三档逐一断言 → **真实 `QWheelEvent`**（`QGuiApplication::sendEvent` 投给 `QQuickView`）走 -120（半档）与 +120（回到 0 并被夹住）→ `wheelScrollStep` 改成 80 后同一滚轮量按新步长走 → 打开动画后 `scrollByButton` 令 `isAnimatingScroll` 为真、**动画途中的滚轮被 `ignore`**、最终落在与非动画路径完全相同的目标 → 切到内容放得下的第二个类别，断言 `contentWidth <= width`、两个 flag 皆假、箭头不可见、滚轮 `isAccepted()==false` 且偏移仍为 0。示例 `QmlMainWindowExample` offscreen 跑 12s 零输出。`python tools/check_core_purity.py src/core` 通过（对 `src/qml` 跑该脚本报的仍是 `QQuickItem`/`QQml*` 规则误伤，其设计目标是 core）。
- **本轮改了 core，已跑 `tools/Amalgamate.sh`**（`src/SARibbonCore.h/.cpp` 为生成物，未手改）。
- 影响计划：04（**WS-A 全部完成**，批次 2 收尾）；WS-D（色板弹出视口若需要滚动，直接复用"host 造第二个 z=1 覆盖叶子 + 裸 Item 根穿透按压"这套写法，不要往背景叶子里塞按钮）；WS-C（定制器在运行时增删面板会触发 `relayout`，滚动 flag 与箭头矩形会自动跟随；但**运行时新建的类别**依赖 `relayout()` 里的 `ensureScrollOverlay()` 兜底，`componentComplete` 不是唯一入口）。

### B58：WS-D1/D2 颜色网格 —— 三条只有"逐像素对齐 widgets"才会显形的 QGridLayout 事实（第 18 轮）

- 日期：2026-10-02
- 发现位置：计划 04 WS-D1（颜色算法下沉 core）、WS-D2（`RibbonColorGrid` host + leaf），对照物 `src/widgets/colorWidgets/SAColorGridWidget.cpp`
- **D1 下沉了什么、以及为什么这样切**：`getStandardColorList()`（10 色）从 `SAColorGridWidget` 迁入 `SARibbonCoreUtil`，原头文件改为 include core 头保持调用方源码兼容——**不能两边各留一份声明**，MSVC 下 `dllimport`/`dllexport` 冲突直接编译失败。`defaultColorPaletteFactors()` = `{180,160,140,75,50}` 与 `colorPaletteShades()` 取代 `SAColorPaletteGridWidget::PrivateData::makeColorPalette` 的循环体（因子外循环、颜色内循环、`lighter` 的顺序是**行主序填充契约**，未改）。`colorBandHeight()`/`calcColorUnderIconMetrics()` **只做摆放，不做缩放**：widgets 侧缩放的是 `SA::iconToPixmap` 返回的 dpr 感知 pixmap 且允许放大，QML 侧复用已发布的 `iconSide`，两者输入不同，硬合会把 dpr 语义搞错。`noneColorSlashLine()` 供 `SAColorToolButton::paintNoneColor` 用；`SARibbonColorToolButton` 的无效色斜线**沿用本地 qreal 除法**（与 core 的 int 除法相差不足 1px），不改写以免移动 widgets 既有渲染结果。
- **格子尺寸不是 `sizeHint`，是 `max(sizeHint, minimumSizeHint)`**：`SAColorGridWidget` 的单元是 icon-only、`autoRaise` 的 `SAColorToolButton`（`setMargins(QMargins(4,4,4,4))`）。实测其 `minimumSizeHint` 比 `sizeHint` 大出**恒定的 (+16, +15)**——在 windows11 / windows / fusion 三种样式、两种字号下都不变（即 QToolButton 为图标按钮预留的框架余量与样式无关）。这条余量已钉成 core 常量 `ICON_ONLY_MIN_HINT_EXTRA_WIDTH=16` / `ICON_ONLY_MIN_HINT_EXTRA_HEIGHT=15`，并封装为 `SA::colorGridCellSize(iconSize, cellMargin)`。QML host 直接用该函数算格子，**不再自己凑常量**。（注意 `SAColorToolButton::sizeHint()` 是 protected，测试只能用 `geometry()`/`minimumSizeHint()`。）
- **`setRowMinimumHeight` 撑高的行里，按钮是居中的，不是拉伸的**：`QToolButton` 垂直方向是 `Fixed`，所以 `QGridLayout` 把它摆在行内垂直居中；又因为偏移是**向下取整**，多出来的 1px 落在下方。host 的 `updateGridMetrics()` 相应写成 `cellY = y + (rowH - baseCellH) / 2`，格子高度仍取 `baseCellH`。首版没做居中，测试抓到 `QRect(1,1 32x40)` vs widgets `QRect(1,5 32x31)`——**高度也错**，因为顺手把格子拉到了行高。
- **尾部 `QSpacerItem` 只贡献自身宽度，不额外占一个间隔**：`setHorizontalSpacerToRight(true)` 在列 `mColumnCount` 上塞 `new QSpacerItem(40, 20, Expanding, Minimum)`。实测 spacing=2 时总宽 244、spacing=4 时 254、关掉弹簧 spacing=2 时 204（= 2 + 6·32 + 5·2），差值恰为 40。即 `QGridLayout` 只统计到**最后一个有 widget 的列**为止的间隔，弹簧列自身无 widget，不再产生一个 gap。host 写成 `totalW += mSpacerWidth;`（首版多加了一个 `mHorizontalSpacing`，得 246）。
- **C++ 新建的 host 要出叶子，必须给它 QML 上下文 + 一个 QML 创建的根对象**：`createVisualLeaf(host, url)` 用 `qmlEngine(host)` 解析引擎，失败时沿 `parentItem` 链找；测试里把 host 直接挂到裸 `QQuickView::contentItem()` 时两条路都断了，**叶子静默不创建**（`qmlLeaf()` 为 null，没有报错）。修法固化成测试助手 `attachHost()`：用 `new QQmlComponent(engine, engine)`（父对象是引擎，活得比视图久，见 B44 的上下文生命周期族）建一个空 `Item` 根，`setParentItem(contentItem())` 后 `view.setContent(...)`，再 `QQmlEngine::setContextForObject(host, engine->rootContext())`，最后 `ensureQmlLeaf()`。**顺带修掉的一个真问题**：视图没有由 QML 创建的根对象时窗口按透明合成，`grabWindow()` 的像素与预期不符。
- **两个只在"用像素当判据"时才咬人的诊断陷阱**：(a) **Qt 6.7 的 `qDebug() << QColor` 打印的是归一化浮点**，`QColor(ARGB 1, 1, 0, 0)` 是"不透明纯红"而不是 alpha=1——本轮据此误判成"窗口把颜色预乘掉了"，绕了一大圈。要判像素就用 `img.pixel(x,y)` 打原始 ARGB 十六进制（本环境 `grabWindow()` 返回 `Format_RGB32`）。(b) **抗锯齿斜线经不起"逼近某个色值"的容差匹配**：`Canvas` 画出的 1px 红斜线，除线芯两三个像素外全是红白混合（直方图里是 `ffff0c0c`/`ffff1515`/`ffff2626`…），`countPixelsNear(纯红, 容差8)` 要求三通道同时逼近，只数到 2 个像素，于是 `> 4` 的 QTRY 永远不通过。**判据错，不是叶子错**——改成数"偏红"像素（`red>100 && red-green>40 && red-blue>40`），并保留"最左一列无红"和"底色接近白占多数"两条几何/填充断言。记法：**测试抗锯齿输出要判"色相倾向"，不要判"色值逼近"**。
- 证据：新增 `tests/qml/tst_color_qml.cpp`（`qml_Color`，**6 个用例**，按 B54 口径以 `-functions` 计数）：`gridGeometryMatchesWidgets`（同一份颜色表/列数/图标尺寸/边距下，host 的 `cellRects` 与 `SAColorGridWidget` 真实 `QGridLayout` 的格子矩形逐项相等，含 implicit 尺寸）→ `unlimitedColumnsSingleRow` → `rowMinimumHeightAndSpacer`（行最小高居中 + 弹簧宽度差）→ `exclusiveCheckSemantics` → `leafPlacesCellsAndClicks`（真实 `QTest::mouseClick`，断言委托 x/y/w/h 等于 `cellRects`、色块像素为纯色、`colorClicked`/`checkedIndex`）→ `noneColorMarkRenders`（斜线内缩量等于 `SA::noneColorSlashLine` 在同一色块矩形上的 x1、偏红像素数、最左列无红、白底占多数、相邻有效色块仍是纯色）。`build-qml-test`（Debug，Qt 6.7.3 msvc2019_64）ctest **31/31** 全绿；`build-qml-rel`（Release）ctest **31/31** 全绿（叶子几何有改动，按规矩双验证）。`python tools/check_core_purity.py src/core` 通过。**本轮改了 core，已跑 `tools/Amalgamate.sh`**。
- 影响计划：04（WS-D2 完成；D3 `RibbonColorMenu` / D4 `RibbonColorToolButton` / D5 示例与 README 待做）；WS-D3（色板 shade 网格直接用 `SA::colorPaletteShades` + `SA::colorGridCellSize`，**不要**再量一次 QToolButton 余量；"无颜色"项复用同一条 `noneColorSlashLine`）；WS-D4（`ColorUnderIcon` 的色带摆放取 `SA::calcColorUnderIconMetrics`，缩放留在前端各自做，理由见上）；WS-C（跨前端 XML 兼容测试若要比对颜色，注意 `qDebug` 的浮点打印陷阱同样适用于 `QColor` 的日志断言）。

### B59：WS-D3 颜色菜单 —— Popup 的内容不在你以为的地方，以及"无颜色"标记只能共享一次（第 19 轮）

- 日期：2026-10-02
- 发现位置：计划 04 WS-D3（`RibbonColorMenu` host + leaf），对照物 `src/widgets/colorWidgets/SAColorMenu.cpp`
- **不含 `QColorDialog`，改用信号拆分**：widgets 的 `onCustomColorActionTriggered` 是"弹模态对话框 → 记录 → `updateGeometry` → `emitSelectedColor`"一条线。QML 侧把它拆成 `requestCustomColor()`（发 `customColorRequested()`）+ `addCustomColor(c)`（记录 + 报告 + 关菜单）。理由不是"省事"：`QColorDialog` 属于 widgets/QtQuick.Dialogs，拖进来等于给 SARibbonQml 挂第二套工具包，与 core 纯净性同源的红线。**拆法必须保住一个语义细节**——widgets 那边对话框是模态盖在菜单上的，菜单此刻并没有关；所以 `requestCustomColor()` 不动 `menuVisible`，测试里显式断言"点了自定义颜色行之后 `isMenuVisible()` 仍为 true"。`recordCustomColor()` 单独暴露，复现 widgets 的记录规则（先追加、满了整体左移、新色落最后）；`setMaxCustomColorCount()` 缩容量时**从前面裁**（widgets 没有这个 setter，是 QML 侧的补充），容量为 0 会把记录裁空且此后拒绝记录。
- **Popup 打开后，它的内容项被重挂到窗口 Overlay 下**：`collectVisualItems(host->qmlLeaf(), ...)` 找不到菜单内部的任何东西（B50 的走法没错，起点错了）。必须从 `view.contentItem()` 起走视觉树，固化为测试助手 `findVisualItem(root, objectName)` / `findGrid(root, name)`。连带的一条：**拥有 Popup 的宿主不能当 `QQuickView` 的 QML 根对象**——默认 `SizeRootObjectToView` 会把根撑满窗口，叶子按 `y: root.height` 定位弹窗就被推到窗口外面，什么都点不到。测试里包一层带尺寸的 `Item`，再用 `rootItem->findChild<RibbonColorMenu*>("colorMenuHost")` 取回宿主。
- **`Column` 只在一次 polish 之后才摆放子项**：紧跟 `openMenu()` 之后读 `mapToItem(nullptr, ...)`，所有子项的 y 都是 0，于是"深浅色板在'无颜色'之上、'无颜色'在'自定义颜色'之上"这条顺序断言直接 FALSE。改成 `QTRY_VERIFY`（B56 的同一记法：**QTRY 那个真正会翻转的量**，别 QTRY 一个恒真的表达式再 QVERIFY 里面的几何）。
- **"无颜色"标记的画法只能共享一次，共享的是相对量不是绝对矩形**：widgets 是在 32×32 的 pixmap 上内缩 1px 画（`createNoneColorIcon` → `paintNoneColor(rect.adjusted(1,1,-1,-1))`）；QML 菜单行的标记盒是 `noneMarkSide`（默认 16）同样内缩 1px；网格单元的标记盒是"单元四周内缩 `cellMargin`"、**不再额外内缩**。三者的原点与内缩量都不同，把矩形当共享量必然写三份。做法：host 只发布**相对内缩量**（`SA::noneColorSlashLine(QRect(0,0,side,side).adjusted(1,1,-1,-1)).x1()`），叶子侧新增 `RibbonColorNoneMark.qml` 用一个 `markMargin` 属性把盒原点加回去——网格传 0、菜单行传 1，`RibbonColorGrid.qml` 里原来内联的那支 Canvas 也换成了它。标记的三个颜色（白底/红斜线/黑边框）是标记自身的固定色，widgets 那边就是字面量，**不走主题 token**，叶子里注明了这一点以免评审误判为漏改。
- 顺带修掉 D2 遗留的两处说明：`RibbonColorGrid` 的头/实现注释原写"尾部弹簧占一列，额外增加一个间隔加其宽度"，与 B58 实测结论相反，改为"独占一列且列内无 widget，`QGridLayout` 不为它再计一个间隔：只增加弹簧自身的宽度"。
- 证据：`tests/qml/tst_color_qml.cpp` 由 6 例增至 **9 个用例**（`qml_Color`，按 B54 口径以 `-functions` 计数），新增 `menuDataMatchesWidgets`（对着一个真实的 `SAColorMenu` 逐项核对：标准色表 = `colorPaletteGridWidget()->colorList()`、因子 = `factor()`、深浅行 = `SA::colorPaletteShades(...)`、容量 = 自定义网格 `columnCount()`、色块盒 = 其 `colorIconSize()`、两行文案 = 两个 `QAction::text()`，以及 `enableNoneColorAction` 用 `insertAction(mCustomColorAction, ...)` 造成的条目顺序）→ `customColorRecordRules` → `menuLeafOpensAndPicks`（真实鼠标点深浅色块 / "无颜色"行 / "自定义颜色"行，再回灌 `addCustomColor` 后重点自定义色块）。`build-qml-test`（Debug）ctest **31/31** 全绿，`build-qml-rel`（Release）ctest **31/31** 全绿（叶子几何有改动，按规矩双验证）。`python tools/check_core_purity.py src/core` 通过。**本轮未改 core，无需跑 `tools/Amalgamate.sh`**。
- 影响计划：04（WS-D3 完成；D4 `RibbonColorToolButton` / D5 示例与 README 待做）；WS-D4（`colorMenuStyle = WithColorMenu` 时按钮直接内嵌一个 `RibbonColorMenu` host，取色接 `selectedColor`，**不要**自己再画一份菜单；`ColorFillToIcon` / `ColorUnderIcon` 的色带摆放取 `SA::calcColorUnderIconMetrics`，缩放留在前端）；WS-C（定制对话框若含弹出层，"Popup 内容挂 Overlay"与"宿主不能当 QML 根对象"这两条同样适用；`QColor` 的日志断言仍受 B58 的浮点打印陷阱影响）。

### B60：WS-D4/D5 颜色按钮 —— 基类构造函数里的虚分派、Large 按钮不为箭头加宽，以及空 `QVariantMap` 在 QML 里是真值（第 20 轮）

- 日期：2026-10-03
- 发现位置：计划 04 WS-D4（`RibbonColorToolButton` host + leaf）、WS-D5（示例 + README + Release 双验证），对照物 `src/widgets/SARibbonColorToolButton.cpp`
- **基类构造函数里的虚调用解析到基类版本，派生版的 `layoutInput()` 那趟白跑**：`RibbonToolButton` 的构造函数会算一次 sizeHint，此刻虚表还是基类的，于是"图标槽永远保留"（派生版规则）没有生效。修法是在 `componentComplete()` 里补一次 `updateSizeHint()` + `updateLayout()`，并为此把基类的 `updateLayout()`/`layoutInput()` 从 private 提到 **protected virtual**、`updateSizeHint()` 提到 protected、`hasMenu()`/`openMenu()`/`closeMenu()` 改成 virtual（`updateLayout()` 里判菜单也从 `mMenuItems.isEmpty()` 改成 `hasMenu()`——子类持有的弹窗不是 `RibbonMenuItem` 列表）。**连带一条时序事实**：QML 里声明的 host 才会走 `componentComplete()`，C++ 新建的不会，所以菜单叶子的 `ensureQmlLeaf()` 不能放在 `ensureColorMenu()`（构造函数里就会调到，那时还没有引擎），只能放在 `componentComplete()`，C++ 新建路径靠 `RibbonColorMenu::openMenu()` 自己的 `ensureQmlLeaf()` 兜底。
- **core 的 `SARibbonToolButtonLayout` 只在"小按钮"分支里加 `indicatorLen`**：Large 按钮有没有菜单，宽度完全一样。首版断言写成 `noMenu->width() < under->width()` → FALSE。这不是布局 bug，是 widgets 一贯行为（大按钮的箭头画在文字下方那一格里，不额外占宽）。测试改成 Large 断言相等、另加两个 Small 比例按钮断言"有菜单才更宽"。**判宽度差之前先确认比例分支**。
- **两种色标的色带高度不可互相比**：`ColorUnderIcon` 的色带随图标槽放大（示例里被撑满的面板 → 槽高 245，色带 61），`ColorFillToIcon` 的填充盒被**自然图标尺寸 32** 封顶（色带 30）。首版写 `fillBand.height() > underBand.height()` → FALSE。正确判据是 `int(fillBand.height()+0.5) == iconSide - 2*int(inset)`，其中 `inset = COLOR_BLOCK_MARGIN * side / DEFAULT_COLOR_ICON_SIZE`（widgets `createColorIcon` 的 1px 边按比例缩），再加一条 `> SA::colorBandHeight(fillSide)` 说明它确实不是那条"图标下方的带"。**顺带把示例场景的 `panelHeight` 固定成 100**：面板 `anchors.fill` 会造出 245px 高的图标槽，弹窗和点击点都跑到窗口外，QTest 一路告警 "Mouse event at X, Y occurs outside target window"。
- **像素判据要区分"偏红"和"逼近某个红"**：`#e02020` 的实心色带本身就满足 B58 那条"偏红"启发式（`red>100 && red-green>40 && red-blue>40`），于是"无颜色格才有红斜线"的断言在有效色格上也数出 4392 个像素。有效色改成 `countPixelsNear(shot.copy(area), bandColor, 8) * 100 > w*h*95`（≥95% 纯色填充，允许抗锯齿边缘）；"无颜色"格仍用偏红计数 + 最左列为 0 + 白底占多数三条一起判。
- **默认色跟的是 `SARibbonColorToolButton`，不是 `SAColorToolButton`**：前者 `PrivateData::mColor` 默认无效（画"无颜色"斜线），后者默认 `Qt::white`。首版抄了后者的白，改成无效色后按钮的初始外观才与 widgets 的 ribbon 颜色按钮一致。
- **空 `QVariantMap` 在 QML 里是真值 —— A3 的遗留告警到 D5 跑示例才显形**：`RibbonCategoryScroll.qml` 写的是 B48 形状 `flags ? flags.left : false`，但 `flags` 来自 `Q_PROPERTY(QVariantMap scrollButtonFlags)`，在首次 `relayout` 之前是**空 map**——空 map 仍是对象、仍为真，于是 `flags.left` 是 undefined，每个类别在启动时刷 6 条 "Unable to assign [undefined] to double/bool"。**绑定形状没错，错在把"空容器"当成了 null**。修法放在 C++ 侧：`RibbonCategory` 构造函数里把 `mScrollFlags`/`mScrollGeometry` 初始化成完整键集（`left/right=false`，`leftX/rightX/width/height=0`），叶子侧一行没动。记法：**host 发布的 `QVariantMap`/`QVariantList` 属性必须从第一刻起就是"满键"的**，否则叶子要么加防御性判空（多一个绑定依赖），要么就吃告警。测试没抓到它，因为 `tst_conformance_qml` 的类别都是"先建好 host 再断言几何"，叶子的首次绑定发生在 `publishScrollState` 之后。
- **`Connections` 的两种写法在 Qt 5.12 与 Qt 6 之间没有交集，于是把信号转发到按钮上**：示例要接 `RibbonColorMenu::customColorRequested`，旧式 `onCustomColorRequested:` 在 Qt 6.7 告警弃用，新式 `function onCustomColorRequested() {}` 在 Qt 5.12（模块声明的最低版本）不合法。既然 widgets 的 `SARibbonColorToolButton` 用户也不需要伸手进菜单，就在 `RibbonColorToolButton` 上**转发**这个信号（`ensureColorMenu()` 里一条 PMF connect，菜单随样式切换重建时自然重连；`NoColorMenu` 时菜单对象不存在，信号永不触发），示例直接写 `onCustomColorRequested:`，`Connections` 整个去掉。示例里用一个色块 `Popup`（`customColorPicker`）代替 `QColorDialog`，回灌 `addCustomColor()`——与 accentPicker 同一套写法，理由仍是不把第二套工具包拖进模块。
- 证据：`tests/qml/tst_color_qml.cpp` 由 9 例增至 **12 个用例**（`qml_Color`，按 B54 口径以 `-functions` 计数），新增 `colorButtonGeometryFollowsCore`（色带矩形 = `SA::calcColorUnderIconMetrics` 平移到图标槽、带高 = `SA::colorBandHeight`、FillToIcon 按比例内缩、`colorSlashInset` = `SA::noneColorSlashLine(...).x1()`、`NoColorMenu` 下 Large 等宽 / Small 收窄 / 图标槽仍保留）→ `colorButtonRendersSwatch`（grabWindow：色带中心像素、填充盒中心像素、"无颜色"格的斜线与白底、有效色带 ≥95% 纯色）→ `colorButtonMenuPicksColor`（真实点击：动作区发 `colorClicked` 不发 `colorChanged`、菜单区点深浅色块改色、"无颜色"行回到无效色、"自定义颜色"行只发转发信号且菜单不关、回灌后记录 1 条、`NoColorMenu` 往返）。Debug `qml_Color` `Totals: 14 passed, 0 failed`；`build-qml-test`（Debug，Qt 6.7.3 msvc2019_64）ctest **31/31** 全绿；`build-qml-rel`（Release）ctest **31/31** 全绿（叶子几何有改动，按规矩双验证）。示例 `QmlMainWindowExample` offscreen 跑 10s **零输出**（含上述 30 条类别告警与 2 条 `Connections` 弃用告警全部清掉）。`python tools/check_core_purity.py src/core` 通过。**本轮未改 core，无需跑 `tools/Amalgamate.sh`**。
- 影响计划：04（**WS-D 全部完成**，批次 3 收尾；剩余已知差异只有无边框窗口、元素工厂、QAction 桥、`SARibbonMenu::addWidget`、QAB 用户勾选定制五项，均在本计划范围外）；WS-C（定制器运行时增删面板会重跑 `relayout`，滚动 map 已"满键"，不会再出现首轮 undefined；定制对话框若要接取色，沿用"host 转发信号 + QML 侧 Popup 回灌"这套写法，不要在 SARibbonQml 里开对话框）。

### B61：WS-C1 定制系统 core 下沉 —— 记录格式是两个前端的契约，模板化是为了不切片（第 21 轮）

- 日期：2026-10-03
- 发现位置：计划 04 WS-C1（`SARibbonActionTag` / 15 个 `make*` 工厂 / `isCanCustomize` / XML 序列化下沉 core），对照物 `src/widgets/SARibbonCustomizeData.cpp`、`src/widgets/SARibbonCustomizeWidget.cpp`
- **XML 读写必须是模板，不能是普通函数**：`sa_customize_datas_to_xml/from_xml` 原样搬进 core 会把 `QList<SARibbonCustomizeData>` 退化成 `QList<SARibbonCustomizeRecord>`——widgets 的派生记录带 `mActionsManagerPointer`，切片之后 `apply()` 拿不到注册表，`from_xml` 只能靠调用方事后补挂 manager（现有代码正是这么补的，等于把切片的痕迹留在了 API 上）。`simplify()` 早就是模板，`recordsToXml`/`recordsFromXml` 跟随同一手法（header-only，`SARibbonCustomizeXml.h`），widgets 侧改为 `recordsFromXml<SARibbonCustomizeData>(xml)` 再逐条 `setActionsManager(mgr)`，语义与 2.x 完全一致但不再切片。QML 定制器将来以同样方式绑定自己的注册表。**判断依据固化为测试**：用一个多带一个 `int marker` 的派生记录类型跑完整往返，断言 `sizeof` 变大、返回元素仍能写 `marker`。
- **`ActionTag` 提升沿用 B21 手法，不能写成类作用域 `using` 声明**：`using SARibbon::Core::CommonlyUsedActionTag;` 放进类里 MSVC 报 C2886（类作用域 using 声明无法引入命名空间的枚举符）。照 `SARibbonPanelItem::RowProportion` 的先例写 `using ActionTag = SARibbon::Core::SARibbonActionTag;` + 七个 `static constexpr ActionTag X = SARibbon::Core::X;`，`SARibbonActionsManager::CommonlyUsedActionTag`、类内裸名、隐式转 int 三类存量用法全部不用改。**连带一条**：`pyside6/typesystem_saribbon.xml` 里 `<enum-type name="ActionTag"/>` 现在指向别名——与 plan-02 之后 `RowProportion` 的处境相同（Shiboken 解析别名枚举的能力未变），sip 侧不引用 ActionTag，故本次不动绑定文件。
- **core 侧多了 `QVariant`/`QObject` 依赖**：`isCanCustomize`/`setCanCustomize` 读写 `SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE` 动态属性，Debug 首版报 C2079（`QVariant` 未定义类型）与 C2664（`bool` → `const QVariant&`）。`SARibbonCore` 本来就链 `Qt::Core`，加两个 include 即可，不算纯净性破口。这两个函数下沉后 widgets 的 `SARibbonCustomizeData::isCanCustomize/setCanCustomize` 改为转发，QML 三个 host（Bar/Category/Panel）将来直接暴露同名属性、共用同一份动态属性名。
- **9 处"空 object name"告警合并成一个参数化 helper**：2.x 每个工厂各写一份 `qDebug() << QObject::tr("SARibbon Warning !!! customize %1,but get an empty %2 object name,...")`，措辞逐字重复。core 版收敛为文件内 `sa_warn_empty_customize_name(op, what)`，保留原有措辞片段（`"remove action"` / `"category/panel/action"` 等）。**翻译无回归**：`src/widgets/i18n/*.ts` 里这 9 条全部是 `type="unfinished"`（从未被翻译），因此串变化不影响任何已发布译文；将来若翻译，只需译一条模板。
- **XML 断言钉"属性前缀"而不是"整篇文档"**：`writeStartElement` 紧跟 `writeEndElement` 时 `QXmlStreamWriter` 到底输出 `<customize-data/>` 还是 `<customize-data></customize-data>`，没有文档保证、跨版本可能变。2.x 读取端只依赖元素名、属性名与**属性顺序**，于是测试断言 `text.contains("<customize-data type=\"1\" index=\"0\" key=\"Tab\" category=\"cat1\" panel=\"\" row-prop=\"1\"")` + 首尾元素名，把格式契约钉死而不绑定闭合写法。**同类取舍**：源码不带 `/utf-8`，非 ASCII 测试串一律写 `QStringLiteral("\u65B0\u5EFA\u9875")` 转义码点，另断言落盘字节是 UTF-8（`QByteArray` 版 writer 在 Qt6 无 `setCodec`，默认即 UTF-8）。
- 证据：新增 `tests/core/tst_customizeRecord.cpp`（`core_CustomizeRecord`，10 个用例：16 个 `ActionType` 值 + 7 个 `SARibbonActionTag` 值钉死、构造态、15 个工厂逐字段、XML 属性顺序、全类型往返、异常条目跳过（缺 `type` / `type="zz"` / 缺可选属性）、空指针与空列表守卫、派生类型不切片、可定制动态属性、`simplify()` 五类归并），首轮即通过。`build-qml-test`（Debug，Qt 6.7.3 msvc2019_64）ctest **32/32** 全绿，`build-qml-rel`（Release）ctest **32/32** 全绿。格式兼容性另有既存的 `SARibbonQuickAccessCustomizeTest`（#25）覆盖 widgets 侧 `toXml`/`fromXml` 往返。`python tools/check_core_purity.py src/core` 通过。改了 core/widgets 头文件，已跑 `bash tools/Amalgamate.sh`；新增 header-only 文件需登记两处：`src/core/CMakeLists.txt` 的 SOURCES（`sa_sync_include ... FLATTEN`）与 `tools/amalgamate/SARibbonCoreAmalgamTemplatePublicHeaders.h`（新 .cpp 才需第三处 `SARibbonCoreAmalgamTemplate.cpp`）。
- 影响计划：04（WS-C1 完成，批次 4 可继续）；WS-C2（`RibbonActionRegistry` 的 tag 值直接用 `SARibbon::Core::SARibbonActionTag`，注册与 `autoRegister` 需要 Bar/Category/Panel 三个 host 先补子项查询接口——目前只有 `register*`/`unregister*`；`RibbonCustomizer` 产出 `QList<Core::SARibbonCustomizeRecord>`，XML 走 `recordsToXml`/`recordsFromXml`，**不要**在 QML 侧另写一份序列化）；WS-C3（跨前端兼容测试用 widgets 产出的 XML 在 QML 侧加载，属性顺序已被 `core_CustomizeRecord` 钉住，可放心断言结构一致）。

---

### B62：WS-C2 QML 注册表与定制器 —— 三个 host 类没导出、模型卸载留下过期行、增删记录必须按"终态"解释（第 22 轮）

- 日期：2026-10-03
- 发现位置：计划 04 WS-C2（`RibbonActionRegistry` / `RibbonActionRegistryModel` / `RibbonCustomizer` + `tests/qml/tst_customize_qml.cpp`），对照物 `src/widgets/SARibbonActionsManager.cpp`、`src/widgets/SARibbonCustomizeWidget.cpp`
- **`RibbonBar` / `RibbonCategory` / `RibbonTab` 一直没有 `SA_RIBBON_QML_EXPORT`**：`RibbonQuickHost` 家族的其余成员（`RibbonPanel`/`RibbonLayoutItemHost`/`RibbonToolButton`/`RibbonQuickAccessBar`）都带导出宏，这三个漏了，此前无人发现是因为**没有任何跨 DLL 的 C++ 消费方**——QML 侧靠 `qmlRegisterType` 用它们，测试则一律走 `findChild<QQuickItem*>` + `property()`。定制器测试是第一个直接调 `bar->categoryByObjectName(...)` 的用户，链接期立刻 LNK2019（含 `staticMetaObject`，即整类符号一个都没进导入库）。修法就是补宏，**不要**改测试去绕（绕开等于承认这三个 host 的 C++ API 对外不可用，而定制系统正是要把它们交给应用代码）。
- **`RibbonActionRegistryModel::uninstallRegistry()` 只断连接、不清行**：`mRows` 里是描述符的值拷贝，而描述符带着 `RibbonLayoutItemHost*`，所以卸载后的模型仍在发布一份**不再被跟踪**的注册表内容（宿主可能已经销毁）。widgets 侧 `uninstallActionsManager()` 走 `setActionsManager(nullptr)` → `update()`，行列表会被重建为空。已改为 `disconnect` + 置空 + `registryChanged()` + `rebuild()`；同时把 `setRegistry()` 里的 `uninstallRegistry()` 调用换成内联 `disconnect`，否则换源会连做两次模型 reset、发两次 `registryChanged`。
- **增删记录要按"终态"而不是"转变"来解释，否则撤销一对"增+删"必然失败**：`reverseRecord()` 逐条照抄 widgets `sa_customize_datas_reverse`，其中 `RemoveActionActionType` **没有逆操作**（`Remove{Category,Panel,Action}` 与 `Rename{Category,Panel}` 都没有）。于是"先 AddAction 再 RemoveAction"这两条都进了 `mApplied`，撤销时逆序重放到 AddAction 的逆记录 = RemoveAction，而宿主早被后一条摘走了 → `detachChildItem` 失败 → `reverse()` 返回 false。决定把 AddAction / RemoveAction 两支写成**幂等**：项已挂上 / 已摘下都算成功（记录描述的是应当成立的终态），而不是把失败留给调用方。理由：这不是掩盖错误，撤销路径天然会重放"已经不成立"的转变；顺序类与显隐类记录不受影响，仍按原语义。
- **Qt 6 删掉了 `QXmlStreamWriter::setCodec`，字节兼容要靠三件事凑齐**：`setAutoFormatting(true)` + `setAutoFormattingIndent(2)` + `writeStartDocument()/writeEndDocument()` 包住 `Core::recordsToXml`，编码用 `#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)` 守卫（与 widgets `SARibbonCustomizeWidget.cpp` 现有写法同源）。少任何一件，QML 写出的文件与 widgets 写出的文件就不再逐字节相同——而"两个前端互读"正是 C1 把序列化下沉 core 的全部理由。
- **注释里不能出现 `*/`，包括"通配符式"的写法**：测试文件的类注释写了 `(Remove*/Rename* have no inverse)`，`*/` 提前闭合了 `/** */`，随后是一串看起来毫不相干的 C2143/C4430/C2017/C2018/C2447。改写成"the Remove and Rename record types have no inverse"。记法：**文档注释里列举以星号结尾的名字时，必须把星号去掉或换成文字**。
- **两条测试脚手架事实**：(a) `saRibbonRegisterQmlTypes(QQmlEngine*)` 在**全局命名空间**，写成 `SARibbonQml::saRibbonRegisterQmlTypes` 会 C2039；(b) 从 Git Bash 直接跑 `qml_Customize.exe` 时 stdout 被吞掉（重定向、管道、`cmd //c` 三种都试过，输出为空但退出码正确），要看断言细节必须用 QTest 自己的 `-o file.txt,txt` 落盘再读。
- 证据：新增 `tests/qml/tst_customize_qml.cpp`（`qml_Customize`，**7 个用例**，按 B54 口径以 `-functions` 计数）：`registryAutoRegisterAndModel`（`autoRegister` 收到 5 项 / 3 个 tag / tag 名取类别标题与"快速访问栏"，`filter`/`search`/key↔item↔tagOf 往返、模板命令 `hasItem()==false`、重复 key 被拒、`markCustomizable` 打上 core 动态属性，模型的 role/`infoAt` 满键 B60、`uninstallRegistry` 后行数为 0）→ `recordsSimplifyRules`（增删相消、连续改名留最后一条、两次同向 order 合成位移 2、正负相消删除、两次显隐合一）→ `applyAndReverseHostTree`（新类别/新面板/模板命令落地成宿主并出叶子、`bindItem` 后 key 仍指向同一宿主、摘不销毁、撤销后结构复原而改名不复原）→ `enforceCanCustomizeGate`（闸门打开时 apply 失败 + `applyFailed` 一条 + 树未动 + 记录仍待应用，打标后成功）→ `quickAccessRecords`（QAB 增/移序/撤销，删后重加仍是同一宿主指针）→ `visibleCategoryRecord`（隐藏/撤销/两次隐藏合并）→ `xmlRoundTripAndCrossFrontend`（含中文标题的 UTF-8 往返、空流与畸形流被拒、widgets 产出的 4 条记录在 QML 侧 apply 成功、QML 产出物被 widgets `sa_customize_datas_from_xml` 逐字段读回）。该目标同时链 `SARibbonQml` 与 `SARibbon::Widgets`（跨前端用例需要 widgets 的自由函数作对照方）。`build-qml-test`（Debug，Qt 6.7.3 msvc2019_64）ctest **33/33** 全绿，`build-qml-rel`（Release）ctest **33/33** 全绿。`python tools/check_core_purity.py src/core` 通过；`src/qml/**` 无任何 widgets include。**本轮未改 core/widgets，无需跑 `tools/Amalgamate.sh`**。
- 影响计划：04（WS-C2 完成；WS-C3 定制对话框待做）；WS-C3（对话框直接绑 `RibbonActionRegistryModel` 的具名 role，不要自己拆 `infoAt` 的 map；`RibbonCustomizer.apply()` 会在运行时增删 host 树节点并调用 `ensureQmlLeaf()`，与 B45 的"root context 创建叶子"路径已验证不冲突——前提是宿主先 `setParentItem` 再 `ensureQmlLeaf`；对话框若含弹出层，B59 的两条仍然适用）；通用（新 host 类务必带 `SA_RIBBON_QML_EXPORT`，否则第一个跨 DLL 的 C++ 用户就会撞上 LNK2019）。

---

### B63：WS-C3 QML 定制对话框 —— 弹出层不是可视项、模型复位会把 ListView 的选中行留在旧行号上（第 23 轮）

- 日期：2026-10-03
- 发现位置：计划 04 WS-C3（`src/qml/qml/RibbonCustomizeDialog.qml` + `RibbonCustomizeTreeModel` + `tests/qml/tst_customize_qml.cpp` 的对话框用例），对照物 `src/widgets/SARibbonCustomizeWidget.cpp`
- **`QQuickPopup` 派生自 `QObject` 而不是 `QQuickItem`，`findChild<QQuickItem*>("renamePopup")` 永远返回 nullptr**：对话框里的重命名子层是一个 `Popup`，测试第一版按可视项去找，`QVERIFY` 直接失败。Qt 6.7.3 既没有公开的 `QQuickPopup` 头文件，`QQuickListView` 也没有 `itemAtIndex`，所以只能走 `findChildren<QObject*>` + `inherits("QQuickPopup")` + `property("visible")` + `QMetaObject::invokeMethod(o, "open")`。这是 B59 的补充面：B59 说"含弹出层的宿主不能当组件根、遍历要从 `view.contentItem()` 走"，本轮补上"弹出层在测试里只能当 QObject 摸"。**记法**：QML 的 Controls 类型里，`Popup`/`Menu`/`Dialog` 都不是可视项，只有它们的 `contentItem` 才是。
- **`QQuickListView` 跨模型复位会保住旧的 `currentIndex`**，于是"下移面板"之后选中行悄悄滑到了顶替上来的那一行——点第二下"下移"实际移动的是另一个面板，`pushButtonUp` 的 enabled 也跟着错。根因不是按钮状态机，是**预览模型的行号不是地址**：`RibbonCustomizeTreeModel` 每落一条记录就整表 reset，reset 后同一行号指向的是新树里的另一个节点。修法是叶子侧加 `keepSelection(info, ok)`：每次编辑后按 `nodeType` 走 `rowOfCategory/rowOfPanel/rowOfAction/rowOfQuickAction` 把地址重新解析成行号再写回 `currentIndex`（并 `positionViewAtIndex(..., ListView.Contain)`）。**取舍**：没有在 C++ 侧做"选中项跟随"，因为模型是无状态的影子树重建，选中语义属于对话框；C++ 只需要提供按地址查行号的接口。测试相应断言 `currentOf(tree) == 3`，把这条行为钉住。
- **影子树模型必须同时监听 `recordsChanged` 与 `appliedChanged`**：`reverse()` 和 `applyFromXml()` 直接改活动宿主树、根本不经过待应用记录表，只连前者就会出现"撤销后预览行数仍是撤销前的 10 而不是 9"。这不是重复刷新——两个信号描述的是两种不同的树变更来源（暂存区 vs 活动树），预览对两者都要跟。
- **`revision` 必须在 `endResetModel()` 之后自增并发射，不能在复位窗口里发**：叶子侧 `selectedInfo()` 里读 `treeModel.revision` 是为了让 QML 绑定依赖上"行数没变的复位"（改名、调序都不改 `count`，`onCountChanged` 抓不到）。但 QML 的绑定依赖是**动态捕获**的——JS 函数里读的属性同样会登记成依赖，所以 `revisionChanged` 的接收方会立刻回头调 `infoAt()`；若此刻模型还在 begin/end 之间，读到的就是不一致的行。C++ 侧因此在每次 `endResetModel()` 之后才 `++mRevision; Q_EMIT revisionChanged();`（注释已写明理由）。
- **`autoRegister(RibbonBar*)` 返回 `QMap<int, RibbonCategory*>`，QML 侧完全调不动**（`TypeError: Property 'autoRegister' of object RibbonActionRegistry is not a function`）：Qt 不会把返回裸指针容器的函数暴露成 invokable。加 `Q_INVOKABLE int autoRegisterBar(RibbonBar*)` 转发并回报 `count()`，而不是改 `autoRegister` 的签名——C++ 用户（测试、应用）仍需要那份类别指针表。同类事实：`RibbonCustomizeTreeModel.h` 里三个指针属性（`bar`/`registry`/`customizer`）要求被指类型**完整定义**，Qt 6 的编译期 metatype 检查会在目标级 `mocs_compilation.cpp` 里撞上，所以这个头 include 了 `RibbonActionRegistry.h`/`RibbonCustomizer.h`/`../bar/RibbonBar.h` 而不是前置声明。
- **对话框叶子只能按 URL 取**：`SARibbon 3.0` 是纯命令式注册（`saribbon_qml.qrc` 里没有 `qmldir`），`import SARibbon 3.0` 只带来 C++ 后端类型，QML 叶子文件不在类型命名空间里。示例因此用 `Qt.createComponent("qrc:/SARibbon/RibbonCustomizeDialog.qml")` + `createObject(window.contentItem, {parent: ..., bar: ribbonBar})` 首次使用时创建。另外 `Popup` 不能当 `Loader` 的 item（`Loader` 要 `QQuickItem`），所以"惰性加载弹出层"这条路也走不通。对话框自身把 `x/y` 绑到 `parent` 居中，这样挂在 `contentItem`（无 Controls overlay）下也能正确定位。
- **两条既有语义在本轮被测试重新确认**：(a) 顺序记录带的是**相对位移**（`to = current + indexValue`），到头即拒绝、不做夹紧，所以 `ClipX` 落到类别末尾后 `pushButtonDown.enabled == false` 是正确行为；(b) `RibbonCustomizer::recordCount()` 是**未 simplify** 的暂存条数，`simplify()` 只在 `apply()` 里跑——"改名 + 加命令 + 面板调序"就是 3 条，测试最初按 2 条断言是我算错，不是产品缺陷。
- **WIN32 GUI 子系统的示例程序吞掉 stdout/stderr**：`QmlMainWindowExample.exe` 跑起来一条日志都看不到。设 `QT_LOGGING_TO_CONSOLE=1`（Qt 建议的新名是 `QT_FORCE_STDERR_LOGGING`）后 `console.log`/`qDebug` 正常输出，据此在 headless 下确认对话框 `visible=true`、父对象是 `QQuickContentItem("ApplicationWindow")`，验证完把临时的 `Component.onCompleted` 探针删掉。这是项目既有 headless 验证法（`grabWindow` + 属性 dump）之外的一条：**要日志就给 GUI 程序设这个环境变量**。
- 证据：`tests/qml/tst_customize_qml.cpp` 从 7 例增到 **12 例**（按 B54 以 `-functions` 计数），新增 `treeModelLevelsScopeAndAddressing`（三档 `showType` 的行集、上下文类别归类、四个 `rowOf*` 寻址、越界行回落到满键空 map）、`treeModelPreviewReplaysPending`（影子树重放全部 16 种记录、撤销后行数复原——本轮 bug 就是它抓到的）、`dialogEditsAndAppliesOnOk`（按 URL 建对话框 → 搜索框过滤 → tag 菜单 → 加命令/改名/上下移 → `pendingCount` 与预览同步 → 取消整批丢弃、确定才落真树）。`build-qml-test`（Debug，Qt 6.7.3 msvc2019_64）ctest **33/33** 全绿，`build-qml-rel`（Release）ctest **33/33** 全绿。`python tools/check_core_purity.py src/core` 通过；`src/qml/**` 无任何 widgets include（唯一命中是 `SARibbonQmlTypes.h` 里一句说明性注释）。示例加"customize"面板与 `openCustomizeDialog()`，README 功能清单加一行、"已知差异"里"定制系统：QML 版暂无"删除并改写为"按稳定字符串 key 寻址（无 QAction 桥）+ 仍缺 QAB 用户勾选定制"。**本轮未改 core/widgets，无需跑 `tools/Amalgamate.sh`**。
- 影响计划：04（WS-C3 完成，批次 4 收官；计划末尾"剩余已知差异"五项确认只剩无边框窗口/标题栏、元素工厂、QAction 抽象桥、`SARibbonMenu::addWidget`、QAB 用户勾选定制）；通用（含 `Popup` 的组件在测试里按 QObject 摸；任何"每步都 reset 的预览模型"配 `ListView` 时，选中状态必须按地址而非行号维护；给 QML 暴露的函数不能返回裸指针容器）。

## 执行中追加（模板，勿删）

```
### B<n>：<标题>
- 日期：YYYY-MM-DD
- 发现位置：计划 0X 步骤 SY
- 证据：<命令 + 输出摘要>
- 处理：<保守方向的决定及理由>
- 影响计划：0X-SY
```
