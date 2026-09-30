# MultiScreenDpiExample（多屏 DPI 最大化异常诊断工程）

关联 issue：[GitHub #109](https://github.com/czyt1988/SARibbon/issues/109)（1080p 笔记本主屏 + 4K 外接屏，最大化异常）

## 用途

在**不同缩放比的多显示器**环境下（例如 1080p 100% + 4K 150%），复现"最大化→还原 / 跨屏拖动→最大化"时窗口几何异常（尺寸不对、位置偏移、跨屏抖动）的问题，并输出结构化诊断日志用于归因。

本工程**不做任何修复**，只负责复现与输出数据。

## 操作说明

1. 构建并运行本示例（需要多显示器环境；单屏也可以运行，但只能验证代码路径不崩，不能复现问题）。
2. 按界面提示的顺序点击按钮：
   - **打印当前状态**：输出所有屏幕信息与窗口几何；
   - **跨屏移动**：把窗口依次移动到每一块屏幕中心（每次移动后延迟 300ms 再打印——DPI 切换是异步的）；
   - **最大化 / 还原**：`showMaximized()` / `showNormal()`，操作后自动打印；
   - **反复两侧横跳**：在屏幕之间来回移动 5 次，复现抖动。
3. 日志同时输出到**窗口内日志区**与**控制台**（前缀 `[MSDPI]`）。
4. 把**全部日志**（从程序启动到你认为异常发生）回贴到 issue。

## 期望回贴的日志字段

每段日志包含：

| 字段 | 含义 |
|------|------|
| `screens().size()` | 屏幕数量 |
| `screen[i] name/geometry/availableGeometry/devicePixelRatio/logicalDotsPerInch` | 每块屏的名称、几何、可用区域、缩放比、逻辑 DPI |
| `window: handleScreen/handleDpr` | 窗口实际所在屏与 DPR |
| `geometry(logical)` / `frameGeometry` | 窗口逻辑坐标几何 |
| `geometry(physical)` | 按 DPR 换算的物理像素矩形（判断跨屏后逻辑尺寸被重算导致抖动的关键） |
| `isMaximized/windowState` | 窗口状态 |
| `QWindow::screenChanged` | 换屏时机 |

## 注意

- 单屏 + `QT_SCALE_FACTOR=1.5` 环境变量只能模拟 DPI（对整进程生效），**机制与真实多屏不同**，日志请注明来源（"来自真实双屏"或"来自单屏模拟"）。
- 两条无边框路径都可以构建本示例：默认（`SAFramelessHelper`）与 `-DSARIBBON_USE_FRAMELESS_LIB=ON`（QWindowKit），如果两条路径表现不同，请分别回贴两份日志并注明。
