# 计划 02：Core 子系统下沉与布局引擎（对应 v2 里程碑 M1，3.0 的关键路径）

> 前置计划：[01-infra-restructure.md](01-infra-restructure.md) 验收门全部通过
> 后续计划：[03-build-ecosystem.md](03-build-ecosystem.md)
> 设计依据：v2 计划 §3（core 全部设计）、§4（widgets 适配器）、§7（测试策略）、§8-M1、§9-D3/D6/D7、§10-R1/R2/R3/R5
> 分支：`dev-3.0`（延续）

## 1. 目标

把 v2 §3 定义的七个子系统（即下列 global/theme/metrics/contract/layout/data/factory）下沉到 `SARibbonCore`，三个布局类退化为引擎适配器，并以黄金几何测试证明**行为零变化**：

1. **global/**：全部公共枚举 + 属性名字符串常量 + 无 widget 的 Util 入 core（逐函数去向见附录 B）。
2. **theme/**：主题数据层（新建 `SARibbonThemeData`，QObject，变更通知）入 core + widgets QSS 应用层；色板派生规则入 core。注意：**2.9.5 没有 `SARibbonThemeManager` 类**，只有 `SA::applyRibbonTheme` 两个自由函数（见 S2 实况）。
3. **metrics/**：`SARibbonMetrics` 收口全部尺寸常量与行高推导（原 `SARibbonBarLayout` 六个**公共**度量函数；其中 5 个是 PrivateData 转发、1 个（`calcMinTabBarWidth`）触碰控件，非全部"纯计算"，见 S3 实况与附录 E）。
4. **contract/**：`SARibbonAbstractLayoutItem` / `SARibbonAbstractLayoutHost` 窄接口。
5. **layout/**：`SARibbonPanelLayoutEngine` / `SARibbonCategoryLayoutEngine` / `SARibbonBarGeometryEngine`，从三个布局类经 **Step A（接口化原地重构）→ Step B（纯 move 搬移）** 两步提取。
6. **data/**：`SARibbonCustomizeData` 纯数据部分下沉。
7. **factory/**：`SARibbonElementFactoryInterface`（可按 D6 降级为"命名空间对齐"）。

**本计划的最高纪律：M1 期间禁止修改任何算法逻辑。** 发现 bug 只允许记录到
NOTES.md 并单独提交修复（修复前后黄金测试都要过）。引擎提取必须是"先接口化、
后搬移"两步，搬移是纯 move，diff 必须可 review（v2 R1）。

## 2. 范围与非目标

**非目标**：
- 结构控制器拆分（RibbonBar/Category/Panel 的 model 下沉）——v2 D7 明确 gate 到 Tier 2（3.1+），出现真实 QML 用户需求前**不做**。
- `SARibbonElementFactory` 的 QML 侧实现（计划 04）。
- core Action 抽象 / Gallery 下沉（D8/D3，Tier 3）。
- QML 模块本体（计划 04，可与本计划 S13 之后并行）。

## 3. 前置条件（逐项验证）

| # | 检查 | 期望 |
|---|------|------|
| P1 | 计划 01 验收门全绿 | 复核 [01 验收门](01-infra-restructure.md#6-完成验收门全部满足才算完成本计划) 全勾 |
| P2 | 测试基线 | `ctest --test-dir build -C Release --output-on-failure` 通过数 == N₀（**N₀ = 26**，评审轮3修正：原稿"25"是计数错误——tests/*.cpp 顶层 25 个之外另有 `tests/auto/SARibbonThemePalette/tst_themepalette.cpp` 注册为 `SARibbonThemePaletteTest`，`tests/CMakeLists.txt` 共 26 个 `add_saribbon_test()` 调用（计划01 S7 后位于 `tests/widgets/CMakeLists.txt`）；与 README 事实快照、NOTES.md B8、计划01 P3、v2 §7.3 一致。注意 `scripts/build.ps1` 的 `-Tests` 默认 OFF，首次 configure 必须带 `-Tests ON`，见 README R2：`pwsh -NoProfile -File scripts/build.ps1 rebuild -Tests ON -Examples ON`） |
| P3 | 布局三巨头行数 | `wc -l src/widgets/SARibbon{Panel,Category,Bar}Layout.cpp` == 1863/1425/1993（已对 2.9.5 基线核实为精确值，非约数；计划 01 的 `git mv` 不改行数，若命中数漂移说明基线已变，需重录本文所有行号） |
| P4 | 度量函数在位 | `grep -n "tabBarHeight\|categoryHeight\|panelTitleHeight\|normalModeMainBarHeight\|minimumModeMainBarHeight\|calcMinTabBarWidth" src/widgets/SARibbonBarLayout.h` 命中 6 行（已核实恰为 h:66/89/92/96/108/114 六行声明；注意 setter 行 `setTabBarHeight` 等含大写 `TabBarHeight`，不被小写 pattern 命中，不会多算） |
| P5 | 纯净门禁可用 | `python3 tools/check_core_purity.py src/core` 退出码 0（该脚本是计划 01 S9 的交付物；2.9.5 基线的 `tools/` 下**没有**此文件，P1 未过则本计划不能开工。调用名口径 round3 终审统一：本地 Windows 若无 `python3` 用 `python`，CI linux/mac 用 `python3`、windows workflow 用 `python`，见 01 S9） |

> **本文行号约定**：所有 `文件:行号` 以 dev 分支 2.9.5 基线的 `src/SARibbonBar/` 为准（= 计划 01 完成后的 `src/widgets/` 同文件同行号）。函数级搬移清单见附录 C/D/E。

## 4. 全程纪律（本计划额外强调）

1. **一步一提交一验证（R3）**：下面每个 S 的每个子步骤（S2.1、S2.2…）独立提交，提交前 `build.ps1 build` + `ctest` 通过数 == N₀。
2. **行为守卫**：每完成一个 Step B，必须在 NOTES.md 记录"迁移前后 `build/bin/Release/MainWindowExample` 截图对比一致"（Windows 实际为 `MainWindowExample.exe`；截图工具建议统一用固定窗口尺寸 + 同一 Qt 版本，两张图 diff 用像素级比较工具而非目测）。视觉回归用截图（3 行/2 行/最小模式 × 亮/暗主题，共 6 张）。
3. **core 纯净（§3.7 禁区）**：每步结束跑 P5；core 内出现 `#include <QWidget>` 等 = 该步失败，必须整改后重提。
4. **禁止双实现**：任何算法只允许存在于 core 一处；widgets 里若发现还在用旧副本立即删除。

## 5. 执行步骤

### S1 枚举与 Global 收编（global/）

**S1.0 实况修正（评审轮1核实，执行前必读）**：
- 2.9.5 中**不存在** `RibbonButtonStyle` 枚举，实名是 `SARibbonToolButton::RibbonButtonType`（SARibbonToolButton.h:35，Q_ENUM 在 :48，值 LargeButton/SmallButton）。
- `ToolButtonPopupMode` **不是** SARibbon 枚举，是 Qt 原生 `QToolButton::ToolButtonPopupMode`（Qt5 属 QtWidgets，core 禁区），不可下沉；SARibbon 只拥有配套的属性名宏 `SA_ActionPropertyName_ToolButtonPopupMode`（SARibbonPanelItem.h:62-63），宏可下沉。
- `SARibbonApplicationButton.h` **没有任何枚举**（已 grep 核实），原"其枚举等"的说法作废。
- "BarMode"实名是 `SARibbonBar::RibbonMode`（SARibbonBar.h:211，值 MinimumRibbonMode/NormalRibbonMode，Q_ENUM 在 :216），另有位标志 `SARibbonBar::RibbonStyleFlag`（SARibbonBar.h:189，Q_ENUM :204，`Q_DECLARE_FLAGS(RibbonStyles,...)` :205 + `Q_FLAG` :206）。
- 原盘点命令 `git grep -n "^enum class\|	enum class" src/widgets --include="*.h"` **有两处错误**：`--include` 不是 git grep 的合法参数（那是 GNU grep 的）；pattern 只匹配 `enum class` 会漏掉全部普通 `enum`。修正为已验证命令：
  ```bash
  git grep -n -E "^\s*enum (class )?[A-Za-z]" -- "src/widgets/*.h"
  ```
  **命中数（评审轮3真跑修正）**：原始命中 **17 行**，其中 `SARibbonThemeManager.h:7` 的 `enum class SARibbonTheme;` 是**前置声明**须剔除，实得 **16 处枚举声明**（13 处普通 enum + 3 处 enum class；原稿"12 处普通 enum"计数有误）。git pathspec 的 `*` 跨目录匹配，`colorWidgets/` 子目录一并命中（已实测）。执行结果整理成**附录 A：公共枚举全量盘点与去向**（16 项，含行号/Q_ENUM/处置），执行时以附录 A 为准，勿再口头盘点。

**操作**：
1. 新建 `src/core/global/SARibbonEnums.h`（命名空间 `SARibbon::Core` 内，或维持全局命名空间以兼容 2.x 用户——**决策：维持全局命名空间**，避免 3.0 用户 `SARibbonTheme` 等全部改名；在 NOTES.md 记录此决策及理由）：
   - move：`SARibbonGlobal.h:197` `SARibbonAlignment`（enum class，Q_DECLARE_METATYPE 在 :203）、`:220` `SARibbonTheme`（enum class，Q_DECLARE_METATYPE 在 :233）、`:245` `SARibbonMainWindowStyleFlag`（enum class : int，Q_DECLARE_FLAGS/Q_DECLARE_OPERATORS_FOR_FLAGS 紧随其后）——三者均为**自由枚举，无 Q_ENUM 耦合，可干净 move**（连同各自的 Q_DECLARE_METATYPE/Q_DECLARE_FLAGS 一起搬，widgets 侧不得重复声明）。
   - move：`SARibbonPanelItem.h:36-42` 的 `RowProportion`（**提升为自由枚举** `SARibbonRowProportion`，**放 `namespace SARibbon::Core`**——它是 core 新增类型名，与 v2 §3.5 的命名空间决策一致；**不放全局命名空间**，否则 `None/Large/Medium/Small` 四个枚举符泄漏到全局（X11 的 `None` 是宏，Linux 下与 X11 头同用即编译灾难）。**必须保持 unscoped enum（非 enum class）**：`SARibbonCustomizeWidget.cpp:52` 的 `QString::number(d.actionRowProportionValue)` 依赖枚举到 int 的隐式转换，enum class 会破坏该调用点。
     **兼容机制（评审轮3修正——仅类型别名不够，照抄旧稿会编译失败）**：unscoped enum 的枚举符属于其**声明所在命名空间**而非枚举类型本身，`using RowProportion = SARibbon::Core::SARibbonRowProportion;` 只引入类型名，存量的类限定访问 `SARibbonPanelItem::None/Large`（h:39 与 h:151 默认参数、SARibbonCustomizeData.cpp:12/19、SARibbonPanel.cpp:789、SARibbonPanelLayout.cpp:744-745/855/903 等）**不会**随之解析。必须在 `SARibbonPanelItem` 类内补类型别名 + 4 条枚举符 using 声明：
     ```cpp
     using RowProportion = SARibbon::Core::SARibbonRowProportion;
     using SARibbon::Core::None;  using SARibbon::Core::Large;
     using SARibbon::Core::Medium;  using SARibbon::Core::Small;
     ```
     （using 声明把命名空间成员引入类作用域，合法 C++；类无 Q_OBJECT，moc 不受影响。已核实全仓 RowProportion 枚举符访问均为 `SARibbonPanelItem::` 类限定或类内裸名，二者在该机制下均保持可编译。）已核实安全：`SARibbonPanelItem` 无 Q_OBJECT/Q_ENUM，`RowProportion` 不参与任何 Q_PROPERTY。注意其值序为 None/Large/Medium/Small，且**构造函数默认值是 Large 而非 None**（SARibbonPanelItem.cpp:15），契约基类字段默认值（v2 §3.4.1 草案为 None）与之不一致——搬移时以 2.x 实际行为为准，所有创建点必须显式赋值（现 `createItem` 已显式赋值，SARibbonPanelLayout.cpp:778），并把契约字段默认值对齐为 Large 或在 NOTES.md 记录差异及理由。**绑定波及（轮3新增）**：`sip/SARibbonPanelItem.sip:10`、`pyqt6/sip/SARibbonPanelItem.sip:10`（类内 `enum RowProportion` 声明）与 `pyside6/typesystem_saribbon.xml:46`（`<enum-type name="RowProportion"/>`）受别名化影响——绑定文件正式适配与冒烟验证**均归计划 03 S3**（sip 冒烟在 01→03 窗口不可行，见 S1.1-4 的 round3 终审裁决；RowProportion 三文件已列入 03 S3.2-1"枚举别名化适配"承接清单），本计划只把别名化波及面记 NOTES.md，不动绑定文件。
   - move（**有 Q_ENUM/Q_PROPERTY 耦合，按 S1.1 策略执行**）：`SARibbonPanel::PanelLayoutMode`（SARibbonPanel.h:115-157，Q_ENUM :158，值 ThreeRowMode/TwoRowMode/SingleRowMode）、`SARibbonBar::RibbonMode`、`SARibbonBar::RibbonStyleFlag`、`SARibbonToolButton::RibbonButtonType`（清单与理由见附录 A）。
   - **不动**（widgets 专属，见附录 A 理由列）：`SARibbonActionsManager::ActionTag`、`SARibbonColorToolButton::ColorStyle`、`SARibbonCustomizeWidget::RibbonTreeShowType`/`ItemRole`、`SARibbonGalleryGroup::GalleryGroupStyle`/`DisplayRow`、`SAColorToolButton::ColorToolButtonStyle`。
   - `SARibbonCustomizeData::ActionType`（SARibbonCustomizeData.h:33-51）在 **S4.2** 随 data/ 下沉，本步不动。
   - 属性名字符串常量：move `SARibbonPanelItem.h:59-60` 的 `SA_ActionPropertyName_RowProportion`（值 `"_sa_RowProportion"`，已核实）及同族的 `SA_ActionPropertyName_ToolButtonPopupMode`（:62-63，值 `"_sa_ToolButtonPopupMode"`）、`SA_ActionPropertyName_ToolButtonStyle`（:65-66，值 `"_sa_ToolButtonStyle"`）到 `SARibbonEnums.h`（v2 §3.5）。三个宏均带 `#ifndef` 守卫，字符串值一律不变。
2. **S1.1 嵌套 Q_ENUM 的下沉策略（新增，执行 agent 必须照做）**：`PanelLayoutMode/RibbonMode/RibbonStyleFlag/RibbonButtonType` 是 widget 类内嵌 Q_ENUM，且 `RibbonStyles` 被 `Q_PROPERTY(RibbonStyles ribbonStyle ...)`（SARibbonBar.h:176）、`PanelLayoutMode` 被 `Q_PROPERTY(SARibbonPanel::PanelLayoutMode panelLayoutMode ...)`（SARibbonBar.h:186）引用。**Q_ENUM 不能作用于 using 别名**，因此"core 定义自由枚举 + 类内留别名"会导致类内 Q_ENUM 必须删除，元对象枚举表（`QMetaEnum::fromType<SARibbonBar::RibbonMode>()` 等）随之失效。执行策略：
   1. core 中定义自由枚举（如 `SARibbonPanelLayoutMode`），值序与 2.x 完全一致；**命名空间（轮3明确）**：这些提升名都是 core 新增类型名（2.x 拼写 `SARibbonBar::RibbonMode` 等经类内别名保留），按 v2 §3.5 决策放 `namespace SARibbon::Core`，不放全局；
   2. widget 类内保留 `using PanelLayoutMode = SARibbonPanelLayoutMode;`（Q_PROPERTY 的类型经别名解析仍可编译，moc 按 C++ 类型解析）；
   3. 删除类内 Q_ENUM，在 core 头对自由枚举补 `Q_DECLARE_METATYPE`；若需保留反射能力，core 头内用 `Q_NAMESPACE` + `Q_ENUM_NS`（core 允许，QObject 属 QtCore）；
   4. 提交前全仓 grep `QMetaEnum\|fromType<` 确认无消费者（2.9.5 基线已核实 src/ 内无 QMetaEnum 使用）。**本步验证降级为 C++ 侧（round3 终审裁决，03-dryrun 体系建议 2 采纳）**：widgets 全量编译 + ctest == N₀ 即为通过判据；**sip/PyQt 构建冒烟在本计划执行窗口不可行、延迟至计划 03 S3**——计划 01 目录搬移后，三轨绑定的 include-dirs/源清单全部指向已不存在的 `src/SARibbonBar` 旧路径，且转发头的 `<SARibbonCore/...>` include 在绑定构建场景无解析（计划 03 S3.0 才建镜像），绑定整体不可构建，02 执行期强跑 sip 冒烟必然失败且无法归因。绑定侧验证（含"属性类型经别名对外仍显示为原枚举名"的核对）由计划 03 S3.2/S3.3 显式承接（承接清单见 03 S3.2-1"枚举别名化适配"条）；本步的降级决定与理由记 NOTES.md（B13）；
   5. `RibbonStyleFlag` 额外携带 `Q_DECLARE_FLAGS(RibbonStyles,...)`（h:205，类内）/`Q_FLAG(RibbonStyles)`（h:206，随 Q_ENUM 一并删除——Q_FLAG 同样不能作用于别名）/`Q_DECLARE_OPERATORS_FOR_FLAGS(SARibbonBar::RibbonStyles)`（**轮3补锚点：SARibbonBar.h:704，类外文件尾**，须删除并改由 core 对提升后的 flags 类型重新声明）：flags 声明随枚举入 core（core 侧自由形式 `Q_DECLARE_FLAGS(SARibbonRibbonStyles, SARibbonRibbonStyleFlag)` + `Q_DECLARE_OPERATORS_FOR_FLAGS(SARibbon::Core::SARibbonRibbonStyles)`），类内 `using RibbonStyles = SARibbon::Core::SARibbonRibbonStyles;` 同法处理（命名在 NOTES.md 定稿；`Q_PROPERTY(RibbonStyles ribbonStyle ...)` h:176 经别名解析仍可编译）。
   以上任何一步造成 moc/编译失败且无法在"不改行为"前提下解决时，**允许把这四个枚举留在 widgets 原地**（core 引擎用 int/独立 core 枚举作 Input），在 NOTES.md 记录降级原因——枚举下沉不是 M1 验收硬门（引擎提取才是）。
3. `SARibbonUtil` 拆分：**逐函数去向已核实并固化在附录 B**（轮3计数修正：13 个声明/12 个函数名——**core 8 个声明**（7 个名，scaleSizeByHeight 含 2 重载）/ **留 widgets 3 个**（widgetDevicePixelRatio + QSS 相关的 replaceQssTokens/getBuiltInRibbonThemeQss）/ **特殊处理 2 个**（saIsRTL/saMirrorX）；原稿"core 7/widgets 3/特殊 2/QSS 2"合计 14 且 QSS 两项与 widgets 三项重复计数），按附录 B 执行 `git mv` + 拆分；碰 widget 的（`widgetDevicePixelRatio`）留 widgets（可挪到新文件 `src/widgets/SARibbonWidgetUtil.h` 或原地保留）。**特别注意** `saIsRTL()` 现实现用 `QApplication::layoutDirection()`（SARibbonUtil.cpp:307，QtWidgets 类）——core 化需改用 `QGuiApplication::layoutDirection()`（行为等价，QtGui）或留 widgets；三个布局引擎一律**不得**调用它，RTL 必须是引擎入参（见 S4.1/S5）。
4. `src/widgets/SARibbonGlobal.h`（转发头）追加 `#include <SARibbonCore/SARibbonEnums.h>`（**轮3修正：同步后的平铺路径，与下条第 5 点决策一致**——勿写 `<SARibbonCore/global/SARibbonEnums.h>`：构建树内跨模块 include 经同步目录 `build/include/` 解析，那里没有 `global/` 子层级，写了会断），保证 2.x 用户 `#include "SARibbonGlobal.h"` 拿到全部枚举。
5. 同步头：`sa_sync_include` 会把 `src/core/global/SARibbonEnums.h` 同步到 `build/include/SARibbonCore/SARibbonEnums.h`——**决策：core 头的同步目录去掉子目录层级**，对外 include 形如 `#include <SARibbonCore/SARibbonEnums.h>`。源码树内部子目录只是物理组织。**执行注意（轮3新增）**：计划 01 S5.4 交付的 `sa_sync_include(<target> <模块名>)` 签名**没有平铺参数**，其既定行为是"保持相对子目录"（widgets 的 `colorWidgets/` 需要保留层级）——因此本步需给 `sa_sync_include` **增加 `FLATTEN` 选项**（CMake 函数小改：FLATTEN 时把头文件拍平复制到 `include/<模块名>/` 根下；仅 core 调用传入，widgets 调用不变），这是对计划 01 产物的构建脚本改动，随 S1 一起做、进 S1 提交，并在 P1 复核时知会。

**验证**：构建绿 + ctest == N₀ + 纯净扫描绿。Amalgamate 重跑（core 头并入单文件，计划 01 S8 模式）。附加验证：`SARibbonQuickAccessCustomizeTest`、`SARibbonUtilTest` 重点跑（属性名宏与 Util 拆分的直接消费者）。

**提交**：`重构：公共枚举与属性名常量下沉 core/global`

### S2 theme/ 子系统（§3.2）

**S2.0 实况修正（评审轮1核实，执行前必读）**——2.9.5 的主题代码与 v2/本文旧稿的想象有出入：
- **没有 `SARibbonThemeManager` 类，没有单例，没有 `themeChanged` 信号，没有 `isDarkTheme()` 函数**。`SARibbonThemeManager.h`（全文 23 行）只是 `namespace SA` 里两个自由函数：
  `SA::applyRibbonTheme(QWidget* w, SARibbonBar* bar, SARibbonTheme theme)`（h:15）与
  `SA::applyRibbonTheme(QWidget* w, SARibbonBar* bar, SARibbonTheme theme, const SARibbonThemePalette& palette)`（h:18-19）。
- 主题**状态**目前分散在每个窗口对象上：`SARibbonMainWindow::ribbonTheme`（Q_PROPERTY h:111，getter h:146，信号 `ribbonThemeChanged` h:201）与 `SARibbonWidget::ribbonTheme`（h:20/33/55）。**没有全局"当前主题"持有者**，`SARibbonThemeData` 单例是 3.0 新引入，不是"承接现有状态"。
- 明暗判断现状：只有 `SA::SARibbonThemePalette::isDark()`（ThemePalette.h:157，按 palette 的 JSON `isDark` 字段）。若 core 需要"按 `SARibbonTheme` 枚举判明暗"的函数，那是**新增 API**，实现必须由既有数据推导（如 S2.1-2 清单中的主题静态表），并在 NOTES.md 记录推导依据。
- `SARibbonThemeManager.cpp`（350 行）内真正可下沉的"主题数据"是 5 个静态表 + 2 个高亮 lambda（详见 S2.1 清单）；QSS 加载/渲染部分（`loadResourceText` cpp:149、`themeToTemplatePath` cpp:193、`themeToPalettePath` cpp:228、两个 `applyRibbonTheme` 重载）**全部留 widgets**。
- `SA::SARibbonThemePalette`（ThemePalette.h:126-176，.cpp 347 行）**已核实无任何 widget 依赖**（仅 QColor/QHash/QString/QByteArray/QJsonDocument/QFile，QtCore+QtGui），可直接 `git mv` 入 core，命名空间 `SA` 保持不变。

**S2.1 数据层下沉**：
1. 新建 `src/core/theme/SARibbonThemeData.h/.cpp`：QObject；持 `SARibbonTheme` + `SARibbonThemePalette`；信号声明在 `Q_SIGNALS:` 段（**注意拼写：`Q_SIGNALS`，不是 `Q_SIGNAL`**，本项目禁用 `signals` 关键字，AGENTS.md）：`themeChanged(SARibbonTheme)`、`paletteChanged()`；setter 触发信号（v2 §3.2-1）。
2. 把 `SARibbonThemeManager.cpp` 的静态主题数据表 move 进 core（**纯 move，值不改**）：
   - `s_themeMargins`（cpp:24，theme→QMargins 映射）；
   - `s_csDarkerHighlight` / `s_csVibrantHighlight`（cpp:38/43，`QColor(const QColor&)` 纯函数 lambda，依赖 `SA::makeColorVibrant`——该函数随 S1 已入 core）；
   - `s_themeContextHighlights`（cpp:48）、`s_themeContextColorLists`（cpp:62）、`s_themeBaselineColors`（cpp:76）。
   注意：`FpContextCategoryHighlight` 类型别名现定义于 `SARibbonBar.h:223`（widgets 类内 using；消费点 `setContextCategoryColorHighLight` h:529、PrivateData 字段 SARibbonBar.cpp:100）。数据表入 core 需先把该 `using` 提升到 core（`std::function<QColor(const QColor&)>`，仅依赖 QColor，无 widget 依赖，可安全 move；落点建议 `SARibbon::Core`，`SARibbonBar` 类内留 `using FpContextCategoryHighlight = SARibbon::Core::SARibbonFpContextCategoryHighlight;` 别名转发以保公共 API 拼写不变，命名 NOTES.md 定稿）。
3. `SARibbonThemePalette.*` → `git mv` 到 `src/core/theme/`（已核实无 widget 依赖；`replaceQssTokens` 虽签名只碰 QString+palette，但按 v2 §3.2 "QSS 模板渲染留 widgets" 的决策**留在 widgets** 的 SARibbonUtil 中，core 化 palette 后 widgets 侧照常可用）。
4. widgets 的 `SA::applyRibbonTheme` **两个重载签名与行为完全不变**（它是公共 API），实现改为"写 `SARibbonThemeData` → 读回数据刷 QSS/设置 bar 属性"或维持现状仅数据来源改为 core 表——**M1 期间允许先只做数据表 move、不改函数体**，信号驱动重构可留到 QML 桥（计划 04）真正需要时，避免为纯 widgets 场景引入无消费者的信号链（在 NOTES.md 记录取舍）。
5. ThemeData 需要单例访问（双前端共享一个信号源）：`SARibbonThemeData::instance()`。**选型已定案（评审轮2，依据 KDDW 2.0.1 三个单例的实证）：Meyers 函数内静态对象**，不挂 QCoreApplication，不用 Q_GLOBAL_STATIC：
   ```cpp
   // src/core/theme/SARibbonThemeData.cpp
   SARibbonThemeData* SARibbonThemeData::instance()
   {
       static SARibbonThemeData s_themeData;  // C++11 magic static，线程安全初始化
       return &s_themeData;
   }
   ```
   理由与 KDDW 证据：① KDDW 三个 core 单例无一挂 app 对象、无一用 Q_GLOBAL_STATIC——Config 就是同款 Meyers 静态对象（Config.cpp:76-78），DockRegistry 源码注释明言"please don't change this to be deleted at static dtor time with Q_GLOBAL_STATIC"（DockRegistry.cpp:82-84，它为 LSAN 采用"空时自删"，ThemeData 常驻且极小，不需要）；② 挂 `QCoreApplication::instance()` 有硬伤：`instance()` 可能在 app 创建前被调（单测/静态初始化期），parent 为 null 且永不补挂，行为随调用时序漂移。**注意事项（写进代码注释与 NOTES.md）**：析构函数不得发射信号、不得触碰 QCoreApplication（静态析构期 app 已亡）；监听方 widgets/QML 桥先亡无风险（QObject 析构自动断连）；对象线程亲和为主线程（首次调用应在主线程，跨线程只读数据字段可以，连接信号自动按接收者线程投递）。

**验证**：构建绿 + ctest == N₀（`SARibbonThemeAutoSwitchTest`、`ThemeCoverageTest`、`SARibbonUtilTest` 重点跑——暗色自动切换依赖 `SA::isOperatingSystemInDarkMode`/`isEnableSystemDarkModeAutoSwitch`，其调用点在 SARibbonMainWindow.cpp:242、SARibbonWidget.cpp:55，函数去向见附录 B）+ 6 张主题截图对比。

**提交**：`重构：主题数据层下沉 core（ThemeData/ThemePalette/静态主题表）`

### S3 metrics/ 子系统（§3.3）

**S3.0 实况修正（评审轮1核实，执行前必读）**：
- 六个度量函数是 `SARibbonBarLayout` 的**公共 API**（h:66/89/92/96/108/114），不是私有函数；类名、签名、返回值在 3.0 必须原样保留（v2 §4.4 兼容承诺），只把实现体换成转发。
- 六个函数中**五个只是一行转发进 PrivateData**（实现体都在 `SARibbonBarLayout.cpp` 的 `PrivateData` 类内，cpp:17-560）：
  | 公共函数 | 转发目标 | 纯度 |
  |---|---|---|
  | `tabBarHeight()` cpp:925 | `d_ptr->getActualTabBarHeight()` cpp:303-310（userDef 覆盖优先，否则字段 `tabBarHeight{28}` cpp:24） | 纯字段读 |
  | `titleBarHeight()` cpp:959 | `d_ptr->getActualTitleBarHeight()` cpp:283-290（字段 `titleBarHeight{30}` cpp:23） | 纯字段读 |
  | `categoryHeight()` cpp:995 | `d_ptr->getActualCategoryHeight()` cpp:323-330（字段 `categoryHeight{60}` cpp:26） | 纯字段读 |
  | `panelTitleHeight()` cpp:1027 | 直接读 `d_ptr->panelTitleHeight`（字段默认 15，cpp:25） | 纯字段读 |
  | `minimumModeMainBarHeight()` cpp:891 / `normalModeMainBarHeight()` cpp:909 | `d_ptr->minimumModeMainBarHeight()` cpp:532-539 / `normalModeMainBarHeight()` cpp:552-559 → 都调 `static calcMainBarHeight(...)` cpp:466-483 | **纯计算**（入参全是 int/bool/RibbonMode） |
  | `calcMinTabBarWidth()` cpp:819-827 | 无 PrivateData 中转，**直接触碰控件**：`tabBar->sizeHint().width() + tabMargin().left()+right()` | **非纯**，见下 |
- 字段默认值（`titleBarHeight{30}`/`tabBarHeight{28}`/`categoryHeight{60}`）只是初始值，运行期会被 `estimateSizeHint()`（cpp:341-347，由 `resetSize()` cpp:494-519 调用）用下面三个推导函数覆写——**真正要进 `SARibbonMetrics` 的是这三个推导公式**：
  - `calcDefaultTabBarHeight()` cpp:378-391：依赖 `ribbonBar->style()->pixelMetric(PM_TabBarBaseHeight/PM_TabBarTabHSpace/PM_TabBarTabOverlap)`（经 `systemTabBarHeight()` cpp:360-365）+ `ribbonBar->fontMetrics().lineSpacing()`。**QStyle 属 QtWidgets，core 不可调用** → 公式可下沉，但三个 pixelMetric 值必须由适配器算好作为 `SARibbonMetrics` 输入字段传入。
  - `calcDefaultTitleBarHeight()` cpp:404-413：依赖 `style()->pixelMetric(PM_TitleBarHeight)` + `fontMetrics().height()*1.8`，同上处理。
  - `calcCategoryHeight()` cpp:430-443：依赖 `fontMetrics().lineSpacing()` + `ribbonBar->isThreeRowStyle()/isSingleRowStyle()`（可由 `RibbonStyles` 值代替，`SARibbonBar::isThreeRowStyle(s)` 本身是静态纯函数，Bar.h 已声明）+ `panelTitleHeight` 字段 → 入参化后纯。
- `calcMinTabBarWidth()` **不能纯 move**：`SARibbonTabBar::sizeHint()`/`tabMargin()` 是控件查询。处理方式二选一（NOTES.md 记录）：(a) 留在 `SARibbonBarLayout`（v2 §3.4.4 表格允许，它本质是"引擎引用"的输入采集）；(b) metrics 提供 `minTabBarWidth(int tabBarSizeHintWidth, const QMargins& tabMargin)` 纯函数，适配器采集入参。
- `SARibbonMetrics` 的输入**不止 v2 §3.3 示例的 QFontMetrics + devicePixelRatio**，按上述实况至少还需：`pmTabBarBaseHeight/pmTabBarTabHSpace/pmTabBarTabOverlap/pmTitleBarHeight`（style 派生 int）、`userDef*` 三个可选覆盖（对应 PrivateData 的 `std::unique_ptr<int>` 语义，cpp:32-34，可用 `std::optional<int>` 或 -1 哨兵，选型记 NOTES.md）。
- **"适配器采集平台数值 → 引擎只读纯数据"模式有 KDDW 同构实证（评审轮2）**：KDDW core 全程无 QStyle，其"平台像素值"（separatorThickness、绝对 min/max 尺寸）由前端经 Config 门面写入引擎静态量（Config.cpp:156-174, 235-256 → Item_p.h:211-213），DPI/屏幕尺寸则抽象为 core `Screen` 接口由前端实现（Screen_p.h:41 `devicePixelRatio()` 纯虚）。SARibbon 的 Metrics 实例字段方案与之同构且**更优**：每 layout 实例一份字段，天然多窗口隔离，且无 KDDW"必须在创建任何窗口前设置全局静态量"的启动时序约束（Config.h:60-62）——`userDef*` 覆盖走实例字段而非全局 Config 正是对这一点的规避，执行时不得把任何度量常量改回全局静态。

**S3.1 建类**：新建 `src/core/metrics/SARibbonMetrics.h/.cpp`（同步目录平铺为 `SARibbonCore/SARibbonMetrics.h`）：
- 输入：`QFontMetrics`（构造传入）、`devicePixelRatio`、上述 style 派生 pixelMetric 字段；core 内**不查询任何控件**（v2 要点）。
- move 实现体（按 S3.0 表格）：`calcMainBarHeight`（静态纯函数，直接 move）、`calcDefaultTabBarHeight`/`calcDefaultTitleBarHeight`/`calcCategoryHeight`（入参化后 move，**公式一行不改**）、`getActual*` 三个 userDef 覆盖逻辑（move 为字段读取方法）；`panelTitleHeight` 成为普通字段。公共六函数在 widgets 侧退化为 `return mMetrics.xxx();` 一行转发（**保留公共函数名与签名**，`doLayout` 等调用点零改动）。
- 收口散落常量（已核实的实名与默认值，直接采用，勿再凭想象 grep）：
  | 常量 | 实名与出处 | 默认值 |
  |---|---|---|
  | Panel 项间距 | `QLayout::spacing()`，由 `SARibbonPanel` 构造时 `lay->setSpacing(2)`（SARibbonPanel.cpp:85），用户可经 `SARibbonPanel::setSpacing`（cpp:1536）改 | **2**（注意：v2 §3.3 示例写 spacing=1，与实测不符，以 2 为准并在度量对照表标注） |
  | Panel 行间距 | 局部常量 `spacingRow = 1`（SARibbonPanelLayout.cpp:806，硬编码） | 1 |
  | Panel 标题高 | `mTitleHeight{15}`（SARibbonPanelLayout.h:180） | 15 |
  | Panel 标题间隔 | `mTitleSpace{2}`（h:181） | 2 |
  | 大/小图标尺寸 | `mLargeToolButtonIconSize{32,32}` / `mSmallToolButtonIconSize{22,22}`（h:176-177） | 32/22 |
  | largeHeight/smallHeight | `updateGeomArray` 内联计算（cpp:821/831，公式见附录 C） | 派生 |
  | BarLayout 侧 | PrivateData 字段 cpp:23-26（30/28/15/60）、`minWidth{500}`/`maxMinWidth{1000→屏宽0.8}`（cpp:28-29） | 见表 |
  校验命令（修正版，原稿 `--include` 语法对 git grep 无效、`mTabBarHeight`/`rowSpacing` 实名不存在）：
  ```bash
  git grep -n -E "setSpacing\(|mTitleHeight|mTitleSpace|spacingRow|titleSpace" -- "src/widgets/*.cpp" "src/widgets/*.h"
  git grep -n -E "tabBarHeight|titleBarHeight|categoryHeight|panelTitleHeight" -- src/widgets/SARibbonBarLayout.cpp
  ```

**S3.2 widgets 接线**：`SARibbonBarLayout` 持 `SARibbonMetrics` 实例；六个公共度量函数退化为一行转发（见 S3.1）；用户 setter 实名已核实，**存在两级**，都要保留原签名、改写 `mMetrics` 字段：
- layout 级：`SARibbonBarLayout::setTabBarHeight(int)`（h:98 → d_ptr cpp:243-250）、`setTitleBarHeight(int)`（h:104 → cpp:223-230）、`setCategoryHeight(int)`（h:110 → cpp:263-270）、`setPanelTitleHeight(int)`（h:116，直接写字段 cpp:1043-1046）；
- bar 级：`SARibbonBar::setTabBarHeight(int h, bool resizeByNow)`（SARibbonBar.cpp:1689）、`setTitleBarHeight(int,bool)`（cpp:1741）、`setCategoryHeight(int,bool)`（cpp:1785），内部调 layout setter 后按需 `resetSize()`。
度量的重算触发点已核实：`resetSize()`（cpp:1377-1380 → d_ptr->resetSize cpp:494-519，内含 `estimateSizeHint()` + `calcMainBarHeight` + `ribbonBar->setFixedHeight`）由 `SARibbonBar.cpp` 9 处调用（:261 setMinimumMode / :276 setNormalMode / :1689 / :1741 / :1785 三个 bar 级 setter / :2431 / :2658 updateRibbonGeometry / :2706 / :4118 `changeEvent` 的 `QEvent::FontChange` 分支）——**触发点一律不动**（字体变化重建 metrics 即发生在 cpp:4118 这条既有路径上），只把 resetSize 内部的公式数据源换成 metrics 实例；`setFixedHeight` 等控件操作留 widgets。PrivateData 构造函数里的 `QGuiApplication::primaryScreen()` 屏宽查询（cpp:55-65）留 widgets。

**验证**：构建绿 + ctest == N₀；**度量对照表**：写临时 main 或在 MainWindowExample 加 debug 输出，导出 2.9.5 与 3.0 各度量值（tabBarHeight/titleBarHeight/categoryHeight/panelTitleHeight/normalModeMainBarHeight/minimumModeMainBarHeight/calcMinTabBarWidth）对照——**必须逐项相等**，作为 `docs/3.0/metrics-comparison.md` 存档（v2 R3 验收物）。对照须在**同一字体、同一 style（QStyle pixelMetric 影响默认值）、亮暗主题各一次**的条件下录制。

**提交**（两笔）：`重构：新增 SARibbonMetrics 度量收口类`；`重构：SARibbonBarLayout 度量委托 metrics`

### S4 contract/ + data/ + factory/（§3.4.1、§3.5、§3.6）

**S4.1 契约接口**：新建 `src/core/contract/SARibbonAbstractLayoutItem.h`、`SARibbonAbstractLayoutHost.h`（内容以 v2 §3.4.1 代码块为底，**但按下列评审轮1核实的缺口修订后再落地**；同步目录平铺：`SARibbonCore/SARibbonAbstractLayoutItem.h`）：
1. v2 草案的五个纯虚（sizeHint/minimumSizeHint/isHidden/expandingDirections/applyGeometry）**不足以承载现有算法**，实际代码还依赖（缺一条引擎就编不过或行为变化）：
   - `virtual int maximumWidth() const`：`recalcExpandGeomArray` 用 `item->widget()->maximumWidth()` 做列扩展上限（SARibbonPanelLayout.cpp:1238；死代码 `columnWidthInfo` cpp:1395 同）；
   - `virtual int stretchFactor() const`（默认 0）：列扩展加权来自 `SARibbonGallery::stretchFactor()`（cpp:1244-1246，issue #47 语义）——契约抽象为"item 的拉伸权重"，widgets 适配器对 Gallery 返回其 stretchFactor，其余返回 0，**引擎不得 qobject_cast Gallery**；
   - `isHidden()` 语义按布局不同（见 S5.1-1 修正）：Panel 侧 = action 不可见，Category 侧 = widget 隐藏——契约只约定"引擎跳过 isHidden()==true 的项"，具体语义由各前端 item 实现；
   - Category 侧每 item 有**两块几何**（panel 本体 + separatorWidget，SARibbonCategoryLayout.h:155-156 `mWillSetGeometry`/`mWillSetSeparatorGeometry`），契约单一 `geometry` 字段不够——为 Category 引擎定义 core 侧扩展结构（如 `SARibbonAbstractCategoryItem : SARibbonAbstractLayoutItem` 增加 `separatorGeometry` 字段与 `separatorHidden` 输出），不要塞进通用契约。
2. **命名冲突警示（必须规避）**：v2 草案把引擎回写字段命名为 `QRect geometry;`。双继承场景（`SARibbonPanelItem : QWidgetItem + 契约`）下，`QLayoutItem::geometry()` 是继承来的成员函数，与契约数据成员 `geometry` 同名——派生类中 `item->geometry` 将产生"成员函数 vs 数据成员"歧义，编译失败。契约字段改名（建议 `resultGeometry` 或 `itemGeometry`），v2 草案此处不可照抄。
3. 共享数据字段（rowIndex/columnIndex/isExpandItem/rowProportion）与 2.x `SARibbonPanelItem` 公有字段同名（PanelItem.h:51-57），搬入基类后删除派生类字段即可保持源码兼容——**类型注意（评审轮2修正，轮1"同名同型"不准确）**：2.x 的 `rowIndex` 是 **`short`**（PanelItem.h:51），契约基类定为 `int`（与 v2 草案一致，引擎内部计算全 int，避免 short/int 混用提升警告）；读/赋值场景隐式转换兼容，仅 `short*`/`short&` 取址绑定会断——**轮2已预验证**：`git grep -n "rowIndex" -- src/ tests/ example/ sip/ pyside6/ pyqt6/` 命中全部为赋值/读取（无取址用法），sip/pyside6 未以 short 暴露该字段；落地时按同命令复核一遍即可，NOTES.md 记录此微小类型差异；`itemWillSetGeometry`（h:53）是 2.x 公有字段，若引擎统一写契约字段，为兼容存量代码可在 `SARibbonPanelItem` 保留 `QRect& itemWillSetGeometry` 引用成员绑定到基类字段（构造时初始化），或在 NOTES.md 记录为 3.0 允许的源码破坏项（3.0 是大版本，二选一，定稿记 NOTES.md）。
4. Host 接口仅 `metrics()` 不够：引擎还需要 `isRTL`（现读 `SA::saIsRTL()`，其实现用 `QApplication::layoutDirection()`，SARibbonUtil.cpp:307，core 禁调）、contentsMargins、spacing（现读 `QLayout::spacing()`/`contentsMargins()`，cpp:804-805）。这些走引擎 `Input` 结构（值传参）而非 Host 虚接口，保持引擎无状态依赖。
5. **对照 KDDW 的设计说明（评审轮2新增，含最终契约代码块）**。KDDW 契约 `LayoutingGuest` 为 8 纯虚（minSize/maxSizeHint/setGeometry/setVisible/geometry/setHost/host/id）+ 2 默认虚（freed/debugName）+ 3 个 KDBindings 信号（LayoutingGuest_p.h:31-70）。SARibbon 契约与之的关键分叉及理由：
   - **不需要 `id()`/序列化**：KDDW 的 QString id 只服务 LayoutSaver 的 JSON 存恢（Item.cpp:242-243, 252-257）；SARibbon 3.0 无布局保存需求（CustomizeData 的 keyValue 是数据层字符串，不是布局 id）。identity = item 指针（单次 layout() 调用期 + 引擎缓存 key），悬垂防护不抄 KDDW 的 ObjectGuard/beingDestroyed 重方案（Item.cpp:4122-4147），改用 2.x 既有不变量：takeAt 移除缓存条目（cpp:302-303）+ invalidate() 全清（cpp:367-368），见 S5.1-2。
   - **不需要 `setHost/host()`**：KDDW guest 会在 host 间迁移（浮动窗口）；SARibbon item 不跨 panel 迁移。
   - **`isHidden()` 只读、不做 KDDW 式 `setVisible` 回写**：KDDW 引擎常驻且有 placeholder 语义（关闭的 dock 保留不可见 Item 待原位恢复，Item.cpp:874-882），可见性权威在引擎，故必须回写；SARibbon 引擎是瞬态纯函数（layout() 一进一出），可见性权威在 widget/action 侧，引擎的显隐决策（如 Category separator）经 Result 标志流向适配器批处理（S6-3），信息流等价而纯净性更强。
   - **新增 `debugName()`（默认空实现）**：抄 KDDW（LayoutingGuest_p.h:50-53，Item.cpp:884-898 用它做诊断名、dumpLayout 打名字不打地址）；widgets 适配器返回 `widget()->objectName()`，黄金测试失败诊断可读（S5.0-2）。
   - **信号面为零**：KDDW 用 KDBindings 信号做约束变化通知（layoutInvalidated→重拉 min/max，Item.cpp:212-213, 913-931），因为它要服务非 Qt 前端；SARibbon 双前端都在 Qt 内，约束变化由 QLayout::invalidate 驱动重入 layout()，契约不需要任何信号/回调注册。
   - **最终契约代码块（S4.1 落地以此为准，整合 v2 §3.4.1 草案 + 轮1缺口修订 + 轮2增补；round2 整合已把 v2 §3.4.1 代码块同步为本块——两处字段/方法名一致，本块仍是执行依据）**。**轮3修正两处照抄即翻车的细节**：① 导出宏实名是 **`SA_RIBBON_CORE_EXPORT`**（计划 01 S6.3 的宏映射表），v2/旧稿代码块写的 `SARIBBON_CORE_EXPORT` 是误名（宏未定义时该记号会被当类型名导致编译错），下面代码块已更正，v2 §3.4.1/§3.6 的同名误写已由轮3终审同步修正；② **头文件自足性**——Item 头需 `#include <QRect>` `<QSize>` `<QString>` `<Qt>`（Qt::Orientations）与 `<SARibbonCore/SARibbonEnums.h>`（`SARibbonRowProportion`，在 `namespace SARibbon::Core`）及本模块全局头（SA_RIBBON_CORE_EXPORT 定义处）；Host 头对 `SARibbonMetrics` 用**前置声明**即可（`namespace SARibbon::Core { class SARibbonMetrics; }`，const 引用返回不需完整类型）；两类的虚析构在各自 .cpp 中定义（`= default;`）。`namespace SARibbon::Core {}` 嵌套定义语法需 C++17——计划 01 已把三模块统一 `cxx_std_17`（PUBLIC 传播），满足。
   ```cpp
   // src/core/contract/SARibbonAbstractLayoutItem.h
   namespace SARibbon::Core {
   class SA_RIBBON_CORE_EXPORT SARibbonAbstractLayoutItem
   {
   public:
       virtual ~SARibbonAbstractLayoutItem();
       // —— 前端提供（引擎输入，全纯虚）——
       virtual QSize sizeHint() const = 0;
       virtual QSize minimumSizeHint() const = 0;
       virtual bool isHidden() const = 0;   // Panel 侧=action 不可见；Category 侧=QWidgetItem::isEmpty 语义（S5.1-1/S6-1，不得统一）
       virtual Qt::Orientations expandingDirections() const = 0;
       // —— 前端提供（带默认实现，按需覆写）——
       virtual int maximumWidth() const { return 16777215; }  // =QWIDGETSIZE_MAX 值（该宏属 QtWidgets 头，core 禁 include，用字面量；KDDW 同款做法 Platform.h:342）；默认即 QWidget::maximumWidth() 缺省值，行为等价（cpp:1238 消费）
       virtual int stretchFactor() const { return 0; }               // 仅 Gallery 适配器覆写（cpp:1244-1246，引擎禁 qobject_cast）
       virtual QString debugName() const { return {}; }              // 诊断名（KDDW LayoutingGuest_p.h:50 同款），测试输出用
       // —— 前端实现（引擎输出回写）——
       virtual void applyGeometry(const QRect& rect) = 0;  // widgets: widget()->setGeometry; qml: QQuickItem 几何
       // —— 引擎回写字段（引擎填，双端读取；命名避开 QLayoutItem::geometry() 成员函数冲突，见上第 2 条）——
       int rowIndex = -1;                 // 2.x 为 short（PanelItem.h:51），契约定 int，见上第 3 条
       int columnIndex = -1;
       QRect resultGeometry;              // 对应 2.x itemWillSetGeometry / mWillSetGeometry
       bool isExpandItem = false;
       // —— 共享数据 ——
       SARibbonRowProportion rowProportion = SARibbonRowProportion::Large;  // 默认值对齐 2.x 构造行为（PanelItem.cpp:15），见 S1
   };
   // Category 专用扩展（panel 本体 + separator 双几何，S4.1-1）：
   class SA_RIBBON_CORE_EXPORT SARibbonAbstractCategoryItem : public SARibbonAbstractLayoutItem
   {
   public:
       QRect resultSeparatorGeometry;      // 对应 2.x mWillSetSeparatorGeometry（CategoryLayout.h:156）
       bool isSeparatorHidden = false;     // 引擎输出，适配器据此 hide/show separatorWidget（cpp:538-541 的应用侧）
   };
   }
   // src/core/contract/SARibbonAbstractLayoutHost.h
   namespace SARibbon::Core {
   class SA_RIBBON_CORE_EXPORT SARibbonAbstractLayoutHost
   {
   public:
       virtual ~SARibbonAbstractLayoutHost();
       virtual const SARibbonMetrics& metrics() const = 0;  // isRTL/margins/spacing 等一律走引擎 Input（S4.1-4），Host 仅此一个纯虚
   };
   }
   ```
   对照结论：KDDW Guest 契约 8 纯虚，SARibbon 采纳其"输入约束 + 几何回写"骨架，裁掉 host 迁移/序列化(id)/可见性回写(setVisible)三组（均无对应需求，理由见上），补 maximumWidth/stretchFactor/debugName 三个 SARibbon 算法实况所需（全部带默认实现，不加重实现负担）——最终纯虚仅 Item 5 个 + Host 1 个，**比 KDDW 更窄**，符合 v2 §3.4.1"接口保持最小"要点。

**S4.2 data/**：`SARibbonCustomizeData.h/.cpp` 已核实成员构成（h:21-204）：
- **纯数据部分（入 core）**：`ActionType` 枚举（h:33-51，16 个值 UnknowActionType=0 … ChangeQuickActionOrderActionType=15）+ 公有字段 `int indexValue`（h:159）、`QString keyValue`（h:176）、`QString categoryObjNameValue`（h:187）、`QString panelObjNameValue`（h:198）、`actionRowProportionValue`（h:200，类型改用 core 自由枚举 `SARibbonRowProportion`）+ 私有 `ActionType mType`（h:202）+ `isValid()`（cpp:68-71，纯：`actionType() != UnknowActionType`）。
- **含 widget 依赖部分（留 widgets）**：`mActionsManagerPointer`（h:203，`SARibbonActionsManager*`，**类中唯一指针成员**；无 QWidget*/QAction* 直接成员）、`actionManager()`（h:69）/`setActionsManager()`（h:72）存取器（**轮3补列**，与 manager 指针同去留）、`apply(SARibbonBar*)`（cpp:86 起，内部经 manager 取 QAction、操作 quickAccessBar）、全部 `make*CustomizeData` 静态工厂（共 **15** 个，其中带 `SARibbonActionsManager*` 签名的是 **6** 个而非旧稿的 8 个——**轮3已逐一清点**：makeAddAction h:83 / makeChangeActionOrder h:109 / makeRemoveAction h:119 / makeAddQuickAction h:125 / makeRemoveQuickAction h:128 / makeChangeQuickActionOrder h:132）、`isCanCustomize/setCanCustomize(QObject*)`（基于动态属性 `SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE`，QObject 属 QtCore 技术上可入 core，但语义绑定 widgets 类，建议留 widgets，NOTES.md 记录）。
- `simplify()`（cpp:926-1040）**轮3已核实为纯**：全程只读 `actionType()` 与 indexValue/keyValue/categoryObjNameValue/panelObjNameValue 五个数据字段，唯一辅助是文件级纯函数 `remove_indexs`（cpp:896-898 声明+定义），无任何 manager/widget 访问——**随数据层一并入 core，`remove_indexs` 同迁**（旧稿"待核实"就此了结，执行时可再跑 `sed -n '926,1040p'` 复核一遍）。
- 现头文件 include `SARibbonActionsManager.h` 与 `SARibbonPanel.h`（h:4-5）——core 版记录结构必须摆脱这两个 include（RowProportion 用 core 枚举，manager 指针留 widgets 壳类）。**拆分机制（轮3修正——旧稿"组合"不可行）**：`SARibbonCustomizeData` 的五个字段是**公有数据成员**，全仓直接字段访问经 grep 计 **161 处**（如 `SARibbonCustomizeWidget.cpp:52` 的 `d.actionRowProportionValue`、全部 make* 函数体、example/tests），若 widgets 类"包含（组合）"core 记录，`data.indexValue` 等既有写法全部编译失败。改为**公有继承**：core 定义记录基类（建议名 `SARibbon::Core::SARibbonCustomizeRecord`，命名 NOTES.md 定稿），持有 `ActionType` 枚举（**保留为记录类的类内 enum**——`SARibbonCustomizeData::ActionType`、`SARibbonCustomizeData::UnknowActionType` 等类限定拼写经继承自动可解析，零改名，无需自由枚举+别名的绕行）、五个公有字段、私有 `mType` + `actionType()` 存取、`isValid()`、`simplify()`；widgets 的 `SARibbonCustomizeData` **public 继承**该记录类，manager 指针/apply/make*/isCanCustomize 留在派生类，公共 API（含 Q_DECLARE_METATYPE h:205、typedef h:207）不变。

