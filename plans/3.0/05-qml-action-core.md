# 计划 05：QML 命令层核心——RibbonAction 与按钮/容器 action 化

> **对应契约**：D1 / D2 / D4 / D5 / D6 / D8 的 QML 侧落地
> **前置**：无（本计划是契约时代起点）；与计划 07 无依赖可并行
> **全程纪律**：开工前通读 [00-architecture-contract.md](00-architecture-contract.md)；遵守 AGENTS.md（worktree 工作流、注释规范、Q_SLOTS/Q_EMIT）；本计划不触碰 `src/SARibbon.cpp/.h`（QML 模块不在合并范围，无需跑 Amalgamate）

## 1. 目标

1. QML 模块获得**活的命令对象**：`RibbonAction : QAction` 注册为 QML 类型；C++ 裸 QAction 同样可直接绑定
2. `RibbonToolButton` 支持 `action: QAction*`：text/icon/toolTip/checked/enabled 全部派生自 action，click → `action->trigger()`
3. 快捷键**真实触发**（废除"纯展示文本"现状）
4. C++ 后端可在运行期以 QAction 为单位动态构建 ribbon
5. QML 模块切换 **Qt6-only**（契约 D5）
6. 无障碍与键盘导航基线（契约 D8）

## 2. 非目标

- 注册表/定制器/菜单树/Gallery/示例全量重构 → **计划 06**
- widgets 侧任何改动 → **计划 07**
- core 布局引擎改动：**无**（放置属性语义不变，引擎不知道命令层的存在）
- RibbonMenuItem/menuItems 体系：保持现状（计划 06 S3 改造）

## 3. 前置核验（S0，先核后动）

| # | 核验项 | 方法 | 结论去向 |
|---|-------|------|---------|
| V1 | Qt 最低小版本 | 以可得的最低 Qt 6.x（预期 6.2 LTS）完整构建 QML 模块 + 示例 + 测试；失败则逐级上调 | 回填契约 D5 |
| V2 | `QAction::shortcut` 在 QQuickWindow 场景是否自动触发 | 最小测试程序：QQuickView + `QAction::setShortcut("Ctrl+S")`，**不经任何 QML Shortcut 类型**，按键看 triggered 是否发出；同时验证 shortcutContext 语义 | 决定 S4 走原生还是回退设计 |
| V3 | Qt5 兼容分支清单 | `grep -rn "QT_VERSION\|QT_CONFIG" src/qml/` + qml 相关 CMake 条件 | S1 删除清单 |
| V4 | QML 字符串 → QKeySequence 赋值 | `RibbonAction { shortcut: "Ctrl+S" }` 是否经 QVariant 转换成功 | 不行则 S2 补 `shortcutText` 便捷属性 |

## 4. 执行步骤

### S1 Qt6-only 切换（V3 清单执行）

- 删除 `SARibbonQmlTypes.cpp:118-137` 的 Qt5 `qRegisterMetaType` 块及全模块版本宏分支（含 Qt5 QFontDatabase 等历史 workaround）
- CMake：`SARIBBON_BUILD_QML=ON` 时强制 `find_package(Qt6 ...)`；Qt5 下配置**明确报错**并说明原因（契约 D5）
- CI 矩阵更新：Qt5 线 = widgets-only（QML OFF），Qt6 线 = 全量
- 回填 V1 结论到契约 D5

### S2 RibbonAction 类型（src/qml/SARibbonQmlAction.h/.cpp）

- `class RibbonAction : public QAction`，Q_OBJECT；注册 `"SARibbon", 3, 0, "RibbonAction"`
- 属性（契约 §5 边界内）：
  - `iconSource: QUrl` — WRITE 时惰性构建 QIcon 并 `QAction::setIcon` 同步（同一 action 可喂 widgets 世界）；`setIcon` 反向变化时同步回 url 可为空（QIcon 侧以 provider 链路渲染，见 S3）
  - `toolTip: QString` — QAction 无此属性，补齐
  - `menuActions: QVariantList`（QAction* 列表）— 纯数据容器，本计划仅存储与 NOTIFY，渲染在计划 06
- `shortcut` 直接用 QAction::shortcut（V4 结论决定是否补便捷属性）
- objectName 照常可设；后续注册表对无 objectName 的 action 拒绝注册（契约 §5，落实在计划 06 S1）
- **禁止**添加任何放置属性（契约 D3，评审时逐属性过一遍）

### S3 图标链路（QIcon ⇄ QML url 双向）

- **QML 声明侧**（主路径，零开销）：宿主发布的 `iconUrl` = action 的 `iconSource`（RibbonAction 路径），叶子照旧绑定渲染
- **C++ 裸 QAction 路径**：`SAIconImageProvider`（`image://saribbon/act/<id>`）——QPointer 登记表管理存活 action，`QIcon::pixmap` → QImage，按 (action, size) 缓存；action iconChanged 时失效重发布
- 两条路径对叶子透明（叶子只见 iconUrl）

### S4 RibbonToolButton.action 绑定

