#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QActionGroup>
#include <QButtonGroup>
#include "rtdcalculator.h"
#include "thermocouplecalculator.h"
#include "chartdialog.h"
#include "thermocouplechartdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Measurement mode enumeration
enum class MeasurementMode {
    RTD_MODE = 0,
    THERMOCOUPLE_MODE = 1
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void onCalculateClicked();

    // RTD slots
    void onStandardChanged(int index);
    void onMaterialChanged(int index);
    void onAlphaChanged(int index);
    void onToleranceChanged(int index);
    void onConversionModeChanged(int index);
    void onR0Changed(int index);

    // Thermocouple slots
    void onThermocoupleTypeChanged(int index);
    void onColdJunctionTempChanged(double value);

    // Mode switching
    void onModeChanged(int mode);

    // Common slots
    void onLanguageChanged(QAction *action);
    void onAboutClicked();
    void onTechnicalDocClicked();
    void onShowGraphClicked();
    void onShowTCGraphClicked();

private:
    Ui::MainWindow *ui;

    // Calculators
    RTDCalculator m_calculator;
    ThermocoupleCalculator m_thermocoupleCalculator;

    // UI state
    MeasurementMode m_currentMode;
    QTranslator m_translator;
    QActionGroup *m_languageGroup;
    QButtonGroup *m_modeButtonGroup;
    QString m_currentLanguage;

    // Setup methods
    void setupConnections();
    void setupModeSelector();
    void setupThermocoupleUI();

    // RTD-specific methods
    void updateCalculation();
    void populateR0Combo(int materialIndex);
    void populateAlphaCombo(int standardIndex, int materialIndex);
    void updateSpinboxRanges();

    // Thermocouple-specific methods
    void updateThermocoupleCalculation();
    void updateThermocoupleInfo();
    QString getTranslatedColorCode(ThermocoupleType type);
    QString getTranslatedApplications(ThermocoupleType type);
    double getStandardLimitsOfError(ThermocoupleType type);
    double getStandardLimitsPercent(ThermocoupleType type);

    // Mode management
    void switchMode(MeasurementMode mode);
    void showRTDControls(bool show);
    void showThermocoupleControls(bool show);

    // Translation
    void switchLanguage(const QString &language);
    void retranslateUi();

    // Helper structures
    struct ResistanceRange {
        double minRes;
        double maxRes;
    };

    ResistanceRange calculateResistanceRange(double minTemp, double maxTemp);
};

#endif // MAINWINDOW_H
