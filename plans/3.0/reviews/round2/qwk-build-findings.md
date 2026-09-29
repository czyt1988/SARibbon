# Round2 评审记录：QWindowKit 构建体系深读（视角=参考项目深度学习）

> 评审对象：`plans/3.0/01-infra-restructure.md`（S4/S5/S6/S7/S10/S11）与 `plans/3.0/03-build-ecosystem.md`（S5/S6）。
> 本轮独立于 round1：所有结论基于本轮亲自读取的文件，引用格式如下。
>
> **证据源与时效声明**：
> - `QWK:` = 本机副本 `F:\src\3rdparty\qwindowkit`（git 仓库，HEAD `1fb3ec7` "Update README"，`project(QWindowKit VERSION 1.0.1.0)`，即 1.0.1 时代快照；**该快照不含 `.github/`，`git ls-files` 实证**）。
> - `qmsetup:` = QWK `.gitmodules:2-4` pin 的 `stdware/qmsetup@99ca80fd63e34f4bb54dcc48db09a66e86f5517d`（本地 submodule 为空目录，实现经 GitHub API 抓取该 commit 的 `cmake/QMSetupAPI.cmake`、`cmake/modules/Preprocess.cmake`、`cmake/modules/Filesystem.cmake` 全文核对）。
> - `QWK-upstream:` = stdware/qwindowkit **main 分支**（比本地快照新，经 GitHub API 抓取 `.github/workflows/ci.yml`，blob sha `f939185`；引用处均已标注）。
> - `SR:` = SARibbon 仓库（分支 v3，基线 2.9.5）。

## 一、QWK 构建体系深读笔记

### 1.1 qwk_add_library 宏（QWK:src/CMakeLists.txt:38-126）

**签名**：`macro(qwk_add_library _target)`，options `AUTOGEN / NO_SYNC_INCLUDE / NO_WIN_RC`，单值 `SYNC_INCLUDE_PREFIX / PREFIX`，其余透传 `qm_configure_target`（:38-42）。实际调用例：`qwk_add_library(QWKCore AUTOGEN SOURCES ... LINKS ... LINKS_PRIVATE ... QT_LINKS Core Gui QT_INCLUDE_PRIVATE Core Gui INCLUDE_PRIVATE kernel contexts shared PREFIX QWK_CORE SYNC_INCLUDE_OPTIONS ...)`（QWK:src/core/CMakeLists.txt:88-97）。

逐项实证：

