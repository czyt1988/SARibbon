#include "mainwindow.h"
#include "helpdialog.h"
#include "syntaxhighlighter.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPanel.h"
#include "SARibbonGallery.h"
#include "SARibbonGalleryGroup.h"
#include "SARibbonColorToolButton.h"
#include "SARibbonThemeManager.h"
#include "SARibbonThemePalette.h"
#include "SARibbonUtil.h"
#include "SARibbonToolButton.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonMenu.h"
#include "SARibbonContextCategory.h"
#include "SARibbonGlobal.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QFile>
#include <QStatusBar>
#include <QPixmap>
#include <QApplication>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>

namespace {

struct ThemeEntry {
    SARibbonTheme theme;
    const char* displayName;
    const char* palettePath;
};

static const ThemeEntry s_builtinThemes[] = {
    { SARibbonTheme::RibbonThemeWindows7,       "Windows 7",         ":/SARibbonTheme/resource/palettes/win7-default.json" },
    { SARibbonTheme::RibbonThemeOffice2013,     "Office 2013",       ":/SARibbonTheme/resource/palettes/office2013-default.json" },
    { SARibbonTheme::RibbonThemeOffice2016Blue,  "Office 2016 Blue",  ":/SARibbonTheme/resource/palettes/office2016-blue.json" },
    { SARibbonTheme::RibbonThemeOffice2016Green, "Office 2016 Green", ":/SARibbonTheme/resource/palettes/office2016-green.json" },
    { SARibbonTheme::RibbonThemeOffice2016Dark,  "Office 2016 Dark",  ":/SARibbonTheme/resource/palettes/office2016-dark.json" },
    { SARibbonTheme::RibbonThemeOffice2021Blue,  "Office 2021 Blue",  ":/SARibbonTheme/resource/palettes/office2021-blue.json" },
    { SARibbonTheme::RibbonThemeOffice2021Green, "Office 2021 Green", ":/SARibbonTheme/resource/palettes/office2021-green.json" },
    { SARibbonTheme::RibbonThemeOffice2021Dark,  "Office 2021 Dark",  ":/SARibbonTheme/resource/palettes/office2021-dark.json" },
    { SARibbonTheme::RibbonThemeDark,            "Dark",              ":/SARibbonTheme/resource/palettes/dark-default.json" },
    { SARibbonTheme::RibbonThemeDark2,           "Dark2",             ":/SARibbonTheme/resource/palettes/dark2-default.json" },
};
static constexpr int s_builtinThemesCount = sizeof(s_builtinThemes) / sizeof(s_builtinThemes[0]);

struct TokenInfo {
    const char* token;
    const char* displayName;
    const char* defaultHex;
    bool isLarge;
};

static const TokenInfo s_allTokens[] = {
    { "accent",                  "Accent",           "#e5e3e5",   true  },
    { "content-bg",              "Content BG",       "#ffffff",   true  },
    { "text-color",              "Text Color",       "#242424",   true  },
    { "subtitle",                "Subtitle",         "#666666",   true  },
    { "accent-hover",            "Accent Hover",     "#cdcacd",   false },
    { "accent-pressed",          "Accent Pressed",   "#c4c1c4",   false },
    { "accent-text",             "Accent Text",      "#242424",   false },
    { "content-hover-bg",        "Hover BG",         "#d4d4d4",   false },
    { "content-pressed-bg",      "Pressed BG",       "#c6c6c6",   false },
    { "border-color",            "Border",           "#c5c5c5",   false },
    { "separator",               "Separator",        "#bec0c2",   false },
    { "input-border",            "Input Border",     "#c0c2c4",   false },
    { "input-focus",             "Input Focus",      "#269bf4",   false },
    { "hover-border",            "Hover Border",     "#badffa",   false },
    { "focus-border",            "Focus Border",     "#b9defa",   false },
    { "focus-highlight",         "Focus Highlight",  "#269bf4",   false },
    { "menu-bg",                 "Menu BG",          "#fcfcfc",   false },
    { "menu-border",             "Menu Border",      "#fcfcfc",   false },
    { "menu-btn-hover",          "Menu Btn Hover",   "#ffe580",   false },
    { "sys-button-hover",        "Sys Btn Hover",    "#f5f6f6",   false },
    { "sys-button-pressed",      "Sys Btn Pressed",  "#cacacb",   false },
    { "selection-bg",            "Selection BG",     "#9bbbf7",   true  },
    { "close-bg",                "Close BG",         "#e81123",   true  },
    { "close-bg-pressed",        "Close Pressed",    "#f1707a",   true  },
    { "white",                   "White",            "#ffffff",   false },
    { "black",                   "Black",            "#000000",   false },
    { "ribbon-text",             "Ribbon Text",      "#b2b2b2",   false },
    { "text-disabled",           "Text Disabled",    "#666666",   false },
    { "bar-border",              "Bar Border",       "#e5e3e5",   false },
    { "scroll-border",           "Scroll Border",    "#c5d2e0",   false },
    { "tab-accent",              "Tab Accent",       "#2760a7",   true  },
    { "tab-accent-hover",        "Tab Hover",        "#3b6fb5",   false },
    { "tab-text",                "Tab Text",         "#5779af",   false },
    { "tab-hover-border",        "Tab Hover Border", "#ecbc3d",   false },
    { "tab-selected-border",     "Tab Sel Border",   "#bac9db",   false },
    { "content-checked-bg",      "Checked BG",       "#cee8fc",   false },
    { "content-checked-hover-bg","Checked Hover",    "#b8d6f7",   false },
};
static constexpr int s_allTokensCount = sizeof(s_allTokens) / sizeof(s_allTokens[0]);

static QSet< QString > s_keyColorTokens = { "accent", "content-bg", "text-color", "subtitle" };
static QSet< QString > s_derivedTokens = { "accent-hover", "accent-pressed", "content-hover-bg", "content-pressed-bg", "tab-accent-hover" };

static const char* s_stateGroupTokens[] = {
    "accent-hover", "accent-pressed", "accent-text", "content-hover-bg", "content-pressed-bg"
};
static constexpr int s_stateCount = sizeof(s_stateGroupTokens) / sizeof(s_stateGroupTokens[0]);

static const char* s_borderTokens[] = {
    "border-color", "separator", "input-border", "input-focus",
    "hover-border", "focus-border", "focus-highlight"
};
static constexpr int s_borderCount = sizeof(s_borderTokens) / sizeof(s_borderTokens[0]);

static const char* s_menuSysTokens[] = {
    "menu-bg", "menu-border", "menu-btn-hover", "sys-button-hover", "sys-button-pressed"
};
static constexpr int s_menuSysCount = sizeof(s_menuSysTokens) / sizeof(s_menuSysTokens[0]);

static bool isInSet(const char* token, const QSet<QString>& set) { return set.contains(QString(token)); }
static bool isInArray(const char* token, const char* const* arr, int count)
{
    for (int i = 0; i < count; ++i) if (strcmp(token, arr[i]) == 0) return true;
    return false;
}

}

