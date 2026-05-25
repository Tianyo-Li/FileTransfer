/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLineEdit *ipLineEdit;
    QLineEdit *portLineEdit;
    QPushButton *startServerBtn;
    QPushButton *connectBtn;
    QPushButton *selectFileBtn;
    QPushButton *selectDirBtn;
    QPushButton *sendBtn;
    QProgressBar *progressBar;
    QListWidget *fileListWidget;
    QTextEdit *logTextEdit;
    QLabel *statusLabel;
    QLabel *ipLabel;
    QLabel *label;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(912, 479);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        ipLineEdit = new QLineEdit(centralwidget);
        ipLineEdit->setObjectName("ipLineEdit");
        ipLineEdit->setGeometry(QRect(90, 20, 113, 25));
        portLineEdit = new QLineEdit(centralwidget);
        portLineEdit->setObjectName("portLineEdit");
        portLineEdit->setGeometry(QRect(90, 90, 113, 25));
        startServerBtn = new QPushButton(centralwidget);
        startServerBtn->setObjectName("startServerBtn");
        startServerBtn->setGeometry(QRect(260, 10, 93, 28));
        connectBtn = new QPushButton(centralwidget);
        connectBtn->setObjectName("connectBtn");
        connectBtn->setGeometry(QRect(260, 50, 93, 28));
        selectFileBtn = new QPushButton(centralwidget);
        selectFileBtn->setObjectName("selectFileBtn");
        selectFileBtn->setGeometry(QRect(260, 90, 93, 28));
        selectDirBtn = new QPushButton(centralwidget);
        selectDirBtn->setObjectName("selectDirBtn");
        selectDirBtn->setGeometry(QRect(260, 130, 93, 28));
        sendBtn = new QPushButton(centralwidget);
        sendBtn->setObjectName("sendBtn");
        sendBtn->setGeometry(QRect(260, 170, 93, 28));
        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName("progressBar");
        progressBar->setGeometry(QRect(20, 170, 120, 23));
        progressBar->setValue(24);
        fileListWidget = new QListWidget(centralwidget);
        fileListWidget->setObjectName("fileListWidget");
        fileListWidget->setGeometry(QRect(400, 10, 491, 191));
        logTextEdit = new QTextEdit(centralwidget);
        logTextEdit->setObjectName("logTextEdit");
        logTextEdit->setGeometry(QRect(400, 230, 491, 191));
        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setGeometry(QRect(20, 240, 69, 19));
        ipLabel = new QLabel(centralwidget);
        ipLabel->setObjectName("ipLabel");
        ipLabel->setGeometry(QRect(20, 20, 69, 19));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 90, 69, 19));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 912, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        startServerBtn->setText(QCoreApplication::translate("MainWindow", "\345\220\257\345\212\250\346\234\215\345\212\241", nullptr));
        connectBtn->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\220\257\350\277\236\346\216\245", nullptr));
        selectFileBtn->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        selectDirBtn->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\347\233\256\345\275\225", nullptr));
        sendBtn->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        statusLabel->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        ipLabel->setText(QCoreApplication::translate("MainWindow", "ip\345\234\260\345\235\200", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\347\253\257\345\217\243\345\217\267", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