| 机制 | QWK/qmsetup 实证 | 与 01-S5.3 骨架核对结论 |
|---|---|---|
| STATIC/SHARED 判定 | `if(QWINDOWKIT_BUILD_STATIC) set(_type STATIC) else() SHARED`（:50-54），无 BUILD_SHARED_LIBS 参与 | 骨架同款（`SARIBBON_BUILD_STATIC_LIBS`），一致 |
| AUTOMOC/AUTORCC/AUTOUIC | `AUTOGEN` 关键字设**目录级** `CMAKE_AUTOMOC/AUTOUIC/AUTORCC ON`（:44-48） | 骨架用 target 属性，作用域更窄，等价偏优，维持 |
| 导出宏 define | `qm_export_defines(${_target} PREFIX ${FUNC_PREFIX})`（:73）。实现在 qmsetup:QMSetupAPI.cmake:200-230：PREFIX 缺省=target 名 TOUPPER（:210-214）；**STATIC 库时 `target_compile_definitions(PUBLIC <PREFIX>_STATIC)`（:222-226）；`<PREFIX>_LIBRARY` 恒 `PRIVATE` 定义、静态构建也定义（:228）**——即 CMake 层不做互斥，优先级由头文件"先 `ifdef STATIC`"裁决 | 骨架的 if/else 互斥发宏与 QWK **行为等价**（三段式头 STATIC 优先），且与 SR 2.9.5 现状一致（SR:src/SARibbonBar/CMakeLists.txt:186-199 静态只发 NO_EXPORT、动态只发 MAKE_LIB）。互斥+头优先级=双保险，维持骨架；已在 01-S5.2/S5.3 补注 |
| CXX 标准 | **不在宏内**，各模块 `set_target_properties(... CXX_STANDARD 17 CXX_STANDARD_REQUIRED TRUE)`（QWK:src/core/CMakeLists.txt:99-102，widgets/quick 同构） | 骨架集中 `target_compile_features(PUBLIC cxx_std_17)`，少一处重复且 PUBLIC 传播，有意优于 QWK，维持（01-S5.1 已注） |
| include 传播 | 源目录 `.` **PRIVATE**（:80）；构建生成目录 `${BINARY}/etc/include` PRIVATE（:79，放 qwkconfig.h）；同步头目录 PUBLIC `$<BUILD_INTERFACE:.../include>`（:119-124）；安装树 PUBLIC `$<INSTALL_INTERFACE:include/QWindowKit>`（:108-110，仅 INSTALL 时） | 与 01-S5.5 决策（源目录 PUBLIC 保平铺 include + 同步目录 PUBLIC）差异是**有意的过渡兼容**，已在 01-S5.5 补精确行号与伞目录布局差异说明 |
| install(TARGETS/EXPORT) | `install(TARGETS ... EXPORT QWindowKitTargets RUNTIME/LIBRARY/ARCHIVE DESTINATION ... OPTIONAL)` 三个目的地**均带 OPTIONAL**，且整段在 `QWINDOWKIT_INSTALL` 守卫内（:100-106）；`install(EXPORT ... NAMESPACE QWindowKit::)` 全项目唯一一处在 src 层（:208-212） | 骨架原缺 OPTIONAL，**已补**（01-S5.3）；单 export set + src 层唯一 install(EXPORT) 与骨架 S6.7 同构，已补证据 |
| POSTFIX/输出目录 | 宏内不处理；顶层 `if(NOT DEFINED CMAKE_DEBUG_POSTFIX) set(... "d")`（QWK:CMakeLists.txt:28-30）+ `qm_init_directories()` 统一 `out-<arch>-<config>/{bin,lib}`（qmsetup:Filesystem.cmake:12-24） | SARibbon 用 CACHE STRING postfix（SR:CMakeLists.txt:105）+ `${CMAKE_BINARY_DIR}/{bin,lib}`（SR:src/SARibbonBar/CMakeLists.txt:172-183），等价或更适合现状，不抄（01-S4.2 已注） |
| Qt5/Qt6 分支 | 宏内无；由 qmsetup `qm_find_qt` 惰性探测：`find_package(QT NAMES ${QMSETUP_FIND_QT_ORDER} ...)`（缺省 `Qt6 Qt5`，qmsetup:QMSetupAPI.cmake:37-38,106-116），链接 `Qt${QT_VERSION_MAJOR}::<mod>`（:123-128）；QT_LINKS→PUBLIC、QT_INCLUDE_PRIVATE→Qt 私有头目录（:186-194） | SARibbon 顶层两段式探测必须保留（frameless Qt 门槛依赖），骨架 `Qt${QT_VERSION_MAJOR}::` 拼接与 qmsetup 同式；QT_LINKS PUBLIC 语义一致（不抄惰性探测，01-S4.2 已注） |
| Windows 版本资源 | `WIN32 AND NOT NO_WIN_RC AND SHARED` 时 `qm_add_win_rc`（:58-64） | 骨架条件同构（用 SR 自己的 `create_win32_resource_version`），一致 |

### 1.2 导出宏头（QWK:src/core/qwkglobal.h:12-22；widgets/quick 同构）

三段式实证（qwkglobal.h:12-22、qwkwidgetsglobal.h:9-19、qwkquickglobal.h:9-19 三处同文）：

```cpp
#ifndef QWK_CORE_EXPORT
#  ifdef QWK_CORE_STATIC          // 先判 STATIC → 空宏
#    define QWK_CORE_EXPORT
#  else
#    ifdef QWK_CORE_LIBRARY       // 再判 LIBRARY → Q_DECL_EXPORT
#      define QWK_CORE_EXPORT Q_DECL_EXPORT
#    else
#      define QWK_CORE_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif
```

宏名对应关系：CMake `PREFIX QWK_CORE`（QWK:src/core/CMakeLists.txt:95）→ `qm_export_defines` 拼出 `QWK_CORE_STATIC`/`QWK_CORE_LIBRARY` → 头文件消费为 `QWK_CORE_EXPORT`。整段被 `#ifndef QWK_CORE_EXPORT` 外层守卫（允许使用者预定义覆盖）。

