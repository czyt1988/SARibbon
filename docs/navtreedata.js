/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "SARibbon", "index.html", [
    [ "SARibbon简介", "index.html#saribbon简介", [
      [ "功能特点", "index.html#功能特点", null ]
    ] ],
    [ "构建", "index.html#构建", [
      [ "编译为动态库", "index.html#编译为动态库", null ],
      [ "静态引入工程", "index.html#静态引入工程", [
        [ "使用qmake", "index.html#使用qmake", null ],
        [ "使用cmake", "index.html#使用cmake", null ]
      ] ]
    ] ],
    [ "使用方法", "index.html#使用方法", [
      [ "引入库", "index.html#引入库", [
        [ "qmake", "index.html#qmake", null ],
        [ "cmake", "index.html#cmake", null ]
      ] ],
      [ "快速开始", "index.html#快速开始", [
        [ "在MainWindow中使用Ribbon", "index.html#在mainwindow中使用ribbon", null ],
        [ "在QWidget或QDialog中使用SARibbonBar", "index.html#在qwidget或qdialog中使用saribbonbar", null ],
        [ "创建Category和Pannel", "index.html#创建category和pannel", null ],
        [ "ContextCategory 上下文标签", "index.html#contextcategory-上下文标签", null ],
        [ "ApplicationButton", "index.html#applicationbutton", null ],
        [ "QuickAccessBar和rightButtonGroup", "index.html#quickaccessbar和rightbuttongroup", null ],
        [ "SARibbonBar布局方案", "index.html#saribbonbar布局方案", null ],
        [ "SARibbonBar文字换行，及图标大小", "index.html#saribbonbar文字换行及图标大小", null ],
        [ "不同的“按钮”布局方式", "index.html#不同的按钮布局方式", null ],
        [ "SARibbonPannel的布局模式", "index.html#saribbonpannel的布局模式", [
          [ "3行模式", "index.html#autotoc_md3行模式", null ],
          [ "2行模式", "index.html#autotoc_md2行模式", null ]
        ] ],
        [ "SARibbon的自定义功能", "index.html#saribbon的自定义功能", [
          [ "给界面添加自定义功能", "index.html#给界面添加自定义功能", null ]
        ] ]
      ] ]
    ] ],
    [ "更多截图", "index.html#更多截图", null ],
    [ "文档生成", "index.html#文档生成", null ],
    [ "常见问题", "index.html#常见问题", [
      [ "1、高分屏显示问题", "index.html#autotoc_md1高分屏显示问题", null ],
      [ "2、快捷键问题", "index.html#autotoc_md2快捷键问题", null ],
      [ "3、主题设置不生效", "index.html#autotoc_md3主题设置不生效", null ],
      [ "4、遇到宏重定义", "index.html#autotoc_md4遇到宏重定义", null ],
      [ "5、最大最小化图标不在右上角而在左上角", "index.html#autotoc_md5最大最小化图标不在右上角而在左上角", null ],
      [ "6、图标没有显示", "index.html#autotoc_md6图标没有显示", null ],
      [ "7、提示Could not create pixmap from :\\SARibbon\\xxx.svg", "index.html#autotoc_md7提示could-not-create-pixmap-from-saribbonimageresourcexxxsvg", null ]
    ] ],
    [ "给我一个鼓励", "index.html#给我一个鼓励", null ],
    [ "版本记录(change log):", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html", [
      [ "2025-07-31 -> 2.4.2", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2025-07-31---242", null ],
      [ "2025-07-14 -> 2.4.1", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2025-07-14---241", null ],
      [ "2025-03-12 -> 2.3.0", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2025-03-12---230", null ],
      [ "2025-02-05 -> 2.2.9", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2025-02-05---229", null ],
      [ "2025-01-04 -> 2.2.8 [hotfix]", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2025-01-04---228-hotfix", null ],
      [ "2024-12-23 -> 2.2.7 [hotfix]", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-12-23---227-hotfix", null ],
      [ "2024-12-23 -> 2.2.6", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-12-23---226", null ],
      [ "2024-10-17 -> 2.2.5", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-10-17---225", null ],
      [ "2024-09-12 -> 2.2.4", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-09-12---224", null ],
      [ "2024-07-25 -> 2.2.1", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-07-25---221", null ],
      [ "2024-07-15 -> 2.2.0", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-07-15---220", null ],
      [ "2024-05-23 -> 2.1.0", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-05-23---210", null ],
      [ "2024-02-07 -> 2.0.3", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-02-07---203", null ],
      [ "2024-02-07 -> 2.0.2", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-02-07---202", null ],
      [ "2024-01-27 -> 2.0.1【失效，请使用v2.0.2及以上版本】", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-01-27---201失效请使用v202及以上版本", null ],
      [ "2024-01-09 -> 2.0.0", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-01-09---200", null ],
      [ "2023-12-29 -> 1.1.2", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-12-29---112", null ],
      [ "2023-12-25 -> 1.1.1", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-12-25---111", null ],
      [ "2023-12-25 -> 1.1.0", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-12-25---110", null ],
      [ "2023-11-19 -> 1.0.6", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-11-19---106", null ],
      [ "2023-11-10 -> 1.0.5", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-11-10---105", null ],
      [ "2023-11-02 -> 1.0.4", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-11-02---104", null ],
      [ "2023-10-26 -> 1.0.2", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-10-26---102", null ],
      [ "2023-10-14 -> 1.0.1", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-10-14---101", null ],
      [ "2023-10-09 -> 0.7.1", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-10-09---071", null ],
      [ "2023-10-09 -> 0.7.0", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-10-09---070", null ],
      [ "2023-09-27 -> 0.6.0", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-09-27---060", null ],
      [ "2023-05-28 -> 0.5.0", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2023-05-28---050", null ],
      [ "0.5.1", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md051", null ],
      [ "0.5.2", "md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md052", null ]
    ] ],
    [ "how-to-build-cn", "how-to-build-cn.html", [
      [ "准备工作", "how-to-build-cn.html#准备工作", null ],
      [ "关于安装位置", "how-to-build-cn.html#关于安装位置", null ],
      [ "编译QWindowkit库(如果不开启跳过此步)", "how-to-build-cn.html#编译qwindowkit库如果不开启跳过此步", [
        [ "使用Qt Creator构建和安装QWindowkit库", "how-to-build-cn.html#使用qt-creator构建和安装qwindowkit库", null ],
        [ "使用visual studio构建和安装QWindowkit库", "how-to-build-cn.html#使用visual-studio构建和安装qwindowkit库", null ],
        [ "使用命令行构建(适用Qt5及vs2019以下)", "how-to-build-cn.html#使用命令行构建适用qt5及vs2019以下", null ]
      ] ],
      [ "构建SARibbonBar库", "how-to-build-cn.html#构建saribbonbar库", [
        [ "基于CMake构建SARibbonBar库", "how-to-build-cn.html#基于cmake构建saribbonbar库", [
          [ "vs下基于cmake的构建", "how-to-build-cn.html#vs下基于cmake的构建", null ],
          [ "Qt Creator下基于CMake的构建", "how-to-build-cn.html#qt-creator下基于cmake的构建", null ]
        ] ],
        [ "基于QMake构建SARibbonBar", "how-to-build-cn.html#基于qmake构建saribbonbar", null ]
      ] ],
      [ "使用SARibbonBar库", "how-to-build-cn.html#使用saribbonbar库", [
        [ "基于cmake引入SARibbonBar库", "how-to-build-cn.html#基于cmake引入saribbonbar库", null ],
        [ "基于qmake引入SARibbonBar库", "how-to-build-cn.html#基于qmake引入saribbonbar库", null ]
      ] ],
      [ "公开的预定义宏", "how-to-build-cn.html#公开的预定义宏", null ]
    ] ],
    [ "ribbon-toolbutton", "ribbon-toolbutton.html", [
      [ "ribbon工具栏按钮绘制方案", "ribbon-toolbutton.html#ribbon工具栏按钮绘制方案", [
        [ "文字换行显示的布局方案", "ribbon-toolbutton.html#文字换行显示的布局方案", null ],
        [ "文字不换行显示的布局方案", "ribbon-toolbutton.html#文字不换行显示的布局方案", null ]
      ] ],
      [ "如何布置一个更美观的Ribbon界面", "ribbon-toolbutton.html#如何布置一个更美观的ribbon界面", null ]
    ] ],
    [ "待办事项列表", "todo.html", null ],
    [ "类", "annotated.html", [
      [ "类列表", "annotated.html", "annotated_dup" ],
      [ "类索引", "classes.html", null ],
      [ "类继承关系", "hierarchy.html", "hierarchy" ],
      [ "类成员", "functions.html", [
        [ "全部", "functions.html", "functions_dup" ],
        [ "函数", "functions_func.html", "functions_func" ],
        [ "变量", "functions_vars.html", null ],
        [ "类型定义", "functions_type.html", null ],
        [ "枚举", "functions_enum.html", null ],
        [ "枚举值", "functions_eval.html", null ]
      ] ]
    ] ],
    [ "文件", "files.html", [
      [ "文件列表", "files.html", "files_dup" ]
    ] ],
    [ "示例", "examples.html", "examples" ]
  ] ]
];

var NAVTREEINDEX =
[
"_c_1_2src_2_qt_2_s_a_ribbon_2src_2_s_a_ribbon_bar_2_s_a_frameless_helper_8h-example.html",
"class_s_a_ribbon_bar.html#a69be16e6d73306d32c64c47e9dde64c0",
"class_s_a_ribbon_color_tool_button_1_1_private_data.html#a20d83a914a9dfdbd661a255cd452d9d2",
"class_s_a_ribbon_pannel_item.html#af4282b5e09c0b7ad37f1cc555a763926a945a30a994e0ae67c403424d018cf36c",
"md__c_1_2src_2_qt_2_s_a_ribbon_2changlog.html#autotoc_md2024-07-25---221"
];

var SYNCONMSG = '点击 关闭 面板同步';
var SYNCOFFMSG = '点击 开启 面板同步';