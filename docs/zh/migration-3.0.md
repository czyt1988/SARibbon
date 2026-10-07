# SARibbon 3.0 迁移指南

> 适用于从 SARibbon 2.x（≤2.9.5）升级到 3.0 的用户。

## 摘要

3.0 是一次**源码级高度兼容**的大版本重构：所有控件类名、公共 API、信号与行为保持不变
（布局算法经黄金几何测试证明与 2.9.5 逐字节一致）。绝大多数用户工程只需关注构建系统
层面的少量改名。

## 构建要求

| 项 | 2.x | 3.0 |
|----|-----|-----|
| CMake | 3.15+ | **3.21+** |
| Qt | 5.12+ | **5.15+** |
| C++ | C++14（frameless 时 C++17） | **C++17**（全模块统一） |

## include 路径

```cpp
// 2.x
#include <SARibbonBar/SARibbonBar.h>
// 3.0（推荐）
#include <SARibbonWidgets/SARibbonBar.h>
```

旧路径 `<SARibbonBar/...>` 通过**兼容转发头**继续可用（安装树含 `include/SARibbonBar/`
转发目录），过渡一个版本周期后移除。

## CMake 消费

```cmake
# 2.x
find_package(SARibbonBar REQUIRED)
target_link_libraries(app PRIVATE SARibbonBar::SARibbonBar)

# 3.0（推荐）
find_package(SARibbon 3.0 REQUIRED COMPONENTS Widgets)   # 或 Core / Qml
target_link_libraries(app PRIVATE SARibbon::Widgets)
```

旧包名 `SARibbonBar` 保留兼容薄壳一个版本周期；树内构建的裸 target 名 `SARibbonBar`
与 `SARibbonBar::SARibbonBar` 别名同样保留。

## 构建选项改名

| 2.x | 3.0 |
|-----|-----|
| `BUILD_TESTS` | `SARIBBON_BUILD_TESTS`（旧名兼容一版） |
| — | `SARIBBON_BUILD_WIDGETS`（core-only 组合构建） |
| — | `SARIBBON_BUILD_QML`（默认 OFF） |
| — | `SARIBBON_INSTALL`（add_subdirectory 嵌入场景可 OFF） |

## API 破坏性变更（3.0 API 冻结前重构）

### 放置属性通道废除（计划 07）

2.x 把放置参数（行占比/弹出模式/按钮样式）经 `_sa_RowProportion` 等 QAction 动态属性走私，
`SARibbonPanel` 的六个静态属性读写函数族已**删除**：

- `SARibbonPanel::setActionRowProportionProperty` / `getActionRowProportionProperty`
- `SARibbonPanel::setActionToolButtonPopupModeProperty` / `getActionToolButtonPopupModeProperty`
- `SARibbonPanel::setActionToolButtonStyleProperty` / `getActionToolButtonStyleProperty`

迁移：**改用带参重载**。`addAction(act, rp)` / `addAction(act, popMode, rp)` /
`addLargeAction` / `addMediumAction` / `addSmallAction`（含 popMode 重载）的签名与语义不变，
"先 `setActionRowProportionProperty` 再 `addAction`"的用法直接删除属性设置行即可。
放置参数现在是面板私有的放置记录，同一 action 进两个面板给不同比例互不覆盖。

### 命令级 isCanCustomize 标记归置（计划 07）

QAction 上的 `_sa_isCanCustomize` 动态属性通道对**命令级**标记不再使用：改由
`SARibbonActionsManager::setCanCustomize(act)` / `isCanCustomize(act)` 持有（未注册进
管理器的 action 恒不可定制）。category/panel 的 `isCanCustomize` 仍为 Q_PROPERTY，不变。

## 单文件发行

`src/SARibbon.h/.cpp` 不再随仓库分发。获取方式：

```bash
# 本地生成（4 个产物：SARibbonCore.h/.cpp 与 SARibbonWidgets.h/.cpp）
cd tools && bash Amalgamate.sh
```

或从 GitHub Release 附件下载打包好的产物。StaticExample 已改为守卫式构建：产物缺失时
警告并跳过，不影响其余示例构建。

## Python 绑定

三轨发行名与导入名全部不变（`PyQtSARibbon` / `PyQt6SARibbon` / `PySideSARibbon`），
版本号 3.0.0。绑定内部改为编译三模块源码树（含 core 头镜像机制），用户侧无感知。

## 术语说明

- `SARibbonPannel` 拼写问题**已于 2.9.x 修正**（`SARibbonPanel`），3.0 无需任何别名。
- 度量对照（tabBarHeight/categoryHeight 等 7 项）与 2.9.5 逐项相等，见
  [docs/3.0/metrics-comparison.md](../3.0/metrics-comparison.md)。

## QML（新增能力）

3.0 新增 `SARibbonQml` 模块（`import SARibbon 3.0`）：C++ 结构宿主驱动与 widgets
完全相同的 core 布局引擎，QML 只做视觉。应用侧在 `engine.load()` 前调用一次：

```cpp
saRibbonRegisterQmlTypes(&engine);
```

注意：单例注册使用回调式 API，多引擎场景安全；详见 QML 开发文档。
