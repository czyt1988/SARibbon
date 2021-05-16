#ifndef SARIBBONCATEGORYCONTAINEREXTENSION_H
#define SARIBBONCATEGORYCONTAINEREXTENSION_H
#include <QDesignerContainerExtension>
class SARibbonCategory;
namespace SA_PLUGIN {
/**
 * @brief The SARibbonCategoryContainerExtension class
 */
class SARibbonCategoryContainerExtension : public QObject,
                       public QDesignerContainerExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerContainerExtension)
public:
    explicit SARibbonCategoryContainerExtension(SARibbonCategory *c, QObject *p = nullptr);
    void addWidget(QWidget *widget) Q_DECL_OVERRIDE;
    int count() const Q_DECL_OVERRIDE;
    int currentIndex() const Q_DECL_OVERRIDE;
    void insertWidget(int index, QWidget *widget) Q_DECL_OVERRIDE;
    void remove(int index) Q_DECL_OVERRIDE;
    void setCurrentIndex(int index) Q_DECL_OVERRIDE;
    QWidget *widget(int index) const Q_DECL_OVERRIDE;

private:
    SARibbonCategory *m_category;
};
}
#endif // SARIBBONCATEGORYCONTAINEREXTENSION_H