**对照 01-S6.9 三个全局头代码块**：
- 模板逐 token 同构，无需修改；`SA_RIBBON_CORE_STATIC` 命名与 QWK `<PREFIX>_STATIC` 约定对齐 ✓。
- **STATIC 优先于 LIBRARY 的判断顺序必须保留**——这是 qmsetup"CMake 层不互斥"设计的安全网（1.1 表）；SARibbon 头文件块顺序正确。
- **旧宏映射方向核实（SR 侧证据）**：SR:src/SARibbonBar/SARibbonGlobal.h:9-18 实证 `SA_RIBBON_BAR_NO_EXPORT` 定义时 `SA_RIBBON_EXPORT` 为空宏（静态语义）、未定义时按 `SA_RIBBON_BAR_MAKE_LIB` 分 EXPORT/IMPORT；SR:src/SARibbonBar/colorWidgets/SAColorWidgetsGlobal.h:97-111 同构（NO_DLL 外层优先）。**01-S6.9 代码块 C 的 `NO_EXPORT→WIDGETS_STATIC`、`MAKE_LIB→WIDGETS_LIBRARY` 映射方向没有写反**，已在 01-S5.2 补核实记录。

### 1.3 QWindowKitConfig.cmake.in + 版本文件（QWK:src/QWindowKitConfig.cmake.in 全文 8 行；src/CMakeLists.txt:181-215）

```cmake
@PACKAGE_INIT@
include(CMakeFindDependencyMacro)
find_dependency(QT NAMES Qt6 Qt5 COMPONENTS Core Gui REQUIRED)
find_dependency(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui REQUIRED)
include("${CMAKE_CURRENT_LIST_DIR}/QWindowKitTargets.cmake")
```

**关键实证：QWK 1.0.1 没有组件化 find_package。**
- 三模块共用**单一 export set** `QWindowKitTargets`，`install(EXPORT ... FILE QWindowKitTargets.cmake NAMESPACE QWindowKit:: DESTINATION lib/cmake/QWindowKit)`（src/CMakeLists.txt:208-212）；命名空间 target `QWindowKit::Core/Widgets/Quick` 来自 EXPORT_NAME（:83-88）+ install NAMESPACE，**不存在** `find_package(QWindowKit COMPONENTS Widgets)` 的组件逻辑。
- `configure_package_config_file(... NO_CHECK_REQUIRED_COMPONENTS_MACRO)`（:193-198）——显式禁用组件宏，反证其无组件支持。
- 版本文件 `write_basic_package_version_file(... VERSION ${PROJECT_VERSION} COMPATIBILITY AnyNewerVersion)`（:186-190），VERSION 为四段 1.0.1.0。
- 组件间依赖（Widgets→Core、Quick→Core）靠 target PUBLIC 链接自动进导出文件（QWK:src/widgets/CMakeLists.txt:23 `LINKS QWKCore`；qmsetup:QMSetupAPI.cmake:186 `target_link_libraries(PUBLIC ${FUNC_LINKS})`）。

**对照 01-S11**：SARibbon 的组件化 Config（COMPONENTS→target 存在性 + `check_required_components`）是**超出 QWK 的自主设计**，无现成范本可抄。已做修订：
1. `find_dependency` 补 `REQUIRED`（QWK Config.cmake.in:5-6 同款；缺失时错误延迟难排查）。
2. 加警告：骨架用了 `check_required_components(SARibbon)`，则 `configure_package_config_file` **不得**传 `NO_CHECK_REQUIRED_COMPONENTS_MACRO`（照抄 QWK 调用方式会踩坑）。
3. 补"单 export set 自动传播组件间依赖"的机制说明（S6.7/S11 附注）。
4. **不抄**两段式 `find_dependency(QT NAMES Qt6 Qt5)`：它把 Qt 大版本裁决权交给消费端环境，若消费端先 `find_package(Qt5)`，将与 Targets 文件里生产端固定的 `Qt6::Core` 混版；SARibbon baked-in `Qt@QT_VERSION_MAJOR@` 与导出文件确定一致，且 SR 现状同式（SR:src/SARibbonBar/SARibbonBarConfig.cmake.in:12）、本地安装目录本就按 Qt 版本隔离（`bin_qtX_编译器_x架构/`）。
5. 版本兼容策略：QWK=AnyNewerVersion，SARibbon 维持 SameMajorVersion（SR:src/SARibbonBar/CMakeLists.txt:325-329 现状，更严格），不抄。
6. 旧包名 `SARibbonBarConfig` 兼容薄壳：QWK 无旧包名先例（不构成参照也不构成反证），SARibbon 自设 INTERFACE IMPORTED 转发方案自洽，维持。

