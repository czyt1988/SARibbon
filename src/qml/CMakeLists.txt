# SARibbonQml module (plan-04): imperative registration, Qt5/Qt6 same code
find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED
    COMPONENTS Core Gui Qml Quick QuickControls2)

# 契约 D5（R1）/ 计划 05 S1：Qt5 车道 QAction/QActionGroup 属 QtWidgets，模块
# 经 SARibbonQmlActionCompat.h 消费它们，因此 PRIVATE 链接 Qt5::Widgets ——
# 这与该头一起构成钉死的三处条件编译面之一（PRIVATE：不向消费者传播）
if(QT_VERSION_MAJOR EQUAL 5)
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
    set(_sa_qml_private_links Qt5::Widgets)
else()
    set(_sa_qml_private_links "")
endif()

# QML 模块的无边框窗口方案绑定 QWindowKit（QuickWindowAgent 路线）：
# 根 CMakeLists 已保证 _SARIBBON_USE_FRAMELESS_LIB 与 QWindowKit::Quick 存在
# （已安装包 find_package 或 3rdparty 树内构建两种路径）
if(NOT TARGET QWindowKit::Quick)
    find_package(QWindowKit REQUIRED COMPONENTS Quick)
endif()

sa_add_library(SARibbonQml
    SOURCES
        SARibbonQmlGlobal.h
        SARibbonQmlTypes.h
        SARibbonQmlTypes.cpp
        SARibbonQmlActionCompat.h
        SARibbonQmlAction.h
        SARibbonQmlAction.cpp
        SARibbonQmlIconProvider.h
        SARibbonQmlIconProvider.cpp
        SARibbonQmlShortcutMatcher.h
        SARibbonQmlShortcutMatcher.cpp
        SARibbonQmlQuickHost.h
        SARibbonQmlQuickHost.cpp
        SARibbonQmlLayoutItemHost.h
        SARibbonQmlLayoutItemHost.cpp
        SARibbonQmlButtonRowHost.h
        SARibbonQmlButtonRowHost.cpp
        SARibbonQmlTheme.h
        SARibbonQmlTheme.cpp
        SARibbonQmlMetrics.h
        SARibbonQmlMetrics.cpp
        SARibbonQmlContextCategory.h
        SARibbonQmlContextCategory.cpp
        SARibbonQmlPanel.h
        SARibbonQmlPanel.cpp
        SARibbonQmlToolButton.h
        SARibbonQmlToolButton.cpp
        SARibbonQmlControlContainer.h
        SARibbonQmlControlContainer.cpp
        SARibbonQmlGallery.h
        SARibbonQmlGallery.cpp
        SARibbonQmlGalleryGroup.h
        SARibbonQmlGalleryGroup.cpp
        SARibbonQmlGalleryItem.h
        SARibbonQmlGalleryItem.cpp
        SARibbonQmlColorGrid.h
        SARibbonQmlColorGrid.cpp
        SARibbonQmlColorMenu.h
        SARibbonQmlColorMenu.cpp
        SARibbonQmlColorToolButton.h
        SARibbonQmlColorToolButton.cpp
        SARibbonQmlSeparator.h
        SARibbonQmlSeparator.cpp
        SARibbonQmlQuickAccessBar.h
        SARibbonQmlQuickAccessBar.cpp
        SARibbonQmlButtonGroup.h
        SARibbonQmlButtonGroup.cpp
        SARibbonQmlApplicationWindow.h
        SARibbonQmlApplicationWindow.cpp
        SARibbonQmlCategory.h
        SARibbonQmlCategory.cpp
        SARibbonQmlTab.h
        SARibbonQmlTab.cpp
        SARibbonQmlBar.h
        SARibbonQmlBar.cpp
        SARibbonQmlActionRegistry.h
        SARibbonQmlActionRegistry.cpp
        SARibbonQmlActionRegistryModel.h
        SARibbonQmlActionRegistryModel.cpp
        SARibbonQmlCustomizer.h
        SARibbonQmlCustomizer.cpp
        SARibbonQmlCustomizeTreeModel.h
        SARibbonQmlCustomizeTreeModel.cpp
        SARibbonQmlWindowAgent.h
        SARibbonQmlWindowAgent.cpp
        qml/saribbon_qml.qrc
    PREFIX SA_RIBBON_QML
    QT_LINKS Core Gui Quick Qml QuickControls2
    LINKS SARibbonCore QWindowKit::Quick
    LINKS_PRIVATE ${_sa_qml_private_links}
)
sa_sync_include(SARibbonQml SARibbonQml)
