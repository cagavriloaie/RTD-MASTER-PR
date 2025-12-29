#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "documentationdialog.h"
#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

// R0 value mapping by material type
struct R0Option {
    QString displayName;
    double value;
};

static const QVector<R0Option> PLATINUM_R0_OPTIONS = {
    {"Pt100 (100 Ω)", 100.0},
    {"Pt500 (500 Ω)", 500.0},
    {"Pt1000 (1000 Ω)", 1000.0}
};

static const QVector<R0Option> COPPER_R0_OPTIONS = {
    {"Cu50 (50 Ω)", 50.0},
    {"Cu100 (100 Ω)", 100.0}
};

static const QVector<R0Option> NICKEL_R0_OPTIONS = {
    {"Ni100 (100 Ω)", 100.0},
    {"Ni120 (120 Ω)", 120.0},
    {"Ni1000 (1000 Ω)", 1000.0}
};

// Alpha coefficient mapping by (standard, material) combination
struct AlphaOption {
    QString displayName;
    AlphaCoefficient value;
};

struct AlphaMapping {
    QVector<AlphaOption> options;
    int defaultIndex;
};

// [standard_index][material_index]
// Standards: 0=IEC, 1=GOST, 2=ASTM, 3=Legacy US
// Materials: 0=Platinum, 1=Copper, 2=Nickel
static const AlphaMapping ALPHA_MAPPINGS[4][3] = {
    // IEC 60751
    {
        // Platinum
        {{{"Pt 0.00385 (IEC/ASTM)", AlphaCoefficient::PT_385}}, 0},
        // Copper
        {{{"Cu 0.00427 (IEC)", AlphaCoefficient::CU_427}}, 0},
        // Nickel
        {{{"Ni 0.00617 (Ni100)", AlphaCoefficient::NI_617},
          {"Ni 0.00618 (Ni1000)", AlphaCoefficient::NI_618},
          {"Ni 0.00672 (Ni120)", AlphaCoefficient::NI_672}}, 0}
    },
    // GOST 6651
    {
        // Platinum
        {{{"Pt 0.00391 (GOST)", AlphaCoefficient::PT_391}}, 0},
        // Copper
        {{{"Cu 0.00426 (GOST)", AlphaCoefficient::CU_426}}, 0},
        // Nickel
        {{{"Ni 0.00617 (Ni100)", AlphaCoefficient::NI_617},
          {"Ni 0.00618 (Ni1000)", AlphaCoefficient::NI_618},
          {"Ni 0.00672 (Ni120)", AlphaCoefficient::NI_672}}, 0}
    },
    // ASTM E1137
    {
        // Platinum
        {{{"Pt 0.00385 (IEC/ASTM)", AlphaCoefficient::PT_385}}, 0},
        // Copper
        {{{"Cu 0.00427 (IEC)", AlphaCoefficient::CU_427}}, 0},
        // Nickel
        {{{"Ni 0.00617 (Ni100)", AlphaCoefficient::NI_617},
          {"Ni 0.00618 (Ni1000)", AlphaCoefficient::NI_618},
          {"Ni 0.00672 (Ni120)", AlphaCoefficient::NI_672}}, 0}
    },
    // Legacy US
    {
        // Platinum
        {{{"Pt 0.00392 (Legacy US)", AlphaCoefficient::PT_392}}, 0},
        // Copper (all copper coefficients)
        {{{"Cu 0.00426 (GOST)", AlphaCoefficient::CU_426},
          {"Cu 0.00427 (IEC)", AlphaCoefficient::CU_427},
          {"Cu 0.00428", AlphaCoefficient::CU_428}}, 1},
        // Nickel
        {{{"Ni 0.00617 (Ni100)", AlphaCoefficient::NI_617},
          {"Ni 0.00618 (Ni1000)", AlphaCoefficient::NI_618},
          {"Ni 0.00672 (Ni120)", AlphaCoefficient::NI_672}}, 0}
    }
};

// Temperature range mapping by (standard, material) combination
struct TemperatureRange {
    double minTemp;
    double maxTemp;
};

// [standard_index][material_index]
// Standards: 0=IEC, 1=GOST, 2=ASTM, 3=Legacy US
// Materials: 0=Platinum, 1=Copper, 2=Nickel
static const TemperatureRange TEMP_RANGE_MAPPINGS[4][3] = {
    // IEC 60751
    {{-200.0, 850.0}, {-50.0, 150.0}, {-60.0, 180.0}},
    // GOST 6651
    {{-200.0, 600.0}, {-50.0, 150.0}, {-60.0, 180.0}},
    // ASTM E1137
    {{-200.0, 850.0}, {-50.0, 150.0}, {-60.0, 180.0}},
    // Legacy US
    {{-200.0, 850.0}, {-50.0, 150.0}, {-60.0, 180.0}}
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentMode(MeasurementMode::RTD_MODE)
    , m_currentLanguage("en")
{
    ui->setupUi(this);

    // Remove window title bar buttons (minimize, maximize, close)
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    // Initialize thermocouple calculator with default Type K
    m_thermocoupleCalculator.setType(ThermocoupleType::TYPE_K);

    setWindowTitle("Temperature Master Pro - Professional Measurement System");

    // Set application icon
    setWindowIcon(QIcon(":/icon.png"));

    // Setup language action group
    m_languageGroup = new QActionGroup(this);
    m_languageGroup->addAction(ui->actionEnglish);
    m_languageGroup->addAction(ui->actionRomanian);
    m_languageGroup->addAction(ui->actionGerman);
    m_languageGroup->addAction(ui->actionFrench);
    m_languageGroup->addAction(ui->actionSpanish);
    m_languageGroup->addAction(ui->actionRussian);

    // Load saved language preference
    QSettings settings("RTDMaster", "LanguageSettings");
    QString savedLanguage = settings.value("language", "en").toString();

    // Set default checked language
    if (savedLanguage == "ro") {
        ui->actionRomanian->setChecked(true);
    } else if (savedLanguage == "de") {
        ui->actionGerman->setChecked(true);
    } else if (savedLanguage == "fr") {
        ui->actionFrench->setChecked(true);
    } else if (savedLanguage == "es") {
        ui->actionSpanish->setChecked(true);
    } else if (savedLanguage == "ru") {
        ui->actionRussian->setChecked(true);
    } else {
        ui->actionEnglish->setChecked(true);
    }

    switchLanguage(savedLanguage);

    // Force retranslation after language is set
    ui->retranslateUi(this);
    retranslateUi();

    // Populate combo boxes
    ui->comboStandard->addItem(tr("IEC 60751"));
    ui->comboStandard->addItem(tr("GOST 6651"));
    ui->comboStandard->addItem(tr("ASTM E1137"));
    ui->comboStandard->addItem(tr("Legacy US"));

    ui->comboMaterial->addItem(tr("Platinum"));
    ui->comboMaterial->addItem(tr("Copper"));
    ui->comboMaterial->addItem(tr("Nickel"));

    ui->comboTolerance->addItem(tr("Class AA"));
    ui->comboTolerance->addItem(tr("Class A"));
    ui->comboTolerance->addItem(tr("Class B"));
    ui->comboTolerance->addItem(tr("Class C"));
    ui->comboTolerance->setCurrentIndex(2); // Default to Class B

    ui->comboConversionMode->addItem(tr("Temperature → Resistance"));
    ui->comboConversionMode->addItem(tr("Resistance → Temperature"));

    // Set default values
    ui->comboStandard->setCurrentIndex(0);  // Default to IEC 60751
    ui->comboMaterial->setCurrentIndex(0);  // Default to Platinum
    ui->comboConversionMode->setCurrentIndex(0);  // Default to Temperature → Resistance
    populateAlphaCombo(0, 0);  // Initialize with IEC + Platinum
    populateR0Combo(0); // Initialize with Platinum options
    ui->doubleSpinInput->setValue(25.0);  // Default to 25°C (room temperature)

    // Setup mode selector and thermocouple UI
    setupModeSelector();
    setupThermocoupleUI();

    setupConnections();

    // Set initial suffixes for spinboxes
    ui->doubleSpinInput->setSuffix(" °C");
    ui->doubleSpinOutput->setSuffix(" Ω");

    // Initialize to RTD mode
    switchMode(MeasurementMode::RTD_MODE);

    // Initialize spinbox ranges after all defaults are set
    updateSpinboxRanges();

    // Perform initial calculation to populate output fields
    updateCalculation();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    connect(ui->btnCalculate, &QPushButton::clicked, this, &MainWindow::onCalculateClicked);
    connect(ui->comboStandard, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onStandardChanged);
    connect(ui->comboMaterial, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onMaterialChanged);
    connect(ui->comboAlpha, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onAlphaChanged);
    connect(ui->comboR0, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onR0Changed);
    connect(ui->comboTolerance, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onToleranceChanged);
    connect(ui->comboConversionMode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onConversionModeChanged);

    // Language menu connections
    connect(m_languageGroup, &QActionGroup::triggered, this, &MainWindow::onLanguageChanged);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutClicked);
    connect(ui->actionTechnicalDoc, &QAction::triggered, this, &MainWindow::onTechnicalDocClicked);

    // Tools menu connections
    connect(ui->actionShowGraph, &QAction::triggered, this, &MainWindow::onShowGraphClicked);
    connect(ui->actionShowTCGraph, &QAction::triggered, this, &MainWindow::onShowTCGraphClicked);

    // File menu connections
    connect(ui->actionMinimize, &QAction::triggered, this, &QWidget::showMinimized);
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);
}

