# SARibbon 开发指引

## 禁止事项

- **禁止读取或修改** `src/SARibbon.cpp` 和 `src/SARibbon.h` — 这是 amalgamate 工具生成的合并文件，所有源码改动应在 `src/widgets/` 与 `src/core/` 下进行，源码改动后，运行`tools/Amalgamate.sh`脚本即可自动更新`src/SARibbon.cpp` 和 `src/SARibbon.h`
- **禁止** `slots`/`signals`/`emit` → 用 `Q_SLOTS`/`Q_SIGNALS`/`Q_EMIT`
- **禁止** 头文件 public 函数加双语 Doxygen → 仅用单行英文 `///`
- **禁止** Q_PROPERTY 上加任何注释（分组注释如 `// == Ribbon properties ==` 可以）
- **禁止** 类注释用 `@param`/`@class`/`@ingroup` → 仅 `@brief`/`@details`/`@note`/`@see`
- **禁止** `as any`/`@ts-ignore` 类型的类型安全压制（本项目无 TypeScript，但精神一致：不压制编译错误）

## Worktree 工作流（所有任务强制）

多任务并行时互不干扰：任何改动都不在主工作树进行，一律在 `.worktree/` 下的独立 worktree 内完成。开始工作前必须执行以下四步：

1. **拉取 worktree**：先确定源分支（任务结束必须合并回它），从它拉出任务分支：
   ```bash
   git worktree add .worktree/<任务名> -b <feature|fix>/<任务名> <源分支>
   ```
   此后改动、构建、提交只在 `.worktree/<任务名>` 内进行。worktree 共享主仓库 `.git` 对象库，拉取本身不需要网络。项目应把 `.worktree/` 加入 `.gitignore`，避免主工作树出现未跟踪目录。

2. **submodule 离线复用**：含 submodule 的项目禁止直接 `git submodule update --init`——它会从远端重新克隆。改用主工作树已下载的 submodule 目录作本地克隆源（在**主工作树根目录**执行，`<sub>` 为 submodule 相对路径，如 `3rdparty/qwindowkit`）：
   ```bash
   ROOT="$(git rev-parse --show-toplevel)"
   git -C .worktree/<任务名> -c protocol.file.allow=always \
       -c "submodule.<sub>.url=$ROOT/<sub>" submodule update --init <sub>
   ```
   全程离线（本地硬链接克隆，不改动主仓库配置）；`protocol.file.allow=always` 必需，Git 2.38.1+ 默认禁止 submodule 走 file 传输；嵌套 submodule 逐层同样处理。若主工作树的 submodule 未下载（`git submodule status` 前缀为 `-`），或所需提交本地不存在导致命令失败，**立即停下来询问用户**，禁止自行联网克隆。

3. **合并回源分支**：任务完成后，任务分支必须合并回当初拉取它的源分支——从哪个分支拉取，就合并回哪个分支，禁止合并到其他分支。源分支在主工作树时，切到源分支执行 `git merge <任务分支>`。

4. **完成标准**：合并必须干净成功、无冲突并通过构建/测试验证后才算完成；冲突必须当场解决，不得把冲突、半合并或未验证状态留给用户。收尾：确认 worktree 内改动已全部提交并合并后，用 `git worktree remove --force .worktree/<任务名>` 清理现场（含 submodule 的 worktree 不加 `--force` 会被 git 拒绝删除），再删除任务分支。

## 项目结构

```
src/core/                 ← SARibbonCore：宏/枚举/契约基座（计划02起下沉算法），可编辑
src/widgets/              ← SARibbonWidgets：全部控件源码（.h/.cpp），可编辑
src/widgets/colorWidgets/ ← SAColorWidgets 子模块（SAColorToolButton等）
src/widgets/i18n/         ← 翻译文件 (.ts/.qm)
src/qml/                  ← SARibbonQml 骨架（计划04实现；QML 模式强制 QWindowKit 无边框：`windowAgent: RibbonWindowAgent{}` 一行启用）
src/SARibbon.cpp/.h       ← ⛔ 合并文件，禁止触碰，调用 tools/Amalgamate.sh 自动生成
3rdparty/                 ← 第三方代码（qwindowkit submodule 等）
examples/widgets/         ← 示例程序（MainWindowExample是最主要的）
tests/widgets/            ← 单元测试（Qt Test框架，需 SARIBBON_BUILD_TESTS=ON）
tools/                    ← Amalgamate合并工具、core纯净性扫描、Python绑定构建脚本
sip/                      ← PyQt5 SIP绑定定义文件（.sip）
pyqt6/sip/                ← PyQt6 SIP绑定定义文件（独立维护）
pyside6/                  ← PySide6绑定（Shiboken6）：CMakeLists.txt、typesystem XML、glue代码
pyexamples/               ← Python示例程序（pyqt5/pyqt6/pyside6三个子目录）
pyproject.toml            ← PyQt5 PyPI打包配置
pyproject-pyqt6.toml      ← PyQt6 PyPI打包配置
pyside6/pyproject.toml    ← PySide6 PyPI打包配置
plans/3.0/                ← 3.0 重构执行计划与偏差记录（NOTES.md）
docs/zh/dev-guide/        ← 开发规范文档（编码前必读）
docs/zh/build-guide/      ← 构建指引
docs/zh/python-guide/     ← Python绑定文档（中文）
docs/en/python-guide/     ← Python绑定文档（英文）
```

3.0 开发在 `dev-3.0` 长期分支进行，合并进 master 前不与 `dev`（2.x 线）交互；期间 2.x bugfix 直接 cherry-pick 到 `dev-3.0`，布局引擎相关 fix 须手动同步 core 版并跑黄金测试。

