#ifndef SWITCHLABEL_H
#define SWITCHLABEL_H

#include <QLabel>
#include <QWidget>

class SwitchLabel : public QLabel
{
    Q_OBJECT
public:
    SwitchLabel(QWidget *parent = nullptr);

signals:
    void indexChanged(int index);

protected:
    void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    int index = 0;
};

#endif // SWITCHLABEL_H
