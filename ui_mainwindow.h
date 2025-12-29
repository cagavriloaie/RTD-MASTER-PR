/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionEnglish;
    QAction *actionRomanian;
    QAction *actionGerman;
    QAction *actionFrench;
    QAction *actionSpanish;
    QAction *actionRussian;
    QAction *actionAbout;
    QAction *actionTechnicalDoc;
    QAction *actionMinimize;
    QAction *actionExit;
    QAction *actionShowGraph;
    QAction *actionShowTCGraph;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label;
    QComboBox *comboStandard;
    QLabel *label_2;
    QComboBox *comboMaterial;
    QLabel *label_3;
    QComboBox *comboAlpha;
    QLabel *label_4;
    QComboBox *comboR0;
    QLabel *label_5;
    QComboBox *comboTolerance;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout_2;
    QLabel *label_6;
    QComboBox *comboConversionMode;
    QLabel *labelInput;
    QDoubleSpinBox *doubleSpinInput;
    QLabel *labelOutput;
    QDoubleSpinBox *doubleSpinOutput;
    QLabel *labelToleranceLabel;
    QDoubleSpinBox *doubleSpinTolerance;
    QPushButton *btnCalculate;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_2;
    QPlainTextEdit *textInfo;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuLanguage;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(750, 650);
        MainWindow->setMinimumSize(QSize(1050, 700));
        MainWindow->setMaximumSize(QSize(1200, 800));
        MainWindow->setStyleSheet(QString::fromUtf8("/* Professional RTD Master Theme */\n"
"/* Color Palette:\n"
"   - Primary: #1e3a8a (Navy Blue)\n"
"   - Secondary: #0891b2 (Cyan)\n"
"   - Success: #059669 (Green)\n"
"   - Warning: #d97706 (Amber)\n"
"   - Info: #2563eb (Blue)\n"
"   - Background: #f8fafc\n"
"   - Surface: #ffffff\n"
"   - Border: #cbd5e1\n"
"   - Text: #1e293b\n"
"*/\n"
"\n"
"QMainWindow {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #f8fafc, stop:1 #e2e8f0);\n"
"}\n"
"\n"
"/* Global Font Settings */\n"
"* {\n"
"    font-family: \"Segoe UI\", \"Roboto\", \"Arial\", sans-serif;\n"
"    font-size: 10pt;\n"
"}\n"
"\n"
"/* Group Box Styling */\n"
"QGroupBox {\n"
"    font-size: 12pt;\n"
"    font-weight: 600;\n"
"    color: #1e293b;\n"
"    background-color: #ffffff;\n"
"    border: 2px solid #0891b2;\n"
"    border-radius: 10px;\n"
"    margin-top: 14px;\n"
"    padding-top: 20px;\n"
"    padding: 16px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcont"
                        "rol-position: top left;\n"
"    left: 18px;\n"
"    top: -8px;\n"
"    padding: 6px 14px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #0891b2, stop:1 #06b6d4);\n"
"    color: white;\n"
"    border-radius: 5px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"/* Label Styling */\n"
"QLabel {\n"
"    font-size: 10pt;\n"
"    color: #1e293b;\n"
"    font-weight: 500;\n"
"}\n"
"\n"
"/* ComboBox Styling */\n"
"QComboBox {\n"
"    font-size: 10pt;\n"
"    padding: 7px 10px;\n"
"    border: 2px solid #cbd5e1;\n"
"    border-radius: 6px;\n"
"    background-color: white;\n"
"    color: #1e293b;\n"
"    min-height: 22px;\n"
"}\n"
"\n"
"QComboBox:hover {\n"
"    border: 2px solid #0891b2;\n"
"    background-color: #f0fdfa;\n"
"}\n"
"\n"
"QComboBox:focus {\n"
"    border: 2px solid #0284c7;\n"
"}\n"
"\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    width: 28px;\n"
"    background: transparent;\n"
"}\n"
"\n"
"QComboBox::down-arrow {\n"
"    image: none;\n"
"    bo"
                        "rder-left: 4px solid transparent;\n"
"    border-right: 4px solid transparent;\n"
"    border-top: 7px solid #64748b;\n"
"    margin-right: 6px;\n"
"}\n"
"\n"
"QComboBox::down-arrow:hover {\n"
"    border-top: 7px solid #0891b2;\n"
"}\n"
"\n"
"/* SpinBox Styling */\n"
"QDoubleSpinBox {\n"
"    font-size: 10pt;\n"
"    font-weight: 500;\n"
"    padding: 6px 26px 6px 8px;\n"
"    border: 1px solid #cbd5e1;\n"
"    border-radius: 4px;\n"
"    background-color: white;\n"
"    color: #1e293b;\n"
"    min-height: 20px;\n"
"}\n"
"\n"
"QDoubleSpinBox:hover {\n"
"    border: 1px solid #0891b2;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QDoubleSpinBox:focus {\n"
"    border: 1px solid #0284c7;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QDoubleSpinBox:read-only {\n"
"    background-color: white;\n"
"    color: #475569;\n"
"    font-weight: 600;\n"
"    border: 1px solid #cbd5e1;\n"
"}\n"
"\n"
"QDoubleSpinBox::up-button {\n"
"    subcontrol-origin: border;\n"
"    subcontrol-position: top right;\n"
"    width: 2"
                        "2px;\n"
"    border: none;\n"
"    border-left: 1px solid #cbd5e1;\n"
"    border-top-right-radius: 3px;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QDoubleSpinBox::up-button:hover {\n"
"    background-color: #0891b2;\n"
"    border-left: 1px solid #0891b2;\n"
"}\n"
"\n"
"QDoubleSpinBox::up-button:pressed {\n"
"    background-color: #0284c7;\n"
"    border-left: 1px solid #0284c7;\n"
"}\n"
"\n"
"QDoubleSpinBox::down-button {\n"
"    subcontrol-origin: border;\n"
"    subcontrol-position: bottom right;\n"
"    width: 22px;\n"
"    border: none;\n"
"    border-left: 1px solid #cbd5e1;\n"
"    border-bottom-right-radius: 3px;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QDoubleSpinBox::down-button:hover {\n"
"    background-color: #0891b2;\n"
"    border-left: 1px solid #0891b2;\n"
"}\n"
"\n"
"QDoubleSpinBox::down-button:pressed {\n"
"    background-color: #0284c7;\n"
"    border-left: 1px solid #0284c7;\n"
"}\n"
"\n"
"QDoubleSpinBox::up-arrow {\n"
"    image: none;\n"
"    border-left: 4px solid transpare"
                        "nt;\n"
"    border-right: 4px solid transparent;\n"
"    border-bottom: 5px solid #64748b;\n"
"    width: 0px;\n"
"    height: 0px;\n"
"}\n"
"\n"
"QDoubleSpinBox::up-arrow:hover {\n"
"    border-bottom: 5px solid white;\n"
"}\n"
"\n"
"QDoubleSpinBox::down-arrow {\n"
"    image: none;\n"
"    border-left: 4px solid transparent;\n"
"    border-right: 4px solid transparent;\n"
"    border-top: 5px solid #64748b;\n"
"    width: 0px;\n"
"    height: 0px;\n"
"}\n"
"\n"
"QDoubleSpinBox::down-arrow:hover {\n"
"    border-top: 5px solid white;\n"
"}\n"
"\n"
"QDoubleSpinBox:read-only::up-button,\n"
"QDoubleSpinBox:read-only::down-button {\n"
"    background-color: white;\n"
"    border-left: 2px solid #cbd5e1;\n"
"}\n"
"\n"
"QDoubleSpinBox:read-only::up-arrow,\n"
"QDoubleSpinBox:read-only::down-arrow {\n"
"    border-bottom-color: #94a3b8;\n"
"    border-top-color: #94a3b8;\n"
"}\n"
"\n"
"/* LineEdit Styling */\n"
"QLineEdit {\n"
"    font-size: 10pt;\n"
"    padding: 7px 10px;\n"
"    border: 2px solid #cbd5e1;\n"
"   "
                        " border-radius: 6px;\n"
"    background-color: white;\n"
"    color: #1e293b;\n"
"    selection-background-color: #0891b2;\n"
"    selection-color: white;\n"
"}\n"
"\n"
"QLineEdit:hover {\n"
"    border: 2px solid #0891b2;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border: 2px solid #0284c7;\n"
"}\n"
"\n"
"QLineEdit:read-only {\n"
"    background-color: white;\n"
"    color: #475569;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"/* Button Styling */\n"
"QPushButton {\n"
"    font-size: 10pt;\n"
"    font-weight: 600;\n"
"    color: white;\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #0891b2, stop:1 #0e7490);\n"
"    border: none;\n"
"    border-radius: 6px;\n"
"    padding: 6px 16px;\n"
"    min-height: 26px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #06b6d4, stop:1 #0891b2);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #0e7490;\n"
"}\n"
"\n"
"/* P"
                        "lainTextEdit Styling */\n"
"QPlainTextEdit {\n"
"    font-size: 10pt;\n"
"    font-family: \"Consolas\", \"Courier New\", monospace;\n"
"    background-color: #f8fafc;\n"
"    border: 2px solid #cbd5e1;\n"
"    border-radius: 7px;\n"
"    padding: 12px;\n"
"    color: #1e293b;\n"
"    selection-background-color: #0891b2;\n"
"    selection-color: white;\n"
"}\n"
"\n"
"QPlainTextEdit:focus {\n"
"    border: 2px solid #0891b2;\n"
"}\n"
"\n"
"/* Tolerance Label Special Styling */\n"
"#labelTolerance {\n"
"    font-size: 11pt;\n"
"    font-weight: 700;\n"
"    color: #065f46;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #d1fae5, stop:1 #a7f3d0);\n"
"    padding: 10px 14px;\n"
"    border-radius: 6px;\n"
"    border: 2px solid #10b981;\n"
"}\n"
"\n"
"/* Radio Button Styling */\n"
"QRadioButton {\n"
"    font-size: 10pt;\n"
"    color: #1e293b;\n"
"    spacing: 8px;\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    width: 18px;\n"
"    height: 18px;\n"
"    bor"
                        "der-radius: 9px;\n"
"    border: 2px solid #cbd5e1;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QRadioButton::indicator:hover {\n"
"    border: 2px solid #0891b2;\n"
"    background-color: #f0fdfa;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked {\n"
"    border: 2px solid #0891b2;\n"
"    background: qradialgradient(cx:0.5, cy:0.5, radius:0.4,\n"
"                                fx:0.5, fy:0.5,\n"
"                                stop:0 #0891b2, stop:0.5 #0891b2,\n"
"                                stop:0.51 white, stop:1 white);\n"
"}\n"
"\n"
"/* Menu Bar Styling */\n"
"QMenuBar {\n"
"    background-color: #1e293b;\n"
"    color: white;\n"
"    border-bottom: 2px solid #0891b2;\n"
"    padding: 3px;\n"
"    font-size: 9pt;\n"
"}\n"
"\n"
"QMenuBar::item {\n"
"    background-color: transparent;\n"
"    padding: 4px 10px;\n"
"    margin: 1px;\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"QMenuBar::item:selected {\n"
"    background-color: #334155;\n"
"}\n"
"\n"
"QMenuBar::item:pressed {\n"
"    background-color"
                        ": #0891b2;\n"
"}\n"
"\n"
"/* Menu Styling */\n"
"QMenu {\n"
"    background-color: white;\n"
"    border: 2px solid #cbd5e1;\n"
"    border-radius: 6px;\n"
"    padding: 6px;\n"
"}\n"
"\n"
"QMenu::item {\n"
"    padding: 8px 24px 8px 12px;\n"
"    border-radius: 4px;\n"
"    color: #1e293b;\n"
"}\n"
"\n"
"QMenu::item:selected {\n"
"    background-color: #f0fdfa;\n"
"    color: #0891b2;\n"
"}\n"
"\n"
"QMenu::separator {\n"
"    height: 2px;\n"
"    background-color: #e2e8f0;\n"
"    margin: 4px 8px;\n"
"}"));
        actionEnglish = new QAction(MainWindow);
        actionEnglish->setObjectName("actionEnglish");
        actionEnglish->setCheckable(true);
        actionRomanian = new QAction(MainWindow);
        actionRomanian->setObjectName("actionRomanian");
        actionRomanian->setCheckable(true);
        actionGerman = new QAction(MainWindow);
        actionGerman->setObjectName("actionGerman");
        actionGerman->setCheckable(true);
        actionFrench = new QAction(MainWindow);
        actionFrench->setObjectName("actionFrench");
        actionFrench->setCheckable(true);
        actionSpanish = new QAction(MainWindow);
        actionSpanish->setObjectName("actionSpanish");
        actionSpanish->setCheckable(true);
        actionRussian = new QAction(MainWindow);
        actionRussian->setObjectName("actionRussian");
        actionRussian->setCheckable(true);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        actionTechnicalDoc = new QAction(MainWindow);
        actionTechnicalDoc->setObjectName("actionTechnicalDoc");
        actionMinimize = new QAction(MainWindow);
        actionMinimize->setObjectName("actionMinimize");
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        actionShowGraph = new QAction(MainWindow);
        actionShowGraph->setObjectName("actionShowGraph");
        actionShowTCGraph = new QAction(MainWindow);
        actionShowTCGraph->setObjectName("actionShowTCGraph");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(12);
        horizontalLayout->setObjectName("horizontalLayout");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName("formLayout");
        label = new QLabel(groupBox);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        comboStandard = new QComboBox(groupBox);
        comboStandard->setObjectName("comboStandard");

        formLayout->setWidget(0, QFormLayout::FieldRole, comboStandard);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        comboMaterial = new QComboBox(groupBox);
        comboMaterial->setObjectName("comboMaterial");

        formLayout->setWidget(1, QFormLayout::FieldRole, comboMaterial);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        comboAlpha = new QComboBox(groupBox);
        comboAlpha->setObjectName("comboAlpha");

        formLayout->setWidget(2, QFormLayout::FieldRole, comboAlpha);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        comboR0 = new QComboBox(groupBox);
        comboR0->setObjectName("comboR0");

        formLayout->setWidget(3, QFormLayout::FieldRole, comboR0);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_5);

        comboTolerance = new QComboBox(groupBox);
        comboTolerance->setObjectName("comboTolerance");

        formLayout->setWidget(4, QFormLayout::FieldRole, comboTolerance);


        horizontalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName("groupBox_2");
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        formLayout_2 = new QFormLayout(groupBox_2);
        formLayout_2->setObjectName("formLayout_2");
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName("label_6");

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_6);

        comboConversionMode = new QComboBox(groupBox_2);
        comboConversionMode->setObjectName("comboConversionMode");

        formLayout_2->setWidget(0, QFormLayout::FieldRole, comboConversionMode);

        labelInput = new QLabel(groupBox_2);
        labelInput->setObjectName("labelInput");

        formLayout_2->setWidget(1, QFormLayout::LabelRole, labelInput);

        doubleSpinInput = new QDoubleSpinBox(groupBox_2);
        doubleSpinInput->setObjectName("doubleSpinInput");
        doubleSpinInput->setMinimum(-273.000000000000000);
        doubleSpinInput->setMaximum(1000.000000000000000);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, doubleSpinInput);

        labelOutput = new QLabel(groupBox_2);
        labelOutput->setObjectName("labelOutput");

        formLayout_2->setWidget(2, QFormLayout::LabelRole, labelOutput);

        doubleSpinOutput = new QDoubleSpinBox(groupBox_2);
        doubleSpinOutput->setObjectName("doubleSpinOutput");
        doubleSpinOutput->setFocusPolicy(Qt::NoFocus);
        doubleSpinOutput->setReadOnly(true);
        doubleSpinOutput->setMaximum(10000.000000000000000);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, doubleSpinOutput);

        labelToleranceLabel = new QLabel(groupBox_2);
        labelToleranceLabel->setObjectName("labelToleranceLabel");

        formLayout_2->setWidget(3, QFormLayout::LabelRole, labelToleranceLabel);

        doubleSpinTolerance = new QDoubleSpinBox(groupBox_2);
        doubleSpinTolerance->setObjectName("doubleSpinTolerance");
        doubleSpinTolerance->setFocusPolicy(Qt::NoFocus);
        doubleSpinTolerance->setReadOnly(true);
        doubleSpinTolerance->setMinimum(0.000000000000000);
        doubleSpinTolerance->setMaximum(100.000000000000000);
        doubleSpinTolerance->setDecimals(3);

        formLayout_2->setWidget(3, QFormLayout::FieldRole, doubleSpinTolerance);

        btnCalculate = new QPushButton(groupBox_2);
        btnCalculate->setObjectName("btnCalculate");

        formLayout_2->setWidget(4, QFormLayout::SpanningRole, btnCalculate);


        horizontalLayout->addWidget(groupBox_2);


        verticalLayout->addLayout(horizontalLayout);

        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setMaximumHeight(180);
        verticalLayout_2 = new QVBoxLayout(groupBox_3);
        verticalLayout_2->setObjectName("verticalLayout_2");
        textInfo = new QPlainTextEdit(groupBox_3);
        textInfo->setObjectName("textInfo");
        textInfo->setFocusPolicy(Qt::NoFocus);
        textInfo->setReadOnly(true);
        textInfo->setMaximumHeight(150);

        verticalLayout_2->addWidget(textInfo);


        verticalLayout->addWidget(groupBox_3);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 750, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuLanguage = new QMenu(menubar);
        menuLanguage->setObjectName("menuLanguage");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuLanguage->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionMinimize);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuLanguage->addAction(actionEnglish);
        menuLanguage->addAction(actionRomanian);
        menuLanguage->addAction(actionGerman);
        menuLanguage->addAction(actionFrench);
        menuLanguage->addAction(actionSpanish);
        menuLanguage->addAction(actionRussian);
        menuHelp->addAction(actionShowGraph);
        menuHelp->addAction(actionShowTCGraph);
        menuHelp->addSeparator();
        menuHelp->addAction(actionTechnicalDoc);
        menuHelp->addSeparator();
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "RTD MASTER - Professional Temperature Sensor Calculator", nullptr));
        actionEnglish->setText(QCoreApplication::translate("MainWindow", "English", nullptr));
        actionRomanian->setText(QCoreApplication::translate("MainWindow", "Rom\303\242n\304\203", nullptr));
        actionGerman->setText(QCoreApplication::translate("MainWindow", "Deutsch", nullptr));
        actionFrench->setText(QCoreApplication::translate("MainWindow", "Fran\303\247ais", nullptr));
        actionSpanish->setText(QCoreApplication::translate("MainWindow", "Espa\303\261ol", nullptr));
        actionRussian->setText(QCoreApplication::translate("MainWindow", "\320\240\321\203\321\201\321\201\320\272\320\270\320\271", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About Temperature Master Pro", nullptr));
#if QT_CONFIG(shortcut)
        actionAbout->setShortcut(QCoreApplication::translate("MainWindow", "F12", nullptr));
#endif // QT_CONFIG(shortcut)
        actionTechnicalDoc->setText(QCoreApplication::translate("MainWindow", "Technical Documentation", nullptr));
#if QT_CONFIG(shortcut)
        actionTechnicalDoc->setShortcut(QCoreApplication::translate("MainWindow", "F1", nullptr));
#endif // QT_CONFIG(shortcut)
        actionMinimize->setText(QCoreApplication::translate("MainWindow", "Minimize", nullptr));
#if QT_CONFIG(shortcut)
        actionMinimize->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
#if QT_CONFIG(shortcut)
        actionExit->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        actionShowGraph->setText(QCoreApplication::translate("MainWindow", "Show R-T Curves", nullptr));
#if QT_CONFIG(tooltip)
        actionShowGraph->setToolTip(QCoreApplication::translate("MainWindow", "Display Resistance vs Temperature curves", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionShowGraph->setShortcut(QCoreApplication::translate("MainWindow", "F5", nullptr));
#endif // QT_CONFIG(shortcut)
        actionShowTCGraph->setText(QCoreApplication::translate("MainWindow", "Show TC Voltage Curves", nullptr));
#if QT_CONFIG(tooltip)
        actionShowTCGraph->setToolTip(QCoreApplication::translate("MainWindow", "Display Thermocouple Voltage vs Temperature curves", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionShowTCGraph->setShortcut(QCoreApplication::translate("MainWindow", "F6", nullptr));
#endif // QT_CONFIG(shortcut)
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "RTD Configuration", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Standard:", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Material:", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Alpha Coefficient:", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "R0 (\316\251 at 0\302\260C):", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Tolerance Class:", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "Conversion", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Mode:", nullptr));
        labelInput->setText(QCoreApplication::translate("MainWindow", "Temperature (\302\260C):", nullptr));
        labelOutput->setText(QCoreApplication::translate("MainWindow", "Resistance (\316\251):", nullptr));
        labelToleranceLabel->setText(QCoreApplication::translate("MainWindow", "Tolerance (\302\260C):", nullptr));
        doubleSpinTolerance->setPrefix(QCoreApplication::translate("MainWindow", "\302\261", nullptr));
        btnCalculate->setText(QCoreApplication::translate("MainWindow", "Calculate", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "Information", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuLanguage->setTitle(QCoreApplication::translate("MainWindow", "Language", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
