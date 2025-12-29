#include "thermocouplechartdialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QEvent>

ThermocoupleChartDialog::ThermocoupleChartDialog(QWidget *parent)
    : QDialog(parent)
    , m_calculator(new ThermocoupleCalculator())
    , m_nextColorIndex(0)
    , m_curveCount(0)
{
    setWindowTitle(tr("Thermocouple Voltage vs Temperature Curves"));
    resize(1200, 700);

    // Initialize calculator with default Type K
    m_calculator->setType(ThermocoupleType::TYPE_K);

    // Initialize color palette (same as RTD)
    m_colorPalette = {
        QColor(31, 119, 180),   // Blue
        QColor(255, 127, 14),   // Orange
        QColor(44, 160, 44),    // Green
        QColor(214, 39, 40),    // Red
        QColor(148, 103, 189),  // Purple
        QColor(140, 86, 75),    // Brown
        QColor(227, 119, 194),  // Pink
        QColor(127, 127, 127)   // Gray
    };

    setupUI();
    setupChart();
}

ThermocoupleChartDialog::~ThermocoupleChartDialog()
{
    delete m_calculator;
}

void ThermocoupleChartDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Control panel
    m_controlGroup = new QGroupBox(tr("Curve Configuration"));
    QHBoxLayout *controlLayout = new QHBoxLayout(m_controlGroup);

    // Left column: Type and Cold Junction
    QFormLayout *leftForm = new QFormLayout();

    m_typeLabel = new QLabel(tr("Thermocouple Type:"));
    m_typeCombo = new QComboBox();
    populateTypeCombo();
    leftForm->addRow(m_typeLabel, m_typeCombo);

    m_coldJunctionLabel = new QLabel(tr("Cold Junction:"));
    m_coldJunctionSpin = new QDoubleSpinBox();
    m_coldJunctionSpin->setRange(-50, 100);
    m_coldJunctionSpin->setValue(0);
    m_coldJunctionSpin->setSuffix(" °C");
    m_coldJunctionSpin->setToolTip(tr("Cold junction (reference) temperature"));
    leftForm->addRow(m_coldJunctionLabel, m_coldJunctionSpin);

    controlLayout->addLayout(leftForm);

    // Middle column: Temperature range
    QFormLayout *middleForm = new QFormLayout();

    m_minTempLabel = new QLabel(tr("Min Temp:"));
    m_minTempSpin = new QDoubleSpinBox();
    m_minTempSpin->setRange(-270, 2000);
    m_minTempSpin->setValue(0);
    m_minTempSpin->setSuffix(" °C");
    middleForm->addRow(m_minTempLabel, m_minTempSpin);

    m_maxTempLabel = new QLabel(tr("Max Temp:"));
    m_maxTempSpin = new QDoubleSpinBox();
    m_maxTempSpin->setRange(-270, 2000);
    m_maxTempSpin->setValue(1000);
    m_maxTempSpin->setSuffix(" °C");
    middleForm->addRow(m_maxTempLabel, m_maxTempSpin);

    controlLayout->addLayout(middleForm);

    // Display options - aligned to the right
    QVBoxLayout *displayLayout = new QVBoxLayout();
    displayLayout->addSpacing(10);  // Small top spacing for alignment

    m_showFormulaCheck = new QCheckBox(tr("Show Formula Curve"));
    m_showFormulaCheck->setChecked(true);
    displayLayout->addWidget(m_showFormulaCheck);

    displayLayout->addSpacing(5);  // Small spacing between checkboxes

    m_showReferenceCheck = new QCheckBox(tr("Show Reference Points (NIST ITS-90)"));
    m_showReferenceCheck->setChecked(true);
    displayLayout->addWidget(m_showReferenceCheck);

    displayLayout->addStretch();  // Push content to top

    controlLayout->addLayout(displayLayout);

    // Right column: Buttons
    QVBoxLayout *buttonLayout = new QVBoxLayout();

    m_generateBtn = new QPushButton(tr("Generate Curve"));
    m_generateBtn->setStyleSheet("QPushButton { background-color: #FF5722; color: white; font-weight: bold; padding: 8px; }");
    buttonLayout->addWidget(m_generateBtn);

    m_addComparisonBtn = new QPushButton(tr("Add Comparison"));
    buttonLayout->addWidget(m_addComparisonBtn);

    m_clearAllBtn = new QPushButton(tr("Clear All"));
    buttonLayout->addWidget(m_clearAllBtn);

    m_exportBtn = new QPushButton(tr("Export PNG"));
    buttonLayout->addWidget(m_exportBtn);

    m_exportCSVBtn = new QPushButton(tr("Export CSV"));
    buttonLayout->addWidget(m_exportCSVBtn);

    m_resetViewBtn = new QPushButton(tr("Reset View"));
    m_resetViewBtn->setStyleSheet("QPushButton { background-color: #607D8B; color: white; font-weight: bold; padding: 8px; }");
    buttonLayout->addWidget(m_resetViewBtn);

    buttonLayout->addStretch();

    controlLayout->addLayout(buttonLayout);

    mainLayout->addWidget(m_controlGroup);

    // Chart view with zoom and pan enabled
    m_chartView = new QChartView(this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setRubberBand(QChartView::RectangleRubberBand);  // Enable rubber band zoom
    mainLayout->addWidget(m_chartView);

    // Connect signals
    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ThermocoupleChartDialog::onTypeChanged);
    connect(m_generateBtn, &QPushButton::clicked, this, &ThermocoupleChartDialog::onGenerateClicked);
    connect(m_addComparisonBtn, &QPushButton::clicked, this, &ThermocoupleChartDialog::onAddComparisonClicked);
    connect(m_clearAllBtn, &QPushButton::clicked, this, &ThermocoupleChartDialog::onClearAllClicked);
    connect(m_exportBtn, &QPushButton::clicked, this, &ThermocoupleChartDialog::onExportPNGClicked);
    connect(m_exportCSVBtn, &QPushButton::clicked, this, &ThermocoupleChartDialog::onExportCSVClicked);
    connect(m_resetViewBtn, &QPushButton::clicked, this, &ThermocoupleChartDialog::onResetViewClicked);
    connect(m_showFormulaCheck, &QCheckBox::toggled, this, &ThermocoupleChartDialog::onShowFormulaToggled);
    connect(m_showReferenceCheck, &QCheckBox::toggled, this, &ThermocoupleChartDialog::onShowReferenceToggled);

    // Initialize with Type K
    onTypeChanged(0);
}