MainWindow::MainWindow(QWidget* parent)
    : SARibbonMainWindow(parent)
    , m_splitter(nullptr)
    , m_templateEdit(nullptr)
    , m_tabWidget(nullptr)
    , m_builtinQssEdit(nullptr)
    , m_customJsonEdit(nullptr)
    , m_customQssEdit(nullptr)
    , m_currentBaseTheme(SARibbonTheme::RibbonThemeOffice2021Blue)
    , m_contextCategory(nullptr)
{
    setWindowTitle(tr("Theme Designer Example"));
    setMinimumSize(1000, 700);

    m_splitter = new QSplitter(Qt::Horizontal, this);

    m_templateEdit = new QTextEdit();
    m_templateEdit->setReadOnly(true);
    m_templateEdit->setFont(QFont("Consolas", 10));
    m_templateEdit->setPlaceholderText(tr("QSS Template"));
    new QssSyntaxHighlighter(m_templateEdit->document());

    m_tabWidget = new QTabWidget();
    m_builtinQssEdit = new QTextEdit();
    m_builtinQssEdit->setReadOnly(true);
    m_builtinQssEdit->setFont(QFont("Consolas", 10));
    new QssSyntaxHighlighter(m_builtinQssEdit->document());
    m_tabWidget->addTab(m_builtinQssEdit, tr("Built-in QSS"));

    m_splitter->addWidget(m_templateEdit);
    m_splitter->addWidget(m_tabWidget);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 1);
    setCentralWidget(m_splitter);

    SARibbonBar* bar = ribbonBar();

    SARibbonBar* rb = ribbonBar();
    rb->applicationButton()->setText(tr("&File"));

    SARibbonCategory* catDesigner = bar->addCategoryPage(tr("Designer"));
    createCategoryDesigner(catDesigner);

    SARibbonCategory* catPreview = bar->addCategoryPage(tr("Preview"));
    createCategoryPreview(catPreview);

    createQuickAccessBar();
    createRightButtonGroup();

    statusBar()->showMessage(tr("Note: Some color tokens only apply to specific templates (e.g. Win7 gradients, Office 2013 specific colors)."));

    applyBuiltinTheme(m_currentBaseTheme);
    updateTemplateDisplay(m_currentBaseTheme);
    updateBuiltinQssDisplay(m_currentBaseTheme);
}

