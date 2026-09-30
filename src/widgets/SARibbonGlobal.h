#ifndef SARIBBONGLOBAL_H
#define SARIBBONGLOBAL_H
// 3.0 兼容转发头：原内容拆分至 SARibbonCore/SARibbonCoreGlobal.h（PIMPL/导出宏基座）
// 与 SARibbonWidgetsGlobal.h（widgets 导出宏），本文件保留以兼容既有 include。
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include "SARibbonWidgetsGlobal.h"
// 计划 02 S1：公共枚举与属性名常量已下沉 core/global/SARibbonEnums.h
#include <SARibbonCore/SARibbonEnums.h>
// 原 Global.h:6 行为保持：版本宏随全局头可见
// （注释单独成行，不放 include 行尾注：Amalgamate 对带尾注的重复 include 行无法去重，见 NOTES B17）
#include "SARibbonBarVersionInfo.h"
class QWidget;                        // 原 Global.h:7 前置声明保留（widgets 侧需要）

#endif  // SARIBBONGLOBAL_H
