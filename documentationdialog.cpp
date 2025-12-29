#include "documentationdialog.h"
#include <QVBoxLayout>
#include <QFont>
#include <QFontDatabase>
#include <QRegularExpression>

DocumentationDialog::DocumentationDialog(const QString &title, const QString &content, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    setupUI();

    // Convert plain text to formatted HTML and set content
    QString htmlContent = formatAsHtml(content);
    m_textBrowser->setHtml(htmlContent);

    // Set initial size (900x700 for better table visibility)
    resize(900, 700);
}

DocumentationDialog::~DocumentationDialog()
{
}

void DocumentationDialog::setupUI()
{
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create text browser for documentation (supports HTML formatting)
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setReadOnly(true);
    m_textBrowser->setOpenExternalLinks(false);

    // Use a monospace font as base
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(10);
    font.setStyleHint(QFont::Monospace);
    font.setFamily("Consolas");
    m_textBrowser->setFont(font);

    // Create close button
    m_closeButton = new QPushButton(tr("Close"), this);
    m_closeButton->setMaximumWidth(100);

    // Add widgets to layout
    mainLayout->addWidget(m_textBrowser);

    // Create button layout (centered)
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_closeButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    // Connect close button
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

QString DocumentationDialog::formatAsHtml(const QString &plainText)
{
    QString html;
    html += "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<style>";
    html += "body { font-family: Consolas, 'Courier New', monospace; font-size: 10pt; background-color: #f8f9fa; }";
    html += "pre { margin: 0; padding: 0; white-space: pre; }";
    html += ".main-title { color: #0066cc; font-size: 14pt; font-weight: bold; }";
    html += ".section-title { color: #2c5aa0; font-size: 12pt; font-weight: bold; }";
    html += ".subsection-title { color: #1a7f9e; font-size: 11pt; font-weight: bold; }";
    html += ".section-number { color: #cc6600; font-weight: bold; }";
    html += ".table-line { color: #2d3436; background-color: #ffffff; }";
    html += ".separator { color: #b2bec3; }";
    html += "</style></head><body><pre>";

    QStringList lines = plainText.split('\n');

    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        QString htmlLine;

        // Check if line is a major separator (===)
        if (line.trimmed().startsWith("====")) {
            htmlLine = "<span class='separator'>" + line.toHtmlEscaped() + "</span>";
        }
        // Check if line is a subsection separator (---)
        else if (line.trimmed().startsWith("---")) {
            htmlLine = "<span class='separator'>" + line.toHtmlEscaped() + "</span>";
        }
        // Check if previous line was === (this is a main title)
        else if (i > 0 && lines[i-1].trimmed().startsWith("====") && !line.trimmed().isEmpty()) {
            htmlLine = "<span class='main-title'>" + line.toHtmlEscaped() + "</span>";
        }
        // Check if line starts with section numbers (1., 2.1, etc.)
        else if (!line.trimmed().isEmpty() && line.trimmed().at(0).isDigit()) {
            QRegularExpression sectionPattern("^(\\d+(?:\\.\\d+)*\\.)");
            QRegularExpressionMatch match = sectionPattern.match(line.trimmed());
            if (match.hasMatch()) {
                QString number = match.captured(1);
                QString rest = line.trimmed().mid(number.length());
                // Preserve leading whitespace from original line
                int leadingSpaces = line.length() - line.trimmed().length();
                QString spaces = line.left(leadingSpaces);
                htmlLine = spaces + "<span class='section-number'>" + number.toHtmlEscaped() + "</span>" + rest.toHtmlEscaped();
            } else {
                htmlLine = line.toHtmlEscaped();
            }
        }
        // Check if next line is --- (this is a subsection title)
        else if (i < lines.size() - 1 && lines[i+1].trimmed().startsWith("---") && !line.trimmed().isEmpty()) {
            htmlLine = "<span class='subsection-title'>" + line.toHtmlEscaped() + "</span>";
        }
        // Check if line is a table border (+---+)
        else if (line.trimmed().startsWith("+") && line.contains("---")) {
            htmlLine = "<span class='table-line'>" + line.toHtmlEscaped() + "</span>";
        }
        // Check if line is a table row (|...|)
        else if (line.trimmed().startsWith("|") && line.trimmed().endsWith("|")) {
            htmlLine = "<span class='table-line'>" + line.toHtmlEscaped() + "</span>";
        }
        // Regular line
        else {
            htmlLine = line.toHtmlEscaped();
        }

        html += htmlLine + "\n";
    }

    html += "</pre></body></html>";
    return html;
}