### 1.4 顶层与 src 层 CMake 组织（QWK:CMakeLists.txt 全文 83 行）

- 选项实名（:8-17）：`QWINDOWKIT_BUILD_STATIC/WIDGETS/QUICK/EXAMPLES/DOCUMENTATIONS/INSTALL` + `QWINDOWKIT_ENABLE_QT_WINDOW_CONTEXT/WINDOWS_SYSTEM_BORDERS/STYLE_AGENT`。命名模式 `<项目>_BUILD_<对象>` / `<项目>_ENABLE_<特性>`——SARibbon 新选项名与之同构（01-S4.2 已注先例）。
- **`QWINDOWKIT_INSTALL` 守卫一切安装物**：GNUInstallDirs/CMakePackageConfigHelpers 仅 INSTALL 时 include（:36-39）；qwkconfig.h 安装（src/:29-33）、install(TARGETS)+INSTALL_INTERFACE（src/:100-115）、Config/Targets 生成安装（src/:181-215）全在守卫内。→ 设计级建议 D1。
- MSVC：`/manifest:no` 三链接器旗标 + `/utf-8`（:22-26）、`if(NOT DEFINED CMAKE_DEBUG_POSTFIX) set("d")`（:28-30）；MINGW：清空 `CMAKE_STATIC/SHARED_LIBRARY_PREFIX`（:31-33）。→ 不抄清单见 01-S4.2（/utf-8 违反 SR 行为零变化；MinGW 去前缀属产物名行为变化）。
- qmsetup 依赖：`find_package(qmsetup QUIET)` 失败则**就地构建 submodule 回退**（:50-72，`qm_install_package`）+ `qm_import(Filesystem)` + `qm_init_directories()`（:74-75）。→ SARibbon 不抄（外部工具链依赖，01-S5.1 决策不变）。
- Qt 探测：顶层**零** find_package(Qt)，全靠 qmsetup 惰性探测（1.1 表）。→ SARibbon 顶层探测必须保留（frameless 门槛 + MIN_QT_VERSION 检查），不抄。
- examples 开关 `QWINDOWKIT_BUILD_EXAMPLES`（:82-84），**无顶层 `CMAKE_SOURCE_DIR` 守卫、无 tests**（快照无 tests 目录；QWK-upstream:ci.yml 出现 `-DQWINDOWKIT_BUILD_TESTS=ON`，证明上游新版有测试体系）。→ SARibbon S4 第 7 条守卫是更严格的自有做法，保留。
- 文档：`QWINDOWKIT_BUILD_DOCUMENTATIONS` + `qm_setup_doxygen`（src/:144-176），模块用 `QWINDOWKIT_ENABLED_TARGETS/SUBDIRECTORIES` PARENT_SCOPE 累积（src/core/:104-105）。→ 归 04 文档计划参考，本轮不动。
- windeployqt/部署辅助：**无**（快照与 src 层均未见；qmsetup 另有 Deploy.cmake 模块但 QWK 未 import 它）。

### 1.5 quick 模块构建（QWK:src/quick/CMakeLists.txt 全文 38 行）

