此文针对qmake和vsproj如何使用SARibbon做说明

无论哪种方法，先用cmake编译完成SARibbon，然后进行install安装项目，获取到`bin`、`include`和`lib`三个目录，具体方法见[how-to-build-cn.md](./how-to-build-cn.md)

# cmake

如果使用cmake，那么只需要在`CMakeLists.txt`中添加如下内容：
```cmake
find_package(SARibbonBar REQUIRED)
...
target_link_libraries({your_target_name} PUBLIC SARibbonBar::SARibbonBar)
```

# 静态引入如何开启qwk

静态引入只需把`SARibbon.h`和`SARibbon.cpp`添加到工程里即可，此时默认是不开启`qwk`,如果你需要开启`qwk`，那么需要做如下处理：

1. 添加预定义宏：
- 设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER为1
- 设置SARIBBON_ENABLE_SNAP_LAYOUT为1

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