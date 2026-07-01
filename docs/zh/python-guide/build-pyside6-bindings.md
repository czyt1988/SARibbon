# PySide6 绑定构建指南

本文档介绍如何从源码构建 SARibbon 的 PySide6 绑定（PySideSARibbon），使用 Qt 官方的 Shiboken6 生成器。

与 PyQt 绑定（基于 SIP）不同，PySide6 绑定基于 Shiboken6 + CMake 构建系统，但共享同一套 C++ 源码。

## 与 PyQt 绑定的对比

| 对比项 | PyQt (SIP) | PySide6 (Shiboken6) |
|--------|-----------|-------------------|
| 绑定描述 | `.sip` 文件 | `typesystem_saribbon.xml` |
| 构建系统 | pyproject.toml + sip-build | CMakeLists.txt + CMake |
| 枚举处理 | `%MappedType` 转 int | `<enum-type>` 原生 Python Enum |
| 资源处理 | qmake RESOURCES | CMake qt_add_resources |
| 模块名 | PyQtSARibbon.saribbon | PySideSARibbon.saribbon |

## 环境要求

| 软件 | 版本要求 | 说明 |
|------|----------|------|
| Python | 3.9 或更高 | 建议使用 3.10+ |
| PySide6 | 6.5 或更高 | Qt 官方 Python 绑定 |
| shiboken6-generator | 匹配 PySide6 版本 | Shiboken6 绑定生成器 |
| **Qt6 开发包** | 6.5 或更高 | **必须**：需要 Qt6 C++ 头文件和 CMake 配置 |
| CMake | 3.22 或更高 | 构建系统 |
| Ninja | 最新版 | 推荐的构建后端 |
| C++ 编译器 | MSVC 2019+ / GCC 9+ | 编译原生代码 |

!!! warning "重要"
    PySide6 pip 包**不包含**完整的 Qt6 C++ 开发头文件（如 `qglobal.h`）。
    必须通过 Qt Online Installer 或 `aqtinstall` 安装 Qt6 开发包。

### 安装 Qt6 开发包

#### 方式一：Qt Online Installer（推荐）