**S4.3 factory/**（D6/D7 允许降级）：已核实 `SARibbonElementFactory`（SARibbonElementFactory.h:42-83）共 **17 个虚 create 函数 + 虚析构**，全部返回 widgets 类型指针（createRibbonBar/TabBar/ApplicationButton/Category/ContextCategory/Panel/SeparatorWidget/Gallery/GalleryGroup/ToolButton/StackedWidget/ButtonGroupWidget(createButtonGroupWidget)/QuickAccessBar/SystemButtonBar(createWindowButtonGroup)/PanelOptionButton/TitleIconWidget/PanelLabel），入参为 `QWidget*` 或 `SARibbonBar*`/`SARibbonPanel*`；单例入口 `SARibbonElementManager::instance()`（SARibbonElementManager.h:57）+ 宏 `RibbonSubElementFactory`（h:69-70）。core 接口化只能返回 void*/不透明句柄，成本高且 3.0 无 QML 消费者——**确认执行降级路径**：只建 `src/core/factory/SARibbonElementFactoryInterface.h` 占位头（仅虚析构 + 注释），NOTES.md 记录"接口化推迟到 QML 需要时（D7 gate）"。**KDDW 成本实证（评审轮2，同时支撑 D8"推迟 core Action 抽象"）**：KDDW 为跨前端抽象一个 `Core::Action` 付出 core 142 行（Action.h 65 + Action.cpp 36 + Action_p.h 41）+ 三前端实现 264 行 ≈ **406 行基础设施，而 core 内消费点只有 2 处**（DockWidget.h:151/157 的 toggleAction/floatAction）——SARibbon 的 action 面（属性系统/`_sa_RowProportion` 动态属性/ActionsManager 分组语义）远大于此，D8 触发时的成本下限即 400+ 行 × 消费面放大；其完全体形态（factory 接口放 core、实现留前端、Config 可整体替换：ViewFactory.h:68-136）与本节降级路径方向一致，QML 需要时按附录 F 衔接。

