# SARibbon开发要求


## 项目概述

- 项目仓库：[github](https://github.com/czyt1988/SARibbon),[gitee](https://gitee.com/czyt1988/SARibbon)
- 源码文件位置：`src/SARibbonBar`
- 项目文档位置: `docs/zh`
- 核心头文件: `src/SARibbonBar/SARibbonGlobal.h`（包含SA_RIBBON_EXPORT、SA_RIBBON_DECLARE_PRIVATE等宏定义）

**注意**：
- `src/SARibbon.cpp`和`src/SARibbon.h`文件是整个项目的合并文件，所有agent禁止读取这两个文件，不要对这两个文件进行任何修改

## 构建指引

具体构建过程可参阅：[build.md文件](./build.md)

## 编码要求

### Qt 集成方案

#### 1. 信号槽设计

充分发挥Qt的信号槽机制，工具类使用信号和槽进行事件通讯。

#### 2. 属性暴露方式

为了贴合Qt框架，类的属性使用`Q_PROPERTY`暴露。

#### 3. PIMPL模式

SARibbon使用PIMPL模式封装私有实现，使用 `SA_RIBBON_DECLARE_PRIVATE`/`SA_RIBBON_DECLARE_PUBLIC` 系列宏。

### 注释与文档规范

以下是最关键的规范要点速查，详细内容请阅读对应文档：

- **Qt宏**：禁止使用 `slots`/`signals`/`emit`，必须使用 `Q_SLOTS`/`Q_SIGNALS`/`Q_EMIT`
- **注释规范（强制）**：
    - ❌ **头文件 public 函数**：禁止双语 Doxygen，只能用单行英文 `///`
    - ❌ **Q_PROPERTY**：禁止加任何 Doxygen 注释
    - ❌ **类注释**：禁止使用 `@param`、`@class`、`@ingroup`，仅允许 `@brief`/`@details`/`@note`/`@see`
    - ✅ **源文件(.cpp)**：必须使用双语 Doxygen（`\if ENGLISH`/`\if CHINESE`）
    - ✅ **类和信号**：在头文件中必须使用双语 Doxygen
    - ⚠️ 详细规范见 [coding-standards.md](docs/zh/dev-guide/coding-standards.md)，**务必在开始编码前阅读**

### 开发规范文档

| 文档 | 说明 |
|------|------|
| [代码风格与注释规范](docs/zh/dev-guide/coding-standards.md) | 命名规范、Doxygen注释格式、Git提交规范 |
| [PIMPL开发规范](docs/zh/dev-guide/pimpl-dev-guide.md) | SA_RIBBON_DECLARE_PRIVATE等宏的使用方式和规范 |
| [Qt集成规范](docs/zh/dev-guide/qt-integration.md) | Q_PROPERTY、信号槽、Qt宏使用规范 |

## Git 提交

在完成当前任务后，需提交所有更改到 Git 仓库。
创建有意义的提交信息保证下次任务能清楚了解这次任务的实现情况
提交信息最好包含以下信息：

- 任务类型（例如：实现、修复、文档更新）
- 实现内容的简要描述
- 相关文件列表
- 关联到计划书（如果适用）

最后再次强调：`src/SARibbon.cpp`和`src/SARibbon.h`文件是整个项目的合并文件，所有agent禁止读取这两个文件，不要对这两个文件进行任何修改