从 [Qt 官网](https://www.qt.io/download) 下载安装器，安装 Qt 6.7+ for MSVC 2019 64-bit。

#### 方式二：aqtinstall

```bash
pip install aqtinstall
python -m aqt install-qt windows desktop 6.7.3 win64_msvc2019_64 --outputdir D:\Qt6
```

安装后 Qt6 路径为 `D:\Qt6\6.7.3\msvc2019_64`。

## 安装依赖

### 1. 安装 Python 依赖

```bash
pip install PySide6 shiboken6-generator
```

验证安装：

```bash
python -c "import PySide6; print(PySide6.__version__)"
python -c "import shiboken6_generator; print('shiboken6-generator OK')"
```

### 2. 确认 Qt 安装

```bash
qmake --version
```

### 3. 安装 CMake 和 Ninja

```bash
pip install cmake ninja
```

或从 [CMake 官网](https://cmake.org/download/) 下载安装。

### 4. 配置编译器

#### Windows (MSVC)

使用 Visual Studio 开发者命令提示符：

```bash
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
```

#### Linux (GCC)

```bash
sudo apt-get install build-essential
```

## 构建步骤

### 1. 克隆源码

```bash
git clone https://github.com/czyt1988/SARibbon.git
cd SARibbon
```

### 2. 构建 PySide6 绑定

#### 方式一：使用构建脚本（Windows 推荐）

```bash
tools\build_pyside6_bindings.bat
```

也可指定 Python 和 Qt 路径：

```bash
tools\build_pyside6_bindings.bat "C:\Python311" "D:\Qt\6.7.3\msvc2019_64"
```

脚本会自动：

1. 检查 PySide6 和 shiboken6-generator 是否安装
2. 初始化 MSVC 环境
3. 执行 CMake 配置和构建
4. 安装到 site-packages

#### 方式二：手动 CMake 构建

```bash
cmake -S pyside6 -B build-pyside6 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-pyside6 --parallel
```

如果需要指定 Qt 路径：

```bash
cmake -S pyside6 -B build-pyside6 -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="D:/Qt/6.7.3/msvc2019_64"
```

### 3. 安装模块

```bash
cmake --install build-pyside6
```

或手动复制构建产物到 site-packages。

## 验证安装

```bash
python -c "from PySideSARibbon import saribbon; print('PySideSARibbon 安装成功！')"
```

## 示例程序

```bash
python pyexamples/pyside6/ribbon_demo.py
```

## 常见问题

### 问题 1：找不到 shiboken6-generator

**错误信息**：`Could not find shiboken6 executable`

**解决方案**：确保已安装 shiboken6-generator：

```bash
pip install shiboken6-generator
```

> **注意**：当前 CMakeLists.txt 不依赖 `find_package(Shiboken6Tools)`，而是手动定位 `shiboken6.exe`、库文件和头文件路径。PySide6 6.8.x 不包含 `Shiboken6ToolsConfig.cmake`，因此旧版本的 `find_package(Shiboken6Tools)` 方式不再使用。

### 问题 2：找不到 Qt 头文件

**错误信息**：`fatal error: QtWidgets: No such file or directory`

**解决方案**：指定 Qt 路径：

```bash
cmake -S pyside6 -B build-pyside6 -DCMAKE_PREFIX_PATH="/path/to/Qt/6.7.3/msvc2019_64"
```

### 问题 3：编译器找不到

**错误信息**：Cannot run compiler 'cl'

**解决方案**：确保在 Visual Studio 开发者命令提示符中执行构建。

### 问题 4：DLL 加载失败

**错误信息**：`ImportError: DLL load failed while importing saribbon`

**解决方案**：确保 PySide6 和 Qt 的 DLL 在 PATH 中，或将 Qt DLL 复制到 site-packages。

### 问题 5：PyQt 和 PySide6 共存冲突

建议使用虚拟环境隔离 PyQt 和 PySide6 环境：

```bash
python -m venv venv-pyside6
venv-pyside6\Scripts\activate
pip install PySide6 shiboken6-generator
```

### 问题 6：PySide6 与 Qt6 版本不匹配

**现象**：构建成功但运行时崩溃，或出现 `qjsonparseerror.h: No such file or directory` 等头文件版本不匹配错误。

**根因**：PySide6 pip 包中的 Qt 头文件版本必须与系统安装的 Qt6 C++ 开发包版本匹配。例如 PySide6 6.8.3 需要搭配 Qt 6.8.3 开发包。

**解决方案**：确保版本一致：

```bash
# 检查 PySide6 版本
python -c "import PySide6; print(PySide6.__version__)"

# 安装匹配的 Qt6 开发包（以 6.8.3 为例）
python -m aqt install-qt windows desktop 6.8.3 win64_msvc2022_64 -O D:\Qt
```

### 问题 7：运行时主题和图标不显示

**现象**：窗口启动后无 QSS 主题样式、图标不显示，控制台输出 `can not load build in ribbon theme`。

**根因**：Qt 资源文件（`.qrc`）的 RCC 静态初始化器被 MSVC 链接器的 `/OPT:REF` 优化剥离。当 `qt_add_resources` 的输出放在静态库 `saribbon_lib` 中时，由于没有代码显式引用 qrc 生成的符号，整个 qrc 对象文件被从最终的 `.pyd` 中移除。

**解决方案**：确保 `qt_add_resources` 的输出编译进最终的 MODULE 目标（`.pyd`），而非静态库。当前 CMakeLists.txt 已正确处理此问题，注释中说明了原因：

```cmake
# NOTE: qrc resources are NOT added to saribbon_lib. They are compiled
# directly into the final .pyd module (see below) to ensure the RCC
# static initializer is included.
```

### 问题 8：QAction 构造函数间接引用崩溃

> **更新**：经深入排查，此问题并非 shiboken 类型注册冲突导致，而是文件编码问题引起的假象。间接引用 Qt 类（通过变量、字典、模块属性）在 PySide6 绑定中**完全正常**，符合 Python 惯例。确保 Python 源文件统一使用 UTF-8 无 BOM 编码、LF 换行即可。

## 下一步

请参阅 [使用 Python 绑定](./use-python-bindings.md) 了解如何在 Python 中使用 SARibbon。