- `Q_PROPERTY(QAction* action READ action WRITE setAction NOTIFY actionChanged)`
- **派生规则**（action 非空时生效）：
  - text / icon(iconUrl) / toolTip / checked / enabled 派生自 action
  - 本地写 → **写穿**到 action（`btn.text = "x"` 即 `action->setText("x")`）——属性保持可绑定可写，action 恒为唯一权威（契约 §3 铁律）
  - action 为空时回退现行为（自有属性存储）——两级语义（契约 D6）
- **状态同步**：连接 `QAction::changed` → 宿主对派生属性逐个 re-emit NOTIFY；`QAction::triggered` → 宿主 `clicked` 信号（保持现有信号面不破坏）
- **click() 语义**：action 非空 → `action->trigger()`（checkable 的 toggle 交给 QAction 原生语义）；为空 → 现行为
- **放置属性不动**：proportion / toolButtonStyle / popupMode / flat / titleRow 上下文解析全部照旧（契约 D3/D6——本计划不得顺手"改进"它们）
- menuItems 现状保留（计划 06 S3 改 menuActions）
- 派生期间叶子绑定不破坏：宿主现有"单依赖绑定形状"纪律（旧 NOTES B48）继续适用

### S5 快捷键真实化（契约 D8-1）

按 V2 结论二选一：

- **原生可用**：文档化 shortcutContext 语义、与 QML `Shortcut` 类型并存的防双触发规则；结束
- **回退设计（预期走此路）**：RibbonBar 级快捷键匹配器——
  - bar 收集 action 绑定按钮的 (QAction, QKeySequence) 集合（attach/detach/action 属性变化时维护）
  - 事件过滤 `window->contentItem()` 的 ShortcutOverride / KeyPress
  - 匹配 → `action->trigger()`；尊重 enabled / visible / shortcutContext（Window/Application 近似语义）
  - 单次按键单次触发；预计 100–150 行，全部公开 API
- 验收：示例中 Ctrl+S 等真实可用；与 QML Shortcut 类型并存不双触发

### S6 容器与后端驱动适配

- **Panel**：`Q_INVOKABLE RibbonToolButton* addAction(QAction*, int proportion)` —— 创建 action 绑定按钮 + `attachChildItem`（对齐 widgets 的便捷方法族语义）；现有 attach/detach/moveChildItem 不变
- **ButtonRowHost**（QAB/ButtonGroup）：action 绑定按钮照常 registerButton；titleRow 强制小按钮 + flat 逻辑**不变**（放置语义，契约 D6——快速访问栏正是"同命令不同大小"的证明场景）
- **C++ 后端范式**（写入示例与文档）：
  ```cpp
  auto* btn = new SARibbonQml::RibbonToolButton;
  btn->setAction(act);            // act 可以是裸 QAction
  panel->attachChildItem(btn);    // 叶子由宿主自动加载（现有 createLeaf 机制）
  ```
- **互斥**：action 加入 QActionGroup → 勾选互斥天然生效（QAction 原生）；容器 `exclusive` 属性保留管声明式按钮，两者并存规则文档化

### S7 无障碍与键盘导航基线（契约 D8-2/3）

- QQuickAccessibleAttached：role = Button、name = text、checked 状态（菜单展开态可暂缓）
- activeFocusOnTab + Space/Enter → click()（Keys.onPressed）
- 验收：示例全程可键盘完成"切 tab → 聚焦按钮 → 触发"

### S8 测试与文档（本计划范围）

- 单测：action 派生 / 写穿 / 同一 action 两按钮状态同步 / detach 不销毁 action / 空 action 回退
- 一致性套件（RibbonConformance 场景数据）增加 action 场景（跨端数据共享机制已存在）
- `docs/zh/dev-guide/qml-guide.md` 增补 action 章（计划 06 S7 全面重写前的过渡）
- 快捷键机制结论（V2）写入 qml-guide

## 5. 验收门

1. 契约 §3 典型代码段与迁移故事在示例中真实可跑（C++ 裸 QAction 与 QML RibbonAction 各演示一路）
2. 同一 QAction 同时出现在面板 + 快速访问栏：状态单点同步、快捷键真实触发
3. Qt5 配置下 `SARIBBON_BUILD_QML=ON` 明确报错；Qt6 全量绿
4. 无障碍/键盘基线验收（S7）
5. 既有 QML 布局/渲染测试全绿（重构不得引入视觉回归）

## 6. 风险与缓解

| 风险 | 缓解 |
|------|------|
| V2 快捷键原生不可用 | 回退设计已备（S5，设计已写明） |
| image provider 高频刷新开销 | 缓存 + 仅 iconChanged 时失效 |
| QAction 只有 changed() 单信号，派生属性 NOTIFY 粒度粗 | 宿主统一 re-emit 全部派生 NOTIFY，接受冗余刷新（正确性优先） |
| 写穿语义与 QML 绑定冲突（绑定写回） | 单测覆盖：绑定 + 手写并存场景 |
