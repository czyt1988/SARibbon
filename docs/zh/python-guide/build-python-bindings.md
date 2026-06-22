# Python 绑定构建指南

本文档详细介绍如何从源码构建 SARibbon 的 Python 绑定（PyQtSARibbon）。

SARibbon 同时支持 PyQt5 和 PyQt6 绑定，两者共享同一套 SIP 绑定源码，仅构建配置不同。

## 子文档导航

| 文档 | 内容 |
|------|------|
| [构建 Python 绑定](./build-python-bindings.md) | 编译安装 PyQtSARibbon |
| [使用 Python 绑定](./use-python-bindings.md) | 在 Python 中使用 SARibbon |

## 环境要求

### PyQt5 构建

| 软件 | 版本要求 | 说明 |
|------|----------|------|
| Python | 3.8 或更高 | 建议使用 3.10+ |
| PyQt5 | 5.15.4 或更高 | Python Qt 绑定 |
| Qt | 5.15 或更高 | C++ Qt 框架 |
| sip | 6.0.2 或更高 | PyQt 构建工具 |
| PyQt-builder | 1.6 或更高 | PyQt 构建系统 |
| C++ 编译器 | MSVC 2019+ / GCC 9+ | 编译原生代码 |

### PyQt6 构建

| 软件 | 版本要求 | 说明 |
|------|----------|------|
| Python | 3.8 或更高 | 建议使用 3.10+ |
| PyQt6 | 6.2 或更高 | Python Qt6 绑定 |
| Qt | 6.2 或更高 | C++ Qt6 框架 |
| sip | 6.0.2 或更高 | PyQt 构建工具 |
| PyQt-builder | 1.6 或更高 | PyQt 构建系统 |
| C++ 编译器 | MSVC 2019+ / GCC 9+ | 编译原生代码 |

## 安装依赖

### 1. 安装 Python 依赖

=== "PyQt5"

    ```bash
    pip install PyQt5 PyQt5-sip PyQt-builder
    ```

    验证安装：

    ```bash
    python -c "import PyQt5; print(PyQt5.__version__)"
    python -c "import sipbuild; print(sipbuild.version.SIP_VERSION_STR)"
    ```

=== "PyQt6"

    ```bash
    pip install PyQt6 PyQt6-sip PyQt-builder
    ```

    验证安装：

    ```bash
    python -c "import PyQt6; print(PyQt6.__version__)"
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

#### 方式一：使用构建脚本（Windows 推荐）

项目提供了 `tools/build_python_bindings.bat` 构建脚本，自动处理 MSVC 环境初始化和安装：

=== "PyQt5"

    ```bash
    tools\build_python_bindings.bat
    ```

    也可指定 Python 和 Qt 路径：

    ```bash
    tools\build_python_bindings.bat "C:\Python311" "D:\Qt\5.15.16\msvc2019_64\bin\qmake.exe"
    ```

=== "PyQt6"

    ```bash
    tools\build_python_bindings.bat --pyqt6
    ```

    也可指定 Python 和 Qt 路径：

    ```bash
    tools\build_python_bindings.bat "C:\Python311" "D:\Qt\6.7.3\msvc2019_64\bin\qmake.exe" --pyqt6
    ```

    脚本会自动：
    1. 将 `pyproject-pyqt6.toml` 临时替换为 `pyproject.toml`
    2. 执行 `sip-build`，构建到 `build-python6` 目录
    3. 恢复原始 `pyproject.toml`
    4. 安装到 site-packages，`__init__.py` 中写入 `import PyQt6.QtCore`

#### 方式二：手动 sip-build

=== "PyQt5"

    在项目根目录执行：

    ```bash
    sip-build --build-dir build-python
    ```

    !!! tip "提示"
        如果构建过程中出现 Qt 路径错误，可以显式指定 qmake 路径：

        ```bash
        sip-build --qmake /path/to/qmake --build-dir build-python
        ```

=== "PyQt6"

    PyQt6 构建需要使用 `pyproject-pyqt6.toml` 配置文件。先将其临时替换为 `pyproject.toml`：

    ```bash
    copy pyproject.toml pyproject.toml.bak
    copy pyproject-pyqt6.toml pyproject.toml
    sip-build --build-dir build-python6
    copy pyproject.toml.bak pyproject.toml
    del pyproject.toml.bak
    ```

    !!! tip "提示"
        如果构建过程中出现 Qt 路径错误，可以显式指定 qmake 路径：

        ```bash
        sip-build --qmake /path/to/qmake --build-dir build-python6
        ```

这个命令会：

1. 生成 C++ 绑定代码
2. 编译原生扩展模块
3. 生成 PyQtSARibbon 包目录，包含 `__init__.py` 和 `saribbon.pyd` (Windows) 或 `saribbon.so` (Linux/macOS)

### 3. 安装模块

如果使用了构建脚本（方式一），模块已自动安装，可跳过此步骤。

构建成功后，将编译好的模块复制到 Python 的 site-packages 目录：

#### Windows

=== "PyQt5"

    ```bash
    xcopy /s /y build-python\PyQtSARibbon <Python路径>\Lib\site-packages\PyQtSARibbon\
    ```

=== "PyQt6"

    ```bash
    xcopy /s /y build-python6\PyQtSARibbon <Python路径>\Lib\site-packages\PyQtSARibbon\
    ```

#### Linux/macOS

=== "PyQt5"

    ```bash
    cp -r build-python/PyQtSARibbon <Python路径>/lib/python3.x/site-packages/
    ```

=== "PyQt6"

    ```bash
    cp -r build-python6/PyQtSARibbon <Python路径>/lib/python3.x/site-packages/
    ```

或者使用 pip 安装：

```bash
pip install .
```

## 验证安装

```bash
python -c "from PyQtSARibbon import saribbon; print('PyQtSARibbon 安装成功！')"
```

## 示例程序

构建安装完成后，可以运行示例程序验证功能：

=== "PyQt5"

    ```bash
    python pyexamples/pyqt5/ribbon_demo.py
    ```

=== "PyQt6"

    ```bash
    python pyexamples/pyqt6/ribbon_demo.py
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

=== "PyQt5"

    ```bash
    copy <Qt路径>\bin\Qt5Widgets.dll <Python路径>\Lib\site-packages\
    copy <Qt路径>\bin\Qt5Gui.dll <Python路径>\Lib\site-packages\
    copy <Qt路径>\bin\Qt5Core.dll <Python路径>\Lib\site-packages\
    ```

=== "PyQt6"

    ```bash
    copy <Qt路径>\bin\Qt6Widgets.dll <Python路径>\Lib\site-packages\
    copy <Qt路径>\bin\Qt6Gui.dll <Python路径>\Lib\site-packages\
    copy <Qt路径>\bin\Qt6Core.dll <Python路径>\Lib\site-packages\
    ```

### 问题 4：版本不匹配

**错误信息**：ImportError: Module use of python3x.dll conflicts

**解决方案**：确保 PyQt（5 或 6）、sip 和 Python 版本兼容。

### 问题 5：PyQt5/PyQt6 共存冲突

**错误信息**：构建 PyQt6 绑定时使用了 PyQt5 的头文件，或运行时导入错误的 PyQt 版本

**解决方案**：

- 确保构建 PyQt6 绑定时使用的是 Qt6 的 qmake 路径
- 使用 `--pyqt6` 标志让构建脚本自动切换配置文件
- 在同一 Python 环境中不建议同时安装 PyQt5 和 PyQt6，建议使用虚拟环境隔离

## 下一步

请参阅 [使用 Python 绑定](./use-python-bindings.md) 了解如何使用 SARibbon。