void MainWindow::onStandardChanged(int index)
{
    m_calculator.setStandard(static_cast<RTDStandard>(index));

    // Update alpha combo based on current material
    int currentMaterial = ui->comboMaterial->currentIndex();
    populateAlphaCombo(index, currentMaterial);

    // Update spinbox ranges
    updateSpinboxRanges();
}

void MainWindow::onMaterialChanged(int index)
{
    m_calculator.setMaterial(static_cast<RTDMaterial>(index));

    // Get current standard selection
    int currentStandard = ui->comboStandard->currentIndex();

    // Update alpha combo based on standard + material
    populateAlphaCombo(currentStandard, index);

    // Update R0 combo box based on material
    populateR0Combo(index);

    // Update spinbox ranges
    updateSpinboxRanges();
}

void MainWindow::onAlphaChanged(int index)
{
    if (index < 0 || ui->comboAlpha->count() == 0) {
        return;
    }

    // Retrieve AlphaCoefficient enum from user data
    int alphaValue = ui->comboAlpha->itemData(index).toInt();
    m_calculator.setAlphaCoefficient(static_cast<AlphaCoefficient>(alphaValue));
}

void MainWindow::onToleranceChanged(int index)
{
    m_calculator.setToleranceClass(static_cast<ToleranceClass>(index));
}

void MainWindow::onConversionModeChanged(int index)
{
    if (m_currentMode == MeasurementMode::RTD_MODE) {
        // RTD mode
        if (index == 0) {
            ui->labelInput->setText("Temperature (°C):");
            ui->labelOutput->setText("Resistance (Ω):");
            ui->doubleSpinInput->setSuffix(" °C");
            ui->doubleSpinOutput->setSuffix(" Ω");
        } else {
            ui->labelInput->setText("Resistance (Ω):");
            ui->labelOutput->setText("Temperature (°C):");
            ui->doubleSpinInput->setSuffix(" Ω");
            ui->doubleSpinOutput->setSuffix(" °C");
        }
        // Update spinbox ranges (mode change swaps input/output)
        updateSpinboxRanges();
    } else {
        // Thermocouple mode
        if (index == 0) {
            ui->labelInput->setText("Temperature (°C):");
            ui->labelOutput->setText("Voltage (mV):");
        } else {
            ui->labelInput->setText("Voltage (mV):");
            ui->labelOutput->setText("Temperature (°C):");
        }
        // Update thermocouple calculation which will set suffixes
        updateThermocoupleCalculation();
    }
}

void MainWindow::onR0Changed(int index)
{
    if (index < 0 || ui->comboR0->count() == 0) {
        return;
    }

    double r0Value = ui->comboR0->itemData(index).toDouble();
    m_calculator.setNominalResistance(r0Value);

    // Update spinbox ranges (resistance range depends on R0)
    updateSpinboxRanges();
}

void MainWindow::populateR0Combo(int materialIndex)
{
    ui->comboR0->blockSignals(true);
    ui->comboR0->clear();

    const QVector<R0Option>* options = nullptr;
    int defaultIndex = 0;

    switch (materialIndex) {
        case 0: // Platinum
            options = &PLATINUM_R0_OPTIONS;
            defaultIndex = 0; // Pt100
            break;
        case 1: // Copper
            options = &COPPER_R0_OPTIONS;
            defaultIndex = 1; // Cu100
            break;
        case 2: // Nickel
            options = &NICKEL_R0_OPTIONS;
            defaultIndex = 0; // Ni100
            break;
    }

    if (options) {
        for (const R0Option& opt : *options) {
            ui->comboR0->addItem(opt.displayName, opt.value);
        }
        ui->comboR0->setCurrentIndex(defaultIndex);
    }

    ui->comboR0->blockSignals(false);
    onR0Changed(ui->comboR0->currentIndex());
}

void MainWindow::populateAlphaCombo(int standardIndex, int materialIndex)
{
    ui->comboAlpha->blockSignals(true);
    ui->comboAlpha->clear();

    if (standardIndex < 0 || standardIndex >= 4 ||
        materialIndex < 0 || materialIndex >= 3) {
        ui->comboAlpha->blockSignals(false);
        return;
    }

    const AlphaMapping& mapping = ALPHA_MAPPINGS[standardIndex][materialIndex];

    for (const AlphaOption& opt : mapping.options) {
        ui->comboAlpha->addItem(opt.displayName,
                                static_cast<int>(opt.value));
    }

    if (mapping.options.size() > 0) {
        int defaultIdx = std::min(mapping.defaultIndex,
                                  static_cast<int>(mapping.options.size()) - 1);
        ui->comboAlpha->setCurrentIndex(defaultIdx);
    }

    ui->comboAlpha->blockSignals(false);
    onAlphaChanged(ui->comboAlpha->currentIndex());
}

