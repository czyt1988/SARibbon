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