void MainWindow::createCategoryDesigner(SARibbonCategory* category)
{
    SARibbonPanel* galleryPanel = category->addPanel(tr("Theme Gallery"));
    galleryPanel->setExpanding(true);
    SARibbonGallery* gallery = galleryPanel->addGallery();
    populateGallery(gallery);
    connect(gallery, &SARibbonGallery::triggered, this, &MainWindow::onGalleryTriggered);

    SARibbonPanel* keyPanel = category->addPanel(tr("Key Colors"));
    for (int i = 0; i < s_allTokensCount; ++i) {
        if (isInSet(s_allTokens[i].token, s_keyColorTokens)) {
            createColorButton(keyPanel, QString(s_allTokens[i].token), tr(s_allTokens[i].displayName),
                              QColor(s_allTokens[i].defaultHex), true);
        }
    }

    SARibbonPanel* statePanel = category->addPanel(tr("State Colors"));
    for (int i = 0; i < s_stateCount; ++i) {
        for (int j = 0; j < s_allTokensCount; ++j) {
            if (strcmp(s_allTokens[j].token, s_stateGroupTokens[i]) == 0) {
                createColorButton(statePanel, QString(s_allTokens[j].token), tr(s_allTokens[j].displayName),
                                  QColor(s_allTokens[j].defaultHex), false);
                break;
            }
        }
    }

    SARibbonPanel* borderPanel = category->addPanel(tr("Borders"));
    for (int i = 0; i < s_borderCount; ++i) {
        for (int j = 0; j < s_allTokensCount; ++j) {
            if (strcmp(s_allTokens[j].token, s_borderTokens[i]) == 0) {
                createColorButton(borderPanel, QString(s_allTokens[j].token), tr(s_allTokens[j].displayName),
                                  QColor(s_allTokens[j].defaultHex), false);
                break;
            }
        }
    }

    SARibbonPanel* menuSysPanel = category->addPanel(tr("Menu && System"));
    for (int i = 0; i < s_menuSysCount; ++i) {
        for (int j = 0; j < s_allTokensCount; ++j) {
            if (strcmp(s_allTokens[j].token, s_menuSysTokens[i]) == 0) {
                createColorButton(menuSysPanel, QString(s_allTokens[j].token), tr(s_allTokens[j].displayName),
                                  QColor(s_allTokens[j].defaultHex), false);
                break;
            }
        }
    }

    SARibbonPanel* otherPanel = category->addPanel(tr("Others"));
    for (int j = 0; j < s_allTokensCount; ++j) {
        const char* tok = s_allTokens[j].token;
        if (!isInSet(tok, s_keyColorTokens) &&
            !isInArray(tok, s_stateGroupTokens, s_stateCount) &&
            !isInArray(tok, s_borderTokens, s_borderCount) &&
            !isInArray(tok, s_menuSysTokens, s_menuSysCount)) {
            createColorButton(otherPanel, QString(tok), tr(s_allTokens[j].displayName),
                              QColor(s_allTokens[j].defaultHex), s_allTokens[j].isLarge);
        }
    }

    SARibbonPanel* genPanel = category->addPanel(tr("Generate"));
    QAction* genAction = new QAction(tr("Generate"), this);
    genAction->setToolTip(tr("Generate JSON palette from current colors and apply it"));
    genPanel->addLargeAction(genAction);
    connect(genAction, &QAction::triggered, this, &MainWindow::onGenerate);
}