MainWindow::ResistanceRange MainWindow::calculateResistanceRange(double minTemp, double maxTemp)
{
    ResistanceRange range;

    // Calculate resistance at both temperature extremes
    double resAtMin = m_calculator.temperatureToResistance(minTemp);
    double resAtMax = m_calculator.temperatureToResistance(maxTemp);

    // RTD resistance is monotonically increasing with temperature
    range.minRes = resAtMin;
    range.maxRes = resAtMax;

    // Add small margin (0.1%) for floating-point precision
    double margin = (range.maxRes - range.minRes) * 0.001;
    range.minRes -= margin;
    range.maxRes += margin;

    // Ensure minimum is non-negative
    if (range.minRes < 0.0) {
        range.minRes = 0.0;
    }

    return range;
}

void MainWindow::updateSpinboxRanges()
{
    int standardIndex = ui->comboStandard->currentIndex();
    int materialIndex = ui->comboMaterial->currentIndex();
    int conversionMode = ui->comboConversionMode->currentIndex();

    // Validate indices
    if (standardIndex < 0 || standardIndex >= 4 ||
        materialIndex < 0 || materialIndex >= 3) {
        return;
    }

    // Get temperature range
    const TemperatureRange& tempRange = TEMP_RANGE_MAPPINGS[standardIndex][materialIndex];

    // Calculate resistance range
    ResistanceRange resRange = calculateResistanceRange(tempRange.minTemp, tempRange.maxTemp);

    // Block signals during update
    ui->doubleSpinInput->blockSignals(true);
    ui->doubleSpinOutput->blockSignals(true);

    if (conversionMode == 0) {
        // Mode 0: Temperature → Resistance
        ui->doubleSpinInput->setMinimum(tempRange.minTemp);
        ui->doubleSpinInput->setMaximum(tempRange.maxTemp);
        ui->doubleSpinOutput->setMinimum(resRange.minRes);
        ui->doubleSpinOutput->setMaximum(resRange.maxRes);
    } else {
        // Mode 1: Resistance → Temperature
        ui->doubleSpinInput->setMinimum(resRange.minRes);
        ui->doubleSpinInput->setMaximum(resRange.maxRes);
        ui->doubleSpinOutput->setMinimum(tempRange.minTemp);
        ui->doubleSpinOutput->setMaximum(tempRange.maxTemp);
    }

    // Reset if current value is outside new range
    double currentValue = ui->doubleSpinInput->value();
    double inputMin = ui->doubleSpinInput->minimum();
    double inputMax = ui->doubleSpinInput->maximum();

    if (currentValue < inputMin || currentValue > inputMax) {
        if (conversionMode == 0) {
            ui->doubleSpinInput->setValue(0.0);  // Reset to 0°C
        } else {
            double r0 = ui->comboR0->currentData().toDouble();
            ui->doubleSpinInput->setValue(r0);  // Reset to R0
        }
    }

    // Restore signals
    ui->doubleSpinInput->blockSignals(false);
    ui->doubleSpinOutput->blockSignals(false);
}

void MainWindow::onCalculateClicked()
{
    updateCalculation();
}

void MainWindow::updateCalculation()
{
    // R0 value already set by onR0Changed(), no need to set again

    double inputValue = ui->doubleSpinInput->value();
    double outputValue = 0.0;
    double tolerance = 0.0;
    
    try {
        if (ui->comboConversionMode->currentIndex() == 0) {
            // Temperature to Resistance
            outputValue = m_calculator.temperatureToResistance(inputValue);
            tolerance = m_calculator.calculateTolerance(inputValue);
        } else {
            // Resistance to Temperature
            outputValue = m_calculator.resistanceToTemperature(inputValue);
            tolerance = m_calculator.calculateTolerance(outputValue);
        }

        ui->doubleSpinOutput->setValue(outputValue);
        ui->doubleSpinTolerance->setValue(tolerance);
        ui->doubleSpinTolerance->show();  // Show tolerance for RTD

        // Display calculation-specific information
        QString info;
        double r0 = ui->comboR0->currentData().toDouble();
        double alpha = m_calculator.getAlphaValue();

        if (ui->comboConversionMode->currentIndex() == 0) {
            // Temperature to Resistance mode - show resistance breakdown
            double resistanceChange = outputValue - r0;
            double resistanceChangePercent = (resistanceChange / r0) * 100.0;
            double tempCoeff = alpha * 1000.0; // Convert to ppm/°C

            QString equation = (inputValue >= 0) ?
                "R(T) = R₀(1 + AT + BT²)" :
                "R(T) = R₀[1 + AT + BT² + C(T-100)T³]";

            info = QString("CALCULATION DETAILS:\n"
                          "Temperature: %1°C\n"
                          "Base Resistance (R₀): %2 Ω\n"
                          "Calculated Resistance: %3 Ω\n"
                          "Resistance Change: %4 Ω (%5%)\n\n"
                          "ACCURACY:\n"
                          "Tolerance Class: %6\n"
                          "Tolerance at %1°C: ±%7°C\n"
                          "Reference: %8\n\n"
                          "TECHNICAL:\n"
                          "Temp. Coefficient: %9 ppm/°C\n"
                          "Equation Used: %10")
                    .arg(inputValue, 0, 'f', 2)
                    .arg(r0, 0, 'f', 2)
                    .arg(outputValue, 0, 'f', 4)
                    .arg(resistanceChange, 0, 'f', 4)
                    .arg(resistanceChangePercent, 0, 'f', 2)
                    .arg(ui->comboTolerance->currentText())
                    .arg(tolerance, 0, 'f', 3)
                    .arg(ui->comboStandard->currentText())
                    .arg(tempCoeff, 0, 'f', 0)
                    .arg(equation);
        } else {
            // Resistance to Temperature mode - show temperature calculation
            double resistanceChange = inputValue - r0;
            double resistanceChangePercent = (resistanceChange / r0) * 100.0;
            double tempCoeff = alpha * 1000.0; // Convert to ppm/°C

            QString equation = (outputValue >= 0) ?
                "Quadratic equation (T ≥ 0°C)" :
                "Newton-Raphson iteration (T < 0°C)";

            info = QString("CALCULATION DETAILS:\n"
                          "Measured Resistance: %1 Ω\n"
                          "Base Resistance (R₀): %2 Ω\n"
                          "Resistance Change: %3 Ω (%4%)\n"
                          "Calculated Temperature: %5°C\n\n"
                          "ACCURACY:\n"
                          "Tolerance Class: %6\n"
                          "Tolerance at %5°C: ±%7°C\n"
                          "Reference: %8\n\n"
                          "TECHNICAL:\n"
                          "Temp. Coefficient: %9 ppm/°C\n"
                          "Calculation Method: %10")
                    .arg(inputValue, 0, 'f', 4)
                    .arg(r0, 0, 'f', 2)
                    .arg(resistanceChange, 0, 'f', 4)
                    .arg(resistanceChangePercent, 0, 'f', 2)
                    .arg(outputValue, 0, 'f', 2)
                    .arg(ui->comboTolerance->currentText())
                    .arg(tolerance, 0, 'f', 3)
                    .arg(ui->comboStandard->currentText())
                    .arg(tempCoeff, 0, 'f', 0)
                    .arg(equation);
        }

        ui->textInfo->setPlainText(info);
        
    } catch (...) {
        QMessageBox::warning(this, tr("Error"), tr("Calculation error occurred."));
    }
}

