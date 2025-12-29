#include "chartdialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QGraphicsLayout>
#include <QTextStream>
#include <QFile>
#include <QEvent>

ChartDialog::ChartDialog(QWidget *parent)
    : QDialog(parent)
    , m_calculator(new RTDCalculator())
    , m_nextColorIndex(0)
    , m_curveCount(0)
    , m_toleranceBand(nullptr)
{
    setWindowTitle(tr("RTD Resistance vs Temperature Curves"));
    resize(1200, 700);

    // Initialize color palette
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

ChartDialog::~ChartDialog()
{
    delete m_calculator;
}

void ChartDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Control panel
    m_controlGroup = new QGroupBox(tr("Curve Configuration"));
    QHBoxLayout *controlLayout = new QHBoxLayout(m_controlGroup);

    // Left column: Material and Type selection
    QFormLayout *leftForm = new QFormLayout();

    m_materialLabel = new QLabel(tr("Material:"));
    m_materialCombo = new QComboBox();
    populateMaterialCombo();
    leftForm->addRow(m_materialLabel, m_materialCombo);

    m_typeLabel = new QLabel(tr("Type:"));
    m_typeCombo = new QComboBox();
    leftForm->addRow(m_typeLabel, m_typeCombo);

    m_standardLabel = new QLabel(tr("Standard:"));
    m_standardCombo = new QComboBox();
    leftForm->addRow(m_standardLabel, m_standardCombo);

    controlLayout->addLayout(leftForm);

    // Middle column: Temperature range
    QFormLayout *middleForm = new QFormLayout();

    m_minTempLabel = new QLabel(tr("Min Temp:"));
    m_minTempSpin = new QDoubleSpinBox();
    m_minTempSpin->setRange(-273.15, 2000);
    m_minTempSpin->setValue(-200);
    m_minTempSpin->setSuffix(" °C");
    middleForm->addRow(m_minTempLabel, m_minTempSpin);

    m_maxTempLabel = new QLabel(tr("Max Temp:"));
    m_maxTempSpin = new QDoubleSpinBox();
    m_maxTempSpin->setRange(-273.15, 2000);
    m_maxTempSpin->setValue(850);
    m_maxTempSpin->setSuffix(" °C");
    middleForm->addRow(m_maxTempLabel, m_maxTempSpin);

    m_toleranceBandCheck = new QCheckBox(tr("Show Tolerance Band"));
    middleForm->addRow("", m_toleranceBandCheck);

    m_showFormulaCheck = new QCheckBox(tr("Show Formula Curve"));
    m_showFormulaCheck->setChecked(true); // Default to showing formula
    middleForm->addRow("", m_showFormulaCheck);

    m_showReferenceCheck = new QCheckBox(tr("Show Reference Points (IEC 60751)"));
    m_showReferenceCheck->setChecked(true); // Default to showing reference
    middleForm->addRow("", m_showReferenceCheck);

    controlLayout->addLayout(middleForm);

    // Right column: Buttons
    QVBoxLayout *buttonLayout = new QVBoxLayout();

    m_generateBtn = new QPushButton(tr("Generate Curve"));
    m_generateBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 8px; }");
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
    connect(m_materialCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ChartDialog::onMaterialChanged);
    connect(m_generateBtn, &QPushButton::clicked, this, &ChartDialog::onGenerateClicked);
    connect(m_addComparisonBtn, &QPushButton::clicked, this, &ChartDialog::onAddComparisonClicked);
    connect(m_clearAllBtn, &QPushButton::clicked, this, &ChartDialog::onClearAllClicked);
    connect(m_exportBtn, &QPushButton::clicked, this, &ChartDialog::onExportPNGClicked);
    connect(m_exportCSVBtn, &QPushButton::clicked, this, &ChartDialog::onExportCSVClicked);
    connect(m_resetViewBtn, &QPushButton::clicked, this, &ChartDialog::onResetViewClicked);
    connect(m_toleranceBandCheck, &QCheckBox::toggled, this, &ChartDialog::onToleranceBandToggled);
    connect(m_showFormulaCheck, &QCheckBox::toggled, this, &ChartDialog::onShowFormulaToggled);
    connect(m_showReferenceCheck, &QCheckBox::toggled, this, &ChartDialog::onShowReferenceToggled);

    // Initialize with Platinum
    onMaterialChanged(0);
}

