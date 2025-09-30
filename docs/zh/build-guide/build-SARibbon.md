# SARibbon库构建

`SARibbon`库提供`CMake`和`QMake`两种方式构建，推荐使用`CMake`

!!! warning "注意"
    Qt6之后不再维护`QMake`，逐渐转向`CMake`。SARibbon的未来版本将会移除`QMake`支持。

## 使用CMake构建SARibbon库

### 使用vs下构建

1. 如果要启用`QWindowKit`，在`CMakeLists.txt`中将`SARIBBON_USE_FRAMELESS_LIB`的选项改为ON。

2. 打开CMake项目，选中`CMakeLists.txt`。

   ![build-cmake-vs](../../assets/pic/build-cmake-vs-01.png)

3. 将会形成如下的构建树。

   ![build-cmake-vs](../../assets/pic/build-cmake-vs-02.png)

4. 直接在CMake菜单中选择“全部生成”（有些版本没有CMake菜单，可在`CMakeLists.txt`上点击右键）。

   ![build-cmake-vs](../../assets/pic/build-cmake-vs-03.png)

5. 全部生成完成后，在CMake菜单中选择“安装”->“SARibbon”（有些版本没有CMake菜单，可在`CMakeLists.txt`上点击右键）。

   ![build-cmake-vs](../../assets/pic/build-cmake-vs-04.png)

此时，你会在源码根目录下看到一个新文件夹，文件夹命名格式为`bin_qt{version}_[MSVC/GNU]_x[64/86]`（你可以使用默认安装位置，将`SARIBBON_INSTALL_IN_CURRENT_DIR`变量设置为OFF即可：`SARIBBON_INSTALL_IN_CURRENT_DIR=OFF`）。

   ![build-cmake-vs](../../assets/pic/build-cmake-install-dir.png)

### 使用Qt Creator构建

点击文件->打开文件或项目选中CMakeLists.txt，加载完成后形成如下的构建树

![build-cmake-qtc](../../assets/pic/build-cmake-qtc-01.png)

Qt Creator可以在界面修改`SARIBBON_USE_FRAMELESS_LIB`值，也可以手动修改，在Qt Creator中，点击左侧边，切换到项目模式（Ctrl+5），在Current Configuration中设置`SARIBBON_USE_FRAMELESS_LIB`为ON即可开启`QWindowKit`，前提是要先编译`QWindowKit`

点击运行按钮

![build-cmake-qtc](../../assets/pic/build-cmake-qtc-02.png)

运行结束，会弹出例子窗口，点击左侧边栏的项目标签，Build的步骤，选中install

![build-cmake-qtc](../../assets/pic/build-cmake-qtc-03.png)

再次点击运行按钮，这时候你会看到源码的根目录下多出一个文件夹,文件夹命名方式为`bin_qt{version}_[MSVC/GNU]_x[64/86]`（前提是你没有改变CMAKE_INSTALL_PREFIX）

![build-cmake-qtc](../../assets/pic/build-cmake-install-dir.png)

使用SARibbon的所有内容都在这个文件夹下

## 使用QMake构建SARibbonBar

qmake构建SARibbon只需使用Qt Creator打开`SARibbon.pro`文件即可

!!! warning "注意"
    果使用Qt Creator打开`SARibbon.pro`文件过程报错，那么你的账户可能是没有足够的写权限，不同版本的Qt Creator在不同操作系统由不一样的表现，建议使用cmake
