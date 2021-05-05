#ifndef SARIBBONMAINWINDOWCONTAINEREXTENSION_H
#define SARIBBONMAINWINDOWCONTAINEREXTENSION_H
#include <QDesignerContainerExtension>
class SARibbonMainWindow;
namespace SA_PLUGIN {
/**
 * @brief SARibbonMainWindow的子窗口容器的动作由此类来操作，
 * 此类生成到qt designer由SARibbonMainWindowContainerFactory来产生，
 * SARibbonMainWindowContainerFactory将注册到SARibbonMainWindowDesignerPlugin的QExtensionManager中，
 * 注册过程发生在SARibbonMainWindowDesignerPlugin::initialize函数中
 *
 */
class SARibbonMainWindowContainerExtension : public QObject,
                         public QDesignerContainerExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerContainerExtension)
public:
    explicit SARibbonMainWindowContainerExtension(SARibbonMainWindow *w, QObject *p = nullptr);
    void addWidget(QWidget *widget) Q_DECL_OVERRIDE;
    int count() const Q_DECL_OVERRIDE;
    int currentIndex() const Q_DECL_OVERRIDE;
    void insertWidget(int index, QWidget *widget) Q_DECL_OVERRIDE;
    void remove(int index) Q_DECL_OVERRIDE;
    void setCurrentIndex(int index) Q_DECL_OVERRIDE;
    QWidget *widget(int index) const Q_DECL_OVERRIDE;

private:
    SARibbonMainWindow *m_mainwindow;
    QList<QWidget *> m_widgets;
};
}
#endif // SARIBBONMAINWINDOWCONTAINEREXTENSION_H