void ChartDialog::setupChart()
{
    m_chart = new QChart();
    m_chart->setTitle(tr("RTD Resistance vs Temperature"));
    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignRight);

    // Setup axes
    m_axisX = new QValueAxis();
    m_axisX->setTitleText(tr("Temperature (°C)"));
    m_axisX->setLabelFormat("%.0f");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);

    m_axisY = new QValueAxis();
    m_axisY->setTitleText(tr("Resistance (Ω)"));
    m_axisY->setLabelFormat("%.2f");
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    m_chartView->setChart(m_chart);
}

void ChartDialog::populateMaterialCombo()
{
    m_materialCombo->addItem(tr("Platinum (Pt)"), "Platinum");
    m_materialCombo->addItem(tr("Copper (Cu)"), "Copper");
    m_materialCombo->addItem(tr("Nickel (Ni)"), "Nickel");
}

void ChartDialog::populateTypeCombo(const QString &material)
{
    m_typeCombo->clear();

    if (material == "Platinum") {
        m_typeCombo->addItem("Pt100 (100 Ω)", 100.0);
        m_typeCombo->addItem("Pt500 (500 Ω)", 500.0);
        m_typeCombo->addItem("Pt1000 (1000 Ω)", 1000.0);
    } else if (material == "Copper") {
        m_typeCombo->addItem("Cu50 (50 Ω)", 50.0);
        m_typeCombo->addItem("Cu100 (100 Ω)", 100.0);
    } else if (material == "Nickel") {
        m_typeCombo->addItem("Ni100 (100 Ω)", 100.0);
        m_typeCombo->addItem("Ni120 (120 Ω)", 120.0);
        m_typeCombo->addItem("Ni1000 (1000 Ω)", 1000.0);
    }
}

void ChartDialog::populateStandardCombo(const QString &material)
{
    m_standardCombo->clear();

    if (material == "Platinum") {
        m_standardCombo->addItem("IEC 60751 (α=0.00385)", "IEC");
        m_standardCombo->addItem("ASTM E1137 (α=0.00385)", "ASTM");
        m_standardCombo->addItem("GOST 6651 (α=0.00391)", "GOST_391");
        m_standardCombo->addItem("US Legacy (α=0.00392)", "US");
    } else if (material == "Copper") {
        m_standardCombo->addItem("GOST 6651 (α=0.00426)", "GOST");
        m_standardCombo->addItem("IEC 60751 (α=0.00427)", "IEC");
    } else if (material == "Nickel") {
        m_standardCombo->addItem("DIN 43760 (α=0.00617)", "DIN_617");
        m_standardCombo->addItem("DIN 43760 (α=0.00672)", "DIN_672");
    }
}

AlphaCoefficient ChartDialog::getAlphaFromStandard(const QString &material, const QString &standard)
{
    if (material == "Platinum") {
        if (standard == "IEC" || standard == "ASTM") {
            return AlphaCoefficient::PT_385;
        } else if (standard == "GOST_391") {
            return AlphaCoefficient::PT_391;
        } else if (standard == "US") {
            return AlphaCoefficient::PT_392;
        }
    } else if (material == "Copper") {
        if (standard == "GOST") {
            return AlphaCoefficient::CU_426;
        } else if (standard == "IEC") {
            return AlphaCoefficient::CU_427;
        }
    } else if (material == "Nickel") {
        if (standard == "DIN_617") {
            return AlphaCoefficient::NI_617;
        } else if (standard == "DIN_672") {
            return AlphaCoefficient::NI_672;
        }
    }

    return AlphaCoefficient::PT_385; // Default
}

void ChartDialog::onMaterialChanged(int index)
{
    QString material = m_materialCombo->itemData(index).toString();
    populateTypeCombo(material);
    populateStandardCombo(material);

    // Update temperature range based on material
    if (material == "Platinum") {
        m_minTempSpin->setValue(-200);
        m_maxTempSpin->setValue(850);
    } else if (material == "Copper") {
        m_minTempSpin->setValue(-50);
        m_maxTempSpin->setValue(150);
    } else if (material == "Nickel") {
        m_minTempSpin->setValue(-60);
        m_maxTempSpin->setValue(180);
    }
}

