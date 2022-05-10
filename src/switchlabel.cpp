#include "switchlabel.h"

#include <QMouseEvent>
#include <QStringLiteral>
#include <QDebug>


SwitchLabel::SwitchLabel(QWidget *parent)
    : QLabel{parent}
{
    setText(QStringLiteral("%1: %2").arg(tr("View HTML"), tr("Off")));
}

void SwitchLabel::mouseDoubleClickEvent(QMouseEvent *e)
{
    // double click with left mouse button?
    if (e->button() == Qt::LeftButton) {
        qDebug() << index;

        if (index == 0) {
            ++index;
            setText(QStringLiteral("%1: %2").arg(tr("View HTML"), tr("On")));
        }
        if (index == 1) {
            --index;
            setText(QStringLiteral("%1: %2").arg(tr("View HTML"), tr("Off")));
        }

        emit indexChanged(index);
    }

    QLabel::mouseDoubleClickEvent(e);
}
