# plans/3.0 —— 架构契约与执行计划

## 目录

| 文档 | 内容 | 状态 |
|------|------|------|
| [00-architecture-contract.md](00-architecture-contract.md) | **架构契约（一切任务开工前必读）**：QAction 统一命令模型、三层分工、命令/放置属性分离、模块版本边界、已否决方案登记 | 生效中 |
| [05-qml-action-core.md](05-qml-action-core.md) | QML 命令层核心：RibbonAction、按钮 action 绑定、快捷键真实化、Qt6-only、无障碍基线 | 待执行 |
| [06-qml-action-ecosystem.md](06-qml-action-ecosystem.md) | QML 命令生态：注册表/定制器 QAction 寻址、菜单树、Gallery、示例重写、跨端一致性 | 待执行（依赖 05） |
| [07-widgets-placement-cleanup.md](07-widgets-placement-cleanup.md) | widgets 放置属性清理：废除 `_sa_*` 动态属性通道，参数直达 PanelItem | 待执行（可与 05/06 并行） |

## 执行顺序

```
05 (QML 命令层核心) ──→ 06 (QML 命令生态)
07 (widgets 清理) ──────┘（无依赖，任意时段并行）
```

每个计划独立 worktree + 独立分支执行（AGENTS.md Worktree 工作流），完成后合并回 dev-3.0。

## 历史说明

旧 `plans/3.0/01–04` 计划、`NOTES.md`、`reviews/` 与根目录 `SARibbon-3.0-plan-v2.md` 已于 2026-10-07 删除：其"不做 action 桥"（v2 D8）裁决被当天的架构评审推翻，保留会误导后续工作。历史内容查 git log（删除提交的父提交）。

源代码注释中残留的 "NOTES Bxx" 引用（如 `src/qml/qml/RibbonToolButton.qml` 的 B48）指向已删除的旧 NOTES.md，解析请查 git 历史。