void ChartDialog::onGenerateClicked()
{
    // Clear existing curves
    m_chart->removeAllSeries();
    m_curveCount = 0;
    m_nextColorIndex = 0;
    m_toleranceBand = nullptr;
    m_formulaSeries.clear();
    m_referenceSeries.clear();

    // Get parameters
    QString material = m_materialCombo->currentData().toString();
    double r0 = m_typeCombo->currentData().toDouble();
    QString standardKey = m_standardCombo->currentData().toString();
    AlphaCoefficient alpha = getAlphaFromStandard(material, standardKey);

    QString label = QString("%1 %2")
                        .arg(m_typeCombo->currentText().split(" ").first())
                        .arg(m_standardCombo->currentText().split(" ").first());

    QColor color = getNextColor();

    // Add formula curve if enabled
    if (m_showFormulaCheck->isChecked()) {
        addCurve(label, r0, alpha, color, true);
    }

    // Add reference points if enabled (only for Pt100 with IEC standard)
    if (m_showReferenceCheck->isChecked() && material == "Platinum") {
        double r0Factor = r0 / 100.0; // Scale factor for Pt500, Pt1000
        addReferencePoints(label + " (IEC 60751)", r0Factor, color);
    }
}

void ChartDialog::onAddComparisonClicked()
{
    // Get parameters
    QString material = m_materialCombo->currentData().toString();
    double r0 = m_typeCombo->currentData().toDouble();
    QString standardKey = m_standardCombo->currentData().toString();
    AlphaCoefficient alpha = getAlphaFromStandard(material, standardKey);

    QString label = QString("%1 %2")
                        .arg(m_typeCombo->currentText().split(" ").first())
                        .arg(m_standardCombo->currentText().split(" ").first());

    QColor color = getNextColor();

    // Add formula curve if enabled
    if (m_showFormulaCheck->isChecked()) {
        addCurve(label, r0, alpha, color, false);
    }

    // Add reference points if enabled (only for Platinum)
    if (m_showReferenceCheck->isChecked() && material == "Platinum") {
        double r0Factor = r0 / 100.0;
        addReferencePoints(label + " (IEC 60751)", r0Factor, color);
    }
}

