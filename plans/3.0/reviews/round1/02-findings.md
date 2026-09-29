# 计划 02（core 下沉与布局引擎）评审记录 — 第 1 轮

视角：事实准确性与可执行信息完备度。评审对象：`plans/3.0/02-core-sinking.md`（修订前 209 行版本）。
方法：逐条主张对照 2.9.5 基线源码（`src/SARibbonBar/`）实读核实；所有行号以 dev 分支当前基线为准。
处理约定：错误→已直接改入 02 文档；缺失→已补充（含附录 A–E）；存疑→文档内标"待核实"并给命令。

## 一、核实通过（未改动，列出以证已验）

| 主张 | 证据 |
|---|---|
| P3 三布局行数 1863/1425/1993 | `wc -l`：SARibbonPanelLayout.cpp=1863、SARibbonCategoryLayout.cpp=1425、SARibbonBarLayout.cpp=1993，**精确相等**（原文"≈"已改为"=="并注明基线漂移即需重录行号） |
| P4 度量 grep 命中 6 处 | 对 SARibbonBarLayout.h 实跑恰 6 行（h:66/89/92/96/108/114），setter 行因大写 `TabBarHeight` 不被小写 pattern 命中 |
| `SARibbonGlobal.h:197/220/245` 三枚举 | 实读确认：SARibbonAlignment(:197)、SARibbonTheme(:220)、SARibbonMainWindowStyleFlag(:245)，均自由 enum class，附 Q_DECLARE_METATYPE/FLAGS |
| `SARibbonPanelItem.h:36` RowProportion、`:59-60` 属性名宏 | 实读确认，宏值 `"_sa_RowProportion"`；同族宏 :62-63、:65-66 |
| SARibbonPanelItem 已是 QWidgetItem 子类 | PanelItem.h:24；且无 Q_OBJECT（多继承无 moc 障碍） |
| `mButtonSizeHintCache`/`SARibbonPanelLayout_DEBUG_PRINT` 实名 | PanelLayout.h:174-175；PanelLayout.cpp:14-15（默认 0），伴生宏 HELP_DRAW_RECT cpp:27 |
| v2 R2 "PanelLayout.cpp:1831 setGeometry 重入守卫" | cpp:1831 函数起始、1837 `if (mInDoLayout)` 守卫体，行号精确 |
| `makeColorVibrant`/`widgetDevicePixelRatio` 在 SARibbonUtil | Util.h:11/:30，实现 cpp:28/cpp:265 |
| SARibbonThemePalette 无 widget 依赖 | h/cpp include 仅 QtCore+QtGui（QColor/QHash/QJson/QFile） |
| 文档引用的测试名全部存在；tests/*.cpp 共 25 个 | `ls tests/*.cpp`；9 个被引用测试逐一命中 |
| `.github/workflows/` 6 个构建 workflow | cmake-{linux,mac,win}-qt{5.15,6.8}.yml 恰 6 个 |
| ctest 命令与 README R2 一致 | README.md:44/47 与 02 文档 P2/§4 相同 |
| "计划 01 S8 模式"交叉引用 | 01-infra-restructure.md:198 `### S8 amalgamate 应急适配` 存在 |
| v1 悬空引用 | `grep -n "v1" 02-core-sinking.md` 零命中，本文档无 v1 引用需处理 |
| 已知偏差"Pannel 单 n" | 仓库文件名核实均为 SARibbonPanel* |

## 二、发现与处理明细

格式：位置 | 类型 | 证据 | 处理。

1. §1-3、S3.1、S3.2 | **错误** | "六个**私有**度量函数""**纯 move**"：六函数全在 public 段（BarLayout.h:66-116）；`calcMinTabBarWidth`（cpp:819-827）直接调 `tabBar->sizeHint()`/`tabMargin()`；默认值推导 `calcDefaultTabBarHeight/TitleBarHeight`（cpp:378-391/404-413）依赖 `QStyle::pixelMetric`（QtWidgets，core 禁区） | 已改写 S3.0 实况表：五函数是 PrivateData 一行转发（getActual* cpp:283-330 纯字段读、calcMainBarHeight cpp:466-483 静态纯函数），三个推导公式入参化后方可下沉，calcMinTabBarWidth 二选一处置（留 widgets 或 metrics 纯函数 + 适配器采集）
2. S1 | **错误** | `RibbonButtonStyle` 全仓不存在；实名 `SARibbonToolButton::RibbonButtonType`（ToolButton.h:35，Q_ENUM :48）。`ToolButtonPopupMode` 是 Qt 原生 `QToolButton::ToolButtonPopupMode`（Panel.h:175 等 15 处使用），非 SARibbon 枚举，不可下沉；SARibbon 只有属性名宏（PanelItem.h:62-63）。`SARibbonApplicationButton.h` 无任何枚举（grep 零命中）。"BarMode"实名 `SARibbonBar::RibbonMode`（Bar.h:211） | 已改写 S1.0 实况修正 + 附录 A 全量表；属性名宏三个一并列明
3. S1 盘点命令 | **错误** | `git grep -n "^enum class\|	enum class" src/widgets --include="*.h"`：`--include` 非 git grep 合法参数；pattern 只匹配 `enum class`，漏掉 16 处枚举中的 12 处普通 `enum`（含带缩进的嵌套枚举） | 已替换为验证过的 `git grep -n -E "^\s*enum (class )?[A-Za-z]" -- "src/widgets/*.h"`，产出固化为附录 A
4. S2 全节 | **错误** | "ThemeManager 改为薄壳""`applyRibbonTheme(QWidget*, theme)` 系列签名不变""isDarkTheme 等"：2.9.5 **没有 SARibbonThemeManager 类**，只有 `SA::applyRibbonTheme` 两个自由函数（ThemeManager.h:15/:18-19，真实签名含 `SARibbonBar*` 与 palette 重载）；无单例、无 themeChanged 信号、无 isDarkTheme 函数（只有 `SARibbonThemePalette::isDark()`，ThemePalette.h:157）；主题状态分散在每窗口（MainWindow.h:111 Q_PROPERTY、Widget.h:20） | 已重写 S2.0/S2.1：可下沉物=cpp 内 5 个静态表+2 个高亮 lambda（cpp:24/38/43/48/62/76，逐一给行号）；ThemeData 单例标注为 3.0 新引入；`FpContextCategoryHighlight` 别名需先提升到 core；允许 M1 只做数据表 move、信号驱动重构延到计划 04（记 NOTES）
5. S2.1 | **错误**（笔误） | "`Q_SIGNAL themeChanged(...)`"——Qt 无 Q_SIGNAL 宏用于信号段（Q_SIGNAL 是 cast 宏），且本项目规定 Q_SIGNALS | 已改为 `Q_SIGNALS:` 段并加粗提示
6. S5.1-1 | **错误**（高危） | "`isHidden()` = `widget()->isHidden() && !widget()->isWindow()`（照抄 2.x isEmpty() 语义）"：那是 **QWidgetItem::isEmpty 的默认语义**；2.x `SARibbonPanelItem` 覆盖了它——`isEmpty() { return action == nullptr || !action->isVisible(); }`（PanelItem.cpp:43-46），算法判断的是 **action 可见性**。照抄旧稿会把"action 隐藏但 widget 未显式隐藏"的项错误纳入布局，黄金测试必炸且难归因 | 已修正 S5.1-1 并在 S4.1/S6-1 注明：Panel 侧 isHidden=action 可见性，Category 侧（未覆盖 isEmpty，CategoryLayoutItem 用 QWidgetItem 默认语义）=widget 隐藏，**两侧语义不同、不得统一**
7. S8-2 | **错误** | 清扫判据 `grep "updateGeomArray..."` 无效：`updateGeomArray()` 是公共 API（PanelLayout.h:61），按 v2 §4.4 兼容承诺必须保留转发壳，按名 grep 永远命中 | 已改为算法体内部标记物判据（columMaxWidth/yMediumRow/columnExpandInfo/spacingRow 零命中）
8. S9 | **错误** | `ctest -R "core"` 假绿：现有测试以文件名注册（tests/CMakeLists.txt:32 `add_test(NAME ${TEST_NAME})`），无 "core" 字样，-R 匹配为空时 ctest 仍返回 0 | 已改为 LABELS 方案（`-L core` + set_tests_properties），并在 S5.0-1 固化注册名约定
9. §1 首句 | **错误** | "六个子系统"随后列 7 项（global/theme/metrics/contract/layout/data/factory；v2 §3.1 目录亦 7 个） | 已改"七个子系统"
10. S3.1 常量盘点命令 | **错误** | `git grep ... "mTabBarHeight\|rowSpacing" --include=...`：`mTabBarHeight`/`rowSpacing` 实名不存在（实际字段 `tabBarHeight` 等无前缀，BarLayout.cpp:23-26；行间距是局部常量 `spacingRow`，PanelLayout.cpp:806）；`--include` 同第 3 条无效 | 已替换为验证过的命令 + 实测默认值表（spacing=2 来自 Panel.cpp:85 `setSpacing(2)`，**v2 §3.3 示例 spacing=1 与实测不符**，已标注以 2 为准）
11. S1（Q_ENUM 耦合） | **缺失**（高危） | PanelLayoutMode/RibbonMode/RibbonStyleFlag/RibbonButtonType 是类内 Q_ENUM（Panel.h:158、Bar.h:204/216、ToolButton.h:48），其中 `Q_PROPERTY(SARibbonPanel::PanelLayoutMode ...)`（Bar.h:186）、`Q_PROPERTY(RibbonStyles ribbonStyle)`（Bar.h:176）直接耦合；**Q_ENUM 不能作用于 using 别名**，"core 自由枚举+类内别名"必然删除类内 Q_ENUM，改变元对象枚举表 | 已新增 S1.1 五步策略（含 Q_NAMESPACE/Q_ENUM_NS 补反射、QMetaEnum 消费者 grep、sip 冒烟）+ 明确降级出口（四枚举留 widgets，不阻塞 M1）
12. S5/S6/S7（函数级搬移清单） | **缺失**（本轮最大补充） | 执行 agent 需要"哪些函数 move、哪些留守"的逐函数依据；原文只有算法名 | 已新增附录 C（PanelLayout 约 40 个函数，含 .cpp 行号与 widget 触点行号）、附录 D（CategoryLayout 全函数+scroll 钳制段 1156-1184）、附录 E（BarLayout 度量+几何全函数）
13. S4.1（契约接口缺口） | **缺失**（高危） | v2 §3.4.1 五个纯虚不够：`item->widget()->maximumWidth()`（PanelLayout.cpp:1238，列扩展上限）、`SARibbonGallery::stretchFactor()`（cpp:1244-1246，issue #47 加权）无契约对应；`SA::saIsRTL()` 实现用 `QApplication::layoutDirection()`（Util.cpp:307，QtWidgets）core 禁调，isRTL 必须是 Input；契约草案字段 `QRect geometry` 与 `QLayoutItem::geometry()` 在双继承下**同名歧义、编译失败**；Category item 有 panel+separator **两块几何**（CategoryLayout.h:155-156） | 已在 S4.1 列 4 条修订：补 maximumWidth()/stretchFactor() 纯虚、geometry 字段改名（建议 resultGeometry）、Category 扩展结构、Input 值传 isRTL/margins/spacing/titleTextWidth/optionBtnSize
14. S5.1-3（标题伪项） | **缺失/偏差** | v2 示意"标题/optionButton 作伪项一并传入引擎"与实况不符：2.x 标题几何在主循环**之后**特殊计算并回抬 totalWidth（cpp:1080-1092，依赖 `mTitleLabel->fontMetrics()`+`panel->panelName()`），optionBtn 几何依赖标题几何（cpp:1095-1122）；伪项化进循环=改算法结构，违反纯 move | 已改为 Input.titleTextWidth/optionBtnSize + Result.titleGeometry/optionBtnGeometry 方案，并明令不得伪项化进装箱循环；`optionActionButtonSize`（cpp:1530-1533）核实为纯函数
15. S5.2-1（panelHeightHint） | **缺失** | 引擎 sizeHint 依赖 `SARibbonPanel::panelHeightHint`（cpp:1170 调用；Panel.h:344/cpp:1587-1608）——未列入任何搬移清单 | 已核实为静态纯函数（fm.lineSpacing×系数+titleHeight），写入 S5.2-1：随引擎入 core，SARibbonPanel 留转发
16. S6（clampScrollOffset 签名） | **错误/缺失** | v2 草案 `clampScrollOffset(requested, contentWidth, viewportWidth)` 缺 isRTL：实际钳制 RTL=[0,total-available]、LTR=[available-total,0]（setScrollPosition cpp:1156-1184），且滚动按钮可见标志判定（cpp:469-505）也分 RTL/LTR | 已在 S6-3 修正签名（+bool isRTL）并把标志判定划入引擎 Result
17. S6（Category 重入守卫） | **缺失** | CategoryLayout 也有一套 `mInDoLayout` setGeometry 重入守卫（cpp:1371 起；RAII guard cpp:623-631），原文守卫留守只写了 Panel | 已补入 S6-3 与风险表
18. S5.2-3（行数目标） | **存疑→修正** | "≤300 行量级"不现实：算法体（795-1186、1204-1371 共约 560 行）移出后，仍余约 40 个属性存取/QLayout 重写/createItem/doLayout（附录 C 清单），实测远超 300 | 已改为"行数不作硬门，以 S8-2 算法体零残留为准"（v2 的 <300 行是对 doLayout 适配逻辑的估计，注明出处）
19. S5.2-2（死代码） | **缺失** | `columnWidthInfo`（h:165/cpp:1388-1398）已无任何调用者（recalc 优化后废弃，仅注释提及） | 已写明：不 move；S8 删除（单独提交）或保留+NOTES
20. S1（RowProportion 默认值） | **缺失** | PanelItem 构造初始化 `rowProportion(Large)`（cpp:15），v2 契约草案字段默认 None——不一致会在遗漏赋值点时改变行为 | 已写入 S1：契约字段默认值对齐 Large 或 NOTES 记录；现有 createItem 均显式赋值（cpp:778）
21. S4.1-3（itemWillSetGeometry 兼容） | **缺失** | `SARibbonPanelItem::itemWillSetGeometry/rowIndex/columnIndex/isExpandItem` 是导出类公有字段（事实 API），契约基类字段名不同（geometry） | 已给两方案（引用成员绑定 / 记录为 3.0 允许破坏项），NOTES 定稿
22. S5.0-3（字体方案） | **缺失→细化** | 现有 25 个测试均未固定字体（grep SimSun 零命中）；`QFont("SimSun",9)` 在 linux/mac CI 必 fallback，录制值不可复现 | 已细化：`tests/core/fonts/` 部署开源字体 + `QFontDatabase::addApplicationFont` + `QFontInfo(f).family()` 断言防静默 fallback + `QT_QPA_PLATFORM=offscreen`；并澄清**引擎级 fixture 输入是显式尺寸、零字体依赖**（字体只影响录制工具产出的输入值）
23. S5.0-3（读取路径） | **存疑→已证实可行** | `SARibbonPanelItem::geometry` 读取：QWidgetItem 继承的 geometry() 需布局激活后有效；更稳妥路径是公共 `updateGeomArray()`（h:61）后直接读公共字段 `itemWillSetGeometry`（h:53） | 已把两条路径与前置条件（show+processEvents 或 offscreen）写入 S5.0-3
24. P2/P5（前置命令） | **缺失** | `build.ps1` 的 `-Tests` 默认 OFF（scripts/build.ps1:27），不带 `-Tests ON` 时 ctest 基线为空；`tools/check_core_purity.py` 当前不存在（计划 01 S9 交付物，tools/ 现仅 Amalgamate+绑定脚本） | 已在 P2/P5 注明（含 README R2 的 rebuild 命令引用）
25. S3.2（字体/触发点） | **缺失** | 度量重算触发点未落实：`resetSize()` 由 SARibbonBar.cpp 9 处调用，其中**字体变化在 `SARibbonBar::changeEvent` 的 `QEvent::FontChange` 分支（cpp:4118）**；且用户 setter 有 bar 级（`SARibbonBar::setTabBarHeight(int,bool)` cpp:1689 等）与 layout 级两套 | 已把 9 个调用点行号、两级 setter 实名写入 S3.2，"触发点一律不动"
26. S4.2（CustomizeData 字段清单） | **缺失→已补** | 原文只有口头描述"无 QWidget 指针部分" | 已核实并写入：纯数据=ActionType（h:33-51，16 值）+indexValue/keyValue/categoryObjNameValue/panelObjNameValue/actionRowProportionValue（h:159-200）+mType+isValid()（cpp:68-71 纯）；widget 依赖=唯一指针成员 `mActionsManagerPointer`（h:203）+apply（cpp:86）+8 个带 manager 的 make* 工厂；无 QWidget*/QAction* 直接成员；头 include 依赖（h:4-5）需在 core 版摆脱
27. S4.2（simplify 纯度） | **存疑** | `simplify()`（cpp:926）是否只读纯字段未逐行核 | 文档已标"待核实"并给核实命令（`sed -n '926,1000p' ...`）
28. S4.3（factory 成本评估） | **缺失→已补** | 原文"若评估成本高允许降级"未给评估依据 | 已核实并写入：17 个虚 create 函数全返回 widgets 类型（ElementFactory.h:42-83 逐一列名）、单例 `SARibbonElementManager::instance()`（h:57）+宏 RibbonSubElementFactory（h:69-70）；确认默认执行降级路径
29. 附录 B（Util 拆分依据） | **缺失→已补** | 原文只给盘点命令（且 `git grep "QWidget\|QApplication"` 会把纯函数所在的整文件误判） | 已逐函数核实 13 项去向表（core 7 / widgets 3 / 特殊 2 / QSS 相关留 widgets 2），含导出宏缺失警示（iconToPixmap h:23、widgetDevicePixelRatio h:30 无 SA_RIBBON_EXPORT）
30. 行号基准约定 | **缺失** | 文档全部命令用计划 01 后的 `src/widgets/` 路径，但评审/执行时可能仍在 `src/SARibbonBar/`；行号出处未声明基线 | 已在 §3 前置条件后新增"本文行号约定"块（2.9.5 基线，git mv 不改行号）

