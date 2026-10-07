# SARibbon 3.0 架构契约：QAction 统一命令模型

> **状态**：生效中——2026-10-07 架构评审确立，取代已删除的 v2 总计划（根目录 `SARibbon-3.0-plan-v2.md`）与旧 `plans/3.0/01–04` 计划（内容可查 git 历史）
> **效力范围**：dev-3.0 分支上的全部开发，直至 3.0.0 发布
> **阅读要求（强制）**：任何任务——功能实现、bugfix、示例、测试、文档——开工前必须通读本文件。与本文冲突的旧文档、旧代码注释一律以本文为准；执行中发现契约与 Qt 现实冲突时，停下提请架构评审修订契约，**禁止局部变通**：妥协一次，技术债务一生。

## 1. 缘起：为什么推翻旧方案

2026-10-07 架构评审（议题：QML 模块的按钮添加机制是否应向 widgets 的 action 模式看齐）做出三项判断：

1. **旧裁决的前提已失效。** v2 计划 D8 以"core 不消费 action，抽象层没有消费方"否决 action 抽象；但如今 QML 侧 Gallery、RibbonActionRegistry、RibbonCustomizer 已全部落地，命令的**身份层**（key/tag/XML 记录）实际已存在，缺的只是一个**活的命令对象**——持有状态、发信号、可绑快捷键的东西。
2. **两个模块的命令模型已经分裂。** widgets 是纯 action 驱动：QAction 是唯一内容抽象，按钮/菜单/快速访问栏/gallery 全部经它进入；QML 是"前后端结合"：按钮直接声明为容器子项，命令身份靠字符串 key 事后登记。分裂的代价三重——概念不一致（学习与迁移成本）、功能不对齐（快捷键纯展示、同命令多处状态手动同步）、后端驱动缺失（C++/插件无法以命令为单位操纵界面）。
3. **允许推倒重来。** 已完成的 QML 实现中与本契约冲突的部分按计划 05/06 重构，不以工程量作为保留错误设计的理由。3.0 尚未发布，当前是没有兼容包袱的唯一窗口。

## 2. 使命（一切取舍的标尺）

按优先级：

1. **概念一致性**：同一概念在两个模块同名同义。会 widgets 版即会 QML 版，反之亦然；两模块间迁移只改视图层。
2. **功能对齐**：widgets 用户白拿的能力（快捷键真实触发、text/icon/tooltip/checked/enabled 单点维护、同一命令多处共享、菜单与面板共享命令），QML 用户同样白拿。
3. **后端驱动**：C++/插件在运行期以命令为单位构建与操纵 ribbon 内容；QML 视图只是渲染。

视角声明：所有设计判断以**库使用者**的便利与可扩展性为准，不以库开发者实现省事为准。

## 3. 目标架构：三层分工

| 层 | 职责（唯一） | widgets 实现 | QML 实现 |
|----|-------------|-------------|---------|
| **命令层** | 命令的身份与状态：text / icon / shortcut / checkable / checked / enabled / tooltip / objectName | QAction（Qt 原生） | 同一 QAction；`RibbonAction : QAction` 薄适配补 QML 友好属性（计划 05） |
| **视图层** | 呈现：视觉、布局、放置 | SARibbonToolButton / SARibbonPanel 等 | C++ 宿主（QQuickItem，布局权威）+ QML 叶子（纯渲染） |
| **操纵层** | 运行期结构操纵 | `panel->addAction(act, rp)`、`removeAction`、`moveAction` | `attachChildItem` / `insertCategory` / `insertPanel` / 面板级 `addAction`（计划 05 S6） |

三条铁律：

- 命令状态的**唯一权威**在命令层；视图只镜像，不持有副本语义。
- 声明式 QML 是**编写界面**的表面语法，不是架构层。`.qml` 文件退化为纯视图声明，命令以 `action` 属性绑定。
- 同一 QAction 可出现在任意多个位置（面板/快速访问栏/菜单/gallery），每处一个独立视图，状态天然同步。

目标形态（3.0）：

```qml
// 后端 C++（原 widgets 应用的 QAction 创建/连接/快捷键/状态管理）一行不改
RibbonPanel {
    panelTitle: qsTr("Main")
    RibbonToolButton { action: backend.actionSave;  proportion: Ribbon.Large }
    RibbonToolButton { action: backend.actionPaste; proportion: Ribbon.Small }
}
```

