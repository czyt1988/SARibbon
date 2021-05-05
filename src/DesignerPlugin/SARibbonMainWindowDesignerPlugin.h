#ifndef SARIBBONMAINWINDOWDESIGNERPLUGIN_H
#define SARIBBONMAINWINDOWDESIGNERPLUGIN_H
#include <QDesignerCustomWidgetInterface>
class QDesignerFormEditorInterface;
namespace SA_PLUGIN {
/**
 * @brief 对应SARibbonMainWindow的插件类
 *
 * 这个插件比较复杂，它的bool isContainer()函数返回true，表明它能接收qt designer一些拖曳窗口的事件
 *
 * qt 帮助文档中有个较为详细的例子：Container Extension Example介绍了此类插件的编写
 */
class SARibbonMainWindowDesignerPlugin : public QObject,
                     public QDesignerCustomWidgetInterface
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID "SA.SARibbon.SARibbonMainWindowDesignerPlugin")
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    explicit SARibbonMainWindowDesignerPlugin(QObject *p = nullptr);
public:
    bool isContainer() const Q_DECL_OVERRIDE;
    bool isInitialized() const Q_DECL_OVERRIDE;
    QIcon icon() const Q_DECL_OVERRIDE;
    QString domXml() const Q_DECL_OVERRIDE;
    QString group() const Q_DECL_OVERRIDE;
    QString includeFile() const Q_DECL_OVERRIDE;
    QString name() const Q_DECL_OVERRIDE;
    QString toolTip() const Q_DECL_OVERRIDE;
    QString whatsThis() const Q_DECL_OVERRIDE;
    QWidget *createWidget(QWidget *parent) Q_DECL_OVERRIDE;
    void initialize(QDesignerFormEditorInterface *core) Q_DECL_OVERRIDE;

private:
    bool m_isInitialized;
    QDesignerFormEditorInterface *m_formEditor;
};
}//end namespace SA_PLUGIN



#endif // SARIBBONDESIGNERPLUGIN_H