- QWKQuick 是**纯 C++ SHARED/STATIC 库**：`qwk_add_library(QWKQuick AUTOGEN SOURCES ... LINKS QWKCore QT_LINKS Core Gui Quick QT_INCLUDE_PRIVATE Core Gui Quick PREFIX QWK_QUICK)`（:22-29）+ 模块级 CXX_STANDARD 17（:31-34）。
- **Qt5/Qt6 无任何分支**：没有 qt_add_qml_module、没有 qrc、没有 qmldir/plugin、没有 qml 目录安装规则；Qt5 下同样只是链 `Qt5::Quick`（qmsetup 惰性 find 统一处理）。
- QML 类型注册 = **命令式导出函数**：`QWK_QUICK_EXPORT void registerTypes(QQmlEngine*)`（qwkquickglobal.h:27），实现 `qmlRegisterType<QuickWindowAgent>("QWindowKit", 1, 0, "WindowAgent") + qmlRegisterModule("QWindowKit", 1, 0)` 带 `static bool once` 守卫（qwkquickglobal.cpp:12-25）；应用 `main()` 在 `engine.load()` 前手动调用（examples/qml/main.cpp:26）。
- QML 文件在**应用侧 qrc**（examples/qml/qml.qrc：main.qml + QWKButton.qml），不进库、不安装。
- 链接细节：库只显式 `QT_LINKS Core Gui Quick`（Qml 由 Quick 传递），QML **示例**显式 `QT_LINKS Core Gui Qml Quick`（examples/qml/CMakeLists.txt:7）。
- **落点**：01-S6-5（qml 空骨架）已补上述实证注记；03-S6-3 已补"QML 安装布局预警"（当前清单无 qml 项是正确状态；04 若走 qt_add_qml_module 则需增补）。
- **供 04 整合的重大事实（本轮不改 04）**：v2 §5.3 双轨中 Qt5 命令式轨有 QWK 完整同款先例；**Qt6 `qt_add_qml_module` 轨在 QWK 1.0.1 无任何先例**（其 qmldir/plugin 安装布局、URI/IMPORT_VERSION、Qt6.2+ qmlcachegen 行为均需 04 自行设计并自行验证安装树）。

### 1.6 CI（本地快照无 .github；证据=QWK-upstream:ci.yml，blob f939185，经 GitHub API 抓取）

**时效警示**：QWK-upstream main 的 CI 比本地快照 1fb3ec7 新（引用了快照中不存在的 `QWINDOWKIT_BUILD_TESTS`、`.github/actions/setup`、consumer tests），以下按"上游新版做法"记录：
- 组织：**单 ci.yml**，`strategy.fail-fast: false`，matrix 主键 `platform: [linux-gcc, linux-clang, macos, windows-msvc, windows-clang-cl]` + `include:` 条目携带 `os/cc/cxx/qt_arch/qt_version`（多编译器轴：linux gcc+clang、windows msvc+clang-cl）；MinGW **独立 job**（msys2/setup-msys2 + `mingw-w64-x86_64-{gcc,cmake,ninja,make,qt6-base,qt6-declarative}`，理由注释：aqt 的 MinGW Qt 与编译器不配套会链接错）。
- Qt 安装：`jurplel/install-qt-action@v4` + `cache: true` + `arch: ${{ matrix.qt_arch }}`；checkout 带 `submodules: recursive`（qmsetup 还有嵌套 submodule stdcorelib，一层不够）。
- 构建：统一 `cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug`，全选项 ON（QUICK/EXAMPLES/TESTS）。
- 测试：`ctest --test-dir build --output-on-failure --no-tests=error`，注释明言 "ctest exits 0 when it finds nothing to run, and every test in this suite registers itself conditionally"；另有专门步骤 "Check that the consumer tests registered"（`ctest -N -R` 逐项核对 `buildsystems.cmake/qmake/msbuild` 已注册，Windows 才要求 msbuild 项）。
- 缓存策略：仅 Qt action 自带 cache；**无 ccache/sccache**（SARibbon 规划中"抄 QWK 的 ccache"无实证依据，不得写）。
- **落点**：01-S10 新增第 7 条（6 workflow ctest 加 `--no-tests=error`，防选项改名后测试静默不注册假绿）；03-S5 新增第 4 条（复核纪律）与第 5 条（可选：test-find-package 注册为 ctest consumer 测试）。SR 现状已有 `fail-fast: false` 与 `jurplel/install-qt-action@v4 + cache:'true'`（SR:.github/workflows/cmake-linux-qt6.8.yml:17-37），无需重复添加；SR ctest 现无 `--no-tests=error`（同文件 :52-55）。
- 不抄：单 ci.yml 重组（SR 6-workflow 结构既定）、多编译器轴、MinGW job、Ninja+Debug 统一——记 §三 D3。