**验证**：构建绿 + ctest == N₀ + 纯净扫描绿。

**提交**：`重构：core 新增契约接口、CustomizeData 纯数据与工厂占位`

### S5 PanelLayoutEngine 提取（§3.4.2，本计划最重的一步，严格两步走）

**S5.0 录制基线（先于一切改动）**：
1. 新建 `tests/core/` 目录骨架与 `tests/core/CMakeLists.txt`（独立小工程，链接 `SARibbon::Core` + `Qt::Test`，无 widget 依赖；**挂接点（轮3按计划01 S7 的 tests 层级修正）**：计划01 后 `tests/CMakeLists.txt` = `find_package(Qt... Test Core Gui Widgets)` + `add_subdirectory(widgets)`（26 个旧测试已迁 `tests/widgets/`），本步把该文件改为 `if(TARGET SARibbonWidgets) add_subdirectory(widgets) endif()` + `add_subdirectory(core)`——widgets 守卫是 S8-3/CI 在 core-only 下开 `SARIBBON_BUILD_TESTS=ON` 的前提（widgets 测试链不到不存在的 target），**CI 中最先运行**（v2 §6.3）。测试 main 用 `QTEST_MAIN`：不链 Widgets 时其实例化的是 QGuiApplication——引擎测试无需任何 widget 树，但 `SARibbonMetrics` 携 QFontMetrics、字体库访问要求 QGuiApplication 先行，配合 `QT_QPA_PLATFORM=offscreen` 即可无显示运行）。**tests/core 的每个测试必须设置 `LABELS core`（硬性要求，round3 终审收口）**——S9 与 03 S5-1 的 CI 过滤口径统一为 `ctest -L core`（LABELS），**不用 `-R core`**（现有 `add_saribbon_test` 以文件名注册测试名，如 `SARibbonUtilTest`，不含 "core" 字样，`-R core` 会零匹配且 ctest 照样退出 0 = 假绿）；只加 `core_` 名字前缀而不设 LABELS 不满足 S9 过滤，注册名前缀可作可选增强但不可替代 `set_tests_properties(... PROPERTIES LABELS core)`。
2. 写 `tests/core/layout_fixtures.h`：`struct PanelCase { 名字; 项列表(sizeHint,minSizeHint,hidden,expanding,maxWidth,stretchFactor,rowProportion); availableRect; contentsMargins; spacing; Input(mode,showPanelTitle,isRTL,titleTextWidth,optionBtnSize); 期望输出(每项 resultGeometry/rowIndex/columnIndex + titleGeometry/optionBtnGeometry + Result{sizeHint,columnCount,largeHeight}) }`（**轮3修正**：Input 去掉 `enableExpanding`——2.x 无此用户开关，扩展重算的触发条件是宽度比较式，属算法本体，见 S5.2-1）。
   **确定性关键**：fixture 的 sizeHint/minSizeHint/maxWidth 全部是**显式录入的 QSize/int 输入**，Step B 后的 core 测试用 FakeItem 直接喂这些值——**引擎级黄金测试完全不依赖字体与平台**（字体只影响 S5.0-3 录制工具产出的输入值，不影响"输入→输出"映射的确定性）。
   **fixture 四项补强（评审轮2，对照 KDDW 测试实践）**：
   - **失败诊断 dump**：提供 `tests/core/fixture_dump.h` helper——首个断言失败即打印整个 case 名 + Result 全字段 + 每项的输入（sizeHint/maxWidth/stretch/rowProportion/debugName）与实际输出（resultGeometry/rowIndex/columnIndex），对照 KDDW checkSanity 失败前自动 `dumpLayout()` 打全树的做法（Item.cpp:738-741, 821-842）；契约字段全 public，测试侧直接打印，core 不引入任何日志设施。
   - **退化输入 fixture 族**：仿 KDDW `tests/layouts/invalid*.json`（畸形真实场景录制回放，tst_docks.cpp:3464-3485）——空 items、零宽/负宽 availableRect、全隐藏项、min 总和超可用宽（溢出）、maxWidth<minimumSizeHint 的矛盾输入；先对 2.9.5 录制"不崩溃 + 实际行为"作黄金值（**只录不改**，2.x 若有未定义行为原样记录并在 NOTES.md 标注）。
   - **dpr 变体**：每个代表性 case 增加 devicePixelRatio=1.0 与 2.0 两份 Metrics 输入，断言取整确定性（对照 KDDW 把 DPI 数据化为 Screen 抽象输入、引擎不查屏，Screen_p.h:41）。
   - **录制 JSON 保留为可审查源**：S5.0-3 dump 工具产出的 JSON 入库到 `tests/core/fixtures/recorded/*.json`（黄金值的可 diff 审查介质；layout_fixtures.h 的 C++ 结构体仍是回放介质，二者对账一致）。