void ThermocoupleChartDialog::setupChart()
{
    m_chart = new QChart();
    m_chart->setTitle(tr("Thermocouple Voltage vs Temperature"));
    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignRight);

    // Setup axes
    m_axisX = new QValueAxis();
    m_axisX->setTitleText(tr("Temperature (°C)"));
    m_axisX->setLabelFormat("%.0f");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);

    m_axisY = new QValueAxis();
    m_axisY->setTitleText(tr("Voltage (mV)"));
    m_axisY->setLabelFormat("%.2f");
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    m_chartView->setChart(m_chart);
}

void ThermocoupleChartDialog::populateTypeCombo()
{
    m_typeCombo->addItem("Type K (Chromel-Alumel)", static_cast<int>(ThermocoupleType::TYPE_K));
    m_typeCombo->addItem("Type J (Iron-Constantan)", static_cast<int>(ThermocoupleType::TYPE_J));
    m_typeCombo->addItem("Type T (Copper-Constantan)", static_cast<int>(ThermocoupleType::TYPE_T));
    m_typeCombo->addItem("Type E (Chromel-Constantan)", static_cast<int>(ThermocoupleType::TYPE_E));
    m_typeCombo->addItem("Type N (Nicrosil-Nisil)", static_cast<int>(ThermocoupleType::TYPE_N));
    m_typeCombo->addItem("Type S (Pt10%Rh-Pt)", static_cast<int>(ThermocoupleType::TYPE_S));
    m_typeCombo->addItem("Type R (Pt13%Rh-Pt)", static_cast<int>(ThermocoupleType::TYPE_R));
    m_typeCombo->addItem("Type B (Pt30%Rh-Pt6%Rh)", static_cast<int>(ThermocoupleType::TYPE_B));
}