void MainWindow::createCategoryPreview(SARibbonCategory* category)
{
    SARibbonPanel* clipboardPanel = category->addPanel(tr("Clipboard"));
    QAction* actPaste   = new QAction(createSimpleIcon("P", QColor(0x46, 0x7F, 0xBD)), tr("Paste"), this);
    actPaste->setShortcut(QKeySequence::Paste);
    clipboardPanel->addLargeAction(actPaste);
    QAction* actCut     = new QAction(createSimpleIcon("X", QColor(0x2A, 0x5F, 0xAC)), tr("Cut"), this);
    actCut->setShortcut(QKeySequence::Cut);
    clipboardPanel->addSmallAction(actCut);
    QAction* actCopy    = new QAction(createSimpleIcon("C", QColor(0x1A, 0x40, 0x88)), tr("Copy"), this);
    actCopy->setShortcut(QKeySequence::Copy);
    clipboardPanel->addSmallAction(actCopy);
    QAction* actFormatP = new QAction(createSimpleIcon("F", QColor(0x41, 0x9A, 0xCF)), tr("Format\nPainter"), this);
    clipboardPanel->addLargeAction(actFormatP);

    SARibbonPanel* fontPanel = category->addPanel(tr("Font"));
    QAction* actBold      = new QAction(createSimpleIcon("B", QColor(0x33, 0x33, 0x33)), tr("Bold"), this);
    actBold->setCheckable(true);
    actBold->setShortcut(QKeySequence::Bold);
    fontPanel->addSmallAction(actBold);
    QAction* actItalic    = new QAction(createSimpleIcon("I", QColor(0x33, 0x33, 0x33)), tr("Italic"), this);
    actItalic->setCheckable(true);
    actItalic->setShortcut(QKeySequence::Italic);
    fontPanel->addSmallAction(actItalic);
    QAction* actUnderline = new QAction(createSimpleIcon("U", QColor(0x33, 0x33, 0x33)), tr("Underline"), this);
    actUnderline->setCheckable(true);
    actUnderline->setShortcut(QKeySequence::Underline);
    fontPanel->addSmallAction(actUnderline);
    fontPanel->addSeparator();
    QAction* actLargeFont = new QAction(createSimpleIcon("A", QColor(0x22, 0x54, 0x97)), tr("Large\nFont"), this);
    fontPanel->addLargeAction(actLargeFont);
    QAction* actSmallFont = new QAction(createSimpleIcon("a", QColor(0x22, 0x54, 0x97)), tr("Small\nFont"), this);
    fontPanel->addLargeAction(actSmallFont);

    SARibbonPanel* paraPanel = category->addPanel(tr("Paragraph"));
    QAction* actAlignL = new QAction(createSimpleIcon("L", QColor(0x44, 0x44, 0x44)), tr("Align\nLeft"), this);
    actAlignL->setCheckable(true);
    paraPanel->addLargeAction(actAlignL);
    QAction* actAlignC = new QAction(createSimpleIcon("C", QColor(0x44, 0x44, 0x44)), tr("Align\nCenter"), this);
    actAlignC->setCheckable(true);
    paraPanel->addLargeAction(actAlignC);
    QAction* actAlignR = new QAction(createSimpleIcon("R", QColor(0x44, 0x44, 0x44)), tr("Align\nRight"), this);
    actAlignR->setCheckable(true);
    paraPanel->addLargeAction(actAlignR);

    SARibbonMenu* popupMenu = new SARibbonMenu(this);
    for (int i = 0; i < 6; ++i) {
        popupMenu->addAction(createSimpleIcon(QString::number(i + 1), QColor(0x55, 0x55, 0xaa)),
                             tr("Style %1").arg(i + 1));
    }

    SARibbonPanel* btnStylePanel = category->addPanel(tr("Button Styles"));
    QAction* act1 = new QAction(createSimpleIcon("D", QColor(0x22, 0x54, 0x97)), tr("Delayed\nPopup"), this);
    act1->setMenu(popupMenu);
    btnStylePanel->addLargeAction(act1, QToolButton::DelayedPopup);
    QAction* act2 = new QAction(createSimpleIcon("M", QColor(0x22, 0x54, 0x97)), tr("Menu\nPopup"), this);
    act2->setCheckable(true);
    act2->setMenu(popupMenu);
    btnStylePanel->addLargeAction(act2, QToolButton::MenuButtonPopup);
    QAction* act3 = new QAction(createSimpleIcon("I", QColor(0x22, 0x54, 0x97)), tr("Instant\nPopup"), this);
    act3->setMenu(popupMenu);
    btnStylePanel->addLargeAction(act3, QToolButton::InstantPopup);
    QAction* act4 = new QAction(createSimpleIcon("-", QColor(0x99, 0x99, 0x99)), tr("Disabled"), this);
    act4->setEnabled(false);
    btnStylePanel->addLargeAction(act4);
    btnStylePanel->addSeparator();
    QAction* actSmall1 = new QAction(createSimpleIcon("a", QColor(0x22, 0x54, 0x97)), tr("Small Menu"), this);
    actSmall1->setMenu(popupMenu);
    btnStylePanel->addSmallAction(actSmall1, QToolButton::MenuButtonPopup);
    QAction* actSmall2 = new QAction(createSimpleIcon("b", QColor(0x2A, 0x5F, 0xAC)), tr("Check Small"), this);
    actSmall2->setCheckable(true);
    btnStylePanel->addSmallAction(actSmall2);

    SARibbonPanel* insertPanel = category->addPanel(tr("Insert"));
    insertPanel->setExpanding(true);
    SARibbonGallery* gallery = insertPanel->addGallery();
    QList< QAction* > galActions;
    struct { const char* name; const char* letter; int r; int g; int b; } items[] = {
        { "Document",   "D", 0x46, 0x7F, 0xBD },
        { "Download",   "d", 0x2A, 0x5F, 0xAC },
        { "Drive File", "F", 0x1A, 0x40, 0x88 },
        { "Dropbox",    "x", 0x41, 0x9A, 0xCF },
        { "Email",      "E", 0x22, 0x54, 0x97 },
        { "Encode",     "e", 0x44, 0x88, 0xBB },
        { "Favorit",    "F", 0xBB, 0x55, 0x22 },
        { "File Error", "!", 0xCC, 0x33, 0x33 },
        { "Read Only",  "R", 0x66, 0x66, 0x66 },
        { "Settings",   "S", 0x33, 0x88, 0x33 },
        { "Picture",    "P", 0x99, 0x44, 0x88 },
        { "Table",      "T", 0x44, 0x88, 0x44 },
    };
    for (auto& item : items) {
        galActions.append(new QAction(createSimpleIcon(item.letter, QColor(item.r, item.g, item.b)),
                                     tr(item.name), this));
    }
    SARibbonGalleryGroup* group = gallery->addCategoryActions(tr("Items"), galActions);
    group->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    group->setGridMinimumWidth(80);
    gallery->setCurrentViewGroup(group);

    SARibbonPanel* ctrlPanel = category->addPanel(tr("Controls"));
    QComboBox* combo = new QComboBox(this);
    combo->addItems({ tr("Option 1"), tr("Option 2"), tr("Option 3"), tr("Option 4") });
    combo->setEditable(true);
    ctrlPanel->addSmallWidget(combo);
    QLineEdit* lineEdit = new QLineEdit(this);
    lineEdit->setPlaceholderText(tr("Type here..."));
    ctrlPanel->addSmallWidget(lineEdit);
    QCheckBox* chk1 = new QCheckBox(tr("Check Box 1"), this);
    ctrlPanel->addSmallWidget(chk1);
    QCheckBox* chk2 = new QCheckBox(tr("Check Box 2"), this);
    chk2->setChecked(true);
    ctrlPanel->addSmallWidget(chk2);
    ctrlPanel->addSeparator();
    QAction* actContext = new QAction(createSimpleIcon("C", QColor(0xAA, 0x44, 0x44)), tr("Show\nContext"), this);
    actContext->setCheckable(true);
    connect(actContext, &QAction::triggered, this, &MainWindow::onShowContextCategory);
    ctrlPanel->addLargeAction(actContext);
}

