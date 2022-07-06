#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QDialogButtonBox>


class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    explicit About(const QString &title, QWidget *parent = nullptr);

    QLabel* addCredit(QString text);
    QList<QLabel*> addCredits(QStringList credits);
    void deleteCreditPage();
    inline bool creditPageAvaiable() { return creditPage; }
    void setAppUrl(QUrl url);
    void setAppUrl(const QString &url);
    void setAppVersion(const QString &version);
    void setDescription(const QString &description);

private:
    void setupUi();
    void retranslateUi();

    bool creditPage = true;
    QString homepage;
    QString _version = nullptr;
    QString _description = nullptr;

    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QFormLayout *formLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_9;
    QLabel *label_10;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_8;
    QSpacerItem *verticalSpacer;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_5;
    QDialogButtonBox *buttonBox;
};

#endif // ABOUT_H
