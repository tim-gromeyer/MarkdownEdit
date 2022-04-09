#include "settings.h"
#include "ui_settings.h"


Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    connect(ui->addToPath, &QCheckBox::clicked, this, &Settings::onAddPath);
}

void Settings::updateSet()
{
    ui->addToPath->setChecked(_set);
}

Settings::~Settings()
{
    delete ui;
}
