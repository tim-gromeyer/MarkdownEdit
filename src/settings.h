#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();
    bool addPath() const { return _set; };

public slots:
    void setAddPath(bool set) { _set = set; updateSet(); };

private slots:
    void onAddPath(bool checked) { _set = checked; };
    void updateSet();

private:
    Ui::Settings *ui;

    bool _set = true;
};

#endif // SETTINGS_H
