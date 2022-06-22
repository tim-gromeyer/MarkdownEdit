#include "previewwidget.h"

#include <QUrl>
#include <QEventLoop>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>


PreviewWidget::PreviewWidget(QWidget *parent)
    : QTextBrowser(parent)
{
    QTextBrowser::setOpenLinks(false);
    QTextBrowser::setOpenExternalLinks(true);
    QTextBrowser::setReadOnly(true);
    QTextBrowser::setUndoRedoEnabled(false);
    // netAccesMan = new QNetworkAccessManager(this);

    /*QLiteHtmlWidget::setResourceHandler([this](const QUrl &url) {
        // create blocking request
        // TODO implement asynchronous requests in container_qpainter
        QEventLoop loop;
        QByteArray data;
        QNetworkReply *reply = netAccesMan->get(QNetworkRequest(url));
        connect(reply, &QNetworkReply::finished, this, [&data, &loop, reply] {
            if (reply->error() == QNetworkReply::NoError)
                data = reply->readAll();
            reply->deleteLater();
            loop.exit();
        });
        loop.exec(QEventLoop::ExcludeUserInputEvents);
        return data;
    });*/
}
