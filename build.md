# 构建指引

## 构建环境

### Windows

- CMake 3.15+
- Visual Studio 2019（MSVC 14.29+）或 Visual Studio 2022（MSVC 14.34+）
- Qt 6.7+ (msvc2019\_64 / msvc2022\_64) 或 Qt 5.12+

### Linux / WSL

- CMake 3.15+
- GCC 9+（推荐 GCC 13+）
- Qt 6.x（通过 apt 安装 `qt6-base-dev` 等）或 Qt 5.12+（手动安装）

#### Ubuntu 24.04 (WSL) apt 依赖安装

```bash
# Qt6 核心开发包（必需）
sudo apt install qt6-base-dev qt6-base-dev-tools qt6-svg-dev

# Qt6 扩展模块（翻译工具 LinguistTools 需要）
sudo apt install qt6-tools-dev

# Ninja（推荐，编译更快）
sudo apt install ninja-build
```

> **注意**：SARibbon 不依赖 Qt private headers，因此 `qt6-base-private-dev` 不是必需的。如使用 `SARIBBON_USE_FRAMELESS_LIB=ON` 启用 QWindowKit 无边框方案，则可能需要 Qt private headers，请参考 QWindowKit 的构建要求。

## 构建步骤

### 第一步：获取源码

```bash
git clone https://github.com/czyt1988/SARibbon.git
cd SARibbon

# 如果未使用 --recursive 参数克隆，需拉取子模块（QWindowKit 等）
git submodule update --init --recursive
```

### 第二步：构建 SARibbon

#### Windows (推荐：使用 build.ps1 脚本)

`scripts/build.ps1` 自动完成所有环境设置（检测 Qt 路径、自动匹配 VS 版本、清理构建、配置和编译），无需手动指定任何路径：

```powershell
# 标准构建（Release，自动检测 Qt 和 MSVC）
.\scripts\build.ps1

# Debug 构建
.\scripts\build.ps1 -Config Debug

# 清理并重建
.\scripts\build.ps1 -Clean

# 指定 Qt 路径（如果不从 C:\Qt 自动检测）
.\scripts\build.ps1 -QtPath "D:/Qt/6.7.3/msvc2019_64"

# 指定 MSVC 版本（2019 或 2022，默认从 Qt 路径自动推断）
.\scripts\build.ps1 -MsvcVersion 2022
```

> 首次使用可能需要 `Set-ExecutionPolicy -Scope CurrentUser RemoteSigned` 以允许执行 PowerShell 脚本。

#### Windows (手动构建 — Visual Studio 生成器)

如果不使用脚本，需手动完成以下步骤：

```powershell
# 1. CMake 配置
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64"

# 2. 构建
cmake --build build --config Release --parallel
```

#### Windows (手动构建 — Ninja 生成器，需初始化 MSVC 环境)

```powershell
# 初始化 MSVC 环境（vcvarsall.bat 会设置 INCLUDE、LIB、PATH 等关键变量）
cmd /c '"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 & set' | ForEach-Object {
    if ($_ -match '^([^=]+)=(.*)$') {
        [Environment]::SetEnvironmentVariable($matches[1], $matches[2], 'Process')
    }
}

# 然后使用 Ninja 构建
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64"
cmake --build build
```

> **注意**：Ninja 生成器是单配置生成器，`CMAKE_BUILD_TYPE` 在配置时指定，构建时无需再传 `--config`。

#### Linux / WSL

```bash
# 使用 Ninja 构建（推荐）
cmake -S . -B build-linux -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-linux --parallel

# 或使用 Unix Makefiles
cmake -S . -B build-linux -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build-linux --parallel
```

> **注意**：Linux 上系统 Qt6 通过 apt 安装后无需指定 `CMAKE_PREFIX_PATH`，cmake 会自动找到。如使用自定义 Qt 安装路径，需添加 `-DCMAKE_PREFIX_PATH=<Qt路径>`。

### 第三步：运行测试（可选）

使用脚本一键构建并运行测试：

