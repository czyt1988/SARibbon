# SARibbon 开发指引

## 禁止事项

- **禁止读取或修改** `src/SARibbon.cpp` 和 `src/SARibbon.h` — 这是 amalgamate 工具生成的合并文件，所有源码改动应在 `src/SARibbonBar/` 下进行
- **禁止** `slots`/`signals`/`emit` → 用 `Q_SLOTS`/`Q_SIGNALS`/`Q_EMIT`
- **禁止** 头文件 public 函数加双语 Doxygen → 仅用单行英文 `///`
- **禁止** Q_PROPERTY 上加任何注释（分组注释如 `// == Ribbon properties ==` 可以）
- **禁止** 类注释用 `@param`/`@class`/`@ingroup` → 仅 `@brief`/`@details`/`@note`/`@see`
- **禁止** `as any`/`@ts-ignore` 类型的类型安全压制（本项目无 TypeScript，但精神一致：不压制编译错误）

## 项目结构

```
src/SARibbonBar/          ← 所有源码（.h/.cpp），这是唯一应编辑的目录
src/SARibbon.cpp/.h       ← ⛔ 合并文件，禁止触碰
src/SARibbonBar/colorWidgets/  ← SAColorWidgets 子模块（SAColorToolButton等）
src/SARibbonBar/3rdparty/      ← 第三方代码
src/SARibbonBar/i18n/          ← 翻译文件 (.ts/.qm)
example/                  ← 示例程序（MainWindowExample是最主要的）
tests/                    ← 单元测试（Qt Test框架，需 BUILD_TESTS=ON）
tools/                    ← Amalgamate合并工具
docs/zh/dev-guide/        ← 开发规范文档（编码前必读）
docs/zh/build-guide/      ← 构建指引
```

核心头文件 `src/SARibbonBar/SARibbonGlobal.h` 定义了：`SA_RIBBON_EXPORT`、`SA_RIBBON_DECLARE_PRIVATE`/`SA_RIBBON_DECLARE_PUBLIC`、`SA_RIBBON_IMPL_CONSTRUCT`、`SA_D`/`SA_DC`/`SA_Q`/`SA_QC`、`SARibbonAlignment`/`SARibbonTheme`/`SARibbonMainWindowStyleFlag` 枚举。

## 构建

CMake 构建，最低 Qt 5.12，支持 Qt5 和 Qt6：

必须使用 Visual Studio 生成器，不要使用 Ninja（PowerShell 中 MSVC 环境无法通过 vcvars64.bat 注入）：

示例：

```powershell
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64"
cmake --build build --config Release
```

> 构建损坏时先关掉 `build/bin` 下占用的程序，再 `Remove-Item -Recurse -Force build` 重配。

关键选项：
- `SARIBBON_BUILD_STATIC_LIBS=ON` → 静态库，自动定义 `SA_RIBBON_BAR_NO_EXPORT`
- `SARIBBON_USE_FRAMELESS_LIB=ON` → 强制 C++17，需 QWindowKit 库；OFF 时 Qt5 用 C++14，Qt6 用 C++17
- `SARIBBON_BUILD_EXAMPLES` 默认 ON
- Windows 默认 `SARIBBON_INSTALL_IN_CURRENT_DIR=ON`，安装到 `bin_qt{版本}_{编译器}_x{架构}/`

## 注释规范（强制，AI代码尤其容易违反）

| 位置 | 格式 | 规则 |
|------|------|------|
| .h public函数 | `// Get the category name` | 单行英文，禁止双语Doxygen |
| .h Q_PROPERTY | 无注释 | 禁止任何Doxygen块 |
| .h 类注释 | 双语 `\if ENGLISH`/`\if CHINESE` | 仅 `@brief`/`@details`/`@note`/`@see` |
| .h 信号注释 | 双语 `\if ENGLISH`/`\if CHINESE` | 信号没有.cpp定义，注释必须在头文件 |
| .cpp 函数实现 | 双语 `\if ENGLISH`/`\if CHINESE` | 详细注释放.cpp，不放.h |

## PIMPL 模式

所有核心类使用 PIMPL，宏来自 `SARibbonGlobal.h`：

```cpp
// .h — 紧跟 Q_OBJECT 之后
class SA_RIBBON_EXPORT SARibbonCategory : public QFrame
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonCategory)  // 生成 d_ptr + PrivateData前置声明
    // Q_PROPERTY 不加注释
    Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
public:
    // Constructor
    explicit SARibbonCategory(QWidget* p = nullptr);
};

// .cpp — PrivateData 定义在cpp中（不在.h中）
class SARibbonCategory::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonCategory)  // 生成 q_ptr 反向指针
public:
    PrivateData(SARibbonCategory* p);
    bool enableShowPanelTitle { true };  ///< 行尾注释标记成员
};

// 构造函数 — 项目惯用 d_ptr(new ...) 而非 SA_RIBBON_IMPL_CONSTRUCT
SARibbonCategory::SARibbonCategory(QWidget* p)
    : QFrame(p), d_ptr(new SARibbonCategory::PrivateData(this))
{
}

// 函数体中
void SARibbonBar::showMinimumModeButton(bool isShow)
{
    SA_D(d);   // 非const: PrivateData* d = d_ptr.get()
    // const函数用 SA_DC(d)
}
```

PIMPL 注意：`d_ptr` 用 `std::unique_ptr`（非 QScopedPointer），PrivateData 析构必须在 .cpp 可见。私有成员变量全在 PrivateData 中，不在 .h 的类体中。

## Qt 集成要点

- 属性用 `Q_PROPERTY` 暴露（布尔getter用 `is*` 前缀如 `isMinimumMode()`）
- 信号命名：`xxxChanged` 模式（`ribbonStyleChanged`、`categoryNameChanged`）
- 信号发射：`Q_EMIT`（禁止 `emit`）
- 槽可见性：`public Q_SLOTS`/`protected Q_SLOTS`/`private Q_SLOTS`

## 格式化

`.clang-format` 基于 WebKit 风格，4空格缩进，120字符行宽，指针靠左（`QWidget* p`），CRLF换行。**注意**：SARibbonAlignment枚举要求文件换行为CRLF，LF会导致编译错误。

## Git 提交格式

```
修复：SARibbonCategory布局计算错误

- 修复了在紧凑模式下面板高度计算不正确的问题
- 相关文件：SARibbonCategory.cpp, SARibbonCategoryLayout.cpp
- 关联计划：Ribbon布局优化计划
```

## 开发规范文档（编码前必读）

| 文档 | 内容 |
|------|------|
| [coding-standards.md](docs/zh/dev-guide/coding-standards.md) | 命名规范、Doxygen注释、Git提交 |
| [pimpl-dev-guide.md](docs/zh/dev-guide/pimpl-dev-guide.md) | PIMPL宏完整用法 |
| [qt-integration.md](docs/zh/dev-guide/qt-integration.md) | Q_PROPERTY、信号槽、Qt宏 |
| [build-SARibbon.md](docs/zh/build-guide/build-SARibbon.md) | CMake构建选项详解 |