#ifndef CHARTDIALOG_H
#define CHARTDIALOG_H

#include <QDialog>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include <QAreaSeries>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include "rtdcalculator.h"
#include "rtdreferencedata.h"

class ChartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChartDialog(QWidget *parent = nullptr);
    ~ChartDialog();

private slots:
    void onMaterialChanged(int index);
    void onGenerateClicked();
    void onAddComparisonClicked();
    void onClearAllClicked();
    void onExportPNGClicked();
    void onExportCSVClicked();
    void onToleranceBandToggled(bool checked);
    void onShowFormulaToggled(bool checked);
    void onShowReferenceToggled(bool checked);
    void onResetViewClicked();

protected:
    void changeEvent(QEvent *event) override;

private:
    // UI Components
    QGroupBox *m_controlGroup;
    QLabel *m_materialLabel;
    QLabel *m_typeLabel;
    QLabel *m_standardLabel;
    QLabel *m_minTempLabel;
    QLabel *m_maxTempLabel;
    QComboBox *m_materialCombo;
    QComboBox *m_typeCombo;
    QComboBox *m_standardCombo;
    QDoubleSpinBox *m_minTempSpin;
    QDoubleSpinBox *m_maxTempSpin;
    QCheckBox *m_toleranceBandCheck;
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
    RTDCalculator *m_calculator;

    // Helper methods
    void setupUI();
    void setupChart();
    void populateMaterialCombo();
    void populateTypeCombo(const QString &material);
    void populateStandardCombo(const QString &material);
    void addCurve(const QString &label, double r0, AlphaCoefficient alpha,
                  const QColor &color, bool isPrimary = false);
    void addReferencePoints(const QString &label, double r0Factor, const QColor &color);
    void updateAxesRanges();
    void updateSeriesVisibility();
    void retranslateUi();
    AlphaCoefficient getAlphaFromStandard(const QString &material, const QString &standard);

    // Color management for multiple curves
    QVector<QColor> m_colorPalette;
    int m_nextColorIndex;
    QColor getNextColor();

    // Tracking curves
    int m_curveCount;
    QAreaSeries *m_toleranceBand;
    QVector<QLineSeries*> m_formulaSeries;
    QVector<QScatterSeries*> m_referenceSeries;
};

#endif // CHARTDIALOG_H
