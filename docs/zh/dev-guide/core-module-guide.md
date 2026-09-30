# SARibbon 3.0 core 模块与布局引擎开发指引

> 适用版本：3.0（dev-3.0 分支）。本文对应计划 02 交付的开发者文档。

## core 模块结构（七子系统）

```
src/core/
├── SARibbonCoreGlobal.h/.cpp    # 导出宏三段式 + PIMPL 宏族 + ABI 探针
├── SARibbonCoreConfig.h.in      # 版本宏 + SA_RIBBON_CONFIG feature 开关（build 树生成）
├── SARibbonQt5Compat.hpp        # Qt5/Qt6 差异兼容（eventPos/horizontalAdvance 等）
├── global/
│   ├── SARibbonEnums.h          # 公共枚举（全局命名空间）+ RowProportion（SARibbon::Core）+ 属性名宏
│   └── SARibbonCoreUtil.h/.cpp  # 无 widget 依赖的 SA:: 工具函数（saIsRTL 用 QGuiApplication）
├── theme/
│   ├── SARibbonThemePalette.h/.cpp  # JSON 调色板（namespace SA 保持）
│   └── SARibbonThemeData.h/.cpp     # 主题数据单例（Meyers 静态）+ 静态主题表
├── metrics/
│   └── SARibbonMetrics.h/.cpp   # 度量收口（QStyle pixelMetric 与 QFontMetrics 由适配器喂入）
├── contract/
│   ├── SARibbonAbstractLayoutItem.h   # 布局项窄契约（含 Category 双几何扩展）
│   ├── SARibbonAbstractLayoutHost.h   # 宿主契约（仅 metrics() 一个纯虚）
│   └── SARibbonContract.cpp
├── layout/
│   ├── SARibbonPanelLayoutEngine.h/.cpp     # Panel 装箱引擎（含 sizeHint 缓存）
│   ├── SARibbonCategoryLayoutEngine.h/.cpp  # Category 引擎 + scrollButtonFlags/clampScrollOffset 纯函数
│   └── SARibbonBarGeometryEngine.h/.cpp     # Bar 标题区几何（D6 范围：metrics+titleRect）
├── data/
│   └── SARibbonCustomizeRecord.h/.cpp  # CustomizeData 纯数据基类（simplify 算法模板）
└── factory/
    └── SARibbonElementFactoryInterface.h  # 占位（D7 gate，QML 需要时实现）
```

## 纯净性铁律（两层）

- **模块层（CI 硬门）**：`src/core/` 禁止 include QtWidgets/QtQuick 头、禁止 `qApp`/`QApplication`/`QWidget`/`QLayout`。
  扫描：`python3 tools/check_core_purity.py src/core`。
- **确定性层（引擎专属）**：`src/core/layout/` 三引擎禁止调用 QGuiApplication 的动态状态
  （`layoutDirection()/styleHints()/primaryScreen()`）——引擎必须"同输入同输出"，这些值只能经
  Input/Metrics 字段进来（`Input.isRTL` 等）。

## 契约接口（SARibbon::Core）

```cpp
class SARibbonAbstractLayoutItem {
public:
    virtual ~SARibbonAbstractLayoutItem();
    // 前端提供（引擎输入）
    virtual QSize sizeHint() const = 0;
    virtual bool isHidden() const = 0;            // Panel=action 可见性；Category=QWidgetItem::isEmpty
    virtual Qt::Orientations expandingDirections() const = 0;
    // 前端提供（带默认）
    virtual int maximumWidth() const { return 16777215; }  // QWIDGETSIZE_MAX 值
    virtual int stretchFactor() const { return 0; }        // 仅 Gallery 适配器覆写
    virtual QString debugName() const { return {}; }
    // 前端实现（引擎输出应用）
    virtual void applyGeometry(const QRect& rect) = 0;
    // 引擎回写字段（注意 resultGeometry 命名——避开 QLayoutItem::geometry()）
    int rowIndex = -1; int columnIndex = -1;
    QRect resultGeometry; bool isExpandItem = false;
    SARibbonRowProportion rowProportion = SARibbonRowProportion::Large;
};
```

widgets 侧 `SARibbonPanelItem : QWidgetItem + 契约`；2.x 的 `itemWillSetGeometry` 字段以
`QRect& resultGeometry` 引用成员绑定（单真源、存量读写零改动）。

## 引擎适配器模式

1. **适配器收集 Input**（控件查询、字体、RTL、spacing）——core 永不自查；
2. **调引擎**：`Result layout(items, rect, input)`——引擎写契约字段；
3. **回写输出**：适配器把 Result 写回 2.x 的成员（mSizeHint/mColumnCount 等）；
4. **widget 副作用留适配器**：separator hide、label alignment、LayoutRequest、重入守卫
   （`mInDoLayout`）全部不在引擎内。

## 修改算法的流程（R7：2.x fix 同步 core）

布局算法的**唯一实现在 core 引擎**。修 bug：

1. 修 `src/core/layout/SARibbonXxxEngine.cpp`；
2. 跑 `ctest -L core`（黄金几何测试必须全绿——黄金值来自 2.9.5 录制，若需改黄金值即为行为变化，
   必须查明原因并同步 changelog）；
3. widgets 侧若有镜像逻辑（如 2.x 时代的双实现），一并删除。

## 最小 FakeItem 走查

`tests/core/tst_panelLayoutEngine.cpp` 展示了完整路径：不建 widget 树（QGuiApplication +
offscreen 即可），FakePanelItem 内存实现契约，喂显式 sizeHint，断言 resultGeometry——
引擎测试跨平台确定性由此保证。