**迁移故事 = 第一验收标准**：widgets 应用转 QML，后端 QAction 代码零改动，QML 只写视图；反向同理。重构是否成功，以这条故事能否原样跑通为准。

## 4. 十条裁决

| # | 裁决 | 推翻/否决了什么 |
|---|------|----------------|
| D1 | QAction 是唯一命令抽象，两前端共享 | QML 的"无命令对象 + 字符串 key 补登记"模型 |
| D2 | 三层分工（§3） | QML 前后端结合模式 |
| D3 | 命令属性与放置属性严格分离 | widgets 的 `_sa_*` 动态属性通道 |
| D4 | QML 按钮 = C++ QQuickItem 宿主（路线 A），不继承 AbstractButton | Controls 原生按钮基类路线 |
| D5 | QML 模块 Qt6-only；widgets 维持 Qt ≥ 5.12；core 永不 include QAction | QML 模块的 Qt5 兼容分支 |
| D6 | action 可选，两级语义（action 绑定 / 纯声明） | — |
| D7 | 重构在 3.0 API 冻结前完成（计划 05/06/07） | "3.1 再补" |
| D8 | 快捷键真实化、无障碍、键盘导航为路线 A 必修欠账 | — |
| D9 | 否决自研 Core::Action（KDDW 模式） | v2 D8 引用的 KDDW 论证（结论实际用反） |
| D10 | 本契约只能整体评审修订，禁止执行中局部变通 | — |

### D1 QAction 是唯一命令抽象

- QAction 是 Qt 生态的不动点：QMenu / QToolBar / QMenuBar / QActionGroup 全部围绕它；widgets 模块已被深度锁定（`SARibbonPanel::addAction`、ActionsManager、Gallery 内部的 QActionGroup）。再引入任何第二种命令类型，只会把"两模块分裂"变成"三向分裂"。
- C++ 后端持有的 QAction 可直接赋给 QML 视图（属性类型 `QAction*`）——这就是后端驱动的全部秘密，无需任何桥接层。
- 命令层与 core 解耦：core 不 include QAction（见 D5），两个前端各自消费同一 Qt 原生类型，共享的序列化/枚举/引擎已在 core 且无 QAction 依赖。

### D2 三层分工

见 §3。补充：**渲染上下文**（如 titleRow）由容器压入视图层，属于放置语义，永远不上命令对象。

### D3 命令属性与放置属性严格分离

- **命令属性**（属于 QAction）：text、icon、shortcut、checkable/checked、enabled、tooltip、持久化身份（objectName）。
- **放置属性**（属于放置点）：`proportion`（大/中/小）、`toolButtonStyle`（图标文本排布）、`popupMode`（弹出模式）。同一命令在面板是大按钮、在快速访问栏是小按钮——大小由放置语境决定，不是命令语义。
- 横评佐证（三个成熟设计无一把"此处多大"固化为命令属性）：Office Ribbon XML 的 `size` 写在放置元素 `<button size="large"/>` 上；WPF Ribbon 的命令携带双份图标资产（Large/SmallImageSource）但大小决定权在 GroupSizeDefinition（布局层）；Qt QToolBar 的 toolButtonStyle 在容器上。
- **widgets 现状的偏差**（2.x 历史包袱，计划 07 清理）：`_sa_RowProportion` / `_sa_ToolButtonPopupMode` / `_sa_ToolButtonStyle` 三个动态属性把放置语义写到 QAction 上——同一 action 进两个面板给不同比例时会互相覆盖（共享可变状态），目前没出错只是"布局插入时读取一次"的时序巧合。
- **允许的例外**：定制记录（core 的 SARibbonCustomizeRecord）中的 `actionRowProportionValue` 是"用户的放置决定"的持久化，不是命令属性，保留。

### D4 QML 按钮 = C++ QQuickItem 宿主（路线 A）

