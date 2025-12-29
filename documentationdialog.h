#ifndef DOCUMENTATIONDIALOG_H
#define DOCUMENTATIONDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>

class DocumentationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DocumentationDialog(const QString &title, const QString &content, QWidget *parent = nullptr);
    ~DocumentationDialog();

private:
    QTextBrowser *m_textBrowser;
    QPushButton *m_closeButton;

    void setupUI();
    QString formatAsHtml(const QString &plainText);
};

#endif // DOCUMENTATIONDIALOG_H