void MainWindow::createRightButtonGroup()
{
    SARibbonButtonGroupWidget* rightBar = ribbonBar()->rightButtonGroup();
    QAction* actHelp = new QAction(createSimpleIcon("?", QColor(0x22, 0x54, 0x97)), tr("Help"), this);
    connect(actHelp, &QAction::triggered, this, &MainWindow::onShowHelp);
    rightBar->addAction(actHelp);
}

void MainWindow::createQuickAccessBar()
{
    SARibbonQuickAccessBar* qab = ribbonBar()->quickAccessBar();
    qab->addAction(new QAction(createSimpleIcon("S", QColor(0x22, 0x54, 0x97)), tr("Save"), this));
    qab->addSeparator();
    qab->addAction(new QAction(createSimpleIcon("U", QColor(0x2A, 0x5F, 0xAC)), tr("Undo"), this));
    qab->addAction(new QAction(createSimpleIcon("R", QColor(0x1A, 0x40, 0x88)), tr("Redo"), this));
}

void MainWindow::createColorButton(SARibbonPanel* panel, const QString& tokenName,
                                    const QString& displayName, const QColor& defaultColor,
                                    bool isLarge)
{
    SARibbonColorToolButton* btn = new SARibbonColorToolButton(panel);
    btn->setColor(defaultColor);
    btn->setColorStyle(SARibbonColorToolButton::ColorFillToIcon);
    btn->setToolTip(tokenName);
    if (isLarge) {
        btn->setButtonType(SARibbonToolButton::LargeButton);
        btn->setText(displayName);
        panel->addLargeWidget(btn);
    } else {
        btn->setText(displayName);
        panel->addSmallWidget(btn);
    }
    btn->setupStandardColorMenu();
    m_colorButtons.insert(tokenName, btn);
}

