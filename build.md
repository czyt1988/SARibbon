# SARibbon 构建指南

## 前置依赖

| 依赖 | 最低版本 | 说明 |
|------|---------|------|
| CMake | 3.15+ | 项目使用 CMake 构建系统（脚本自动检测 VS 内嵌或独立安装） |
| C++14/C++17 编译器 | - | MSVC 2019+ / GCC 9+；Qt6 或启用 frameless 时强制 C++17 |
| Qt | 5.12+ | 需要 Core、Gui、Widgets、Svg 模块；Qt6 额外需要 Core5Compat（可选） |

> **注意**：`SARIBBON_USE_FRAMELESS_LIB` 对 Qt 版本有要求：Qt 5.14+、Qt 6.2+，低版本会自动禁用。

## 一键构建（推荐）

`scripts/build.ps1` 脚本自动探测 Qt 安装路径、VS 版本、CMake 位置，无需手动指定任何路径：

```powershell
# 一键构建（默认 Release + Examples ON）
.\scripts\build.ps1

# 常用操作
.\scripts\build.ps1 build            # 增量编译
.\scripts\build.ps1 rebuild          # 清除 + 重配 + 编译
.\scripts\build.ps1 configure -Examples OFF  # 仅库（不编示例）
.\scripts\build.ps1 help             # 查看所有选项
```

脚本功能：
- **Qt 自动检测**：搜索 `D:\Qt`、`C:\Qt`、`~\Qt`、`Program Files\Qt` 等常见目录，查找 msvc*_64 安装
- **VS 自动检测**：通过 vswhere.exe 自动确定 Visual Studio 版本（2019/2022）
- **CMake 自动检测**：先查 PATH，再查 VS 内嵌路径，最后查独立安装
- **锁定文件保护**：清除构建目录前检测运行中的 exe，提示用户先关闭

也可手动指定路径（覆盖自动检测）：

```powershell
.\scripts\build.ps1 full -QtPath "D:\Qt\Qt5.15.16\5.15.16\msvc2019_64" -VSVersion 2019
```

## 手动构建

如果不使用脚本，需手动指定所有参数。**关键注意事项**：

1. **必须使用 Visual Studio 生成器**，不要用 Ninja — PowerShell 中 `vcvars64.bat` 无法注入 MSVC 环境
2. **Qt 版本必须与 VS 编译器匹配**：Qt msvc2019 对应 VS2019，Qt msvc2022 对应 VS2022
3. **CMake 可能不在 PATH 中**：VS2019 内嵌的 CMake 在 `C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`

```powershell
# 配置（首次）
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH="<Qt安装路径>"
# 编译
cmake --build build --config Release
# 安装（可选）
cmake --install build --config Release
```

常见 Qt 安装路径示例（仅作参考，实际路径取决于安装位置）：
```powershell
# Qt 5.15.x
-DCMAKE_PREFIX_PATH="D:/Qt/Qt5.15.16/5.15.16/msvc2019_64"
# Qt 6.x.x
-DCMAKE_PREFIX_PATH="C:/Qt/6.7.3/msvc2019_64"
```

## 常见构建问题

### 1. CMake 找不到 / 不在 PATH

**原因**：VS2019 内嵌的 CMake 不在系统 PATH 中。

**解决方案**：使用 `scripts/build.ps1`（自动检测），或将 CMake 加入 PATH：
```powershell
# VS2019 内嵌 CMake 路径（视 VS 安装版本而定）
$env:PATH += ";C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin"
```

### 2. Qt 路径找不到

**原因**：`CMAKE_PREFIX_PATH` 指向了不存在的 Qt 目录。

**解决方案**：
- **推荐**：使用 `scripts/build.ps1`（自动探测 Qt）
- 手动确认 Qt 安装路径是否存在，常见位置：`D:\Qt\Qt5.x.x\5.x.x\msvc2019_64`、`C:\Qt\6.x.x\msvc2019_64`
- Qt 目录下应有 `lib/cmake/Qt5/Qt5Config.cmake` 或 `lib/cmake/Qt6/Qt6Config.cmake`

