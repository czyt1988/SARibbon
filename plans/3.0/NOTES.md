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
- 影响计划：01（P2/S1）

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