void MainWindow::populateGallery(SARibbonGallery* gallery)
{
    QList< QAction* > galleryActions;
    for (int i = 0; i < s_builtinThemesCount; ++i) {
        const ThemeEntry& entry = s_builtinThemes[i];
        QIcon icon = createThemePreviewIcon(entry.palettePath);
        QAction* action = new QAction(icon, tr(entry.displayName), this);
        action->setData(i);
        m_galleryThemeMap.insert(QString::number(i), entry.theme);
        galleryActions.append(action);
    }
    SARibbonGalleryGroup* group = gallery->addCategoryActions(tr("Built-in Themes"), galleryActions);
    group->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    group->setGridMinimumWidth(80);
    gallery->setCurrentViewGroup(group);
}

QIcon MainWindow::createThemePreviewIcon(const QString& palettePath)
{
    QFile file(palettePath);
    QColor accent(200, 200, 200);
    QColor contentBg(240, 240, 240);
    QColor textColor(60, 60, 60);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject root = doc.object();
        QJsonObject keyColors = root["keyColors"].toObject();
        if (keyColors.contains("accent"))     accent     = QColor(keyColors["accent"].toString());
        if (keyColors.contains("content-bg")) contentBg  = QColor(keyColors["content-bg"].toString());
        if (keyColors.contains("text-color")) textColor  = QColor(keyColors["text-color"].toString());
    }
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0, 0, 64, 32, accent);
    painter.fillRect(0, 32, 32, 32, contentBg);
    painter.fillRect(32, 32, 32, 32, textColor);
    painter.setPen(QColor(150, 150, 150));
    painter.drawLine(0, 32, 64, 32);
    painter.drawLine(32, 32, 32, 64);
    painter.end();
    return QIcon(pixmap);
}

QIcon MainWindow::createSimpleIcon(const QString& letter, const QColor& bg)
{
    QPixmap pm(32, 32);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(bg);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(0, 0, 32, 32, 4, 4);
    p.setPen(Qt::white);
    QFont f = p.font();
    f.setPointSize(16);
    f.setBold(true);
    p.setFont(f);
    p.drawText(pm.rect(), Qt::AlignCenter, letter);
    p.end();
    return QIcon(pm);
}