3. 写 `tools/dump_panel_geometry.py` + 配套 `tools/dump_panel_geometry.cpp`（临时 main，**链接 2.9.5 的 SARibbonWidgets**，用真实 `SARibbonPanel` 构造各类场景：3 行/2 行/单行模式 × Large/Medium/Small 任意排列 × 隐藏项 × expand 项）。读取路径已核实可行：`SARibbonPanelLayout::updateGeomArray()` 是公共无参函数（h:61，cpp:547-550 转发 `updateGeomArray(geometry())`），调用后直接读 `SARibbonPanelItem` 的公共字段 `itemWillSetGeometry`/`rowIndex`/`columnIndex`（h:51-53）；或 `panel->show()` + `QApplication::processEvents()` 激活布局后读 `item->geometry()`（QWidgetItem 继承，= widget 几何）。CI/无显示环境用 `QT_QPA_PLATFORM=offscreen`。打印 JSON，跑一次，人工抽查 3 例与截图一致，fixture 固化为黄金值。
   - **采集路径规格（轮3补齐，照此可直接写出工具）**：项级输入从 `lay->itemAt(i)` 取（`dynamic_cast<SARibbonPanelItem*>`——该类无 Q_OBJECT 不能用 qobject_cast，但基类虚析构保证 RTTI 可用）读 `sizeHint()/minimumSizeHint()/widget()->maximumWidth()/rowProportion` 与 Gallery 的 `stretchFactor()`；场景级输入 availableRect=`panel->geometry()`、contentsMargins/spacing 走 layout 公共 API、mode=`panel->panelLayoutMode()`、isRTL=`SA::saIsRTL()`。**titleGeometry/optionBtnGeometry 无公共访问器**（`mTitleLabelGeometry/mOptionActionBtnGeometry` 是私有字段）——改在 `panel->show()` + `processEvents()` 后读实际几何：标题用公共的 `panelTitleLabel()->geometry()`，optionButton 用 `panel->findChild<SARibbonPanelOptionButton*>()`（**轮3精确化**：mOptionActionBtn 实为工厂创建的 `SARibbonPanelOptionButton`（cpp:210，导出类、panel 内唯一实例），勿用泛型 QToolButton 查找——SARibbonToolButton 也是 QToolButton 子类会混入）。Input.titleTextWidth 由工具按 cpp:1085-1087 同款公式重算：`SA::compat::horizontalAdvance(panelTitleLabel()->fontMetrics(), panel->panelName()) + 4`；Input.optionBtnSize 由公共 API 推导（cpp:1530-1533 同款公式：`isEnableShowPanelTitle() ? QSize(12,12) : QSize(panelTitleHeight(), panelTitleHeight())`，两函数均公共）。**JSON schema 最小集** = S5.0-2 PanelCase 全字段 + meta 段（字体文件哈希/家族/磅点、devicePixelRatio、Qt 版本、OS、录制时间），保证黄金值录制条件可追溯、可对账。
   - **字体（细化 v2 R5）**：`QFont("SimSun", 9)` 在 linux/mac CI 不存在（现有 26 个测试均未固定字体，已核实无 SimSun 引用——仅 SARibbonLargeButtonAspectRatioTest 用 QFontMetrics 做比例断言，与字体绝对值无关），会导致录制值不可复现。方案：随仓库部署一开源字体（如 Noto Sans SC 或 DejaVu Sans）到 `tests/core/fonts/`，录制工具与 core 测试启动时 `QFontDatabase::addApplicationFont(":/...或磁盘路径")` 后 `QFont(该家族, 9)`；录制前断言 `QFontInfo(f).family()` 命中（防静默 fallback）。**同一 fixture 的录制与回放必须用同一字体文件**；由于 fixture 输入是显式尺寸（见 S5.0-2），Qt5/Qt6、win/linux 的字体度量差异不会进入引擎断言。
4. `tests/core/tst_panelLayoutEngineBaseline.cpp`：此时尚无引擎，先**直接调 2.9.5 的 `SARibbonPanelLayout`**（该测试临时链接 widgets，文件头注释注明"Step B 后改链 core 引擎"）跑 fixture 全绿。
   > 该测试工程暂时需要 Widgets——CI 的 core-only job 会编不过它。处理：`tests/core/CMakeLists.txt` 里此测试单独 `if(TARGET SARibbonWidgets)` 才注册；纯净扫描只管 `src/core/`，不受影响。

**S5.1 Step A 接口化原地重构**（仍在 `src/widgets/SARibbonPanelLayout.cpp` 内）：
1. `SARibbonPanelItem`（`src/widgets/SARibbonPanelItem.h/.cpp`）多继承：`class SARibbonPanelItem : public QWidgetItem, public SARibbon::Core::SARibbonAbstractLayoutItem`。已核实它是 `QWidgetItem` 子类（h:24）且**无 Q_OBJECT**（多继承不引入 moc 问题）；`sizeHint()/minimumSizeHint()/expandingDirections()` 两基类签名一致（`QSize()/Qt::Orientations ... const`），派生类一份 override 同时满足两侧（QWidgetItem 实现直接沿用/显式转调）。
   **`isHidden()` 语义修正（评审轮1，旧稿错误）**：旧稿写 `isHidden() = widget()->isHidden() && !widget()->isWindow()`"照抄 2.x isEmpty() 语义"——**错**。那是 `QWidgetItem::isEmpty()` 的默认语义；2.x 的 `SARibbonPanelItem` 覆盖了它：`isEmpty() const { return action == nullptr || !action->isVisible(); }`（SARibbonPanelItem.cpp:43-46），算法各处 `item->isEmpty()` 判断的都是 **action 可见性**。因此契约 `isHidden()` 的 Panel 侧实现必须是 `return action == nullptr || !action->isVisible();`（与现 `isEmpty()` 等价），照抄 widget isHidden 会把"action 隐藏但 widget 未隐藏"的项错误参与布局，黄金测试必炸。
   `applyGeometry(rect)` = `widget()->setGeometry(rect)`；`maximumWidth()` = `widget()->maximumWidth()`；`stretchFactor()` = Gallery 时返回其值否则 0（见 S4.1-1）；`rowProportion` 字段改用契约基类的（删除派生类字段，类型经 S1 的 using 别名保持源码兼容；注意构造默认值 Large 的对齐问题，见 S1）。
