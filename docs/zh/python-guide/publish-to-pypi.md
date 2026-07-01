# PyPI 发布指南

本文档说明如何将 SARibbon Python 绑定发布到 PyPI，让用户通过 `pip install` 安装。

## 包结构

SARibbon 有三套 Python 绑定，发布为三个独立的 PyPI 包：

| PyPI 包名 | 绑定 | 构建后端 | 依赖 | 安装命令 |
|-----------|------|----------|------|----------|
| `PyQtSARibbon` | PyQt5 (SIP) | `sipbuild.api` | `PyQt5>=5.15` | `pip install PyQtSARibbon` |
| `PyQt6SARibbon` | PyQt6 (SIP) | `sipbuild.api` | `PyQt6>=6.2` | `pip install PyQt6SARibbon` |
| `PySideSARibbon` | PySide6 (Shiboken6) | `scikit-build-core` | `PySide6>=6.5` | `pip install PySideSARibbon` |

### 项目目录结构

```
SARibbon/
├── pyproject.toml          # PyQt5: SIP 构建配置 (包名 PyQtSARibbon)
├── project.py              # PyQt5: PyQtBuilder 项目类
├── sip/                    # PyQt5: SIP 绑定描述文件
│   ├── SARibbon.sip        #   模块入口 (name=PyQtSARibbon.saribbon)
│   └── *.sip               #   各类的 SIP 描述
├── pyqt6/                  # PyQt6: 独立 SIP 项目目录
│   ├── pyproject.toml      #   SIP 构建配置 (包名 PyQt6SARibbon)
│   └── sip/                #   SIP 绑定描述文件 (name=PyQt6SARibbon.saribbon)
├── pyside6/                # PySide6: Shiboken6 + CMake 项目目录
│   ├── pyproject.toml      #   scikit-build-core 构建配置 (包名 PySideSARibbon)
│   ├── CMakeLists.txt      #   CMake 构建脚本
│   ├── typesystem_saribbon.xml  # Shiboken 类型系统
│   ├── saribbon_python_glue.h   # C++ 胶水代码
│   └── PySideSARibbon/
│       └── __init__.py     #   Python 包入口
└── .github/workflows/
    └── publish-python-bindings.yml  # CI 自动构建+发布
```

## 构建后端说明

### PyQt5 / PyQt6 (SIP)

使用 `sipbuild.api` 作为 PEP 517 构建后端。`sip-build` / `pip wheel` 会自动：
1. 解析 `.sip` 文件生成 C++ 绑定代码
2. 编译 C++ 源码和绑定代码为扩展模块
3. 打包为标准 wheel

```bash
# PyQt5 (在项目根目录)
pip wheel . --no-build-isolation

# PyQt6 (在 pyqt6/ 目录)
pip wheel pyqt6/ --no-build-isolation
```

### PySide6 (scikit-build-core)

使用 `scikit-build-core` 作为 PEP 517 构建后端，桥接 `pyproject.toml` 和 CMake。`pip wheel` 会自动：
1. 调用 CMake 配置和构建
2. 运行 shiboken6 生成绑定代码
3. 编译为扩展模块
4. 通过 CMake install 目标打包为 wheel

```bash
# PySide6 (在 pyside6/ 目录)
pip wheel pyside6/
```

> **注意**：PySide6 构建需要 Qt6 C++ 开发包（头文件），PySide6 pip 包不包含。
> 构建环境需额外安装 Qt6 开发包，但最终用户只需 `pip install PySideSARibbon` 即可使用。

## CI/CD 自动化

GitHub Actions workflow: `.github/workflows/publish-python-bindings.yml`

### 触发条件
- GitHub Release 发布时自动触发
- 支持手动触发（`workflow_dispatch`）

### 构建矩阵

| 平台 | Python 版本 | Qt 版本 |
|------|-------------|---------|
| Windows x64 | 3.10, 3.11, 3.12 | Qt 6.8.3 (MSVC 2022) / Qt 5.15.2 (MSVC 2019) |
| Linux x64 | 3.10, 3.11, 3.12 | Qt 6 (apt) / Qt 5 (apt) |
| macOS | 3.10, 3.11, 3.12 | Qt 6 (brew) / Qt 5 (brew) |

### 发布方式

使用 PyPI Trusted Publishing (OIDC) — 无需 API Token：
1. 在 [pypi.org](https://pypi.org/manage/account/publishing/) 配置 Trusted Publisher
2. GitHub Actions 自动构建所有平台的 wheel
3. 使用 `pypa/gh-action-pypi-publish` 上传到 PyPI

## 版本同步

版本号在以下位置维护，需保持一致：

| 文件 | 字段 |
|------|------|
| `pyproject.toml` (PyQt5) | `version = "x.y.z"` |
| `pyqt6/pyproject.toml` (PyQt6) | `version = "x.y.z"` |
| `pyside6/pyproject.toml` (PySide6) | `version = "x.y.z"` |
| `pyside6/CMakeLists.txt` | `project(PySideSARibbon VERSION x.y.z)` |
| `pyside6/PySideSARibbon/__init__.py` | `__version__ = "x.y.z"` |

## 首次发布步骤

1. **注册 PyPI 包名**：在 [pypi.org](https://pypi.org) 注册以下包名（首次上传时自动注册）：
   - `PyQtSARibbon`
   - `PyQt6SARibbon`
   - `PySideSARibbon`

2. **配置 Trusted Publishing**：
   - 登录 PyPI → Account settings → Publishing → Add a publisher
   - 选择 GitHub，填入仓库 `czyt1988/SARibbon`
   - Workflow: `publish-python-bindings.yml`
   - Environment: `pypi`

3. **创建 Release**：
   ```bash
   git tag v2.8.0
   git push origin v2.8.0
   # 在 GitHub 上创建 Release
   ```

4. CI 自动构建并发布到 PyPI

5. **验证安装**：
   ```bash
   pip install PyQtSARibbon      # PyQt5
   pip install PyQt6SARibbon     # PyQt6
   pip install PySideSARibbon    # PySide6
   ```

## 参考资料

- [scikit-build-core 文档](https://scikit-build-core.readthedocs.io/)
- [PyPI Trusted Publishing](https://docs.pypi.org/trusted-publishers/)
- [SIP / PyQt-builder](https://www.riverbankcomputing.com/static/Docs/sip/)
