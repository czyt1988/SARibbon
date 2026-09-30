# Qt3DWindowExample（Qt3D 嵌入位置左偏复现工程）

关联 issue：[GitHub #105](https://github.com/czyt1988/SARibbon/issues/105)（`QWidget::createWindowContainer(new Qt3DWindow)` 嵌入 SARibbonMainWindow 后，启动时 container 起始位置向左偏移）

## 用途

把 Qt3D 渲染窗口嵌入 `SARibbonMainWindow` 中心区，量化 container 实际落点与期望落点之差（左偏量），用于判断问题归属（SARibbon 侧 vs Qt 用法侧）。

## 输出数据

程序启动后（以及点击"刷新对照数据"按钮）输出 `[QT3D]` 前缀日志：

- 主窗口 `geometry` / `frameGeometry` / `contentsMargins`（SARibbon 无边框模式默认 `setContentsMargins(2,0,2,0)`，分析左偏时先扣除这 2px）；
- 中心区 `geometry` 与全局坐标；
- container 的 `geometry`、全局坐标、原生 `windowHandle()->geometry()`；
- **左偏量**：container 全局 X 与中心区全局 X 之差（逻辑像素 + DPR）。

## 注意

- Qt3D 需要 OpenGL；无 GPU / 远程桌面环境下渲染窗口可能创建失败，窗口会保留但内容为空——几何数据仍然有效。
- `main()` 中已按 FAQ 第 6 节设置 `Qt::AA_DontCreateNativeWidgetSiblings`（嵌入原生 HWND 的推荐用法）。
- 分别在 100% 缩放与 `QT_SCALE_FACTOR=1.5` 下运行，对比左偏量是否随 DPR 变化（区分逻辑坐标错误还是物理像素错误）。
- 两条无边框路径（默认 `SAFramelessHelper` 与 `-DSARIBBON_USE_FRAMELESS_LIB=ON` 的 QWK）都应各跑一次。
