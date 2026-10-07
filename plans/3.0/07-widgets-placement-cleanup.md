# 计划 07：widgets 放置属性清理——废除 `_sa_*` 动态属性通道

> **对应契约**：D3 的 widgets 侧落地（命令属性与放置属性分离）
> **前置**：无（可与计划 05/06 并行；合入 dev-3.0 时注意与 QML 侧计划的文件不相交）
> **全程纪律**：开工前通读 [00-architecture-contract.md](00-architecture-contract.md)；遵守 AGENTS.md（**涉及 src/widgets 改动，提交前必须跑 `tools/Amalgamate.sh`** 更新合并文件；禁止触碰 `src/SARibbon.cpp/.h` 本体）

## 1. 目标

把"放置参数经 QAction 动态属性走私"的 2.x 通道废除，改为**添加调用参数直达 SARibbonPanelItem**；对外 API 形态不变（除被废除的属性读写静态函数族）。

## 2. 背景与病灶

- **现状**：`addMediumAction(act)` = `setActionRowProportionProperty(act, Medium)` + `QWidget::addAction`；布局在 `actionEvent` → `insertAction` 时读回属性（写入点 `SARibbonPanel.cpp:226/273/321`，属性名宏 `core/SARibbonEnums.h:146-154`）
- **病灶**：放置语义成了 QAction 上的**共享可变状态**——同一 action 进两个面板给不同比例时后写覆盖先写，未爆雷只因"插入时读取一次"的时序巧合；且外部裸 `QWidget::addAction` 与便捷方法走同一读取路径，语义耦合在属性副作用上
- **佐证**：契约 D3 横评（Office Ribbon XML / WPF Ribbon / QToolBar 三家成熟设计均不把"此处多大"放命令对象上）
- **病灶另一面（顺带修复）**：`ActionChanged` 路径重读属性意味着"运行期改动态属性会移动按钮行位"——语义上就不该存在

## 3. 执行步骤

### S1 调用面清点（先清单后动手）

全库 grep 以下符号，产出迁移清单（含示例、测试、文档）：

- `_sa_RowProportion` / `_sa_ToolButtonPopupMode` / `_sa_ToolButtonStyle`（含 `SA_RIBBON_BAR_PROP_*` 宏引用）
- `setActionRowProportionProperty` / `getActionRowProportionProperty` / `setActionToolButtonPopupMode` / `getActionToolButtonPopupMode` / `setActionToolButtonStyle` / `getActionToolButtonStyle`（`SARibbonPanel.h:345-356` 一族）
- `SARibbonBar.h:63-64` 的过时文档块（描述的是 2.x 旧签名）

### S2 传输通道重构（pending 表方案）

保持 `QWidget::addAction` 作为 action 进入面板的传输通道（保留 actionEvent 的自动状态同步与 `actions()`/`removeAction` 语义），放置参数改走**面板私有 pending 表**：

- SARibbonPanel 私有：`QHash<QAction*, Placement> mPendingPlacement`（RowProportion + PopupMode + ToolButtonStyle 打包结构）
- `addAction(QAction*, rp[, popupMode])` → 记入 pending → `QWidget::addAction` → `actionEvent(ActionAdded)` → 查 pending → `insertAction(index, action, placement)` → **摘除 pending**
- 外部裸 `QWidget::addAction(action)`（未经便捷方法）：pending 无记录 → 默认 Large / InstantPopup / IconOnly（与现默认行为完全一致）
- `ActionChanged` **不再读取放置属性**：item 自持放置值，运行期属性变化不再影响行位（修复 §2 的隐性语义）
- 移除路径（`QWidget::removeAction` → ActionRemoved）不受影响；`actionIndex` / `moveAction` 不变
- `SARibbonPanelLayout::createItem` 从 item 取放置参数（现状 `:763-764` 从 action 属性恢复 popupMode 的逻辑迁到 item）
- 每面板独立 pending 表 = **每次放置独立记录**，同一 action 双面板不同比例结构性成立（契约 D6"每次放置 = 独立视图"）

### S3 公开 API 删除与保留

- **删除**：`set/getActionRowProportionProperty`、`set/getActionToolButtonPopupMode`、`set/getActionToolButtonStyle` 静态函数族与三个 `_sa_*` 宏（`core/SARibbonEnums.h:146-154`）
- **保留**（语义本就是"添加时给放置参数"，正确）：
  - `addLargeAction` / `addMediumAction` / `addSmallAction`（含 popupMode 重载）
  - `addAction(QAction*, rp[, popupMode])`
  - 便捷创建 `addAction(text, icon, popMode, rp)`
- **`_sa_isCanCustomize`（`SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE`，action 级）归置**：这是命令级标志（"此命令可否被定制"），不是放置——迁入 `SARibbonActionsManager` 注册数据（`registeAction` 扩展 canCustomize 参数或独立设置 API）；category/panel 的 `isCanCustomize` 已是真 Q_PROPERTY，不动
- `SARibbonBar.h:63-64` 过时文档块修正
- 2.x→3.0 迁移文档补破坏性变更条目："先 `setActionRowProportionProperty` 再 `addAction`"的用法废除，改传参

### S4 测试与示例

- **新增回归用例（结构性修复的固化）**：同一 QAction 进两个面板、分别 Large 与 Small，互不干扰；这是旧通道下理论上就会错的场景
- 黄金测试 / 一致性套件全绿；MainWindowExample 按 S1 清单清点（若示例未用公开属性函数则零改动）
- 顺带核验按钮 tooltip 的 action 来源链与 QML 派生链对齐（计划 06 S6 的跨端期望在 widgets 侧的锚点）

## 4. 验收门

1. 全库无 `_sa_RowProportion` / `_sa_ToolButtonPopupMode` / `_sa_ToolButtonStyle` 残留（含 core 宏与 amalgamate 产物——跑 Amalgamate 后合并文件同步）
2. 双面板双比例回归用例绿
3. widgets 全部现有测试 / 黄金快照绿（视觉零回归）
4. 定制器 round-trip 不受影响（记录中 `actionRowProportionValue` → `panel->addAction(act, rp)` 签名未变）

## 5. 风险与缓解

| 风险 | 缓解 |
|------|------|
| pending 表与 actionEvent 时序耦合（如添加后立刻 remove、外部裸 addAction 插队） | 用例覆盖"添加即删""裸 addAction 默认值""连续便捷添加"三场景 |
| 2.x cherry-pick 冲突面 | 本计划落地后，来自 dev(2.x) 的 fix 合并须注意通道差异（AGENTS.md 已有"布局引擎 fix 手动同步 core 版"的既成要求，扩展到本通道） |
| isCanCustomize 迁移遗漏动态属性读取方 | S1 清单含 `SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE` 全部 grep |
