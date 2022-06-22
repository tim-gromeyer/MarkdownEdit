#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QTextBrowser>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
QT_END_NAMESPACE


class PreviewWidget : public QTextBrowser
{
public:
    PreviewWidget(QWidget *parent = nullptr);

private:
    // QNetworkAccessManager *netAccesMan;
};

#endif // PREVIEWWIDGET_H
