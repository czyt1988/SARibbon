#ifndef SARIBBONMAINWINDOW_H
#define SARIBBONMAINWINDOW_H
#include "SARibbonGlobal.h"
#include <QMainWindow>
class SARibbonMainWindowPrivate;
class SARibbonBar;
///
/// \brief The SARibbonMainWindow class
///
class SA_RIBBON_EXPORT SARibbonMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    SARibbonMainWindow(QWidget* parent = nullptr);
    //
    const SARibbonBar* ribbonBar() const;
    SARibbonBar* ribbonBar();
    enum RibbonTheme{
        NormalTheme  ///< 普通主题

    };
    void setRibbonTheme(RibbonTheme theme);
    RibbonTheme ribbonTheme() const;
#if 0
    //重写设置stylesheet
    void setStyleSheet(const QString &styleSheet);
    ///
    /// \brief 枚举所有ribbon的元素
    ///
    enum RibbonElement
    {
        RibbonApplicationButton = 0 ///< ApplicationButton
        ,RibbonBar
        ,RibbonCategory
        ,RibbonStackedWidget
        ,RibbonTabBar
        ,RibbonToolButton
        ,RibbonMenu
        ,RibbonPannelOptionButton
        ,RibbonPannel
        ,RibbonControlButton
        ,RibbonGallery
        ,RibbonGalleryGroup
        ,RibbonComboBox
        ,RibbonLineEdit
        ,RibbonSeparatorWidget
        ,RibbonCtrlContainer
        ,RibbonQuickAccessBar
        ,RibbonButtonGroupWidget
    };
    QString ribbonElementStyleSheet(RibbonElement element) const;
    void setRibbonElementStyleSheet(RibbonElement element,const QString& styleSheet);
#endif
protected:
    void loadTheme(const QString &themeFile);
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *e);
private:
    SARibbonMainWindowPrivate* m_d;
};

#endif // SARIBBONMAINWINDOW_H