宏基座在 `src/core/SARibbonCoreGlobal.h`（PIMPL 宏族与 `SA_RIBBON_CORE_EXPORT`），`src/widgets/SARibbonGlobal.h` 为兼容转发头（`SARibbonAlignment`/`SARibbonTheme`/`SARibbonMainWindowStyleFlag` 枚举仍在其中，计划02下沉 core）。

## 构建

CMake 构建，最低 Qt 5.12，支持 Qt5 和 Qt6，C++ 标准由 CMakeLists 根据 Qt 版本和选项自动设定（最低 C++14）。完整构建指引见 [build.md](build.md)。

### 构建环境

- **Windows**：CMake 3.15+，Visual Studio 2019（MSVC 14.29+）或 2022（MSVC 14.34+），Qt 6.7+ 或 Qt 5.12+
- **Linux / WSL**：CMake 3.15+，GCC 9+（推荐 GCC 13+），Qt 6.x（apt）或 Qt 5.12+（手动安装）

### 快速构建（推荐使用 scripts/build.ps1 脚本）

Windows 下推荐使用 `scripts/build.ps1` 脚本，自动检测 Qt 路径和 MSVC 版本，并自动处理 VS 生成器选择：

```powershell
.\scripts\build.ps1                          # full: configure + build + install (Release)
.\scripts\build.ps1 build                    # 增量编译（跳过 configure）
.\scripts\build.ps1 rebuild                  # 清理并重新构建（clean + configure + build + install）
.\scripts\build.ps1 configure -Examples OFF  # 仅库，不编示例
.\scripts\build.ps1 configure -StaticLibs ON # 静态库
.\scripts\build.ps1 install                  # 执行 install（build 后运行）
.\scripts\build.ps1 help                     # 查看所有选项
```

脚本支持的全部参数：`-Examples`、`-Tests`、`-StaticLibs`、`-Frameless`、`-SnapLayout`、`-Qml`（均接受 `ON`/`OFF`），以及 `-QtPath`、`-VSVersion`、`-Config`。详细构建指引见 [build.md](build.md)。

Linux/WSL 快速构建：

```bash
sudo apt install qt6-base-dev qt6-base-dev-tools qt6-svg-dev qt6-tools-dev ninja-build
cmake -S . -B build-linux -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-linux --parallel
```

> 构建损坏时先关掉 `build/bin` 下占用的程序，再删除 build 目录重配。Linux apt 安装的 Qt6 无需指定 `CMAKE_PREFIX_PATH`。

### vcpkg 构建

项目支持 vcpkg manifest 模式，`vcpkg.json` 声明了依赖，`CMakePresets.json` 提供了预设配置：

```bash
# 使用 vcpkg preset 构建（自动拉取依赖）
cmake --preset=vcpkg-msvc-x64-release
cmake --build --preset=vcpkg-msvc-x64-release
```

vcpkg 的 `frameless` feature 会自动启用 `SARIBBON_USE_FRAMELESS_LIB`；`qml` feature 会自动启用 `SARIBBON_BUILD_QML`（并连带依赖 `frameless`）。

### CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `SARIBBON_BUILD_STATIC_LIBS` | OFF | 静态库，ON 时自动定义 `SA_RIBBON_BAR_NO_EXPORT` |
| `SARIBBON_USE_FRAMELESS_LIB` | OFF | 使用 QWindowKit 无边框方案，需 C++17 和 QWindowKit 库 |
| `SARIBBON_BUILD_QML` | OFF | 构建 SARibbonQml 模块；ON 时强制 `SARIBBON_USE_FRAMELESS_LIB=ON`（QML 模式无本地化无边框回退，必须引入 QWindowKit，其 Quick 组件需可用）。QWK 查找顺序：已安装包 → 3rdparty/qwindowkit 树内自动构建（submodule 初始化后无需单独编译，QWK 随顶层构建一起编译）；两者皆无时配置直接报错 |
| `SARIBBON_BUILD_EXAMPLES` | ON | 控制是否编译示例程序 |
| `SARIBBON_ENABLE_SNAPLAYOUT` | OFF | 启用 Windows 11 Snap Layout（仅 frameless 模式有效） |
| `SARIBBON_INSTALL_IN_CURRENT_DIR` | ON (Windows) | 安装到 `bin_qt{版本}_{编译器}_x{架构}/` |
| `BUILD_TESTS` | OFF | 启用单元测试（Qt Test 框架） |

> 根据实际 Qt 安装位置调整 `CMAKE_PREFIX_PATH`。

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

## Git 提交

每次任务完成后，如果涉及代码/文档的改动，应该考虑是否提交git，如果需要提交，应按照 conventional commits 格式，用中文生成commits messages

## 开发规范文档（编码前必读）

| 文档 | 内容 |
|------|------|
| [coding-standards.md](docs/zh/dev-guide/coding-standards.md) | 命名规范、Doxygen注释、Git提交 |
| [pimpl-dev-guide.md](docs/zh/dev-guide/pimpl-dev-guide.md) | PIMPL宏完整用法 |
| [qt-integration.md](docs/zh/dev-guide/qt-integration.md) | Q_PROPERTY、信号槽、Qt宏 |
| [build-SARibbon.md](docs/zh/build-guide/build-SARibbon.md) | CMake构建选项详解 |

## 开发原则

当前项目有三个模块：core、widgets 和 QML。这三个模块的依赖关系是，widgets 和 QML 都要依赖 core 模块。因此，如果某个功能 widgets 和 QML 都要用到，则要把这个功能提取到 core 模块里面去，形成一个共性功能，在开发过程中一定要时刻记住：在规划某个功能的时候，首先要想这个功能是否是共用的功能。如果是共用的功能，则把它提取到 core 内部去