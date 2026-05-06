# 构建指引

## 构建环境

- CMake 3.15+
- Visual Studio 2019（MSVC 14.29+）
- Qt 6.7+ (msvc2019\_64) 或 Qt 5.14+

## 构建步骤

本项目使用 CMake 构建，推荐使用 **Visual Studio 生成器**，因为它会自动初始化 MSVC 编译器环境。如果使用 Ninja 生成器，需要手动初始化 MSVC 环境变量。

### 方法一：Visual Studio 生成器（推荐）

```powershell
# 在项目根目录下
cd build

# 配置（使用 Visual Studio 2019 生成器）
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH="D:/Qt/6.7.3/msvc2019_64"

# 构建（Debug 或 Release）
cmake --build . --config Debug
# 或
cmake --build . --config Release
```

### 方法二：Ninja 生成器（需初始化 MSVC 环境）

使用 Ninja 时必须先初始化 MSVC 环境变量（`INCLUDE`、`LIB`、`PATH` 等），否则编译器会找不到标准库头文件。

在 PowerShell 中手动初始化 MSVC 环境**

```powershell
# 初始化 MSVC 环境（vcvarsall.bat 会设置 INCLUDE、LIB、PATH 等关键变量）
cmd /c '"D:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 & set' | ForEach-Object {
    if ($_ -match '^([^=]+)=(.*)$') {
        [Environment]::SetEnvironmentVariable($matches[1], $matches[2], 'Process')
    }
}

# 然后使用 Ninja 构建
cd F:\src\QIm\build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="D:/Qt/6.7.3/msvc2019_64"
cmake --build .
```

## 注意事项

1. **生成器选择**：推荐使用 Visual Studio 生成器（自动处理 MSVC 环境）；使用 Ninja 需先初始化 MSVC 环境变量
2. **Qt路径**：根据实际 Qt 安装位置调整 `CMAKE_PREFIX_PATH`
3. **清理构建**：如需重新配置，删除 `build` 目录下的 `CMakeCache.txt` 和 `CMakeFiles` 目录：