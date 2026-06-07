#include "helpdialog.h"
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QPushButton>

HelpDialog::HelpDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Theme Configuration Guide"));
    setMinimumSize(700, 500);

    auto* layout = new QVBoxLayout(this);

    m_browser = new QTextBrowser(this);
    m_browser->setOpenExternalLinks(false);
    m_browser->setHtml(
        tr(R"HTML(
        <h2>SARibbon JSON Theme Configuration</h2>

        <h3>Architecture: Template + Palette</h3>
        <p>SARibbon uses a <b>template + palette</b> architecture for theming:
        <ul>
            <li><b>Template (.qss)</b>: Defines the visual structure and layout using <code>{{token}}</code> placeholders for colors.</li>
            <li><b>Palette (.json)</b>: Provides actual color values to fill the placeholders.</li>
        </ul>
        One template + different palettes = multiple visual theme variants.</p>

        <h3>Three-Layer Color System</h3>
        <p>The JSON palette organizes colors into three layers:</p>
        <table border="1" cellpadding="5">
            <tr><th>Layer</th><th>Purpose</th><th>Example</th></tr>
            <tr><td><b>keyColors</b></td><td>Primary design tokens. Changing these propagates to derived colors.</td><td><code>accent</code>, <code>content-bg</code>, <code>text-color</code></td></tr>
            <tr><td><b>derived</b></td><td>Computed from keyColors via lighten/darken rules.</td><td><code>accent-hover</code> = darken(accent, 10)</td></tr>
            <tr><td><b>fixed</b></td><td>Absolute color values that don't derive from other tokens.</td><td><code>white</code>, <code>black</code>, <code>close-bg</code></td></tr>
        </table>

        <h3>Key Color Tokens</h3>
        <table border="1" cellpadding="5">
            <tr><th>Token</th><th>Description</th></tr>
            <tr><td><code>accent</code></td><td>Ribbon bar background, tab background, application widget background</td></tr>
            <tr><td><code>content-bg</code></td><td>Category, panel, tool button, gallery, menu background</td></tr>
            <tr><td><code>text-color</code></td><td>Main text color (tool buttons, menus, gallery)</td></tr>
            <tr><td><code>subtitle</code></td><td>Panel title, auxiliary text</td></tr>
        </table>

        <h3>How to Use This Example</h3>
        <ol>
            <li>In the <b>Base Themes</b> tab, select a built-in theme from the gallery to apply it immediately.</li>
            <li>Switch to the <b>Color Design</b> tab to customize colors.</li>
            <li>Click any color button to open a color picker and change its value.</li>
            <li>Click the <b>Generate</b> button to create a JSON palette from your color choices and apply it.</li>
            <li>The generated JSON will be displayed in the central text area.</li>
        </ol>

        <h3>Note on Token Support</h3>
        <p>Not all color tokens are used by every template. For example:
        <ul>
            <li><code>app-btn-bg</code>, <code>app-btn-hover-bg</code>, <code>gallery-selected-bg</code> are Win7-specific gradient values (not included in this editor).</li>
            <li><code>tab-accent</code> is Office 2021 specific.</li>
            <li><code>content-checked-bg</code> is Office 2013 specific.</li>
        </ul>
        Tokens not used by the current template will have no visual effect but will still appear in the generated JSON.</p>

        <h3>Dark Mode</h3>
        <p>Set <code>"isDark": true</code> in the JSON to reverse derive rules (darken becomes lighten and vice versa), ensuring correct contrast in dark themes.</p>
        )HTML"));

    layout->addWidget(m_browser);

    auto* closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeButton, 0, Qt::AlignRight);

    setLayout(layout);
}
