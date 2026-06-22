# Python 绑定构建指南

本文档详细介绍如何从源码构建 SARibbon 的 Python 绑定（PyQtSARibbon）。

## 子文档导航

| 文档 | 内容 |
|------|------|
| [构建 Python 绑定](./build-python-bindings.md) | 编译安装 PyQtSARibbon |
| [使用 Python 绑定](./use-python-bindings.md) | 在 Python 中使用 SARibbon |

## 环境要求

在开始构建之前，请确保已安装以下软件：

| 软件 | 版本要求 | 说明 |
|------|----------|------|
| Python | 3.8 或更高 | 建议使用 3.10+ |
| PyQt5 | 5.15.4 或更高 | Python Qt 绑定 |
| Qt | 5.15 或更高 | C++ Qt 框架 |
| sip | 6.0.2 或更高 | PyQt 构建工具 |
| PyQt-builder | 1.6 或更高 | PyQt 构建系统 |
| C++ 编译器 | MSVC 2019+ / GCC 9+ | 编译原生代码 |

## 安装依赖

### 1. 安装 Python 依赖

使用 pip 安装必要的 Python 包：

```bash
pip install PyQt5 PyQt5-sip PyQt-builder
```

验证安装：

```bash
python -c "import PyQt5; print(PyQt5.__version__)"
python -c "import sipbuild; print(sipbuild.version.SIP_VERSION_STR)"
```

### 2. 确认 Qt 安装

确保 Qt 已正确安装并配置环境变量：

```bash
qmake --version
```

如果命令未找到，需要将 Qt 的 bin 目录添加到系统 PATH 中。

### 3. 配置编译器

#### Windows (MSVC)

使用 Visual Studio 开发者命令提示符：

```bash
# 在开始菜单搜索 "x64 Native Tools Command Prompt for VS 2019"
# 或者运行：
"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" x64
```

#### Linux (GCC)

确保已安装开发工具：

```bash
sudo apt-get install build-essential
```

#### macOS (Clang)

确保已安装 Xcode 命令行工具：

```bash
xcode-select --install
```

## 构建步骤

### 1. 克隆源码

```bash
git clone https://github.com/czyt1988/SARibbon.git
cd SARibbon
```

### 2. 构建 Python 绑定

在项目根目录执行：

```bash
sip-build --build-dir build-python
```

这个命令会：

1. 生成 C++ 绑定代码
2. 编译原生扩展模块
3. 生成 PyQtSARibbon.pyd (Windows) 或 PyQtSARibbon.so (Linux/macOS)

!!! tip "提示"
    如果构建过程中出现 Qt 路径错误，可以显式指定 qmake 路径：

    ```bash
    sip-build --qmake /path/to/qmake --build-dir build-python
    ```

### 3. 安装模块

构建成功后，将编译好的模块复制到 Python 的 site-packages 目录：

#### Windows

```bash
copy build-python\PyQtSARibbon\PyQtSARibbon.pyd <Python路径>\Lib\site-packages\
```

#### Linux/macOS

```bash
cp build-python/PyQtSARibbon/PyQtSARibbon.so <Python路径>/lib/python3.x/site-packages/
```

或者使用 pip 安装：

```bash
pip install .
```

## 验证安装

```bash
python -c "from PyQtSARibbon import SARibbonBar; print('PyQtSARibbon 安装成功！')"
```

## 常见问题

### 问题 1：找不到 Qt 头文件

**错误信息**：atal error: QtWidgets: No such file or directory

**解决方案**：

```bash
sip-build --qmake /path/to/qmake --build-dir build-python
```

### 问题 2：编译器找不到

**错误信息**：Cannot run compiler

**解决方案**：确保在 Visual Studio 开发者命令提示符中执行构建命令。

### 问题 3：DLL 加载失败

**错误信息**：ImportError: DLL load failed while importing PyQtSARibbon

**解决方案**：将 Qt 的 DLL 复制到 site-packages 目录：

```bash
copy <Qt路径>\bin\Qt5Widgets.dll <Python路径>\Lib\site-packages\
copy <Qt路径>\bin\Qt5Gui.dll <Python路径>\Lib\site-packages\
copy <Qt路径>\bin\Qt5Core.dll <Python路径>\Lib\site-packages\
```

### 问题 4：版本不匹配

**错误信息**：ImportError: Module use of python3x.dll conflicts

**解决方案**：确保 PyQt5、sip 和 Python 版本兼容。

## 下一步

请参阅 [使用 Python 绑定](./use-python-bindings.md) 了解如何使用 SARibbon。
