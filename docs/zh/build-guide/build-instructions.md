# SARibbon构建说明

此文会详细介绍如何动态构建SARibbon，如果你不熟悉构建，建议你用静态引入的方法，你只需把SARibbon.h和SARibbon.cpp引入你的工程即可使用

SARibbon采用[QWindowkit](https://github.com/stdware/qwindowkit)作为无边框窗口方案，同时也支持简单的无边框设置。如果你需要操作系统原生的窗口特性，如Windows 7及以后版本的贴边处理，或Windows 11的Snap Layout效果，建议启用[QWindowkit](https://github.com/stdware/qwindowkit)库。该库还能有效解决多屏幕移动问题。

启用QWindowkit后，你将能实现如下效果：

![set-qwindowkit-on-snap](../../assets/pic/set-qwindowkit-on-snap.gif)

若要启用[QWindowkit](https://github.com/stdware/qwindowkit)，需先编译该库。

!!! warning "注意"
   作为SARibbon项目的子模块，如果你在`git clone`时没有使用`--recursive`参数，需执行`submodule update`命令：

   ```shell
   git submodule update --init --recursive
   ```

## 关于安装位置

通过CMake构建完成后，使用`install`命令可以安装所有依赖。引用库时，只需通过`find_package`命令，即可将所有依赖和预定义宏等配置一并引入，这是目前最推荐的做法。

然而，在程序开发过程中，可能会遇到不同编译器（如MSVC、MinGW）和不同Qt版本的编译问题。如果使用默认的安装位置（Windows下为C:\Program Files），则只能安装一个版本的库

为了区分不同编译器和Qt版本，`SARibbon`默认使用本地安装。本地安装会根据编译器和Qt版本生成一个安装文件夹，文件夹命名格式为`bin_qt{version}_[MSVC/GNU]_x[64/86]`

通过`CMake`的`SARIBBON_INSTALL_IN_CURRENT_DIR`选项可以配置是否根据编译器和Qt版本安装到本地，该选项默认为`ON`，即会根据编译器和Qt版本生成一个本地文件夹进行安装