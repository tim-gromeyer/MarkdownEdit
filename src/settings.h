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
    bool useWebBrowser() const { return _use; }
    bool addPath() const { return _set; };

public slots:
    void setUseWebBrowser(bool use) { _use = use; updateUse(); };
    void setAddPath(bool set) { _set = set; updateSet(); };

private slots:
    void onUseWebBrowser(bool checked) { _use = checked; };
    void onAddPath(bool checked) { _set = checked; };
    void updateUse();
    void updateSet();

private:
    Ui::Settings *ui;

    bool _use = false;
    bool _set = true;
};

#endif // SETTINGS_H
