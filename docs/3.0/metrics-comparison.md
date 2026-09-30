# SARibbon 3.0 度量对照表（计划 02 S3 验收物，v2 R3）

录制条件：同机（Windows 10 x64）、同 Qt（6.7.3 msvc2019_64，同 style=windowsvista）、同默认字体、
Release 构建、SARibbonBar 默认构造（无 QSS 主题）。2.9.5 基线 = commit `7a617fc` 全新构建；
3.0 = dev-3.0（S3 完成后）构建。dump 工具：临时最小工程（QApplication + SARibbonBar +
SARibbonBarLayout 七个公共度量函数打印，两侧同码）。

| 度量项 | 2.9.5 基线 | 3.0（S3 后） | 相等 |
|--------|-----------|--------------|------|
| tabBarHeight | 27 | 27 | ✅ |
| titleBarHeight | 31 | 31 | ✅ |
| categoryHeight | 87 | 87 | ✅ |
| panelTitleHeight | 15 | 15 | ✅ |
| normalModeMainBarHeight | 145 | 145 | ✅ |
| minimumModeMainBarHeight | 58 | 58 | ✅ |
| calcMinTabBarWidth | 6 | 6 | ✅ |

结论：**逐项相等**。SARibbonMetrics 的四个推导公式（calcDefaultTabBarHeight /
calcDefaultTitleBarHeight / calcCategoryHeight / calcMainBarHeight）自
SARibbonBarLayout.cpp 纯 move（`git diff` 确认公式体一字未改），QStyle pixelMetric 与
QFontMetrics 仍由 widgets 适配器在同一采集点喂入，行为零变化成立。

注：暗色/亮色主题不影响本组度量（度量公式仅依赖字体与 QStyle，不读 QSS）；按计划 02
S3 验证要求在亮色默认主题下录制。dpr=1.0（录制环境主屏）。