### 1.7 examples/tests 的 target 组织

- `qwk_add_example` 宏（examples/CMakeLists.txt:3-13）：目录级 AUTOGEN 三连 + `add_executable` + `qm_configure_target(${_target} ${ARGN})` + win rc/manifest + mac bundle，一次封装。
- **树内消费链裸 target 名**：`LINKS QWKWidgets WidgetFrame` / `LINKS QWKQuick`（examples/mainwindow/CMakeLists.txt:5-9、examples/qml/CMakeLists.txt:5-9）→ `target_link_libraries PUBLIC`（qmsetup:QMSetupAPI.cmake:186）；共享代码编成 STATIC 辅助库 `WidgetFrame`（examples/shared/widgetframe/CMakeLists.txt:9-14，`target_include_directories PUBLIC . ..`）。命名空间别名 `QWindowKit::*` 树内定义（src/CMakeLists.txt:90）但 examples 不用它——别名主要服务 `install(EXPORT ... NAMESPACE)`（:208-212）与外部消费者。
- 快照**无 tests**（1.6 已注：上游新版有）。
- **对照 01-S7**：方向一致——tests 改链裸 `SARibbonWidgets`（S7-2）、examples 过渡期保留 `SARibbonBar::SARibbonBar` 别名（S6.6 既定决策，不推翻）、03 切 `SARibbon::Widgets`（安装态口径）。已在 S7-4 补 QWK 对照注记。`qwk_add_example` 式统一 example 宏可作为 03/04 整理 examples 时的可选模式（本轮未写入正文，避免扩大 01-S7 范围）。

## 二、对 01/03 的修改清单

