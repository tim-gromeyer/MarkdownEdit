#ifndef TODOOBJECTINTERFACE_H
#define TODOOBJECTINTERFACE_H

#include <QPainter>
#include <QTextDocument>
#include <QTextObjectInterface>

class TodoObjectInterface : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    enum ObjectType { TodoObjectType = QTextFormat::UserObject + 1 };

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) override
    {
        Q_UNUSED(doc);
        Q_UNUSED(posInDocument);

        // Adjust the size as needed for your todo widget
        return {20, 20};
    }

    void drawObject(QPainter *painter,
                    const QRectF &rect,
                    QTextDocument *doc,
                    int posInDocument,
                    const QTextFormat &format) override
    {
        Q_UNUSED(posInDocument);

        // Draw the todo checkbox
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        // Extract the todo status from the QTextFormat properties
        // bool checked = format.property("checked").toBool();
        bool checked = true;

        // Draw the checkbox outline
        painter->setPen(Qt::black);
        painter->drawRect(rect);

        // Draw the checkmark if it's checked
        if (checked) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(Qt::black);
            painter->drawLine(rect.topLeft(), rect.bottomRight());
            painter->drawLine(rect.topRight(), rect.bottomLeft());
        }

        painter->restore();
    }
};

#endif // TODOOBJECTINTERFACE_H
