#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "SARibbonMainWindow.h"
#include "SARibbonGlobal.h"
#include <QTextEdit>
#include <QMap>

class SARibbonBar;
class SARibbonCategory;
class SARibbonPanel;
class SARibbonGallery;
class SARibbonGalleryGroup;
class SARibbonColorToolButton;
class SARibbonContextCategory;

class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void createCategoryDesigner(SARibbonCategory* category);
    void createCategoryPreview(SARibbonCategory* category);
    void createRightButtonGroup();
    void createQuickAccessBar();
    void populateGallery(SARibbonGallery* gallery);
    QIcon createThemePreviewIcon(const QString& palettePath);
    QIcon createSimpleIcon(const QString& letter, const QColor& bg);
    void applyBuiltinTheme(SARibbonTheme theme);
    QJsonObject generatePaletteJson() const;
    void createColorButton(SARibbonPanel* panel, const QString& tokenName,
                           const QString& displayName, const QColor& defaultColor,
                           bool isLarge);
    void syncColorButtonsFromPalette(const QString& palettePath);

private Q_SLOTS:
    void onGalleryTriggered(QAction* action);
    void onGenerate();
    void onShowHelp();
    void onShowContextCategory(bool show);

private:
    QTextEdit* m_textEdit;
    QMap<QString, SARibbonColorToolButton*> m_colorButtons;
    SARibbonTheme m_currentBaseTheme;
    QMap<QString, SARibbonTheme> m_galleryThemeMap;
    SARibbonContextCategory* m_contextCategory;
};

#endif
