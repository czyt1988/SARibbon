# 构建指引

## 构建环境

### Windows

- CMake 3.15+
- Visual Studio 2019（MSVC 14.29+）
- Qt 6.7+ (msvc2019\_64) 或 Qt 5.12+

### Linux / WSL

- CMake 3.15+
- GCC 9+（推荐 GCC 13+）
- Qt 6.x（通过 apt 安装 `qt6-base-dev` 等）或 Qt 5.12+（手动安装）

#### Ubuntu 24.04 (WSL) apt 依赖安装

```bash
# Qt6 核心开发包
sudo apt install qt6-base-dev qt6-base-dev-tools qt6-svg-dev

# Qt6 扩展模块（翻译需要 LinguistTools）
sudo apt install qt6-tools-dev

# Ninja（编译更快）
sudo apt install ninja-build
```

> **注意**：SARibbon 不依赖 Qt private headers，因此 `qt6-base-private-dev` 不是必需的。如使用 `SARIBBON_USE_FRAMELESS_LIB=ON` 启用 QWindowKit 无边框方案，则可能需要 Qt private headers，请参考 QWindowKit 的构建要求。

## 构建步骤

### Windows (Visual Studio 生成器，推荐)

```powershell
# 在项目根目录下
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64"
cmake --build build --config Release
```

### Windows (Ninja 生成器，需初始化 MSVC 环境)

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

### Linux / WSL (Unix Makefiles 或 Ninja)

```bash
# 在项目根目录下
cmake -S . -B build-linux -G Ninja -DCMAKE_BUILD_TYPE=Release

# 或使用 Unix Makefiles
cmake -S . -B build-linux -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release

# 构建
cmake --build build-linux --parallel
```

> **注意**：Linux 上系统 Qt6 通过 apt 安装后无需指定 `CMAKE_PREFIX_PATH`，cmake 会自动找到。如使用自定义 Qt 安装路径，需添加 `-DCMAKE_PREFIX_PATH=<Qt路径>`。

### CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `SARIBBON_BUILD_STATIC_LIBS` | OFF | 静态库，ON 时自动定义 `SA_RIBBON_BAR_NO_EXPORT` |
| `SARIBBON_USE_FRAMELESS_LIB` | OFF | 使用 QWindowKit 无边框方案，需 C++17 和 QWindowKit 库 |
| `SARIBBON_BUILD_EXAMPLES` | ON | 控制是否编译示例程序 |
| `SARIBBON_ENABLE_SNAPLAYOUT` | OFF | 启用 Windows 11 Snap Layout（仅 `SARIBBON_USE_FRAMELESS_LIB=ON` 时有效） |
| `SARIBBON_INSTALL_IN_CURRENT_DIR` | ON (Windows) | 安装到 `bin_qt{版本}_{编译器}_x{架构}/` |
| `BUILD_TESTS` | OFF | 启用单元测试（Qt Test 框架） |

## 注意事项

1. **生成器选择**：Windows 推荐使用 Visual Studio 生成器（自动处理 MSVC 环境）；Linux/WSL 推荐 Ninja（编译更快）
2. **Qt路径**：Windows 需指定 `CMAKE_PREFIX_PATH`；Linux apt 安装的 Qt6 无需指定
3. **清理构建**：如需重新配置，删除 `build` 目录重新 cmake 即可
4. **安装目录命名**：自动生成的安装目录名为 `bin_qt{版本}_{编译器}_x{架构}`，例如 Windows 下为 `bin_qt6.7.3_msvc2019_x64`

## 常见问题

### Linux 构建常见问题

| 问题 | 原因 | 解决方案 |
|------|------|---------|
| `Could NOT find Qt6LinguistTools` | 缺少 Qt6 Tools 开发包 | `sudo apt install qt6-tools-dev` |
| `uint64_t` ambiguous overload | Linux 上 `uint64_t` = `unsigned long` ≠ `unsigned long long` | 使用 `qulonglong` 或 `static_cast<qulonglong>()` |
| moc 异常退出 / 不完整类型 | Qt 信号槽传递自定义类型指针时只有前向声明 | 在 .cpp 文件中 `#include` 完整头文件而非仅前向声明 |
| SARibbonAlignment 枚举编译错误 | 文件换行为 LF 而非 CRLF | SARibbonGlobal.h 要求 CRLF 换行，用 `git config core.autocrlf true` 或手动转换 |

### Windows 构建常见问题

| 问题 | 原因 | 解决方案 |
|------|------|---------|
| Ninja 生成器找不到标准库头文件 | 未初始化 MSVC 环境变量 | 先运行 `vcvarsall.bat x64` 或使用 VS 生成器 |
| moc 异常退出 | Qt moc 大量项目时的已知 bug | 重新构建即可 |
| 构建损坏 / 无法重配 | `build/bin` 下程序占用了文件 | 先关掉占用程序，再 `Remove-Item -Recurse -Force build` 重配 |