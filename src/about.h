/**
 ** This file is part of the MarkdownEdit project.
 ** Copyright 2022 Tim Gromeyer <sakul8825@gmail.com>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/


#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QScrollArea;
class QFormLayout;
class QVBoxLayout;
class QTabWidget;
class QTabWidget;
class QLabel;
class QSpacerItem;
QT_END_NAMESPACE


class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    explicit About(const QString &title, QWidget *parent = nullptr);

    QLabel* addCredit(const QString &text);
    QList<QLabel*> addCredits(const QStringList &credits);
    void deleteCreditPage();
    inline bool creditPageAvaiable() { return creditPage; }
    void setAppUrl(const QUrl &url);
    void setAppUrl(const QString &url);
    void setAppVersion(const QString &version);
    void setDescription(const QString &description);

private:
    void setupUi();
    void retranslateUi();

    bool creditPage = true;
    QString homepage;
    QString _version;
    QString _description;

    QVBoxLayout *verticalLayout = nullptr;
    QTabWidget *tabWidget = nullptr;
    QWidget *tab = nullptr;
    QFormLayout *formLayout = nullptr;
    QLabel *label = nullptr;
    QLabel *label_2 = nullptr;
    QLabel *label_3 = nullptr;
    QLabel *label_4 = nullptr;
    QLabel *label_6 = nullptr;
    QLabel *label_7 = nullptr;
    QLabel *label_9 = nullptr;
    QLabel *label_10 = nullptr;
    QWidget *tab_2 = nullptr;
    QVBoxLayout *verticalLayout_3 = nullptr;
    QLabel *label_8 = nullptr;
    QSpacerItem *verticalSpacer = nullptr;
    QWidget *tab_3 = nullptr;
    QVBoxLayout *verticalLayout_2 = nullptr;
    QScrollArea *scrollArea = nullptr;
    QWidget *scrollAreaWidgetContents = nullptr;
    QVBoxLayout *verticalLayout_4 = nullptr;
    QLabel *label_5 = nullptr;
    QDialogButtonBox *buttonBox = nullptr;
};

#endif // ABOUT_H