2. `updateGeomArray(const QRect&)`（cpp:795-1186）/`recalcExpandGeomArray`（cpp:1204-1371）内全部 `item->widget()->xxx()` 改为经接口：sizeHint 缓存（实名已核实 `mButtonSizeHintCache`，`QHash<QWidget*, QSize>`，h:174；失效锚 `mButtonSizeHintCacheLargeHeight` h:175，逻辑 cpp:826-829）的 key 从 widget 指针改为 item 指针；所有几何写入维持"先写 `itemWillSetGeometry`、doLayout 统一应用"的 2.x 现状（cpp:636 `item->widget()->setGeometry(item->itemWillSetGeometry)`——**2.x 已是两阶段**，Step A 只需把第二阶段归口到 `applyGeometry`）。
   **缓存生命周期不变量（评审轮2新增，纯 move 时最易丢的副作用）**：key 换指针后，2.x 的两处既有清理必须同步换成引擎缓存 API——① `takeAt()` 移除单个条目（cpp:302-303 `mButtonSizeHintCache.remove(item->widget())`，原注释 "Remove from cache to prevent stale entries"；且 takeAt 内 widget 被 `deleteLater()`，若漏掉此清理，新 item 复用地址会命中陈旧缓存）→ 引擎提供 `removeFromCache(SARibbonAbstractLayoutItem*)`，适配器 takeAt 调用；② `invalidate()` 全清 + 锚复位（cpp:367-368）→ 引擎 `clearCache()`。这是 SARibbon 对 KDDW"ObjectGuard/beingDestroyed 生命周期握手"（Item.cpp:4122-4147, 900-911）的轻量替代：引擎瞬态调用 + 适配器持缓存，悬垂防护完全靠这两条不变量，**G-A/G-B 验证门需含"takeAt 后重建同尺寸新 item，断言无陈旧缓存命中"用例**。
3. 标题/optionButton **不进 items 循环**（实况与 v2 示意不同）：2.x 在 `updateGeomArray` 末尾特殊处理——标题宽度用 `mTitleLabel->fontMetrics()` 对 `panel->panelName()` 求 `horizontalAdvance + 4`（cpp:1085-1087）并回抬 totalWidth（cpp:1088-1091），optionButton 几何依赖标题几何（cpp:1095-1122），`optionActionButtonSize()` 本身是纯函数（cpp:1530-1533：有标题 12×12，无标题 mTitleHeight 见方）。Step A 做法：适配器把 `titleTextWidth`（上述 fontMetrics 计算结果）与 `optionBtnSize` 作为引擎 Input，引擎算出 `titleGeometry/optionBtnGeometry` 放 Result 回写；**不要**把标题/optionButton 伪项化塞进装箱循环——那会改变算法结构，违反"纯 move"。RTL 分支里 `mTitleLabel->setAlignment(...)`（cpp:1160-1167）是 widget 操作，留适配器，按引擎 Result 的 isRTL 应用。
4. **逐函数机械替换，不重排逻辑**；每改一个函数就构建 + 跑 S5.0 基线测试。函数级去向总表见**附录 C**（40 个函数逐一定性：move 引擎 / 留适配器 / 死代码）。

**验证门 G-A**：构建绿 + `tests/core` 基线 fixture 全绿 + ctest == N₀（PanelLayout 相关 RTL 测试重点：`SARibbonPanelLayoutRTLTest`、`SARibbonGalleryStretchFactorTest`、`SARibbonLargeButtonAspectRatioTest`）。

**提交**：`重构：SARibbonPanelLayout 算法改经契约接口读写（Step A）`

**S5.2 Step B 纯 move 搬移**：
1. 新建 `src/core/layout/SARibbonPanelLayoutEngine.h/.cpp`（同步目录平铺 `SARibbonCore/SARibbonPanelLayoutEngine.h`）：`Result layout(QVector<SARibbonAbstractLayoutItem*>&, const QRect&, const SARibbonMetrics&, const Input&)` + sizeHint 缓存结构（以 item 指针为 key，`largeHeight` 变化统一失效，v2 §3.4.1 要点）。引擎持有缓存实例由适配器保存（无状态引擎 + 适配器持缓存，或引擎带缓存——**选引擎带缓存、适配器长期持有引擎实例**，与 2.x 生命周期一致）。缓存公共 API 随迁：`removeFromCache(item)`/`clearCache()`（对应 S5.1-2 不变量，适配器 takeAt/invalidate 调用；`invalidateButtonSizeHintCache(QWidget*)` 重载经 key 映射转发或标记 deprecated，见 S5.2-3）。`Input` 至少含：mode/showPanelTitle/isRTL/contentsMargins/spacing/titleTextWidth(-1=无标题)/optionBtnSize/titleHeight/titleSpace（**轮3修正：不含 `enableExpanding`**——2.x 无此用户开关，`recalcExpandGeomArray` 的唯一触发条件是宽度比较式 `totalWidth < setrect.width() && (setrect.width() - totalWidth) > 10`（cpp:1075-1078），该判定连同 **>10 阈值**属算法本体，随 move 原样进引擎；v2 §3.4.2 草案 Input 的 enableExpanding 字段据此作废，若 QML 侧未来需要开关另立字段且默认 true，M1 判定逻辑不得引用）；`Result` 至少含：sizeHint/columnCount/largeHeight/titleGeometry/optionBtnGeometry/totalWidth。引擎还需 `panelHeightHint` 公式——`SARibbonPanel::panelHeightHint(fm, layMode, panelTitleHeight)`（SARibbonPanel.h:344，cpp:1587-1608）**已核实是静态纯函数**（仅 fm.lineSpacing() × 系数 + titleHeight），随引擎 move 进 core（SARibbonPanel 侧保留转发以兼容公共 API）。
2. `updateGeomArray(const QRect&)` + `recalcExpandGeomArray` 函数体**整体 move** 进 `layout()`，`widget()` 相关残留全数已在 Step A 消除；调试打印（实名已核实：宏 `SARibbonPanelLayout_DEBUG_PRINT`，cpp:14-15 默认 0；伴生宏 `SARibbonPanelLayout_HELP_DRAW_RECT` cpp:27；静态计数器 `s_p_debug_seq`/`s_p_doLayoutDepth`）**不随迁**，留适配器（v2 §3.4.2 末段）。**死代码警示**：`columnWidthInfo`（h:165，cpp:1388-1398）在 2.9.5 已无任何调用者（仅注释提及，recalc 优化后废弃）——**不 move**；建议 S8 清扫时删除并单独提交（删除私有死函数不破坏 API），或保留原地并在 NOTES.md 记录。
   **禁止顺手结构体化（评审轮2新增）**：KDDW 把中间计算状态显式化为可序列化的 `SizingInfo::List` 快照（Item_p.h:93-188；算法三段式"快照→纯函数→统一回写"，Item.cpp:3021-3067），Panel 侧的 `columMaxWidth` 等散量按此收敛会更可测——但那是**结构重构，M1 纯 move 纪律禁止**；列为 3.1 引擎优化项（见 round2 findings 设计级建议 6）。唯一例外：Category 侧已有的 `SizeHintCollection`（SARibbonCategoryLayout.cpp:29-35）本身就是半显式中间态，S6 搬移时**保留其结构体形态**、不得打散回局部变量（归属见 S6-2/附录 D：结构体定义入 core 作引擎 Input 载体，收集函数 collectSizeHints 留适配器）。
3. widgets 侧 `SARibbonPanelLayout`：`doLayout()`/`sizeHint()`/`setGeometry()` 契约为"collectItems → mEngine.layout(...) → items 逐个 applyGeometry → show/hide 批处理与标题应用 → 通知重绘"（v2 §4.2 示意）；**重入守卫（cpp:1831-1843 `setGeometry` 的 `mInDoLayout` 检查，h:190；doLayout 内 RAII guard cpp:612-620）、LayoutRequest 投递、QLayout 系统交互全部留在适配器**（v2 R2）。公共 API 全保留：无参 `updateGeomArray()`（h:61）退化为一行转发（注意它是**公共 API**，不可删；**轮3补外部锚点**——`SARibbonPanel::updateItemGeometry()`（SARibbonPanel.cpp:1470-1480）直接调它做"只重算不摆放"的刷新，转发壳必须保持 2.x 全量写回副作用：引擎计算后回写 mSizeHint/mColumnCount/mLargeHeight/mTitleLabelGeometry/mOptionActionBtnGeometry 及各 item 的 itemWillSetGeometry/rowIndex/columnIndex，少写回一项外部刷新路径就失效），`invalidate()`（cpp:356-370，含清缓存副作用）改为"清引擎缓存 + QLayout::invalidate"，`invalidateButtonSizeHintCache()` 两个重载（cpp:563/579）转发引擎缓存 API（QWidget* 版重载按 key 映射处理或标记 deprecated，NOTES.md 记录）。行数目标修正：v2 的"< 300 行"是对适配逻辑的估计，实测 PanelLayout.cpp 还有约 40 个属性存取/QLayout 重写/工厂函数留守（见附录 C），**行数不作硬门**，以验收门"算法体零残留"（S8-2 的标记物 grep）为准。
4. `tests/core/tst_panelLayoutEngine.cpp`：改为链接 core，`FakeLayoutItem`（纯内存实现契约接口，喂 fixture 的显式 sizeHint/maxWidth/stretch）跑同一 fixture；删除 S5.0 的 widgets 版基线测试（或保留为 `tests/widgets/` 的适配器级回归）。CI：`tests/core` 不再依赖 widgets。

**验证门 G-B**：构建绿 + `tst_panelLayoutEngine` 全 fixture 绿（黄金值零修改——**若需改黄金值即为行为回归，必须查明原因**，见 §4-2）+ ctest == N₀ + 6 张截图对比 + 纯净扫描绿。

**提交**（两笔）：`重构：Panel 装箱算法纯搬移至 SARibbonPanelLayoutEngine（Step B）`；`测试：黄金几何测试改链 core 引擎并移除 widgets 依赖`

### S6 CategoryLayoutEngine 提取（§3.4.3，复用 S5 方法论）

1. `SARibbonCategoryLayoutItem`（包装 `SARibbonPanel`，定义在 SARibbonCategoryLayout.h:145-157，**已是 `QWidgetItem` 子类**，公有字段 `separatorWidget`/`mWillSetGeometry`/`mWillSetSeparatorGeometry`）实现契约接口（原地接口化）。**与 Panel 侧的语义差异（已核实）**：它**没有**覆盖 `isEmpty()`，用的是 `QWidgetItem::isEmpty()` 默认语义（widget 隐藏且非窗口）——与 `SARibbonPanelItem` 的 action 可见性语义**不同**，契约 `isHidden()` 在此侧的实现就是 `QWidgetItem::isEmpty()` 转调，两个布局各按各自 2.x 语义实现，**不得统一**。separator 的第二块几何按 S4.1-1 的 core 扩展结构回写。**隐藏项 separator 副作用的位置（轮3修正）**：2.x 对 isEmpty 项的 `item->separatorWidget->hide()` 发生在 `updateGeometryArr()` **函数体内**（cpp:536-544），不是 doLayout；且 `updateGeometryArr()` 有外部调用者——`SARibbonCategory::PrivateData::updateItemGeometry()`（SARibbonCategory.cpp:187-197，主题切换等场景的"只重算不摆放"路径）直接调它。Step B 引擎化后 hide 是 widget 操作不能进引擎，归置到**公共 `updateGeometryArr()` 适配器壳**内（壳 = 引擎计算 + 回写 item 双几何字段/mTotalWidth/mCachedSizeHint/mCachedMinSizeHint + 对隐藏项 separator 执行 hide），doLayout 的 hideWidgets 批处理（cpp:666-738）照旧——两条路径重复 hide 是幂等的；**不得**把该副作用只挪进 doLayout，否则外部调用路径丢失 2.x 行为。
2. **Step A**：`updateGeometryArr()`（无参，cpp:431-592）/`categoryContentSize()`（cpp:408-418）/`PrivateData::collectSizeHints()`（cpp:116-172，轮3行号收紧——旧稿"116-199"把 ctor 段误含进来；产出 `SizeHintCollection` cpp:29-35：totalWidth/canExpandingCount/panelSizes/separatorSizes。**归属定调（轮3）**：结构体定义随引擎入 core 作 Input 载体（`SARibbon::Core` 命名空间，保持结构体形态、不打散），**收集函数留适配器**——separatorSizes 来自 `item->separatorWidget->sizeHint()`（widget 查询，契约扩展只有 separator 的输出字段、无输入方法），panelSizes/isExpanding 经契约接口取）改经接口读写；跑通（沿用 S5.0 思路先录 fixture——用 `SARibbonCategory` 真实构造 panel 组合场景：不同 sizeHint 的 panel 序列、扩展 panel（`SARibbonPanel::isExpanding`，updateGeometryArr cpp:554 消费）、scroll 偏移边界、RTL/LTR 双份）。widget 依赖清单（Step A 必须接口化/入参化）：`category->width()/height()`（cpp:437/439）、`contentsMargins()`（cpp:438）、`SA::saIsRTL()`（cpp:470/573）、`categoryAlignment()`（cpp:522）、`item->isEmpty()`、`p->isExpanding()`（→ 契约 `expandingDirections()` 或专用输入）、隐藏项的 `separatorWidget->hide()`（cpp:538-541，widget 操作，留适配器的应用阶段）。
3. **Step B**：几何部分 move 到 `src/core/layout/SARibbonCategoryLayoutEngine.h/.cpp`（`Result layout(panels, availableRect, metrics, Input{isRTL, alignment, margins, xBase})`，Result 含每 item 的 panel/separator 双几何、totalWidth、**sizeHint/minSizeHint（轮3补：对应 cpp:568-570 尾部对 mCachedSizeHint/mCachedMinSizeHint 的写入，壳照抄回写）**、**newXBase（轮3补：非滚动分支 cpp:510 有 `d_ptr->mXBase = 0` 的状态副作用，引擎不得直写适配器状态，经 Result 带回由壳应用）**、左右滚动按钮可见标志。标志判定逻辑 cpp:469-505 是纯计算（total/categoryWidth/mXBase/isRTL），随算法入 core——**但轮3发现它是 2.x 双实现之一**：`updateScrollButtonVisibility()`（cpp:1053-1091，由 `updateScrollOffset` cpp:1136 调用，即滚动/动画期间不重算几何只重算标志的路径）**逐分支重算同一套 RTL/LTR 标志**（两处唯一文本差异是 maxBase 的 `qMax(0,...)`，在 needsScrolling 前提即 total>categoryWidth 下恒正，语义等价；真正的按钮 setVisible/raise 在 doLayout cpp:707-714，旧稿附录 D"按钮显隐执行"的描述有误）。处理：引擎把标志判定做成**独立纯函数**（如 `static ScrollFlags scrollButtonFlags(int totalWidth, int viewportWidth, int xBase, bool isRTL)`），`layout()` 内部复用同一函数；适配器 `updateScrollButtonVisibility()` 改为"调引擎函数 + 写 d_ptr 两标志字段"。这是把两份文本级重复收敛为 core 单实现（§4-4 禁止双实现的直接要求），零逻辑改动，NOTES.md 记录并跑 `SARibbonCategoryVisibilityTest` + 滚动交互冒烟双验。滚动按钮 12px 宽度硬编码几何 cpp:646-652 是 widget 摆放，留适配器）+ `clampScrollOffset(...)` 纯计算——**v2 草案签名缺 isRTL**：实际钳制逻辑（`setScrollPosition` cpp:1156-1184）RTL 为 `[0, totalWidth-availableWidth]`、LTR 为 `[availableWidth-totalWidth, 0]`，两侧边界不同，签名须为 `clampScrollOffset(int requested, int contentWidth, int viewportWidth, bool isRTL)`。`scrollByAnimate/scrollToByAnimate` 的 `QPropertyAnimation`（cpp:971-1033，动画挂在 Q_PROPERTY `scrollPosition` 上，h:29）**留 widgets**，动画目标值取自引擎的 clampScrollOffset。`updateScrollOffset`（cpp:1109，平移预计算几何）与 `updateScrollButtonVisibility`（cpp:1053，按钮显隐执行）留适配器。**CategoryLayout 自己也有 setGeometry 重入守卫**（cpp:1371 起，`d_ptr->mInDoLayout`，RAII guard 在 doLayout cpp:623-631）——与 Panel 同理全部留适配器，不得搬入引擎。
4. `tests/core/tst_categoryLayoutEngine.cpp` 用 FakeItem 跑 fixture（注册名带 `core_` 前缀或 LABELS core，见 S5.0-1）。函数级去向总表见**附录 D**。