| # | 位置 | 修改 | 依据 |
|---|---|---|---|
| 1 | 01-S5.1 第2条 | "qmsetup 实现不可读、规格系反推"改为"已从上游 pin commit 核实"，注明抓取来源与引用格式；宏行号 38-124→38-126；补 corecmd 外部工具依赖作为"不依赖 qmsetup"的实证理由 | qmsetup:Preprocess.cmake:49-51；QWK:.gitmodules:2-4 |
| 2 | 01-S5.1 第3条 | 关键机制清单全面升级为实证版：AUTOGEN 目录级、win_rc 条件、qm_export_defines 非互斥细节、CXX 标准在模块侧、install OPTIONAL、include 三件套精确行号、qm_sync_include=corecmd 转发头机制+EXCLUDE 用例 | QWK:src/CMakeLists.txt:44-126；qmsetup:QMSetupAPI.cmake:164-230；QWK:src/core/CMakeLists.txt:85,88-102 |
| 3 | 01-S5.2 | 新增"round2 实证补注"：互斥性结论（QWK 不互斥、头文件裁决；骨架互斥等价维持）+ 旧宏映射方向核实（未写反） | qmsetup:QMSetupAPI.cmake:222-228；QWK:src/core/qwkglobal.h:12-22；SR:src/SARibbonBar/SARibbonGlobal.h:9-18；SR:colorWidgets/SAColorWidgetsGlobal.h:97-111 |
| 4 | 01-S5.3 | install bullet 与骨架代码补 `OPTIONAL`×3；骨架导出宏块补 qm_export_defines 行为注释；install(EXPORT) 唯一性补 QWK 行号 | QWK:src/CMakeLists.txt:100-106,208-212 |
| 5 | 01-S5.4 | "与 qm_sync_include 语义一致"改为"语义层一致、机制层不同且有意为之"（转发头+corecmd vs file(COPY)，不引入外部工具的代价说明） | qmsetup:Preprocess.cmake:20-22,27-51,92-104,107-140 |
| 6 | 01-S5.5 | QWK 模式补精确行号；新增伞目录（include/QWindowKit/<mod>）vs SARibbon 无伞（include/<mod>）布局差异说明（消费端写法相同，维持无伞） | QWK:src/CMakeLists.txt:80,108-110,122-124 |
| 7 | 01-S4.2（新增附注块） | QWK 顶层组织逐项对照：抄 2 项（选项命名先例、GNUInstallDirs 顶层 include）、postfix 守卫等价说明、不抄 6 项及理由（INSTALL 选项→D1、qmsetup、/utf-8+/manifest:no、MinGW 前缀、惰性 Qt 探测、统一输出目录）、顶层守卫无 QWK 先例说明 | QWK:CMakeLists.txt:8-39,50-75,82-84；qmsetup:QMSetupAPI.cmake:37-38,106-116；qmsetup:Filesystem.cmake:12-24；SR:CMakeLists.txt:105 |
| 8 | 01-S6-5 | qml 空骨架补 QWK quick 实证注记：纯 C++ 库无 qml 安装物、registerTypes 命令式注册、qml 文件在应用 qrc、对 v2 §5.3 双轨的含义（Qt5 轨有先例/Qt6 轨无先例）、QT_LINKS 显式四件建议 | QWK:src/quick/CMakeLists.txt:22-34；qwkquickglobal.h:27；qwkquickglobal.cpp:12-25；examples/qml/main.cpp:26；examples/qml/CMakeLists.txt:7 |
| 9 | 01-S6.7 | src 层统一导出块后补 QWK 同构实证 + 组件依赖经 PUBLIC 链接自动进导出文件的机制说明 | QWK:src/CMakeLists.txt:131-139,181-212；QWK:src/widgets/CMakeLists.txt:23；qmsetup:QMSetupAPI.cmake:186 |
| 10 | 01-S7-4 | examples 决策后补 QWK 对照：树内消费链裸名、别名服务导出与外部消费（支持既有方向，不改决策） | QWK:examples/mainwindow/CMakeLists.txt:5-9；examples/qml/CMakeLists.txt:5-9；examples/shared/widgetframe/CMakeLists.txt:9-14；src/CMakeLists.txt:90,208-212 |
| 11 | 01-S10-7（新增条目） | 6 workflow ctest 统一加 `--no-tests=error`（防选项改名后测试静默不注册假绿）；注明证据来自上游 main（非本地快照）；其余 QWK CI 做法指向 findings §三；已核实 SR 已有 fail-fast:false 与 jurplel cache，不重复添加 | QWK-upstream:ci.yml Test 步骤；SR:.github/workflows/cmake-linux-qt6.8.yml:17-37,52-55 |
| 12 | 01-S11 | Config 骨架 find_dependency 补 `REQUIRED`×2 + 注释；新增"QWK 对照附注"5 条（无组件化范本、NO_CHECK_REQUIRED_COMPONENTS_MACRO 陷阱、单 export set 依赖传播、AnyNewerVersion 差异不抄、NAMES 两段式不抄理由、薄壳无先例） | QWK:src/QWindowKitConfig.cmake.in:1-8；src/CMakeLists.txt:186-212；SR:src/SARibbonBar/SARibbonBarConfig.cmake.in:12；SR:src/SARibbonBar/CMakeLists.txt:325-329 |
| 13 | 03-S5-4/5（新增条目） | ctest `--no-tests=error` 复核纪律（承接 01-S10-7）；可选增强：test-find-package 注册为 ctest consumer 测试（QWK 上游 buildsystems.* 模式） | QWK-upstream:ci.yml（Test 步骤 + "Check that the consumer tests registered" 步骤） |
| 14 | 03-S6-3 | 安装树复核清单补"QML 安装布局预警"bullet（当前无 qml 项=正确状态；04 定型 qt_add_qml_module 后必须增补）+ test-find-package 条指向 S5-5 可选升级 | QWK:src/quick/CMakeLists.txt 全文；src/CMakeLists.txt:100-115 |

## 三、设计级建议（供整合者裁决）

