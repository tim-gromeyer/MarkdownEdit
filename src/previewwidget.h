#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QTextBrowser>


class PreviewWidget : public QTextBrowser
{
public:
    PreviewWidget(QWidget *parent = nullptr);

    void scrollToHeader(QString name);

private slots:
    void openUrl(const QUrl &);
};

#endif // PREVIEWWIDGET_H