**验证门**：同 S5 G-B（ctest 含 `SARibbonCategoryLayoutRTLTest`、`SARibbonCategoryVisibilityTest`）。

**提交**：`重构：Category 布局引擎下沉 core（Step A 接口化 + Step B 搬移）`

### S7 BarGeometryEngine 提取（§3.4.4，允许 D6 降级）

按 v2 §3.4.4 的"下沉/留守"表：
1. **下沉**：`layoutTitleRect` 的几何推导、`calcMinTabBarWidth`（S3 已定处理方式，此处引擎引用）、上下文标签页 tab 几何排布、最小/正常模式高度切换度量（S3 已完成）。**实况警示**：`layoutTitleRect()`（cpp:1242-1366）**不是纯函数**，它直接读 `ribbonTabBar->geometry()/tabRect(i)`、`quickAccessBar->geometry()/x()`、`ribbon->width()`、`ribbon->currentVisibleContextCategoryTabIndexs()`、`d_ptr->systemButtonSize`、`contentsMargins()`、`SA::saIsRTL()`、`isCompactStyle()`。提取方式只能是"适配器采集上述值填 `TitleRectInput` 结构 → core 纯函数返回 titleRect"，且 RTL/LTR、Loose/Compact 四分支（cpp:1251-1365）逻辑一行不改。
2. **留守 widgets**：`doLayout`（cpp:749-756，仅分派）对具体 QWidget 的摆放执行主体 `resizeInLooseStyle()`（cpp:1446-1742，约 300 行）/`resizeInCompactStyle()`（cpp:1743-1993，约 250 行）、`SARibbonSystemButtonBar` 尺寸联动、frameless/`SAFramelessHelper`、窗口图标控件（`setWindowIcon` cpp:1059-1062 直接摸 titleIconWidget）、QSS、mdi 样式。`sizeHint()`（cpp:697-705）是字段纯计算（minWidth/minHeight/maxMinWidth），可随 metrics 数据下沉，但收益低，允许留守。
3. **允许降级（D6）**：若完整 `BarGeometryEngine` 提取风险高（`SARibbonBarLayout` 1993 行中窗口按钮/frameless 交织深，两大 resize 函数全是 widget 摆放），可只完成"度量部分（已在 S3 完成）+ `layoutTitleRect` 纯几何提取（按 S7-1 的 Input 结构方式）"两项；**是否降级在 S7 开始时评估并记 NOTES.md**。Panel/Category 引擎（S5/S6）不允许降级。
4. 对应黄金测试 `tests/core/tst_barGeometryEngine.cpp`（若降级则只测 titleRect 部分；注册名带 `core_` 前缀或 LABELS core）。

**验证门**：构建绿 + ctest == N₀（`SARibbonBarLayoutRTLTest`、`SARibbonSystemButtonBarGeometryTest`、`SARibbonTitleBarHitTestTest` 重点）+ 截图（含最小模式切换、上下文标签页高亮色）。

**提交**：`重构：Bar 几何与标题区算法下沉 core（SARibbonBarGeometryEngine）`

### S8 黄金测试补全 + widgets 侧残留清扫

1. 覆盖矩阵补全（v2 §7.1）：三行/两行/单行 × Large/Medium/Small 任意排列、隐藏项、expand 项（`recalcExpandGeomArray`，含 Gallery stretchFactor 加权分支 cpp:1275-1329）、sizeHint 缓存失效（改 metrics 再 layout 断言新值；对应 cpp:826-829 的 largeHeight 锚定逻辑）、RTL 镜像（cpp:1136-1168）。**矩阵四类补强（评审轮2，对照 KDDW）**：① **关系不变量断言**与绝对黄金值并用——如"同行项 x 严格递增且互不重叠""列宽 = 该列最大项宽""Result.sizeHint ≥ 各项 min 之和 + spacing"（KDDW 引擎测试以关系断言为主：`CHECK(item3->x() > item2->x())`、`CHECK_EQ(container3->height(), item3->height() + st + item31->height())`，tst_multisplitter.cpp:284-285, 352-364；黄金值管"与 2.x 一致"，不变量管"跨平台恒真"）；② **退化输入族**入矩阵（空 items/零宽 rect/全隐藏/溢出/矛盾约束，见 S5.0-2）；③ **dpr 变体**入矩阵（dpr=1.0/2.0，见 S5.0-2）；④ **takeAt 陈旧缓存用例**（S5.1-2 不变量的验证门）。
2. 残留清扫校验（**命令修正**：`updateGeomArray`/`invalidateButtonSizeHintCache` 是保留的公共 API 转发壳，按函数名 grep 必然命中，不能作为判据；改按**算法体内部标记物**检查。**轮3补全 Category/Bar 标记并排除误报**——下列每个标记物均已真跑全仓 grep 核实只出现在对应算法函数体内；门范围**限定三个布局 .cpp 文件**而非整个 `src/widgets/` 目录，原因：① `contextRegionLeft/titleStart` 等在 `SARibbonBar.cpp:3810-3854` 有一段**注释掉的旧代码**会误报；② `canExpandingCount/panelSizes/separatorSizes` 不能当标记——`collectSizeHints` 按 S6-2 合法留守适配器）：
   ```bash
   # Panel：标记只在 updateGeomArray(QRect)/recalcExpandGeomArray 内出现（已核实）
   git grep -n -E "columMaxWidth|yMediumRow|ySmallRow|columnExpandInfo|spacingRow" -- src/widgets/SARibbonPanelLayout.cpp
   git grep -n "mButtonSizeHintCache" -- src/widgets/   # 应零命中（缓存成员随引擎走，转发壳只调引擎缓存 API，不再出现该成员名；旧稿"只剩引擎转发"措辞作废）
   # Category：expandWidth 只在 updateGeometryArr cpp:454-556 出现（已核实）
   git grep -n "expandWidth" -- src/widgets/SARibbonCategoryLayout.cpp
   # Bar：四标记只在 layoutTitleRect cpp:1255-1361 出现（已核实；validTitleBarHeight 不可当标记——
   #      resizeInLooseStyle/resizeInCompactStyle cpp:1453/1749 留守处仍有合法命中）
   git grep -n -E "contextRegionLeft|contextRegionRight|titleEnd|titleStart" -- src/widgets/SARibbonBarLayout.cpp
   ```
   全部应零命中（算法体已全在 core；R4-禁止双实现）。`columnWidthInfo` 死代码处置见 S5.2-2。
3. 运行 `python3 tools/check_core_purity.py src/core`（Windows 本机若无 python3 用 python，口径见 P5）；core-only 构建（**轮3修正命令**：旧稿仅 `-DSARIBBON_BUILD_WIDGETS=OFF` 会因 examples 默认 ON 链接不存在的 widgets target 而 configure 失败；TESTS 显式 ON 配合 S5.0-1 的 `if(TARGET SARibbonWidgets)` 守卫，恰好验证 tests/core 已不依赖 widgets——即 S5.2-4 的目标；与 README R2 的 core-only 命令形态对齐）：
   ```bash
   cmake -S . -B build-3.0-coreonly -DSARIBBON_BUILD_WIDGETS=OFF -DSARIBBON_BUILD_EXAMPLES=OFF -DSARIBBON_BUILD_TESTS=ON
   cmake --build build-3.0-coreonly --config Release
   ctest --test-dir build-3.0-coreonly -C Release -L core --output-on-failure   # 全绿
   ```
   **纯净门禁分两层（评审轮2细化，对 v2 §3.7 清单的执行语义定调；KDDW 实证见 round2 findings 一-3。round2 整合注：v2 §3.7 已修订为与本节一致的两层表述，v2 §6.2 的 CI 扫描示例已同步两层命令）**：
   - **模块层（硬门，CI 阻断）**：`src/core/` 全目录禁止 include QtWidgets/QtQuick 头、禁止链接 Qt::Widgets/Qt::Quick。扫描清单在 v2 §3.7 基础上扩充为：`QWidget`、`QApplication`、`QStyle`、`QStyledItemDelegate`、`QLayout`（及 `Q*Layout` 族）、`QQuickItem`、`QQml`、以及**模块化 include 路径形式** `<QtWidgets/`、`<QtQuick/`（裸类名扫描可被其绕过）。**明确合法（不得误伤）**：`QGuiApplication`/`QScreen`/`QFontMetrics`/`QColor`/`QIcon` 属 QtGui——S1 的 `isOperatingSystemInDarkMode`（QGuiApplication::styleHints）、S2 的 ThemeData 等合法依赖它们；`QAction` 维持 v2 §3.7 的 Qt5/Qt6 归属分歧处理（core 一律不 include）。
   - **确定性层（引擎专属，CI 定向 grep + 评审门）**：`src/core/layout/` 三引擎文件内禁止调用 QGuiApplication 的动态状态（`layoutDirection()`/`styleHints()`/`primaryScreen()`/`screens()`）——引擎必须"同输入同输出"以保黄金测试跨平台有效，这些值只能经 Input/Metrics 字段进来（S4.1-4、S3.0）。global/theme/metrics 的采集器函数不受此层约束。KDDW 佐证：其 layouting/ 净室对 Platform 的唯一触碰是析构期守卫且被前端宏隔离（Item.cpp:1114-1122），DPI/屏幕全部数据化（Screen_p.h:41）。扫描命令（进 check_core_purity.py 或 CI step）：
   ```bash
   git grep -n -E "QGuiApplication|QScreen|primaryScreen|styleHints|layoutDirection\(\)" -- src/core/layout/ && exit 1 || exit 0
   ```
4. 6 张截图终验 + 度量对照表复核。

**提交**：`测试：黄金几何测试覆盖矩阵补全；清扫 widgets 侧算法残留`

### S9 core 测试升格 CI 前置

**操作**：`.github/workflows/` 6 个构建 workflow（已核实：cmake-{linux,mac,win}-qt{5.15,6.8}.yml；另有 page.yml/publish-python-bindings.yml 不属构建 workflow，不动）：在现有构建 job 的构建步骤前，新增独立 step"core golden tests"（构建并 `ctest --test-dir <build> -C Release -L core --output-on-failure --no-tests=error`——**用 LABELS 过滤而非 `-R "core"`**：现有测试以文件名注册（tests/CMakeLists.txt:32 `add_test(NAME ${TEST_NAME} ...)`），名称不含 core 字样，`-R core` 将匹配不到任何测试而"假绿"；tests/core 的 CMakeLists 须 `set_tests_properties(... PROPERTIES LABELS core)`，与 S5.0-1 的注册约定配套。**`--no-tests=error` 必带（round3 终审补，与 01 S10-7/03 S5-4 的防空转纪律同口径）**：LABELS 零匹配时 ctest 默认退出 0，该参数把"tests/core 静默未注册"变成硬失败；注意本 step 与 01 S10-2 的 `if: matrix.widgets == 'ON'` 条件联动——linux-qt6.8 的 widgets=OFF 组合按 01 S10-2 的矩阵设计 TESTS 也联动为 OFF，该组合下本 step 应与 Test step 同条件跳过，避免零测试撞 `--no-tests=error`）；保证该 step 失败即 job 失败（v2 §6.3：tests/core 最先运行）。

**提交**：`CI：core 黄金几何测试前置为门禁`

### S10 文档与收尾

1. `docs/zh/dev-guide/` 新增或更新：core 模块结构说明（七子系统，与本计划 §1 清单一致；round3 终审统一计数——原"六子系统"漏 global）、契约接口用法、引擎适配器模式、"纯计算归 core"checklist（v2 §2.2 六问照抄成中文小节）；**附最小 FakeItem 走查示例**（评审轮2，仿 KDDW 的独立复用示例 `src/core/layouting/examples/qtwidgets/main.cpp:14-30`——用裸 QWidget 子类化三个 layouting 接口即可驱动引擎，证明契约面自足）：dev-guide 用 tests/core 的 FakeLayoutItem 展示"**不建任何 widget 树**跑一次 PanelLayoutEngine::layout()"的完整代码路径（**轮3措辞修正**：旧稿"不建 QApplication"不准确——`SARibbonMetrics` 携 QFontMetrics、引擎的 panelHeightHint 公式调 `fm.lineSpacing()`，字体库访问要求先有 **QGuiApplication**（QTEST_MAIN 在不链 Widgets 时自动实例化的正是它，配 `QT_QPA_PLATFORM=offscreen`）；自足性指不需要 QApplication/widget 树）。
2. 更新 AGENTS.md 项目结构段（`src/core/` 各子目录）。
3. NOTES.md 汇总：每笔 Step B 的 diff 行数、度量对照表结论、降级决策（D6/D7/S4.3）。

**提交**：`文档：core 子系统与适配器模式开发指引`

## 6. 完成验收门（= v2 M1 交付判据，全部满足）

- [ ] 仅 Core+Widgets 构建通过；现有 example/tests 全绿（ctest == N₀，无跳过无失败）
- [ ] **黄金几何测试 100% 通过**（`tests/core` 三引擎 fixture 全绿；fixture 值来自 2.9.5 实测录制，全程零修改）
- [ ] core-only 独立编译通过（`SARIBBON_BUILD_WIDGETS=OFF`）
- [ ] `python3 tools/check_core_purity.py src/core` 退出码 0（CI 上亦绿；Windows 本机若无 python3 用 python，口径见 P5）
- [ ] 6 张截图（3 行/2 行/最小 × 亮/暗）与 2.9.5 基线一致
- [ ] `docs/3.0/metrics-comparison.md` 度量对照表逐项相等
- [ ] `SARibbonPanelLayout/CategoryLayout/BarLayout` 公共类名、方法、信号未变。**机检方法（轮3补充，替代旧稿不可执行的"git diff public 段"人工目测）**：基线 commit（S5 开工前，记入 NOTES.md）与 HEAD 各提取三个布局头文件的声明行集合，规范化（去注释/空白归一/排序）后 diff，**要求无删除行**（新增 using/include/friend/Q_DECL_OVERRIDE 行允许；头文件内声明顺序变化不构成破坏）：
  ```bash
  BASE=<S5开工前commit>
  for f in SARibbonPanelLayout SARibbonCategoryLayout SARibbonBarLayout; do
    for rev in "$BASE" HEAD; do
      git show "$rev:src/widgets/$f.h" | sed 's,//.*,,' | grep -vE '^\s*(/\*|\*)' \
        | sed 's/[[:space:]]\+/ /g; s/^ //; s/ $//' | grep -E '[;(]$|[;(] ' | sort > "/tmp/$f.$rev.txt"
    done
    if diff "/tmp/$f.$BASE.txt" "/tmp/$f.HEAD.txt" | grep -q '^<'; then
      echo "$f: 红灯——存在被删除的声明行"; else echo "$f: OK（仅新增或无变化）"; fi
  done
  ```
  出现 `^<` 行即红灯，按 R3 回滚排查（该方法对宏拼接生成的声明不敏感，三布局头文件已核实无此情形）
- [ ] widgets 三布局 .cpp 中不再含装箱/分列/宽度分配算法体（适配器形态；**判据 = S8-2 的四组标记物 grep 全部零命中**，轮3已补 Category/Bar 标记）
- [ ] NOTES.md 记录完整（每个 Step B、每项决策、每张截图路径）

## 7. 风险与缓解（v2 R1/R2/R5 本地化）

