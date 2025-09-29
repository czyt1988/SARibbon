# SARibbon库的引入

## 基于cmake引入SARibbon库

基于cmake引入SARibbon库，首先要通过cmake编译SARibbon并执行安装

安装后在自己的工程`CMakeLists.txt`按照如下步骤执行：

### 1. 指定SARibbon的安装目录

把安装目录下的`lib/cmake/SARibbonBar`位置设置给`SARibbonBar_DIR`变量(CMake在执行`find_package(xx)`时，会先查看是否有`xx_DIR`变量，如果有，会先查询`xx_DIR`下是否有对应的`xxConfig.cmake`文件)

例如:

```cmake
set(SARibbonBar_DIR "C:\src\Qt\SARibbon\bin_qt5.14.2_MSVC_x64\lib\cmake\SARibbonBar")
```

### 2. 使用find_package加载库

使用`find_package`加载库，`find_package`会找到`SARibbonBar`的`Config`文件，这个函数实际上是调用`lib/cmake/SARibbonBar/SARibbonBarConfig.cmake`文件，这里会把需要include的路径、预定义的宏，和需要添加的库给指定好，此时`SARibbonBar_INCLUDE_DIR`就是SARibbonBar的include文件路径

```cmake
find_package(SARibbonBar)
```

### 3. 链接到项目

最后在`cmake`中调用`target_link_libraries`添加SARibbonBar库到自己的工程中,这里${myapp_target_name}是自己工程的target名字

```cmake
target_link_libraries(${myapp_target_name} PUBLIC
    SARibbonBar::SARibbonBar
)
```

## 基于qmake引入SARibbon库

!!! warning "警告"
    Qt6开始，不再推荐使用`qmake`，SARibbon未来的版本有可能会取消qmake的支持

qmake的编译过程会在SARibbon下生成`bin_qt{Qt version}_{MSVC/GNU}_x{32/64}`文件夹，库文件和dll文件都在此文件夹下，importSARibbonBarLib.pri会自动把这个文件夹下的库引用进来，在引入之前需要先进行配置，配置内容位于`common.pri`中

步骤如下：

### 1. 工程拷贝

先在你的工程中建立一个`3rdparty`文件夹，再把整个`SARibbon`文件夹拷贝过去

> SARibbon内部已经有几个pri文件可以很方便的让你把工程引入到自己目录中，`./importSARibbonBarLib.pri`文件就是用于引入SARibbon库的

importSARibbonBarLib.pri文件按照本库目录结构引入了依赖和头文件，如果你自己需要调整目录结构，可参考此文件进行修改

### 2. 配置common.pri文件

按照你实际的库的编译情况，配置SARibbon/common.pri文件，目前可选配置如下：

```shell
# SA_RIBBON_CONFIG+=use_frameless
#     此选项将使用frameless第三方库，这个选项在SARibbonBar.pri中会自动判断，如果，达到frameless的使用要求将会自动定义
#     frameless第三方库必须C++17且只有几个版本的qt可用，目前支持（qt5.14,qt5.15,qt6.4以上）
#     除了上诉版本SA_RIBBON_CONFIG中不会加入use_frameless
#     frameless库能实现Ubuntu下和mac下的显示，同时多屏幕的支持也较好
# 使用frameless库，需要定义QWindowKit的安装目录，默认在SARIBBON_BIN_DIR
# SA_RIBBON_QWindowKit_Install_DIR = $$SARIBBON_BIN_DIR
# 
# SA_RIBBON_CONFIG+=enable_snap_layout
#      此选项将允许开启windows11的snap layout效果，目前在qt6.5下是正常显示位置，其它已知qt版本的snap layout位置会有偏移
#      此选项必须在 SA_RIBBON_CONFIG+=use_frameless 下才有效
```

你根据实际情况打开配置项（把注释去掉）

### 3. pro文件引入

在自己的Qt工程pro文件中加入如下语句即可

```shell
include($$PWD/3rdparty/SARibbon/importSARibbonBarLib.pri)
```

此时你的工程目录结构大致如下：

```
|-[you-project-dir]
|  |-you-project.pro
|  |-[3rdparty]
|     |-[SARibbon](直接把SARibbon完整复制过来)
|        |-importSARibbonBarLib.pri（这个是方面你的qmake工程导入的文件，实际是引入了SARibbonBar.pri）
|        |-SARibbonBar.pri（用于引入库和依赖）
|        |-common.pri（这里是你的配置内容）
|        |-[bin_qtx.x.x_{MSVC/GNU}_x{32/64}]
|        |-[src]
|        |   |-[SARibbonBar]
```

`importSARibbonBarLib.pri`、`SARibbonBar.pri`、`common.pri`这三个文件是引入工程的关键文件

!!! warning "再次声明"
    Qt6.0版本后已经放弃qmake，建议使用cmake来管理工程

## 公开的预定义宏

SARibbon在编译过程中有些预定义宏，这些宏在基于visual studio的库引入是必须的

`SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1/0`,此宏用来定义是否引入了`QWindowkit`库

`SARIBBON_ENABLE_SNAP_LAYOUT=1/0`,此宏在SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1时才有用，用于定义是否开始windows11的snap layout效果

## 静态引入如何开启qwk

静态引入只需把`SARibbon.h`和`SARibbon.cpp`添加到工程里即可，此时默认是不开启`qwk`,如果你需要开启`qwk`，那么需要做如下处理：

1. 添加预定义宏

- 设置`SARIBBON_USE_3RDPARTY_FRAMELESSHELPER`为1
- 设置`SARIBBON_ENABLE_SNAP_LAYOUT`为1

如果是使用qmake，那么在`pro`文件中添加如下内容：

```shell
# 定义带值的宏
DEFINES += SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1
DEFINES += SARIBBON_ENABLE_SNAP_LAYOUT=1
```

如果使用vsproj

你你可如下操作：
- 右键点击项目 → 属性 (Properties)
- 选择 C/C++ → 预处理器 (Preprocessor)
- 在 预处理器定义 (Preprocessor Definitions) 中添加宏