- **D1（建议采纳，需上达 v2 §6/README 选项表）**：新增 `SARIBBON_INSTALL` 选项（默认 ON），把所有 `install()`/`install(EXPORT)`/Config 生成收进守卫——QWK 用 `QWINDOWKIT_INSTALL` 实现（QWK:CMakeLists.txt:13,36-39；src/:29,100,181），解决 `add_subdirectory` 嵌入场景下 SARibbon 的 install/export 规则污染宿主工程的问题。01-S4 第 7 条的顶层守卫只覆盖 examples/tests，覆盖不到 install。改动小（一个 option + 若干 if 包裹），但属新公共选项，应由整合者决定是否进 01-S4 正文与 v2 选项表。
- **D2（建议采纳为可选项，已按"可选增强"写入 03-S5-5）**：把 `tools/test-find-package` 升级为 ctest 注册的 consumer 测试（QWK-upstream:ci.yml `buildsystems.*` 模式 + 注册核对步骤）。是否升格为验收门必选项由整合者定；01-S11.4 的手动冒烟基线未被推翻。
- **D3（建议仅记 backlog，不进 3.0）**：CI 多编译器轴（linux clang、windows clang-cl）与 MinGW/msys2 独立 job（QWK-upstream:ci.yml matrix include 组织）。价值真实（QWK 注释明言 win32 context 在 MinGW 下才暴露 SDK 头差异），但 SARibbon 3.0 的 CI 改造范围已由 01-S10/03-S5 锁定，扩轴成本与 runner 时间显著，建议 3.x 期间单独评估。
- **D4（建议不做）**：MinGW 去 `lib` 前缀（QWK:CMakeLists.txt:31-33）。属产物名行为变化，违反 01"行为零变化"纪律；若未来要对齐，单独决策。
- **D5（供 04 整合者）**：QWK 实证支持 v2 §5.3 的 Qt5 命令式注册轨（同款先例完整），但 **Qt6 `qt_add_qml_module` 轨无 QWK 先例**，其 qmldir/plugin 安装布局、URI/IMPORT_VERSION、与 `SARIBBON_INSTALL`/伞目录决策的交互需 04 自行设计；建议 04 风险表加一条"QML 模块安装布局无参考实现，需在 Qt6.5/6.8 两版实测"。（本轮未改 04。）
- **D6（记录，无需动作）**：版本兼容策略差异——QWK=AnyNewerVersion（src/CMakeLists.txt:189），SARibbon=SameMajorVersion（维持，更严格）。
- **D7（记录，无需动作）**：安装 include 布局差异——QWK 有伞目录 `include/QWindowKit/<模块>/`，SARibbon 计划为 `include/<模块>/`（与 2.9.5 `include/SARibbonBar/` 同层级，消费端 include 写法两者相同）。维持无伞目录。
- **D8（建议记录）**：不得在任何文档中写"参考 QWK 的 ccache 配置"——QWK 快照与上游 ci.yml 均无 ccache/sccache（仅 jurplel action 自带 Qt 下载缓存）。

## 四、未解决 / 待核实

1. **corecmd incsync 的产物形态细节**（转发头的确切内容、`-s`/`-d` 标志行为）：实现在 qmsetup 的嵌套 submodule `stdcorelib` 中，本轮未读（不影响结论——SARibbon 决策为不用该工具，`file(COPY)` 方案自足）。替代核实路径：`github.com/stdware/stdcorelib` 的 `src/corecmd`。
2. **QWK-upstream ci.yml 与本地快照的版本差**：上游 main 已含 tests 体系、`.github/actions/setup` 复合 action、consumer tests，均不在 1fb3ec7 内。本文所有 `QWK-upstream:` 引用仅代表上游新版做法，不代表 SARibbon submodule pin 的 fork（czyt1988/qwindowkit f93657f）状态；若后续 SARibbon 升级 QWK pin，应重新核对。
3. **`install(TARGETS ... OPTIONAL)` 的确切语义**：QWK 三个目的地全带 OPTIONAL 且长期可用（QWK:src/CMakeLists.txt:103-105）；各 CMake 版本对"产物类型缺席"的容忍度本轮未实测（评审纪律：不构建）。S5.3 落地时在 static 与 shared 各跑一次 `cmake --install` 即可验证，预期无差异。
4. **qm_add_win_rc/qm_add_win_manifest/qm_add_mac_bundle 实现**（qmsetup:QMSetupAPI.cmake:236-470）未逐行读：01-S5.3 保留 SARibbon 自有 `cmake/WinResource.cmake`，QWK 仅作为"WIN32 AND SHARED AND NOT NO_WIN_RC"条件模式的参照，无需实现细节。
5. **QWK 的 qmake/msbuild 集成文件**（share/qmake/*.pri.in、share/msbuild/*.props.in，share/install.cmake:1-82 生成安装）：SARibbon 无 qmake/msbuild 发行需求，未深读；若 3.x 要发布 vcpkg 之外的集成物料，可回看此模式（相对路径反推 install prefix 的写法）。
