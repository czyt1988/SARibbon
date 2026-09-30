# SARibbon 3.0.0

按 [plans/3.0](plans/3.0/) 四份计划（M0 基建重组 → M1 核心下沉 → M2 构建生态 → M3+M4 QML 与发布）完成的 3.0 主版本，共 50+ 提交。

## 架构变化

- **三模块拆分**：`src/core`（SARibbonCore，无 QtWidgets 依赖，CI 纯净扫描把关）、`src/widgets`（SARibbonWidgets）、`src/qml`（SARibbonQml，QML 类型注册与桥接）
- **核心下沉**：主题（SARibbonThemePalette/ThemeData）、度量（SARibbonMetrics）、三大布局引擎（panel/category/bar geometry engine，纯函数 + Input 结构体）、契约接口（SARibbonAbstractLayoutItem 等）、数据（SARibbonCustomizeRecord）、工厂全部下沉 core
- **公共枚举上移**至 `SARibbon::Core` 命名空间（`SARibbonAlignment`/`SARibbonTheme`/`SARibbonRowProportion`…），widgets 侧保留兼容拼写
- **amalgamate 双产物**：`src/SARibbonCore.h/.cpp` + `src/SARibbonWidgets.h/.cpp`（由 `tools/Amalgamate.sh` 从源码生成，合并文件不再手改）

## 构建与测试

- CMake 函数化组织（sa_add_library/sa_sync_include），Qt5.12–Qt6.x 双系列支持
- **黄金几何测试**：布局引擎 blob 回归（`tests/core/golden_panel_geometry.txt`）+ 引擎级 FakeItem 测试（字体无关）
- CI 矩阵：Windows/Linux × Qt5.15/Qt6.8 × widgets/qml 轴 × static 轴全绿；mac 两条为环境性豁免（AGL framework 被 Apple 移除、macos-13 Intel runner 退役——master 同样红，证据见 NOTES B29/B32）
- **Python 绑定三轨修复**（2.9.5 起从未绿过）：PyQt5/PyQt6 sip 轨与 PySide6 shiboken 轨 dry-run 双平台全绿；修复链 20+ 缺陷登记于 NOTES B33–B38（cp936 编码地雷、libclang 版本、PySide6↔Qt 版本对齐、typesystem 枚举上移适配等）
- 发布 workflow 安全门：dispatch 只跑 dry-run，真实 PyPI 上传仅 Release 触发（Trusted Publishing OIDC）

## 兼容性（破坏性变化）

- 头文件路径：`SARibbonCore/...`（FLATTEN）与 `SARibbonWidgets/...`
- C++ 枚举新家：`SARibbon::Core::SARibbonRowProportion`（`SARibbonPanelItem::RowProportion` 为别名，`SARibbonPanelItem::Large` 等拼写保持）
- Python：`SARibbonPanelItem.RowProportion` → `SARibbon.Core.SARibbonRowProportion`（pyside6）；PySide6 轨道钉 6.10.3（soname 对齐）
- QML 模块：`SARibbonQml`（新能力）

## 维护者发布清单（人工步骤）

1. review 并合并本 PR
2. 确认 PyPI Trusted Publishing（OIDC）已在 pypi.org 为三个包名（SARibbon/PyQtSARibbon/PyQt6SARibbon/PySideSARibbon）配置 trusted publisher，environment `pypi`
3. `gh release create v3.0.0` 附 `release-assets/SARibbon-3.0.0-amalgamation.zip`（内含 4 个 amalgamate 产物）——Release 发布将触发真实 PyPI 上传 workflow

## 详细过程记录

偏差与修复台账：[plans/3.0/NOTES.md](plans/3.0/NOTES.md)（B1–B38）
