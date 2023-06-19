#ifndef SARIBBONBARCONTAINEREXTENSION_H
#define SARIBBONBARCONTAINEREXTENSION_H
#include <QDesignerContainerExtension>
class SARibbonBar;
namespace SA_PLUGIN {
class SARibbonBarContainerExtension : public QObject,
                      public QDesignerContainerExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerContainerExtension)
public:
    explicit SARibbonBarContainerExtension(SARibbonBar *bar, QObject *p = nullptr);
    void addWidget(QWidget *widget) Q_DECL_OVERRIDE;
    int count() const Q_DECL_OVERRIDE;
    int currentIndex() const Q_DECL_OVERRIDE;
    void insertWidget(int index, QWidget *widget) Q_DECL_OVERRIDE;
    void remove(int index) Q_DECL_OVERRIDE;
    void setCurrentIndex(int index) Q_DECL_OVERRIDE;
    QWidget *widget(int index) const Q_DECL_OVERRIDE;

private:
    SARibbonBar *m_ribbonbar;
};
}
#endif // SARIBBONBARCONTAINEREXTENSION_H