void ChartDialog::addCurve(const QString &label, double r0, AlphaCoefficient alpha,
                           const QColor &color, bool isPrimary)
{
    QLineSeries *series = new QLineSeries();
    series->setName(label);

    double minTemp = m_minTempSpin->value();
    double maxTemp = m_maxTempSpin->value();
    double step = (maxTemp - minTemp) / 200.0; // 200 points

    // Configure calculator
    m_calculator->setNominalResistance(r0);
    m_calculator->setAlphaCoefficient(alpha);

    // Calculate points
    for (double t = minTemp; t <= maxTemp; t += step) {
        double r = m_calculator->temperatureToResistance(t);
        series->append(t, r);
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

    // Add tolerance band for primary curve if enabled
    if (isPrimary && m_toleranceBandCheck->isChecked()) {
        onToleranceBandToggled(true);
    }
}

void ChartDialog::updateAxesRanges()
{
    m_axisX->setRange(m_minTempSpin->value(), m_maxTempSpin->value());

    // Auto-scale Y axis based on all series
    if (m_chart->series().isEmpty()) return;

    double minR = std::numeric_limits<double>::max();
    double maxR = std::numeric_limits<double>::lowest();

    foreach (QAbstractSeries *series, m_chart->series()) {
        if (QLineSeries *lineSeries = qobject_cast<QLineSeries *>(series)) {
            foreach (const QPointF &point, lineSeries->points()) {
                minR = qMin(minR, point.y());
                maxR = qMax(maxR, point.y());
            }
        }
    }

    double margin = (maxR - minR) * 0.1;
    m_axisY->setRange(minR - margin, maxR + margin);

    // Store original ranges for reset functionality
    m_originalMinX = m_axisX->min();
    m_originalMaxX = m_axisX->max();
    m_originalMinY = m_axisY->min();
    m_originalMaxY = m_axisY->max();
}

void ChartDialog::onClearAllClicked()
{
    m_chart->removeAllSeries();
    m_curveCount = 0;
    m_nextColorIndex = 0;
    m_toleranceBand = nullptr;
}

void ChartDialog::onExportPNGClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Export Chart as PNG"),
                                                    "RTD_Curve.png",
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

void ChartDialog::onExportCSVClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Export Data as CSV"),
                                                    "RTD_Data.csv",
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
    QString material = m_materialCombo->currentText();
    double r0 = m_typeCombo->currentData().toDouble();
    QString standard = m_standardCombo->currentText();
    AlphaCoefficient alpha = getAlphaFromStandard(material, standard);
    double minTemp = m_minTempSpin->value();
    double maxTemp = m_maxTempSpin->value();

    // Convert AlphaCoefficient enum to double value
    auto alphaToValue = [](AlphaCoefficient alpha) -> double {
        switch (alpha) {
            case AlphaCoefficient::PT_385: return 0.00385;
            case AlphaCoefficient::PT_391: return 0.00391;
            case AlphaCoefficient::PT_392: return 0.00392;
            case AlphaCoefficient::CU_426: return 0.00426;
            case AlphaCoefficient::CU_427: return 0.00427;
            case AlphaCoefficient::CU_428: return 0.00428;
            case AlphaCoefficient::NI_617: return 0.00617;
            case AlphaCoefficient::NI_618: return 0.00618;
            case AlphaCoefficient::NI_672: return 0.00672;
            default: return 0.00385;
        }
    };

    // Write header with configuration info
    out << "RTD Data Export\n";
    out << "Material," << material << "\n";
    out << "R0," << r0 << " Ohm\n";
    out << "Standard," << standard << "\n";
    out << "Alpha," << QString::number(alphaToValue(alpha), 'f', 8) << "\n";
    out << "Temperature Range," << minTemp << " to " << maxTemp << " degC\n";
    out << "\n";

    // Write column headers
    out << "Temperature (degC),";
    out << "Resistance Formula (Ohm),";

    // Only add reference columns for Platinum with IEC 60751
    bool hasReference = (material == "Platinum");
    if (hasReference) {
        out << "Resistance Reference IEC 60751 (Ohm),";
        out << "Absolute Difference (Ohm),";
        out << "Percentage Difference (%),";
    }
    out << "\n";

    // Configure calculator
    m_calculator->setAlphaCoefficient(alpha);
    m_calculator->setNominalResistance(r0);

    double r0Factor = r0 / 100.0;  // Scale factor for reference table

    // Generate data points (every 10°C)
    double step = 10.0;
    int rowCount = 0;

    for (double temp = minTemp; temp <= maxTemp; temp += step) {
        // Calculate formula resistance
        double resistanceFormula = m_calculator->temperatureToResistance(temp);

        out << QString::number(temp, 'f', 1) << ",";
        out << QString::number(resistanceFormula, 'f', 4) << ",";

        if (hasReference) {
            // Get reference resistance from IEC 60751 table
            double resistanceReference = RTDReferenceData::getTableResistance(temp, r0Factor);

            if (resistanceReference > 0) {
                double absDiff = resistanceFormula - resistanceReference;
                double percDiff = (absDiff / resistanceReference) * 100.0;

                out << QString::number(resistanceReference, 'f', 4) << ",";
                out << QString::number(absDiff, 'f', 4) << ",";
                out << QString::number(percDiff, 'f', 6) << ",";
            } else {
                out << "N/A,N/A,N/A,";
            }
        }

        out << "\n";
        rowCount++;
    }

    file.close();

    QMessageBox::information(this, tr("Success"),
                           tr("CSV data exported successfully!\n%1 rows written.").arg(rowCount));
}