| 风险 | 缓解 |
|------|------|
| 引擎提取引入行为回归 | 两步走 + 黄金基线先录（S5.0）+ M1 期间禁改算法；回归=查因，不允许"顺手修" |
| 重入守卫/Qt 布局系统交互被误搬 | 守卫清单全留适配器（S5.2-3、S6-3 明示：Panel 与 Category **各有一套** `mInDoLayout` 守卫）；RTL 测试与截图双验 |
| 黄金值平台字体差异 | fixture 输入为显式尺寸、引擎断言零字体依赖（S5.0-2）；录制工具固定部署字体（S5.0-3，`tests/core/fonts/` + `QFontDatabase::addApplicationFont`） |
| `RowProportion` 提升枚举破坏 QAction 属性名兼容 | 属性名常量一并入 core 且字符串值不变；`SARibbonPanelItem` 保留类型别名；QuickAccessCustomize 测试验证 |
| 嵌套 Q_ENUM 枚举下沉破坏元对象/Q_PROPERTY | S1.1 五步策略 + 允许留在 widgets 的降级出口；提交前 grep QMetaEnum 消费者（sip 冒烟顺延至计划 03 S3，S1.1-4 round3 裁决；moc/sip 工具链灰区另见遗留风险——建议开工前做独立小工程 spike，final-audit.md §五） |
| 契约接口缺 maximumWidth/stretchFactor/isRTL 导致引擎行为缺口 | S4.1-1 已把实际算法依赖全部列入契约与 Input，Step A 期间以 G-A 黄金测试兜底 |
| BarGeometryEngine 风险高 | D6 降级路径已定义（S7-3），降级不影响 M1 验收 |
| widgets 版基线测试期间破坏 core-only CI | S5.0-4 的 `if(TARGET SARibbonWidgets)` 条件注册 |

## 8. 已知偏差

见 [NOTES.md](NOTES.md)。特别提醒：v2 计划引用的 `SARibbonPannelLayout.h` 等旧文件名在本仓库均为单 n（`SARibbonPanelLayout.h`）。

评审轮1新增已核实偏差（详情见对应 S 节与附录，执行时以本文而非 v2 原文为准）：
- v2/旧稿所称 `RibbonButtonStyle`、`SARibbonApplicationButton` 枚举、`BarMode` 均不存在，实名见 S1.0；`ToolButtonPopupMode` 是 Qt 原生枚举不可下沉。
- 旧稿"SARibbonThemeManager 拆分为薄壳"不成立：2.9.5 无此类，只有 `SA::applyRibbonTheme` 自由函数（S2.0）。
- 旧稿"六个私有度量函数纯 move"不成立：它们是公共 API，且 `calcMinTabBarWidth` 触碰控件、三个默认值推导依赖 `QStyle::pixelMetric`（S3.0）。
- v2 §3.4.1 契约草案的 `QRect geometry` 字段名与 `QLayoutItem::geometry()` 冲突；五个纯虚缺 `maximumWidth`/`stretchFactor`/isRTL 输入（S4.1）。
- 旧稿 `isHidden()` 照抄 widget-isHidden 语义错误：Panel 侧 2.x `isEmpty()` 实为 action 可见性判断（S5.1-1）。
- v2 metrics 示例 `spacing = 1` 与实测 Panel 默认 `setSpacing(2)` 不符（S3.1 常量表）。
- `SARibbonPanelLayout::columnWidthInfo` 是死代码（无调用者），不随算法迁移（S5.2-2）。

评审轮2（KDDW core 深读，证据详见 [reviews/round2/kddw-core-findings.md](reviews/round2/kddw-core-findings.md)）新增已核实偏差与决策：
- 轮1 S4.1-3"共享数据字段同名**同型**"不准确：2.x `rowIndex` 是 `short`（SARibbonPanelItem.h:51），契约定 `int`，处置与验证命令见 S4.1-3。
- KDDW core 并非绝对零 GUI（widget 触点靠前端宏裁剪：DragController.cpp:30-37 等），其 `layouting/` 子目录才是真净室——v2 §3.7 禁区清单按"模块层/确定性层"两层执行，扩充与豁免清单见 S8-3。
- "引擎不持有 QObject"的通行说法需纠偏：KDDW 引擎节点 Item 本身是 QObject（Item_p.h:190-192，Qt 前端 Core::Object=QObject，QtCompat_p.h:83），QObject-free 的是契约面（Guest/Host/Separator）；SARibbon 引擎连 Item 层 QObject 都不需要（瞬态纯函数 + Result 返回），是比 KDDW 更彻底的形态，属有意分叉而非缺口。
- ThemeData 单例选型定案：Meyers 函数内静态对象（S2.1-5），Q_GLOBAL_STATIC 与挂 QCoreApplication 两案否决。
- KDDW 引擎完全无 RTL 支持（src/core/ grep RightToLeft/isRTL/layoutDirection 零命中）——SARibbon 的 RTL 入参化设计无先例可抄，S6-2/S8-1 的 RTL fixture 是唯一保障，不得以"参考项目没做"为由裁剪。

评审轮3（首次执行者全量 dry-run，全部结论经只读命令真跑核实，证据明细见 [reviews/round3/02-dryrun-findings.md](reviews/round3/02-dryrun-findings.md)）新增已核实偏差与修正：
- P2 旧稿 N₀=25 错误，实为 **26**（tests/CMakeLists.txt 26 个 add_saribbon_test；README/NOTES B8/计划01 P3/v2 §7.3 同口径）——所有"ctest == N₀"验证依赖此值。
- RowProportion"类型别名即可兼容存量代码"不成立：unscoped enum 的枚举符不随别名迁移，`SARibbonPanelItem::None/Large` 类限定访问需 4 条枚举符 using 声明；且枚举应入 `SARibbon::Core` 命名空间避免全局 `None`（X11 宏）污染，并保持 unscoped（CustomizeWidget.cpp:52 依赖隐式 int 转换）（S1）。
- 契约代码块导出宏 `SARIBBON_CORE_EXPORT` 是误名，实为计划01 S6.3 的 **`SA_RIBBON_CORE_EXPORT`**（S4.1 已更正；v2 §3.4.1/§3.6 同误，已由轮3终审同步修正）。
- S4.2"widgets 类组合 core 记录"与 161 处公有字段直接访问冲突，改**公有继承**；"8 个 make* 带 manager"实为 **6** 个（共 15 个 make*）；`simplify()` 已核实为纯（cpp:926-1040 + remove_indexs cpp:896-898），入 core 定案；补 `actionManager()/setActionsManager()`（h:69/72）留守清单。
- 附录 E `init 607-696 创建子控件（经 factory）`错误：init 是空桩（607-610），623-696 为 addItem/itemAt/takeAt/count；附录 D/E 各补构造/析构行，附录 D 补 doLayout 707-714 滚动按钮 setVisible 段。
- Category 滚动按钮标志在 2.x 是**双实现**（updateGeometryArr:469-505 与 updateScrollButtonVisibility:1053-1091，语义等价），Step B 必须收敛为引擎 `scrollButtonFlags` 单一纯函数（S6-3）；附录 D 对该函数"显隐执行"的描述有误。
- `enableExpanding` 不是 2.x 开关（recalcExpandGeomArray 触发条件是宽度比较式 cpp:1075-1078，>10 阈值属算法本体），从引擎 Input 与 fixture 删除（S5.2-1/S5.0-2；v2 §3.4.2 草案字段作废）。
- `updateGeomArray()`/`updateGeometryArr()` 公共无参壳有**外部调用者**（SARibbonPanel.cpp:1478、SARibbonCategory.cpp:195 的 updateItemGeometry 路径），壳必须保 2.x 全量写回/副作用（S5.2-3/S6-1）。
- S1-4 与 S1-5 同步头路径矛盾（`<SARibbonCore/global/...>` vs 平铺决策）已统一为平铺；计划01 sa_sync_include 无平铺参数，需加 FLATTEN 选项（S1-5）。
- S5.0-1 tests/core 挂接点按计划01 S7 实际层级修正（tests/CMakeLists.txt 而非根），tests/widgets 需 `if(TARGET SARibbonWidgets)` 守卫；S8-3 core-only 命令补齐 EXAMPLES=OFF/TESTS=ON 并加 ctest -L core。
- S8-2 残留门补 Category（expandWidth）/Bar（contextRegionLeft|contextRegionRight|titleEnd|titleStart）标记物并限定三布局 .cpp 范围（SARibbonBar.cpp:3810-3854 有注释掉的同名词会误报；validTitleBarHeight 不可作标记）。
- §6"public 段无破坏性改动"门补充可执行机检脚本（声明行集合 diff、禁删除行）；S5.0-3 dump 工具补 title/optionBtn 几何采集路径与 JSON schema 最小集；S10-1"不建 QApplication"修正为"不建 widget 树（QGuiApplication 仍必需）"。

---

## 附录 A：公共枚举全量盘点与去向（S1 依据）

盘点命令（已验证）：`git grep -n -E "^\s*enum (class )?[A-Za-z]" -- "src/SARibbonBar/*.h"`（计划 01 后路径为 `src/widgets/*.h`；git pathspec 的 `*` 跨目录，colorWidgets 子目录一并命中——已实测）。**原始命中 17 行，其中 SARibbonThemeManager.h:7 是前置声明，剔除后恰为下表 16 项**（轮3真跑核对）。行号基于 2.9.5。

| 枚举 | 位置 | Q_ENUM | 使用者/耦合 | 去向 |
|---|---|---|---|---|
| `SARibbonAlignment`（enum class） | SARibbonGlobal.h:197，Q_DECLARE_METATYPE :203 | 无（自由枚举） | CategoryLayout 对齐（setCategoryAlignment h:115）、SARibbonBar 对齐属性 | **core**（干净 move） |
| `SARibbonTheme`（enum class） | SARibbonGlobal.h:220，Q_DECLARE_METATYPE :233 | 无 | ThemeManager 静态表、MainWindow/Widget Q_PROPERTY `ribbonTheme` | **core**（干净 move） |
| `SARibbonMainWindowStyleFlag`（enum class : int） | SARibbonGlobal.h:245，Q_DECLARE_FLAGS/OPERATORS 紧随 | 无 | SARibbonMainWindow | **core**（连 flags 声明一起 move） |
| `SARibbonPanelItem::RowProportion` | SARibbonPanelItem.h:36-42 | 无（类无 Q_OBJECT） | PanelLayout 装箱、CustomizeData.actionRowProportionValue、QAction 属性 `_sa_RowProportion` | **core**（提升自由枚举 + 类内 using；构造默认值 Large 注意，PanelItem.cpp:15） |
| `SARibbonPanel::PanelLayoutMode` | SARibbonPanel.h:115-157 | Q_ENUM :158 | `Q_PROPERTY(SARibbonPanel::PanelLayoutMode panelLayoutMode)`（SARibbonBar.h:186）、panelHeightHint 入参 | **core**（按 S1.1 五步策略；不成立则降级留 widgets） |
| `SARibbonBar::RibbonStyleFlag` | SARibbonBar.h:189-203 | Q_ENUM :204 + Q_FLAG(RibbonStyles) :206 | `Q_PROPERTY(RibbonStyles ribbonStyle)`（:176）、isThreeRowStyle 等静态判断 | **core**（按 S1.1；flags 声明随迁） |
| `SARibbonBar::RibbonMode` | SARibbonBar.h:211-215 | Q_ENUM :216 | Minimum/Normal 模式切换、calcMainBarHeight 入参 | **core**（按 S1.1） |
| `SARibbonToolButton::RibbonButtonType` | SARibbonToolButton.h:35-47 | Q_ENUM :48 | createItem 按 RowProportion 映射（PanelLayout.cpp:744-745） | **core**（按 S1.1；引擎不需要它，仅 createItem 适配器用，降级留 widgets 代价最小） |
| `SARibbonCustomizeData::ActionType` | SARibbonCustomizeData.h:33-51 | 无 | CustomizeData 记录类型 | **core/data**（随 S4.2） |
| `SARibbonActionsManager::ActionTag` | SARibbonActionsManager.h:36 | — | ActionsManager（v2 §4.3 留守清单） | 留 widgets |
| `SARibbonColorToolButton::ColorStyle` | SARibbonColorToolButton.h:31 | — | colorWidgets（留守） | 留 widgets |
| `SARibbonCustomizeWidget::RibbonTreeShowType` | SARibbonCustomizeWidget.h:60 | — | CustomizeDialog/Widget（留守） | 留 widgets |
| `SARibbonCustomizeWidget::ItemRole` | SARibbonCustomizeWidget.h:76 | — | 同上 | 留 widgets |
| `SARibbonGalleryGroup::GalleryGroupStyle` | SARibbonGalleryGroup.h:109 | — | Gallery（留守，D3） | 留 widgets |
| `SARibbonGalleryGroup::DisplayRow` | SARibbonGalleryGroup.h:119 | — | 同上 | 留 widgets |
| `SAColorToolButton::ColorToolButtonStyle` | colorWidgets/SAColorToolButton.h:78 | — | colorWidgets（留守） | 留 widgets |

.cpp 内部枚举：已核实 `src/SARibbonBar/*.cpp` 中无文件级 `enum` 声明（grep 零命中），"仅 .cpp 用的实现枚举"当前不存在，无需处理。
前置声明注意：`SARibbonThemeManager.h:7` 有 `enum class SARibbonTheme;` 前置声明——SARibbonTheme 入 core 后此处改为 include core 头。

## 附录 B：SARibbonUtil 逐函数去向（S1.2 依据）

`SARibbonUtil.h`（96 行，namespace SA）共 **13 个声明/12 个函数名**（scaleSizeByHeight 两个重载；轮3计数修正，S1 第 3 条的分组统计与此对齐），实现行号为 SARibbonUtil.cpp（全部锚点轮3已逐条真跑复核）：

| 函数 | 声明 | 实现 | 依赖 | 去向 |
|---|---|---|---|---|
| `makeColorVibrant(QColor,int,int)` | h:11 | cpp:28-56 | 纯 QColor（QtGui） | **core** |
| `scaleSizeByHeight(QSize,int)` | h:14 | cpp:58 | 纯 QSize | **core** |
| `scaleSizeByHeight(QSize,int,qreal)` | h:17 | cpp:89 | 纯 QSize | **core** |
| `scaleSizeByWidth(QSize,int)` | h:20 | cpp:126 | 纯 QSize | **core** |
| `iconToPixmap(QIcon,QSize,qreal,Mode,State)` | h:23（**无导出宏**） | cpp:250-258 | QIcon/QPixmap（QtGui） | **core**（补 `SA_RIBBON_CORE_EXPORT`） |
| `widgetDevicePixelRatio(QWidget*)` | h:30（**无导出宏**） | cpp:265-292 | QWidget/QScreen/QApplication | **留 widgets** |
| `saIsRTL()` | h:43 | cpp:305-308 | **QApplication::layoutDirection()**（QtWidgets） | 特殊：core 版改 `QGuiApplication::layoutDirection()`（行为等价）或留 widgets；**引擎一律经 Input.isRTL，不调用它** |
| `saMirrorX(int,int,int)` | h:62 | cpp:327-333 | 内部调 `saIsRTL()` | 特殊：core 版加显式 isRTL 入参（或依赖 core 版 saIsRTL）；纯算术部分可入 core |
| `isOperatingSystemInDarkMode()` | h:65 | cpp:347-382 | QGuiApplication::styleHints/QSettings/QProcess（无 widgets） | **core**（theme 配套） |
| `setEnableSystemDarkModeAutoSwitch(bool)` | h:68 | cpp:418-421 | 函数内静态 bool（cpp:390-394） | **core** |
| `isEnableSystemDarkModeAutoSwitch()` | h:71 | cpp:436-439 | 同上 | **core** |
| `replaceQssTokens(QString,palette)` | h:77 | cpp:447-~500 | QString/QRegularExpression/QColor（技术上无 widget） | **留 widgets**（v2 §3.2 决策：QSS 渲染留 widgets） |
| `getBuiltInRibbonThemeQss(SARibbonTheme)` | h:93 | cpp:153-~230 | QFile/qrc 资源 + QSS | **留 widgets** |

拆分执行注意：现 SARibbonUtil.cpp 顶部 include 了 QWidget/QApplication/QScreen 等（cpp:4-15），拆 core 文件时只带需要的 include；`SA::compat::horizontalAdvance`（SARibbonQt5Compat.hpp:92，算法 cpp:1086 用到）在 Qt5Compat 头中，该头已按计划 01 入 core，无需处理。

## 附录 C：SARibbonPanelLayout 函数级搬移清单（S5 依据）

.h 193 行 / .cpp 1863 行（已核实）。"去向"指 Step B 完成后的归属；**move 引擎**的只有两个大算法函数体。行号为 .cpp 实现起始行（函数体止于下一函数注释前）。