ThermocoupleChartDialog::TempRange ThermocoupleChartDialog::getTypeRange(ThermocoupleType type)
{
    switch (type) {
        case ThermocoupleType::TYPE_K:
            return {-200, 1372};
        case ThermocoupleType::TYPE_J:
            return {-210, 1200};
        case ThermocoupleType::TYPE_T:
            return {-270, 400};
        case ThermocoupleType::TYPE_E:
            return {-270, 1000};
        case ThermocoupleType::TYPE_N:
            return {-270, 1300};
        case ThermocoupleType::TYPE_S:
            return {-50, 1768};  // S can go lower than R/B
        case ThermocoupleType::TYPE_R:
            return {-50, 1768};  // R can go lower than B
        case ThermocoupleType::TYPE_B:
            return {200, 1820};  // B requires minimum 200°C due to low output below this
        default:
            return {0, 1000};
    }
}

QString ThermocoupleChartDialog::getTypeDisplayName(ThermocoupleType type)
{
    switch (type) {
        case ThermocoupleType::TYPE_K: return "Type K";
        case ThermocoupleType::TYPE_J: return "Type J";
        case ThermocoupleType::TYPE_T: return "Type T";
        case ThermocoupleType::TYPE_E: return "Type E";
        case ThermocoupleType::TYPE_N: return "Type N";
        case ThermocoupleType::TYPE_S: return "Type S";
        case ThermocoupleType::TYPE_R: return "Type R";
        case ThermocoupleType::TYPE_B: return "Type B";
        default: return "Unknown";
    }
}

void ThermocoupleChartDialog::onTypeChanged(int index)
{
    try {
        ThermocoupleType type = static_cast<ThermocoupleType>(m_typeCombo->itemData(index).toInt());
        TempRange range = getTypeRange(type);

        // Update temperature range spin boxes
        m_minTempSpin->setValue(range.min);
        m_maxTempSpin->setValue(range.max);

        // Update cold junction range
        // Cold junction is typically at room temperature (0-50°C), even for high-temp thermocouples
        // Type B is special - it has negligible output below 200°C, so cold junction is typically treated as 0V
        double coldJunctionMin = -50.0;  // Standard cold junction minimum
        double coldJunctionMax = 100.0;  // Standard cold junction maximum

        // For types that don't support negative temperatures in their calculation,
        // we still allow low cold junction temps but handle them specially in calculations
        if (type == ThermocoupleType::TYPE_B) {
            // Type B cold junction is typically 0-50°C, output is negligible at these temps
            coldJunctionMin = 0.0;
        }

        m_coldJunctionSpin->setRange(coldJunctionMin, coldJunctionMax);

        // Reset cold junction to 0°C if current value is out of new range
        if (m_coldJunctionSpin->value() < coldJunctionMin || m_coldJunctionSpin->value() > coldJunctionMax) {
            m_coldJunctionSpin->setValue(0.0);
        }
    } catch (const std::exception& e) {
        qWarning() << "Error changing thermocouple type:" << e.what();
    }
}

void ThermocoupleChartDialog::onGenerateClicked()
{
    // Clear existing curves
    m_chart->removeAllSeries();
    m_curveCount = 0;
    m_nextColorIndex = 0;
    m_formulaSeries.clear();
    m_referenceSeries.clear();

    // Get parameters
    ThermocoupleType type = static_cast<ThermocoupleType>(m_typeCombo->currentData().toInt());
    double coldJunctionTemp = m_coldJunctionSpin->value();

    QColor color = getNextColor();

    // Add formula curve if checked
    if (m_showFormulaCheck->isChecked()) {
        QString label = QString("%1 (CJ: %2°C)")
                            .arg(getTypeDisplayName(type))
                            .arg(coldJunctionTemp, 0, 'f', 0);
        addCurve(label, type, coldJunctionTemp, color, true);
    }

    // Add reference points if checked
    if (m_showReferenceCheck->isChecked()) {
        QString refLabel = QString("%1 NIST (CJ: %2°C)")
                              .arg(getTypeDisplayName(type))
                              .arg(coldJunctionTemp, 0, 'f', 0);
        addReferencePoints(refLabel, type, coldJunctionTemp, color);
    }
}

