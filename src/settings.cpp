#include "settings.h"
#include "ui_settings.h"


Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    connect(ui->useWebBrowser, &QCheckBox::clicked, this, &Settings::onUseWebBrowser);
    connect(ui->addToPath, &QCheckBox::clicked, this, &Settings::onAddPath);
}

void Settings::updateUse()
{
    ui->useWebBrowser->setChecked(_use);
}

void Settings::updateSet()
{
    ui->addToPath->setChecked(_set);
}

Settings::~Settings()
{
    delete ui;
}
