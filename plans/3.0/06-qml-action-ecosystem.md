# 计划 06：QML 命令生态——注册表/定制器/菜单树/示例全量 action 化

> **对应契约**：D1 / D2 / D6 落地到定制体系与全部生态位
> **前置**：计划 05 完成（按钮已可绑定 QAction）
> **全程纪律**：开工前通读 [00-architecture-contract.md](00-architecture-contract.md)；遵守 AGENTS.md；QML 模块改动无需 Amalgamate

## 1. 目标

把计划 05 建立的命令层接入 QML 的全部生态位：注册表/定制器、菜单树、Gallery、快速访问栏定制、示例与跨端一致性。完成后 QML 模块的"命令身份"不再有任何字符串 key 补登记的影子。

## 2. 非目标

- core 的 SARibbonCustomizeRecord / XML 格式：**不动**（已无 QAction 依赖且与 widgets 格式互读，契约 D5——这是 core 契约纯净性的现有成果，本计划只换寻址层）
- RibbonControlContainer 及 CheckBox/ComboBox/SpinBox 等嵌入控件：**不 action 化**（对应 widgets addWidget 语义，两级语义中的纯声明层）
- widgets 侧 → 计划 07

## 3. 执行步骤

### S1 RibbonActionRegistry → QAction 寻址

- 内部改 `QHash<QAction*, ...>`，key = `action->objectName()`（契约 §5）
- `registeAction(QAction*, tag, key = QString())`：key 缺省取 objectName；**无 objectName 拒绝注册**（对齐 widgets ActionsManager，含 `id_N_M` 自增盐生成规则——直接移植 widgets 实现）
- RibbonActionDescriptor 重构：`QAction*` + tag + 活动宿主项列表（保留现状字段）；`bindItem` API **废除**（绑定关系已由按钮 action 属性直接表达，无需旁路）
- `autoRegister(bar)` / `autoRegisterBar(bar)`：遍历改为收集 action 绑定按钮（纯声明按钮跳过——两级语义，契约 D6）
- RibbonActionRegistryModel 同步改造

### S2 RibbonCustomizer / RibbonCustomizeTreeModel

- **记录层不动**（core，key 持久化语义 = objectName）；寻址层 key → `registry->action(objectName)`
- `addAction(key, ...)`：注册表取 QAction → 创建绑定该 action 的 RibbonToolButton → attach（"归定制器所有的模板按钮"模式保留，按钮 action 化）
- `addQuickAction` / `removeAction` / `changeActionOrder` 同理改造
- XML 读写回归：与 widgets 定制文件**互读**（验收门 1）

### S3 菜单树 QAction 化

- RibbonToolButton 宿主：`menuItems(QVariantList<RibbonMenuItem*>)` → `menuActions(QVariantList<QAction*>)`
- RibbonMenu.qml 从 QAction 渲染：text / icon / shortcut **文本** / checkable / checked / enabled
- 子菜单：`RibbonAction.menuActions` 嵌套（契约 §5；裸 QAction 限平面菜单，文档化）
- 应用菜单（RibbonBar 应用按钮）同链路
- `activateMenuItemPath` → 直接 `action->trigger()`
- **RibbonMenuItem 类型退役**（3.0 未发布，直接删除，不留兼容别名；S3 开工前全库 grep 使用点）
- 菜单勾选互斥：QActionGroup 天然支持（与 widgets Gallery 内部机制同构）
- 快捷键列显示的文本与 S5（计划 05）触发机制同源自 `QAction::shortcut`

### S4 Gallery 命令化（两级语义）

- RibbonGalleryGroup 增加 actions 填充路径（对齐 widgets `addActionItem(QAction*)`）
- RibbonGalleryItem 增加可选 `action: QAction*` 绑定（派生规则同按钮，计划 05 S4 同款）
- 纯声明 gallery item 保留（addWidget 语义）

### S5 示例全量重写（标准范式）

QmlMainWindowExample：

- 命令集中声明：共享 RibbonAction 集（QML 内声明）+ C++ 后端 QObject 暴露 `Q_PROPERTY(QAction* ...)`（两种范式都演示——后者即迁移故事的活样张）
- 面板 / 快速访问栏 / 菜单全部 action 绑定 + `proportion` 放置
- 动态插入演示改为 `panel.addAction(act, Ribbon.Large)`（计划 05 S6 API）
- 保留少量纯声明按钮演示两级语义（对应 widgets 示例的 addWidget 演示位）
- example README 更新：删除"QML 版没有 action 桥（plan-04 D8 延后项）"的历史说明

### S6 跨前端一致性套件扩展

- RibbonConformance 场景数据增加：action 派生属性 / 共享状态同步 / 菜单-面板共享命令 / 快捷键触发
- **tooltip 链路两前端对齐核验**：widgets 侧按钮 tooltip 的 action 来源语义 vs QML 派生链，不一致则以场景数据固化期望
- 黄金快照按需重录（视觉预期不变则只加用例不加快照）

### S7 文档

- `docs/zh/dev-guide/qml-guide.md` 全面重写：命令层 / 两级语义 / 放置契约 / 后端驱动范式
- 新增迁移指南（widgets→QML / QML→widgets，以契约 §3 迁移故事为骨架）
- docs/en 对应页同步（如存在）

## 4. 验收门

1. 定制器全流程（增/删/挪/快速访问/序列化 XML → 重载）在 action 化示例上可用；**QML 示例可直接应用 widgets 示例导出的 customize.xml**（key=objectName 互认）
2. 菜单与面板共享同一 QAction：勾选 / 禁用 / 文本单点同步
3. 契约 §3 迁移故事以一致性测试形式固化（场景数据驱动，两前端跑同一份期望）
4. 示例中无任何"手动同步两按钮状态"的命令式代码残留（现 main.qml:384-391 注释承认的 imperative sync 全部清除）
5. 全库无 `RibbonMenuItem` / `menuItems` / `bindItem` 残留

## 5. 风险与缓解

| 风险 | 缓解 |
|------|------|
| 定制记录 key 语义变化（旧 QML registry key 生成规则 vs objectName） | 3.0 未发布，直接切换无兼容包袱；文档写明 key=objectName |
| RibbonMenuItem 删除波及面（示例/叶子/宿主/定制器） | S3 前全库 grep 清单化 |
| 与 widgets 互读 XML 失败 | tag 枚举/记录格式已在 core 共享（现状即如此），失败只会出在 objectName 语义——验收门 1 专门覆盖 |
