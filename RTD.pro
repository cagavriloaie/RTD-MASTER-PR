QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Application information
TARGET = RTDMaster
VERSION = 1.0.0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    rtdcalculator.cpp \
    documentationdialog.cpp \
    thermocouplecalculator.cpp \
    chartdialog.cpp \
    thermocouplechartdialog.cpp

HEADERS += \
    mainwindow.h \
    rtdcalculator.h \
    documentationdialog.h \
    thermocouplecalculator.h \
    chartdialog.h \
    thermocouplechartdialog.h

FORMS += \
    mainwindow.ui

# Application icon
RC_ICONS = icon.ico
ICON = icon.png

# Resources
RESOURCES += resources.qrc

# Translations
TRANSLATIONS += \
    translations/rtdmaster_ro.ts \
    translations/rtdmaster_de.ts \
    translations/rtdmaster_fr.ts \
    translations/rtdmaster_es.ts \
    translations/rtdmaster_ru.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Note: Documentation files will be found automatically by the smart search
# in MainWindow::onTechnicalDocClicked() which searches multiple locations