```powershell
# 方式一：通过 build.ps1（自动检测 CTest）
.\scripts\build.ps1 -EnableTests

# 方式二：手动运行
cmake --build build --config Release --target SARibbonThemePaletteTest
cd build && ctest --output-on-failure -C Release
```

```bash
# Linux / WSL
cmake -S . -B build-linux -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build-linux --parallel
cd build-linux && ctest --output-on-failure
```

## CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `SARIBBON_BUILD_STATIC_LIBS` | OFF | 静态库，ON 时自动定义 `SA_RIBBON_BAR_NO_EXPORT` |
| `SARIBBON_USE_FRAMELESS_LIB` | OFF | 使用 QWindowKit 无边框方案，需 C++17 和 QWindowKit 库；OFF 时由 CMakeLists 根据 Qt 版本自动选择 C++ 标准 |
| `SARIBBON_BUILD_EXAMPLES` | ON | 控制是否编译示例程序（如 MainWindowExample 等） |
| `SARIBBON_ENABLE_SNAPLAYOUT` | OFF | 启用 Windows 11 Snap Layout（仅 `SARIBBON_USE_FRAMELESS_LIB=ON` 时有效） |
| `SARIBBON_INSTALL_IN_CURRENT_DIR` | ON (Windows) | 安装到 `bin_qt{版本}_{编译器}_x{架构}/` |
| `BUILD_TESTS` | OFF | 启用单元测试（Qt Test 框架） |

## 注意事项

1. **生成器选择**：Windows 推荐使用 Visual Studio 生成器（自动处理 MSVC 环境）；如使用 Ninja 需先初始化 MSVC 环境变量；Linux/WSL 推荐 Ninja（编译更快）
2. **Qt 路径**：Windows 需指定 `CMAKE_PREFIX_PATH`；Linux apt 安装的 Qt6 无需指定
3. **清理构建**：如需重新配置，删除 `build` 目录重新 cmake 即可。需先关掉 `build/bin` 下占用的程序
4. **安装目录命名**：自动生成的安装目录名为 `bin_qt{版本}_{编译器}_x{架构}`，例如 Windows 下为 `bin_qt6.7.3_msvc2019_x64`
5. **C++ 标准**：Qt6 构建始终使用 C++17；启用 `SARIBBON_USE_FRAMELESS_LIB` 时强制 C++17；其余情况使用 C++14
6. **安装命令**：构建完成后可执行 `cmake --install build --config Release` 将库安装到本地目录，便于其他项目通过 `find_package` 引用

## 常见问题

### Linux 构建常见问题

| 问题 | 原因 | 解决方案 |
|------|------|---------|
| `Could NOT find Qt6LinguistTools` | 缺少 Qt6 Tools 开发包 | `sudo apt install qt6-tools-dev` |
| `QIODevice` incomplete type | 缺少 `#include <QIODevice>` | Qt6 不再通过 `QDataStream` 隐式包含，需手动添加 |
| `uint64_t` ambiguous overload | Linux 上 `uint64_t` = `unsigned long` ≠ `unsigned long long` | 使用 `qulonglong` 或 `static_cast<qulonglong>()` |
| moc 异常退出 / 不完整类型 | Qt 信号槽传递自定义类型指针时只有前向声明 | 在 .cpp 文件中 `#include` 完整头文件而非仅前向声明 |
| SARibbonAlignment 枚举编译错误 | 文件换行为 LF 而非 CRLF | SARibbonGlobal.h 要求 CRLF 换行，用 `git config core.autocrlf true` 或手动转换 |

### Windows 构建常见问题

| 问题 | 原因 | 解决方案 |
|------|------|---------|
| Ninja 生成器找不到标准库头文件 | 未初始化 MSVC 环境变量 | 先运行 `vcvarsall.bat x64` 或使用 VS 生成器 |
| moc 异常退出 | Qt moc 大量项目时的已知 bug | 重新构建即可 |
| 构建损坏 / 无法重配 | `build/bin` 下程序占用了文件 | 先关掉占用程序，再 `Remove-Item -Recurse -Force build` 重配 |