- 已核验（本机 Qt 6.7.3，`include/QtQuickControls2/` 公开头文件仅 `QQuickStyle` 与 `QQuickAttachedPropertyPropagator`）：Qt6 Controls 的 AbstractButton、QQuickAction 均为**私有 C++ 实现，无公开继承/创建路径**。
- 若改用 QML 文档根植 Button 的路线：命令对象被锁死在 Controls Action（私有 C++，后端无法公开创建），后端驱动被堵死；且 Controls Button 没有"按 proportion 切换布局算法"的概念，core 布局引擎的跨前端复用随之丢失。
- 路线 A 的代价即义务（见 D8）：无障碍与键盘导航必须自建，不得延后。
- 现有分工保持：**布局在 core，渲染在叶子，逻辑在宿主**。C++ 宿主是注册类型（承载 Q_PROPERTY 与 core 布局契约多继承），QML 叶子是纯渲染文档。

### D5 版本与模块边界

| 模块 | Qt 下限 | 边界 |
|------|--------|------|
| core | 5.12（随 widgets） | **永不 include QAction**；记录/枚举/引擎/XML 已无 QAction 依赖，保持 |
| widgets | 5.12 | QAction 消费方（Qt5 中 QAction 属 QtWidgets） |
| qml | Qt6-only（最低小版本由计划 05 S0 构建核验钉死；开发/CI 基准 6.7+） | QAction 消费方 + RibbonAction 薄适配 |

QML 模块放弃 Qt5 的理由：Qt5 中 QAction 属 QtWidgets，支持它意味着 QML 应用背上整个 widgets 依赖或维护双份行为分支（现有 `QT_VERSION` 分支即此债）；Qt5 开源版早已停止维护。

### D6 两级语义

| | action 绑定按钮 | 纯声明按钮 |
|---|---|---|
| 对应 widgets | `panel->addAction(act, rp)` | `panel->addWidget(w)` |
| text/icon/tooltip/checked/enabled | 派生自 QAction（唯一权威；本地写穿到 action） | 按钮自身属性 |
| 快捷键 | `action->shortcut`，真实触发（D8） | 无（用户可自行绑 QML Shortcut） |
| 注册表 / 定制器 | 纳管（key = objectName） | 不纳管 |
| 销毁 | detach 只摘不销毁 action | 同现状 |

纯声明按钮**不是二等公民**：展示型/一次性内容用它是正确选择；定制体系只管命令——这与 widgets 行为完全一致（widgets 定制器也只管理注册过的 action，不管 addWidget 塞进去的控件）。

### D7 重构时点

3.0 API 冻结**之前**，作为专门的破坏性重构立项（计划 05/06/07）。冻结后再改等于亲手制造第一笔技术债务。

### D8 路线 A 的必修欠账

1. **快捷键真实化**：`QAction::shortcut` 在 QML 场景必须真实触发（机制含核验与回退设计，见计划 05 S4）。
2. **无障碍**：QQuickAccessibleAttached（role=Button、name=text）。
3. **键盘导航基线**：tab 焦点、Space/Enter 触发。

这三项是纯 QQuickItem 相对 AbstractButton 放弃的生态的偿还，随计划 05 落地，不得再延。

### D9 否决自研 Core::Action

v2 D8 引用 KDDW 论证"不做自研 Action"，但结论用反了：KDDW 自研 `Core::Action` 是因为它有 **Flutter** 非 Qt 前端，QAction 无法跨过去；SARibbon 两个前端都是 Qt，QAction 就是不动点。自研只会出现第三种命令类型，与使命 1 直接冲突。

### D10 契约修订规则

本契约任何裁决只能通过"架构评审 → 修订契约 → 记录推翻理由"整体变更。执行任务时若发现契约与现实冲突（如某 Qt 版本行为不符）：停下、报告、评审；禁止在实现里悄悄绕开。

## 5. 命令对象契约（QAction 之上允许/禁止的增补）

- `RibbonAction : QAction`（src/qml，计划 05 落地）：**仅允许**添加 QML 友好属性——
  - `iconSource: QUrl`（内部惰性构建 QIcon 并与 `QAction::setIcon` 双向同步，使同一 action 可同时喂给 widgets 世界；C++ 侧只 setIcon 的裸 QAction 经 image provider 链路反向暴露给 QML，见计划 05 S3）
  - `toolTip: QString`（QAction 无此属性）
  - `menuActions: list<QAction*>`（子菜单容器，见计划 06 S3）
  - **禁止**在其上添加任何放置语义（proportion 等）。