## 三、统计

- **错误**（文档主张与源码不符，已直接改正）：10 条（明细 1、2、3、4、5、6、7、8、9、10、16 前半）
- **缺失**（信息不足以让执行 agent 免猜，已补充）：17 条（明细 11–15、17–26、28、29、30）
- **存疑**（已标注并给核实路径）：3 条（明细 18 已按保守方案修正、23 已证实、27 待核实）
- 核实通过未改动：16 项（见第一节）

## 四、建议后续轮次关注

1. **契约接口定稿评审**：S4.1 修订后的接口（maximumWidth/stretchFactor/resultGeometry 命名/Category 双几何扩展结构）是全计划的地基，建议下一轮对照 KDDockWidgets `src/core/layouting/` 的 Item/SizingInfo 再做一次接口形状评审（本轮只保证了"能承载 2.x 算法"）。
2. **Q_ENUM 降级决策落地**：S1.1 允许四个嵌套枚举留 widgets——若真降级，S5/S6 引擎 Input 中 mode/RibbonMode 的类型表达（core 枚举 vs int）需要与计划 04（QML 注册）对齐，避免两次返工。
3. **simplify()/isCanCustomize 纯度终判**（本轮遗留的唯一"待核实"）。
4. **度量对照表的录制脚本**：S3 验证要求"临时 main 导出度量值"，建议评审其是否值得固化为 `tools/dump_metrics.cpp` 与 dump_panel_geometry 共用工程。
5. **Step A 期间的双状态风险**：`itemWillSetGeometry`（widgets 字段）与契约 `resultGeometry` 并存期间的一致性（S4.1-3 引用成员方案若被否，需要明确单一数据源），建议下一轮检查执行 agent 的实际选择。
6. **与计划 03 的接口**：tests/core 独立工程 + LABELS core 的 CMake 写法需与计划 03 的构建生态（preset/包配置）核对，防止 `-R/-L` 约定在 03 中被覆盖。
