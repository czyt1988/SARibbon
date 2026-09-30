# SARibbonQml 开发指引（3.0）

> 适用版本：3.0。SARibbonQml 是 3.0 新增的 QML 前端，与 widgets 前端共享同一套 core 布局引擎。

## 模块概览

- **构建开关**：`SARIBBON_BUILD_QML=ON`（默认 OFF）
- **import**：`import SARibbon 3.0`
- **依赖**：仅依赖 SARibbonCore + Qt（Core/Gui/Qml/Quick），**禁止链接 SARibbonWidgets**（依赖矩阵红线，CI 组合矩阵 `Widgets=OFF Qml=ON` 验证）

## 注册（命令式单轨）

应用在 `engine.load()` 前调用一次（Qt5/Qt6 同码）：

```cpp
#include <SARibbonQml/SARibbonQmlGlobal.h>

QQmlApplicationEngine engine;
saRibbonRegisterQmlTypes(&engine);   // 必须：漏调会报 "module SARibbon is not installed"
engine.load(QUrl("qrc:///main.qml"));
```

**单例注册范式（重要）**：本项目的 QML 单例一律用**回调式 `qmlRegisterSingletonType`**
并在回调内 `setObjectOwnership(CppOwnership)`。**禁止 `qmlRegisterSingletonInstance`**——
它把实例硬绑到首个引擎，多引擎场景第二个引擎取到 nullptr（Qt 5.14/6.7 源码实证）。

## P0 类型清单

| QML 类型 | 形态 | 说明 |
|---------|------|------|
| `RibbonTheme` | 单例 | core 主题数据桥（currentTheme、token 颜色查询） |
| `RibbonMetrics` | 单例 | core 度量桥（tabBarHeight 等只读属性） |
| `RibbonBar` | 类型 | bar 结构宿主（tab 行排布、高度、标题空闲区） |
| `RibbonCategory` | 类型 | category 结构宿主（panel 排布 + 引擎钳制滚动） |
| `RibbonTab` | 类型 | tab 宿主（text/current/contextColor） |
| `RibbonPanel` | 类型 | panel 结构宿主（驱动 PanelLayoutEngine） |
| `RibbonToolButton` | 类型 | 按钮宿主（text/iconSource/proportion） |
| `Ribbon` | 不可实例化 | 枚举持有（`Ribbon.Large` / `Ribbon.ThreeRowMode` / ...） |

枚举一律通过 `Ribbon.` 前缀访问（如 `proportion: Ribbon.Large`），不散进各类型。

## 视觉叶子规范

- 颜色/尺寸**一律绑定 RibbonTheme/RibbonMetrics 单例**，禁止散落颜色常量——
  叶子里出现字面量色值 = 评审打回（本项目特有增强）。
- 宿主↔叶子配对：叶子根声明 `property QtObject panelCpp`（C++ setProperty 注入），
  `onPanelCppChanged` 把自己赋回宿主的 `panelQmlItem`（握手属性，兼作测试可达性入口）。
- 叶子创建三部曲（宿主侧）：`QQmlComponent` → `create()` → `setProperty("panelCpp")`
  → 双 setParent；失败检查 `errorString()` + `QFile::exists(qrc路径)`。
- 析构：叶子 `setParent(nullptr)` + `deleteLater()`，**禁止直接 delete**。

## 铁律

QML 侧发现需要"复制"widgets 的一段算法 = core 缺口 = 停下来补 core，
**禁止在 QML 里重写布局计算**。渲染允许双实现。

- sizeHint 传导链（S5）：core metrics → 宿主 C++ `sizeHint()` → 引擎装箱 →
  `resultGeometry` → `applyGeometry`。**不得取 QML 叶子的 implicitWidth/Height 反推**。

## 滚动动画

Category 滚动的动画用 QML `Behavior on x`（前端动画），目标值经引擎
`clampScrollOffset` 钳制——动画属表现层，钳制属算法层。