void MainWindow::switchLanguage(const QString &language)
{
    if (m_currentLanguage == language) {
        return;
    }

    m_currentLanguage = language;

    // Remove previous translator
    qApp->removeTranslator(&m_translator);

    // Load new translation
    if (language != "en") {
        QString qmFile = QString(":/translations/rtdmaster_%1.qm").arg(language);
        if (m_translator.load(qmFile)) {
            qApp->installTranslator(&m_translator);
        }
    }

    // Save language preference
    QSettings settings("RTDMaster", "LanguageSettings");
    settings.setValue("language", language);

    // Force UI update by sending LanguageChange event to all widgets
    QEvent languageChangeEvent(QEvent::LanguageChange);

    // Update main window
    QApplication::sendEvent(this, &languageChangeEvent);

    // Update all child widgets and dialogs
    foreach (QWidget *widget, QApplication::allWidgets()) {
        if (widget) {
            QApplication::sendEvent(widget, &languageChangeEvent);
        }
    }

    // Force repaint
    this->update();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateUi();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::retranslateUi()
{
    // Retranslate dynamic content that's not in the UI file
    // Update window title
    setWindowTitle(tr("Temperature Master Pro - Professional Measurement System"));

    // Update combo box items
    int currentStandard = ui->comboStandard->currentIndex();
    ui->comboStandard->blockSignals(true);
    ui->comboStandard->clear();
    ui->comboStandard->addItem(tr("IEC 60751"));
    ui->comboStandard->addItem(tr("GOST 6651"));
    ui->comboStandard->addItem(tr("ASTM E1137"));
    ui->comboStandard->addItem(tr("Legacy US"));
    ui->comboStandard->setCurrentIndex(currentStandard);
    ui->comboStandard->blockSignals(false);

    int currentMaterial = ui->comboMaterial->currentIndex();
    ui->comboMaterial->blockSignals(true);
    ui->comboMaterial->clear();
    ui->comboMaterial->addItem(tr("Platinum"));
    ui->comboMaterial->addItem(tr("Copper"));
    ui->comboMaterial->addItem(tr("Nickel"));
    ui->comboMaterial->setCurrentIndex(currentMaterial);
    ui->comboMaterial->blockSignals(false);

    int currentTolerance = ui->comboTolerance->currentIndex();
    ui->comboTolerance->blockSignals(true);
    ui->comboTolerance->clear();
    ui->comboTolerance->addItem(tr("Class AA"));
    ui->comboTolerance->addItem(tr("Class A"));
    ui->comboTolerance->addItem(tr("Class B"));
    ui->comboTolerance->addItem(tr("Class C"));
    ui->comboTolerance->setCurrentIndex(currentTolerance);
    ui->comboTolerance->blockSignals(false);

    int currentMode = ui->comboConversionMode->currentIndex();
    ui->comboConversionMode->blockSignals(true);
    ui->comboConversionMode->clear();
    ui->comboConversionMode->addItem(tr("Temperature → Resistance"));
    ui->comboConversionMode->addItem(tr("Resistance → Temperature"));
    ui->comboConversionMode->setCurrentIndex(currentMode);
    ui->comboConversionMode->blockSignals(false);

    // Repopulate R0 and Alpha combos to update labels
    populateR0Combo(currentMaterial);
    populateAlphaCombo(currentStandard, currentMaterial);

    // Update thermocouple configuration group title
    QGroupBox *tcGroup = ui->centralwidget->findChild<QGroupBox*>("groupBoxThermocouple");
    if (tcGroup) {
        tcGroup->setTitle(tr("Thermocouple Configuration"));
    }

    // Update thermocouple form labels
    QLabel *labelTCType = ui->centralwidget->findChild<QLabel*>("labelTCType");
    if (labelTCType) labelTCType->setText(tr("Type:"));

    QLabel *labelTCColdJunction = ui->centralwidget->findChild<QLabel*>("labelTCColdJunction");
    if (labelTCColdJunction) labelTCColdJunction->setText(tr("Cold Junction:"));

    QLabel *labelTCTempRange = ui->centralwidget->findChild<QLabel*>("labelTCTempRange");
    if (labelTCTempRange) labelTCTempRange->setText(tr("Temp. Range:"));

    QLabel *labelTCVoltRange = ui->centralwidget->findChild<QLabel*>("labelTCVoltRange");
    if (labelTCVoltRange) labelTCVoltRange->setText(tr("Voltage Range:"));

    QLabel *labelTCSensitivity = ui->centralwidget->findChild<QLabel*>("labelTCSensitivity");
    if (labelTCSensitivity) labelTCSensitivity->setText(tr("Sensitivity:"));

    QLabel *labelTCColorCode = ui->centralwidget->findChild<QLabel*>("labelTCColorCode");
    if (labelTCColorCode) labelTCColorCode->setText(tr("Color Code:"));

    QLabel *labelTCApplications = ui->centralwidget->findChild<QLabel*>("labelTCApplications");
    if (labelTCApplications) labelTCApplications->setText(tr("Applications:"));

    // Update thermocouple type combo box
    QComboBox *comboType = ui->centralwidget->findChild<QComboBox*>("comboTCType");
    if (comboType) {
        int currentType = comboType->currentIndex();
        comboType->blockSignals(true);
        comboType->clear();
        comboType->addItem(tr("Type K (Chromel-Alumel)"));
        comboType->addItem(tr("Type J (Iron-Constantan)"));
        comboType->addItem(tr("Type T (Copper-Constantan)"));
        comboType->addItem(tr("Type E (Chromel-Constantan)"));
        comboType->addItem(tr("Type N (Nicrosil-Nisil)"));
        comboType->addItem(tr("Type S (Pt-Pt/Rh 10%)"));
        comboType->addItem(tr("Type R (Pt-Pt/Rh 13%)"));
        comboType->addItem(tr("Type B (Pt/Rh 30%-Pt/Rh 6%)"));
        comboType->setCurrentIndex(currentType);
        comboType->blockSignals(false);
    }

    // Update mode selector radio buttons
    QRadioButton *rtdModeButton = ui->centralwidget->findChild<QRadioButton*>("radioRTDMode");
    QRadioButton *thermocoupleModeButton = ui->centralwidget->findChild<QRadioButton*>("radioThermocoupleMode");
    if (rtdModeButton) {
        rtdModeButton->setText(tr("RTD (Resistance Temperature Detector)"));
    }
    if (thermocoupleModeButton) {
        thermocoupleModeButton->setText(tr("Thermocouple"));
    }

    // Update mode selector group title
    QGroupBox *modeGroup = ui->centralwidget->findChild<QGroupBox*>();
    if (modeGroup && modeGroup->title().contains("Measurement Mode")) {
        modeGroup->setTitle(tr("Measurement Mode"));
    }

    // Update thermocouple info if in thermocouple mode
    if (m_currentMode == MeasurementMode::THERMOCOUPLE_MODE) {
        updateThermocoupleInfo();
        updateThermocoupleCalculation();
    }

    // Update calculation to refresh labels
    updateCalculation();
}

void MainWindow::onLanguageChanged(QAction *action)
{
    QString language = "en";

    if (action == ui->actionRomanian) {
        language = "ro";
    } else if (action == ui->actionGerman) {
        language = "de";
    } else if (action == ui->actionFrench) {
        language = "fr";
    } else if (action == ui->actionSpanish) {
        language = "es";
    } else if (action == ui->actionRussian) {
        language = "ru";
    }

    switchLanguage(language);
}

void MainWindow::onAboutClicked()
{
    QMessageBox::about(this, tr("About Temperature Master Pro"),
        tr("<h2>Temperature Master Pro</h2>"
           "<p><b>Professional Temperature Measurement System</b></p>"
           "<p>Version 2.0.0</p>"
           "<p>A professional application for precision temperature measurements using "
           "RTD (Resistance Temperature Detectors) and Thermocouples, supporting multiple "
           "international standards.</p>"
           "<p><b>RTD Standards:</b></p>"
           "<ul>"
           "<li>IEC 60751 (International)</li>"
           "<li>GOST 6651 (Russian)</li>"
           "<li>ASTM E1137 (American)</li>"
           "<li>Legacy US</li>"
           "</ul>"
           "<p><b>Thermocouple Types:</b></p>"
           "<ul>"
           "<li>Type K, J, T, E, N (Base Metal)</li>"
           "<li>Type R, S, B (Noble Metal)</li>"
           "</ul>"
           "<hr>"
           "<p><b>Author:</b> Constantin Agavriloaie<br>"
           "<b>Position:</b> Engineer, Porsche Romania<br>"
           "<b>Email:</b> <a href='mailto:agavriloaie.constantin@gmail.com'>agavriloaie.constantin@gmail.com</a></p>"
           "<p>© 2025 Temperature Master Pro. All rights reserved.</p>"));
}

void MainWindow::onTechnicalDocClicked()
{
    // Determine which documentation file to open based on current language
    QString docFile;

    if (m_currentLanguage == "en") {
        docFile = "Technical_Documentation_EN.txt";
    } else if (m_currentLanguage == "ro") {
        docFile = "Technical_Documentation_RO.txt";
    } else if (m_currentLanguage == "de") {
        docFile = "Technical_Documentation_DE.txt";
    } else if (m_currentLanguage == "fr") {
        docFile = "Technical_Documentation_FR.txt";
    } else if (m_currentLanguage == "es") {
        docFile = "Technical_Documentation_ES.txt";
    } else if (m_currentLanguage == "ru") {
        docFile = "Technical_Documentation_RU.txt";
    } else {
        docFile = "Technical_Documentation_EN.txt"; // Fallback to English
    }

    // Try multiple locations to find the documentation file
    QStringList searchPaths;
    QString appDir = QApplication::applicationDirPath();

    // 1. Application directory (for deployed builds)
    searchPaths << appDir + "/" + docFile;

    // 2. Parent directory (common during development)
    searchPaths << appDir + "/../" + docFile;

    // 3. Two levels up (for Qt Creator build folders like build-RTD-Desktop-Debug)
    searchPaths << appDir + "/../../" + docFile;

    // 4. Three levels up
    searchPaths << appDir + "/../../../" + docFile;

    // 5. Source directory relative path (for development)
    searchPaths << appDir + "/../RTD/" + docFile;

    // Find the first existing file
    QString filePath;
    for (const QString& path : searchPaths) {
        QString canonicalPath = QFileInfo(path).canonicalFilePath();
        if (!canonicalPath.isEmpty() && QFile::exists(canonicalPath)) {
            filePath = canonicalPath;
            break;
        }
    }

    // Check if file was found
    if (filePath.isEmpty()) {
        QString searchInfo = tr("Searched in:\n");
        for (const QString& path : searchPaths) {
            searchInfo += "- " + QFileInfo(path).absoluteFilePath() + "\n";
        }

        QMessageBox::warning(this, tr("Error"),
            tr("Technical documentation file not found:\n%1\n\n%2").arg(docFile).arg(searchInfo));
        return;
    }

    // Read the documentation file content
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
            tr("Could not open technical documentation file:\n%1").arg(filePath));
        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString content = in.readAll();
    file.close();

    // Create and show the documentation dialog
    QString dialogTitle = tr("Technical Documentation");
    DocumentationDialog *dialog = new DocumentationDialog(dialogTitle, content, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::onShowGraphClicked()
{
    // Create and show the chart dialog
    ChartDialog *chartDialog = new ChartDialog(this);
    chartDialog->setAttribute(Qt::WA_DeleteOnClose);
    chartDialog->show();
}

void MainWindow::onShowTCGraphClicked()
{
    // Create and show the thermocouple chart dialog
    ThermocoupleChartDialog *tcChartDialog = new ThermocoupleChartDialog(this);
    tcChartDialog->setAttribute(Qt::WA_DeleteOnClose);
    tcChartDialog->show();
}

// ============================================================================
// MODE SELECTOR AND THERMOCOUPLE UI SETUP
// ============================================================================

void MainWindow::setupModeSelector()
{
    // Create mode selector group box
    QGroupBox *modeGroup = new QGroupBox(tr("Measurement Mode"), this);
    modeGroup->setMaximumHeight(80);  // Fixed height for mode selector
    QHBoxLayout *modeLayout = new QHBoxLayout();

    // Create radio buttons
    QRadioButton *rtdModeButton = new QRadioButton(tr("RTD (Resistance Temperature Detector)"), this);
    QRadioButton *thermocoupleModeButton = new QRadioButton(tr("Thermocouple"), this);

    // Set object names for later access
    rtdModeButton->setObjectName("radioRTDMode");
    thermocoupleModeButton->setObjectName("radioThermocoupleMode");

    // Create button group for mutual exclusion
    m_modeButtonGroup = new QButtonGroup(this);
    m_modeButtonGroup->addButton(rtdModeButton, static_cast<int>(MeasurementMode::RTD_MODE));
    m_modeButtonGroup->addButton(thermocoupleModeButton, static_cast<int>(MeasurementMode::THERMOCOUPLE_MODE));

    // Set RTD as default
    rtdModeButton->setChecked(true);

    // Add to layout
    modeLayout->addWidget(rtdModeButton);
    modeLayout->addWidget(thermocoupleModeButton);
    modeLayout->addStretch();

    modeGroup->setLayout(modeLayout);

    // Insert at the top of the main layout
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
    if (mainLayout) {
        mainLayout->insertWidget(0, modeGroup);
    }

    // Connect signal
    connect(m_modeButtonGroup, QOverload<int>::of(&QButtonGroup::idClicked),
            this, &MainWindow::onModeChanged);
}

void MainWindow::setupThermocoupleUI()
{
    // Find the first group box (RTD Configuration)
    QGroupBox *rtdGroup = ui->centralwidget->findChild<QGroupBox*>("groupBox");
    if (!rtdGroup) return;

    // Create thermocouple configuration group (initially hidden)
    QGroupBox *tcGroup = new QGroupBox(tr("Thermocouple Configuration"), this);
    tcGroup->setObjectName("groupBoxThermocouple");

    // Set size policy to match RTD configuration group (equal width)
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(0);
    tcGroup->setSizePolicy(sizePolicy);

    QFormLayout *tcLayout = new QFormLayout();
    tcLayout->setVerticalSpacing(12);  // Comfortable vertical spacing
    tcLayout->setHorizontalSpacing(10);

    // Thermocouple Type (editable combo box)
    QLabel *labelType = new QLabel(tr("Type:"), this);
    labelType->setObjectName("labelTCType");
    QComboBox *comboType = new QComboBox(this);
    comboType->setObjectName("comboTCType");
    comboType->addItem(tr("Type K (Chromel-Alumel)"));
    comboType->addItem(tr("Type J (Iron-Constantan)"));
    comboType->addItem(tr("Type T (Copper-Constantan)"));
    comboType->addItem(tr("Type E (Chromel-Constantan)"));
    comboType->addItem(tr("Type N (Nicrosil-Nisil)"));
    comboType->addItem(tr("Type S (Pt-Pt/Rh 10%)"));
    comboType->addItem(tr("Type R (Pt-Pt/Rh 13%)"));
    comboType->addItem(tr("Type B (Pt/Rh 30%-Pt/Rh 6%)"));
    comboType->setCurrentIndex(0);
    tcLayout->addRow(labelType, comboType);

    // Cold Junction Temperature (editable spinbox)
    QLabel *labelColdJunction = new QLabel(tr("Cold Junction:"), this);
    labelColdJunction->setObjectName("labelTCColdJunction");
    QDoubleSpinBox *spinColdJunction = new QDoubleSpinBox(this);
    spinColdJunction->setObjectName("spinColdJunction");
    spinColdJunction->setRange(-50.0, 100.0);
    spinColdJunction->setValue(25.0);
    spinColdJunction->setSuffix(" °C");
    spinColdJunction->setDecimals(2);
    tcLayout->addRow(labelColdJunction, spinColdJunction);

    // White background style for all fields
    QString infoStyle = "QLineEdit { "
                       "background-color: white; "
                       "color: #475569; "
                       "font-weight: 600; "
                       "border: 2px solid #cbd5e1; "
                       "border-radius: 6px; "
                       "padding: 7px 10px; "
                       "}";

    // Temperature Range (read-only line edit)
    QLabel *labelTempRange = new QLabel(tr("Temp. Range:"), this);
    labelTempRange->setObjectName("labelTCTempRange");
    QLineEdit *editRange = new QLineEdit(this);
    editRange->setObjectName("editTCRange");
    editRange->setReadOnly(true);
    editRange->setStyleSheet(infoStyle);
    tcLayout->addRow(labelTempRange, editRange);

    // Voltage Range (read-only line edit)
    QLabel *labelVoltRange = new QLabel(tr("Voltage Range:"), this);
    labelVoltRange->setObjectName("labelTCVoltRange");
    QLineEdit *editVoltRange = new QLineEdit(this);
    editVoltRange->setObjectName("editTCVoltRange");
    editVoltRange->setReadOnly(true);
    editVoltRange->setStyleSheet(infoStyle);
    tcLayout->addRow(labelVoltRange, editVoltRange);

    // Sensitivity (read-only line edit)
    QLabel *labelSensitivity = new QLabel(tr("Sensitivity:"), this);
    labelSensitivity->setObjectName("labelTCSensitivity");
    QLineEdit *editSensitivity = new QLineEdit(this);
    editSensitivity->setObjectName("editTCSensitivity");
    editSensitivity->setReadOnly(true);
    editSensitivity->setStyleSheet(infoStyle);
    tcLayout->addRow(labelSensitivity, editSensitivity);

    // Color Code (read-only line edit)
    QLabel *labelColorCode = new QLabel(tr("Color Code:"), this);
    labelColorCode->setObjectName("labelTCColorCode");
    QLineEdit *editColorCode = new QLineEdit(this);
    editColorCode->setObjectName("editTCColorCode");
    editColorCode->setReadOnly(true);
    editColorCode->setStyleSheet(infoStyle);
    tcLayout->addRow(labelColorCode, editColorCode);

    // Common Applications (read-only line edit)
    QLabel *labelApplications = new QLabel(tr("Applications:"), this);
    labelApplications->setObjectName("labelTCApplications");
    QLineEdit *editApplications = new QLineEdit(this);
    editApplications->setObjectName("editTCApplications");
    editApplications->setReadOnly(true);
    editApplications->setStyleSheet(infoStyle);
    tcLayout->addRow(labelApplications, editApplications);

    tcGroup->setLayout(tcLayout);
    tcGroup->setVisible(false); // Hide initially

    // Find the horizontal layout that contains both configuration groups
    QHBoxLayout *horizontalLayout = ui->centralwidget->findChild<QHBoxLayout*>("horizontalLayout");
    if (horizontalLayout) {
        // Insert thermocouple group at position 0 (same as RTD group position)
        // Both groups will occupy the same space and switch visibility
        horizontalLayout->insertWidget(0, tcGroup);
    }

    // Connect signals
    connect(comboType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onThermocoupleTypeChanged);
    connect(spinColdJunction, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onColdJunctionTempChanged);

    // Initialize thermocouple info
    onThermocoupleTypeChanged(0);
}

// ============================================================================
// MODE SWITCHING
// ============================================================================

void MainWindow::onModeChanged(int mode)
{
    switchMode(static_cast<MeasurementMode>(mode));
}

void MainWindow::switchMode(MeasurementMode mode)
{
    m_currentMode = mode;

    if (mode == MeasurementMode::RTD_MODE) {
        showRTDControls(true);
        showThermocoupleControls(false);
        updateCalculation();
    } else {
        showRTDControls(false);
        showThermocoupleControls(true);
        updateThermocoupleCalculation();
    }
}

void MainWindow::showRTDControls(bool show)
{
    // Find RTD-specific group boxes
    QGroupBox *rtdGroup = ui->centralwidget->findChild<QGroupBox*>("groupBox");
    if (rtdGroup) rtdGroup->setVisible(show);
}

void MainWindow::showThermocoupleControls(bool show)
{
    // Find thermocouple group box
    QGroupBox *tcGroup = ui->centralwidget->findChild<QGroupBox*>("groupBoxThermocouple");
    if (tcGroup) tcGroup->setVisible(show);

    // Update conversion mode label
    if (show) {
        ui->comboConversionMode->clear();
        ui->comboConversionMode->addItem("Temperature → Voltage");
        ui->comboConversionMode->addItem("Voltage → Temperature");
    } else {
        ui->comboConversionMode->clear();
        ui->comboConversionMode->addItem("Temperature → Resistance");
        ui->comboConversionMode->addItem("Resistance → Temperature");
    }
}

// ============================================================================
// THERMOCOUPLE SLOTS
// ============================================================================

void MainWindow::onThermocoupleTypeChanged(int index)
{
    try {
        // Set thermocouple type
        m_thermocoupleCalculator.setType(static_cast<ThermocoupleType>(index));

        // Update spinbox ranges to match new thermocouple type
        double minTemp = m_thermocoupleCalculator.getMinTemperature();
        double maxTemp = m_thermocoupleCalculator.getMaxTemperature();

        // Adjust input value if it's outside the new range
        if (ui->comboConversionMode->currentIndex() == 0) {
            // Temperature to Voltage mode
            ui->doubleSpinInput->setRange(minTemp, maxTemp);

            // If current value is outside new range, set to minimum valid value
            if (ui->doubleSpinInput->value() < minTemp) {
                ui->doubleSpinInput->setValue(minTemp);
            } else if (ui->doubleSpinInput->value() > maxTemp) {
                ui->doubleSpinInput->setValue(maxTemp);
            }
        } else {
            // Voltage to Temperature mode - update voltage range
            double minV = m_thermocoupleCalculator.getMinVoltage();
            double maxV = m_thermocoupleCalculator.getMaxVoltage();
            ui->doubleSpinInput->setRange(minV, maxV);

            // Adjust value if needed
            if (ui->doubleSpinInput->value() < minV) {
                ui->doubleSpinInput->setValue(minV);
            } else if (ui->doubleSpinInput->value() > maxV) {
                ui->doubleSpinInput->setValue(maxV);
            }
        }

        // Update info labels
        updateThermocoupleInfo();

        // Update calculation
        if (m_currentMode == MeasurementMode::THERMOCOUPLE_MODE) {
            updateThermocoupleCalculation();
        }
    } catch (const std::out_of_range& e) {
        // Handle initialization errors gracefully
        qWarning() << "Thermocouple type change error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error in thermocouple type change";
    }
}

void MainWindow::onColdJunctionTempChanged(double value)
{
    Q_UNUSED(value);

    // Update calculation
    if (m_currentMode == MeasurementMode::THERMOCOUPLE_MODE) {
        updateThermocoupleCalculation();
    }
}

QString MainWindow::getTranslatedColorCode(ThermocoupleType type)
{
    switch (type) {
        case ThermocoupleType::TYPE_K: return tr("Yellow");
        case ThermocoupleType::TYPE_J: return tr("Black");
        case ThermocoupleType::TYPE_T: return tr("Blue");
        case ThermocoupleType::TYPE_E: return tr("Purple");
        case ThermocoupleType::TYPE_N: return tr("Orange");
        case ThermocoupleType::TYPE_S: return tr("Green");
        case ThermocoupleType::TYPE_R: return tr("Green");
        case ThermocoupleType::TYPE_B: return tr("Gray");
        default: return tr("Unknown");
    }
}

QString MainWindow::getTranslatedApplications(ThermocoupleType type)
{
    switch (type) {
        case ThermocoupleType::TYPE_K:
            return tr("General purpose, wide range");
        case ThermocoupleType::TYPE_J:
            return tr("Iron/steel industry, reducing atmospheres");
        case ThermocoupleType::TYPE_T:
            return tr("Low temperature, cryogenics");
        case ThermocoupleType::TYPE_E:
            return tr("High output, oxidizing atmospheres");
        case ThermocoupleType::TYPE_N:
            return tr("High temperature, improved stability");
        case ThermocoupleType::TYPE_S:
        case ThermocoupleType::TYPE_R:
            return tr("High temperature, laboratory standards");
        case ThermocoupleType::TYPE_B:
            return tr("Very high temperature, glass manufacturing");
        default:
            return tr("Unknown");
    }
}

double MainWindow::getStandardLimitsOfError(ThermocoupleType type)
{
    // Standard Limits of Error per ASTM E230 / IEC 60584
    switch (type) {
        case ThermocoupleType::TYPE_K:
        case ThermocoupleType::TYPE_N:
            return 2.2;  // ±2.2°C or 0.75% (whichever is greater)
        case ThermocoupleType::TYPE_J:
            return 2.2;  // ±2.2°C or 0.75%
        case ThermocoupleType::TYPE_T:
            return 1.0;  // ±1.0°C or 0.75%
        case ThermocoupleType::TYPE_E:
            return 1.7;  // ±1.7°C or 0.5%
        case ThermocoupleType::TYPE_S:
        case ThermocoupleType::TYPE_R:
            return 1.5;  // ±1.5°C or 0.25%
        case ThermocoupleType::TYPE_B:
            return 0.5;  // ±0.5% (no fixed °C component)
        default:
            return 2.0;
    }
}

double MainWindow::getStandardLimitsPercent(ThermocoupleType type)
{
    // Standard Limits of Error (percent component)
    switch (type) {
        case ThermocoupleType::TYPE_K:
        case ThermocoupleType::TYPE_N:
        case ThermocoupleType::TYPE_J:
        case ThermocoupleType::TYPE_T:
            return 0.75;  // 0.75%
        case ThermocoupleType::TYPE_E:
            return 0.5;   // 0.5%
        case ThermocoupleType::TYPE_S:
        case ThermocoupleType::TYPE_R:
            return 0.25;  // 0.25%
        case ThermocoupleType::TYPE_B:
            return 0.5;   // 0.5%
        default:
            return 0.75;
    }
}

void MainWindow::updateThermocoupleInfo()
{
    try {
        // Find line edit widgets
        QLineEdit *editRange = ui->centralwidget->findChild<QLineEdit*>("editTCRange");
        QLineEdit *editVoltRange = ui->centralwidget->findChild<QLineEdit*>("editTCVoltRange");
        QLineEdit *editSensitivity = ui->centralwidget->findChild<QLineEdit*>("editTCSensitivity");
        QLineEdit *editColorCode = ui->centralwidget->findChild<QLineEdit*>("editTCColorCode");
        QLineEdit *editApplications = ui->centralwidget->findChild<QLineEdit*>("editTCApplications");

        // Update temperature range
        if (editRange) {
            double minT = m_thermocoupleCalculator.getMinTemperature();
            double maxT = m_thermocoupleCalculator.getMaxTemperature();
        editRange->setText(QString("%1°C %2 %3°C").arg(minT, 0, 'f', 0).arg(tr("to")).arg(maxT, 0, 'f', 0));
    }

    // Update voltage range
    if (editVoltRange) {
        double minV = m_thermocoupleCalculator.getMinVoltage();
        double maxV = m_thermocoupleCalculator.getMaxVoltage();
        editVoltRange->setText(QString("%1 %2 %3 mV").arg(minV, 0, 'f', 2).arg(tr("to")).arg(maxV, 0, 'f', 2));
    }

    // Update sensitivity
    if (editSensitivity) {
        double sens = m_thermocoupleCalculator.getSensitivity();
        editSensitivity->setText(QString("%1 µV/°C").arg(sens, 0, 'f', 1));
    }

    // Update color code (translated)
    if (editColorCode) {
        editColorCode->setText(getTranslatedColorCode(m_thermocoupleCalculator.getType()));
    }

    // Update applications (translated)
    if (editApplications) {
        editApplications->setText(getTranslatedApplications(m_thermocoupleCalculator.getType()));
    }

        // Update spinbox ranges
        QDoubleSpinBox *inputSpin = ui->doubleSpinInput;
        if (inputSpin && ui->comboConversionMode->currentIndex() == 0) {
            // Temperature to Voltage mode
            inputSpin->setRange(m_thermocoupleCalculator.getMinTemperature(),
                               m_thermocoupleCalculator.getMaxTemperature());
            inputSpin->setSuffix(" °C");
            inputSpin->setDecimals(2);
        }
    } catch (const std::out_of_range& e) {
        // Handle initialization errors gracefully
        qWarning() << "Thermocouple info update error:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error in thermocouple info update";
    }
}

void MainWindow::updateThermocoupleCalculation()
{
    QComboBox *comboType = ui->centralwidget->findChild<QComboBox*>("comboTCType");
    QDoubleSpinBox *spinColdJunction = ui->centralwidget->findChild<QDoubleSpinBox*>("spinColdJunction");

    if (!comboType || !spinColdJunction) return;

    double inputValue = ui->doubleSpinInput->value();
    double coldJunctionTemp = spinColdJunction->value();
    double outputValue = 0.0;

    try {
        if (ui->comboConversionMode->currentIndex() == 0) {
            // Temperature to Voltage
            outputValue = m_thermocoupleCalculator.temperatureToVoltage(inputValue);
            ui->doubleSpinOutput->setSuffix(" mV");
            ui->doubleSpinOutput->setDecimals(4);
            ui->doubleSpinInput->setSuffix(" °C");
            ui->doubleSpinInput->setDecimals(2);

            // Set reasonable range for temperature input
            ui->doubleSpinInput->setRange(m_thermocoupleCalculator.getMinTemperature(),
                                         m_thermocoupleCalculator.getMaxTemperature());
        } else {
            // Voltage to Temperature
            outputValue = m_thermocoupleCalculator.voltageToTemperature(inputValue, coldJunctionTemp);
            ui->doubleSpinOutput->setSuffix(" °C");
            ui->doubleSpinOutput->setDecimals(2);
            ui->doubleSpinInput->setSuffix(" mV");
            ui->doubleSpinInput->setDecimals(4);

            // Set reasonable range for voltage input
            ui->doubleSpinInput->setRange(m_thermocoupleCalculator.getMinVoltage(),
                                         m_thermocoupleCalculator.getMaxVoltage());
        }

        ui->doubleSpinOutput->setValue(outputValue);

        // Update info text with calculation-specific information
        QString info;

        if (ui->comboConversionMode->currentIndex() == 0) {
            // Temperature to Voltage mode - show voltage breakdown
            double hotJunctionVoltage = m_thermocoupleCalculator.temperatureToVoltage(inputValue);
            double coldJunctionVoltage = m_thermocoupleCalculator.temperatureToVoltage(coldJunctionTemp);
            double netVoltage = hotJunctionVoltage - coldJunctionVoltage;

            info = QString("CALCULATION DETAILS:\n"
                          "Hot Junction: %1°C → %2 mV\n"
                          "Cold Junction: %3°C → %4 mV\n"
                          "Net Output: %5 mV\n\n"
                          "ACCURACY:\n"
                          "Standard Limits: ±%6°C or ±%7%\n"
                          "Reference: NIST ITS-90\n\n"
                          "TECHNICAL:\n"
                          "Seebeck Coefficient: ~%8 µV/°C at %9°C")
                    .arg(inputValue, 0, 'f', 2)
                    .arg(hotJunctionVoltage, 0, 'f', 4)
                    .arg(coldJunctionTemp, 0, 'f', 2)
                    .arg(coldJunctionVoltage, 0, 'f', 4)
                    .arg(netVoltage, 0, 'f', 4)
                    .arg(getStandardLimitsOfError(m_thermocoupleCalculator.getType()), 0, 'f', 1)
                    .arg(getStandardLimitsPercent(m_thermocoupleCalculator.getType()), 0, 'f', 2)
                    .arg(m_thermocoupleCalculator.getSensitivity(), 0, 'f', 1)
                    .arg(inputValue, 0, 'f', 0);
        } else {
            // Voltage to Temperature mode - show temperature calculation details
            double coldJunctionVoltage = m_thermocoupleCalculator.temperatureToVoltage(coldJunctionTemp);
            double totalVoltage = inputValue + coldJunctionVoltage;

            info = QString("CALCULATION DETAILS:\n"
                          "Measured Voltage: %1 mV\n"
                          "Cold Junction Compensation: %2 mV (at %3°C)\n"
                          "Total Hot Junction Voltage: %4 mV\n"
                          "Calculated Temperature: %5°C\n\n"
                          "ACCURACY:\n"
                          "Standard Limits: ±%6°C or ±%7%\n"
                          "Reference: NIST ITS-90\n\n"
                          "TECHNICAL:\n"
                          "Seebeck Coefficient: ~%8 µV/°C at %9°C")
                    .arg(inputValue, 0, 'f', 4)
                    .arg(coldJunctionVoltage, 0, 'f', 4)
                    .arg(coldJunctionTemp, 0, 'f', 2)
                    .arg(totalVoltage, 0, 'f', 4)
                    .arg(outputValue, 0, 'f', 2)
                    .arg(getStandardLimitsOfError(m_thermocoupleCalculator.getType()), 0, 'f', 1)
                    .arg(getStandardLimitsPercent(m_thermocoupleCalculator.getType()), 0, 'f', 2)
                    .arg(m_thermocoupleCalculator.getSensitivity(), 0, 'f', 1)
                    .arg(outputValue, 0, 'f', 0);
        }

        ui->textInfo->setPlainText(info);
        ui->doubleSpinTolerance->hide();  // Hide tolerance spinbox in thermocouple mode

    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error"), tr("Calculation error: %1").arg(e.what()));
    }
}