| 函数 | .cpp 行 | 去向 | 说明 |
|---|---|---|---|
| 构造/析构 | 50 / 69-95（轮3收紧行号） | 适配器 | titleLabel/optionBtn 初始化、析构清理 items；Step B 时增补引擎实例创建 |
| `indexByAction` | 96 | 适配器 | QAction 查询 |
| `ribbonPanel` | 117 | 适配器 | parentWidget cast |
| `addItem` | 140 | 适配器 | QLayout 协议（实际拒绝非 PanelItem） |
| `insertAction` | 169 | 适配器 | 结构管理 |
| `setOptionAction` | 201 | 适配器 | 创建 optionBtn（widget） |
| `isHaveOptionAction` | 246 | 适配器 | Input 数据源 |
| `itemAt` / `takeAt` / `count` | 264 / 285 / 319 | 适配器 | QLayout 协议；**takeAt 含缓存清理副作用**（cpp:302-303 remove + widget deleteLater），Step A/B 改为调引擎 `removeFromCache(item)`，不得丢（S5.1-2 不变量） |
| `isEmpty` | 335 | 适配器 | **QLayout 语义**：`mItems.isEmpty()`，与 item 级 isEmpty 无关 |
| `invalidate` | 356-370 | 适配器 | mDirty + 清引擎缓存 + QLayout::invalidate |
| `expandingDirections` | 383 | 适配器 | 恒 Qt::Horizontal |
| `minimumSize` / `sizeHint` | 399 / 418 | 适配器壳 | dirty 时调 updateGeomArray，返回引擎 Result.sizeHint（2.x 二者同值 mSizeHint） |
| `panelItem` / `lastItem` / `lastWidget` / `move` / `isDirty` | 444 / 465 / 484 / 507 / 533 | 适配器 | 结构查询 |
| `updateGeomArray()`（公共无参） | 547-550 | 适配器 | **公共 API 必须保留**，转发 `updateGeomArray(geometry())` |
| `invalidateButtonSizeHintCache()` / `(QWidget*)` | 563 / 579 | 适配器转发 | 转引擎缓存 API；QWidget* 版处置记 NOTES |
| `doLayout` | 593-688 | 适配器 | 重入 RAII guard 612-620；updateGeomArray 触发 621-623；两阶段几何应用 628-640（`itemWillSetGeometry` → widget）；show/hide 批处理 642-665；title 应用+对齐 667-679；optionBtn 应用+iconSize 680-687 |
| `createItem` | 719-782 | 适配器 | 经 RibbonSubElementFactory 造 widget；rp→ButtonType 映射 744-745；automation 名同步 762-771 |
| **`updateGeomArray(const QRect&)`** | **795-1186** | **move 引擎** | 算法主体。Step A 需接口化的 widget 触点：panel cast+panelLayoutMode 797/817-819（→Input.mode）、contentsMargins/spacing 804-805（→Input）、sizeHint 缓存 869-878（→引擎内，key 改 item*）、isEmpty 859（→isHidden）、expandingDirections 902（已契约）、标题 fontMetrics+panelName 1085-1087（→Input.titleTextWidth）、optionActionButtonSize 1096（→Input.optionBtnSize）、saIsRTL/saMirrorX 1136-1157（→Input.isRTL + core saMirrorX）、titleLabel->setAlignment 1160-1167（留适配器）、panelHeightHint 1170（随迁 core，见 S5.2-1）。核心公式：largeHeight cpp:821、smallHeight cpp:831、行 y 坐标 833-839 |
| **`recalcExpandGeomArray`** | **1204-1371** | **move 引擎** | widget 触点：maximumWidth 1238（→契约）、Gallery::stretchFactor 1244-1246（→契约 stretchFactor）；其余（含加权分配 1275-1329、余数补偿 1303-1328）纯计算 |
| `columnWidthInfo` | 1388-1398 | **死代码** | 无调用者；不 move；S8 处置（删或留+NOTES） |
| `panelTitleLabel` / `setPanelTitleLabel` | 1411 / 1546 | 适配器 | widget 存取（含 stackUnder 1552） |
| `setToolButtonIconSize` / `toolButtonIconSize` / `setLargeIconSize` / `largeIconSize` / `setSmallIconSize` / `smallIconSize` | 1431 / 1450 / 1466 / 1482 / 1498 / 1514 | 适配器 | iconSize 数据可入 metrics 字段，API 壳留守 |
| `optionActionButtonSize` | 1530-1533 | 适配器→Input | 纯（12×12 或 mTitleHeight 见方），值作引擎 Input |
| `isEnableWordWrap`/`setEnableWordWrap`/`setButtonMaximumAspectRatio`/`buttonMaximumAspectRatio`/`setLargeButtonMinimumWidthRatio`/`largeButtonMinimumWidthRatio` | 1568 / 1584 / 1625 / 1652 / 1686 / 1713 | 适配器 | 按钮外观属性（createItem 时写给按钮），与装箱算法无关 |
| `panelTitleSpace`/`setPanelTitleSpace`/`panelTitleHeight`/`setPanelTitleHeight`/`isEnableShowPanelTitle`/`setEnableShowPanelTitle`/`largeButtonHeight` | 1731 / 1749 / 1771 / 1789 / 1803 / 1813 / 1826 | 适配器 | 数据壳（值住 metrics/Input；largeButtonHeight 读引擎 Result.largeHeight） |
| **`setGeometry`** | **1831-1863** | 适配器 | **重入守卫 1837-1843（v2 R2 所指 :1831 即此）**；same-rect+dirty 判定 1853；尺寸守卫 1856；调 QLayout::setGeometry + updateGeomArray(rect) + doLayout |

## 附录 D：SARibbonCategoryLayout 函数级搬移清单（S6 依据）

.h 158 行 / .cpp 1425 行（已核实）。

| 函数 | .cpp 行 | 去向 | 说明 |
|---|---|---|---|
| 构造/析构 | 178-189 / 191-199 | 适配器 | （**轮3补行**）ctor：contentsMargins(1,1,1,1)、左右滚动按钮创建+隐藏、connect 186-187、setupAnimateScroll；dtor：takePanelItem 循环清理（含 separatorWidget deleteLater） |
| `PrivateData::collectSizeHints` | 116-172（轮3收紧，旧稿"116-199"误含 ctor 段） | 适配器采集→Input | 单遍收集 totalWidth/canExpandingCount/panelSizes/separatorSizes（`SizeHintCollection` cpp:29-35，结构体定义随引擎入 core 作 Input 载体，收集函数留守——separatorSizes 是 widget 查询，见 S6-2）；Step A 改经契约接口取 sizeHint/isExpanding |
| `ribbonCategory`/`addItem`/`itemAt`/`takeAt`/`takePanelItem`×2/`takePanel`/`count`/`addPanel`/`insertPanel` | 201-347 | 适配器 | QLayout 协议与结构管理 |
| `sizeHint`/`minimumSize` | 348 / 356 | 适配器壳 | 返回缓存 mCachedSizeHint/mCachedMinSizeHint（h:32-33，mutable） |
| `expandingDirections`/`invalidate` | 375 / 380 | 适配器 | — |
| `categoryContentSize` | 408-418 | 适配器→Input | 读 `category->size()` - margins；值作引擎 Input（availableRect） |
| **`updateGeometryArr`** | **431-592** | **move 引擎**（壳留守，见 S6-1） | 触点：category->width/height 437/439、margins 438、collectSizeHints 450、滚动按钮标志判定 469-505（RTL/LTR 分支，纯计算随迁，**轮3注：与 1053-1091 是双实现，收敛为引擎 scrollButtonFlags 单函数，见 S6-3**）、对齐偏移 520-531、逐项几何 534-567（isEmpty 536 = QWidgetItem 语义、隐藏项 separator hide 536-544 **留壳**、isExpanding 554）、RTL 镜像 572-587；**轮3补：非滚动分支 cpp:510 `mXBase = 0` 与尾部 568-570 mTotalWidth/mCachedSizeHint/mCachedMinSizeHint 写入均为适配器状态，经 Result（newXBase/totalWidth/sizeHint/minSizeHint）带出由壳回写** |
| `doLayout` | 605-739 | 适配器 | 重入 guard 621-631；滚动按钮几何（12px 硬编码）646-652；lastVisibleIndex 决策 657-665；两阶段应用（panel/separator 几何 + 分割线 show/hide 决策）666-705；**滚动按钮 setVisible/raise 707-714（轮3补，消费引擎 Result 标志）**；show/hide 批处理 719-738 |
| `panels`/`panelByObjectName`/`panelByName`/`panelByIndex`/`movePanel`/`panelCount`/`panelIndex`/`panelList` | 752-931 | 适配器 | 结构查询 |
| `scroll`/`scrollTo` | 932 / 953 | 适配器壳 | 委托 setScrollPosition |
| `scrollByAnimate`/`scrollToByAnimate` | 971 / 990-1033 | 适配器 | QPropertyAnimation（目标值先经引擎 clamp） |
| `scrollPosition` | 1035 | 适配器 | Q_PROPERTY 读端 |
| `updateScrollButtonVisibility` | 1053-1091 | 适配器壳 | **轮3修正：并非"按钮显隐执行"（按钮 setVisible 在 doLayout:707-714），而是与 469-505 同构的标志重算双实现**（由 updateScrollOffset cpp:1136 调用，滚动期不重算几何只重算标志）——Step B 改为调引擎 `scrollButtonFlags(total, viewportWidth, xBase, isRTL)` 纯函数 + 写 d_ptr 两标志字段（S6-3，R4-禁止双实现） |
| `updateScrollOffset` | 1109 | 适配器 | 平移预计算几何（读 item->mWillSetGeometry 批量 move widget） |
| **`setScrollPosition` 的钳制段** | **1156-1184** | **move 引擎**（`clampScrollOffset`） | RTL `[0, total-available]` / LTR `[available-total, 0]`；**必须带 isRTL 入参**；widget 副作用（updateScrollOffset+update 调用）留适配器 |
| `isAnimatingScroll`/`isScrolled`/`categoryTotalWidth` | 1196 / 1214 / 1230 | 适配器 | — |
| `setCategoryAlignment`/`categoryAlignment`/`setAnimationDuration`/`animationDuration`/`setupAnimateScroll` | 1248-1333 | 适配器 | alignment 值作引擎 Input |
| `onLeftScrollButtonClicked`/`onRightScrollButtonClicked` | 1334+ | 适配器 | 槽（Q_SLOTS） |
| `setGeometry` | 1371-~1408 | 适配器 | **重入守卫（d_ptr->mInDoLayout）**，与 Panel 同型 |
| `SARibbonCategoryLayoutItem` 构造/析构/toPanelWidget | 1413-1425 | 适配器 | Step A 多继承契约接口（见 S6-1） |

## 附录 E：SARibbonBarLayout 度量与几何函数盘点（S3/S7 依据）

.h 157 行 / .cpp 1993 行（已核实）。度量部分详见 S3.0 表格（含 PrivateData 行号），此处列几何/结构函数：

| 函数 | .cpp 行 | 去向（S7） | 说明 |
|---|---|---|---|
| 构造/析构 | 573-576 / 587-596 | 适配器 | （**轮3补行**）ctor 仅建 PrivateData + 调 init()；dtor 清理 d_ptr->items（hide widget + delete item） |
| `init` | 607-610 | 适配器 | **轮3修正：实为空桩**（函数体仅注释"不需要初始化子控件，它们会从ribbonBar获取"）——旧稿"607-696 创建子控件（经 factory）"错误：子控件全部由 SARibbonBar 侧创建持有，且 623-696 区间实为下列四个 QLayout 协议函数 |
| `addItem`/`itemAt`/`takeAt`/`count` | 623 / 641 / 662 / 681 | 适配器 | （**轮3补行**）QLayout 协议，操作 d_ptr->items |
| `sizeHint`/`minimumSize` | 697 / 718 | 适配器（可选下沉） | 字段纯计算（minWidth/minHeight/maxMinWidth） |
| `setGeometry` | 734-738 | 适配器 | 无重入守卫（与 Panel/Category 不同），QLayout::setGeometry+doLayout |
| `doLayout` | 749-756 | 适配器 | 仅按 loose/compact 分派 |
| `isLooseStyle`/`isCompactStyle`/`titleRect` | 769 / 785 / 801 | 适配器 | — |
| `calcMinTabBarWidth` | 819-827 | 见 S3.0（留 widgets 或入参化） | 触碰 tabBar->sizeHint()/tabMargin() |
| `setSystemButtonSize`/`setTabOnTitle`/`isTabOnTitle` | 840 / 856 / 875 | 适配器 | setTabOnTitle 触发 resetSize（860） |
| 六度量 getter + 四 setter | 891-1046 | 适配器壳（S3） | 转发 mMetrics |
| `setWindowIcon`/`windowIcon`/`setApplicationButtonVerticalExpansion`/`isApplicationButtonVerticalExpansion` | 1059-~1130 | 适配器 | widget 操作 |
| 元素 getter（ribbonBar/ribbonTabBar/stackedContainerWidget/quickAccessBar/rightButtonGroup/applicationButton/titleIconWidget） | ~1130-1241 | 适配器 | — |
| **`layoutTitleRect`** | **1242-1366** | **引擎（Input 结构方式）** | 非纯：读 4 个控件几何 + contextTabIndexs + systemButtonSize + margins + saIsRTL + style 分支；四分支（RTL/LTR × Compact/Loose）逻辑一行不改 |
| `resetSize` | 1377-1380 | 适配器 | 转发 d_ptr->resetSize（494-519：estimateSizeHint+calcMainBarHeight+setFixedHeight；公式入 metrics，控件操作留守） |
| `layoutStackedContainerWidget` | 1391-1429 | 适配器 | widget 摆放 |
| `layoutCategory` | 1430-1445 | 适配器 | widget 摆放 |
| `resizeInLooseStyle` | 1446-1742 | **留 widgets**（D6） | 约 300 行 widget 摆放（quickAccess/appButton/tabBar/context/systemButtons/stacked 全部实控件） |
| `resizeInCompactStyle` | 1743-1993 | **留 widgets**（D6） | 约 250 行，同上 |

## 附录 F：Tier2 结构控制器接口预案（评审轮2新增；**仅 3.1+ D7 gate 触发后生效，M1 不实施**）

目的：若 Tier2 做 RibbonBar/Category/Panel 的结构控制器下沉（v2 D7），接口形态照此预案，避免届时重新发明。依据 KDDW `core/views/*ViewInterface` 族的实测形态（2.0.1）：

1. **每控制器一个窄接口，只含控制器需要命令视图的方法**。KDDW 实测规模：DockWidgetViewInterface 2 个虚函数、SideBar 3、MainWindow/Stack/TitleBar 各 4、Group 5、TabBar 11（`grep -c virtual src/core/views/*ViewInterface.h`）。SARibbon 对应示例（Category）：
   ```cpp
   // 3.1 才建：src/core/contract/SARibbonCategoryViewInterface.h
   class SARibbonCategoryViewInterface
   {
   public:
       explicit SARibbonCategoryViewInterface(SARibbonCategoryController* c);  // ctor 收控制器并存 protected const 成员（KDDW TabBarViewInterface.h:33,57 形态）
       virtual ~SARibbonCategoryViewInterface();
       virtual void applyPanelGeometry(int index, const QRect&) = 0;   // 控制器算好，视图只执行
       virtual void setScrollButtonsVisible(bool left, bool right) = 0;
       virtual void setScrollButtonsGeometry(const QRect& l, const QRect& r);  // 可选能力给默认实现（KDDW :36-38 setTabsAreMovable 形态）
       virtual int nonContentHeight() const = 0;                        // 视图回供控制器的唯一"平台数值"（对照 KDDW GroupViewInterface.h:36 唯一纯虚）
       virtual QString panelText(int index) const { return {}; }        // 测试专用方法允许进接口但必须注明（KDDW TabBarViewInterface.h:40-42 text() 先例）
   protected:
       SARibbonCategoryController* const m_controller;
   };
   ```
2. **控制器消费方式**：KDDW 控制器持通用 View、用 `dynamic_cast<XxxViewInterface*>(view())` 取窄接口（TabBar.cpp:70,158）。SARibbon 无 god-View（v2 §2.3 已否决 Core::View 巨型接口），控制器直接持对应窄接口指针即可，**不需要 dynamic_cast 层**。
3. **视图创建走 factory**：接口定义放 core、实现类留 widgets/QML，经 `SARibbonElementFactoryInterface` 创建（KDDW ViewFactory.h:68-136 + Config::setViewFactory 可整体替换的形态；S4.3 占位头即其衔接点）。
4. **不抄的部分**：KDDW 窄接口建立在 god-View + Controller 基类（Controller.h:48-103）之上，且信号用 KDBindings 以兼容 Flutter——SARibbon 双前端皆 Qt，用 Qt 信号槽；控制器基类是否需要，等 D7 触发时按实际控制器数量再定，不预建。