void MainWindow::applyBuiltinTheme(SARibbonTheme theme)
{
    m_currentBaseTheme = theme;
    SA::applyRibbonTheme(this, ribbonBar(), theme);
}

void MainWindow::onGalleryTriggered(QAction* action)
{
    int idx = action->data().toInt();
    if (idx >= 0 && idx < s_builtinThemesCount) {
        const ThemeEntry& entry = s_builtinThemes[idx];
        applyBuiltinTheme(entry.theme);
        syncColorButtonsFromPalette(entry.palettePath);
        updateTemplateDisplay(entry.theme);
        updateBuiltinQssDisplay(entry.theme);
        removeCustomTab();
    }
}

void MainWindow::syncColorButtonsFromPalette(const QString& palettePath)
{
    SA::SARibbonThemePalette palette;
    if (!palette.loadFromFile(palettePath)) return;
    for (auto it = m_colorButtons.begin(); it != m_colorButtons.end(); ++it) {
        QColor c = palette.color(it.key());
        if (c.isValid()) it.value()->setColor(c);
    }
}

void MainWindow::onGenerate()
{
    QJsonObject root = generatePaletteJson();
    QJsonDocument doc(root);
    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
    SA::SARibbonThemePalette palette;
    palette.loadFromJson(jsonData);

    // Generate custom QSS by replacing tokens in the template
    QString templatePath = templatePathForTheme(m_currentBaseTheme);
    QFile templateFile(templatePath);
    QString customQss;
    if (templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString templateQss = QString::fromUtf8(templateFile.readAll());
        customQss = SA::replaceQssTokens(templateQss, palette);
    }

    if (!m_customJsonEdit) {
        m_customJsonEdit = new QTextEdit();
        m_customJsonEdit->setReadOnly(true);
        m_customJsonEdit->setFont(QFont("Consolas", 10));
        new JsonSyntaxHighlighter(m_customJsonEdit->document());
        m_tabWidget->addTab(m_customJsonEdit, tr("Custom JSON"));
    }
    m_customJsonEdit->setText(QString::fromUtf8(jsonData));

    if (!m_customQssEdit) {
        m_customQssEdit = new QTextEdit();
        m_customQssEdit->setReadOnly(true);
        m_customQssEdit->setFont(QFont("Consolas", 10));
        new QssSyntaxHighlighter(m_customQssEdit->document());
        m_tabWidget->addTab(m_customQssEdit, tr("Custom QSS"));
    }
    m_customQssEdit->setText(customQss);
    m_tabWidget->setCurrentWidget(m_customJsonEdit);

    SA::applyRibbonTheme(this, ribbonBar(), m_currentBaseTheme, palette);
}

void MainWindow::onShowHelp()
{
    HelpDialog dlg(this);
    dlg.exec();
}

void MainWindow::onShowContextCategory(bool show)
{
    if (!m_contextCategory) {
        m_contextCategory = ribbonBar()->addContextCategory(tr("Context"), QColor(180, 50, 50), 1);
        SARibbonCategory* ctxPage1 = m_contextCategory->addCategoryPage(tr("Context Page 1"));
        SARibbonPanel* p1 = ctxPage1->addPanel(tr("Actions"));
        p1->addLargeAction(new QAction(createSimpleIcon("A", QColor(0x88, 0x44, 0xBB)), tr("Action 1"), this));
        p1->addLargeAction(new QAction(createSimpleIcon("B", QColor(0x33, 0x88, 0xBB)), tr("Action 2"), this));
        p1->addSeparator();
        p1->addSmallAction(new QAction(createSimpleIcon("x", QColor(0xAA, 0x22, 0x22)), tr("Small 1"), this));
        p1->addSmallAction(new QAction(createSimpleIcon("y", QColor(0x22, 0xAA, 0x22)), tr("Small 2"), this));

        SARibbonCategory* ctxPage2 = m_contextCategory->addCategoryPage(tr("Context Page 2"));
        SARibbonPanel* p2 = ctxPage2->addPanel(tr("Options"));
        p2->addLargeAction(new QAction(createSimpleIcon("C", QColor(0xDD, 0x88, 0x22)), tr("Option A"), this));
        p2->addSmallAction(new QAction(createSimpleIcon("D", QColor(0x22, 0x88, 0xDD)), tr("Option B"), this));
        p2->addSmallAction(new QAction(createSimpleIcon("E", QColor(0x88, 0x22, 0xDD)), tr("Option C"), this));
    }
    if (show) ribbonBar()->showContextCategory(m_contextCategory);
    else      ribbonBar()->hideContextCategory(m_contextCategory);
}