void ThermocoupleChartDialog::onAddComparisonClicked()
{
    // Get parameters
    ThermocoupleType type = static_cast<ThermocoupleType>(m_typeCombo->currentData().toInt());
    double coldJunctionTemp = m_coldJunctionSpin->value();

    QColor color = getNextColor();

    // Add formula curve if checked
    if (m_showFormulaCheck->isChecked()) {
        QString label = QString("%1 (CJ: %2°C)")
                            .arg(getTypeDisplayName(type))
                            .arg(coldJunctionTemp, 0, 'f', 0);
        addCurve(label, type, coldJunctionTemp, color);
    }

    // Add reference points if checked
    if (m_showReferenceCheck->isChecked()) {
        QString refLabel = QString("%1 NIST (CJ: %2°C)")
                              .arg(getTypeDisplayName(type))
                              .arg(coldJunctionTemp, 0, 'f', 0);
        addReferencePoints(refLabel, type, coldJunctionTemp, color);
    }
}

void ThermocoupleChartDialog::addCurve(const QString &label, ThermocoupleType type,
                                       double coldJunctionTemp, const QColor &color, bool isPrimary)
{
    Q_UNUSED(isPrimary);
    try {
        QLineSeries *series = new QLineSeries();
        series->setName(label);

        double minTemp = m_minTempSpin->value();
        double maxTemp = m_maxTempSpin->value();
        double step = (maxTemp - minTemp) / 200.0; // 200 points

        // Configure calculator
        m_calculator->setType(type);

        // Calculate cold junction voltage (constant for this curve)
        // For Type B, output is negligible below 200°C, so we use 0 for low cold junction temps
        double coldJunctionVoltage = 0.0;
        if (type == ThermocoupleType::TYPE_B && coldJunctionTemp < 200.0) {
            // Type B has negligible output below 200°C, treat as zero
            coldJunctionVoltage = 0.0;
        } else if ((type == ThermocoupleType::TYPE_S || type == ThermocoupleType::TYPE_R) && coldJunctionTemp < -50.0) {
            // Type S/R don't support below -50°C, treat as zero
            coldJunctionVoltage = 0.0;
        } else {
            coldJunctionVoltage = m_calculator->temperatureToVoltage(coldJunctionTemp);
        }

        // Calculate points
        // Measured voltage = V(hot junction) - V(cold junction)
        for (double t = minTemp; t <= maxTemp; t += step) {
            double hotJunctionVoltage = m_calculator->temperatureToVoltage(t);
            double measuredVoltage = hotJunctionVoltage - coldJunctionVoltage;
            series->append(t, measuredVoltage);
        }

        // Style the series
        QPen pen = series->pen();
        pen.setWidth(2);
        pen.setColor(color);
        series->setPen(pen);

        m_chart->addSeries(series);
        series->attachAxis(m_axisX);
        series->attachAxis(m_axisY);

        m_formulaSeries.append(series);
        m_curveCount++;
        updateAxesRanges();
    } catch (const std::out_of_range& e) {
        QMessageBox::warning(this, tr("Error"),
            tr("Temperature out of range: %1\nPlease check your temperature settings.").arg(e.what()));
    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"),
            tr("Error generating curve: %1").arg(e.what()));
    }
}

void ThermocoupleChartDialog::updateAxesRanges()
{
    m_axisX->setRange(m_minTempSpin->value(), m_maxTempSpin->value());

    // Auto-scale Y axis based on all series
    if (m_chart->series().isEmpty()) return;

    double minV = std::numeric_limits<double>::max();
    double maxV = std::numeric_limits<double>::lowest();

    foreach (QAbstractSeries *series, m_chart->series()) {
        if (QLineSeries *lineSeries = qobject_cast<QLineSeries *>(series)) {
            foreach (const QPointF &point, lineSeries->points()) {
                minV = qMin(minV, point.y());
                maxV = qMax(maxV, point.y());
            }
        } else if (QScatterSeries *scatterSeries = qobject_cast<QScatterSeries *>(series)) {
            foreach (const QPointF &point, scatterSeries->points()) {
                minV = qMin(minV, point.y());
                maxV = qMax(maxV, point.y());
            }
        }
    }

    double margin = (maxV - minV) * 0.1;
    m_axisY->setRange(minV - margin, maxV + margin);

    // Store original ranges for reset functionality
    m_originalMinX = m_axisX->min();
    m_originalMaxX = m_axisX->max();
    m_originalMinY = m_axisY->min();
    m_originalMaxY = m_axisY->max();
}