- 持久化身份 = `objectName`，与 widgets 的 ActionsManager 同构；无 objectName 的 action 拒绝进注册表，注册时校验。
- C++ 后端可以用**裸 QAction**（非 RibbonAction）喂给任何 QML 视图——按钮 `action` 属性类型是 `QAction*`。
- 菜单树 = QAction 列表（RibbonAction.menuActions 嵌套）；**不引入 QMenu** 到 QML（QMenu 无法在 Quick 场景渲染，这是文档化的故意分歧：C++ 后端构建菜单用 QAction 列表而非 QMenu）。

## 6. 放置契约

- 放置属性属于放置点：QML = 按钮实例属性（现状正确）；widgets = 添加调用参数直达 SARibbonPanelItem（计划 07 清理动态属性通道）。
- 渲染上下文由容器压入（先例：ButtonRowHost 对标题行容器内按钮强制小按钮渲染 + flat 常态），按钮不得反向探测自己身处何处。
- 每次放置 = 一个独立视图对象：widgets 每处一个 SARibbonToolButton（现状正确）；QML 每处一个 RibbonToolButton 实例（现状正确）。

## 7. 已否决方案登记（防翻案）

| 方案 | 否决理由 | 重开条件 |
|------|---------|---------|
| Controls 原生 Action（QQuickAction）作为命令对象 | 私有 C++，后端无法公开创建/持有，后端驱动堵死 | Qt 公开 Controls C++ API |
| AbstractButton 作为 QML 按钮基类 | 同上；且 core 布局引擎跨前端复用丢失 | 同上 |
| KDDW 式自研 Core::Action | 两前端皆 Qt，QAction 是不动点；自研 = 第三种命令类型 | 出现非 Qt 前端 |
| proportion/大小放命令对象 | Office/WPF/QToolBar 横评反证 + 快速访问栏反例 | 无 |
| QML 模块继续支持 Qt5 | QAction 归属分裂（QtWidgets vs QtGui），双份分支纯债 | 无 |
| "静态声明为中心"的模块定位 | 前后端分离后，动态驱动经命令层成为一等公民，声明式只是编写表面 | 无 |
| 快捷键仅做展示文本（现状） | 显示 Ctrl+S 却不响应 = 谎言式 UI | 无 |

## 8. 术语表

| 术语 | 定义 |
|------|------|
| 命令 Command | 一个可触发/可勾选的用户意图；本契约下恒等于一个 QAction 实例 |
| 命令属性 | 命令的身份与状态：text/icon/shortcut/checked/enabled/tooltip/objectName |
| 放置 Placement | 命令在某容器中的一次呈现（大小/样式/弹出模式/顺序） |
| 放置属性 | proportion / toolButtonStyle / popupMode，属于放置点 |
| 宿主 Host | src/qml 的 C++ QQuickItem 子类，布局与逻辑权威 |
| 叶子 Leaf | src/qml/qml/ 下的 QML 文档，纯渲染，无布局逻辑 |
| 渲染上下文 | 容器压入的呈现语境（如 titleRow），影响放置属性的默认解析 |
| 两级语义 | action 绑定（addAction 语义，进定制体系）/ 纯声明（addWidget 语义） |

## 9. 待核验项（不视为已定，核验后回填本文）

| 项 | 内容 | 核验位置 |
|----|------|---------|
| Qt 最低小版本 | QML 模块在 6.x 最低可构建版本（预期 6.2 可行） | 计划 05 S0-V1 |
| QAction::shortcut 在 Quick 场景的自动触发性 | 决定快捷键机制走原生还是回退设计 | 计划 05 S0-V2 |

## 10. 执行计划索引

| 计划 | 内容 | 状态 | 依赖 |
|------|------|------|------|
| [05-qml-action-core.md](05-qml-action-core.md) | RibbonAction 类型、按钮 action 绑定、快捷键真实化、容器/后端驱动适配、Qt6-only 切换、无障碍基线 | 待执行 | 无 |
| [06-qml-action-ecosystem.md](06-qml-action-ecosystem.md) | 注册表/定制器 QAction 寻址、菜单树 QAction 化、Gallery、示例重写、跨端一致性、文档 | 待执行 | 05 |
| [07-widgets-placement-cleanup.md](07-widgets-placement-cleanup.md) | 废除 `_sa_*` 动态属性通道、参数直达 PanelItem、isCanCustomize 归置 | 待执行 | 无（可与 05/06 并行） |
