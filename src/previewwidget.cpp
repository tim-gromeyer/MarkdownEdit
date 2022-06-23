#include "previewwidget.h"

#include <QUrl>
#include <QEventLoop>


PreviewWidget::PreviewWidget(QWidget *parent)
    : QTextBrowser(parent)
{
    QTextBrowser::setOpenLinks(false);
    QTextBrowser::setOpenExternalLinks(true);
    QTextBrowser::setReadOnly(true);
    QTextBrowser::setUndoRedoEnabled(false);
}