void ThermocoupleChartDialog::addReferencePoints(const QString &label, ThermocoupleType type,
                                                 double coldJunctionTemp, const QColor &color)
{
    try {
        double minTemp = m_minTempSpin->value();
        double maxTemp = m_maxTempSpin->value();

        // Get NIST reference table points
        m_calculator->setType(type);

        // Calculate cold junction voltage
        double coldJunctionVoltage = 0.0;
        if (type == ThermocoupleType::TYPE_B && coldJunctionTemp < 200.0) {
            coldJunctionVoltage = 0.0;
        } else if ((type == ThermocoupleType::TYPE_S || type == ThermocoupleType::TYPE_R) && coldJunctionTemp < -50.0) {
            coldJunctionVoltage = 0.0;
        } else {
            coldJunctionVoltage = m_calculator->temperatureToVoltage(coldJunctionTemp);
        }

        // Get reference points from NIST tables
        QVector<QPair<double, double>> points =
            ThermocoupleReferenceData::getTablePoints(type, minTemp, maxTemp, coldJunctionVoltage);

        if (points.isEmpty()) {
            return; // No data in this range
        }

        QScatterSeries *series = new QScatterSeries();
        series->setName(label);
        series->setMarkerSize(8.0);
        series->setColor(color);
        series->setBorderColor(color.darker(120));

        for (const auto &point : points) {
            series->append(point.first, point.second);
        }

        m_chart->addSeries(series);
        series->attachAxis(m_axisX);
        series->attachAxis(m_axisY);

        m_referenceSeries.append(series);
    } catch (const std::exception& e) {
        qWarning() << "Error adding reference points:" << e.what();
    }
}

void ThermocoupleChartDialog::updateSeriesVisibility()
{
    // Update formula series visibility
    for (QLineSeries *series : m_formulaSeries) {
        series->setVisible(m_showFormulaCheck->isChecked());
    }

    // Update reference series visibility
    for (QScatterSeries *series : m_referenceSeries) {
        series->setVisible(m_showReferenceCheck->isChecked());
    }
}

void ThermocoupleChartDialog::onShowFormulaToggled(bool checked)
{
    Q_UNUSED(checked);
    updateSeriesVisibility();
}

void ThermocoupleChartDialog::onShowReferenceToggled(bool checked)
{
    Q_UNUSED(checked);
    updateSeriesVisibility();
}

void ThermocoupleChartDialog::onResetViewClicked()
{
    // Reset to original axis ranges
    m_axisX->setRange(m_originalMinX, m_originalMaxX);
    m_axisY->setRange(m_originalMinY, m_originalMaxY);
}

void ThermocoupleChartDialog::onClearAllClicked()
{
    m_chart->removeAllSeries();
    m_curveCount = 0;
    m_nextColorIndex = 0;
    m_formulaSeries.clear();
    m_referenceSeries.clear();
}

void ThermocoupleChartDialog::onExportPNGClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Export Chart as PNG"),
                                                    "Thermocouple_Curve.png",
                                                    tr("PNG Images (*.png)"));
    if (!fileName.isEmpty()) {
        QPixmap pixmap = m_chartView->grab();
        if (pixmap.save(fileName)) {
            QMessageBox::information(this, tr("Success"),
                                   tr("Chart exported successfully!"));
        } else {
            QMessageBox::warning(this, tr("Error"),
                               tr("Failed to export chart."));
        }
    }
}