### 3. `fatal error C1083: 无法打开包括文件: "memory"/"type_traits"`

**原因**：使用 Ninja 生成器但 MSVC 环境未注入。

**解决方案**：**不要用 Ninja**。使用 VS 生成器或 `scripts/build.ps1`。

### 4. Qt 版本与 VS 编译器不匹配

**原因**：Qt msvc2019 安装配合 VS2022 编译器，或反之。

**解决方案**：确保 Qt 编译器版本与 VS 版本一致。使用 `scripts/build.ps1` 会自动匹配。

### 5. 构建目录损坏需要重新配置

```powershell
# 使用脚本
.\scripts\build.ps1 rebuild

# 手动
Remove-Item -Recurse -Force build
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH="<Qt安装路径>"
cmake --build build --config Release
```

> **注意**：如果 `build/bin` 下有正在运行的程序占用文件，需要先关闭该程序再删除。`scripts/build.ps1 rebuild` 会检测并提示。

### 6. vcvars64.bat 在 PowerShell 中不生效

**原因**：PowerShell 与 bat 脚本交互的已知限制 — 环境变量不会传递给后续命令。

**解决方案**：**不要依赖 vcvars64.bat**。使用 VS 生成器（自动处理 MSVC 环境）或 `scripts/build.ps1`。

## CMake 构建选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `SARIBBON_BUILD_STATIC_LIBS` | OFF | 静态库，ON 时自动定义 `SA_RIBBON_BAR_NO_EXPORT` |
| `SARIBBON_BUILD_EXAMPLES` | ON | 构建示例程序 |
| `SARIBBON_USE_FRAMELESS_LIB` | OFF | 使用 QWindowKit 无边框方案，需 C++17 和 QWindowKit 库 |
| `SARIBBON_ENABLE_SNAPLAYOUT` | OFF | 启用 Windows 11 Snap Layout（仅 frameless 模式有效） |
| `SARIBBON_INSTALL_IN_CURRENT_DIR` | ON (Windows) | 安装到 `bin_qt{版本}_{编译器}_x{架构}/` |
| `BUILD_TESTS` | OFF | 启用单元测试（Qt Test 框架） |

脚本对应参数：`-Examples <ON|OFF>`、`-Tests <ON|OFF>`、`-StaticLibs <ON|OFF>`、`-Frameless <ON|OFF>`、`-SnapLayout <ON|OFF>`。

手动示例：
```powershell
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 `
    -DCMAKE_PREFIX_PATH="<Qt安装路径>" `
    -DSARIBBON_BUILD_EXAMPLES=OFF `
    -DSARIBBON_BUILD_STATIC_LIBS=OFF
```

## 构建产物

构建完成后，运行 install 步骤将产物安装到 `bin_qt{版本}_{编译器}_x{架构}/` 目录：

```
bin_qt6.7.3_MSVC_x64/
├── bin/                  # 可执行文件和 DLL
│   ├── SARibbon.dll
│   └── *.exe             # 示例程序
├── lib/                  # 导入库（.lib）
├── include/              # 头文件
└── share/                # CMake config 文件
```

## 快速参考（Agent 专用）

```powershell
# 一键构建（推荐）
.\scripts\build.ps1                          # 全量构建
.\scripts\build.ps1 build                    # 增量编译
.\scripts\build.ps1 rebuild                  # 清除重配重编
.\scripts\build.ps1 configure -Examples OFF  # 仅库
.\scripts\build.ps1 configure -StaticLibs ON # 静态库

# 手动构建（不推荐，需自行确定路径）
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH="<Qt路径>"
cmake --build build --config Release
cmake --install build --config Release
```

> **务必使用 Visual Studio 生成器**，不要用 Ninja，因为在 PowerShell 中 MSVC 编译器环境无法通过 `vcvars64.bat` 正确注入。`scripts/build.ps1` 自动处理此问题。
