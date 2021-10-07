# 简介

这里将介绍SARibbon的qt designer插件是如何实现的

这个插件比较复杂，它的`bool isContainer()`函数返回`true`，表明它能接收qt designer一些拖曳窗口的事件。
qt 帮助文档中有个较为详细的例子：Container Extension Example介绍了此类插件的编写。

在插件的`isContainer`为`true`时，插件除了需要继承`QDesignerCustomWidgetInterface`以外，还需要面对几个重要类：

- `QExtensionManager`
- `QExtensionFactory`
- `QDesignerContainerExtension`
- `QDesignerFormEditorInterface`
- `QDesignerFormWindowInterface`
- `QDesignerPropertySheetExtension`

# 一些问题

如果没有成功加载插件，可以通过qt designer的“帮助”->“关于插件”中查看错误信息

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/DesignerPlugin/doc/pic/01-aboutplugin.png)

# 编写插件注意事项

注意插件类要导出信息，通过`Q_PLUGIN_METADATA`宏，如果没有使用这个宏，在designer里将不会显示，且会提示错误：

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/DesignerPlugin/doc/pic/02-fault-info.png)

这时只需加入`Q_PLUGIN_METADATA`宏即可

```cpp
class SARibbonMainWindowDesignerPlugin : public QObject,
                     public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "SA.SARibbon.SARibbonMainWindow")
    Q_INTERFACES(QDesignerCustomWidgetInterface)
```

这是因为作为一个lib，自定义的plugin没有任何导出的描述符，只能通过某些操作通知qt哪些类需要导出。

当然，如果定义了`QDesignerCustomWidgetCollectionInterface`插件集合，只需在继承`QDesignerCustomWidgetCollectionInterface`的类中申明一次`Q_PLUGIN_METADATA`即可，这里会通过customWidgets告诉qt designer有哪些plugin要导出，这样就不需要每个plugin类都定义一下`Q_PLUGIN_METADATA`,如果要导出多个控件，建议使用这种方法。

# 插件相关的几个类

## QDesignerCustomWidgetInterface

这个类是自定义窗口的接口，每个`QDesignerCustomWidgetInterface`对应了一个自定义窗口，会在designer的左侧边的窗口中显示出来。

这个类定义了一个控件的关键描述，例如一些描述性：

- `QString QDesignerCustomWidgetInterface::group() const`自定义控件的分组
- `QIcon QDesignerCustomWidgetInterface::icon() const` 自定义控件的图标

**有个关键函数`QString QDesignerCustomWidgetInterface::name() const`，这个函数非常关键，并不是指控件的名字，而是指控件类名字，因此必须返回此控件对应的类名（大小写敏感）**

另外有三个函数用于插件的生成，
- `QWidget *QDesignerCustomWidgetInterface::createWidget(QWidget - *parent)`
- `QString QDesignerCustomWidgetInterface::domXml() const`
- `void QDesignerCustomWidgetInterface::initialize(QDesignerFormEditorInterface *formEditor)`

###  createWidget

这三个函数对控件的生成非常重要，`createWidget`决定了在qt designer里的显示，qt designer里会调用这个函数获得这个控件，并显示的designer里，因此，这个函数只是提供给qt designer生成控件显示用的，并不会体现在你实际设计好的代码中，因此，有时候为了给qt designer的控件提供一些额外的操作，可以在`createWidget`函数中创建出来的窗体再设置一个事件过滤器，提供一些为qt designer额外赋予的操作，最典型的就是在设计QMainWindow时，能动态操作工具栏和菜单栏dockwidget，查看Qt Designer的源码，大部分的常用控件在`createWidget`都返回一个特定的窗体，处理一些拖曳事件修改事件。

###  domXml

`domXml`函数是真正关系到设计好的窗体和我们最后显示的效果，因为它是决定了*.ui文件是如何生成的，官方对这个函数说明比较少，需要看源码慢慢摸索，如果仅仅只是普通的单一窗口，不涉及嵌套窗体，这个函数还是比较套路化的，但是如果是类似于`QTabBar`、`QStackedWidget`这种，在qt designer中还可以添加子窗口，就比较复杂了属于`Container`类型。

最简单的套路化的domXml返回如下：
``` xml
<widget class="{className}" name="{objectName}">
</widget>
```
其中`{className}`为这个窗口的类名,`{objectName}`是在ui文件里的默认的object name

`<widget></widget>`标签之间可以附加属性标签<property>，如：

``` xml
<widget class="{className}" name="{objectName}">
 <property name="geometry">
  <rect>
   <x>0</x>
   <y>0</y>
   <width>400</width>
   <height>200</height>
  </rect>
 </property>
</widget>
```

上面这个都是普通窗口的`domXml`配置方法，但是如果涉及复杂的窗口，这个`domXml`就需要有另外的形式了。

例如`SARibbonBar`是有3个层级，最顶层是`SARibbonBar`，下面`SARibbonCategory`，再下层是`SARibbonPannel`,层级关系如下所：

![层级关系](../../doc/screenshot/saribbonbar-level.png)

那么，对于这样的层级关系，`domXml`应该如下设置：

```xml
<ui language="c++" displayname="{displayname}">
      <widget class="{className}" name="{objectName}"/>
      <!-- customwidgets是对一些特殊的窗口设置，例如对容器类窗口，需要指定添加子页面的方法 -->
      <customwidgets>
          <customwidget>
              <class>{className}</class>
              <addpagemethod>{添加页面的方法}</addpagemethod>
              <!-- 下面这个propertyspecifications是特殊属性列表的设置，无需求可不用管 -->
              <propertyspecifications>
                  <stringpropertyspecification name="fileName" notr="true" type="singleline"/>
                  <stringpropertyspecification name="text" type="richtext"/>
                  <tooltip name="text">Explanatory text to be shown in Property Editor</tooltip>
            </propertyspecifications>
        </customwidget>
    </customwidgets>
</ui>
```

`<customwidgets>`是对一些特殊的窗口设置标签，例如对容器类窗口，需要指定添加子页面的方法`<addpagemethod>`，那么对于

> `<ui>`标签下的`displayname`属性是qt designer中的显示名字，`<widget>`标签的`name`属性是对象的object name

### initialize

这是一个非常关键的函数之一，控件在初始化的时候会调用，为什么会放到最后讲这个函数呢，主要是这个函数是给予控件附加的高级功能，普通的控件不需要对此函数做特殊处理，按固定模式走就行

**要确保的是调用`initialize`函数之前`isInitialized`返回`false`，调用`initialize`函数之后`isInitialized`返回`true`**

要实现上述效果，其实只要一个bool成员变量记录一下就行，因此，无特殊需求的控件，`initialize`函数和`isInitialized`的写法如下：

头文件通过成员变量`bool m_initialized;`记录状态
```cpp
class XXXPlugin : public QDesignerCustomWidgetInterface
{
    ......
private:
    bool m_initialized;
}
```
cpp文件
```cpp
class ADC_Para_BoolWidgetPlugin : public QDesignerCustomWidgetInterface
{
    ......
private:
    bool m_initialized;
}
```