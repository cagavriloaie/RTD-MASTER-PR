#ifndef THERMOCOUPLECHARTDIALOG_H
#define THERMOCOUPLECHARTDIALOG_H

#include <QDialog>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include "thermocouplecalculator.h"
#include "thermocouplereferencedata.h"

class ThermocoupleChartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ThermocoupleChartDialog(QWidget *parent = nullptr);
    ~ThermocoupleChartDialog();

private slots:
    void onTypeChanged(int index);
    void onGenerateClicked();
    void onAddComparisonClicked();
    void onClearAllClicked();
    void onExportPNGClicked();
    void onExportCSVClicked();
    void onShowFormulaToggled(bool checked);
    void onShowReferenceToggled(bool checked);
    void onResetViewClicked();

protected:
    void changeEvent(QEvent *event) override;

private:
    // UI Components
    QGroupBox *m_controlGroup;
    QLabel *m_typeLabel;
    QLabel *m_coldJunctionLabel;
    QLabel *m_minTempLabel;
    QLabel *m_maxTempLabel;
    QComboBox *m_typeCombo;
    QDoubleSpinBox *m_coldJunctionSpin;
    QDoubleSpinBox *m_minTempSpin;
    QDoubleSpinBox *m_maxTempSpin;
    QCheckBox *m_showFormulaCheck;
    QCheckBox *m_showReferenceCheck;
    QPushButton *m_generateBtn;
    QPushButton *m_addComparisonBtn;
    QPushButton *m_clearAllBtn;
    QPushButton *m_exportBtn;
    QPushButton *m_exportCSVBtn;
    QPushButton *m_resetViewBtn;

    // Chart components
    QChart *m_chart;
    QChartView *m_chartView;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;

    // Store original axis ranges for reset
    double m_originalMinX;
    double m_originalMaxX;
    double m_originalMinY;
    double m_originalMaxY;

    // Calculator
    ThermocoupleCalculator *m_calculator;

    // Helper methods
    void setupUI();
    void setupChart();
    void populateTypeCombo();
    void addCurve(const QString &label, ThermocoupleType type, double coldJunctionTemp,
                  const QColor &color, bool isPrimary = false);
    void addReferencePoints(const QString &label, ThermocoupleType type,
                           double coldJunctionTemp, const QColor &color);
    void updateAxesRanges();
    void updateSeriesVisibility();
    void retranslateUi();
    QString getTypeDisplayName(ThermocoupleType type);

    // Temperature ranges for each type
    struct TempRange {
        double min;
        double max;
    };
    TempRange getTypeRange(ThermocoupleType type);

    // Color management for multiple curves
    QVector<QColor> m_colorPalette;
    int m_nextColorIndex;
    QColor getNextColor();

    // Tracking curves
    int m_curveCount;
    QVector<QLineSeries*> m_formulaSeries;
    QVector<QScatterSeries*> m_referenceSeries;
};

#endif // THERMOCOUPLECHARTDIALOG_H