void ThermocoupleChartDialog::onExportCSVClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Export Data as CSV"),
                                                    "Thermocouple_Data.csv",
                                                    tr("CSV Files (*.csv)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Failed to create CSV file."));
        return;
    }

    QTextStream out(&file);

    // Get current configuration
    ThermocoupleType type = static_cast<ThermocoupleType>(m_typeCombo->currentData().toInt());
    QString typeDisplayName = getTypeDisplayName(type);
    double coldJunctionTemp = m_coldJunctionSpin->value();
    double minTemp = m_minTempSpin->value();
    double maxTemp = m_maxTempSpin->value();

    // Write header with configuration info
    out << "Thermocouple Data Export\n";
    out << "Type," << typeDisplayName << "\n";
    out << "Cold Junction Temperature," << coldJunctionTemp << " degC\n";
    out << "Temperature Range," << minTemp << " to " << maxTemp << " degC\n";
    out << "\n";

    // Write column headers
    out << "Temperature (degC),";
    out << "Voltage Formula (mV),";
    out << "Voltage Reference NIST ITS-90 (mV),";
    out << "Absolute Difference (mV),";
    out << "Percentage Difference (%),";
    out << "\n";

    // Configure calculator
    m_calculator->setType(type);

    // Calculate cold junction voltage
    double coldJunctionVoltage = 0.0;
    if (type == ThermocoupleType::TYPE_B && coldJunctionTemp < 200.0) {
        coldJunctionVoltage = 0.0;
    } else if ((type == ThermocoupleType::TYPE_S || type == ThermocoupleType::TYPE_R) && coldJunctionTemp < -50.0) {
        coldJunctionVoltage = 0.0;
    } else {
        try {
            coldJunctionVoltage = m_calculator->temperatureToVoltage(coldJunctionTemp);
        } catch (...) {
            coldJunctionVoltage = 0.0;
        }
    }

    // Generate data points (every 50°C for wide ranges, 10°C for narrow ranges)
    double tempRange = maxTemp - minTemp;
    double step = (tempRange > 500) ? 50.0 : 10.0;
    int rowCount = 0;

    for (double temp = minTemp; temp <= maxTemp; temp += step) {
        try {
            // Calculate formula voltage
            double hotJunctionVoltage = m_calculator->temperatureToVoltage(temp);
            double voltageFormula = hotJunctionVoltage - coldJunctionVoltage;

            out << QString::number(temp, 'f', 1) << ",";
            out << QString::number(voltageFormula, 'f', 4) << ",";

            // Get reference voltage from NIST tables
            QVector<QPair<double, double>> refPoints =
                ThermocoupleReferenceData::getTablePoints(type, temp, temp, coldJunctionVoltage);

            if (!refPoints.isEmpty()) {
                double voltageReference = refPoints[0].second;
                double absDiff = voltageFormula - voltageReference;
                double percDiff = (voltageReference != 0) ? (absDiff / qAbs(voltageReference)) * 100.0 : 0.0;

                out << QString::number(voltageReference, 'f', 4) << ",";
                out << QString::number(absDiff, 'f', 4) << ",";
                out << QString::number(percDiff, 'f', 6) << ",";
            } else {
                out << "N/A,N/A,N/A,";
            }

            out << "\n";
            rowCount++;
        } catch (const std::exception& e) {
            // Skip temperatures out of range
            qWarning() << "Skipping temperature" << temp << ":" << e.what();
        }
    }

    file.close();

    QMessageBox::information(this, tr("Success"),
                           tr("CSV data exported successfully!\n%1 rows written.").arg(rowCount));
}

QColor ThermocoupleChartDialog::getNextColor()
{
    QColor color = m_colorPalette[m_nextColorIndex % m_colorPalette.size()];
    m_nextColorIndex++;
    return color;
}

void ThermocoupleChartDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QDialog::changeEvent(event);
}

void ThermocoupleChartDialog::retranslateUi()
{
    // Update window title
    setWindowTitle(tr("Thermocouple Voltage vs Temperature Curves"));

    // Update group box
    m_controlGroup->setTitle(tr("Curve Configuration"));

    // Update form labels
    m_typeLabel->setText(tr("Thermocouple Type:"));
    m_coldJunctionLabel->setText(tr("Cold Junction:"));
    m_minTempLabel->setText(tr("Min Temp:"));
    m_maxTempLabel->setText(tr("Max Temp:"));

    // Update chart and axes
    m_chart->setTitle(tr("Thermocouple Voltage vs Temperature"));
    m_axisX->setTitleText(tr("Temperature (°C)"));
    m_axisY->setTitleText(tr("Voltage (mV)"));

    // Update buttons
    m_generateBtn->setText(tr("Generate Curve"));
    m_addComparisonBtn->setText(tr("Add Comparison"));
    m_clearAllBtn->setText(tr("Clear All"));
    m_exportBtn->setText(tr("Export PNG"));
    m_exportCSVBtn->setText(tr("Export CSV"));
    m_resetViewBtn->setText(tr("Reset View"));

    // Update checkboxes
    m_showFormulaCheck->setText(tr("Show Formula Curve"));
    m_showReferenceCheck->setText(tr("Show Reference Points (NIST ITS-90)"));

    // Update tooltips
    m_coldJunctionSpin->setToolTip(tr("Cold junction (reference) temperature"));
}