QJsonObject MainWindow::generatePaletteJson() const
{
    QJsonObject root;
    root["name"]    = tr("Custom Theme");
    root["version"] = "1.0";
    root["isDark"]  = false;

    QJsonObject keyColors;
    for (auto it = m_colorButtons.begin(); it != m_colorButtons.end(); ++it) {
        if (s_keyColorTokens.contains(it.key()))
            keyColors[it.key()] = it.value()->color().name();
    }
    root["keyColors"] = keyColors;

    QJsonObject derived;
    derived["accent-hover"]        = QJsonObject{ {"fn","darken"}, {"base","accent"},     {"amount",10} };
    derived["accent-pressed"]      = QJsonObject{ {"fn","darken"}, {"base","accent"},     {"amount",15} };
    derived["content-hover-bg"]    = QJsonObject{ {"fn","darken"}, {"base","content-bg"}, {"amount",17} };
    derived["content-pressed-bg"]  = QJsonObject{ {"fn","darken"}, {"base","content-bg"}, {"amount",23} };
    derived["tab-accent-hover"]    = QJsonObject{ {"fn","darken"}, {"base","accent"},     {"amount",5}  };
    root["derived"] = derived;

    QJsonObject fixed;
    for (auto it = m_colorButtons.begin(); it != m_colorButtons.end(); ++it) {
        if (!s_keyColorTokens.contains(it.key()) && !s_derivedTokens.contains(it.key()))
            fixed[it.key()] = it.value()->color().name();
    }
    root["fixed"] = fixed;

    return root;
}

QString MainWindow::templatePathForTheme(SARibbonTheme theme) const
{
    switch (theme) {
    case SARibbonTheme::RibbonThemeOffice2016Blue:
    case SARibbonTheme::RibbonThemeOffice2016Green:
    case SARibbonTheme::RibbonThemeOffice2016Dark:
        return ":/SARibbonTheme/resource/templates/office2016.qss";
    case SARibbonTheme::RibbonThemeOffice2021Blue:
    case SARibbonTheme::RibbonThemeOffice2021Green:
    case SARibbonTheme::RibbonThemeOffice2021Dark:
        return ":/SARibbonTheme/resource/templates/office2021.qss";
    case SARibbonTheme::RibbonThemeDark:
        return ":/SARibbonTheme/resource/templates/dark.qss";
    case SARibbonTheme::RibbonThemeDark2:
        return ":/SARibbonTheme/resource/templates/dark2.qss";
    case SARibbonTheme::RibbonThemeWindows7:
        return ":/SARibbonTheme/resource/templates/win7.qss";
    case SARibbonTheme::RibbonThemeOffice2013:
        return ":/SARibbonTheme/resource/templates/office2013.qss";
    default:
        return QString();
    }
}

void MainWindow::updateTemplateDisplay(SARibbonTheme theme)
{
    QString path = templatePathForTheme(theme);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_templateEdit->setText(QString::fromUtf8(file.readAll()));
    } else {
        m_templateEdit->clear();
    }
}

void MainWindow::updateBuiltinQssDisplay(SARibbonTheme theme)
{
    m_builtinQssEdit->setText(SA::getBuiltInRibbonThemeQss(theme));
}

void MainWindow::removeCustomTab()
{
    if (m_customJsonEdit) {
        int idx = m_tabWidget->indexOf(m_customJsonEdit);
        if (idx >= 0)
            m_tabWidget->removeTab(idx);
        delete m_customJsonEdit;
        m_customJsonEdit = nullptr;
    }
    if (m_customQssEdit) {
        int idx = m_tabWidget->indexOf(m_customQssEdit);
        if (idx >= 0)
            m_tabWidget->removeTab(idx);
        delete m_customQssEdit;
        m_customQssEdit = nullptr;
    }
}