void ChartDialog::onToleranceBandToggled(bool checked)
{
    // Remove existing tolerance band
    if (m_toleranceBand) {
        m_chart->removeSeries(m_toleranceBand);
        delete m_toleranceBand;
        m_toleranceBand = nullptr;
    }

    if (!checked || m_chart->series().isEmpty()) return;

    // Get the first series (primary curve)
    QLineSeries *mainSeries = qobject_cast<QLineSeries *>(m_chart->series().first());
    if (!mainSeries) return;

    // Create upper and lower bounds (example: ±0.15 + 0.002*|t|)
    QLineSeries *upperSeries = new QLineSeries();
    QLineSeries *lowerSeries = new QLineSeries();

    foreach (const QPointF &point, mainSeries->points()) {
        double t = point.x();
        double r = point.y();
        double tolerance = 0.15 + 0.002 * qAbs(t); // Class A tolerance example
        double tolResistance = tolerance * (r / (t + 273.15)); // Approximate

        upperSeries->append(t, r + tolResistance);
        lowerSeries->append(t, r - tolResistance);
    }

    m_toleranceBand = new QAreaSeries(upperSeries, lowerSeries);
    m_toleranceBand->setName(tr("Tolerance Band"));
    m_toleranceBand->setColor(QColor(200, 200, 200, 100));
    m_toleranceBand->setBorderColor(Qt::transparent);

    m_chart->addSeries(m_toleranceBand);
    m_toleranceBand->attachAxis(m_axisX);
    m_toleranceBand->attachAxis(m_axisY);
}

QColor ChartDialog::getNextColor()
{
    QColor color = m_colorPalette[m_nextColorIndex % m_colorPalette.size()];
    m_nextColorIndex++;
    return color;
}

void ChartDialog::addReferencePoints(const QString &label, double r0Factor, const QColor &color)
{
    double minTemp = m_minTempSpin->value();
    double maxTemp = m_maxTempSpin->value();

    // Get reference table points (IEC 60751 for Pt100)
    QVector<QPair<double, double>> points =
        RTDReferenceData::getTablePoints(minTemp, maxTemp, r0Factor, 50.0); // Points every 50°C

    if (points.isEmpty()) return;

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
    updateAxesRanges();
}

void ChartDialog::updateSeriesVisibility()
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

void ChartDialog::onShowFormulaToggled(bool checked)
{
    Q_UNUSED(checked);
    updateSeriesVisibility();
}

void ChartDialog::onShowReferenceToggled(bool checked)
{
    Q_UNUSED(checked);
    updateSeriesVisibility();
}

void ChartDialog::onResetViewClicked()
{
    // Reset to original axis ranges
    m_axisX->setRange(m_originalMinX, m_originalMaxX);
    m_axisY->setRange(m_originalMinY, m_originalMaxY);
}

void ChartDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QDialog::changeEvent(event);
}

void ChartDialog::retranslateUi()
{
    // Update window title
    setWindowTitle(tr("RTD Resistance vs Temperature Curves"));

    // Update group box
    m_controlGroup->setTitle(tr("Curve Configuration"));

    // Update form labels
    m_materialLabel->setText(tr("Material:"));
    m_typeLabel->setText(tr("Type:"));
    m_standardLabel->setText(tr("Standard:"));
    m_minTempLabel->setText(tr("Min Temp:"));
    m_maxTempLabel->setText(tr("Max Temp:"));

    // Update chart and axes
    m_chart->setTitle(tr("RTD Resistance vs Temperature"));
    m_axisX->setTitleText(tr("Temperature (°C)"));
    m_axisY->setTitleText(tr("Resistance (Ω)"));

    // Update buttons
    m_generateBtn->setText(tr("Generate Curve"));
    m_addComparisonBtn->setText(tr("Add Comparison"));
    m_clearAllBtn->setText(tr("Clear All"));
    m_exportBtn->setText(tr("Export PNG"));
    m_exportCSVBtn->setText(tr("Export CSV"));
    m_resetViewBtn->setText(tr("Reset View"));

    // Update checkboxes
    m_toleranceBandCheck->setText(tr("Show Tolerance Band"));
    m_showFormulaCheck->setText(tr("Show Formula Curve"));
    m_showReferenceCheck->setText(tr("Show Reference Points (IEC 60751)"));

    // Update combo box items
    int materialIndex = m_materialCombo->currentIndex();
    m_materialCombo->blockSignals(true);
    m_materialCombo->clear();
    m_materialCombo->addItem(tr("Platinum (Pt)"), "Platinum");
    m_materialCombo->addItem(tr("Copper (Cu)"), "Copper");
    m_materialCombo->addItem(tr("Nickel (Ni)"), "Nickel");
    m_materialCombo->setCurrentIndex(materialIndex);
    m_materialCombo->blockSignals(false);
}
