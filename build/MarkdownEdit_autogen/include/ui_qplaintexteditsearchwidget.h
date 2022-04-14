/********************************************************************************
** Form generated from reading UI file 'qplaintexteditsearchwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QPLAINTEXTEDITSEARCHWIDGET_H
#define UI_QPLAINTEXTEDITSEARCHWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QPlainTextEditSearchWidget
{
public:
    QGridLayout *gridLayout;
    QLineEdit *searchLineEdit;
    QLineEdit *replaceLineEdit;
    QLabel *searchCountLabel;
    QLabel *searchLabel;
    QPushButton *searchUpButton;
    QLabel *replaceLabel;
    QPushButton *closeButton;
    QPushButton *replaceToggleButton;
    QPushButton *searchDownButton;
    QPushButton *matchCaseSensitiveButton;
    QLabel *modeLabel;
    QFrame *buttonFrame;
    QHBoxLayout *horizontalLayout;
    QComboBox *modeComboBox;
    QSpacerItem *horizontalSpacer;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;

    void setupUi(QWidget *QPlainTextEditSearchWidget)
    {
        if (QPlainTextEditSearchWidget->objectName().isEmpty())
            QPlainTextEditSearchWidget->setObjectName(QString::fromUtf8("QPlainTextEditSearchWidget"));
        QPlainTextEditSearchWidget->resize(836, 142);
        QPlainTextEditSearchWidget->setAutoFillBackground(true);
        gridLayout = new QGridLayout(QPlainTextEditSearchWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        searchLineEdit = new QLineEdit(QPlainTextEditSearchWidget);
        searchLineEdit->setObjectName(QString::fromUtf8("searchLineEdit"));

        gridLayout->addWidget(searchLineEdit, 0, 3, 1, 1);

        replaceLineEdit = new QLineEdit(QPlainTextEditSearchWidget);
        replaceLineEdit->setObjectName(QString::fromUtf8("replaceLineEdit"));

        gridLayout->addWidget(replaceLineEdit, 1, 3, 1, 2);

        searchCountLabel = new QLabel(QPlainTextEditSearchWidget);
        searchCountLabel->setObjectName(QString::fromUtf8("searchCountLabel"));
        searchCountLabel->setText(QString::fromUtf8("-/-"));

        gridLayout->addWidget(searchCountLabel, 0, 4, 1, 1);

        searchLabel = new QLabel(QPlainTextEditSearchWidget);
        searchLabel->setObjectName(QString::fromUtf8("searchLabel"));
        searchLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(searchLabel, 0, 1, 1, 1);

        searchUpButton = new QPushButton(QPlainTextEditSearchWidget);
        searchUpButton->setObjectName(QString::fromUtf8("searchUpButton"));
        QIcon icon;
        QString iconThemeName = QString::fromUtf8("go-top");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon = QIcon::fromTheme(iconThemeName);
        } else {
            icon.addFile(QString::fromUtf8(":/media/go-top.svg"), QSize(), QIcon::Normal, QIcon::Off);
        }
        searchUpButton->setIcon(icon);
        searchUpButton->setFlat(true);

        gridLayout->addWidget(searchUpButton, 0, 6, 1, 1);

        replaceLabel = new QLabel(QPlainTextEditSearchWidget);
        replaceLabel->setObjectName(QString::fromUtf8("replaceLabel"));
        replaceLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(replaceLabel, 1, 0, 1, 2);

        closeButton = new QPushButton(QPlainTextEditSearchWidget);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));
        QIcon icon1;
        iconThemeName = QString::fromUtf8("window-close");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon1 = QIcon::fromTheme(iconThemeName);
        } else {
            icon1.addFile(QString::fromUtf8(":/media/window-close.svg"), QSize(), QIcon::Normal, QIcon::Off);
        }
        closeButton->setIcon(icon1);
        closeButton->setFlat(true);

        gridLayout->addWidget(closeButton, 0, 0, 1, 1);

        replaceToggleButton = new QPushButton(QPlainTextEditSearchWidget);
        replaceToggleButton->setObjectName(QString::fromUtf8("replaceToggleButton"));
        QIcon icon2;
        iconThemeName = QString::fromUtf8("edit-find-replace");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon2 = QIcon::fromTheme(iconThemeName);
        } else {
            icon2.addFile(QString::fromUtf8(":/media/edit-find-replace.svg"), QSize(), QIcon::Normal, QIcon::Off);
        }
        replaceToggleButton->setIcon(icon2);
        replaceToggleButton->setCheckable(true);
        replaceToggleButton->setFlat(true);

        gridLayout->addWidget(replaceToggleButton, 0, 7, 1, 1);

        searchDownButton = new QPushButton(QPlainTextEditSearchWidget);
        searchDownButton->setObjectName(QString::fromUtf8("searchDownButton"));
        QIcon icon3;
        iconThemeName = QString::fromUtf8("go-bottom");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon3 = QIcon::fromTheme(iconThemeName);
        } else {
            icon3.addFile(QString::fromUtf8(":/media/go-bottom.svg"), QSize(), QIcon::Normal, QIcon::Off);
        }
        searchDownButton->setIcon(icon3);
        searchDownButton->setFlat(true);

        gridLayout->addWidget(searchDownButton, 0, 5, 1, 1);

        matchCaseSensitiveButton = new QPushButton(QPlainTextEditSearchWidget);
        matchCaseSensitiveButton->setObjectName(QString::fromUtf8("matchCaseSensitiveButton"));
        QIcon icon4;
        iconThemeName = QString::fromUtf8("format-text-superscript");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon4 = QIcon::fromTheme(iconThemeName);
        } else {
            icon4.addFile(QString::fromUtf8(":/media/format-text-superscript.svg"), QSize(), QIcon::Normal, QIcon::Off);
        }
        matchCaseSensitiveButton->setIcon(icon4);
        matchCaseSensitiveButton->setCheckable(true);
        matchCaseSensitiveButton->setFlat(true);

        gridLayout->addWidget(matchCaseSensitiveButton, 1, 5, 1, 1);

        modeLabel = new QLabel(QPlainTextEditSearchWidget);
        modeLabel->setObjectName(QString::fromUtf8("modeLabel"));
        modeLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(modeLabel, 2, 0, 1, 2);

        buttonFrame = new QFrame(QPlainTextEditSearchWidget);
        buttonFrame->setObjectName(QString::fromUtf8("buttonFrame"));
        buttonFrame->setFrameShape(QFrame::NoFrame);
        horizontalLayout = new QHBoxLayout(buttonFrame);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        modeComboBox = new QComboBox(buttonFrame);
        modeComboBox->addItem(QString());
        modeComboBox->addItem(QString());
        modeComboBox->addItem(QString());
        modeComboBox->setObjectName(QString::fromUtf8("modeComboBox"));

        horizontalLayout->addWidget(modeComboBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        replaceButton = new QPushButton(buttonFrame);
        replaceButton->setObjectName(QString::fromUtf8("replaceButton"));
        replaceButton->setFlat(false);

        horizontalLayout->addWidget(replaceButton);

        replaceAllButton = new QPushButton(buttonFrame);
        replaceAllButton->setObjectName(QString::fromUtf8("replaceAllButton"));
        replaceAllButton->setFlat(false);

        horizontalLayout->addWidget(replaceAllButton);


        gridLayout->addWidget(buttonFrame, 2, 3, 1, 2);

        QWidget::setTabOrder(searchLineEdit, replaceLineEdit);
        QWidget::setTabOrder(replaceLineEdit, replaceButton);
        QWidget::setTabOrder(replaceButton, replaceAllButton);
        QWidget::setTabOrder(replaceAllButton, searchDownButton);
        QWidget::setTabOrder(searchDownButton, searchUpButton);
        QWidget::setTabOrder(searchUpButton, replaceToggleButton);
        QWidget::setTabOrder(replaceToggleButton, closeButton);

        retranslateUi(QPlainTextEditSearchWidget);

        QMetaObject::connectSlotsByName(QPlainTextEditSearchWidget);
    } // setupUi

    void retranslateUi(QWidget *QPlainTextEditSearchWidget)
    {
        searchLineEdit->setPlaceholderText(QApplication::translate("QPlainTextEditSearchWidget", "Find in text", nullptr));
        replaceLineEdit->setPlaceholderText(QApplication::translate("QPlainTextEditSearchWidget", "Replace with", nullptr));
        searchLabel->setText(QApplication::translate("QPlainTextEditSearchWidget", "Find:", nullptr));
#ifndef QT_NO_TOOLTIP
        searchUpButton->setToolTip(QApplication::translate("QPlainTextEditSearchWidget", "Search backward", nullptr));
#endif // QT_NO_TOOLTIP
        searchUpButton->setText(QString());
        replaceLabel->setText(QApplication::translate("QPlainTextEditSearchWidget", "Replace:", nullptr));
#ifndef QT_NO_TOOLTIP
        closeButton->setToolTip(QApplication::translate("QPlainTextEditSearchWidget", "Close search", nullptr));
#endif // QT_NO_TOOLTIP
        closeButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        replaceToggleButton->setToolTip(QApplication::translate("QPlainTextEditSearchWidget", "Advanced search / replace text", nullptr));
#endif // QT_NO_TOOLTIP
        replaceToggleButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        searchDownButton->setToolTip(QApplication::translate("QPlainTextEditSearchWidget", "Search forward", nullptr));
#endif // QT_NO_TOOLTIP
        searchDownButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        matchCaseSensitiveButton->setToolTip(QApplication::translate("QPlainTextEditSearchWidget", "Match case sensitive", nullptr));
#endif // QT_NO_TOOLTIP
        matchCaseSensitiveButton->setText(QString());
        modeLabel->setText(QApplication::translate("QPlainTextEditSearchWidget", "Mode:", nullptr));
        modeComboBox->setItemText(0, QApplication::translate("QPlainTextEditSearchWidget", "Plain text", nullptr));
        modeComboBox->setItemText(1, QApplication::translate("QPlainTextEditSearchWidget", "Whole words", nullptr));
        modeComboBox->setItemText(2, QApplication::translate("QPlainTextEditSearchWidget", "Regular expression", nullptr));

#ifndef QT_NO_TOOLTIP
        replaceButton->setToolTip(QApplication::translate("QPlainTextEditSearchWidget", "Replace one text occurrence", nullptr));
#endif // QT_NO_TOOLTIP
        replaceButton->setText(QApplication::translate("QPlainTextEditSearchWidget", "Replace", nullptr));
#ifndef QT_NO_TOOLTIP
        replaceAllButton->setToolTip(QApplication::translate("QPlainTextEditSearchWidget", "Replace all text occurrences", nullptr));
#endif // QT_NO_TOOLTIP
        replaceAllButton->setText(QApplication::translate("QPlainTextEditSearchWidget", "Replace all", nullptr));
        Q_UNUSED(QPlainTextEditSearchWidget);
    } // retranslateUi

};

namespace Ui {
    class QPlainTextEditSearchWidget: public Ui_QPlainTextEditSearchWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QPLAINTEXTEDITSEARCHWIDGET_H
